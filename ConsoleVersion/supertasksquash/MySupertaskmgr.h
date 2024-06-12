/*
   Author: zengzhh
   Date: 2021/06/24
*/

// MySupertaskmgr.h

#pragma once

#include <string>
#include <map>
#include <Windows.h>
#include "WinLibrary\DMAlloc.h"
#include "WinLibrary\WinProcess.h"
#include "WinLibrary\WinSystemInfo.h"
#include "easylog.h"

typedef struct {
	BOOL isCurrentProcess; //是否是本进程本身
	BOOL inWhite;//是否为白名单进程
	BOOL inBlack;//是否为黑名单进程
	BOOL inSystem;//是否为系统进程
	BOOL Selected;//是否在列表中被选中
	BOOL Checked;//是否在列表中被复选
	CProcess Process; //能够访问进程信息的进程伪句柄

	LPCTSTR szDispName; //显示的进程名称
	LPCTSTR szPlatform; //显示的平台
	LPCTSTR szPID; //显示的进程 ID
	LPCTSTR szParentID; //显示的父进程 ID
	LPCTSTR szSessionId; //显示的会话 ID

	LPCTSTR name; //进程名称
	LPCTSTR path; //进程文件路径
	LPCTSTR cmdline; //进程命令行
	HICON hIcon; //进程文件图标
	int nImage; //图标列表中的序号
	LPCTSTR FileDescription;//进程文件描述
	FLOAT CPU; //CPU占用率
	LONGLONG NetTotalSpeed; //占用网速
	LONGLONG NetSendSpeed; //上传速度
	LONGLONG NetRecvSpeed; //下载速度
	LONGLONG IOSpeed; //IO读写速度
	LONGLONG IOReadSpeed; //IO读取速度
	LONGLONG IOWriteSpeed; //IO写入速度
	SIZE_T PrivateWorkingSet; //内存（专用工作集）

	LONGLONG NetSendFlow; //已发送的流量
	LONGLONG NetRecvFlow; //已接受的流量
	LONGLONG NetTotalFlow; //已用流量

	BOOL bDisabledNetwork; //是否已禁止联网

	BOOL isWow64Process; //是否是32位子系统
	LPCTSTR szUserName; //用户名
	BOOL isSuspended; //是否已挂起
	ULONG NumberOfThreads; //线程数量
	LONGLONG CreateTime;  //创建时间
	LONGLONG CPUTime;  //CPU时间
	DWORD Priority; //优先权限
	DWORD PID; //进程PID
	DWORD pPID; //父进程PID
	ULONG HandleCount; //句柄数量
	ULONG SessionId; //会话ID

	SIZE_T PeakVirtualSize; //虚拟内存峰值
	SIZE_T VirtualSize; //虚拟内存
	ULONG PageFaultCount; //页面错误
	SIZE_T PeakWorkingSetSize; //内存峰值
	SIZE_T WorkingSetSize; //内存
	SIZE_T QuotaPeakPagedPoolUsage; //页面缓冲池峰值
	SIZE_T QuotaPagedPoolUsage; //页面缓冲池
	SIZE_T QuotaPeakNonPagedPoolUsage; //非页面缓冲池峰值
	SIZE_T QuotaNonPagedPoolUsage; //非页面缓冲池
	SIZE_T PageFileUsage; //页面文件
	SIZE_T PeakPageFileUsage; //页面文件峰值
	SIZE_T PrivateUsage; //虚拟内存(任务管理器)

	LONGLONG ReadOperationCount; //IO读取
	LONGLONG WriteOperationCount; //IO写入
	LONGLONG OtherOperationCount; //IO其他
	LONGLONG ReadTransferCount; //IO读取字节
	LONGLONG WriteTransferCount; //IO写入字节
	LONGLONG OtherTransferCount; //IO其他字节
} PROCESS, * LPPROCESS;

class MySupertaskmgr
{
public:
	MySupertaskmgr();
	~MySupertaskmgr();

private:
	std::multimap<float, std::string> m_mapCpu2Process;
	std::multimap<unsigned __int64, std::string> m_mapMemory2Process;
	std::multimap<unsigned __int64, std::string> m_mapIORead2Process;
	std::multimap<unsigned __int64, std::string> m_mapIOWrite2Process;
	int m_nCpuUsage;
	int m_nMemoryUsage;

	FLOAT fCPUUsage; //总CPU占用率
	LONGLONG qCPUTime; //总CPU时间
	MEMORYSTATUSEX stMem; //总内存信息
	LONGLONG qIoSpeed; //总IO读写速度
	LONGLONG qIoReadSpeed; //总IO读取速度
	LONGLONG qIoWriteSpeed; //总IO写入速度
	LONGLONG qIoOtherSpeed; //总IO其他速度
	LONGLONG qReadTransferCount; //总IO读取字节
	LONGLONG qWriteTransferCount; //总IO写入字节
	LONGLONG qOtherTransferCount; //总IO其它字节

	SYSTEM_BASIC_INFORMATION SystemBasicInfo; //基础信息
	LARGE_INTEGER frequency; //性能计数器的频率
	CDMAlloc<SYSTEM_PROCESSES**> pProcessInfo; //进程信息
	int ProcessNumber; //进程数量
	SYSTEM_TIMEOFDAY_INFORMATION TimeOfDayInfo; //时间信息
	SYSTEM_PERFORMANCE_INFORMATION PerformanceInfo; //性能信息

	LPPROCESS pro[MaxProcessNumber];
	int proN; //记录的进程数
	int proMaxN;
	std::map<LPCTSTR, int>  _mapReleaseBuffSize;
	CNewAlloc<PROCESS, 8> ProcessContainer; //记录进程信息
	CNewAlloc<TCHAR, 16384> ProcessStringContainer; //记录进程名

public:
	void Refresh();
	void RefreshGlobal();
	std::string GetTop5Info();
	std::string GetGlobalInfo();

private:
	BOOL RefreshProcessInfo(); //进程信息
	BOOL RefreshGlobalInfo(); //全局信息
	std::string ConvSpeedStr(LONGLONG speed); //速度单位换算

    void ReleaseSaveNameBuffSize(LPCTSTR lpBuffer);  // 将name释放的内存进行保存以便下次使用
    LPCTSTR GetSaveNameBuffSize(const UNICODE_STRING& us);  // 获取name之前释放的内存用于再次使用
};