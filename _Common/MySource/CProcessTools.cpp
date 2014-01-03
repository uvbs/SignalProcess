// CProcessTools.cpp: implementation of the CProcessTools class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CProcessTools.h"
#include <tlhelp32.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

long CProcessTools::KillProcess(LPCTSTR processName,long lWhich)
	//强行关闭任务管理器中映像名称为processName的进程
	//lWhich:关闭第几个映像名称为processName的进程，-1表示全部，0表示第一个
	//返回成功关闭的个数
{
	ASSERT(processName && _tcslen(processName));
	if(!processName || _tcslen(processName)<1) return 0;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) return 0; 
    PROCESSENTRY32 pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32); 
    if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	HANDLE hProcess=NULL;
	long ret=0;
	long count=0;
	do
	{
		hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
		if(_tcsicmp(pe32.szExeFile,processName)) CloseHandle (hProcess);
		else
		{
			if(count==lWhich || lWhich==-1)//需要关闭
			{
				if(TerminateProcess(hProcess,0)) ret++;
			}
			CloseHandle(hProcess);
			if(lWhich!=-1) break;//不是关闭所有,停止枚举
		}
	} 
	while(Process32Next(hProcessSnap, &pe32));
    CloseHandle (hProcessSnap);
	return ret;
}

long CProcessTools::ProcessKill(LPCTSTR processName,DWORD dwExceptPID)
	//强行关闭任务管理器中映像名称为processName的进程
	//dwExceptPID:不需要关闭的进程
	//返回成功关闭的个数
{
	ASSERT(processName && _tcslen(processName));
	if(!processName || _tcslen(processName)<1) return 0;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) return 0; 
    PROCESSENTRY32 pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32); 
    if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	HANDLE hProcess=NULL;
	long ret=0;
	long count=0;
	do
	{
		hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
		if(_tcsicmp(pe32.szExeFile,processName) || (pe32.th32ProcessID==dwExceptPID)) CloseHandle (hProcess);
		else if(TerminateProcess(hProcess,0)) ret++;
	} 
	while(Process32Next(hProcessSnap, &pe32));
    CloseHandle (hProcessSnap);
	return ret;
}

long CProcessTools::KillProcess(LPCTSTR processNames,LPCTSTR strExceptPIDs,LPCTSTR sep)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) return 0; 
    PROCESSENTRY32 pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32); 
    if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	HANDLE hProcess=NULL;
	long ret=0;
	long count=0;
	CString ts(sep);
	ts+=processNames;
	ts+=sep;
	CString ids(sep);
	if(strExceptPIDs) ids+=strExceptPIDs;
	ids+=sep;
	CString exefile;
	CString id;
	do
	{
		hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
		if(strExceptPIDs)
		{
			id.Format(_T("%s%u%s"),sep,pe32.th32ProcessID,sep);
			if(ids.Find(id)>-1) continue;
		}
		exefile=sep;
		exefile+=pe32.szExeFile;
		exefile+=sep;
		exefile.MakeLower();
		if(ts.Find(exefile)<0)
		{
			CloseHandle(hProcess);
			continue;
		}
		else if(TerminateProcess(hProcess,0)) ret++;
		CloseHandle(hProcess);
	} 
	while(Process32Next(hProcessSnap, &pe32));
    CloseHandle (hProcessSnap);
	return ret;
}

long CProcessTools::KillProcess(LPCTSTR processNames,LPCTSTR sep,DWORD dwExceptPID)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) return 0; 
    PROCESSENTRY32 pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32); 
    if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	HANDLE hProcess=NULL;
	long ret=0;
	long count=0;
	CString ts(sep);
	ts+=processNames;
	ts+=sep;
	ts.MakeLower();
	CString exefile;
	do
	{
		hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
		if(pe32.th32ProcessID==dwExceptPID) continue;
		exefile=sep;
		exefile+=pe32.szExeFile;
		exefile+=sep;
		exefile.MakeLower();
		if(ts.Find(exefile)<0)
		{
			CloseHandle(hProcess);
			continue;
		}
		else if(TerminateProcess(hProcess,0)) ret++;
		CloseHandle(hProcess);
	} 
	while(Process32Next(hProcessSnap, &pe32));
    CloseHandle (hProcessSnap);
	return ret;
}
