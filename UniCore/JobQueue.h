#pragma once
#include "LockContainer.h"
#include "ThreadJob.h"

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	void Push(const std::shared_ptr<ThreadJob>& job);

private:
	void _Execute();

private:
	std::atomic<int32> _jobCount;
	LockContainer<std::shared_ptr<ThreadJob>, std::queue<std::shared_ptr<ThreadJob>>> _concurrent_queue;
};