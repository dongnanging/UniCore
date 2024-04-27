#include "stdafx.h"
#include "ADODBCon.h"

DBConnector::DBConnector(const std::string& provider, const std::string& source, const std::string& dbname, const std::string& id, const std::string& pw, const uint16& port, const bool& async)
{
    _async = async;
    _connection_string = std::format("Provider={};Data Source={};Initial Catalog={};User Id={};Password={};{}", provider, source, dbname, id, pw, (port > 0 ? std::format("Network Address={},{};", source, port) : ""));
}

DBConnector::~DBConnector()
{
    WRITE_LOCK;
    while (!_connection_pool.empty())
    {
        auto cptr = _connection_pool.front();
        if(cptr)
            cptr->_close();

        _connection_pool.pop();
    }
}

std::shared_ptr<ado_conn_object> DBConnector::pop()
{
    std::shared_ptr<ado_conn_object> result = nullptr;

    {
        WRITE_LOCK;
        if (!_connection_pool.empty())
        {
            result = _connection_pool.front();
            _connection_pool.pop();
        }
    }

    if (result == nullptr)
    {
        try 
        {
            result = J_MakeShared<ado_conn_object>(shared_from_this(), static_cast<_bstr_t>(_connection_string.c_str()));
        }
        catch(_com_error& e)
        {
            ADOErrorHandler::HandlerError(e);
        }
        catch (...)
        {
            //알 수 없는 오류
            DYNAMIC_ASSERT(false, "ADODB");
        }
    }
        

    //그래도 nullptr이면 뭔가 문제가 있음
    DYNAMIC_ASSERT(result, "ado connection failed");
    return result;
}

void DBConnector::push(std::shared_ptr<ado_conn_object> con)
{
    {
        WRITE_LOCK;
        _connection_pool.push(con);
    }
}