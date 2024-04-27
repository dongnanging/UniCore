#pragma once
#include <atomic>

/// <summary>
/// RWLock
/// 1. ReadLock 후에 WriteLock 불가능 => 불가능으로 두는게 성능 향상에 좋다는 결론에 도달함
/// 2. WriteLock 후에 ReadLock 가능 => 이미 WriteLock을 잡았기 때문에, 문제가 없어보임 => 재귀도 문제가 없어보임
/// </summary>
class RWLock
{
	enum LockFlags
	{
		READ_LOCK_MASK = 0x0000'ffff,
		WRITE_LOCK_MASK = 0xffff'0000,
		EMPTY_FLAG = 0x0000'0000,

		SPIN_COUNT = 500,

		SLEEP_TIME = 1,			//1 = 1ms휴식, 0 = yield와 유사

		MAX_SPIN_TIMEOUT_TICK = 0xffff,
	};

public:
	void ReadLock();
	void ReadUnlock();

	void WriteLock();
	void WriteUnlock();

private:
	std::atomic<int32>	_lockFlag = 0;
	std::atomic<bool>	_onWrite = false;

	//아직 별 의미는 없음 => _onWrite와 대체?
	int16				_writeLockCount = 0;
};


/// <summary>
/// ReadLock
/// </summary>
class ReadLock
{
public:
	ReadLock(RWLock& lock, int64 instance_id, int32 lock_idx);
	ReadLock(RWLock& lock);
	~ReadLock();
private:
	RWLock& _lock;
	int64 _instance_id;
	int32 _lock_idx;
};


/// <summary>
/// WriteLock
/// </summary>
class WriteLock
{
public:
	WriteLock(RWLock& lock, int64 instance_id, int32 lock_idx);
	WriteLock(RWLock& lock);
	~WriteLock();

private:
	RWLock& _lock;
	int64 _instance_id;
	int32 _lock_idx;
};