#pragma once
#include <atomic>

/// <summary>
/// RWLock
/// 1. ReadLock �Ŀ� WriteLock �Ұ��� => �Ұ������� �δ°� ���� ��� ���ٴ� ��п� ������
/// 2. WriteLock �Ŀ� ReadLock ���� => �̹� WriteLock�� ��ұ� ������, ������ ����� => ��͵� ������ �����
/// </summary>
class RWLock
{
	enum LockFlags
	{
		READ_LOCK_MASK = 0x0000'ffff,
		WRITE_LOCK_MASK = 0xffff'0000,
		EMPTY_FLAG = 0x0000'0000,

		SPIN_COUNT = 500,

		SLEEP_TIME = 1,			//1 = 1ms�޽�, 0 = yield�� ����

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

	//���� �� �ǹ̴� ���� => _onWrite�� ��ü?
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