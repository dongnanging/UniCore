#pragma once
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include "SQL_Query_Sender.h"

//야 컴파일러 나와!
class SQLHandler
{
	//static SQLLEN cbret;
public:
	template<typename... _Params>
	static inline int32 SQLBIndParameter_feed_input(SQLHSTMT& sqlHstmt, int32& cursor, std::tuple<_Params&...>& binders)
	{
		return SQLHandler::SQLBIndParameter_take_input(sqlHstmt, cursor, std::make_index_sequence<sizeof...(_Params)>(), binders);
	}

	template<typename... _Params, std::size_t... _Seq>
	static inline int32 SQLBIndParameter_take_input(SQLHSTMT& sqlHstmt, int32& cursor, std::index_sequence<_Seq...>, std::tuple<_Params&...>& binders)
	{
		return SQLHandler::SQLBIndParameter_all_input(sqlHstmt, cursor, std::get<_Seq>(binders)...);
	}

	template<typename _Head, typename... _Tail>
	static inline int32 SQLBIndParameter_all_input(SQLHSTMT& sqlHstmt, int32& cursor, _Head& item, _Tail&... remains)
	{
		if (SQLHandler::SQLBIndParameter_impl_input(sqlHstmt, cursor, item) == SQL_ERROR) return SQL_ERROR;
		if (SQLHandler::SQLBIndParameter_all_input(sqlHstmt, cursor, remains...) == SQL_ERROR) return SQL_ERROR;
		return 0;
	}

	template<typename _Head>
	static inline int32 SQLBIndParameter_all_input(SQLHSTMT& sqlHstmt, int32& cursor, _Head& item)
	{
		return SQLHandler::SQLBIndParameter_impl_input(sqlHstmt, cursor, item);
	}

	static inline int32 SQLBIndParameter_all_input(SQLHSTMT& sqlHstmt, int32& cursor)
	{
		//no bind val
		return 0;
	}
	
	template<typename _DataObject>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, _DataObject& item)
	{
		auto feed = item.FeedForward();
		return SQLHandler::SQLBIndParameter_feed_input(sqlHstmt, cursor, feed);
	}
	
	template<>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, bool& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(item), 0, (SQLPOINTER)&item, 0, NULL);
	}
	template<>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, int16& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(item), 0, (SQLPOINTER)&item, 0, NULL);
	}
	template<>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, int32& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(item), 0, (SQLPOINTER)&item, 0, NULL);
	}
	template<>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, int64& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(item), 0, (SQLPOINTER)&item, 0, NULL);
	}
	template<>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, std::string& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, item.size(), 0, (SQLPOINTER)item.c_str(), 0, NULL);
	}
	template<>
	static inline int32 SQLBIndParameter_impl_input(SQLHSTMT& sqlHstmt, int32& cursor, std::wstring& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, item.size(), 0, (SQLPOINTER)item.c_str(), 0, NULL);
	}

	//Binder
	template<typename... _Binder>
	static inline int32 SQLBIndParameter_feed_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, std::tuple<_Binder&...>& binders)
	{
		return SQLHandler::SQLBIndParameter_take_output(sqlHstmt, cursor, cbret, std::make_index_sequence<sizeof...(_Binder)>(), binders);
	}

	template<typename... _Binder, std::size_t... _Seq>
	static inline int32 SQLBIndParameter_take_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, std::index_sequence<_Seq...>, std::tuple<_Binder&...>& binders)
	{
		return SQLHandler::SQLBIndParameter_all_output(sqlHstmt, cursor, cbret, std::get<_Seq>(binders)...);
	}

	template<typename _Head, typename... _Tail>
	static inline int32 SQLBIndParameter_all_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, _Head& item, _Tail&... remains)
	{
		if (SQLHandler::SQLBIndParameter_impl_output(sqlHstmt, cursor, cbret, item) == SQL_ERROR) return SQL_ERROR;
		if (SQLHandler::SQLBIndParameter_all_output(sqlHstmt, cursor, cbret, remains...) == SQL_ERROR) return SQL_ERROR;
		return 0;
	}

	template<typename _Head>
	static inline int32 SQLBIndParameter_all_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, _Head& item)
	{
		return SQLHandler::SQLBIndParameter_impl_output(sqlHstmt, cursor, cbret, item);
	}

	static inline int32 SQLBIndParameter_all_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret)
	{
		//no bind val
		return 0;
	}

	template<typename _DataObject>
	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT _DataObject& item)
	{
		auto feed = item.FeedForward();
		return SQLHandler::SQLBIndParameter_feed_output(sqlHstmt, cursor, cbret, feed);
	}

	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT bool &item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_BIT, SQL_BIT, sizeof(item), 0, &item, sizeof(item), &cbret);
	}

	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT int16& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_SHORT, SQL_SMALLINT, sizeof(item), 0, &item, sizeof(item), &cbret);
	}

	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT int32& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, sizeof(item), 0, &item, sizeof(item), &cbret);
	}
	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT int64& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(item), 0, &item, sizeof(item), &cbret);
	}
	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT std::string& item)
	{
		item.resize(128);
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_VARCHAR, item.size(), 0, (SQLPOINTER)item.data(), item.size(), &cbret);
	}
	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT std::wstring& item)
	{
		item.resize(128);
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_WCHAR, SQL_WVARCHAR, item.size(), 0, (SQLPOINTER)item.data(), item.size(), &cbret);
	}

	static inline int32 SQLBIndParameter_impl_output(SQLHSTMT& sqlHstmt, int32& cursor, SQLLEN& cbret, OUT TIMESTAMP_STRUCT& item)
	{
		return SQLBindParameter(sqlHstmt, cursor++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(item), 0, &item, sizeof(item), &cbret);
	}

public:
	//GetData
	template<typename... _Memory>
	static inline int32 GetDatas_feed(SQLHSTMT& sqlHstmt, int32& cursor, std::tuple<_Memory&...>& memories)
	{
		return SQLHandler::GetDatas_take(sqlHstmt, cursor, std::make_index_sequence<sizeof...(_Memory)>(), memories);
	}

	template<typename... _Memory, std::size_t... _Seq>
	static inline int32 GetDatas_take(SQLHSTMT& sqlHstmt, int32& cursor, std::index_sequence<_Seq...> seq, std::tuple<_Memory&...>& memories)
	{
		return GetDatas_all(sqlHstmt, cursor, std::get<_Seq>(memories)...);
	}

	template<typename _Head, typename... _Tail>
	static inline int32 GetDatas_all(SQLHSTMT& sqlHstmt, int32& cursor, _Head& item, _Tail&... remains)
	{
		if (SQLHandler::GetDatas_impl(sqlHstmt, cursor, item) == SQL_ERROR) return SQL_ERROR;
		if (SQLHandler::GetDatas_all(sqlHstmt, cursor, remains...) == SQL_ERROR) return SQL_ERROR;
		return 0;
	}

	template<typename _Head>
	static inline int32 GetDatas_all(SQLHSTMT& sqlHstmt, int32& cursor, _Head& item)
	{
		return SQLHandler::GetDatas_impl(sqlHstmt, cursor, item);
	}

	static inline int32 GetDatas_all(SQLHSTMT& sqlHstmt, int32& cursor)
	{
		//no memory for storing
		return 0;
	}

	template<typename _DataObject>
	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, _DataObject& item)
	{
		auto feed = item.FeedForward();
		return GetDatas_feed(sqlHstmt, cursor, feed);
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, bool& item)
	{
		return SQLGetData(sqlHstmt, cursor++, SQL_C_BIT, &item, 0, NULL);
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, int16& item)
	{
		return SQLGetData(sqlHstmt, cursor++, SQL_C_SHORT, &item, 0, NULL);
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, int32& item)
	{
		return SQLGetData(sqlHstmt, cursor++, SQL_C_LONG, &item, 0, NULL);
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, int64& item)
	{
		return SQLGetData(sqlHstmt, cursor++, SQL_C_SBIGINT, &item, 0, NULL);
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, std::string& item)
	{
		item.resize(128);
		if (SQLGetData(sqlHstmt, cursor++, SQL_C_CHAR, (SQLPOINTER)item.data(), item.size(), NULL) == SQL_ERROR)
			return SQL_ERROR;
		auto cut = item.find('\0');
		if (cut != std::string::npos)
			item.resize(cut);
		return 0;
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, std::wstring& item)
	{
		item.resize(128);
		if (SQLGetData(sqlHstmt, cursor++, SQL_C_WCHAR, (SQLPOINTER)item.data(), item.size(), NULL) == SQL_ERROR)
			return SQL_ERROR;
		auto cut = item.find(L'\0');
		if (cut != std::wstring::npos)
			item.resize(cut);
		return 0;
	}

	static inline int32 GetDatas_impl(SQLHSTMT& sqlHstmt, int32& cursor, TIMESTAMP_STRUCT& item)
	{
		return SQLGetData(sqlHstmt, cursor++, SQL_C_TIMESTAMP, &item, sizeof(item), NULL);
	}

public:
	//with lv input
	template<typename... _InputParams, typename... _OutputParams, typename... _Memory>
	static int32 ProcCaster(SQLWCHAR* w_proc_query, SQLHSTMT& sqlHstmt, std::tuple<_InputParams...> inputs, OUT std::tuple<_OutputParams&...> outputs, OUT std::tuple<_Memory&...> memories)
	{
		SQLINTEGER ret;
		int32 cursor = 1;
		SQLLEN cbret;
		SQLLEN output;

		//proc return
		if (SQLBindParameter(sqlHstmt, cursor, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, sizeof(ret), 0, &ret, 0, &cbret) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}
		cursor++;

		if (SQLHandler::SQLBIndParameter_feed_input(sqlHstmt, cursor, inputs) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		if (SQLHandler::SQLBIndParameter_feed_output(sqlHstmt, cursor, output, outputs) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		auto err = SQLExecDirectW(sqlHstmt, w_proc_query, SQL_NTS);
		if (err != SQL_SUCCESS && err != SQL_SUCCESS_WITH_INFO)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		//stmt cursor가 이미 return까지 내려옴 -> 읽어야 할 데이터가 없음
		if (sizeof...(_Memory) == 0)
			return ret;

		if (SQLFetch(sqlHstmt) == SQL_NO_DATA)
		{
			//_ErrorHandler(sqlHstmt);
			return SQL_NO_DATA;
		}

		cursor = 1;
		if (SQLHandler::GetDatas_feed(sqlHstmt, cursor, memories) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		return ret;
	}

	//with lv input
	template<typename... _InputParams, typename... _OutputParams, typename... _Memory>
	static int32 ProcCaster(SQLWCHAR* w_proc_query, SQLHSTMT& sqlHstmt, std::tuple<_InputParams...> inputs, OUT std::tuple<_OutputParams&...> outputs, std::tuple<_Memory&...> buffer, OUT std::vector<std::tuple<_Memory...>>& result)
	{
		SQLINTEGER ret;
		int32 cursor = 1;
		SQLLEN cbret;
		SQLLEN output;

		//proc return
		if (SQLBindParameter(sqlHstmt, cursor, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, sizeof(ret), 0, &ret, 0, &cbret) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}
		cursor++;

		if (SQLHandler::SQLBIndParameter_take_input(sqlHstmt, cursor, std::make_index_sequence<sizeof...(_InputParams)>(), inputs) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		if (SQLHandler::SQLBIndParameter_take_output(sqlHstmt, cursor, output, std::make_index_sequence<sizeof...(_OutputParams)>(), outputs) == SQL_ERROR)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		auto err = SQLExecDirectW(sqlHstmt, w_proc_query, SQL_NTS);
		if (err != SQL_SUCCESS && err != SQL_SUCCESS_WITH_INFO)
		{
			_ErrorHandler(sqlHstmt);
			return SQL_ERROR;
		}

		//stmt cursor가 이미 return까지 내려옴 -> 읽어야 할 데이터가 없음
		if (sizeof...(_Memory) == 0)
			return ret;

		while (SQLFetch(sqlHstmt) != SQL_NO_DATA)
		{
			cursor = 1;
			if (SQLHandler::GetDatas_take(sqlHstmt, cursor, std::make_index_sequence<sizeof...(_Memory)>(), buffer) == SQL_ERROR)
			{
				_ErrorHandler(sqlHstmt);
				return SQL_ERROR;
			}

			result.push_back(StoreCopy(buffer, std::make_index_sequence<sizeof...(_Memory)>()));
		}

		return ret;
	}


private:
	static void _ErrorHandler(SQLHANDLE handle);

	template<typename... _Memory, std::size_t... _Seq>
	static auto StoreCopy(const std::tuple<_Memory&...>& source, const std::index_sequence<_Seq...>&)
	{
		return std::make_tuple(std::get<_Seq>(source)...);
	}
};