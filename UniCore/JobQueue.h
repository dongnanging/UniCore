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
	std::atomic<std::size_t> _jobCount;
	LockContainer<std::shared_ptr<ThreadJob>, std::queue<std::shared_ptr<ThreadJob>>> _concurrent_queue;
};