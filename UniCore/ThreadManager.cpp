#include "stdafx.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	
}

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
	// _MakeThreadPool(threadCount);

	for (auto i = 0; i < threadCount; i++)
		_athreads.push_back(stdex::pmake_shared<ActorThread>());

	for (auto i = 0; i < threadCount; i++)
	{
		_threads.emplace_back([athread = _athreads[i]]() {
			while (true)
			{
				auto ret = athread->HandleMsg();
				if (!ret) THREAD_SLEEP;
			}
			});
	}
}

void ThreadManager::AddAnchor(thread_func&& func)
{
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

void ThreadManager::StartSchedule()
{
	// _scheduler = std::thread([this]() {
		_Schedule();
	// });
}


void ThreadManager::EnqueueJob(std::function<void()>&& lambda, uint64 tickAfter)
{
	//������ rv
	auto job = stdex::pmake_shared<ThreadJob>(std::move(lambda));
	EnqueueJob(job, tickAfter);
}

void ThreadManager::EnqueueJob(std::shared_ptr<ThreadJob>& threadJob, uint64 tickAfter)
{
	auto item = stdex::pmake_shared<SJItem>(nullptr, std::move(threadJob), tickAfter);

	{
		WRITE_LOCK_IDX(WAITQUEUE);
		_wait_queue.push(item);
	}
}

void ThreadManager::EnqueueJob(std::shared_ptr<ThreadJob>&& threadJob, uint64 tickAfter)
{
	auto item = stdex::pmake_shared<SJItem>(nullptr, std::move(threadJob), tickAfter);

	{
		WRITE_LOCK_IDX(WAITQUEUE);;
		_wait_queue.push(item);
	}
}

void ThreadManager::EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>& job, uint64 tickAfter)
{
	auto item = stdex::pmake_shared<SJItem>(owner, std::move(job), tickAfter);

	{
		WRITE_LOCK_IDX(WAITQUEUE);;
		_wait_queue.push(item);
	}
}

void ThreadManager::EnqueueJob(const std::shared_ptr<JobQueue>& owner, std::shared_ptr<ThreadJob>&& job, uint64 tickAfter)
{
	auto item = stdex::pmake_shared<SJItem>(owner, std::move(job), tickAfter);

	{
		WRITE_LOCK_IDX(WAITQUEUE);;
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
			WRITE_LOCK_IDX(WAITQUEUE);;
			if (_wait_queue.empty())
			{
				busy = false;
				continue;
			}

			item = _wait_queue.top();
			//���� ������ ������ ���� �ʾҴ�.
			if (item->exeTick > GETTICKCOUNT64())
			{
				busy = false;
				continue;
			}

			//���� => ���� ó���Ѵ�?
			_wait_queue.pop();
		}

		//pass or exec
		item->Toss();
	}
	CoUninitialize();
}

template<class _Ty, class _Comp = std::less<_Ty>>
struct access : std::priority_queue<_Ty, std::vector<_Ty>, _Comp> {
	using std::priority_queue<_Ty, std::vector<_Ty>, _Comp>::c; // ���� vector
	using std::priority_queue<_Ty, std::vector<_Ty>, _Comp>::comp; // ���� compare
};

// ������ ���� �����췯�ε�, ���ϰ� Ŀ���� �۾� �������� �θ� �ȴ�.
void ThreadManager::_Schedule()
{
	decltype(_job_queue) local_queue;
	decltype(_wait_queue) local_wait_queue;

	auto threadCount = _athreads.size();
	auto RR = 0;

	std::vector<std::pair<decltype(local_queue), std::queue<decltype(_wait_queue)::value_type>>> distributed(threadCount);
	if (threadCount <= 0)
		return;


	using wait_queue_type = decltype(local_wait_queue);
	auto& dc = static_cast<access<typename wait_queue_type::value_type, typename wait_queue_type::value_compare>&>(local_wait_queue).c;
	auto& cmp = static_cast<access<typename wait_queue_type::value_type, typename wait_queue_type::value_compare>&>(local_wait_queue).comp;

	while (true)
	{
		{
			WRITE_LOCK_IDX(JOBQUEUE);
			_job_queue.swap(local_queue);
		}

		decltype(local_wait_queue) temp_wait_queue;
		{
			WRITE_LOCK_IDX(WAITQUEUE);
			_wait_queue.swap(temp_wait_queue);
		}

		if (!temp_wait_queue.empty())
		{
			auto& sc = static_cast<access<typename wait_queue_type::value_type, typename wait_queue_type::value_compare>&>(temp_wait_queue).c;

			dc.reserve(dc.size() + sc.size());
			dc.insert(dc.end(), std::make_move_iterator(sc.begin()), std::make_move_iterator(sc.end()));
			std::make_heap(dc.begin(), dc.end(), cmp); // ������ 1ȸ ����
		}

		auto now = GETTICKCOUNT64();
		if (local_queue.empty() && (local_wait_queue.empty() || local_wait_queue.top()->exeTick > now))
		{
			THREAD_SLEEP;
			continue;
		}

		// �Ϲ� �۾� �й�
		while (!local_queue.empty())
		{
			distributed[RR].first.emplace(std::move(local_queue.front()));
			local_queue.pop();
			RR++;
			RR = RR >= threadCount ? 0 : RR;
		}

		// �ð� �۾� �й�.
		while (!local_wait_queue.empty() && local_wait_queue.top()->exeTick <= now)
		{
			distributed[RR].second.emplace(std::move(local_wait_queue.top()));
			local_wait_queue.pop();
			RR++;
			RR = RR >= threadCount ? 0 : RR;
		}

		for (std::size_t i = 0; i < distributed.size(); i++)
		{
			auto& [job, sjob] = distributed[i];
			if (job.size() == 0 && sjob.size() == 0)
				continue;

			auto msg = stdex::pmake_shared<ThreadMessage>();
			job.swap(msg->jobs);
			sjob.swap(msg->sjobs);
			_athreads[i]->SendMsg(std::move(msg));
		}
	}
}