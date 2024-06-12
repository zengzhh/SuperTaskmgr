/*
   Author: zengzhh
   Date: 2021/06/24
*/

// MySupertaskmgr.cpp

#include "MySupertaskmgr.h"
#include <stdio.h>
#include <tchar.h>
#include <sstream>
#include "Utility.h"

LPCTSTR Pid0Name = _T("ϵͳ����");
#define MAX_BUF 32767
inline LPTSTR AddNtUnicodeToStringContainer(
    CNewAlloc<TCHAR, 16384>& Container, UNICODE_STRING& us, TCHAR szBuffer[]
)
{
    int cchChar = us.Length / 2;
    LPTSTR result;
#ifdef UNICODE
    result = Container.NewData(us.Buffer, cchChar + 1);
#else
    cchChar = WideCharToMultiByte(CP_ACP, 0, us.Buffer, cchChar, szBuffer, MAX_BUF, "?", NULL);
    result = Container.NewData(szBuffer, cchChar + 1);
#endif
    result[cchChar] = _T('\0');
    return result;
}

MySupertaskmgr::MySupertaskmgr()
{
    fCPUUsage = 0.0f;
    qCPUTime = 0l;
    qIoSpeed = 0l;
    qReadTransferCount = 0l;
    qWriteTransferCount = 0l;
    qOtherTransferCount = 0l;
    proN = 0;
    proMaxN = 0;
    stMem.dwLength = sizeof(MEMORYSTATUSEX);
    if (!NT_SUCCESS(NtQuerySystemBasicInfo(&SystemBasicInfo))) {
        printf("NtQuerySystemBasicInfo error\n");
        SystemBasicInfo.NumberOfProcessors = 1;
    }
    if (!QueryPerformanceFrequency(&frequency)) {
        printf("NtQuerySystemBasicInfo error\n");
        frequency.QuadPart = 10000000l;
    }
    printf("NumberOfProcessors: %d, frequency: %lld\n", SystemBasicInfo.NumberOfProcessors, frequency.QuadPart);
    Refresh();
}

MySupertaskmgr::~MySupertaskmgr()
{
    printf("~");
}

void MySupertaskmgr::Refresh()
{
    LOG_INFO("start");
    NtQueryProcessInfo(pProcessInfo, &ProcessNumber);
    NtQueryTimeOfDayInfo(&TimeOfDayInfo);
    NtQueryPerformanceInfo(&PerformanceInfo);
    GlobalMemoryStatusEx(&stMem);

    RefreshProcessInfo();
    //RefreshGlobalInfo();
    LOG_INFO("end");
}

void MySupertaskmgr::RefreshGlobal()
{
    NtQueryTimeOfDayInfo(&TimeOfDayInfo);
    NtQueryPerformanceInfo(&PerformanceInfo);
    GlobalMemoryStatusEx(&stMem);

    RefreshGlobalInfo();
}

std::string MySupertaskmgr::GetTop5Info()
{
    printf("GetTop5Info...\n");

    for (int i = 0; i < proN; i++)
    {
        PROCESS& npro = *pro[i];
        if (npro.CPU > 0)
        {
            //wprintf(L"%6d | %6d %30s CPU(%2.1f) %10uB %10lld %lld\n", i, npro.PID, npro.name, npro.CPU, npro.PrivateWorkingSet, npro.IOReadSpeed, npro.IOWriteSpeed);
        }
    }

    // Insert
    for (int i = 0; i < proN; i++)
    {
        PROCESS& npro = *pro[i];
        if (npro.PID == 0)
        {
            continue;
        }
        std::string strExe = Utility::UnicodeToUtf8(npro.name) + "_" + std::to_string(npro.PID);
        m_mapCpu2Process.insert(std::pair<float, std::string>(npro.CPU, strExe));
        unsigned __int64 privateWorkingSet = npro.PrivateWorkingSet / 1024 + 0.5; // KB
        m_mapMemory2Process.insert(std::pair<unsigned __int64, std::string>(privateWorkingSet, strExe));
        unsigned __int64 IOReadSpeed = npro.IOReadSpeed; // B/s
        m_mapIORead2Process.insert(std::pair<unsigned __int64, std::string>(IOReadSpeed, strExe));
        unsigned __int64 IOWriteSpeed = npro.IOWriteSpeed; // B/s
        m_mapIOWrite2Process.insert(std::pair<unsigned __int64, std::string>(IOWriteSpeed, strExe));
    }

    // Sort
    std::multimap<float, std::string>::reverse_iterator itFloat;
    std::multimap<unsigned __int64, std::string>::reverse_iterator it64;
    std::stringstream ss;
    m_nCpuUsage = (int)fCPUUsage;
    m_nMemoryUsage = (int)stMem.dwMemoryLoad;
    ss << "cpu:total:" << m_nCpuUsage << ";";
    ss << "memory:total:" << m_nMemoryUsage << ";";
    int cnt = 0;
    for (itFloat = m_mapCpu2Process.rbegin(), cnt = 0; itFloat != m_mapCpu2Process.rend() && cnt < 5; ++itFloat, ++cnt)
        ss << "cpu:" << itFloat->second << ":" << itFloat->first << ";";

    for (it64 = m_mapMemory2Process.rbegin(), cnt = 0; it64 != m_mapMemory2Process.rend() && cnt < 5; ++it64, ++cnt)
        ss << "memory:" << it64->second << ":" << it64->first << ";";

    for (it64 = m_mapIORead2Process.rbegin(), cnt = 0; it64 != m_mapIORead2Process.rend() && cnt < 5; ++it64, ++cnt)
        ss << "io_read:" << it64->second << ":" << it64->first << ";";

    for (it64 = m_mapIOWrite2Process.rbegin(), cnt = 0; it64 != m_mapIOWrite2Process.rend() && cnt < 5; ++it64, ++cnt)
        ss << "io_write:" << it64->second << ":" << it64->first << ";";
    
    m_mapMemory2Process.clear();
    m_mapCpu2Process.clear();
    m_mapIORead2Process.clear();
    m_mapIOWrite2Process.clear();
    return ss.str();
}

std::string MySupertaskmgr::GetGlobalInfo()
{
    printf("GetGlobalInfo...\n");
    
    std::stringstream ss;
    m_nCpuUsage = (int)fCPUUsage;
    m_nMemoryUsage = (int)stMem.dwMemoryLoad;
    ss << "cpu:" << m_nCpuUsage << "\n";
    ss << "memory:" << m_nMemoryUsage << "\n";
    ss << "IoSpeed:" << ConvSpeedStr(qIoSpeed) << "\n";
    ss << "IoReadSpeed:" << ConvSpeedStr(qIoReadSpeed) << "\n";
    ss << "IoWriteSpeed:" << ConvSpeedStr(qIoWriteSpeed) << "\n";
    ss << "IoOtherSpeed:" << ConvSpeedStr(qIoOtherSpeed) << "\n";
    return ss.str();
}

void MySupertaskmgr::ReleaseSaveNameBuffSize(LPCTSTR lpBuffer)
{
    if (!lpBuffer || _tcslen(lpBuffer) <= 0)
        return;
    LOG_INFO("release %s", Utility::UnicodeToAnsi(lpBuffer).c_str());
    memset((void*)lpBuffer, 0, sizeof(TCHAR) * _tcslen(lpBuffer));
    int iNameBuffSize = 0;
    while (lpBuffer[iNameBuffSize] == _T('\0'))
        iNameBuffSize++;
    _mapReleaseBuffSize[lpBuffer] = iNameBuffSize;
    LOG_INFO("<%p , iNameBuffSize %d> size is %d", lpBuffer, iNameBuffSize, _mapReleaseBuffSize.size());
}

LPCTSTR MySupertaskmgr::GetSaveNameBuffSize(const UNICODE_STRING& us)
{
    int cchChar = us.Length / 2;
    for (auto it = _mapReleaseBuffSize.begin(); it != _mapReleaseBuffSize.end(); it++)
    {
        if (it->second >= cchChar + 1)  // �㹻�ռ�
        {
            LPCTSTR lpBuffer = it->first;
            bool bFindOk = (lpBuffer[-1] == _T('\0'));
            for (int i = 0; bFindOk && i < cchChar + 1; i++)  // �ٴ�ȷ�������ֽڶ��� 0
            {
                if (lpBuffer[i] != _T('\0'))
                {
                    bFindOk = false;
                }
            }
            if (bFindOk)
            {
                LOG_INFO("<%p , iNameBuffSize %d> need %d", lpBuffer, it->second, cchChar + 1);
                if (us.Buffer)
                    memcpy_s(
                        (void*)lpBuffer,
                        cchChar * sizeof(TCHAR),
                        us.Buffer,
                        cchChar * sizeof(TCHAR));
                _mapReleaseBuffSize.erase(it);
                return lpBuffer;
            }
        }
    }
    return NULL;
}

BOOL MySupertaskmgr::RefreshProcessInfo()
{
    static LONGLONG _LastTime = 0l; //��һ��ˢ��ʱ��(��λ��100����)
    LONGLONG _llTimeSpan; //������һ��ˢ�µ�ʱ����(��λ��100����)
    double _timeSec; //������һ��ˢ�µ�ʱ���� (��λ����)
    if (_LastTime == 0l) {
        _llTimeSpan = 0l;
        _timeSec = 0;
    }
    else
    {
        _llTimeSpan = TimeOfDayInfo.CurrentTime.QuadPart - _LastTime;
        _timeSec = (double)_llTimeSpan / frequency.QuadPart;
        printf("llTimeSpan: %lld / %lld = %.4fs\n", _llTimeSpan, frequency.QuadPart, _timeSec);
    }
    _LastTime = TimeOfDayInfo.CurrentTime.QuadPart;

    //�ӽ���������ɾ���Ѿ������ڵĽ���
    int i, j;
    LOG_INFO("proN: %d, proMaxN: %d, ProcessNumber: %d", proN, proMaxN, ProcessNumber);
    for (int i = 0; i < proN; i++) {
        PROCESS& npro = *pro[i];
        for (j = 0; j < ProcessNumber; j++) {
            if (pro[i]->PID == (DWORD)(DWORD_PTR)pProcessInfo[j]->ProcessId
                && pro[i]->pPID == (DWORD)(DWORD_PTR)pProcessInfo[j]->InheritedFromProcessId) {
                //LOG_INFO("j: %d", j);
                break;
            }
        }

        //������̲�����
        if (j == ProcessNumber) {

            //LOG_INFO("[del] proN: %d | i: %d, pid: %d, name: %s", proN, i, npro.PID, Utility::UnicodeToAnsi(npro.name).c_str());

            ReleaseSaveNameBuffSize(npro.name);  // ���ٵ��ڴ汸�������´�ʹ��
            memcpy(pro[i], pro[proN - 1], sizeof(PROCESS)); // �����һ�����̿�������������

            if (npro.Process)
                npro.Process.Close();
            if (npro.hIcon)
                DestroyIcon(npro.hIcon);

            proN--;

            //���������н���ǰ��
            //for (int k = i; k < proN; k++)
            //    pro[k] = pro[k + 1];

            i--;
        }
    }

    //��ʼˢ����Ϣ
    TCHAR szBuffer[MAX_BUF];
    for (j = 0; j < ProcessNumber; j++) {
        SYSTEM_PROCESSES& sp = *pProcessInfo[j];

        //�ӽ����������ҵ� pid ����Ľ���
        for (i = 0; i < proN; i++) {
            if (pro[i]->PID == (DWORD)(DWORD_PTR)sp.ProcessId)
                break;
        }

        //������½���
        if (i == proN) {
            if (proN >= proMaxN)
            {
                pro[proN] = ProcessContainer.NewData();
            }
            PROCESS& npro = *pro[proN];
            npro.PID = (DWORD)(DWORD_PTR)sp.ProcessId;//��ȡ���� pid
            npro.pPID = (DWORD)(DWORD_PTR)sp.InheritedFromProcessId;//��ȡ������ pid
            npro.SessionId = sp.SessionId; //��ȡ���̵ĻỰ ID

            //��ȡ��������
            if (npro.PID == 0) {
                npro.name = ProcessStringContainer.NewData(Pid0Name, _tcslen(Pid0Name) + 1);
            }
            else {
                npro.name = GetSaveNameBuffSize(sp.ProcessName);
                if (!npro.name)  // û���ҵ����ʵ��ڴ�
                {
                    npro.name = AddNtUnicodeToStringContainer(
                        ProcessStringContainer, sp.ProcessName, szBuffer);
                }
            }
            LOG_INFO("[add] proN: %d %s %d\n", proN, Utility::UnicodeToAnsi(npro.name).c_str(), _tcslen(npro.name));

            //...

            //��ʼ��CPU��IO��д����
            npro.CPUTime = 0l;
            npro.ReadTransferCount = 0l; //IO��ȡ�ֽ�
            npro.WriteTransferCount = 0l; //IOд���ֽ�
            npro.OtherTransferCount = 0l; //IO�����ֽ�
            //LOG_INFO("pid: %lu\n", npro.PID/*, Utility::UnicodeToAnsi(npro.name).c_str()*/);/*, name: %s*/
            proN++;
        }

        //��ʼ��ȡ������ϸ��Ϣ
        PROCESS& npro = *pro[i];

        npro.inWhite |= npro.isCurrentProcess;

        npro.bDisabledNetwork = FALSE;

        //ר�ù��������ֽڣ�stOsVerInfo.dwMajorVersion >= 6
        npro.PrivateWorkingSet = (SIZE_T)sp.PrivateWorkingSetSize; //ר�ù��������ֽڣ�

        npro.PrivateUsage = sp.PrivateUsage; //�����ڴ�(���������)���ֽڣ�
        npro.WorkingSetSize = sp.WorkingSetSize; //�ڴ棨�ֽڣ�
        npro.PeakWorkingSetSize = sp.PeakWorkingSetSize; //�ڴ��ֵ���ֽڣ�
#ifdef _WIN64
        npro.PeakVirtualSize = sp.PeakVirtualSize & 0x00000000FFFFFFFF; //�����ڴ��ֵ���ֽڣ�
#else
        if ((npro.PeakVirtualSize = sp.PeakWorkingSetSize) == 0xFFFFFFFF)
            npro.PeakVirtualSize = 0;
#endif
#ifdef _WIN64
        npro.VirtualSize = sp.VirtualSize & 0x00000000FFFFFFFF; //�����ڴ棨�ֽڣ�
#else
        if ((npro.VirtualSize = sp.VirtualSize) == 0xFFFFFFFF)
            npro.VirtualSize = 0;
#endif
        npro.QuotaPagedPoolUsage = sp.QuotaPagedPoolUsage; //ҳ�滺��أ��ֽڣ�
        npro.QuotaPeakPagedPoolUsage = sp.QuotaPeakPagedPoolUsage; //ҳ�滺��ط�ֵ���ֽڣ�
        npro.QuotaNonPagedPoolUsage = sp.QuotaNonPagedPoolUsage; //��ҳ�滺��أ��ֽڣ�
        npro.QuotaPeakNonPagedPoolUsage = sp.QuotaPeakNonPagedPoolUsage; //��ҳ�滺��ط�ֵ���ֽڣ�
        npro.PageFileUsage = sp.PageFileUsage; //ҳ���ļ�
        npro.PeakPageFileUsage = sp.PeakPageFileUsage; //ҳ���ļ���ֵ
        npro.PageFaultCount = sp.PageFaultCount; //ҳ����󣨸�����
        npro.ReadOperationCount = sp.ReadOperationCount.QuadPart; //IO��ȡ
        npro.WriteOperationCount = sp.WriteOperationCount.QuadPart; //IOд��
        npro.OtherOperationCount = sp.OtherOperationCount.QuadPart; //IO����
        npro.NumberOfThreads = sp.NumberOfThreads; //�߳�����
        npro.CreateTime = TimeOfDayInfo.CurrentTime.QuadPart - sp.CreateTime.QuadPart; //����������ʱ��
        npro.HandleCount = sp.HandleCount; //�������
        npro.Priority = KernelPriorityClass[sp.BasePriority]; //�������ȼ�

        npro.isSuspended = sp.NumberOfThreads > 0 && sp.Threads[0].ThreadState == 5 && sp.Threads[0].WaitReason == Suspended;

        npro.CPU = 0.0f;//��ʼ��CPUʹ����
        npro.IOSpeed = 0l;//��ʼ��IO��д����
        npro.IOReadSpeed = 0l;
        npro.IOWriteSpeed = 0l;

        if (_timeSec) {
            //����CPUʹ����
            if (npro.CPUTime != 0l) {
                npro.CPU = (FLOAT)(sp.KernelTime.QuadPart + sp.UserTime.QuadPart - npro.CPUTime) * 100.0f
                    / SystemBasicInfo.NumberOfProcessors
                    / (FLOAT)_llTimeSpan;
                if (npro.CPU < 0.0f)
                    npro.CPU = 0.0f;
                if (npro.CPU > 100.0f)
                    npro.CPU = 100.0f;
            }
            //����IO��ȡ����
            if (npro.ReadTransferCount != 0l) {
                npro.IOReadSpeed = (sp.ReadTransferCount.QuadPart - npro.ReadTransferCount) / _timeSec;
                if (npro.IOReadSpeed < 0l)
                    npro.IOReadSpeed = 0l;
            }
            //����IOд������
            if (npro.WriteTransferCount != 0l) {
                npro.IOWriteSpeed = (sp.WriteTransferCount.QuadPart - npro.WriteTransferCount) / _timeSec;
                if (npro.IOWriteSpeed < 0l)
                    npro.IOWriteSpeed = 0l;
            }
            //����IO������
            if (npro.ReadTransferCount != 0l || npro.WriteTransferCount != 0l || npro.OtherTransferCount != 0l) {
                npro.IOSpeed = (
                    (sp.ReadTransferCount.QuadPart - npro.ReadTransferCount) +
                    (sp.WriteTransferCount.QuadPart - npro.WriteTransferCount) +
                    (sp.OtherTransferCount.QuadPart - npro.OtherTransferCount)
                    ) / _timeSec;
                if (npro.IOSpeed < 0l)
                    npro.IOSpeed = 0l;
            }
        }
        npro.CPUTime = sp.KernelTime.QuadPart + sp.UserTime.QuadPart; //CPUʱ��
        npro.ReadTransferCount = sp.ReadTransferCount.QuadPart; //IO��ȡ�ֽ�
        npro.WriteTransferCount = sp.WriteTransferCount.QuadPart; //IOд���ֽ�
        npro.OtherTransferCount = sp.OtherTransferCount.QuadPart; //IO�����ֽ�
    }

    proMaxN = proMaxN > proN ? proMaxN : proN;
    //LOG_INFO("end");
    return TRUE;
}

BOOL MySupertaskmgr::RefreshGlobalInfo()
{
    GlobalMemoryStatusEx(&stMem);
    static LONGLONG LastTime; //��һ��ˢ�µ�ʱ�䣨��λ��100���룩
    LONGLONG llTimeSpan; //������һ��ˢ�µ�ʱ���� (��λ����)
    double timeSec; //������һ��ˢ�µ�ʱ���� (��λ����)
    if (LastTime == 0l)
    {
        llTimeSpan = 0l;
        timeSec = 0;
    }
    else
    {
        llTimeSpan = TimeOfDayInfo.CurrentTime.QuadPart - LastTime;
        timeSec = (double)llTimeSpan / frequency.QuadPart;
        printf("llTimeSpan: %lld %.4fs\n", llTimeSpan, timeSec);
    }
    LastTime = TimeOfDayInfo.CurrentTime.QuadPart;

    //����ȫ��CPUʹ���ʡ�IO��д�ٶ�
    fCPUUsage = 0.0f;
    qIoSpeed = 0l;
    qIoReadSpeed = 0l;
    qIoWriteSpeed = 0l;

    if (timeSec) {
        //����CPUʹ����
        if (qCPUTime != 0l) {
            fCPUUsage = 100.0f - (FLOAT)(PerformanceInfo.IdleProcessTime.QuadPart - qCPUTime) * 100.0f
                / (FLOAT)(SystemBasicInfo.NumberOfProcessors * llTimeSpan);
            if (fCPUUsage < 0.0f)
                fCPUUsage = 0.0f;
            if (fCPUUsage > 100.0f)
                fCPUUsage = 100.0f;
        }
        //����IO��ȡ�ٶ�
        qIoReadSpeed = (PerformanceInfo.IoReadTransferCount.QuadPart - qReadTransferCount) / timeSec;
        if (qIoReadSpeed < 0l)
            qIoReadSpeed = 0l;
        //����IOд���ٶ�
        qIoWriteSpeed = (PerformanceInfo.IoWriteTransferCount.QuadPart - qWriteTransferCount) / timeSec;
        if (qIoWriteSpeed < 0l)
            qIoWriteSpeed = 0l;
        //����IO�����ٶ�
        qIoOtherSpeed = (PerformanceInfo.IoOtherTransferCount.QuadPart - qOtherTransferCount) / timeSec;
        if (qIoOtherSpeed < 0l)
            qIoOtherSpeed = 0l;
        //����IO��д�ٶ�
        qIoSpeed = (
            (PerformanceInfo.IoReadTransferCount.QuadPart - qReadTransferCount) +
            (PerformanceInfo.IoWriteTransferCount.QuadPart - qWriteTransferCount) +
            (PerformanceInfo.IoOtherTransferCount.QuadPart - qOtherTransferCount)
            ) / timeSec;
        if (qIoSpeed < 0l)
            qIoSpeed = 0l;
    }

    qCPUTime = PerformanceInfo.IdleProcessTime.QuadPart;
    qReadTransferCount = PerformanceInfo.IoReadTransferCount.QuadPart;
    qWriteTransferCount = PerformanceInfo.IoWriteTransferCount.QuadPart;
    qOtherTransferCount = PerformanceInfo.IoOtherTransferCount.QuadPart;
    return TRUE;
}

std::string MySupertaskmgr::ConvSpeedStr(LONGLONG speed)
{
    std::string unit = "";
    double value = (double)speed;
    std::stringstream ss;
    ss.precision(1);
    ss.setf(std::ios::fixed);

    if (value <= 0.0f)
    {
        ss << "0KB/s";
    }
    else
    {
        if (value < 1024)
        {
            ss << value << "B/s";
        }
        else
        {
            value /= 1024;
            if (value < 1024)
            {
                ss << value << "KB/s";
            }
            else
            {
                value /= 1024;
                if (value < 1024)
                {
                    ss << value << "MB/s";
                }
                else
                {
                    value /= 1024;
                    ss << value << "GB/s";
                }
            }
        }
    }
    return ss.str();
}
