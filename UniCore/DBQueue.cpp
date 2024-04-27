#include "stdafx.h"
#include "DBQueue.h"
#if defined(__SQL_ODBC)
DBQueue::DBQueue(const bool& serialize, const std::shared_ptr<JobQueue>& callback_queue)
	:_serialize(serialize)
{
	//serialize가 false면 굳이 callback도 serialize 할 필요 없음.
	if (serialize == false)
	{
		_push = [this](const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback) {
			//아무 db스레드나 여유되면 실행하고
				GlobalHandler.dbaThreadManager->EnqueueJob([this, sender, callback]() { 
					sender->Send(); 
					if (callback == nullptr)
						return;

					//완료되면 아무 로직 스레드나 해당 작업을 완료해줘
					GlobalHandler.threadManager->EnqueueJob([callback]() {
						callback->Execute();
						});
				}, shared_from_this());
			};

		_push_mult = [this](const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback)
		{
			GlobalHandler.dbaThreadManager->EnqueueJob([this, senders, callback]() {
				//아무 db스레드나 여유되면 실행하고
				for (auto& sender : senders)
					sender->Send();

				if (callback == nullptr)
					return;

				//완료되면 아무 로직 스레드나 해당 작업을 완료해줘
				GlobalHandler.threadManager->EnqueueJob([callback]() {
					callback->Execute();
					});
				}, shared_from_this());
		};

		return;
	}
	_push = [this](const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback) 
	{
		int prev = _jobCount.fetch_add(1);
		_concurrent_queue.enqueue(J_MakeShared<ThreadJob>([sender, callback, this]() {
			sender->Send();
			//to logic thread
			if (callback)
				_callback_queue->Push(callback);
			}, shared_from_this()));

		//prev가 0이었다. => 시작을 안했거나 return으로 빠져나갔다.
		//== 재등록을 해야한다.
		if (prev == 0)
			GlobalHandler.dbaThreadManager->EnqueueJob([this]() {_Execute(); }, shared_from_this());
	};

	_push_mult = [this](const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback)
	{
		//sender가 여러개긴 해도 1개의 job으로 들어감
		int prev = _jobCount.fetch_add(1);
		_concurrent_queue.enqueue(J_MakeShared<ThreadJob>([senders, callback, this]() {
			for(auto& sender : senders)
				sender->Send();
			//to logic thread
			if (callback)
				_callback_queue->Push(callback);
			}, shared_from_this()));

		//prev가 0이었다. => 시작을 안했거나 return으로 빠져나갔다.
		//== 재등록을 해야한다.
		if (prev == 0)
			GlobalHandler.dbaThreadManager->EnqueueJob([this]() {_Execute(); }, shared_from_this());
	};

	//돌려줄 큐가 있으면
	if (callback_queue)
	{
		_callback_queue = callback_queue;
		return;
	}

	//없으면 내부에서 새로 만들어서 사용
	_callback_queue = J_MakeShared<JobQueue>();
}

void DBQueue::Push(const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback)
{
	_push(sender, callback);
}

void DBQueue::Push(const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback)
{
	_push_mult(senders, callback);
}

void DBQueue::_Execute()
{
	//루프에서 너무 오래 머무르지 않게 하기 위한 방어장치. => 스레드 과점유 방지
	auto startTick = GETTICKCOUNT64() + 0x40;

	while (true)
	{
		std::vector<std::shared_ptr<ThreadJob>> jobs;

		_concurrent_queue.dequeue_all(jobs);

		for (int32 i = 0; i < jobs.size(); i++)
			jobs[i]->Execute();

		int prev = _jobCount.fetch_sub(jobs.size());

		if (prev == 0) return;

		//아직 더 수행이 가능하다.
		if (startTick > GETTICKCOUNT64())
			continue;

		//더이상 실행이 불가능하다. startTick <= GETTICKCOUNT64() => TickTime을 모두 소진했다.
		GlobalHandler.dbaThreadManager->EnqueueJob([this]() {_Execute(); }, shared_from_this());
		return;
	}
}
#endif