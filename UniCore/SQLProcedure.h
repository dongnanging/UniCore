#pragma once
#include "SQLHandler.h"

//�����Ϸ��� �ο�� ����2

//�� Ʃ���̶�, 
// �� Ʃ��(std::tuple<>)�� �ִ��� �˾Ƽ� ó��������
// Procedure���� �ʿ�� �ϴµ� �ȳ����� �翬�� ��������?

class SQLProcedure
{
public:
	//input only
	/*template<typename... _InputParams, typename... _Memory>
	static int32 ProcCaster(SQLWCHAR* w_proc_query, SQLHSTMT& sqlHstmt, std::tuple<_InputParams&...> inputs, std::tuple<_Memory&...> memories)
	{
		SQLINTEGER ret = -1;
		int32 cursor = 1;
		SQLLEN cbret;

		//proc return
		if (SQLBindParameter(sqlHstmt, cursor, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, sizeof(ret), 0, &ret, 0, &cbret) == SQL_ERROR)
			return -1;
		cursor++;

		if (SQLHandler::SQLBIndParameter_take_input(sqlHstmt, cursor, std::make_index_sequence<sizeof...(_InputParams)>(), inputs) == -1)
			return -1;

		auto err = SQLExecDirectW(sqlHstmt, w_proc_query, SQL_NTS);
		if (err != SQL_SUCCESS && err != SQL_SUCCESS_WITH_INFO)
			return -1;

		if (ret < 0)
			return ret;

		if (SQLFetch(sqlHstmt) == SQL_NO_DATA)
			return -1;

		cursor = 1;
		if (SQLHandler::GetDatas_take(sqlHstmt, cursor, std::make_index_sequence<sizeof...(_Memory)>(), memories) == -1)
			return -1;

		return ret;
	}*/

	static inline SQLWCHAR* CREATE_ACCOUNT() { return (SQLWCHAR*)L"{? = CALL CREATE_ACCOUNT(?, ?, ?)}"; }
	static inline SQLWCHAR* SIGN_IN() { return (SQLWCHAR*)L"{? = CALL SIGN_IN(?, ?, ?)}"; }
	static inline SQLWCHAR* GET_ACCOUNT_DATA() { return (SQLWCHAR*)L"{? = CALL GET_ACCOUNT_DATA(?)}"; }
};