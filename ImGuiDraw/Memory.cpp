#include "Memory.h"
#pragma once

#include<stdio.h>

#define NT_SUCCESS(x) ((x) >= 0)
#define ProcessBasicInformation 0

typedef struct _PROCESS_BASIC_INFORMATION32 {
    NTSTATUS ExitStatus;
    UINT32 PebBaseAddress;
    UINT32 AffinityMask;
    UINT32 BasePriority;
    UINT32 UniqueProcessId;
    UINT32 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION32;

typedef struct _PROCESS_BASIC_INFORMATION64 {
    NTSTATUS ExitStatus;
    UINT32 Reserved0;
    UINT64 PebBaseAddress;
    UINT64 AffinityMask;
    UINT32 BasePriority;
    UINT32 Reserved1;
    UINT64 UniqueProcessId;
    UINT64 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64;


typedef
NTSTATUS(WINAPI* pfnNtQueryInformationProcess)
(HANDLE ProcessHandle, ULONG ProcessInformationClass,
    PVOID ProcessInformation, UINT32 ProcessInformationLength,
    UINT32* ReturnLength);

typedef
NTSTATUS(WINAPI* pfnNtWow64QueryInformationProcess64)
(HANDLE ProcessHandle, UINT32 ProcessInformationClass,
    PVOID ProcessInformation, UINT32 ProcessInformationLength,
    UINT32* ReturnLength);


DWORD ReadWrite::GetProcessIdForWindow(const char* clsName, const char* winName)
{
    HWND hwnd = FindWindowA(clsName, winName);

    DWORD pid = 0;
    if (hwnd == NULL)
    {
              OutputDebugStringA("窗口不存在");

              return pid;
    }

    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0)
    {
             OutputDebugStringA("pid获取失败！");

             return pid;
    }

    return pid;
}

void ReadWrite::InitProcessHandleForWindow(const char* clsName, const char* winName)
{
    DWORD ProcessId = GetProcessIdForWindow(clsName, winName);
    if (ProcessId == 0)
    {
        return;
    }
    m_hpro = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessId);

}


HANDLE ReadWrite::GetProcessHandle()
{
    return m_hpro;
}

void ReadWrite::SetProcessHandle(HANDLE hpro)
{
    m_hpro = hpro;
}

DWORD ReadWrite::GetModuleAddress(char* szName)
{
    HMODULE NtdllModule = GetModuleHandleA("ntdll.dll");
    pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(NtdllModule,
        "NtQueryInformationProcess");
    PROCESS_BASIC_INFORMATION32 pbi = { 0 };
    UINT32  ReturnLength = 0;

    if (NtQueryInformationProcess == 0)
    {
        return 0;
    }

    NTSTATUS Status = NtQueryInformationProcess(m_hpro,
        ProcessBasicInformation, &pbi, (UINT32)sizeof(pbi), (UINT32*)&ReturnLength);
    if (NT_SUCCESS(Status))
    {
        DWORD peb = pbi.PebBaseAddress;

        DWORD ImageBaseAddress = 0;
        DWORD Ldr = 0;

        ImageBaseAddress = ReadMemoryDword(peb + 0x8);
        Ldr = ReadMemoryDword(peb + 0xC);


        DWORD pListHeader = NULL;
        DWORD pListNext = NULL;
        pListHeader = ReadMemoryDword(Ldr + 0x0C);
        pListNext = pListHeader;

        WCHAR strW[1024] = { 0 };
        CHAR strA[1024] = { 0 };
        DWORD uName = 0;
        DWORD dwBaseAddress = 0;
        //开始遍历模块
        do {
            memset(strA, 0, sizeof(strA));
            memset(strW, 0, sizeof(strW));
            dwBaseAddress = ReadMemoryDword(pListNext + 0x18);
            uName = ReadMemoryDword(pListNext + 0x30);
            ReadProcessMemory(m_hpro, (PVOID)uName, strW, sizeof(strW), NULL);
            UnicodeToAnsi(strA, strW, 256);
            //printf("模块:%s\n", strA);
            if (strcmp(szName, strA) == 0)
            {
                return dwBaseAddress;
            }
            pListNext = ReadMemoryDword(pListNext + 0x00);
        } while (pListNext != pListHeader);

    }
    return 0;
}


//int ReadWrite::Linked_Integer(int address, const int Deviation1)
//{
//    DWORD value;
//    value = address;
//
//    value = ReadMemoryDword(value) + Deviation1;
//    return value;
//}
int ReadWrite::Linked_Integer2(int address, const int Deviation1, const int Deviation2)
{
    DWORD value;
    value = address;

    value = ReadMemoryDword(value) + Deviation1;
    value = ReadMemoryDword(value) + Deviation2;
    return value;
}
int ReadWrite::Linked_Integer3(int address, const int Deviation1 = NULL, const int Deviation2 = NULL, const int Deviation3=NULL)
{
    DWORD value;
    value = address;
    if (Deviation1 != NULL)value = ReadMemoryDword(value) + Deviation1;
    if (Deviation2 != NULL)value = ReadMemoryDword(value) + Deviation2;
    if(Deviation3!=NULL)value = ReadMemoryDword(value) + Deviation3;
    return value;
}
int ReadWrite::Linked_Integer(int address, const int Deviation1, const int Deviation2, const int Deviation3,  int Deviation4)
{
    DWORD value;
    value = address;

    if (Deviation1 != 0xFFFFFF)value = ReadMemoryDword(value) + Deviation1;
    if (Deviation2 != 0xFFFFFF)value = ReadMemoryDword(value) + Deviation2;
    if (Deviation3 != 0xFFFFFF)value = ReadMemoryDword(value) + Deviation3;
    if (Deviation4 != 0xFFFFFF)value = ReadMemoryDword(value) + Deviation4;
    return value;
}
DWORD ReadWrite::GetAddress(char* Pattern, DWORD Module, DWORD Off)
{
    DWORD Result;
    std::vector<DWORD> SearchResult;

    SearchResult = Function::SearchMemory(m_hpro, Pattern, (DWORD)Module, (DWORD)Module + 0xfffffff);
    if (SearchResult.size() <= 0)
        return 0;

    Result = ReadMemory<DWORD>(SearchResult.at(0));
    //return Result - Module + ResultOff;
    return SearchResult.at(0) + Off;
}





