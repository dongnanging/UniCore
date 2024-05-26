#include "stdafx.h"
#include "stdExtension.h"

std::vector<std::wstring> stdex::split(std::wstring str, wchar_t delimiter)
{
	std::wistringstream iss(str);             // istringstream에 str을 담는다.
	std::wstring buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼

	std::vector<std::wstring> result;

	// istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
	while (getline(iss, buffer, delimiter)) {
		if (buffer.size() == 0) continue;
		result.push_back(buffer);               // 절삭된 문자열을 vector에 저장
	}

	return result;
}

bool stdex::make_directory(const wchar_t* directory_path)
{
	if ((_waccess(directory_path, 0)) == -1) //여기에 jsonBackup폴더가 없으면...    => _T는 L"" 과 유사한 wstring 표현식 
		CreateDirectory(directory_path, NULL);
	else
		return true;

	return (_waccess(directory_path, 0)) != -1;
}
