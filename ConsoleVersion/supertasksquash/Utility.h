/*
  Author: zengzhh
  Date: 2021/06/24
*/

// Utility.h

#pragma once

#include <string>

namespace Utility
{
	const std::string UnicodeToUtf8(const std::wstring& src);
	const std::string UnicodeToAnsi(const std::wstring& src);
}