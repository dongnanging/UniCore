#pragma once
#include "AdoObjects.h"

using proc_sender = std::shared_ptr<SQL_SENDER>;
class DBConnector : public std::enable_shared_from_this<DBConnector>
{
public:
    DBConnector(const std::string& provider, const std::string& source, const std::string& dbname, const std::string& id, const std::string& pw, const uint16& port = 0, const bool& async = true);
    virtual ~DBConnector();



public:
    std::shared_ptr<ado_conn_object> pop();
    void push(std::shared_ptr<ado_conn_object> con);

    template<typename _Shared_Proc_Sender, 
        typename = std::enable_if_t<std::is_same_v<stdex::pure_type_t<_Shared_Proc_Sender>, proc_sender>, _Shared_Proc_Sender>>
    void Send(_Shared_Proc_Sender&& proc)
    {
        auto conn = pop();
        if (!conn)
        {
            DYNAMIC_ASSERT(false, "invalid connection! query send failed :: %s", proc->query_string());
            return;
        }

        proc->set_conn(conn);
        proc->execute(ConnectOptionEnum::adConnectUnspecified);
        if (proc->isDone())
        {
            proc->on_callback();
            return;
        }


        //바로 실행 완료되지 않으면 오류로 간주 (즉시 실행이었기 때문에 오류로 간주
        DYNAMIC_ASSERT(false, "Qeury excute failed :: %s", proc->query_string());
        return;
    }

    template<typename _Shared_Proc_Sender, 
        typename = std::enable_if_t<std::is_same_v<stdex::pure_type_t<_Shared_Proc_Sender>, proc_sender>, _Shared_Proc_Sender>>
    void AsyncSend(_Shared_Proc_Sender&& proc)
    {
        auto conn = pop();
        if (!conn)
        {
            DYNAMIC_ASSERT(false, "invalid connection! query send failed :: %s", proc->query_string());
            return;
        }

        proc->set_conn(conn);
        proc->execute(ConnectOptionEnum::adAsyncConnect);
        if (proc->isDone())
        {
            proc->on_callback();
            return;
        }

        _Awaiting(std::forward<_Shared_Proc_Sender>(proc));
    }

private:
    template<typename _Shared_Proc_Sender, 
        typename = std::enable_if_t<std::is_same_v<stdex::pure_type_t<_Shared_Proc_Sender>, proc_sender>, _Shared_Proc_Sender>>
    void _Awaiting(_Shared_Proc_Sender&& proc)
    {
        GlobalHandler.threadManager->EnqueueJob(J_MakeShared<ThreadJob>([proc = std::forward<_Shared_Proc_Sender>(proc), this]() mutable {
            if (proc->isDone())
            {
                proc->on_callback();
                return;
            }

            _Awaiting(std::forward<_Shared_Proc_Sender>(proc));
            }, shared_from_this()));
    }


private:
    bool _async;
    std::string _connection_string;
    
    USE_MULT_LOCK(2);
    std::queue<std::shared_ptr<ado_conn_object>> _connection_pool;
    std::queue<std::shared_ptr<SQL_SENDER>> _proc_mem;
};

