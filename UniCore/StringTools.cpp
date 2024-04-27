#include "stdafx.h"
#include "StringTools.h"

// trim from left 
std::wstring& StringTools::ltrim(std::wstring& s, const wchar_t* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}
// trim from right 
std::wstring& StringTools::rtrim(std::wstring& s, const wchar_t* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
// trim from left & right 
std::wstring& StringTools::trim(std::wstring& s, const wchar_t* t)
{
	return ltrim(rtrim(s, t), t);
}
std::vector<std::wstring> StringTools::split(std::wstring str, wchar_t Delimiter) {
	std::wistringstream iss(str);             // istringstream�� str�� ��´�.
	std::wstring buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����

	std::vector<std::wstring> result;

	// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
	while (getline(iss, buffer, Delimiter)) {
		if (buffer.size() == 0) continue;
		result.push_back(buffer);               // ����� ���ڿ��� vector�� ����
	}

	return result;
}


std::string& StringTools::ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}
// trim from right 
std::string& StringTools::rtrim(std::string& s, const char* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
// trim from left & right 
std::string& StringTools::trim(std::string& s, const char* t)
{
	return ltrim(rtrim(s, t), t);
}
std::vector<std::string> StringTools::split(std::string str, char Delimiter) {
	std::istringstream iss(str);             // istringstream�� str�� ��´�.
	std::string buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����

	std::vector<std::string> result;

	// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
	while (getline(iss, buffer, Delimiter)) {
		if (buffer.size() == 0) continue;
		result.push_back(buffer);               // ����� ���ڿ��� vector�� ����
	}

	return result;
}

std::string StringTools::reverse(std::string& s)
{
	std::string result;

	for (int32 i = s.size() - 1; i >= 0; i--)
	{
		result += s[i];
	}
	return result;
}
