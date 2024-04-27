#pragma once
#if defined(__SQL_ODBC)
#include "SQL_Query_Sender.h"
#include "SQLHandler.h"

#include "boost/bind.hpp"

class DBConnector;
class ConnectionScope;

struct ConnectionData
{
    SQLHDBC     sqlDBConnection;
    SQLHENV     sqlEnv;
    SQLHSTMT    statement;
};

class DBConnector : public std::enable_shared_from_this<DBConnector>
{
    friend ConnectionScope;
public:
    ~DBConnector();
    int32 Connect(const std::wstring& driver, const std::wstring& server, const std::wstring& dbname, const std::wstring& userid, const std::wstring& passwd);

    std::shared_ptr<boost::asio::io_service> _io_service;

    //스코프가 빠져나갈 변수들을 받을 것 같으므로 참조없이 복사로 받음
    template<typename... _InputParam, typename... _OutputParam, typename... _Memory>
    int32 ProcedureCast(SQLWCHAR* proc, std::tuple<_InputParam...>&& inputs, std::tuple<_OutputParam&...>&& outputs, std::tuple<_Memory&...>&& memories)
    {
        std::shared_ptr<ConnectionData> data = nullptr;
        {
            WRITE_LOCK;
            if (_connectionPool.empty())
                data = _Connect();
            else
            {
                data = _connectionPool.front();
                _connectionPool.pop();
            }
        }

        _ClearCursor(data->statement);
        auto err = SQLHandler::ProcCaster(proc, data->statement, inputs, outputs, memories);

        WRITE_LOCK;
        _connectionPool.push(data);
        return err;
    }

    //스코프가 빠져나갈 변수들을 받을 것 같으므로 참조없이 복사로 받음
    template<typename... _InputParam, typename... _OutputParam, typename... _Memory>
    int32 ProcedureCast(SQLWCHAR* proc, std::tuple<_InputParam...>&& inputs, std::tuple<_OutputParam&...>&& outputs, std::tuple<_Memory&...>&& memories, OUT std::vector<std::tuple<_Memory...>>& result)
    {
        std::shared_ptr<ConnectionData> data = nullptr;
        {
            WRITE_LOCK;
            if (_connectionPool.empty())
                data = _Connect();
            else
            {
                data = _connectionPool.front();
                _connectionPool.pop();
            }
        }

        _ClearCursor(data->statement);
        auto err = SQLHandler::ProcCaster(proc, data->statement, inputs, outputs, memories, OUT result);

        WRITE_LOCK;
        _connectionPool.push(data);
        return err;
    }

    void AsyncSend(const std::shared_ptr<SQL_PROC_SENDER>& sender);
    void Send(const std::shared_ptr<SQL_PROC_SENDER>& sender);
    void Send(const SQL_PROC_SENDER& sender);

    

private:
    std::shared_ptr<ConnectionData> _pop_cn();
    void _push_cn(const std::shared_ptr<ConnectionData>& connection);

    void _ErrorHandler(SQLHANDLE handle);
    void _ClearCursor(SQLHSTMT handle);

    std::shared_ptr<ConnectionData> _Connect();

private:
    USE_LOCK;
    std::queue<std::shared_ptr<ConnectionData>> _connectionPool;

private:
    std::wstring _driver;
    std::wstring _server;
    std::wstring _dbname;
    std::wstring _userid;
    std::wstring _passwd;
};
#endif