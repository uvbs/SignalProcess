// ShareMemory.cpp: implementation of the CShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShareMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShareMemory::CShareMemory():m_hHandle(NULL),m_pByte(NULL)
{
}
CShareMemory::~CShareMemory()
{
	Close();
}

bool CShareMemory::New(LPCTSTR name, DWORD size)
{
	Close();
	m_hHandle = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, size, name);
	return m_hHandle != NULL;

}
bool CShareMemory::Open(LPCTSTR name)
{
	Close();
	m_hHandle = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, name);
	return m_hHandle != NULL;

}
BYTE* CShareMemory::BeginRW()
{
	m_pByte = NULL;
	if(m_hHandle) m_pByte = (BYTE*)MapViewOfFile(m_hHandle, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);;
	return m_pByte; 
}
void CShareMemory::EndRW()
{
	if(m_pByte)
	{
		UnmapViewOfFile(m_pByte);
		m_pByte = NULL;
	}
}
void CShareMemory::Close()
{
	if(m_hHandle)
	{
		CloseHandle(m_hHandle);
		m_hHandle = NULL;
	}
}