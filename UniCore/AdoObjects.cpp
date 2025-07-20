#include "stdafx.h"
#include "AdoObjects.h"
#include "ADODBCon.h"


ado_parameter::ado_parameter()
{
    if (FAILED(_param.CreateInstance(__uuidof(Parameter))))
    {
        DYNAMIC_ASSERT(false, "failed to connect to db");
        CoUninitialize();
        return;
    }
}

bool ado_result::next_record()
{
    if (empty())
        return false;

    try
    {
        VARIANT lngRec; lngRec.llVal = 0;

        _record->NextRecordset(&lngRec);
    }
    catch (_com_error& e)
    {
        ADOErrorHandler::HandlerError(e);
        return false;
    }
    catch (...)
    {
        //알 수 없는 오류
        DYNAMIC_ASSERT(false, "ADODB");
        return false;
    }

    return true;
}

bool ado_result::move_first()
{
    if (empty())
        return false;

    try
    {
        long lngRec = 0;
        _record->MoveFirst();
    }
    catch (_com_error& e)
    {
        ADOErrorHandler::HandlerError(e);
        return false;
    }
    catch (...)
    {
        //알 수 없는 오류
        DYNAMIC_ASSERT(false, "ADODB");
        return false;
    }

    return true;
}

bool ado_result::move_next()
{
    if (empty())
        return false;

    try
    {
        _record->MoveNext();
    }
    catch (_com_error& e)
    {
        ADOErrorHandler::HandlerError(e);
        return false;
    }
    catch (...)
    {
        //알 수 없는 오류
        DYNAMIC_ASSERT(false, "ADODB");
        return false;
    }
    
    return true;
}

ado_conn_object::ado_conn_object(std::shared_ptr<DBConnector> owner, const _bstr_t& conn_string)
    : _owner(owner)
{
    connect(conn_string);
}

ado_conn_object::~ado_conn_object()
{
    _close();
}

void ado_conn_object::connect(const _bstr_t& conn_string)
{
    if (FAILED(_conn.CreateInstance(__uuidof(Connection))))
    {
        DYNAMIC_ASSERT(false, "failed to connect to db");
        CoUninitialize();
        return;
    }

    _conn->ConnectionString = conn_string;
    if (FAILED(_conn->Open("", "", "", adConnectUnspecified)))
    {
        DYNAMIC_ASSERT(false, "failed to connect to db");
        CoUninitialize();
        return;
    }
}

void ado_conn_object::pool_back()
{
    auto owner = _owner.lock();

    if (owner)
        owner->push(shared_from_this());
}

void ado_conn_object::_close() noexcept
{
    if (_conn && _conn->State == adStateOpen)
        _conn->Close();
}


SQL_SENDER::SQL_SENDER(_bstr_t query_string, CommandTypeEnum comm_type)
    : _query(static_cast<char*>(query_string)), _break(false)
{
    if (FAILED(_comm.CreateInstance(__uuidof(Command))))
    {
        DYNAMIC_ASSERT(false, "failed to connect to db");
        CoUninitialize();
        return;
    }

    _result = stdex::pmake_shared<ado_result>();
    _comm->CommandText = query_string;
    _comm->CommandType = comm_type;
    if (_comm->CommandType == CommandTypeEnum::adCmdStoredProc)
    {
        auto ret = ObjectPool<ado_parameter>::PoolShared();
        ret->_param->Type = adInteger;
        ret->_param->Size = sizeof(int);
        ret->_param->Direction = adParamReturnValue;
        ret->_param->Value = _variant_t(-1);

        _comm->Parameters->Append(ret->_param);

        _result->bind_ret(std::move(ret));
    }
}

SQL_SENDER::~SQL_SENDER()
{
    try
    {
        _clear();
    }
    catch (_com_error& e)
    {
        ADOErrorHandler::HandlerError(e);
    }
    catch (...)
    {
        //알 수 없는 오류
        DYNAMIC_ASSERT(false, "Qeury Callback Error :: %s", query_string());
    }
    
    if (_comm)
        _comm = nullptr;

    if (_result)
        _result = nullptr;
}

void SQL_SENDER::on_callback() noexcept
{
    try
    {
        if (_result == nullptr)
            throw -1;

        _callback(_result);
    }
    catch (_com_error& e)
    {
        ADOErrorHandler::HandlerError(e);
    }
    catch (...)
    {
        //알 수 없는 오류
        DYNAMIC_ASSERT(false, "Qeury Callback Error :: %s", query_string());
    }

    //자원 정리 => 소멸자에서 하는걸로 변경
    //_clear();
}

void SQL_SENDER::execute(const ConnectOptionEnum& exec_type)
{
    try
    {
        _result->set_record(_comm->Execute(nullptr, nullptr, exec_type));
        return;
    }
    catch (_com_error& e)
    {
        ADOErrorHandler::HandlerError(e);
    }
    catch (...)
    {
        //알 수 없는 오류
        DYNAMIC_ASSERT(false, "Qeury Callback Error :: [%s], Type :: [%d]", query_string(), static_cast<uint32>(exec_type));
    }

    _break = true;
}

void SQL_SENDER::_clear()
{
    _break = false;

    for (int i = _comm->Parameters->GetCount() - 1; i >= 0; i--)
        _comm->Parameters->Delete(_variant_t(static_cast<long>(i)));

    if (_result)
        _result = nullptr;

    if (_active_conn)
    {
        _active_conn->pool_back();
        _active_conn = nullptr;
    }

    _comm->ActiveConnection = nullptr;
}