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
	bool has_value() const { return _has_value; }
	const auto& type() const { return _value_type; }

	const auto GetInt32() const { if (_value_type != JOType::INTEGER) { DYNAMIC_ASSERT(false, "invalid json value_type") return 0; }							return *reinterpret_cast<int*>(_value_item.get()); }
	const auto GetInt64() const { if (_value_type != JOType::LONGLONG) { DYNAMIC_ASSERT(false, "invalid json value_type") return static_cast<long long>(0); }	return *reinterpret_cast<long long*>(_value_item.get()); }
	const auto GetFloat() const { if (_value_type != JOType::FLOAT) { DYNAMIC_ASSERT(false, "invalid json value_type") return 0.0f; }							return *reinterpret_cast<float*>(_value_item.get()); }
	const auto GetDouble() const { if (_value_type != JOType::DOUBLE) { DYNAMIC_ASSERT(false, "invalid json value_type") return 0.0; }						return *reinterpret_cast<double*>(_value_item.get()); }
	const auto GetString() const { if (_value_type != JOType::STRING) { DYNAMIC_ASSERT(false, "invalid json value_type") return std::string(); }				return *reinterpret_cast<std::string*>(_value_item.get()); }
	const auto GetWstring() const { if (_value_type != JOType::WSTRING) { DYNAMIC_ASSERT(false, "invalid json value_type") return std::wstring(); }			return *reinterpret_cast<std::wstring*>(_value_item.get()); }

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


	void set_type(JOType type)
	{
		_value_type = type;
	}

protected:
	template<typename _Value>
	void _set_value(_Value&& val)
	{
		if constexpr (stdex::is_convertible_ctype_char_v<stdex::pure_type_t<_Value>>)
		{
			_value_item = std::static_pointer_cast<void>(J_MakeShared<std::string>(std::forward<_Value>(val)));
			_value_type = JOType::STRING;
		}
		else if constexpr (stdex::is_convertible_ctype_wchar_v<stdex::pure_type_t<_Value>>)
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
				stdex::is_convertible_ctype_char_v<stdex::pure_type_t<_Value>>	||
				stdex::is_convertible_ctype_wchar_v<stdex::pure_type_t<_Value>>	||
				stdex::is_same_v<int32, _Value>					||
				stdex::is_same_v<int64, _Value>					||
				stdex::is_same_v<float, _Value>					||
				stdex::is_same_v<double, _Value>
				, "set value failed! invalid value type");
		}

		_has_value = true;
	}

	

protected:
	std::vector<std::shared_ptr<item_type>> _arrays;

	// 값을 가집니까
	bool _has_value;
	// value 값 => 소멸자 구현 안하려고 포인터 대신 사용
	std::shared_ptr<void> _value_item;
	// value type
	JOType _value_type;

	std::shared_ptr<item_type> _parent;
};

template<typename _StrType, 
	typename = std::enable_if_t<stdex::is_stl_string_v<_StrType>>>	//stl 스트링이 아니면 허가 x
class JsonDivision : public JsonItemBase<JsonDivision<_StrType>>
{
	friend JsonParser;
	friend JsonParserW;

public:
	using string_type = _StrType;

protected:
	using Base = JsonItemBase<JsonDivision<_StrType>>;
	using item_type = typename Base::item_type;
	using enable_shared = Base::enable_shared;

public:
	JsonDivision(const std::shared_ptr<item_type>& parent)
	{
		Base::_parent = parent;
		Base::_has_value = false;
		Base::_value_type = JOType::NONE;
	}

public:
	const auto& key() const { return _key; }
	

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

public:
	bool set_value(_StrType val)
	{
		
		auto digit = 0;
		auto ndigit = 0;
		for (auto& ch : val)
		{
			if (stdex::isdigit(ch))
				digit++;
		}

		ndigit = std::size(val) - digit;

		try
		{
			if (ndigit == 2)
			{
				// 소수점 표현식이 아님 => 전부 정수형으로 표시
				auto llcount = std::count(val.begin(), val.end(), stdex::str_elem<_StrType>::elem('l'));
				if (llcount > 1)
				{
					Base::_set_value(std::stoll(val));
					return true;
				}
			}

			if (ndigit == 1)
			{
				auto fcount = std::count(val.begin(), val.end(), stdex::str_elem<_StrType>::elem('f'));
				auto dotcount = std::count(val.begin(), val.end(), stdex::str_elem<_StrType>::elem('.'));

				// 플롯
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
			}

			if (ndigit == 0)
			{
				// long long이 아니면 그냥 int로 표기
				Base::_set_value(std::stoi(val));
				return true;
			}


			//일반 문자열
			Base::_set_value(val);
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

	void set_key(const _StrType& key)
	{
		_key = key;
	}

protected:
	// key 값
	_StrType _key;
	std::unordered_map<_StrType, std::shared_ptr<item_type>> _childs;
};

using JsonItem = JsonDivision<std::string>;
using JsonItemW = JsonDivision<std::wstring>;

class JsonParser
{
public:
	static std::shared_ptr<JsonItem> Parse(const std::string& source_file, const int& io_type = std::ios::binary, const stdex::cvt::Encode& type = stdex::cvt::Encode::None);
	static std::string to_string(std::shared_ptr<JsonItem>& header);
	static bool Write(const std::string& target_path, std::shared_ptr<JsonItem>& header, const int& io_type = std::ios::binary, const stdex::cvt::Encode& type = stdex::cvt::Encode::None);
};

class JsonParserW
{
public:
	static std::shared_ptr<JsonItemW> Parse(const std::wstring& source_file, const int& io_type = std::ios::binary, const stdex::cvt::Encode& type = stdex::cvt::Encode::UniCode);
	static std::wstring to_string(std::shared_ptr<JsonItemW>& header);
	static bool Write(const std::wstring& target_path, std::shared_ptr<JsonItemW>& header, const int& io_type = std::ios::binary, const stdex::cvt::Encode& type = stdex::cvt::Encode::None);
};


template<stdex::cvt::Encode _Encode>
class JsonFileBase
{
public:
	static constexpr stdex::cvt::Encode encode_type = _Encode;

	using json_parser = std::conditional_t< stdex::cvt::Encode::UniCode == _Encode, JsonParserW, JsonParser>;

	using json_func_type = decltype(&json_parser::Parse);
	using json_item_type = typename stdex::function_traits<json_func_type>::ret_type;

	using string_type = typename stdex::pure_type_t<typename stdex::unwrap_shared_t<json_item_type>::string_type>;	//만약 shared_ptr로 감싸졌으면 풀어주기

public:
	JsonFileBase()
		: _io_type(-1), _is_open(false)
	{}

public:
	decltype(auto) to_string()
	{
		if (!_is_open.load(std::memory_order_acquire))
			return string_type{};

		return json_parser::to_string(_header);
	}

	bool WriteFile()
	{
		if (!_is_open.load(std::memory_order_acquire))
			return false;

		return json_parser::Write(_source_file, _header);
	}

	void Reload()
	{
		if (!_is_open.load(std::memory_order_acquire))
			return;
		DYNAMIC_ASSERT(_source_file.size() > 0 && _io_type != -1, L"cannot open json file. invalid option. path :: %s, io_type : %d", _source_file, _io_type);
		_header = json_parser::Parse(_source_file, _io_type, encode_type);
	}

	void Open(const string_type& source_file, const int& io_type = std::ios::binary)
	{
		_source_file = source_file;
		_io_type = io_type;
		_header = json_parser::Parse(source_file, io_type, encode_type);

		_is_open.store(true, std::memory_order_release);
	}

public:
	auto& header() { return _header; }



private:
	json_item_type _header;
	int _io_type;
	string_type _source_file;

	std::atomic<bool> _is_open;
};

using JsonFile = JsonFileBase<stdex::cvt::Encode::ASCII>;
using JsonFileW = JsonFileBase<stdex::cvt::Encode::UniCode>;
template<stdex::cvt::Encode _Encode>
using JsonFileEx = JsonFileBase<_Encode>;