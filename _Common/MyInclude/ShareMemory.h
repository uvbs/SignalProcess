// ShareMemory.h: interface for the CShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREMEMORY_H__8EBCE02A_EA82_4436_B3AC_7022A11D503E__INCLUDED_)
#define AFX_SHAREMEMORY_H__8EBCE02A_EA82_4436_B3AC_7022A11D503E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
// CShareMemory g_ShareMemory;

void CMy1Dlg::On1() //1
{
	UpdateData(true);
	g_ShareMemory.New(_T("TWen_CShareMemory_Sample"), 1024000);
	memcpy(g_ShareMemory.BeginRW(), (LPCTSTR)m_strValue, (m_strValue.GetLength() + 1)*sizeof(TCHAR));
	g_ShareMemory.EndRW();
}
void CMy1Dlg::On4() //4
{
	m_strValue = (LPCTSTR)g_ShareMemory.BeginRW();
	g_ShareMemory.EndRW();
	UpdateData(false);
}

void CMy2Dlg::On2() //2
{
	g_ShareMemory.Open(_T("TWen_CShareMemory_Sample"));
	m_strValue = (LPCTSTR)g_ShareMemory.BeginRW();
	g_ShareMemory.EndRW();
	UpdateData(false);
}
void CMy2Dlg::On3() //3
{
	UpdateData(true);
	memcpy(g_ShareMemory.BeginRW(), (LPCTSTR)m_strValue, (m_strValue.GetLength() + 1)*sizeof(TCHAR));
	g_ShareMemory.EndRW();
}
*/

class CShareMemory  
{
public:
	CShareMemory();
	~CShareMemory();

public:
	bool New(LPCTSTR name, DWORD size);
	bool Open(LPCTSTR name);
	BYTE* BeginRW();
	void EndRW();
	void Close();

protected:
	HANDLE m_hHandle;
	BYTE* m_pByte;
};

#endif // !defined(AFX_SHAREMEMORY_H__8EBCE02A_EA82_4436_B3AC_7022A11D503E__INCLUDED_)
