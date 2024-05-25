#include "stdafx.h"
#include "Session.h"

#include "Service.h"
#include "boost/bind.hpp"

Session::Session()
	: _queued_send_count(0), _sid(0), _ip("unknown"), _port(0)
{
	_serializeQueue = J_MakeShared<JobQueue>();
#if defined(__SQL_ODBC)
	_dbAccessor = J_MakeShared<DBQueue>();
#endif
	_recvBuffer = ObjectPool<RecvBuffer>::PoolShared();
	_recvBuffer->Clear();
}

void Session::AttachToService(std::shared_ptr<Service> from)
{
	AttachService(from);
	
	_socket = J_MakeShared<tcp::socket>(*from->GetCore().get());
}

void Session::AttachService(std::shared_ptr<Service> serivce)
{
	_ownedService = serivce;
}

void Session::Send(std::shared_ptr<SendData>& sdata)
{
	_sendQueue.enqueue(sdata);

	auto prev = _queued_send_count.fetch_add(1);
	//prev�� 0�̴�? => send�Ϸ� �� sub����� 0�̾��� => ����� �����Ǿ��� => �����ؾ��Ѵ�.
	if (prev == 0)
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
}

void Session::Send(std::shared_ptr<SendData>&& sdata)
{
	_sendQueue.enqueue(std::move(sdata));

	auto prev = _queued_send_count.fetch_add(1);
	//prev�� 0�̴�? => send�Ϸ� �� sub����� 0�̾��� => ����� �����Ǿ��� => �����ؾ��Ѵ�.
	if (prev == 0)
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
}

void Session::Send(std::vector<std::shared_ptr<SendData>>& sdatas)
{
	auto size = sdatas.size();
	_sendQueue.enqueue_all(sdatas);

	auto prev = _queued_send_count.fetch_add(size);
	//prev�� 0�̴�? => send�Ϸ� �� sub����� 0�̾��� => ����� �����Ǿ��� => �����ؾ��Ѵ�.
	if (prev == 0)
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
}

void Session::Connected()
{
	_AttachSelfSession();

	OnConnected();

	//���� ������ ���·� ����
	_RegisterRecv();
}

void Session::Disconnect(std::string cause)
{
	_JobSerialize([this, cause]() {
		try 
		{
			//change to global stream?
			DEBUG_CODE(std::cout << "Session: " << _sid << " is closed.\ncause::" << cause << std::endl);

			//���⼭ ref�� �����ص� �� lambda�� ������ ������ capture�� �����ȴ�.
			//�� ��Ͽ� ���� ���� �������� �̹� Ȯ���� �����̴�.
			_DetachSelfSession();

			OnDisconnected();

			boost::system::error_code error;
			_socket->shutdown(boost::asio::socket_base::shutdown_both, error);
			_socket->close();
		}
		catch (...)
		{
			DYNAMIC_ASSERT(false, "Disconnect Failed!");
		}
		});
}

#if defined(__SQL_ODBC)
void Session::SessionSideDBJob(const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback)
{
	_dbAccessor->Push(sender, callback);
}

void Session::SessionSideDBJob(const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback)
{
	_dbAccessor->Push(senders, callback);
}
#endif

void Session::_RegisterSend()
{
	std::vector<std::shared_ptr<SendData>> datas;
	try
	{
		_sendQueue.dequeue_all(OUT datas);
		//auto data = _sendQueue.dequeue();
	
		//scatter gather
		std::vector<boost::asio::const_buffer> mdatas;
		for (int32 i = 0; i < datas.size(); i++)
			mdatas.push_back(boost::asio::buffer(datas[i]->Buffer(), datas[i]->AllocSize()));
		

		boost::asio::async_write(
		/*����       */	*_socket,
		/*���� ������*/	mdatas,
		/*callback   */	boost::bind(
							&Session::_HandleSend, 
							shared_from_this(), 
							std::move(datas), boost::asio::placeholders::error, 
							boost::asio::placeholders::bytes_transferred));
	}
	catch (...)
	{
		DYNAMIC_ASSERT(false, "register send failed. %ll, %s, %d", _sid, _ip, _port);

		if (datas.size() > 0)
		{
			_queued_send_count.fetch_sub(datas.size());
			datas.clear();
		}
	}
}

void Session::_RegisterRecv()
{
	try
	{
		_socket->async_read_some(
		/*���� ����*/	boost::asio::buffer(_recvBuffer->WritePos(), _recvBuffer->FreeSize()),
		/*callback */	boost::bind(
							&Session::_HandleRecv, 
							shared_from_this(),
							boost::asio::placeholders::error, 
							boost::asio::placeholders::bytes_transferred));
	}
	catch (...)
	{
		DYNAMIC_ASSERT(false, "register recv failed. %ll, %s, %d", _sid, _ip, _port)
	}
}

void Session::_HandleSend(std::vector<std::shared_ptr<SendData>> raw_datas, const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		_HandleError(error);
	}
	else
	{
		int32 processed_send_size = raw_datas.size();
		//send Success => release all refs;
		raw_datas.clear();

		OnSend(bytes_transferred);

		auto prev = _queued_send_count.fetch_sub(processed_send_size);

		//���� �޼����� ���� �ʾ���.
		if (prev - processed_send_size == 0)
			return;

		//���� ������ ���� �����Ͱ� �� ������, �ٽ� ����
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
	}
}

void Session::_HandleRecv(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		_HandleError(error);
		return;
	}

	// ���� ���� ��û
	if (bytes_transferred == 0)
	{
		Disconnect();
		return;
	}


	//����
	DYNAMIC_ASSERT(_recvBuffer->OnWrite(static_cast<int32>(bytes_transferred)), "buffer corrupted");
	//ó���ϰ�
	int32 plen = _PacketCheck();
	//�о��ֱ�
	DYNAMIC_ASSERT(_recvBuffer->OnRead(plen), "buffer corrupted");

	auto _recycle = ObjectPool<RecvBuffer>::PoolShared();
	//�ʱ�ȭ
	_recycle->Clear();

	//��Ⱑ ���Ҵٸ�
	if (_recvBuffer->DataSize() > 0)
	{
		//��� ����
		::memcpy(_recycle->WritePos(), _recvBuffer->ReadPos(), _recvBuffer->DataSize());
		//������ ����
		DYNAMIC_ASSERT(_recycle->OnWrite(static_cast<int32>(_recvBuffer->DataSize())), "buffer corrupted");
	}

	//������ �ٲ�ġ��
	_recvBuffer = _recycle;
	//�� ó�������� �ٽ� �����ϱ�.

	_RegisterRecv();
}

void Session::_HandleError(const boost::system::error_code& error)
{
	Disconnect("disconnected with : " + error.message());
	//std::cout << "error : " + error.message() << std::endl;
}



int32 Session::_PacketCheck()
{
	int32 plen = 0;
	std::vector<std::shared_ptr<RecvData>> recv_datas;
	while (true)
	{
		//����� ũ�⺸�� ���� ������ => ���� ó�� �Ұ�
		if (_recvBuffer->DataSize() - plen < sizeof(PacketHeader))
			break;

		auto header = reinterpret_cast<PacketHeader*>(_recvBuffer->ReadPos() + plen);

		DYNAMIC_ASSERT(header != nullptr, "reinterpret cast failed (BYTE* => PacketHeader*)");

		if (header == nullptr)
			break;

		//DYNAMIC_ASSERT(header->size != 0, "reinterpret cast failed (BYTE* => PacketHeader*)");
		if (_recvBuffer->DataSize() - plen < header->size)
			break;

		//���� ������ ����
		//���� ���� ������ Ȯ���� ���� ó�� ť�� ���޸� �ϰ� ��� Ȯ��
		recv_datas.push_back(J_MakeShared<RecvData>(_recvBuffer, _recvBuffer->ReadPos() + plen, header->size));
		// = _serializeQueue->Push(J_MakeShared<ThreadJob>(
		// [this, copy]() {OnRecv(copy); },
		// shared_from_this()));

		plen += header->size;
	}

	if (recv_datas.size() > 0)
		_JobSerialize([this, datas = std::move(recv_datas)]() mutable {
		for (auto& data : datas)
			OnRecv(std::move(data));
			});

	return plen;
}

void Session::_JobSerialize(std::function<void()>&& lambda)
{
	_serializeQueue->Push(J_MakeShared<ThreadJob>(std::move(lambda), shared_from_this()));
}

void Session::_AttachSelfSession()
{
	auto service = GetService();
	if (service)
		service->AttachSession(shared_from_this());
}

void Session::_DetachSelfSession()
{
	auto service = GetService();
	if (service)
		service->RemoveSession(shared_from_this());
}


void ActiveSession::Start()
{
	//���⼭ ActiveSession�� ������� �ʾƵ�, ���⼭ bind �ӿ��� ref�� ���̵ȴ�.
	_socket->async_connect(
	_ep,
	boost::bind(&ActiveSession::_HandleConnect, std::static_pointer_cast<ActiveSession>(shared_from_this()), boost::asio::placeholders::error));
}

void ActiveSession::_HandleConnect(const boost::system::error_code& error)
{
	if (error)
	{
		//GlobalHandler.threadManager->EnqueueJob(J_MakeShared<ThreadJob>([this]() {
		Start();
			//}, shared_from_this()), _reconnection_interval);

		return;
	}

	Connected();
}

void PassiveSession::Start()
{
	_ip = _socket->remote_endpoint().address().to_string();
	_port = _socket->remote_endpoint().port();

	Connected();
}
