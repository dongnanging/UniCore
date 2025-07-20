#pragma once
#include "JobQueue.h"

#include <iterator>
#include <algorithm>

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

#include "Actor.h"
struct ThreadMessage {
	std::queue<std::shared_ptr<ThreadJob>> jobs;
	std::queue<std::shared_ptr<SJItem>> sjobs;
};

class ActorThread : public Actor<std::shared_ptr<ThreadMessage>> {
	void _Handle(std::shared_ptr<ThreadMessage>& msg) override {
		while (!msg->jobs.empty())
		{
			msg->jobs.front()->Execute();
			msg->jobs.pop();
		}

		while (!msg->sjobs.empty())
		{
			msg->sjobs.front()->Toss();
			msg->sjobs.pop();
		}
	}
};

using thread_func = std::function<void()>;
class ThreadManager : public stdex::sigleton<ThreadManager>
{
	enum LockEnum : std::size_t {
		JOBQUEUE,
		WAITQUEUE,
		NAX,
	};
	USE_MULT_LOCK(NAX);

public:
	//ThreadManager();
	//ThreadManager(int16 thread_count);
	ThreadManager();
	~ThreadManager() noexcept;

public:
	/// <summary>
	/// warning. it takes call thread forever.
	/// </summary>
	//void AddWorker();
	void AddWorker(int16 threadCount);
	void AddAnchor(thread_func&& func);
	const auto GetThreadCount() { return _threads.size(); }

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
	void _MakeThreadPool(const int16& count);
	void _ThreadWork();
	void _Schedule();
	void _init();
	void _exit();

public:
	void StartSchedule();

public:
	void EnqueueJob(std::function<void()>&& lambda, uint64 tickAfter = 0);
	void EnqueueJob(std::shared_ptr<ThreadJob>& threadJob, uint64 tickAfter = 0);
	void EnqueueJob(std::shared_ptr<ThreadJob>&& threadJob, uint64 tickAfter = 0);
	void EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>& job, uint64 tickAfter = 0);
	void EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>&& job, uint64 tickAfter = 0);

	template<typename _Validator>
	void EnqueueJob(std::function<void()>&& lambda, std::shared_ptr<_Validator>& execValidator, uint64 tickAfter = 0)
	{
		EnqueueJob(stdex::pmake_shared<ThreadJob>(std::move(lambda), std::move(execValidator)), tickAfter);
	}
	template<typename _Validator>
	void EnqueueJob(std::function<void()>&& lambda, std::shared_ptr<_Validator>&& execValidator, uint64 tickAfter = 0)
	{
		EnqueueJob(stdex::pmake_shared<ThreadJob>(std::move(lambda), std::move(execValidator)), tickAfter);
	}

	template<typename _Class, typename _Ret, typename... _Args>
	void EnqueueJob(uint64 tickAfter, _Class* owner, _Ret(_Class::* memFunc)(_Args...), _Args... args)
	{
		EnqueueJob(stdex::pmake_shared<ThreadJob>([=]() {
			(owner->*memFunc)(args...);
			}), tickAfter);
	}


	template<typename _Class, typename _Ret, typename... _Args>
	void EnqueueJob(_Class* owner, _Ret(_Class::* memFunc)(_Args...), _Args... args)
	{
		WRITE_LOCK_IDX(JOBQUEUE);
		_job_queue.push(stdex::pmake_shared<ThreadJob>([=]() {
			(owner->*memFunc)(args...);
		}));
	}

private:

	bool _running = false;
	std::vector<std::thread> _threads;
	std::vector<std::thread> _anchored;

	std::thread _scheduler;
	std::vector<std::shared_ptr<ActorThread>> _athreads;

	std::vector< thread_func> _init_funcs;
	std::vector< thread_func> _exit_funcs;

private:
	std::queue<std::shared_ptr<ThreadJob>> _job_queue;
	std::priority_queue<std::shared_ptr<SJItem>, std::vector<std::shared_ptr<SJItem>>, sj_compare> _wait_queue;
};

