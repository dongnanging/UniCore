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
public:
	NetBuffer();
	NetBuffer(int32 bufferSize);

public:
	/*Buffer Control*/
	//[][][][r][][][][w][][][][][][]
	//���ʿ� ���� ũ�⸦ ũ�� ������ �������� ���� Ȯ���� �۾�����.
	//������ ���۰� Ŀ�� ���� freesize�� ���� ������������ rPos, wPos�� ��ĥ Ȯ���� �����Ѵ�.

	//=> �ڼ��� ������ ������ Clean String/Weak�� OnRead/OnWrite�� Ȯ���� ����.
	void CleanPosPointers_Strong();
	void CleanPosPointers_Weak();

	virtual bool OnRead(int32 numOfBytes);
	virtual bool OnWrite(int32 numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	int32 FreeSize() { return _buffer.size() - _writePos; }
	int32 Capacity() { return _buffer.size(); }

protected:
	int32 _readPos = 0;
	int32 _writePos = 0;
	ads::vector<BYTE> _buffer;
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

	int32 FreeSize() { return _buffer->FreeSize(); }
	int32 Capacity() { return _buffer->Capacity(); }

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