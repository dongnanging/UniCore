#pragma once
#include "Session.h"

using namespace boost::asio::ip;

enum class Enum_ServiceType
{
	None,
	Server,
	Client,
};

class Service;

class ServiceBase : public std::enable_shared_from_this<Service>
{
protected:
	//쓰지마라
	ServiceBase() = default;

public:
	void AnchoringService(int32 serviceCount);

protected:
	void _anchor();

public:
	auto GetCore() { return _service; }

protected:
	std::shared_ptr<boost::asio::io_service> _service;
};

class Service :public ServiceBase
{
private:
	USE_LOCK;

protected:
	Service(Enum_ServiceType serviceType, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service = nullptr);
	
public:
	auto GetServiceType() { return _serviceType; }

	std::shared_ptr<Session> GetSession(uint64 sid);

	template<typename _SharedSendData>
	std::enable_if_t<std::is_same_v<stdex::pure_type_t<_SharedSendData>, std::shared_ptr<SendData>>, 
		void> BroadCast(_SharedSendData&& data)
	{
		READ_LOCK;
		for (auto& pair : _connected_sessions)
		{
			pair.second->Send(std::forward<_SharedSendData>(data));
		}
	}

	virtual void AttachSession(std::shared_ptr<Session> session);
	virtual void RemoveSession(std::shared_ptr<Session> session);
	virtual void RemoveSession(const int64& sid);
	virtual void Start() = 0;

	auto lock_view()
	{

	}

protected:
	std::function<std::shared_ptr<Session>()> _sfactory;

	std::atomic<uint64> _sessionCount = 1;

	std::unordered_map <uint64, std::shared_ptr<Session>> _connected_sessions;
	Enum_ServiceType _serviceType;
};

class ClientService : public Service
{
public:
	ClientService(std::function<std::shared_ptr<Session>()> sessionFactory = nullptr, int32 automake = 0, std::shared_ptr<boost::asio::io_service> service = nullptr)
		: Service(Enum_ServiceType::Client, sessionFactory, service), _maxSessionCount(automake)
	{}

	// Service을(를) 통해 상속됨
	virtual void Start() override;

	void SessionStart(std::shared_ptr<Session> session);
	void SessionStart();

private:
	int32 _maxSessionCount;
};

class ServerService : public Service
{
public:
	ServerService(const int32& port, const int32& acceptorcount, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service = nullptr)
		: Service((_openPort > 65535 || _openPort < 0) ? Enum_ServiceType::None : Enum_ServiceType::Server
		, sessionFactory, service)
		, _openPort(port)
		, _acceptorCount(acceptorcount)
	{}

	// Service을(를) 통해 상속됨
	virtual void Start() override;

public:
	const uint16& GetPort() { return _openPort; }

private:
	void _StartAccept(int32 acceptorCount = 1, int32 listenCount = 0x7fff'ffff);
	void _OnAccept(std::shared_ptr<Session> csession, std::shared_ptr<tcp::acceptor> acceptor, const boost::system::error_code& error);

private:
	uint16 _openPort;
	int32 _acceptorCount;
	std::vector<std::shared_ptr<tcp::acceptor>> _acceptors;
};


class LevelBase
{
public:
	LevelBase() 
		: _next_level(nullptr)
	{}

public:
	auto set_level(const std::size_t& level) { _level = level; return this; }
	auto set_next(const std::shared_ptr<Service>& next) { _next_level = next; return this; }

public:
	const auto& level() const noexcept { return _level; }

protected:
	std::size_t _level;
	std::shared_ptr<Service> _next_level;
};

class LCService : public ClientService, public LevelBase
{
public:
	LCService(std::function<std::shared_ptr<Session>()> sessionFactory = nullptr, int32 automake = 0, std::shared_ptr<boost::asio::io_service> service = nullptr)
		: ClientService(sessionFactory, automake, service)
	{}


public:
	void LevelUp(const std::shared_ptr<Session>& session)
	{
		if (_next_level == nullptr)
		{
			DYNAMIC_ASSERT(_next_level != nullptr, "not exist next level");
			return;
		}

		RemoveSession(session);
		session->AttachToService(_next_level);
		_next_level->AttachSession(session);
	}
};

class LSService : public ServerService, public LevelBase
{
public:
	LSService(const uint16& port, const int32& acceptorcount, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service = nullptr)
		: ServerService(port, acceptorcount, sessionFactory, service)
	{}

public:
	void LevelUp(const std::shared_ptr<Session>& session)
	{
		if (_next_level == nullptr)
		{
			DYNAMIC_ASSERT(_next_level != nullptr, "not exist next level");
			return;
		}

		RemoveSession(session);
		session->AttachToService(_next_level);
		_next_level->AttachSession(session);
	}
};

template<typename _ServiceType, std::size_t _LevelCount = 1>
class BottomUpService
{
public:
	template<typename = std::enable_if_t<std::is_same_v<_ServiceType, LCService>, void>>
	BottomUpService(std::function<std::shared_ptr<Session>()> sessionFactory = nullptr, int32 automake = 0, std::shared_ptr<boost::asio::io_service> service = nullptr)
		noexcept
	{
		static_assert(_LevelCount > 0, "not support for zero level");

		if (!service)
			service = J_MakeShared< boost::asio::io_service>();

		for (int32 i = 0; i < _LevelCount; i++)
		{
			_serivces[i] = J_MakeShared<_ServiceType<i>>(sessionFactory, automake, service);
			_serivces[i]->set_level(i);

			if (i < _LevelCount - 1)
				_serivces[i]->set_next(_serivces[i + 1]);
		}
	}

	template<typename = std::enable_if_t<std::is_same_v<_ServiceType, LSService>, void>>
	BottomUpService(const uint16& port, const int32& acceptorcount, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service = nullptr)
		noexcept
	{
		static_assert(_LevelCount > 0, "not support for zero level");

		if (!service)
			service = J_MakeShared< boost::asio::io_service>();

		for (int32 i = 0; i < _LevelCount; i++)
		{
			_serivces[i] = J_MakeShared<_ServiceType<i>>(port, acceptorcount, sessionFactory, service);
			_serivces[i]->set_level(i);

			if (i < _LevelCount - 1)
				_serivces[i]->set_next(_serivces[i + 1]);
		}
	}

	void AnchoringService(int32 serviceCount) override
	{
		_serivces[0]->AnchoringService(serviceCount);
	}

	void Start() override
	{
		_serivces[0]->Start();
	}

private:
	std::size_t _level_size = _LevelCount;
	std::shared_ptr<_ServiceType> _serivces[_LevelCount];
};