#pragma once
class StringTools
{
public:
	static std::wstring& ltrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v");
	static std::wstring& rtrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v");
	static std::wstring& trim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v");
	static std::vector<std::wstring> split(std::wstring str, wchar_t Delimiter = L' ');

public:
	static std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
	static std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");
	static std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");
	static std::vector<std::string> split(std::string str, char Delimiter = ' ');

	static std::string reverse(std::string& s);
};

