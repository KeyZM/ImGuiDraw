#pragma once
#define _USE_MATH_DEFINES

#include<Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <vector>
//#include <atlconv.h>

using namespace std;
namespace Function {

    std::vector<DWORD> SearchMemory(HANDLE hProcess, char* Pattern, DWORD StartAddress, DWORD EndAddress);
}
class ReadWrite
{
private:
    HANDLE m_hpro;
private:
    DWORD GetProcessIdForWindow(const char* clsName, const char* winName);
public:
    void InitProcessHandleForWindow(const char* clsName, const char* winName);
    HANDLE GetProcessHandle();
    void SetProcessHandle(HANDLE hpro);
    DWORD GetModuleAddress(char* szName);
    //int Linked_Integer(int address, const int Deviation1);
    int Linked_Integer2(int address, const int Deviation1, const int Deviation2);
    int Linked_Integer3(int address, const int Deviation1, const int Deviation2, const int Deviation3);
    int Linked_Integer(int address, const int Deviation1 = 0xFFFFFF, const int Deviation2 = 0xFFFFFF, const int Deviation3 = 0xFFFFFF, const int Deviation4 = 0xFFFFFF);
    DWORD GetAddress(char* Pattern, DWORD Module, DWORD Off);
    
    int WriteProcessMemoryFloat(int Pid, long Base, float WriteValue)
    {
        DWORD byread;
        int Value = 0;
        HANDLE proce = ::OpenProcess(PROCESS_ALL_ACCESS, false, Pid);
        LPCVOID pbase = (LPCVOID)Base;
        LPVOID rbuffer = (LPVOID)&Value;

        /*::ReadProcessMemory(proce, pbase, rbuffer, 4, &byread);
        pbase = (LPCVOID)(Value+one);

        ::ReadProcessMemory(proce, pbase, rbuffer, 4, &byread);
        pbase = (LPCVOID)(Value + two);*/

        ::ReadProcessMemory(proce, pbase, rbuffer, 4, &byread);


        DWORD bywrite;
        LPVOID wbuffer = (LPVOID)&WriteValue;
        WriteProcessMemory(proce, (LPVOID)pbase, wbuffer, 4, &bywrite);

        return 1;

    }

    BYTE ReadMemoryByte(DWORD addr)
    {
        BYTE data;
        ReadProcessMemory(m_hpro, (LPVOID)addr, &data, 1, NULL);
        return data;
    }


    int ReadMemoryDword(DWORD addr)
    {
        int data;
        ReadProcessMemory(m_hpro, (void*)addr, &data, sizeof(data), NULL);
        return data;
    }



    template <typename Type>
    Type ReadMemory(DWORD Address)
    {
        Type Temp;
        if (!ReadProcessMemory(m_hpro, reinterpret_cast<LPCVOID>(Address), &Temp, sizeof(Type), 0))
            return Type{};
        return Temp;
    }

    BOOL UnicodeToAnsi(PCHAR szTartget, PWCHAR szSrc, DWORD size)
    {
        if (szTartget == 0 || szSrc == 0)
        {
            return FALSE;
        }
        int i = 0;
        while (*szSrc != 0 && i < 256)
        {
            *szTartget = *szSrc;
            szTartget++;
            szSrc++;
            i++;
        }
        return TRUE;
    }
    int GetwindowW(HWND hwnd)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        return rect.right - rect.left;
    }

    int GetwindowH(HWND hwnd)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        return rect.bottom - rect.top;
    }

};