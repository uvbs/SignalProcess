
#ifndef _EXCEPTION_H
#define _EXCEPTION_H



#include "Joytech.Defs.h"



///////////////////////////////////////////////////////////////////////////
// CJoytechException
///////////////////////////////////////////////////////////////////////////
class CBase_Exception : public CException
{
	DECLARE_DYNAMIC(CJoytechException)
public:
	CJoytechException(LPCTSTR lpszWhat, LPCTSTR lpszFun = NULL, LPCTSTR lpszFile = NULL, int iLine = -1);

	CString What;
};


///////////////////////////////////////////////////////////////////////////
// CJoytechRangeException
///////////////////////////////////////////////////////////////////////////
class CJoytechRangeException : public CJoytechException
{
	DECLARE_DYNAMIC(CJoytechRangeException)
public:
	CJoytechRangeException(int iCurrent, int iMin, int iMax, LPCTSTR lpszWhat, LPCTSTR lpszFun = NULL, LPCTSTR lpszFile = NULL, int iLine = -1);
};



#endif