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
	//���ʿ� ���� ũ�⸦ ũ�� ������ �������� ���� Ȯ���� �۾�����.
	//������ ���۰� Ŀ�� ���� freesize�� ���� ������������ rPos, wPos�� ��ĥ Ȯ���� �����Ѵ�.

	//=> �ڼ��� ������ ������ Clean String/Weak�� OnRead/OnWrite�� Ȯ���� ����.
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
//��ٷ� NetBuffer�� ��ӹް� NetBuffer�� ������ ������� ���.
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
	/// �� �� �� �ݵ�� ����ŭ ������ �о������.
	/// </summary>
	/// <returns>
	/// <para>������ ���� ���� ������ �κ��� ���� ������</para>
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