#pragma once
#include "DBQueue.h"
#include "PacketHeader.h"

using namespace boost::asio::ip;

class Session;
class Service;

class Session : public std::enable_shared_from_this<Session>
{
	friend class Service;

	using SOCKET = std::shared_ptr<tcp::socket>;

protected:
	Session();

public:
	void AttachToService(std::shared_ptr<Service> from);
	void AttachService(std::shared_ptr<Service> serivce);

public:
	virtual void Start() = 0;
	void Send(std::shared_ptr<SendData>& sdata);
	void Send(std::shared_ptr<SendData>&& sdata);
	void Send(std::vector<std::shared_ptr<SendData>>& sdatas);
	void Connected();
	void Disconnect(std::string cause = "Disconnect Called");

	//tcp -> udp로 변경될 가능성이 있으므로 socket은 연역하는것으로..
	auto GetSocket() { return _socket; }
	auto GetService() { return _ownedService.lock(); }

#if defined(__SQL_ODBC)
	void SessionSideDBJob(const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback = nullptr);
	void SessionSideDBJob(const std::vector<std::shared_ptr<SQL_Query_Sender>>& sender, const std::shared_ptr<ThreadJob>& callback = nullptr);
#endif

protected:
	// shared from this로 생존권 얻고 예약
	void _RegisterSend();
	// shared from this로 생존권 얻고 예약
	void _RegisterRecv();

	// 해당 callback이 끝나기 전까지 자기 자신의 shared_ptr의 shared_count > 0 임이 유지됨
	void _HandleSend(std::vector<std::shared_ptr<SendData>> raw_datas, const boost::system::error_code& error, size_t bytes_transferred);
	// 해당 callback이 끝나기 전까지 자기 자신의 shared_ptr의 shared_count > 0 임이 유지됨
	void _HandleRecv(const boost::system::error_code& error, size_t bytes_transferred);

	void _HandleError(const boost::system::error_code& error);

	int32 _PacketCheck();
	void _JobSerialize(std::function<void()>&& lambda);

protected:
	virtual void _AttachSelfSession();
	virtual void _DetachSelfSession();

	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnSend(size_t bytes_transferred) = 0;
	virtual void OnRecv(const std::shared_ptr<RecvData>& recvData) = 0;

public:
	const uint64& sid() const noexcept { return _sid; }
	void set_sid(const uint64& sid) { _sid = sid; }

	virtual const std::string& ip() const noexcept { return _ip; }
	virtual const uint16& port() const noexcept { return _port; }
protected:
	uint64 _sid;
	std::string _ip;
	uint16 _port;

protected:
	SOCKET _socket;
	std::weak_ptr<Service> _ownedService;

protected:
	//send
	LockContainer < std::shared_ptr<SendData>, std::queue<std::shared_ptr<SendData>>> _sendQueue;
	std::atomic<int32> _queued_send_count;

	//recv
	std::shared_ptr<RecvBuffer> _recvBuffer;
	std::shared_ptr<JobQueue> _serializeQueue;
#if defined(__SQL_ODBC)
	std::shared_ptr<DBQueue> _dbAccessor;
#endif
};

//외부의 연결을 받아오는 세션
class PassiveSession : public Session
{
public:
	PassiveSession() {}

public:
	virtual void Start() override;
};


//외부에 연결을 요청하는 세션
class ActiveSession : public Session
{
public:
	virtual void Start() override;

	auto set_endpoint(const char ip[], const uint16& port)
	{
		_port = port;
		_ep = tcp::endpoint(boost::asio::ip::address::from_string(ip), port); 
		_ip = _ep.address().to_string(); 
		return this; 
	}
	void set_reconnection_interval(const int64& interval) { _reconnection_interval = interval; }

protected:
	void _HandleConnect(const boost::system::error_code& error);

protected:
	tcp::endpoint _ep;
	int64 _reconnection_interval = 1000;
};

