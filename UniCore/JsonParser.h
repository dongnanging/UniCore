#pragma once
#include<map>
#include<unordered_map>
#include<fstream>
#include<stack>


class JsonParser;
class JsonParserW;

enum class JOType
{
	NONE = 0,
	HEADER,
	SUBJSON,
	ARRAY,
	STRING,
	WSTRING,
	INTEGER,
	LONGLONG,
	FLOAT,
	DOUBLE,
};

template<typename _Derived>
class JsonItemBase : public std::enable_shared_from_this<_Derived>
{
	friend JsonParser;
	friend JsonParserW;

protected:
	using item_type = _Derived;
	using enable_shared = std::enable_shared_from_this<_Derived>;

protected:
	JsonItemBase() = default;

public:
	bool has_value() { return _has_value; }
	const auto GetInt32() { if (_value_type != JOType::INTEGER) { DYNAMIC_ASSERT(false, "invalid json value_type") return 0; }							return *reinterpret_cast<int*>(_value_item.get()); }
	const auto GetInt64() { if (_value_type != JOType::INTEGER) { DYNAMIC_ASSERT(false, "invalid json value_type") return static_cast<long long>(0); }	return *reinterpret_cast<long long*>(_value_item.get()); }
	const auto GetFloat() { if (_value_type != JOType::FLOAT) { DYNAMIC_ASSERT(false, "invalid json value_type") return 0.0f; }							return *reinterpret_cast<float*>(_value_item.get()); }
	const auto GetDouble() { if (_value_type != JOType::DOUBLE) { DYNAMIC_ASSERT(false, "invalid json value_type") return 0.0; }						return *reinterpret_cast<double*>(_value_item.get()); }
	const auto GetString() { if (_value_type != JOType::STRING) { DYNAMIC_ASSERT(false, "invalid json value_type") return std::string(); }				return *reinterpret_cast<std::string*>(_value_item.get()); }
	const auto GetWstring() { if (_value_type != JOType::WSTRING) { DYNAMIC_ASSERT(false, "invalid json value_type") return std::wstring(); }			return *reinterpret_cast<std::wstring*>(_value_item.get()); }

	void AddList(std::shared_ptr<item_type> item) { _arrays.push_back(item); }

	std::shared_ptr<item_type> get(const std::size_t& idx) 
	{ 
		if (idx > _arrays.size() - 1 || _value_type != JOType::ARRAY)
		{ 
			DYNAMIC_ASSERT(false, "invalid json value_type"); 
			return nullptr; 
		} 
		return _arrays[idx]; 
	}
	std::shared_ptr<item_type> operator [] (const size_t& idx) 
	{
		if (idx > _arrays.size() - 1 || _value_type != JOType::ARRAY)
		{
			DYNAMIC_ASSERT(false, "invalid json value_type");
			return nullptr;
		} 
		return _arrays[idx]; 
	}

	auto list_view() { return stdex::shared_view(_arrays, enable_shared::shared_from_this()); }

protected:
	template<typename _Value>
	void _set_value(_Value&& val)
	{
		if constexpr (stdex::is_cstr_v<stdex::pure_type_t<_Value>>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<std::string>(std::forward<_Value>(val)));
			_value_type = JOType::STRING;
		}
		else if constexpr (stdex::is_wcstr_v<stdex::pure_type_t<_Value>>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<std::wstring>(std::forward<_Value>(val)));
			_value_type = JOType::WSTRING;
		}
		else if constexpr (stdex::is_same_v<int32, _Value>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<int32>(std::forward<_Value>(val)));
			_value_type = JOType::INTEGER;
		}
		else if constexpr (stdex::is_same_v<int64, _Value>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<int64>(std::forward<_Value>(val)));
			_value_type = JOType::LONGLONG;
		}
		else if constexpr (stdex::is_same_v<float, _Value>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<float>(std::forward<_Value>(val)));
			_value_type = JOType::FLOAT;
		}
		else if constexpr (stdex::is_same_v<double, _Value>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<double>(std::forward<_Value>(val)));
			_value_type = JOType::DOUBLE;
		}
		else
		{
			static_assert(
				stdex::is_cstr_v<stdex::pure_type_t<_Value>>	||
				stdex::is_wcstr_v<stdex::pure_type_t<_Value>>	||
				stdex::is_same_v<int32, _Value>					||
				stdex::is_same_v<int64, _Value>					||
				stdex::is_same_v<float, _Value>					||
				stdex::is_same_v<double, _Value>
				, "set value failed! invalid value type");
		}

		_has_value = true;
	}

protected:
	std::shared_ptr<item_type> _parent;
	std::vector<std::shared_ptr<item_type>> _arrays;


	// 값을 가집니까
	bool _has_value;
	// value 값 => 소멸자 구현 안하려고 포인터 대신 사용
	std::shared_ptr<void> _value_item;
	// value type
	JOType _value_type;
};

template<typename _Derived, typename _StrType, 
	typename = std::enable_if_t<stdex::is_stl_string_v<_StrType>>>	//stl 스트링이 아니면 허가 x
class JsonDivision : public JsonItemBase<_Derived>
{
	friend JsonParser;
	friend JsonParserW;

protected:
	using Base = JsonItemBase<_Derived>;
	using item_type = typename Base::item_type;
	using enable_shared = Base::enable_shared;

public:
	void AddPair(std::shared_ptr<item_type> item) { _childs.insert({ item->_key, item }); }
	std::shared_ptr<item_type> get(const _StrType& key) 
	{ 
		if (_childs.find(key) == _childs.end() || Base::_value_type != JOType::SUBJSON)
		{
			DYNAMIC_ASSERT(false, "invalid json value_type"); return nullptr;
			return nullptr;
		}
		
		return _childs[key]; 
	}
	std::shared_ptr<item_type> operator [] (const _StrType& key) 
	{
		if (_childs.find(key) == _childs.end() || Base::_value_type != JOType::SUBJSON)
		{
			DYNAMIC_ASSERT(false, "invalid json value_type"); return nullptr;
			return nullptr;
		}
		return _childs[key]; 
	}

	auto child_view() { return stdex::shared_view(_childs, enable_shared::shared_from_this()); }

protected:
	template<typename _StrType>
	struct str_elem
	{
		template<typename = stdex::enable_same_t<_StrType, std::string>>
		static constexpr const char elem(const char ch)
		{
			return ch;
		}

		template<typename = stdex::enable_same_t<_StrType, std::wstring>>
		static constexpr const wchar_t elem(const char ch)
		{
			return static_cast<wchar_t>(ch);
		}
	};

public:
	bool set_value(_StrType val)
	{
		try
		{
			// 문자열 표현식이 있는가?
			auto count = std::count(val.begin(), val.end(), str_elem<_StrType>::elem('"'));

			if (count > 1)
			{
				Base::_set_value(val);
				return true;
			}

			// 문자열은 아님
			auto fcount = std::count(val.begin(), val.end(), str_elem<_StrType>::elem('f'));
			auto dotcount = std::count(val.begin(), val.end(), str_elem<_StrType>::elem('.'));
			if (fcount > 0)
			{
				Base::_set_value(std::stof(val));
				return true;
			}

			// 플롯은 아니지만 소수점 표현식
			if (dotcount > 0)
			{
				Base::_set_value(std::stod(val));
				return true;
			}

			// 소수점 표현식이 아님 => 전부 정수형으로 표시
			auto llcount = std::count(val.begin(), val.end(), str_elem<_StrType>::elem('l'));
			if (llcount > 1)
			{
				Base::_set_value(std::stoll(val));
				return true;
			}

			// long long이 아니면 그냥 int로 표기
			Base::_set_value(std::stoi(val));
			return true;
		}
		catch (const std::invalid_argument& e)
		{
			DYNAMIC_ASSERT(false, "set value failed! invalied value");
			DEBUG_CODE(std::cerr << "Invalid argument: " << e.what() << std::endl);
		}
		catch (const std::out_of_range& e)
		{
			DYNAMIC_ASSERT(false, "set value failed! invalied value");
			DEBUG_CODE(std::cerr << "Out of range: " << e.what() << std::endl);
		}
		catch (...)
		{
			DYNAMIC_ASSERT(false, "set value failed! invalied value");
		}

		return false;
	}

protected:
	// key 값
	_StrType _key;
	std::unordered_map<_StrType, std::shared_ptr<item_type>> _childs;
};


class JsonItem : public JsonDivision<JsonItem, std::string>
{
	friend JsonParser;

public:
	JsonItem(const std::shared_ptr<JsonItem>& parent)
	{
		_parent = parent;
		_has_value = false; 
		_value_type = JOType::NONE;
	}
};

class JsonItemW : public JsonDivision<JsonItemW, std::wstring>
{
	friend JsonParserW;

public:
	JsonItemW(const std::shared_ptr<JsonItemW>& parent)
	{
		_parent = parent;
		_has_value = false;
		_value_type = JOType::NONE;
	}
};


class JsonParser
{
public:
	static std::shared_ptr<JsonItem> Parse(const std::string& source_file);
	static std::vector<std::shared_ptr<JsonItem>> _Parse(const std::shared_ptr<JsonItem>& parent, const char opening, const std::string& source, int32& indexer);
};

class JsonParserW
{
public:
	static std::shared_ptr<JsonItemW> Parse(const std::wstring& source_file, const int& io_type = std::ios::binary, const stdex::cvt::Encode& type = stdex::cvt::Encode::UniCode);
	static std::vector<std::shared_ptr<JsonItemW>> _Parse(const std::shared_ptr<JsonItemW>& parent, const wchar_t opening, const std::wstring& source, int32& indexer);
};

template<stdex::cvt::Encode _Encode>
class JsonFileBase
{
public:
	static constexpr stdex::cvt::Encode encode_type = _Encode;

	using json_parser = std::conditional_t< stdex::cvt::Encode::UniCode == _Encode, JsonParserW, JsonParser>;

	using json_func_type = decltype(&json_parser::Parse);
	using json_item_type = typename stdex::function_traits<json_func_type>::ret_type;

	using str_type = std::conditional_t< stdex::cvt::Encode::UniCode == _Encode, std::wstring, std::string>;

public:
	JsonFileBase()
		: _io_type(-1)
	{}

public:
	void WriteFile()
	{

	}

	void Reload()
	{
		DYNAMIC_ASSERT(_source_file.size() > 0 && _io_type != -1, L"cannot open json file. invalid option. path :: %s, io_type : %d", _source_file, _io_type);
		_header = json_parser::Parse(_source_file, _io_type, encode_type);
	}

	void Open(const str_type& source_file, const int& io_type = std::ios::binary)
	{
		_source_file = source_file;
		_io_type = io_type;
		_header = json_parser::Parse(source_file, io_type, encode_type);
	}

public:
	auto& header() { return _header; }



private:
	json_item_type _header;
	int _io_type;
	str_type _source_file;
};

using JsonFile = JsonFileBase<stdex::cvt::Encode::ASCII>;
using JsonFileW = JsonFileBase<stdex::cvt::Encode::UniCode>;
template<stdex::cvt::Encode _Encode>
using JsonFileEx = JsonFileBase<_Encode>;