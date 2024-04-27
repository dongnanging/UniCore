#pragma once
#include <unordered_map>
#include <set>
#include <mutex>

//기본적인 연결은 유향 그래프고, 그 유향 그래프의 간선을 이을 때, 사이클이 생기는지 판별
//1차로 만들어본게 클래스별 이름만 가지고 간단히 판별해 보는거였는데
//단점으론, 클래스 이름이 어떤 인스턴스, 그리고 그 인스턴스의 동기화 객체(lock)를 가리키진 않는다.
//================================
//이번 트라이의 개선 사항은 그런 인스턴스와, 동기화 객체를 하나의 노드로 치부하고,
//그런 노드간의 연결 속에서 순환 간선을 탐색한다.

//동일한 스레드에서 나타나는 순환 간선은 엄밀히 말하면 deadlock은 아니다.
//하지만 반나절동안 생각해본 결과, 판별 요소에 스레드까지 들어가면 고려가 복잡해지고
//구조도 복잡해지고, 판별도 힘들어지고 속도도 매우 느릴것으로 생각되고
//무엇보다 동일한 스레드일지라도 순환 간선을 만드는 잠금(lock)행위는 여전히 비합리적이라는 생각이 든다.

//다음의 개선 사항으로 남겨두겠다.
class DeadLockProfiler
{
	struct PairHash
	{
	public:
		std::size_t operator() (const std::pair<int64, int32> item) const
		{
			return std::hash<long long>()(item.first ^ (item.second * (item.second + 1) * (item.second + 2)));
		}
	};

public:
	void PushLock(const int64& instance_id, const int32& lockIdx);
	void PopLock(const int64& instance_id, const int32& lockIdx);

private:
	void DFS(const std::pair<int64, int32>& now);
	void PushStack(const int64& instance_id, const int32& lockIdx);
	void CheckCycle();

private:
	// ***unordered_map< instance_id(prev), ***unordered_map< lockIdx(prev), ***std::unordered_map< instance_id(now), ***std::set< lockIdx(now) > > > >
	std::unordered_map<std::pair<int64, int32>, std::set<std::pair<int64, int32>>, PairHash> _lockHistory;
	std::mutex _lock;

private:
	int32 _discovered_count = 1;
	std::unordered_map<std::pair<int64, int32>, int32, PairHash>	_discovered;
	std::unordered_map<std::pair<int64, int32>, bool, PairHash>	_ended;
	std::unordered_map<std::pair<int64, int32>, std::pair<int64, int32>, PairHash> _traceback;
};