#pragma once

class CsvParser
{
public:
	static std::vector<std::vector<std::string>> ReadCsv(std::string path, bool ignore_head = false);
	static std::vector<std::vector<std::string>> ReadCsv(std::wstring path, bool ignore_head = false);

	static std::vector<std::string> row_from_string(const std::string& raw);
};