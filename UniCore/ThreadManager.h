#pragma once
#include "JobQueue.h"

struct SJItem
{
	enum Enum_JobType
	{
		ANONYM,
		OCCUPIED,
	};

public:
	SJItem(std::shared_ptr<JobQueue> owner, std::shared_ptr<ThreadJob> job, uint64 tickAfter)
		: _job(job)
	{
		if (owner == nullptr)
		{
			_type = Enum_JobType::ANONYM;
		}
		else
		{
			_type = Enum_JobType::OCCUPIED;
			_owner = owner;
		}

		exeTick = GETTICKCOUNT64() + tickAfter;
	}

public:
	void Toss()
	{
		if (_type == Enum_JobType::ANONYM)
		{
			_job->Execute();
			return;
		}

		auto sptr = _owner.lock();
		if (sptr)
		{
			sptr->Push(_job);
		}
	}

public:
	uint64 exeTick;
private:
	std::weak_ptr<JobQueue> _owner;
	std::shared_ptr<ThreadJob> _job;

	Enum_JobType _type;
};

struct sj_compare
{
	bool operator()(std::shared_ptr<SJItem> first, std::shared_ptr<SJItem> second)
	{
		return first->exeTick > second->exeTick;
	}
};

using thread_func = std::function<void()>;
class ThreadManager
{
public:
	//ThreadManager();
	//ThreadManager(int16 thread_count);
	ThreadManager() = default;
	~ThreadManager() noexcept;

	ThreadManager(const ThreadManager& other) = delete;
	ThreadManager(ThreadManager&& right) = delete;
	ThreadManager& operator=(const ThreadManager& other) = delete;
	ThreadManager& operator=(ThreadManager&& right) = delete;

public:
	/// <summary>
	/// warning. it takes call thread forever.
	/// </summary>
	//void AddWorker();
	void AddWorker(int16 threadCount);
	int16 GetThreadCount() { return _threads.size(); }

	void AddInitFunc(thread_func&& func)
	{
		func();
		_init_funcs.push_back(std::forward<thread_func>(func));
	}

	void AddExitFunc(thread_func&& func)
	{
		func();
		_exit_funcs.push_back(std::forward<thread_func>(func));
	}

private:
	inline void _MakeThreadPool(const int16& count);
	inline void _ThreadWork();
	inline void _init();
	inline void _exit();

public:
	void EnqueueJob(std::function<void()>&& lambda, uint64 tickAfter = 0);
	void EnqueueJob(std::shared_ptr<ThreadJob>& threadJob, uint64 tickAfter = 0);
	void EnqueueJob(std::shared_ptr<ThreadJob>&& threadJob, uint64 tickAfter = 0);
	void EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>& job, uint64 tickAfter = 0);
	void EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>&& job, uint64 tickAfter = 0);

	template<typename _Validator>
	void EnqueueJob(std::function<void()>&& lambda, std::shared_ptr<_Validator>& execValidator, uint64 tickAfter = 0)
	{
		EnqueueJob(J_MakeShared<ThreadJob>(std::move(lambda), std::move(execValidator)), tickAfter);
	}
	template<typename _Validator>
	void EnqueueJob(std::function<void()>&& lambda, std::shared_ptr<_Validator>&& execValidator, uint64 tickAfter = 0)
	{
		EnqueueJob(J_MakeShared<ThreadJob>(std::move(lambda), std::move(execValidator)), tickAfter);
	}

private:
	bool _running = false;
	std::vector<std::thread> _threads;

	std::vector< thread_func> _init_funcs;
	std::vector< thread_func> _exit_funcs;

private:
	USE_LOCK;
	std::priority_queue<std::shared_ptr<SJItem>, std::vector<std::shared_ptr<SJItem>>, sj_compare> _wait_queue;
};