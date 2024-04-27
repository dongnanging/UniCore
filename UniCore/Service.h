#pragma once
#include "Session.h"

using namespace boost::asio::ip;

enum class Enum_ServiceType
{
	Server,
	Client,
};

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(Enum_ServiceType serviceType, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service = nullptr);

public:
	void AnchoringService(int32 serviceCount);
	void BroadCast(const std::shared_ptr<SendData>& data);
	void BroadCast_Except(const std::shared_ptr<SendData>& data, const uint64& except_id);
	void BroadCast(const std::vector<std::shared_ptr<SendData>>& datas);
	void BroadCast_Except(const std::vector<std::shared_ptr<SendData>>& datas, const uint64& except_id);

	auto GetCore() { return _service; }
	auto GetServiceType() { return _serviceType; }
	std::shared_ptr<Session> GetSession(uint64 sid);
	void GetAllSessions(OUT std::vector<std::shared_ptr<Session>>& result);

	virtual void AddSession(std::shared_ptr<Session> session);
	virtual void RemoveSession(std::shared_ptr<Session> session);
	virtual void RemoveSession(const int64& sid);
	virtual void Start() = 0;

protected:
	void _anchor();


protected:
	std::shared_ptr<boost::asio::io_service> _service;
	std::function<std::shared_ptr<Session>()> _sfactory;

	std::atomic<uint64> _sessionCount = 1;

	
	std::unordered_map <uint64, std::shared_ptr<Session>> _connected_sessions;

	Enum_ServiceType _serviceType;

private:
	USE_LOCK;
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

public:


private:
	int32 _maxSessionCount;
};

class ServerService : public Service
{
public:
	ServerService(const uint16& port, const int32& acceptorcount, std::function<std::shared_ptr<Session>()> sessionFactory, std::shared_ptr<boost::asio::io_service> service = nullptr)
		: Service(Enum_ServiceType::Server, sessionFactory, service), _openPort(port), _acceptorCount(acceptorcount)
	{}

	// Service을(를) 통해 상속됨
	virtual void Start() override;

public:
	const uint16& GetPort() { return _openPort; }

	int64 unsafe_session_count() { return _connected_sessions.size(); }


private:
	void _StartAccept(int32 acceptorCount = 1, int32 listenCount = 0x7fff'ffff);
	void _OnAccept(std::shared_ptr<Session> csession, std::shared_ptr<tcp::acceptor> acceptor, const boost::system::error_code& error);

private:
	uint16 _openPort;
	int32 _acceptorCount;
	std::vector<std::shared_ptr<tcp::acceptor>> _acceptors;
};