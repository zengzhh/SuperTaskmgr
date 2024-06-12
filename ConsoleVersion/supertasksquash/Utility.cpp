/*
  Author: zengzhh
  Date: 2021/06/24
*/

// Utility.cpp

#include "Utility.h"
#include <vector>
#include <Windows.h>

namespace Utility
{
	const std::string UnicodeToUtf8(const std::wstring& src)
	{
		int n = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> temp(n);
		::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, &temp[0], n, nullptr, nullptr);
		return &temp[0];
	}

	const std::string UnicodeToAnsi(const std::wstring& src)
	{
		int n = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> temp(n);
		::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, &temp[0], n, nullptr, nullptr);
		return &temp[0];
	}

}