#include "stdafx.h"
#include "JobQueue.h"

void JobQueue::Push(const std::shared_ptr<ThreadJob>& job)
{
	auto prev = _jobCount.fetch_add(1);

	_concurrent_queue.enqueue(job);

	//prev가 0이었다. => 시작을 안했거나 return으로 빠져나갔다.
	//== 재등록을 해야한다.
	if (prev == 0)
		ThreadManager::GetInstnace()->EnqueueJob([this]() {_Execute(); }, shared_from_this());
}

void JobQueue::_Execute()
{
	//루프에서 너무 오래 머무르지 않게 하기 위한 방어장치. => 스레드 과점유 방지
	auto startTick = GETTICKCOUNT64() + 0x40;

	while (true)
	{
		std::vector<std::shared_ptr<ThreadJob>> jobs;

		_concurrent_queue.dequeue_all(jobs);

		for (decltype(jobs.size()) i = 0; i < jobs.size(); i++)
			jobs[i]->Execute();

		auto prev = _jobCount.fetch_sub(jobs.size());

		if (prev == 0) return;

		//아직 더 수행이 가능하다.
		if (startTick > GETTICKCOUNT64())
			continue;

		//더이상 실행이 불가능하다. startTick <= GETTICKCOUNT64() => TickTime을 모두 소진했다.
		ThreadManager::GetInstnace()->EnqueueJob([this]() {_Execute(); }, shared_from_this());
		return;
	}
}