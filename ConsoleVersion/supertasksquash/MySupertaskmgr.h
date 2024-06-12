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
	BOOL isCurrentProcess; //�Ƿ��Ǳ����̱���
	BOOL inWhite;//�Ƿ�Ϊ����������
	BOOL inBlack;//�Ƿ�Ϊ����������
	BOOL inSystem;//�Ƿ�Ϊϵͳ����
	BOOL Selected;//�Ƿ����б��б�ѡ��
	BOOL Checked;//�Ƿ����б��б���ѡ
	CProcess Process; //�ܹ����ʽ�����Ϣ�Ľ���α���

	LPCTSTR szDispName; //��ʾ�Ľ�������
	LPCTSTR szPlatform; //��ʾ��ƽ̨
	LPCTSTR szPID; //��ʾ�Ľ��� ID
	LPCTSTR szParentID; //��ʾ�ĸ����� ID
	LPCTSTR szSessionId; //��ʾ�ĻỰ ID

	LPCTSTR name; //��������
	LPCTSTR path; //�����ļ�·��
	LPCTSTR cmdline; //����������
	HICON hIcon; //�����ļ�ͼ��
	int nImage; //ͼ���б��е����
	LPCTSTR FileDescription;//�����ļ�����
	FLOAT CPU; //CPUռ����
	LONGLONG NetTotalSpeed; //ռ������
	LONGLONG NetSendSpeed; //�ϴ��ٶ�
	LONGLONG NetRecvSpeed; //�����ٶ�
	LONGLONG IOSpeed; //IO��д�ٶ�
	LONGLONG IOReadSpeed; //IO��ȡ�ٶ�
	LONGLONG IOWriteSpeed; //IOд���ٶ�
	SIZE_T PrivateWorkingSet; //�ڴ棨ר�ù�������

	LONGLONG NetSendFlow; //�ѷ��͵�����
	LONGLONG NetRecvFlow; //�ѽ��ܵ�����
	LONGLONG NetTotalFlow; //��������

	BOOL bDisabledNetwork; //�Ƿ��ѽ�ֹ����

	BOOL isWow64Process; //�Ƿ���32λ��ϵͳ
	LPCTSTR szUserName; //�û���
	BOOL isSuspended; //�Ƿ��ѹ���
	ULONG NumberOfThreads; //�߳�����
	LONGLONG CreateTime;  //����ʱ��
	LONGLONG CPUTime;  //CPUʱ��
	DWORD Priority; //����Ȩ��
	DWORD PID; //����PID
	DWORD pPID; //������PID
	ULONG HandleCount; //�������
	ULONG SessionId; //�ỰID

	SIZE_T PeakVirtualSize; //�����ڴ��ֵ
	SIZE_T VirtualSize; //�����ڴ�
	ULONG PageFaultCount; //ҳ�����
	SIZE_T PeakWorkingSetSize; //�ڴ��ֵ
	SIZE_T WorkingSetSize; //�ڴ�
	SIZE_T QuotaPeakPagedPoolUsage; //ҳ�滺��ط�ֵ
	SIZE_T QuotaPagedPoolUsage; //ҳ�滺���
	SIZE_T QuotaPeakNonPagedPoolUsage; //��ҳ�滺��ط�ֵ
	SIZE_T QuotaNonPagedPoolUsage; //��ҳ�滺���
	SIZE_T PageFileUsage; //ҳ���ļ�
	SIZE_T PeakPageFileUsage; //ҳ���ļ���ֵ
	SIZE_T PrivateUsage; //�����ڴ�(���������)

	LONGLONG ReadOperationCount; //IO��ȡ
	LONGLONG WriteOperationCount; //IOд��
	LONGLONG OtherOperationCount; //IO����
	LONGLONG ReadTransferCount; //IO��ȡ�ֽ�
	LONGLONG WriteTransferCount; //IOд���ֽ�
	LONGLONG OtherTransferCount; //IO�����ֽ�
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

	FLOAT fCPUUsage; //��CPUռ����
	LONGLONG qCPUTime; //��CPUʱ��
	MEMORYSTATUSEX stMem; //���ڴ���Ϣ
	LONGLONG qIoSpeed; //��IO��д�ٶ�
	LONGLONG qIoReadSpeed; //��IO��ȡ�ٶ�
	LONGLONG qIoWriteSpeed; //��IOд���ٶ�
	LONGLONG qIoOtherSpeed; //��IO�����ٶ�
	LONGLONG qReadTransferCount; //��IO��ȡ�ֽ�
	LONGLONG qWriteTransferCount; //��IOд���ֽ�
	LONGLONG qOtherTransferCount; //��IO�����ֽ�

	SYSTEM_BASIC_INFORMATION SystemBasicInfo; //������Ϣ
	LARGE_INTEGER frequency; //���ܼ�������Ƶ��
	CDMAlloc<SYSTEM_PROCESSES**> pProcessInfo; //������Ϣ
	int ProcessNumber; //��������
	SYSTEM_TIMEOFDAY_INFORMATION TimeOfDayInfo; //ʱ����Ϣ
	SYSTEM_PERFORMANCE_INFORMATION PerformanceInfo; //������Ϣ

	LPPROCESS pro[MaxProcessNumber];
	int proN; //��¼�Ľ�����
	int proMaxN;
	std::map<LPCTSTR, int>  _mapReleaseBuffSize;
	CNewAlloc<PROCESS, 8> ProcessContainer; //��¼������Ϣ
	CNewAlloc<TCHAR, 16384> ProcessStringContainer; //��¼������

public:
	void Refresh();
	void RefreshGlobal();
	std::string GetTop5Info();
	std::string GetGlobalInfo();

private:
	BOOL RefreshProcessInfo(); //������Ϣ
	BOOL RefreshGlobalInfo(); //ȫ����Ϣ
	std::string ConvSpeedStr(LONGLONG speed); //�ٶȵ�λ����

    void ReleaseSaveNameBuffSize(LPCTSTR lpBuffer);  // ��name�ͷŵ��ڴ���б����Ա��´�ʹ��
    LPCTSTR GetSaveNameBuffSize(const UNICODE_STRING& us);  // ��ȡname֮ǰ�ͷŵ��ڴ������ٴ�ʹ��
};