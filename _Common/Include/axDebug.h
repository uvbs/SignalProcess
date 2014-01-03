// Copyright (c) 2003, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axDebug.h
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

#ifndef _INCLUDE_AXDEBUG_INCLUDE_
#define _INCLUDE_AXDEBUG_INCLUDE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <crtdbg.h>

extern CAxLogFile* g_pLogfile;        // 诊断信息记录文件

void Debug_v(const char* format, ...);

#ifdef _DEBUG
    #define AP_TRACE    Debug_v
    #define AP_ASSERT   _ASSERTE
#else // _DEBUG
    #define AP_TRACE    Debug_v
    #define AP_ASSERT
#endif // _DEBUG

// Debug function
#define DbgMsg(x) ::MessageBox(NULL, x, _T("Debug"), MB_ICONINFORMATION | MB_OK);
void DbgMsgInt(int n);
void DbgMsgHex(int hex);

/*
///////////////////////////////////////////////////////////////////////////////
// [Defines]: 
// [Comment]: custom trace and assert definition for extend
// 
#ifdef _DEBUG
    #define AP_TRACE0(msg)                      _RPTF0(_CRT_WARN, msg)
    #define AP_TRACE1(msg, arg1)                _RPTF1(_CRT_WARN, msg, arg1)
    #define AP_TRACE2(msg, arg1, arg2)          _RPTF2(_CRT_WARN, msg, arg1, arg2)
    #define AP_TRACE3(msg, arg1, arg2, arg3)    _RPTF3(_CRT_WARN, msg, arg1, arg2, arg3)

    #define AP_ASSERT   _ASSERTE
#else//_DEBUG
    #define AP_TRACE0(msg)
    #define AP_TRACE1(msg, arg1)
    #define AP_TRACE2(msg, arg1, arg2)
    #define AP_TRACE3(msg, arg1, arg2, arg3)

    #define AP_ASSERT
#endif//_DEBUG 
*/


///////////////////////////////////////////////////////////////////////////////

#endif // _INCLUDE_AXDEBUG_INCLUDE_