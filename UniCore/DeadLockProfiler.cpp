#include "stdafx.h"
#include "DeadLockProfiler.h"

#include <queue>

void DeadLockProfiler::PushLock(const int64& instance_id, const int32& lockIdx)
{
	//���� �����尡 ���ÿ� �Ǻ��� ���̱� ������, lock�� ��´�.
	auto pair_now = std::make_pair(instance_id, lockIdx);

	if (TLS_LockStack.empty() == false)
	{
		auto prev = TLS_LockStack.top();
		//���� �Ѵ� ���ٸ� ��Ͷ��̶�� ���� �� �ֱ� ������ ����
		if (prev.first != instance_id || prev.second != lockIdx)
		{
			//������ ������ ���������� ���ٸ�?
			if (_lockHistory[prev].find(pair_now) == _lockHistory[prev].end())
			{
				//���� �� ��ȯ üũ
				_lockHistory[prev].insert(pair_now);
				CheckCycle();
			}
		}
	}

	PushStack(instance_id, lockIdx);
}

void DeadLockProfiler::PopLock(const int64& instance_id, const int32& lockIdx)
{
	//���� �����尡 ���ÿ� �Ǻ��� ���̱� ������, lock�� ��´�.
	std::lock_guard<std::mutex> lock(_lock);

	//������ lock�� �־����
	DYNAMIC_ASSERT(TLS_LockStack.empty() == false, "MULTIPLE UNLOCK");

	//lock�� ����� ������ ���� �������� ����.
	DYNAMIC_ASSERT(TLS_LockStack.top().first == instance_id && TLS_LockStack.top().second == lockIdx, "INVALID UNLOCK")

	//lock����
	TLS_LockStack.pop();
}

void DeadLockProfiler::DFS(const std::pair<int64, int32>& now)
{
	//�̹� �߰ߵ�
	if (_discovered[now] != -1)
		return;

	//���� �߰ߵ��� ���� => ���� �߰��ѰŴ�?
	_discovered[now] = _discovered_count++;

	auto connections = _lockHistory[now];

	for (auto& next : connections)
	{
		//���� �߰ߵ��� ���� => ������� ��� ã�ư��� ��
		if (_discovered[next] == -1)
		{
			_traceback[next] = now;
			DFS(next);
			continue;
		}

		//���� ����� �߰� ������, ���� ��庸�� ����. => ������ ����
		if (_discovered[now] < _discovered[next])
			continue;

		//���⼭���� ���� ��尡 ���� ��庸�� �ʰ� �߰ߵ� => ������ ���� ���ɼ��� ����

		//�׷� ���߿�, ���� ��尡 ���� �������鿡 ���� ��� Ž���� ������ ����
		//�׸��� ��, � ��� A�� DFS ȣ���߿� �ִµ�, ��ͷ� B���� ȣ��
		//�׷��� B���� �ٽ� A���� ȣ���� �� A -> B -> A ��ȯ����
		//A�� B ���̿� ��� ���� �� �ִ� A -> ...... -> B -> A �� ���� �ᱹ ��ȯ���� �̾���
		if (_ended[next] == false)
		{
			//TODO => TraceBack�� ���� ������ �����ߴµ�, ptr���¶� � Ŭ�������� �� �� ����. �̺κ� ����

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_ended[now] = true;
}

void DeadLockProfiler::PushStack(const int64& instance_id, const int32& lockIdx)
{
	TLS_LockStack.emplace(instance_id, lockIdx);
}

void DeadLockProfiler::CheckCycle()
{
	_discovered_count = 1;
	_discovered.clear();
	_ended.clear();

	for (auto& itr : _discovered)
	{
		itr.second = -1;
	}
	for (auto& itr : _ended)
	{
		itr.second = false;
	}

	for (auto& item : _lockHistory)
	{
		DFS(item.first);
	}
}