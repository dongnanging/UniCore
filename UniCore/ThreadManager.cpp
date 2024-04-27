#include "stdafx.h"
#include "ThreadManager.h"

//ThreadManager::ThreadManager()
//{
//	_threadCount = std::thread::hardware_concurrency() * 2;
//
//	_MakeThreadPool(_threadCount);
//}
//
//ThreadManager::ThreadManager(int16 thread_count)
//	: _threadCount(thread_count)
//{
//	_MakeThreadPool(thread_count);
//}

ThreadManager::~ThreadManager() noexcept
{
	_running = false;

	try 
	{
		for (auto& thread : _threads)
			thread.join();
	}
	catch (...)
	{
		FILE_LOG("thread manager throws");
	}

	_exit();
}

void ThreadManager::AddWorker(int16 threadCount)
{
	_MakeThreadPool(threadCount);
}


void ThreadManager::_MakeThreadPool(const int16& count)
{
	_running = true;

	for (int16 i = 0; i < count; i++)
	{
		_threads.push_back(std::thread([this]() { 
			_init();
			_ThreadWork(); 
			_exit();
			}));
	}
}

void ThreadManager::_init()
{
	for (auto& func : _init_funcs)
	{
		if (func)
			func();
	}
}

void ThreadManager::_exit()
{
	for (auto& func : _exit_funcs)
	{
		if (func)
			func();
	}
}


void ThreadManager::EnqueueJob(std::function<void()>&& lambda, uint64 tickAfter)
{
	//어차피 rv
	auto job = J_MakeShared<ThreadJob>(std::move(lambda));
	EnqueueJob(job, tickAfter);
}

void ThreadManager::EnqueueJob(std::shared_ptr<ThreadJob>& threadJob, uint64 tickAfter)
{
	auto item = J_MakeShared<SJItem>(nullptr, std::move(threadJob), tickAfter);

	{
		WRITE_LOCK;
		_wait_queue.push(item);
	}
}

void ThreadManager::EnqueueJob(std::shared_ptr<ThreadJob>&& threadJob, uint64 tickAfter)
{
	auto item = J_MakeShared<SJItem>(nullptr, std::move(threadJob), tickAfter);

	{
		WRITE_LOCK;
		_wait_queue.push(item);
	}
}

void ThreadManager::EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>& job, uint64 tickAfter)
{
	auto item = J_MakeShared<SJItem>(owner, std::move(job), tickAfter);

	{
		WRITE_LOCK;
		_wait_queue.push(item);
	}
}

void ThreadManager::EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>&& job, uint64 tickAfter)
{
	auto item = J_MakeShared<SJItem>(owner, std::move(job), tickAfter);

	{
		WRITE_LOCK;
		_wait_queue.push(item);
	}
}

void ThreadManager::_ThreadWork()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	bool busy = false;

	while (true)
	{
		if (!_running) return;

		if (!busy)
		{
			THREAD_SLEEP;
			busy = true;
		}
			
		std::shared_ptr<SJItem> item;
		{
			//Get Lock obj
			WRITE_LOCK;
			if (_wait_queue.empty())
			{
				busy = false;
				continue;
			}

			item = _wait_queue.top();
			//아직 실행할 순간이 오지 않았다.
			if (item->exeTick > GETTICKCOUNT64())
			{
				busy = false;
				continue;
			}

			//제거 => 이제 처리한다?
			_wait_queue.pop();
		}

		//pass or exec
		item->Toss();
	}
	CoUninitialize();
}

