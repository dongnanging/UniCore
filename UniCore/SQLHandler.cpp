#include "stdafx.h"
#include "SQLHandler.h"

void SQLHandler::_ErrorHandler(SQLHANDLE handle)
{
	SQLLEN          numRecs = 0;
	SQLWCHAR        SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH]; //, SQLStmt[100]
	SQLINTEGER      NativeError;
	SQLSMALLINT     i, MsgLen;
	SQLRETURN       rc1;// , rc2;

	SQLGetDiagFieldW(SQL_HANDLE_STMT, handle, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);

	for (i = 1; i <= numRecs; i++)
	{
		rc1 = SQLGetDiagRecW(SQL_HANDLE_STMT, handle, i, SqlState, &NativeError, Msg, SQL_MAX_MESSAGE_LENGTH, &MsgLen);

		std::wcout << SqlState << L" " << NativeError << L" " << Msg << L" " << MsgLen << std::endl;
	}

	//CRASH("test");
}