#include "stdafx.h"
#include "NetBuffer.h"

/*==========================
*		BASE Buffer
===========================*/
NetBuffer::NetBuffer()
{
	_buffer.resize(Enum_NetBufferFlags::BUFFER_SIZE_DEFAULT);
}

NetBuffer::NetBuffer(int32 bufferSize)
	: _buffer(bufferSize)
{
}

void NetBuffer::CleanPosPointers_Strong()
{
	int32 _nowDataSize = DataSize();
	if (_nowDataSize == 0)	//_readPos == _writePos
	{
		//�쿬�� �Ѵ� ��ġ�Ƿ� �ٷ� �ʱ���ġ�� �ʱ�ȭ
		//good case
		_readPos = _writePos = 0;
		return;
	}

	//�Ȱ�ġ���� ������ �����ؾ��Ѵ�..
	//bad case
	::memcpy(&_buffer[0], &_buffer[_readPos], _nowDataSize);
	_readPos = 0;
	_writePos = _nowDataSize;
}

void NetBuffer::CleanPosPointers_Weak()
{
	if (DataSize() == 0)	//_readPos == _writePos
	{
		//�쿬�� �Ѵ� ��ġ�Ƿ� �ٷ� �ʱ���ġ�� �ʱ�ȭ
		//good case
		_readPos = _writePos = 0;
		return;
	}
}

bool NetBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	CleanPosPointers_Weak();
	return true;
}

bool NetBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > _buffer.size())
	{
		DYNAMIC_ASSERT(numOfBytes <= _buffer.size(),
			"new data size is over the maximum size of buffer");
		return false;
	}
	
	//���� ������ ���ٸ�? => ������ ���� ������ ����� ����
	if (numOfBytes > FreeSize())
		CleanPosPointers_Strong();

	if(numOfBytes > FreeSize())
	{
		DYNAMIC_ASSERT(numOfBytes <= FreeSize(),
			"buffer is busy. cannot insert additional data into the buffer");
		return false;
	}

	_writePos += numOfBytes;
	return true;
}



/*==========================
*		Send Buffer
===========================*/
SendBuffer::SendBuffer()
{
	_buffer = J_MakeShared<NetBuffer>();
}


SendData::SendData(const std::shared_ptr<NetBuffer>& owner, BYTE* head, const int32& dataSize)
	:_owner(owner), _head(head), _dsize(dataSize)
{
}

SendData::SendData(const std::shared_ptr<RecvData>& swap)
	:_owner(std::move(swap->_owner)), _head(std::move(swap->_head)), _dsize(std::move(swap->_dsize))
{
}

//per thread. == thread safe
std::shared_ptr<SendData> SendBufferSlicer::Slice(int32 size)
{
	//Release Ref and get new one => ���߿� pool�� ������ �ؼ� ����
	if (TLS_SendBuffer == nullptr || TLS_SendBuffer->FreeSize() < size)
		TLS_SendBuffer = J_MakeShared<SendBuffer>();

	//if (TLS_SendBuffer->Capacity() < size)
	//{
	//	DYNAMIC_ASSERT(TLS_SendBuffer->FreeSize() >= size, "wrong size alloc");
	//	return nullptr;
	//}

	auto sendData = J_MakeShared<SendData>(TLS_SendBuffer->GetNetBuffer(), TLS_SendBuffer->Get(), size);
	if (TLS_SendBuffer->Write(size) == false)
		return nullptr;

	return sendData;
}