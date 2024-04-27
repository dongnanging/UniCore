#pragma once
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#if defined(__SQL_ODBC)
class SQL_PROC_SENDER;
struct SQL_RECORDS;
struct SQL_RESULT;

using sql_varchar = char;
using sql_nvarchar = wchar_t;

#define SQL_RET_FORM(...) __VA_ARGS__
#define SVARS SQL_RET_FORM(SQL_PROC_SENDER* sender)
using sql_callback = std::function<void(SQL_PROC_SENDER* sender, SQL_RECORDS* records)>;
#define SQL_RET(...) [__VA_ARGS__](SQL_PROC_SENDER* sender, SQL_RECORDS* records)

enum
{
	SQL_NO_RETURN = -12349
};

class SQL_Query_Sender : public std::enable_shared_from_this<SQL_Query_Sender>
{
public:
	virtual void Send() = 0;
	const auto& ret() const { return _ret; }

	const auto& build() { return shared_from_this(); }

protected:
	int32 _ret = SQL_NO_RETURN;
};



class SQL_VAL
{
	friend SQL_PROC_SENDER;

public:
	SQL_VAL()
	{}

	template<typename _Ty, typename = std::enable_if_t<!std::is_pointer_v<_Ty>>>
	SQL_VAL(const _Ty& value, const std::size_t& size)
		: _size(size)
	{
		_set_ptr<std::remove_all_const_t<_Ty>>(size);
		//_ptr = custom_alloc(size);
		::memcpy_s(_ptr, size, &value, size);
	}
	template<typename _Ty, typename = std::enable_if_t<std::is_pointer_v<_Ty>>>
	SQL_VAL(_Ty value, const std::size_t& size)
		: _size(size)
	{
		_set_ptr<std::remove_all_const_t<_Ty>>(size);
		//_ptr = custom_alloc(size);
		::memcpy_s(_ptr, size, value, size);
	}

	virtual ~SQL_VAL()
	{
		if (_ptr)
			custom_free(_ptr);
	}


	template<typename _Ty>
	void _set_ptr(const std::size_t& size) { DYNAMIC_ASSERT(false, "invalid sql type"); }

	template<>
	void _set_ptr<bool>(const std::size_t& size)
	{
		_ptr = custom_alloc(sizeof(bool));
		_size = sizeof(bool);
		_ctype = SQL_C_BIT;
		_sqltype = SQL_BIT;
	}
	template<>
	void _set_ptr<int8>(const std::size_t& size)
	{
		_ptr = custom_alloc(sizeof(int8));
		_size = sizeof(int8);
		_ctype = SQL_C_STINYINT;
		_sqltype = SQL_TINYINT;
	}
	template<>
	void _set_ptr<int16>(const std::size_t& size)
	{
		_ptr = custom_alloc(sizeof(int16));
		_size = sizeof(int16);
		_ctype = SQL_C_SHORT;
		_sqltype = SQL_SMALLINT;
	}
	template<>
	void _set_ptr<int32>(const std::size_t& size)
	{
		_ptr = custom_alloc(sizeof(int32));
		_size = sizeof(int32);
		_ctype = SQL_C_LONG;
		_sqltype = SQL_INTEGER;
	}
	template<>
	void _set_ptr<int64>(const std::size_t& size)
	{
		_ptr = custom_alloc(sizeof(int64));
		_size = sizeof(int64);
		_ctype = SQL_C_SBIGINT;
		_sqltype = SQL_BIGINT;
	}
	template<>
	void _set_ptr<sql_varchar*>(const std::size_t& size)
	{
		_ptr = custom_alloc(size);
		_size = size;
		_ctype = SQL_C_CHAR;
		_sqltype = SQL_VARCHAR;
	}
	template<>
	void _set_ptr<sql_nvarchar*>(const std::size_t& size)
	{
		_ptr = custom_alloc(size * 2);	//wchar_t = 2bytes
		_size = size;
		_ctype = SQL_C_WCHAR;
		_sqltype = SQL_WVARCHAR;
	}
	template<>
	void _set_ptr<TIMESTAMP_STRUCT>(const std::size_t& size)
	{
		_ptr = custom_alloc(sizeof(TIMESTAMP_STRUCT));
		_size = sizeof(TIMESTAMP_STRUCT);
		_ctype = SQL_C_TIMESTAMP;
		_sqltype = SQL_TIMESTAMP;
	}

private:
	SQLPOINTER _ptr;
	SQLLEN _size;
	int32 _ctype;
	int32 _sqltype;
};


struct SQL_RECORD_ITEM
{
	SQL_RECORD_ITEM(const int32& ctype, const std::size_t& size)
		: ctype(ctype), size(size)
	{
		switch (ctype)
		{
		case SQL_C_BIT:
			ptr = custom_alloc(sizeof(bool));
			break;
		case SQL_C_STINYINT:
			ptr = custom_alloc(sizeof(int8));
			break;
		case SQL_C_SHORT:
			ptr = custom_alloc(sizeof(int16));
			break;
		case SQL_C_LONG:
			ptr = custom_alloc(sizeof(int32));
			break;
		case SQL_C_SBIGINT:
			ptr = custom_alloc(sizeof(int64));
			break;
		case SQL_C_CHAR:
			ptr = custom_alloc(size);
			::memset(ptr, 0x00, size);
			break;
		case SQL_C_WCHAR:
			ptr = custom_alloc(size * 2);
			::memset(ptr, 0x00, size * 2);
			break;
		case SQL_C_TIMESTAMP:
			ptr = custom_alloc(sizeof(TIMESTAMP_STRUCT));
			break;
		default:
			DYNAMIC_ASSERT(false, "invalid record type");
			break;
		}
	}
	virtual ~SQL_RECORD_ITEM()
	{
		custom_free(ptr);
	}

public:
	SQLPOINTER ptr;
	SQLLEN size;
	int32 ctype;
};
struct SQL_RECORD
{
	friend SQL_PROC_SENDER;
public:
	template<typename _Cast>
	const _Cast& get(const int32& row)
	{
		return *reinterpret_cast<_Cast*>(_items[row].ptr);
	}

private:
	SQL_RECORD_ITEM& add_item(const int32& ctype, const std::size_t& size) { _items.emplace_back(ctype, size); return _items.back(); }

private:
	std::vector<SQL_RECORD_ITEM> _items;
};

struct SQL_RECORDS
{
	friend SQL_PROC_SENDER;
public:
	const auto& get_size() { return _records.size(); }
	auto& get_record(const int& col) { return _records[col]; }

private:
	SQL_RECORD& add_record() { _records.push_back(SQL_RECORD()); return _records.back(); };

private:
	std::vector<SQL_RECORD> _records;
};

struct SQL_RESULT
{
	friend SQL_PROC_SENDER;
public:
	auto& records() { return _record; };
	auto& ret() { return _ret; }

	void set_ret(const int32& ret) { _ret = ret; }
private:
	SQLINTEGER _ret;
	SQL_RECORDS _record;
};

class SQL_PROC_SENDER
{
public:
	template<std::size_t _Seq>
	SQL_PROC_SENDER(const sql_nvarchar(&sp_query)[_Seq])
		: _sp_query(sp_query)
	{
		_inputs.reserve(5);
	}

	SQL_PROC_SENDER(const sql_nvarchar* sp_query)
		: _sp_query(sp_query)
	{
		_inputs.reserve(5);
	}

	auto set_input(const bool& value)
	{
		_inputs.emplace_back(value, sizeof(value));
		return this;
	}
	auto set_input(const int8& value)
	{
		_inputs.emplace_back(value, sizeof(value));
		return this;
	}
	auto set_input(const int16& value)
	{
		_inputs.emplace_back(value, sizeof(value));
		return this;
	}
	auto set_input(const int32& value)
	{
		_inputs.emplace_back(value, sizeof(value));
		return this;
	}
	auto set_input(const int64& value)
	{
		_inputs.emplace_back(value, sizeof(value));
		return this;
	}
	auto set_input(const std::string& value)
	{
		_inputs.emplace_back(value.c_str(), value.size() + 1);
		return this;
	}
	auto set_input(const std::wstring& value)
	{
		_inputs.emplace_back(value.c_str(), value.size() + 1);
		return this;
	}
	//포인터를 제외한 배열일 경우 아래와 같이 처리
	template<typename _Ty, std::size_t _Size, typename = std::enable_if_t<!std::is_pointer_v<_Ty>>>
	auto set_input(_Ty(&value)[_Size])
	{
		_inputs.emplace_back(reinterpret_cast<_Ty*>(value), sizeof(_Ty) * _Size);
	}

	template<typename _Output>
	auto set_output(const std::size_t& size = 0)
	{
		_outputs.emplace_back();
		_outputs.back()._set_ptr<_Output>(size);
		return this;
	}

	template<typename _RecordType>
	auto set_record(const std::size_t& size = 0)
	{
		DYNAMIC_ASSERT(false, "invalid record type");
		return this;
	}
	template<>
	auto set_record<bool>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_BIT, size);
		return this;
	}
	template<>
	auto set_record<int8>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_STINYINT, size);
		return this;
	}
	template<>
	auto set_record<int16>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_SHORT, size);
		return this;
	}
	template<>
	auto set_record<int32>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_LONG, size);
		return this;
	}
	template<>
	auto set_record<int64>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_SBIGINT, size);
		return this;
	}
	template<>
	auto set_record<sql_varchar*>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_CHAR, size);
		return this;
	}
	template<>
	auto set_record<sql_nvarchar*>(const std::size_t& size)
	{
		_records_request.emplace_back(SQL_C_WCHAR, size);
		return this;
	}

	void set_callback(const sql_callback& callback)
	{
		_callback = callback;
	}

	template<typename _Validator>
	void set_callback(sql_callback&& callback)
	{
		_callback = std::move(callback);
	}


	int32 bind_inputs(SQLHSTMT& sqlHstmt, int32& cursor);
	int32 bind_outputs(SQLHSTMT& sqlHstmt, int32& cursor);
	int32 get_datas(SQLHSTMT& sqlHstmt);

	void callback();

public:
	void set_ret(const int32& ret) { _ret = ret; }
	const auto& ret() { return _ret; }

	const auto& records() { return _records; }

public:
	SQLWCHAR* query() { return (SQLWCHAR*)_sp_query.c_str(); }
	const auto& query_string() { return _sp_query; }

private:
	std::wstring _sp_query = L"";
	std::vector<SQL_VAL> _inputs;
	std::vector<SQL_VAL> _outputs;
	std::vector<std::pair<int32, std::size_t>> _records_request;

	SQLINTEGER _ret = 0x7fffffff;
	SQL_RECORDS _records;
	sql_callback _callback = nullptr;
};
#endif

