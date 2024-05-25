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
	//prev가 0이다? => send완료 후 sub결과가 0이었다 => 등록이 해제되었다 => 재등록해야한다.
	if (prev == 0)
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
}

void Session::Send(std::shared_ptr<SendData>&& sdata)
{
	_sendQueue.enqueue(std::move(sdata));

	auto prev = _queued_send_count.fetch_add(1);
	//prev가 0이다? => send완료 후 sub결과가 0이었다 => 등록이 해제되었다 => 재등록해야한다.
	if (prev == 0)
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
}

void Session::Send(std::vector<std::shared_ptr<SendData>>& sdatas)
{
	auto size = sdatas.size();
	_sendQueue.enqueue_all(sdatas);

	auto prev = _queued_send_count.fetch_add(size);
	//prev가 0이다? => send완료 후 sub결과가 0이었다 => 등록이 해제되었다 => 재등록해야한다.
	if (prev == 0)
		GlobalHandler.threadManager->EnqueueJob([this]() { _RegisterSend(); }, shared_from_this());
}

void Session::Connected()
{
	_AttachSelfSession();

	OnConnected();

	//수신 가능한 상태로 변경
	_RegisterRecv();
}

void Session::Disconnect(std::string cause)
{
	_JobSerialize([this, cause]() {
		try 
		{
			//change to global stream?
			DEBUG_CODE(std::cout << "Session: " << _sid << " is closed.\ncause::" << cause << std::endl);

			//여기서 ref를 해제해도 이 lambda가 끝나기 전까진 capture가 유지된다.
			//이 블록에 들어온 순간 생존권이 이미 확보된 상태이다.
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
		/*소켓       */	*_socket,
		/*전송 데이터*/	mdatas,
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
		/*수신 버퍼*/	boost::asio::buffer(_recvBuffer->WritePos(), _recvBuffer->FreeSize()),
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

		//보낼 메세지가 남지 않았음.
		if (prev - processed_send_size == 0)
			return;

		//아직 보내지 못한 데이터가 더 있으니, 다시 재등록
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

	// 연결 해제 요청
	if (bytes_transferred == 0)
	{
		Disconnect();
		return;
	}


	//쓰고
	DYNAMIC_ASSERT(_recvBuffer->OnWrite(static_cast<int32>(bytes_transferred)), "buffer corrupted");
	//처리하고
	int32 plen = _PacketCheck();
	//밀어주기
	DYNAMIC_ASSERT(_recvBuffer->OnRead(plen), "buffer corrupted");

	auto _recycle = ObjectPool<RecvBuffer>::PoolShared();
	//초기화
	_recycle->Clear();

	//찌꺼기가 남았다면
	if (_recvBuffer->DataSize() > 0)
	{
		//찌꺼기 복사
		::memcpy(_recycle->WritePos(), _recvBuffer->ReadPos(), _recvBuffer->DataSize());
		//데이터 쓰기
		DYNAMIC_ASSERT(_recycle->OnWrite(static_cast<int32>(_recvBuffer->DataSize())), "buffer corrupted");
	}

	//기존과 바꿔치기
	_recvBuffer = _recycle;
	//다 처리했으니 다시 수신하기.

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
		//헤더의 크기보다 작은 데이터 => 아직 처리 불가
		if (_recvBuffer->DataSize() - plen < sizeof(PacketHeader))
			break;

		auto header = reinterpret_cast<PacketHeader*>(_recvBuffer->ReadPos() + plen);

		DYNAMIC_ASSERT(header != nullptr, "reinterpret cast failed (BYTE* => PacketHeader*)");

		if (header == nullptr)
			break;

		//DYNAMIC_ASSERT(header->size != 0, "reinterpret cast failed (BYTE* => PacketHeader*)");
		if (_recvBuffer->DataSize() - plen < header->size)
			break;

		//수신 데이터 도착
		//다음 수신 데이터 확인을 위해 처리 큐에 전달만 하고 계속 확인
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
	//여기서 ActiveSession은 등록하지 않아도, 여기서 bind 속에서 ref가 전이된다.
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
