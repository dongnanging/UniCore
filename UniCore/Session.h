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

public:
	Session();
	virtual ~Session();

public:
	void AttachToService(std::shared_ptr<Service> from);

public:
	virtual void Start() = 0;
	void Send(const std::shared_ptr<SendData>& sdata);
	void Send(const std::vector<std::shared_ptr<SendData>>& sdatas);
	void Disconnect(std::string cause = "Disconnect Called");

	//tcp -> udp로 변경될 가능성이 있으므로 socket은 연역하는것으로..
	auto GetSocket() { return _socket; }
	auto GetService() { return _ownedService.lock(); }

	bool IsConnected() { return _connected.load(); }

#if defined(__SQL_ODBC)
	void SessionSideDBJob(const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback = nullptr);
	void SessionSideDBJob(const std::vector<std::shared_ptr<SQL_Query_Sender>>& sender, const std::shared_ptr<ThreadJob>& callback = nullptr);
#endif

protected:
	void _RegisterSend();
	void _RegisterRecv();

	void _HandleSend(std::vector<std::shared_ptr<SendData>> raw_datas, const boost::system::error_code& error, size_t bytes_transferred);
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
	const uint64& sid() const { return _sid; }
	void set_sid(const uint64& sid) { _sid = sid; }
	virtual const int64& unique_id() const noexcept { return _sid; }
	virtual const int64& enter_id() const noexcept { return _sid; }

	virtual const std::string& ip() { return _ip; }
	virtual const uint16& port() { return _port; }
protected:
	uint64 _sid;
	std::string _ip = "unknown";
	uint16 _port = 0;

protected:
	SOCKET _socket;
	std::atomic<bool> _connected = false;

	std::weak_ptr<Service> _ownedService;

protected:
	//send
	LockContainer < std::shared_ptr<SendData>, std::queue<std::shared_ptr<SendData>>> _sendQueue;
	std::atomic<int32> _queued_send_count = 0;

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

	auto set_endpoint(const char ip[], const uint16& port) { _port = port, _ep = tcp::endpoint(boost::asio::ip::address::from_string(ip), port); return this; }
	void set_reconnection_interval(const int64& interval) { _reconnection_interval = interval; }

public:
	virtual const std::string& ip() override { return _ep.address().to_string(); }

protected:
	void _HandleConnect(const boost::system::error_code& error);

protected:
	tcp::endpoint _ep;
	int64 _reconnection_interval = 1000;
};

