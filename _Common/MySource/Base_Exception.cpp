
#include "stdafx.h"
#include "Joytech.Exception.h"



MFC_NAMESPACE_BEGIN(Joytech)
MFC_NAMESPACE_BEGIN(Exception)

///////////////////////////////////////////////////////////////////////////
// CJoytechException
///////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CJoytechException, CException)

CJoytechException::CJoytechException(LPCTSTR lpszWhat, LPCTSTR lpszFun, LPCTSTR lpszFile, int iLine):What(lpszWhat)
{
	if(lpszFun) What = What + _T("\r\n函数：") + lpszFun;
	if(lpszFile) What = What + _T("\r\n文件：") + lpszFile;
	if(iLine > -1)
	{
		CString t;
		t.Format(_T("%d"), iLine);
		What = What + _T("\r\n行：") + t;
	}
}


///////////////////////////////////////////////////////////////////////////
// CJoytechRangeException
///////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CJoytechRangeException, CJoytechException)

CJoytechRangeException::CJoytechRangeException(int iCurrent, int iMin, int iMax, LPCTSTR lpszWhat, LPCTSTR lpszFun, LPCTSTR lpszFile, int iLine):CJoytechException(lpszWhat, lpszFun, lpszFile, iLine)
{
	CString t;
	t.Format(_T("越界：当前：%d；有效[%d, %d]"), iCurrent, iMin, iMax);
	What = t + _T("\r\n") + What;
}



MFC_NAMESPACE_END(Exception)
MFC_NAMESPACE_END(Joytech)