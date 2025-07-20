#include "stdafx.h"
#include "NetBuffer.h"

/*==========================
*		BASE Buffer
===========================*/
NetBuffer::NetBuffer()
{
	_buffer.resize(Enum_NetBufferFlags::BUFFER_SIZE_DEFAULT);
}

NetBuffer::NetBuffer(size_type bufferSize)
	: _buffer(bufferSize)
{
}

void NetBuffer::Init(size_type bufferSize)
{
	_buffer.resize(bufferSize);
}

void NetBuffer::Destroy()
{
	_readPos = 0;
	_writePos = 0;
}

void NetBuffer::CleanPosPointers_Strong()
{
	auto _nowDataSize = DataSize();
	if (_nowDataSize == 0)	//_readPos == _writePos
	{
		//우연히 둘다 겹치므로 바로 초기위치로 초기화
		//good case
		_readPos = _writePos = 0;
		return;
	}

	//안겹치지만 무조건 정리해야한다..
	//bad case
	::memcpy(&_buffer[0], &_buffer[_readPos], _nowDataSize);
	_readPos = 0;
	_writePos = _nowDataSize;
}

void NetBuffer::CleanPosPointers_Weak()
{
	if (DataSize() == 0)	//_readPos == _writePos
	{
		//우연히 둘다 겹치므로 바로 초기위치로 초기화
		//good case
		_readPos = _writePos = 0;
		return;
	}
}

bool NetBuffer::OnRead(size_type numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	CleanPosPointers_Weak();
	return true;
}

bool NetBuffer::OnWrite(size_type numOfBytes)
{
	if (numOfBytes > _buffer.size())
	{
		DYNAMIC_ASSERT(numOfBytes <= _buffer.size(),
			"new data size is over the maximum size of buffer");
		return false;
	}
	
	//여유 공간이 없다면? => 무조건 여유 공간을 만들어 본다
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
	_buffer = stdex::pmake_shared<NetBuffer>();
}

SendBuffer::SendBuffer(std::size_t size)
{
	_buffer = stdex::pmake_shared<NetBuffer>(size);
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
	//Release Ref and get new one => 나중에 pool을 쓰던가 해서 수정
	if (TLS_SendBuffer == nullptr || TLS_SendBuffer->FreeSize() < size)
	{
		// todo log
		if (size > Enum_NetBufferFlags::BUFFER_SIZE_DEFAULT)
			TLS_SendBuffer = stdex::pmake_shared<SendBuffer>(size);
		else
			TLS_SendBuffer = stdex::pmake_shared<SendBuffer>();
	}
		
	auto sendData = stdex::pmake_shared<SendData>(TLS_SendBuffer->GetNetBuffer(), TLS_SendBuffer->Get(), size);
	if (TLS_SendBuffer->Write(size) == false)
		return nullptr;

	return sendData;
}