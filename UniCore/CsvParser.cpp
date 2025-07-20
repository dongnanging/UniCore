#include "stdafx.h"
#include "CsvParser.h"

#include "stdafx.h"
#include "StringTools.h"

#include <fstream>

std::vector<std::string> _parse_row(const std::string& row)
{
	std::vector<std::string> result;
	std::string now;

	bool skip_sep = false;
	for (auto& chr : row)
	{
		switch (chr)
		{
		case ',':
			if (skip_sep)
			{
				now += chr;
				break;
			}

			result.push_back(now);
			now.clear();
			break;

		case '\"':
			skip_sep = !skip_sep;
			break;

		default:
			now += chr;
		}
	}

	DYNAMIC_ASSERT(skip_sep == false, "file parse failed");
	if (now.size() > 0)
		result.push_back(now);

	return result;
}


std::vector<std::vector<std::string>> CsvParser::ReadCsv(std::wstring path, bool ignore_head)
{
	std::vector<std::vector<std::string>> result;

	return result;
}

std::vector<std::string> CsvParser::row_from_string(const std::string& raw)
{
	return _parse_row(raw);
}


std::vector<std::vector<std::string>> CsvParser::ReadCsv(std::string path, bool ignore_head)
{
	std::vector<std::vector<std::string>> result;

	std::string line_buffer;
	std::fstream reader;

	reader.open(path, std::ios::in);
	if (!reader.is_open())
	{
		DEBUG_ASSERT(false, "file open failed. path: %s", path);
		return result;
	}

	if (ignore_head && reader.eof() == false)
	{
		//그냥 한 줄 읽고 넘김
		std::getline(reader, line_buffer);
	}
		

	while (reader.eof() == false)
	{
		//csv;
		std::getline(reader, line_buffer);
		line_buffer = StringTools::trim(line_buffer);
		if (line_buffer == "")
			continue;

		result.push_back(_parse_row(line_buffer));
	}
	reader.close();

	return result;
}