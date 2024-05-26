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
		// �ݱ� (�ֻ��� �� ���, ���� key, value���� �䱸���� ����)
		if (source[i] == close && close != ' ')
		{
			// key�� �����ϰ�, value�� �������
			if (item->_key != "" && item->_has_value == false)
			{
				// ':' ��ū���� �ǰ����鼭, : ��ū ������ ���� value�� ���� (���ڿ��� �ƴ� value)
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
		//��õ� ���ڿ� => ���ڿ� �ȿ� ������� �Ʒ� �ٸ� case�� ������ �ʵ���, ���ڿ� ���Ḧ �ٽ� ���� �� ���� ��� ����
		case '\"':
		{
			//���� ���� ��ū�� '\\' �ϰ��, �ش� ��ū�� \" ��ū���� ����
			if (i > 0 && i < source.size() - 1 && source[i] == '\\')
				break;

			// key�� ���ٸ� key�� ����
			if (item->_key == "")
				item->_key = _StringOpener(source, ++i);
			// key�� ������ value�� ���� ���, value�� ����
			else
				item->set_value(_StringOpener(source, ++i));

			break;
		}

		//���� �������� ������ ���
		case '{':
		{
			// key�� ���µ� ���� �������� ������ ���
			if (item->_key == "" && close != ' ')
				CRASH("");

			auto childs = _Parse(item, '{', source, ++i);
			for (auto child : childs)
			{
				item->AddPair(child);
			}

			break;
		}
		//����Ʈ
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
		// sep. ���� ���ҷ� �Ѿ
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

		// �ּ����� ���� => json�� ���� ���ص� �� �����Ұž� ^^
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

	

	// key�� �����ϰ�, value�� �������
	if (item->_key != "" && item->_has_value == false)
	{
		//��� for���� ��������, �ݱ��� �ʰ�, ���� ���� ���.
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
		// �ݱ� (�ֻ��� �� ���, ���� key, value���� �䱸���� ����)
		if (source[i] == close && close != L' ')
		{
			// key�� �����ϰ�, value�� �������
			if (item->_key.size() > 0 && item->_has_value == false)
			{
				// ':' ��ū���� �ǰ����鼭, : ��ū ������ ���� value�� ���� (���ڿ��� �ƴ� value)
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
			//��õ� ���ڿ� => ���ڿ� �ȿ� ������� �Ʒ� �ٸ� case�� ������ �ʵ���, ���ڿ� ���Ḧ �ٽ� ���� �� ���� ��� ����
		case L'\"':
		{
			//���� ���� ��ū�� '\\' �ϰ��, �ش� ��ū�� \" ��ū���� ����
			if (i > 0 && i < source.size() - 1 && source[i] == L'\\')
				break;

			// key�� ���ٸ� key�� ����
			if (item->_key.size() == 0)
				item->_key = _StringOpenerW(source, ++i);
			// key�� ������ value�� ���� ���, value�� ����
			else
				item->set_value(_StringOpenerW(source, ++i));

			break;
		}

		//���� �������� ������ ���
		case L'{':
		{
			// key�� ���µ� ���� �������� ������ ���
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
		//����Ʈ
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
		// sep. ���� ���ҷ� �Ѿ
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

		// �ּ����� ���� => json�� ���� ���ص� �� �����Ұž� ^^
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

	// key�� �����ϰ�, value�� �������
	if (item->_key.size() > 0 && item->_has_value == false)
	{
		//��� for���� ��������, �ݱ��� �ʰ�, ���� ���� ���.
		DYNAMIC_ASSERT(false, "json parse failed! check json file");

		auto value = RewindW(source, source.size() - 1);
		if (value == L"")
			CRASH("");

		item->set_value(value);
	}

	indexer = source.size();
	return result;
}