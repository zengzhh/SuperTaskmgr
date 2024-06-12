/*
   Author: zengzhh
   Date: 2021/06/24
*/

// WinAPI.h

#pragma once

LPVOID GetAPIAddress(LPCSTR lpModuleName, LPCSTR lpProcName, BOOL bAssertNull);
LPVOID GetAPIAddress(LPCWSTR lpModuleName, LPCSTR lpProcName, BOOL bAssertNull);

#define DEFINE_WINAPI(FUNC_TYPE, lpAddress, lpModuleName, lpProcName, bAssertNull) \
	FUNC_TYPE lpAddress=(FUNC_TYPE)GetAPIAddress(lpModuleName,lpProcName,bAssertNull)
