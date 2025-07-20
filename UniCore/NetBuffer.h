#pragma once
/*==========================
*		BASE Buffer
===========================*/
enum Enum_NetBufferFlags
{
	BUFFER_SIZE_DEFAULT = 0x10000,
};

class NetBuffer : objpool::NoPlacemnetNew
{
protected:
	stdex::vector<BYTE> _buffer;

	using size_type = typename decltype(_buffer)::size_type;

	size_type _readPos = 0;
	size_type _writePos = 0;
public:
	NetBuffer();
	NetBuffer(size_type bufferSize);

	void Init(size_type bufferSize = Enum_NetBufferFlags::BUFFER_SIZE_DEFAULT);
	void Destroy();

public:
	/*Buffer Control*/
	//[][][][r][][][][w][][][][][][]
	//애초에 버퍼 크기를 크게 잡으면 복사비용이 생길 확률이 작아진다.
	//이유는 버퍼가 커서 여유 freesize가 오래 남아있을수록 rPos, wPos가 겹칠 확률이 증가한다.

	//=> 자세한 이유는 각각의 Clean String/Weak와 OnRead/OnWrite를 확인해 보자.
	void CleanPosPointers_Strong();
	void CleanPosPointers_Weak();

	virtual bool OnRead(size_type numOfBytes);
	virtual bool OnWrite(size_type numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	size_type DataSize() { return _writePos - _readPos; }
	size_type FreeSize() { return _buffer.size() - _writePos; }
	size_type Capacity() { return _buffer.size(); }
};


/*==========================
*		Recv Buffer
===========================*/
//곧바로 NetBuffer를 상속받고 NetBuffer와 동일한 방식으로 사용.
class RecvBuffer : public NetBuffer
{
public:
	RecvBuffer()
		: NetBuffer(BUFFER_SIZE_DEFAULT)
	{}
	RecvBuffer(int32 bufferSize)
		: NetBuffer(bufferSize)
	{}

	void Clear()
	{
		_readPos = 0;
		_writePos = 0;
	}
};

class SendData;

class RecvData
{
	friend SendData;
public:
	RecvData(const std::shared_ptr<NetBuffer>& owner, BYTE* head, const int32& dataSize)
		:_owner(owner), _head(head), _dsize(dataSize)
	{
	}

public:
	BYTE* Buffer() { return _head; }
	int32 AllocSize() { return _dsize; }

private:
	//for ref counting
	BYTE* _head;
	int32 _dsize;
	std::shared_ptr<NetBuffer> _owner;
};



/*==========================
*		Send Buffer
===========================*/
class SendBuffer
{
public:
	SendBuffer();
	SendBuffer(std::size_t size);

public:
	/// <summary>
	/// 다 쓴 후 반드시 쓴만큼 앞으로 밀어줘야함.
	/// </summary>
	/// <returns>
	/// <para>버퍼의 현재 가용 가능한 부분의 시작 포인터</para>
	/// <para>starting pointer of part of available in buffer</para>
	/// </returns>
	std::shared_ptr<NetBuffer> GetNetBuffer() { return _buffer; }
	BYTE* Get() { return _buffer->WritePos(); }
	bool Write(int32 writeSize) { return _buffer->OnWrite(writeSize); }

	const auto FreeSize() { return _buffer->FreeSize(); }
	const auto Capacity() { return _buffer->Capacity(); }

private:
	std::shared_ptr<NetBuffer> _buffer;
};

class SendData
{
public:
	SendData(const std::shared_ptr<NetBuffer>& owner, BYTE* head, const int32& dataSize);
	SendData(const std::shared_ptr<RecvData>& swap);

public:
	BYTE* Buffer() { return _head; }
	int32 AllocSize() { return _dsize; }

private:
	//for ref counting
	BYTE* _head;
	int32 _dsize;
	std::shared_ptr<NetBuffer> _owner;
};

class SendBufferSlicer
{
public:
	std::shared_ptr<SendData> Slice(int32 size);
};