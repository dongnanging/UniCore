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
		// 닫김 (최상위 일 경우, 굳이 key, value값을 요구하지 않음)
		if (source[i] == close && close != ' ')
		{
			// key가 존재하고, value가 없을경우
			if (item->_key != "" && item->_has_value == false)
			{
				// ':' 토큰까지 되감으면서, : 토큰 이후의 값을 value로 지정 (문자열은 아닌 value)
				auto value = Rewind(source, i - 1);
				if (value == "")
					CRASH("");

				item->set_value(value);
			}

			indexer = ++i;
			return result;
		}

		switch (source[i])
		{
		//명시된 문자열 => 문자열 안에 내용들이 아래 다른 case에 잡히지 않도록, 문자열 종료를 다시 만날 때 까지 계속 진행
		case '\"':
		{
			//만약 이전 토큰이 '\\' 일경우, 해당 토큰은 \" 토큰으로 간주
			if (i > 0 && i < source.size() - 1 && source[i] == '\\')
				break;

			// key가 없다면 key로 설정
			if (item->_key == "")
				item->_key = _StringOpener(source, ++i);
			// key는 있지만 value는 없는 경우, value로 설정
			else
				item->set_value(_StringOpener(source, ++i));

			break;
		}

		//하위 아이템을 가지는 경우
		case '{':
		{
			// key가 없는데 하위 아이템을 선언한 경우
			if (item->_key == "" && close != ' ')
				CRASH("");

			auto childs = _Parse(item, '{', source, ++i);
			for (auto child : childs)
			{
				item->AddPair(child);
			}

			break;
		}
		//리스트
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
		// sep. 다음 원소로 넘어감
		case ',':
		{
			if (item->_key == "")
				CRASH("");

			auto value = Rewind(source, i - 1);
			if(value == "")
				CRASH("");

			item->set_value(value);

			item = J_MakeShared<JsonItem>(parent);
			result.push_back(item);
			continue;

			break;
		}

		// 주석인지 여부 => json이 지원 안해도 난 지원할거야 ^^
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

	

	// key가 존재하고, value가 없을경우
	if (item->_key != "" && item->_has_value == false)
	{
		//모든 for문을 마쳤지만, 닫기지 않고, 값이 없는 경우.
		DYNAMIC_ASSERT(false, "json parse failed! check json file");

		auto value = Rewind(source, source.size() - 1);
		if (value == "")
			CRASH("");

		item->set_value(value);
	}

	indexer = source.size();
	return result;
}


std::wstring _StringOpenerW(const std::wstring& source, int32& indexer)
{
	std::wstring result;
	for (int32 i = indexer; i < source.size(); i++)
	{
		if (source[i] == L'\"')
		{
			if (i != 0 && source[i - 1] == L'\\')
			{
				result[result.size() - 1] = L'\"';
				continue;
			}

			indexer = ++i;
			return result;
		}

		result += source[i];
	}

	return  result;
}

std::wstring RewindW(const std::wstring& source, const int32& indexer)
{
	std::wstring result;
	for (int32 i = indexer; i >= 0; i--)
	{
		if (source[i] == ':')
		{
			stdex::trim(result);
			stdex::reverse(result);
			return result;
		}

		result += source[i];
	}

	stdex::trim(result);
	stdex::reverse(result);
	return result;
}

std::shared_ptr<JsonItemW> JsonParserW::Parse(const std::wstring& source_file, const int& io_type, const stdex::cvt::Encode& type)
{
	std::wfstream reader;
	reader.open(source_file, io_type);
	stdex::imbue(reader, type);

	std::wstring line_buffer;
	std::wstring json_string;
	while (std::getline(reader, line_buffer))
	{
		json_string += line_buffer + L"\n";
	}

	std::shared_ptr<JsonItemW> head;
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

std::vector<std::shared_ptr<JsonItemW>> JsonParserW::_Parse(const std::shared_ptr<JsonItemW>& parent, const wchar_t opening, const std::wstring& source, int32& indexer)
{
	std::shared_ptr<JsonItemW> item = J_MakeShared<JsonItemW>(parent);
	std::vector<std::shared_ptr<JsonItemW>> result;
	result.push_back(item);

	wchar_t close;
	switch (opening)
	{
	case L'{':
		close = L'}';
		break;
	case L'[':
		close = L']';
		break;
	default:
		close = L' ';
		break;
	}

	for (int32 i = indexer; i < source.size(); i++)
	{
		// 닫김 (최상위 일 경우, 굳이 key, value값을 요구하지 않음)
		if (source[i] == close && close != L' ')
		{
			// key가 존재하고, value가 없을경우
			if (item->_key.size() > 0 && item->_has_value == false)
			{
				// ':' 토큰까지 되감으면서, : 토큰 이후의 값을 value로 지정 (문자열은 아닌 value)
				auto value = RewindW(source, i - 1);
				if (value == L"")
					CRASH("");

				item->set_value(value);
			}

			indexer = ++i;
			return result;
		}

		switch (source[i])
		{
			//명시된 문자열 => 문자열 안에 내용들이 아래 다른 case에 잡히지 않도록, 문자열 종료를 다시 만날 때 까지 계속 진행
		case L'\"':
		{
			//만약 이전 토큰이 '\\' 일경우, 해당 토큰은 \" 토큰으로 간주
			if (i > 0 && i < source.size() - 1 && source[i] == L'\\')
				break;

			// key가 없다면 key로 설정
			if (item->_key.size() == 0)
				item->_key = _StringOpenerW(source, ++i);
			// key는 있지만 value는 없는 경우, value로 설정
			else
				item->set_value(_StringOpenerW(source, ++i));

			break;
		}

		//하위 아이템을 가지는 경우
		case L'{':
		{
			// key가 없는데 하위 아이템을 선언한 경우
			if (item->_key.size() == 0 && close != L' ')
				CRASH("");

			auto childs = _Parse(item, L'{', source, ++i);
			for (auto child : childs)
			{
				item->AddPair(child);
			}

			item->_value_type = JOType::SUBJSON;
			break;
		}
		//리스트
		case L'[':
		{
			if (item->_key.size() == 0)
				CRASH("");

			auto childs = _Parse(item, L'[', source, ++i);
			for (auto child : childs)
			{
				item->AddList(child);
			}

			item->_value_type = JOType::ARRAY;
			break;
		}
		// sep. 다음 원소로 넘어감
		case L',':
		{
			if (item->_key.size() == 0)
				CRASH("");

			auto value = RewindW(source, i - 1);
			if (value == L"")
				CRASH("");

			item->set_value(value);

			item = J_MakeShared<JsonItemW>(parent);
			result.push_back(item);
			continue;

			break;
		}

		// 주석인지 여부 => json이 지원 안해도 난 지원할거야 ^^
		case L'/':
		{
			if (i != 0 && source[i - 1] == L'/')
			{
				while (i < source.size() - 1 && source[i] != L'\n')
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

	// key가 존재하고, value가 없을경우
	if (item->_key.size() > 0 && item->_has_value == false)
	{
		//모든 for문을 마쳤지만, 닫기지 않고, 값이 없는 경우.
		DYNAMIC_ASSERT(false, "json parse failed! check json file");

		auto value = RewindW(source, source.size() - 1);
		if (value == L"")
			CRASH("");

		item->set_value(value);
	}

	indexer = source.size();
	return result;
}