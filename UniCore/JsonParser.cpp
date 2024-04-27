#include "stdafx.h"
#include "JsonParser.h"

#include "StringTools.h"

std::string _StringOpener(const std::string& source, int32& indexer)
{
	std::string result;
	for (int32 i = indexer; i < source.size(); i++)
	{
		if (source[i] == '\"')
		{
			if (i != 0 && source[i - 1] == '\\')
			{
				result[result.size() - 1] = '\"';
				continue;
			}

			indexer = ++i;
			return result;
		}

		result += source[i];
	}

	return  result;
}

std::string Rewind(const std::string& source, const int32& indexer)
{
	std::string result;
	for (int32 i = indexer; i >= 0; i--)
	{
		if (source[i] == ':')
		{
			return StringTools::reverse(StringTools::trim(result));
		}

		result += source[i];
	}

	return StringTools::reverse(StringTools::trim(result));
}




std::shared_ptr<JsonItem> JsonParser::Parse(const std::string& source_file)
{
	std::fstream reader;
	reader.open(source_file);

	std::string line_buffer;
	std::string json_string = "";
	while (std::getline(reader, line_buffer))
	{
		json_string += line_buffer + "\n";
	}

	std::shared_ptr<JsonItem> head;
	int32 index = 0;
	auto result = _Parse(head, ' ', json_string, index);
	if (result.size() == 1)
	{
		head = result[0];
	}
	else
	{
		for (auto item : result)
		{
			head->AddPair(item);
		}
	}

	//auto test = (*head)["default_selling_price"];
	return head;
}

std::vector<std::shared_ptr<JsonItem>> JsonParser::_Parse(const std::shared_ptr<JsonItem>& parent, const char opening, const std::string& source, int32& indexer)
{
	std::shared_ptr<JsonItem> item = J_MakeShared<JsonItem>(parent);
	std::vector<std::shared_ptr<JsonItem>> result;
	result.push_back(item);

	char close;
	switch (opening)
	{
	case '{':
		close = '}';
		break;
	case '[':
		close = ']';
		break;
	default:
		close = ' ';
		break;
	}

	for (int32 i = indexer; i < source.size(); i++)
	{
		if (source[i] == close && close != ' ')
		{
			if (item->_key != "" && item->_has_value == false)
			{
				auto value = Rewind(source, i - 1);
				if (value == "")
					CRASH("");

				item->_int_value = std::stoi(value);
			}

			indexer = ++i;
			return result;
		}

		switch (source[i])
		{
		case '\"':
		{
			if (item->_key == "")
				item->_key = _StringOpener(source, ++i);
			else
				item->_string_value = _StringOpener(source, ++i);

			break;
		}
		case '{':
		{
			if (item->_key == "" && close != ' ')
				CRASH("");

			auto childs = _Parse(item, '{', source, ++i);
			for (auto child : childs)
			{
				item->AddPair(child);
			}

			break;
		}
		case '[':
		{
			if (item->_key == "")
				CRASH("");

			auto childs = _Parse(item, '[', source, ++i);
			for (auto child : childs)
			{
				item->AddList(child);
			}

			break;
		}
		case ',':
		{
			if (item->_key == "")
				CRASH("");

			auto value = Rewind(source, i - 1);
			if(value == "")
				CRASH("");

			//지금은 int만 있음
			item->_int_value = std::stoi(value);
			item->_has_value = true;

			item = J_MakeShared<JsonItem>(parent);
			result.push_back(item);
			continue;

			break;
		}

		case '/':
		{
			if (i != 0 && source[i - 1] == '/')
			{
				while (i < source.size() - 1 && source[i] != '\n')
				{
					i++;
				}
				i++;
				continue;
			}

			break;
		}
		}
	}

	if (item->_key != "" && item->_has_value == false)
	{
		auto value = Rewind(source, source.size() - 1);
		if (value == "")
			CRASH("");

		item->_int_value = std::stoi(value);
	}

	indexer = source.size();
	return result;
}