#pragma once
class CsvParser
{
public:
	static std::vector<std::vector<std::string>> ReadCsv(std::string path);
	static std::vector<std::vector<std::string>> ReadCsv(std::wstring path);
};