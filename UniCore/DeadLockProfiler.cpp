#include "stdafx.h"
#include "DeadLockProfiler.h"

#include <queue>

void DeadLockProfiler::PushLock(const int64& instance_id, const int32& lockIdx)
{
	//여러 스레드가 동시에 판별할 것이기 때문에, lock을 잡는다.
	auto pair_now = std::make_pair(instance_id, lockIdx);

	if (TLS_LockStack.empty() == false)
	{
		auto prev = TLS_LockStack.top();
		//만약 둘다 같다면 재귀락이라고 여길 수 있기 때문에 생략
		if (prev.first != instance_id || prev.second != lockIdx)
		{
			//이전에 연결이 수립된적이 없다면?
			if (_lockHistory[prev].find(pair_now) == _lockHistory[prev].end())
			{
				//연결 후 순환 체크
				_lockHistory[prev].insert(pair_now);
				CheckCycle();
			}
		}
	}

	PushStack(instance_id, lockIdx);
}

void DeadLockProfiler::PopLock(const int64& instance_id, const int32& lockIdx)
{
	//여러 스레드가 동시에 판별할 것이기 때문에, lock을 잡는다.
	std::lock_guard<std::mutex> lock(_lock);

	//해제할 lock이 있어야함
	DYNAMIC_ASSERT(TLS_LockStack.empty() == false, "MULTIPLE UNLOCK");

	//lock을 잠금의 역순에 따라 해제하지 않음.
	DYNAMIC_ASSERT(TLS_LockStack.top().first == instance_id && TLS_LockStack.top().second == lockIdx, "INVALID UNLOCK")

	//lock해제
	TLS_LockStack.pop();
}

void DeadLockProfiler::DFS(const std::pair<int64, int32>& now)
{
	//이미 발견됨
	if (_discovered[now] != -1)
		return;

	//아직 발견되지 않음 => 이제 발견한거다?
	_discovered[now] = _discovered_count++;

	auto connections = _lockHistory[now];

	for (auto& next : connections)
	{
		//아직 발견되지 않음 => 순리대로 계속 찾아가면 됨
		if (_discovered[next] == -1)
		{
			_traceback[next] = now;
			DFS(next);
			continue;
		}

		//다음 노드의 발견 순서가, 현재 노드보다 느림. => 순방향 간선
		if (_discovered[now] < _discovered[next])
			continue;

		//여기서부턴 현재 노드가 다음 노드보다 늦게 발견됨 => 루프를 만들 가능성이 생김

		//그런 와중에, 다음 노드가 아직 연결점들에 대한 모든 탐색이 끝나지 않음
		//그말은 즉, 어떤 노드 A가 DFS 호출중에 있는데, 재귀로 B점을 호출
		//그런데 B점이 다시 A점을 호출한 꼴 A -> B -> A 순환연결
		//A와 B 사이에 몇개의 점이 더 있던 A -> ...... -> B -> A 와 같이 결국 순환고리가 이어짐
		if (_ended[next] == false)
		{
			//TODO => TraceBack에 대한 정보는 저장했는데, ptr형태라 어떤 클래스인지 알 수 없음. 이부분 개선

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