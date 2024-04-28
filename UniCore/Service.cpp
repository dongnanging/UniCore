#include "stdafx.h"
#include "Service.h"
#include <random>

#include "boost/bind.hpp"

Service::Service(Enum_ServiceType serviceType, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service)
	: _serviceType(serviceType)
{
	if (service)
		_service = service;
	else
		_service = J_MakeShared<boost::asio::io_service>();

	if (sessionFactory != nullptr)
	{
		_sfactory = [this, sessionFactory]()
		{
			//미리 부착한 상태로 전달!!!
			auto session = sessionFactory();
			session->AttachToService(shared_from_this());
			return session;
		};
	}
	else
		_sfactory = nullptr;

	_connected_sessions.reserve(0);
}

std::shared_ptr<Session> Service::GetSession(uint64 sid)
{
	READ_LOCK;
	auto itr = _connected_sessions.find(sid);
	if (itr == _connected_sessions.end())
		return nullptr;

	return itr->second;
}

void Service::GetAllSessions(OUT std::vector<std::shared_ptr<Session>>& result)
{
	READ_LOCK;
	for (auto& itr : _connected_sessions)
		result.push_back(itr.second);
}

void Service::AddSession(std::shared_ptr<Session> session)
{
	uint64 sessionID = _sessionCount.fetch_add(1);
	session->set_sid(sessionID);

	WRITE_LOCK;
	_connected_sessions.insert({ sessionID, session });
}

void Service::RemoveSession(std::shared_ptr<Session> session)
{
	WRITE_LOCK;
	auto itr = _connected_sessions.find(session->sid());

	//error?
	if (itr == _connected_sessions.end())
		return;

	//release ref
	itr->second->Disconnect();
	_connected_sessions.erase(itr);
}

void Service::RemoveSession(const int64& sid)
{
	WRITE_LOCK;
	auto itr = _connected_sessions.find(sid);

	//error?
	if (itr == _connected_sessions.end())
		return;

	//release ref
	itr->second->Disconnect();
	_connected_sessions.erase(itr);
}

void Service::_anchor()
{
	GetCore()->run();
	GlobalHandler.threadManager->EnqueueJob(
		[this]() { _anchor(); },
		shared_from_this()
	);
}

void Service::AnchoringService(int32 serviceCount)
{
	for (int32 i = 0; i < serviceCount; i++)
	{
		GlobalHandler.threadManager->EnqueueJob(
			[this]() { _anchor();  },
			shared_from_this()
		);
	}
}

void Service::BroadCast(const std::shared_ptr<SendData>& data)
{
	//보내는 동안은 session 리스트가 변조되지 않도록 lock
	READ_LOCK;
	for (auto& item : _connected_sessions)
		item.second->Send(data);
}

void Service::BroadCast_Except(const std::shared_ptr<SendData>& data, const uint64& except_id)
{
	//보내는 동안은 session 리스트가 변조되지 않도록 lock
	READ_LOCK;
	for (auto& item : _connected_sessions)
	{
		if (except_id == item.second->sid())
			continue;
		item.second->Send(data);
	}
}

void Service::BroadCast(const std::vector<std::shared_ptr<SendData>>& datas)
{
	//보내는 동안은 session 리스트가 변조되지 않도록 lock
	READ_LOCK;
	for (auto& item : _connected_sessions)
		item.second->Send(datas);
}

void Service::BroadCast_Except(const std::vector<std::shared_ptr<SendData>>& datas, const uint64& except_id)
{
	//보내는 동안은 session 리스트가 변조되지 않도록 lock
	READ_LOCK;
	for (auto& item : _connected_sessions)
	{
		if (except_id == item.second->sid())
			continue;
		item.second->Send(datas);
	}
}


void ServerService::_StartAccept(int32 acceptorCount, int32 listenCount)
{
	for (int32 i = 0; i < acceptorCount; i++)
	{
		std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor = nullptr;

		if (_openPort == 0)
		{
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_int_distribution<int> dist(30000, 65535);

			while (_openPort < PORT_BOUNDARY)
			{
				auto randNum = dist(mt);
				if (randNum < PORT_BOUNDARY) continue;

				try
				{
					acceptor = J_MakeShared<tcp::acceptor>(
						*_service.get(),
						boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), randNum));

					if (_openPort == 0)
						_openPort = acceptor->local_endpoint().port();
				}
				catch(...)
				{
					continue;
				}
			}
		}
		else
		{
			acceptor = J_MakeShared<tcp::acceptor>(
				*_service.get(),
				boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), _openPort));
		}

		acceptor->set_option(tcp::acceptor::reuse_address(true));
		acceptor->listen(listenCount);
		_acceptors.push_back(acceptor);

		auto client = _sfactory();

		acceptor->async_accept(
			*client->GetSocket(),
			boost::bind(&ServerService::_OnAccept, this, client, acceptor, boost::asio::placeholders::error));

		
	}

	std::cout << "Start Listening with::" << _openPort << std::endl;
}



void ServerService::_OnAccept(std::shared_ptr<Session> csession, std::shared_ptr<tcp::acceptor> acceptor, const boost::system::error_code& error)
{
	auto client = _sfactory();
	if (error)
	{
		//재등록
		acceptor->async_accept(
			*client->GetSocket(),
			boost::bind(&ServerService::_OnAccept, this, client, acceptor, boost::asio::placeholders::error)
		);
		return;
	}
	
	csession->Start();

	//재등록
	acceptor->async_accept(
		*client->GetSocket(),
		boost::bind(&ServerService::_OnAccept, this, client, acceptor, boost::asio::placeholders::error)
	);
}

void ClientService::Start()
{
	DEBUG_CODE(std::cout << "Start Client Service..." << std::endl);
	for (int i = 0; i < _maxSessionCount; i++) 
	{
		//알아서 서버와 연결됨
		auto session = _sfactory();
		SessionStart(session);

		if (i % 5 == 0)
			THREAD_SLEEP;
	}
}

void ClientService::SessionStart(std::shared_ptr<Session> session)
{
	session->AttachToService(shared_from_this());
	session->Start();
}

void ClientService::SessionStart()
{
	if (_sfactory == nullptr)
		return;

	auto session = _sfactory();
	session->Start();
}

void ServerService::Start()
{
	//연결할 클라이언트를 받을 수 있는 상태로 셋팅
	_StartAccept(_acceptorCount);
}