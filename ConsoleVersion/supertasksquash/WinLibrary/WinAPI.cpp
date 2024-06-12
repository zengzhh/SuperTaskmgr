/*
   Author: zengzhh
   Date: 2021/06/24
*/

// WinAPI.cpp

#include <Windows.h>
#include <assert.h>
#include "WinAPI.h"
#include "WinDebug.h"

#pragma init_seg(lib)

LPVOID GetAPIAddress(LPCSTR lpModuleName, LPCSTR lpProcName, BOOL bAssertNull) {
	HMODULE hModule;
	if (!(hModule = GetModuleHandleA(lpModuleName)))
	{
		if (!(hModule = LoadLibraryA(lpModuleName))) {
			DebugPrintA("Error :Can't find the module \"%s\".\n", lpModuleName);
			assert(0);
		}
	}
	FARPROC lpAddress;
	if (!(lpAddress = GetProcAddress(hModule, lpProcName)) && bAssertNull) {
		DebugPrintA("Error :Can't find the function \"%s\" in the module \"%s\".\n",
			lpProcName, lpModuleName);
		assert(0);
	}
	return lpAddress;
}


LPVOID GetAPIAddress(LPCWSTR lpModuleName, LPCSTR lpProcName, BOOL bAssertNull) {
	HMODULE hModule;
	if (!(hModule = GetModuleHandleW(lpModuleName)))
	{
		if (!(hModule = LoadLibraryW(lpModuleName))) {
			DebugPrintW(L"Error :Can't find the module \"%s\".\n", lpModuleName);
			assert(0);
		}
	}
	FARPROC lpAddress;
	if (!(lpAddress = GetProcAddress(hModule, lpProcName)) && bAssertNull) {
		DebugPrintW(L"Error :Can't find the function \"%s\" in the module \"%s\".\n",
			lpProcName, lpModuleName);
		assert(0);
	}
	return lpAddress;
}


typedef NTSTATUS(NTAPI* FUNC_NtSuspendProcess)(HANDLE);
DEFINE_WINAPI(FUNC_NtSuspendProcess, NtSuspendProcess,
	L"ntdll", "NtSuspendProcess", TRUE);

typedef NTSTATUS(NTAPI* FUNC_NtResumeProcess)(HANDLE);
DEFINE_WINAPI(FUNC_NtResumeProcess, NtResumeProcess,
	L"ntdll", "NtResumeProcess", TRUE);

typedef NTSTATUS(NTAPI* FUNC_NtQueryInformationProcess)(HANDLE, DWORD, PVOID, ULONG, PULONG);
DEFINE_WINAPI(FUNC_NtQueryInformationProcess, NtQueryInformationProcess,
	L"ntdll", "NtQueryInformationProcess", TRUE);

typedef ULONG(NTAPI* FUNC_RtlNtStatusToDosError)(NTSTATUS Status);
DEFINE_WINAPI(FUNC_RtlNtStatusToDosError, RtlNtStatusToDosError,
	L"ntdll", "RtlNtStatusToDosError", TRUE);

typedef void (NTAPI* FUNC_RtlGetNtVersionNumbers)(LPDWORD, LPDWORD, LPDWORD);
DEFINE_WINAPI(FUNC_RtlGetNtVersionNumbers, RtlGetNtVersionNumbers,
	L"ntdll", "RtlGetNtVersionNumbers", TRUE);

typedef NTSTATUS(NTAPI* FUNC_NtQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);
DEFINE_WINAPI(FUNC_NtQuerySystemInformation, NtQuerySystemInformation,
	L"ntdll", "NtQuerySystemInformation", TRUE);