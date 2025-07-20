#include "stdafx.h"
#include "RWLock.h"

#include "DeadLockProfiler.h"
#include <thread>

void RWLock::ReadLock()
{
	//현재 WriteLock을 걸어둔 와중에, ReadLock을 시도
	auto writeLockThreadID = (_lockFlag.load() & WRITE_LOCK_MASK) >> 16;
	//근데 만약 WriteLock을 건 thread가 해당 Thread라면? 문제없이 ReadLock가능
	//WriteLock -> ReadLock
	if (TLS_ThreadID == writeLockThreadID)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	//여기서부터 SpinLock
	auto timeoutTick = GETTICKCOUNT64() + MAX_SPIN_TIMEOUT_TICK;

	//응 스레드 안놔줘~
	while (true)
	{
		for (int32 i = 0; i < SPIN_COUNT; i++)
		{
			auto expected = _lockFlag.load() & READ_LOCK_MASK;

			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		DYNAMIC_ASSERT(timeoutTick < GETTICKCOUNT64(), "Time out in spinlock");

		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
	}
}

void RWLock::ReadUnlock()
{
	//ReadLock에 성공한 후에 UnLock을 한다고 가정하고 무조건 제거만 해준다.
	DYNAMIC_ASSERT((_lockFlag.fetch_sub(1) & READ_LOCK_MASK) > 0, "Invaild Unlock")
}

void RWLock::WriteLock()
{
	//현재 WriteLock을 걸어둔 와중에, WriteLock을 시도
	auto writeLockThreadID = ((_lockFlag.load() & WRITE_LOCK_MASK)) >> 16;
	//근데 만약 WriteLock을 건 thread가 해당 Thread라면? 문제없이 WriteLock가능
	if (TLS_ThreadID == writeLockThreadID)
	{
		_writeLockCount++;
		return;
	}

	//여기서부터 SpinLock
	auto timeoutTick = GETTICKCOUNT64() + MAX_SPIN_TIMEOUT_TICK;

	auto desire = (TLS_ThreadID << 16) & WRITE_LOCK_MASK;

	//응 스레드 안놔줘~
	while (true)
	{
		for (int32 i = 0; i < SPIN_COUNT; i++)
		{
			//나 들어갈거야
			_onWrite.store(true);
			//다 나가
			int32 expected = EMPTY_FLAG;
			//나갔냐? => 그럼 나 들어갈게?
			if (_lockFlag.compare_exchange_strong(OUT expected, desire))
			{
				_writeLockCount++;
				return;
			}
		}

		DYNAMIC_ASSERT(timeoutTick < GETTICKCOUNT64(), "Time out in spinlock");

		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
	}
}

void RWLock::WriteUnlock()
{
	//_writeLockCount 가 1보다 작음
	DYNAMIC_ASSERT(_writeLockCount > 0 && (_lockFlag.load() & READ_LOCK_MASK) == 0, "Invaild Unlock");

	_writeLockCount--;
	if (_writeLockCount == 0)
	{
		_onWrite.store(false);
		_lockFlag.exchange(EMPTY_FLAG);
	}
}





//각각에 ifdef를 적용했는데 하나만 해도 될지 고민
/*==========================
		  ReadLock
==========================*/
ReadLock::ReadLock(RWLock& lock, int64 instance_id, int32 lock_idx)
	:_lock(lock), _instance_id(instance_id), _lock_idx(lock_idx)
{
#ifdef _DEBUG
	GlobalHandler.deadLockProfiler->PushLock(_instance_id, _lock_idx);
#endif

	_lock.ReadLock();
}

ReadLock::ReadLock(RWLock& lock)
	:_lock(lock)
{
	_lock.ReadLock();
}

ReadLock::~ReadLock()
{
//#ifdef _DEBUG
//	GlobalHandler.deadLockProfiler->PopLock(_instance_id, _lock_idx);
//#endif

	_lock.ReadUnlock();
}

/*==========================
		 WriteLock
==========================*/

WriteLock::WriteLock(RWLock& lock, int64 instance_id, int32 lock_idx)
	:_lock(lock), _instance_id(instance_id), _lock_idx(lock_idx)
{
#ifdef _DEBUG
	GlobalHandler.deadLockProfiler->PushLock(_instance_id, _lock_idx);
#endif

	_lock.WriteLock();
}

WriteLock::WriteLock(RWLock& lock)
	:_lock(lock)
{
	_lock.WriteLock();
}

WriteLock::~WriteLock()
{
//#ifdef _DEBUG
//	GlobalHandler.deadLockProfiler->PopLock(_instance_id, _lock_idx);
//#endif

	_lock.WriteUnlock();
}