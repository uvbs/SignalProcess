// Copyright (c) 2003, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axDebug.cpp
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang, John Xu, Cloud Ju
// [Version ]: R0.01
// [Release ]: 2003/12/02
// 
// [Contains]:
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   12/01/03  Leo     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "axDebug.h"


void Debug_v(const char* format, ...)
{
    TCHAR szTimeBuf[1024] = {0};
    SYSTEMTIME	st =  {0};
    GetLocalTime(&st);
    wsprintf(szTimeBuf, "[%2.2d/%2.2d/%2.2d - %2.2d:%2.2d:%2.2d] - ", st.wYear, st.wMonth,  st.wDay, st.wHour, st.wMinute, st.wSecond);

    TCHAR szBuffer[512] = {0};    
    va_list argptr;    
    va_start(argptr, format);
    vsprintf(szBuffer, format, argptr);
    va_end(argptr);

    ::lstrcat(szTimeBuf, szBuffer);

    if (g_pLogfile != NULL)
    {
        g_pLogfile->AddLine(szTimeBuf);
        g_pLogfile->Flush();
    }
}

void DbgMsgInt(int n)
{
    TCHAR szString[100] = {0};
    ::wsprintf(szString, _T("[%d]"), n);
    DbgMsg(szString);
}

void DbgMsgHex(int hex)
{
    TCHAR szString[100] = {0};
    ::wsprintf(szString, _T("[0x%Xh]"), hex);
    DbgMsg(szString);    
}