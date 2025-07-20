#include "stdafx.h"
#include "JobQueue.h"

void JobQueue::Push(const std::shared_ptr<ThreadJob>& job)
{
	auto prev = _jobCount.fetch_add(1);

	_concurrent_queue.enqueue(job);

	//prev�� 0�̾���. => ������ ���߰ų� return���� ����������.
	//== ������ �ؾ��Ѵ�.
	if (prev == 0)
		ThreadManager::GetInstnace()->EnqueueJob([this]() {_Execute(); }, shared_from_this());
}

void JobQueue::_Execute()
{
	//�������� �ʹ� ���� �ӹ����� �ʰ� �ϱ� ���� �����ġ. => ������ ������ ����
	auto startTick = GETTICKCOUNT64() + 0x40;

	while (true)
	{
		std::vector<std::shared_ptr<ThreadJob>> jobs;

		_concurrent_queue.dequeue_all(jobs);

		for (decltype(jobs.size()) i = 0; i < jobs.size(); i++)
			jobs[i]->Execute();

		auto prev = _jobCount.fetch_sub(jobs.size());

		if (prev == 0) return;

		//���� �� ������ �����ϴ�.
		if (startTick > GETTICKCOUNT64())
			continue;

		//���̻� ������ �Ұ����ϴ�. startTick <= GETTICKCOUNT64() => TickTime�� ��� �����ߴ�.
		ThreadManager::GetInstnace()->EnqueueJob([this]() {_Execute(); }, shared_from_this());
		return;
	}
}