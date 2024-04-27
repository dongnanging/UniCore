#include "stdafx.h"
#include "DBConnector.h"

#if defined(__SQL_ODBC)
#include "SQLProcedure.h"

#include "SQL_Query_Sender.h"


DBConnector::~DBConnector()
{
    while (_connectionPool.empty() == false)
    {
        auto data = _connectionPool.front();
        SQLFreeHandle(SQL_HANDLE_STMT, data->statement);
        SQLFreeHandle(SQL_HANDLE_DBC, data->sqlDBConnection);
        SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);
    }
}

int32 DBConnector::Connect(const std::wstring& driver, const std::wstring& server, const std::wstring& dbname, const std::wstring& userid, const std::wstring& passwd)
{
    _driver = driver;
    _server = server;
    _dbname = dbname;
    _userid = userid;
    _passwd = passwd;

    for (int32 i = 0; i < 3; i++)
    {
        auto connection = _Connect();
        WRITE_LOCK;
        _connectionPool.push(connection);
    }

    return 0;
}


std::shared_ptr<ConnectionData> DBConnector::_pop_cn()
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
    if(data)
        _ClearCursor(data->statement);
    return data;
}

void DBConnector::_push_cn(const std::shared_ptr<ConnectionData>& connection)
{
    if (connection == nullptr)
        return;

    _ClearCursor(connection->statement);

    WRITE_LOCK;
    _connectionPool.push(connection);
}

void DBConnector::_ErrorHandler(SQLHANDLE handle)
{
    SQLLEN          numRecs = 0;
    SQLWCHAR        SqlState[6], SQLStmt[100], Msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER      NativeError;
    SQLSMALLINT     i, MsgLen;
    SQLRETURN       rc1, rc2;

    SQLGetDiagFieldW(SQL_HANDLE_STMT, handle, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);

    for (int i = 1; i <= numRecs; i++)
    {
        rc2 = SQLGetDiagRecW(SQL_HANDLE_STMT, handle, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen);

        std::wcout << SqlState << L" " << NativeError << L" " << Msg << L" " << MsgLen << std::endl;
    }
}

void DBConnector::_ClearCursor(SQLHSTMT handle)
{
    SQLCloseCursor(handle);
    SQLFreeStmt(handle, SQL_UNBIND);
}

std::shared_ptr<ConnectionData> DBConnector::_Connect()
{
    auto data = J_MakeShared<ConnectionData>();

    SQLRETURN   ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &data->sqlEnv);
    if (ret == SQL_ERROR)
    {
        _ErrorHandler(data->sqlEnv);
        SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);
        CRASH("DBConnection Failed!");
    }

    ret = SQLSetEnvAttr(data->sqlEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (ret == SQL_ERROR)
    {
        _ErrorHandler(data->sqlEnv);
        SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);
        CRASH("DBConnection Failed!");
    }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, data->sqlEnv, &data->sqlDBConnection);
    if (ret == SQL_ERROR)
    {
        _ErrorHandler(data->sqlEnv);
        SQLFreeHandle(SQL_HANDLE_DBC, data->sqlDBConnection);
        SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);
        CRASH("DBConnection Failed!");
    }

    std::wstring ConnectionWstring =
        L"DRIVER={" + _driver + L"};" +
        L"SERVER=" + _server + L";" +
        L"DATABASE=" + _dbname + L";" +
        L"UID=" + _userid + L";" +
        L"PWD=" + _passwd + L";";

    SQLWCHAR* connectionString = (SQLWCHAR*)ConnectionWstring.c_str();
    ret = SQLDriverConnectW(data->sqlDBConnection, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    if (ret == SQL_ERROR)
    {
        _ErrorHandler(data->sqlDBConnection);
        SQLFreeHandle(SQL_HANDLE_DBC, data->sqlDBConnection);
        SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);
        CRASH("DBConnection Failed!");
    }

    SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);

    ret = SQLAllocHandle(SQL_HANDLE_STMT, data->sqlDBConnection, &data->statement);
    if (ret == SQL_ERROR)
    {
        _ErrorHandler(data->sqlDBConnection);
        SQLFreeHandle(SQL_HANDLE_STMT, data->statement);
        SQLFreeHandle(SQL_HANDLE_DBC, data->sqlDBConnection);
        SQLFreeHandle(SQL_HANDLE_ENV, data->sqlEnv);
        CRASH("DBConnection Failed!");
    }

    return data;
}

void DBConnector::AsyncSend(const std::shared_ptr<SQL_PROC_SENDER>& sender)
{
    
}
void DBConnector::Send(const std::shared_ptr<SQL_PROC_SENDER>& sender)
{
    auto cn = _pop_cn();
    if (cn == nullptr)
        return;

    int32 cursor = 1;
    if (sender->bind_inputs(cn->statement, cursor) == SQL_ERROR)
    {
        sender->set_ret(SQL_ERROR);
        _ErrorHandler(cn->statement);
        _push_cn(cn);

        sender->callback();
        return;
    }
    if (sender->bind_outputs(cn->statement, cursor) == SQL_ERROR)
    {
        sender->set_ret(SQL_ERROR);
        _ErrorHandler(cn->statement);
        _push_cn(cn);

        sender->callback();
        return;
    }

    //즉시 실행
    auto err = SQLExecDirectW(cn->statement, sender->query(), SQL_NTS);
    if (err != SQL_SUCCESS && err != SQL_SUCCESS_WITH_INFO)
    {
        sender->set_ret(SQL_ERROR);
        _ErrorHandler(cn->statement);
        _push_cn(cn);

        sender->callback();
        return;
    }

    if (sender->get_datas(cn->statement) == SQL_ERROR)
    {
        sender->set_ret(SQL_ERROR);
        _ErrorHandler(cn->statement);
        _push_cn(cn);

        sender->callback();
        return;
    }

    sender->callback();
    _push_cn(cn);
    return;
}
void DBConnector::Send(const SQL_PROC_SENDER& sender)
{
    return;
}
#endif