#pragma once
#include<map>
#include<unordered_map>
#include<fstream>
#include<stack>

class JsonParser;

enum class JOType
{
	HEADER = 1,
	ARRAY = 2,
	STRING = 3,
	INTEGER = 4,

};

class JsonItem : public std::enable_shared_from_this<JsonItem>
{
	friend JsonParser;

public:
	JsonItem(const std::shared_ptr<JsonItem>& parent)
		: _parent(parent)
	{}

	bool has_value() { return _has_value; }
	auto& GetInt() { return _int_value; }
	auto& GetString() { return _string_value; }

	void AddPair(std::shared_ptr<JsonItem> item) { _childs.insert({ item->_key, item }); }
	void AddList(std::shared_ptr<JsonItem> item) { _arrays.push_back(item); }

	std::shared_ptr<JsonItem> operator [] (const int32& idx) { if (idx > _arrays.size()) return nullptr;  return _arrays[idx]; }
	std::shared_ptr<JsonItem> operator [] (const std::string& key) { if (_childs.find(key) == _childs.end()) return nullptr; return _childs[key]; }

private:
	bool _has_value = false;

	std::string _key;

	std::string _string_value;
	int32 _int_value = 0;


private:
	std::shared_ptr<JsonItem> _parent;

	
	//std::shared_ptr<JsonItem> operator [] (const char* key) { if (_childs.find(std::string(key)) == _childs.end()) return nullptr; return _childs[key]; }

	std::vector<std::shared_ptr<JsonItem>> _arrays;
	std::unordered_map<std::string, std::shared_ptr<JsonItem>> _childs;
};

class JsonParser
{
public:
	static std::shared_ptr<JsonItem> Parse(const std::string& source_file);
	static std::vector<std::shared_ptr<JsonItem>> _Parse(const std::shared_ptr<JsonItem>& parent, const char opening, const std::string& source, int32& indexer);
};