#include "stdafx.h"
#include "stdExtension.h"

std::vector<std::wstring> stdex::split(const std::wstring& str, wchar_t delimiter)
{
	std::wistringstream iss(str);             // istringstream�� str�� ��´�.
	std::wstring buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����

	std::vector<std::wstring> result;

	// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
	while (getline(iss, buffer, delimiter)) {
		if (buffer.size() == 0) continue;
		result.push_back(buffer);               // ����� ���ڿ��� vector�� ����
	}

	return result;
}

std::vector<std::string> stdex::split(const std::string& str, char delimiter)
{
	std::istringstream iss(str);             // istringstream�� str�� ��´�.
	std::string buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����

	std::vector<std::string> result;

	// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
	while (getline(iss, buffer, delimiter)) {
		if (buffer.size() == 0) continue;
		result.push_back(buffer);               // ����� ���ڿ��� vector�� ����
	}

	return result;
}

bool stdex::make_directory(const wchar_t* directory_path)
{
	if ((_waccess(directory_path, 0)) == -1) //���⿡ jsonBackup������ ������...    => _T�� L"" �� ������ wstring ǥ���� 
		CreateDirectory(directory_path, NULL);
	else
		return true;

	return (_waccess(directory_path, 0)) != -1;
}
