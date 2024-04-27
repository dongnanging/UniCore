#pragma once
#include "DBQueue.h"
#include "JobQueue.h"
#include "ThreadJob.h"
#include "SQL_Query_Sender.h"

#if defined(__SQL_ODBC)
class DBQueue : public std::enable_shared_from_this<DBQueue>
{
public:
	DBQueue(const bool& serialize = true, const std::shared_ptr<JobQueue>& callback_queue = nullptr);

public:
	void Push(const std::shared_ptr<SQL_Query_Sender>& sender, const std::shared_ptr<ThreadJob>& callback = nullptr);
	void Push(const std::vector<std::shared_ptr<SQL_Query_Sender>>& senders, const std::shared_ptr<ThreadJob>& callback = nullptr);

private:
	void _Execute();

private:
	std::atomic<int32> _jobCount;
	LockContainer<std::shared_ptr<ThreadJob>, std::queue<std::shared_ptr<ThreadJob>>> _concurrent_queue;
	//직렬화로한 db작업은 callback도 직렬적으로 수행되도록 유도해야함
	std::shared_ptr<JobQueue> _callback_queue;

private:
	bool _serialize;
	std::function<void(const std::shared_ptr<SQL_Query_Sender>&, const std::shared_ptr<ThreadJob>&)> _push;
	std::function<void(const std::vector<std::shared_ptr<SQL_Query_Sender>>&, const std::shared_ptr<ThreadJob>&)> _push_mult;
};
#endif