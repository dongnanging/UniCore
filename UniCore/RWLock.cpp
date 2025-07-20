#include "stdafx.h"
#include "RWLock.h"

#include "DeadLockProfiler.h"
#include <thread>

void RWLock::ReadLock()
{
	//���� WriteLock�� �ɾ�� ���߿�, ReadLock�� �õ�
	auto writeLockThreadID = (_lockFlag.load() & WRITE_LOCK_MASK) >> 16;
	//�ٵ� ���� WriteLock�� �� thread�� �ش� Thread���? �������� ReadLock����
	//WriteLock -> ReadLock
	if (TLS_ThreadID == writeLockThreadID)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	//���⼭���� SpinLock
	auto timeoutTick = GETTICKCOUNT64() + MAX_SPIN_TIMEOUT_TICK;

	//�� ������ �ȳ���~
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
	//ReadLock�� ������ �Ŀ� UnLock�� �Ѵٰ� �����ϰ� ������ ���Ÿ� ���ش�.
	DYNAMIC_ASSERT((_lockFlag.fetch_sub(1) & READ_LOCK_MASK) > 0, "Invaild Unlock")
}

void RWLock::WriteLock()
{
	//���� WriteLock�� �ɾ�� ���߿�, WriteLock�� �õ�
	auto writeLockThreadID = ((_lockFlag.load() & WRITE_LOCK_MASK)) >> 16;
	//�ٵ� ���� WriteLock�� �� thread�� �ش� Thread���? �������� WriteLock����
	if (TLS_ThreadID == writeLockThreadID)
	{
		_writeLockCount++;
		return;
	}

	//���⼭���� SpinLock
	auto timeoutTick = GETTICKCOUNT64() + MAX_SPIN_TIMEOUT_TICK;

	auto desire = (TLS_ThreadID << 16) & WRITE_LOCK_MASK;

	//�� ������ �ȳ���~
	while (true)
	{
		for (int32 i = 0; i < SPIN_COUNT; i++)
		{
			//�� ���ž�
			_onWrite.store(true);
			//�� ����
			int32 expected = EMPTY_FLAG;
			//������? => �׷� �� ����?
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
	//_writeLockCount �� 1���� ����
	DYNAMIC_ASSERT(_writeLockCount > 0 && (_lockFlag.load() & READ_LOCK_MASK) == 0, "Invaild Unlock");

	_writeLockCount--;
	if (_writeLockCount == 0)
	{
		_onWrite.store(false);
		_lockFlag.exchange(EMPTY_FLAG);
	}
}





//������ ifdef�� �����ߴµ� �ϳ��� �ص� ���� ���
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