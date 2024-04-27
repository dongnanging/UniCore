#include "stdafx.h"
#include "DBQueue.h"
#if defined(__SQL_ODBC)
DBQueue::DBQueue(const bool& serialize, const std::shared_ptr<JobQueue>& callback_queue)
	:_serialize(serialize)
{
	//serialize�� false�� ���� callback�� serialize �� �ʿ� ����.
	if (serialize == false)
	{
		_push = [this](const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback) {
			//�ƹ� db�����峪 �����Ǹ� �����ϰ�
				GlobalHandler.dbaThreadManager->EnqueueJob([this, sender, callback]() { 
					sender->Send(); 
					if (callback == nullptr)
						return;

					//�Ϸ�Ǹ� �ƹ� ���� �����峪 �ش� �۾��� �Ϸ�����
					GlobalHandler.threadManager->EnqueueJob([callback]() {
						callback->Execute();
						});
				}, shared_from_this());
			};

		_push_mult = [this](const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback)
		{
			GlobalHandler.dbaThreadManager->EnqueueJob([this, senders, callback]() {
				//�ƹ� db�����峪 �����Ǹ� �����ϰ�
				for (auto& sender : senders)
					sender->Send();

				if (callback == nullptr)
					return;

				//�Ϸ�Ǹ� �ƹ� ���� �����峪 �ش� �۾��� �Ϸ�����
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

		//prev�� 0�̾���. => ������ ���߰ų� return���� ����������.
		//== ������ �ؾ��Ѵ�.
		if (prev == 0)
			GlobalHandler.dbaThreadManager->EnqueueJob([this]() {_Execute(); }, shared_from_this());
	};

	_push_mult = [this](const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback)
	{
		//sender�� �������� �ص� 1���� job���� ��
		int prev = _jobCount.fetch_add(1);
		_concurrent_queue.enqueue(J_MakeShared<ThreadJob>([senders, callback, this]() {
			for(auto& sender : senders)
				sender->Send();
			//to logic thread
			if (callback)
				_callback_queue->Push(callback);
			}, shared_from_this()));

		//prev�� 0�̾���. => ������ ���߰ų� return���� ����������.
		//== ������ �ؾ��Ѵ�.
		if (prev == 0)
			GlobalHandler.dbaThreadManager->EnqueueJob([this]() {_Execute(); }, shared_from_this());
	};

	//������ ť�� ������
	if (callback_queue)
	{
		_callback_queue = callback_queue;
		return;
	}

	//������ ���ο��� ���� ���� ���
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
	//�������� �ʹ� ���� �ӹ����� �ʰ� �ϱ� ���� �����ġ. => ������ ������ ����
	auto startTick = GETTICKCOUNT64() + 0x40;

	while (true)
	{
		std::vector<std::shared_ptr<ThreadJob>> jobs;

		_concurrent_queue.dequeue_all(jobs);

		for (int32 i = 0; i < jobs.size(); i++)
			jobs[i]->Execute();

		int prev = _jobCount.fetch_sub(jobs.size());

		if (prev == 0) return;

		//���� �� ������ �����ϴ�.
		if (startTick > GETTICKCOUNT64())
			continue;

		//���̻� ������ �Ұ����ϴ�. startTick <= GETTICKCOUNT64() => TickTime�� ��� �����ߴ�.
		GlobalHandler.dbaThreadManager->EnqueueJob([this]() {_Execute(); }, shared_from_this());
		return;
	}
}
#endif