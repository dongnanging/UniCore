#include "stdafx.h"
#include "JsonParser.h"

#include "StringTools.h"

template<typename _StrType>
_StrType _StringOpener(const _StrType& source, std::size_t& indexer)
{
	_StrType result;
	for (std::size_t i = indexer; i < source.size(); i++)
	{
		if (source[i] == stdex::str_elem<_StrType>::elem('\"'))
		{
			if (i != 0 && source[i - 1] == stdex::str_elem<_StrType>::elem('\\'))
			{
				result[result.size() - 1] = stdex::str_elem<_StrType>::elem('\"');
				continue;
			}

			indexer = i;
			return result;
		}

		result += source[i];
	}

	return result;
}

template<typename _StrType>
_StrType Rewind(const _StrType& source, const std::size_t& indexer)
{
	_StrType result;
	for (std::size_t i = indexer; i >= 0; i--)
	{
		if (source[i] == stdex::str_elem<_StrType>::elem(':'))
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

template<typename _JsonItemType>
std::vector<std::shared_ptr<_JsonItemType>> _SealedParser(const std::shared_ptr<_JsonItemType>& parent, const char opening, const typename _JsonItemType::string_type& source, std::size_t& indexer)
{
	using converter = stdex::str_elem<typename _JsonItemType::string_type>;

	std::shared_ptr<_JsonItemType> item = stdex::pmake_shared<_JsonItemType>(parent);
	std::vector<std::shared_ptr<_JsonItemType>> result;
	result.push_back(item);

	typename _JsonItemType::string_type::value_type close;
	switch (opening)
	{
	case converter::elem('{'):
		close = converter::elem('}');
		break;
	case converter::elem('['):
		close = converter::elem(']');
		break;
	default:
		close = converter::elem(' ');
		break;
	}

	for (std::size_t i = indexer; i < std::size(source); i++)
	{
		// �ݱ� (�ֻ��� �� ���, ���� key, value���� �䱸���� ����)
		if (source[i] == close && close != converter::elem(' '))
		{
			// key�� �����ϰ�, value�� �������
			if (std::size(item->key()) > 0 && !item->has_value() &&
				item->type() != JOType::ARRAY && item->type() != JOType::SUBJSON)
			{
				// ':' ��ū���� �ǰ����鼭, : ��ū ������ ���� value�� ���� (���ڿ��� �ƴ� value)
				auto value = Rewind(source, i - 1);
				if (std::size(value) == 0)
				{
					DYNAMIC_ASSERT(false, "json parse failed! check json file");
					indexer = std::size(source);
				}

				item->set_value(value);
			}

			indexer = i;
			return result;
		}

		switch (source[i])
		{
			//��õ� ���ڿ� => ���ڿ� �ȿ� ������� �Ʒ� �ٸ� case�� ������ �ʵ���, ���ڿ� ���Ḧ �ٽ� ���� �� ���� ��� ����
		case converter::elem('\"'):
		{
			//���� ���� ��ū�� '\\' �ϰ��, �ش� ��ū�� \" ��ū���� ����
			if (i > 0 && i < std::size(source) - 1 && source[i] == converter::elem('\\'))
				break;

			// key�� ���ٸ� key�� ����
			if (std::size(item->key()) == 0)
				item->set_key(_StringOpener(source, ++i));
			// key�� ������ value�� ���� ���, value�� ����
			else if (!item->has_value())
				item->set_value(_StringOpener(source, ++i));
			else
				DYNAMIC_ASSERT(false, "json parse failed! check json file");

			break;
		}

		//���� �������� ������ ���
		case converter::elem('{'):
		{
			item->set_type(JOType::SUBJSON);

			auto childs = _SealedParser(item, converter::elem('{'), source, ++i);
			for (auto child : childs)
			{
				item->AddPair(child);
			}

			break;
		}
		//����Ʈ
		case converter::elem('['):
		{
			item->set_type(JOType::ARRAY);

			auto childs = _SealedParser(item, converter::elem('['), source, ++i);
			for (auto child : childs)
				item->AddList(child);

			break;
		}
		// sep. ���� ���ҷ� �Ѿ
		case converter::elem(','):
		{
			//Ű�� �ִµ� ���� ����
			if (std::size(item->key()) > 0 && !item->has_value() &&
				item->type() != JOType::ARRAY && item->type() != JOType::SUBJSON)
			{
				auto value = Rewind(source, i - 1);
				if (std::size(value) == 0)
					CRASH("");

				item->set_value(value);
			}

			item = stdex::pmake_shared<_JsonItemType>(parent);
			result.push_back(item);
			continue;

			break;
		}

		// �ּ����� ���� => json�� ���� ���ص� �� �����Ұž� ^^
		case converter::elem('/'):
		{
			if (i != 0 && source[i - 1] == converter::elem('/'))
			{
				while (i < source.size() - 1 && source[i] != converter::elem('\n'))
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

	if(indexer >= std::size(source))

	// �Ʒ� �׸���� ������� ������ ��ü������ ���� �� �̻��� ����
	if (item->type() == JOType::ARRAY || item->type() == JOType::SUBJSON)
		DYNAMIC_ASSERT(false, "json parse failed! check json file");

	// ���� �Ʒ��δ� �ܼ� key - value 
	// key�� �����ϰ�, value�� ������� �ݴ� '\"' �� ���� => value�� string�� �ƴ� ���
	if (std::size(item->key()) != 0 && item->has_value() == false)
	{
		auto value = Rewind(source, source.size() - 1);
		if (std::size(value) == 0)
		{
			DYNAMIC_ASSERT(false, "json parse failed! check json file");
			indexer = source.size();
		}
			
		item->set_value(value);
	}

	return result;
}


template<typename _StrType>
struct json_stream_trait
{
	using istream_type = std::conditional_t<stdex::is_convertible_ctype_char_v<_StrType>, std::ifstream, std::wifstream>;
	using osteram_type = std::conditional_t<stdex::is_convertible_ctype_char_v<_StrType>, std::ofstream, std::wofstream>;

	static constexpr istream_type open_istream(const _StrType& source_file, const int& io_type, const stdex::cvt::Encode& type)
	{
		istream_type stream;
		
		if constexpr (stdex::is_same_v<istream_type, std::wifstream>)
		{
			stream.open(source_file, io_type);
			stdex::imbue(stream, type);
		}
		else
		{
			stream.open(source_file);
		}

		return stream;
	}

	static constexpr osteram_type open_ostream(const _StrType& source_file, const int& io_type, const stdex::cvt::Encode& type)
	{
		osteram_type stream;

		if constexpr (stdex::is_same_v<osteram_type, std::wofstream>)
		{
			stream.open(source_file, io_type);
			stdex::imbue(stream, type);
		}
		else
		{
			stream.open(source_file);
		}

		return stream;
	}

	static constexpr void close(istream_type& stream) { stream.close(); }
	static constexpr void close(osteram_type& stream) { stream.close(); }
};

template<typename _JsonItemType, typename _StrType>
std::shared_ptr<_JsonItemType> SealedParser(const _StrType& source_file, const int& io_type = 0, const stdex::cvt::Encode& type = stdex::cvt::Encode::None)
{
	static_assert(stdex::is_same_v<_JsonItemType, JsonItem> || stdex::is_same_v<_JsonItemType, JsonItemW>, "invalid json item type!");
	static_assert(stdex::is_same_v<typename _JsonItemType::string_type, _StrType>, "string type mismatch");

	auto reader = json_stream_trait<typename _JsonItemType::string_type>::open_istream(source_file, io_type, type);

	typename _JsonItemType::string_type line_buffer;
	typename _JsonItemType::string_type json_string;
	while (std::getline(reader, line_buffer))
	{
		json_string += line_buffer + stdex::str_elem<typename _JsonItemType::string_type>::elem('\n');
	}

	auto head = stdex::pmake_shared<_JsonItemType>(nullptr);
	//head->set_type(JOType::HEADER);

	typename decltype(json_string)::size_type index = 0;
	auto result = _SealedParser(head, stdex::str_elem<typename _JsonItemType::string_type>::elem(' '), json_string, index);
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

	json_stream_trait<typename _JsonItemType::string_type>::close(reader);
	//auto test = (*head)["default_selling_price"];
	return head;
}


std::shared_ptr<JsonItem> JsonParser::Parse(const std::string& source_file, const int& io_type, const stdex::cvt::Encode& type)
{
	return SealedParser<JsonItem>(source_file, io_type, type);
}

std::shared_ptr<JsonItemW> JsonParserW::Parse(const std::wstring& source_file, const int& io_type, const stdex::cvt::Encode& type)
{
	return SealedParser<JsonItemW>(source_file, io_type, type);
}

template<typename _StrType, typename _JsonItemType>
_StrType _SealedToString(_JsonItemType& item, const int32& tcount)
{
	_StrType tab;
	for (int32 i = 0; i < tcount; i++)
		tab += stdex::str_elem<_StrType>::elem('\t');


	_StrType key_string;
	if (std::size(item->key()) > 0)
		key_string = stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("\"%s\" : "), item->key());

	switch(item->type())
	{
	case JOType::STRING:
		return stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("%s%s\"%s\""), tab, key_string, item->GetString());
	case JOType::WSTRING:
		return stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("%s%s\"%s\""), tab, key_string, item->GetWstring());
	case JOType::INTEGER:
		return stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("%s%s%s"), tab, key_string, std::to_string(item->GetInt32()));
	case JOType::LONGLONG:
		return stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("%s%s%s"), tab, key_string, std::to_string(item->GetInt64()));
	case JOType::FLOAT:
		return stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("%s%s%s"), tab, key_string, std::to_string(item->GetFloat()));
	case JOType::DOUBLE:
		return stdex::sprintf_buffer(stdex::str_elem<_StrType>::ascii("%s%s%s"), tab, key_string, std::to_string(item->GetDouble()));
	case JOType::SUBJSON:
	{
		_StrType result_sub;
		for (auto& itr : item->child_view().view())
		{
			if (std::size(result_sub) > 0)
				result_sub += stdex::str_elem< _StrType>::ascii(",\n");

			result_sub += _SealedToString<_StrType>(itr.second, tcount + 1);
		}

		return stdex::sprintf_buffer(stdex::str_elem< _StrType>::ascii("%s%s{\n%s\n%s}"), tab, key_string, result_sub, tab);
	}
	case JOType::ARRAY:
	{
		_StrType result_arr;
		for (auto& itr : item->list_view().view())
		{
			if (std::size(result_arr) > 0)
				result_arr += stdex::str_elem< _StrType>::ascii(",\n");

			result_arr += _SealedToString<_StrType>(itr, tcount + 1);
		}

		return stdex::sprintf_buffer(stdex::str_elem< _StrType>::ascii("%s%s[\n%s\n%s]"), tab, key_string, result_arr, tab);
	}
	}
}

template<typename _StrType, typename _JsonItemType>
_StrType SealedToString(_JsonItemType& item)
{
	return _SealedToString<_StrType>(item, 0);
}

std::string JsonParser::to_string(std::shared_ptr<JsonItem>& header)
{
	return SealedToString<std::string>(header);
}

std::wstring JsonParserW::to_string(std::shared_ptr<JsonItemW>& header)
{
	return SealedToString<std::wstring>(header);
}

template<typename _StrType>
bool SealedWrite(const _StrType& target_path, const _StrType& raw_string, const int& io_type, const stdex::cvt::Encode& type)
{
	auto writer = json_stream_trait<_StrType>::open_ostream(target_path, io_type, type);
	if (!writer.is_open())
		return false;

	writer << raw_string;
	//writer.write(stdex::sprintf_traits<_StrType>::safe_type(raw_string), std::size(raw_string));
	writer.close();
	return true;
}


bool JsonParser::Write(const std::string& target_path, std::shared_ptr<JsonItem>& header, const int& io_type, const stdex::cvt::Encode& type)
{
	if (!SealedWrite(target_path, to_string(header), io_type, type))
	{
		DYNAMIC_ASSERT(false, "json file write failed! path: %s", target_path)
		return false;
	}
		

	return true;
}

bool JsonParserW::Write(const std::wstring& target_path, std::shared_ptr<JsonItemW>& header, const int& io_type, const stdex::cvt::Encode& type)
{
	if (!SealedWrite(target_path, to_string(header), io_type, type))
	{
		DYNAMIC_ASSERT(false, "json file write failed! path: %s", target_path)
			return false;
	}

	return true;
}