#include "stdafx.h"
#include "CsvParser.h"

#include "stdafx.h"
#include "StringTools.h"

#include <fstream>

std::vector<std::vector<std::string>> CsvParser::ReadCsv(std::wstring path)
{
	std::vector<std::vector<std::string>> result;

	return result;
}


std::vector<std::vector<std::string>> CsvParser::ReadCsv(std::string path)
{
	std::vector<std::vector<std::string>> result;

	std::string line_buffer;
	std::fstream reader;

	reader.open(path, std::ios::in);

	while (reader.eof() == false)
	{
		//csv;
		std::getline(reader, line_buffer);
		line_buffer = StringTools::trim(line_buffer);
		if (line_buffer == "")
			continue;

		auto split_csv = StringTools::split(line_buffer, ',');
		result.push_back(split_csv);
	}
	reader.close();

	return result;
}