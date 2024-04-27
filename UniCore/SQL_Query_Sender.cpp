#include "stdafx.h"
#include "SQL_Query_Sender.h"

#if defined(__SQL_ODBC)
int32 SQL_PROC_SENDER::bind_inputs(SQLHSTMT& sqlHstmt, int32& cursor)
{
	SQLLEN cbret;

	if (SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, sizeof(_ret), 0, &_ret, 0, &cbret) == SQL_ERROR)
	{
		return SQL_ERROR;
	}

	for (auto& item : _inputs)
	{
		if (SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, item._ctype, item._sqltype, item._size, 0, item._ptr, 0, NULL) == SQL_ERROR)
			return SQL_ERROR;
	}

	return 0;
}

int32 SQL_PROC_SENDER::bind_outputs(SQLHSTMT& sqlHstmt, int32& cursor)
{
	SQLLEN cbret;

	for (auto& item : _outputs)
	{
		if (SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, item._ctype, item._sqltype, item._size, 0, item._ptr, sizeof(cbret), &cbret) == SQL_ERROR)
			return SQL_ERROR;
	}

	return 0;
}

int32 SQL_PROC_SENDER::get_datas(SQLHSTMT& sqlHstmt)
{
	if (_records_request.size() == 0)
		return 0;

	int32 cursor = 1;
	while (SQLFetch(sqlHstmt) != SQL_NO_DATA)
	{
		auto& record = _records.add_record();
		for (int32 i = 0; i < _records_request.size(); i++)
		{
			auto& item = record.add_item(_records_request[i].first, _records_request[i].second);
			if (SQLGetData(sqlHstmt, cursor++, item.ctype, item.ptr, 0, NULL) == SQL_ERROR)
				return SQL_ERROR;
		}
	}

	return 0;
}

void SQL_PROC_SENDER::callback()
{
	if(_callback)
		_callback(this, &_records);
}
#endif