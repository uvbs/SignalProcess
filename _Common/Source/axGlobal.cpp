///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2003, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axGlobal.cpp
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang, John Xu, Cloud Ju
// [Version ]: R0.01
// [Release ]: 2003/12/10
// 
// [Contains]:
// <> Function: IPicture* AxLoadPictureFromBuffer(const BYTE*, DWORD)
// <> Function: BOOL AxGetResource(LPCTSTR, LPCTSTR, DWORD, HMODULE, void*, DWORD&)
// <> Function: BOOL AxIsWndClassMatch(HWND, LPCTSTR)
// <> Function: LONG AxGetWidth(const RECT&)
// <> Function: LONG AxGetHeight(const RECT&)
//
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   12/10/03  Leo     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "axGlobal.h"
#include <olectl.h>


///////////////////////////////////////////////////////////////////////////
// [Function]: IPicture* AxLoadPictureFromBuffer(const BYTE*, DWORD)
// [Comment ]: create IPicture* from a buffer which contains image data
// [Input   ]: <> pDataBuff  - [in] Pointer to a buffer which contains image data
//             <> dwDataSize - [in] Specific buffer size
// [Output  ]: NULL - fail ; other - success
// [Others  ]: global
//    
IPicture* AxLoadPictureFromBuffer(const BYTE* pDataBuff, DWORD dwDataSize)
{
    AP_ASSERT(pDataBuff != NULL);
    AP_ASSERT(dwDataSize > 0);

    // [] Allocate memory to store the image data
    //
    HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
    if (hGlobal == NULL)    return NULL;

    void* pvImageBuff = ::GlobalLock(hGlobal);
    if (pvImageBuff == NULL)
    {
        ::GlobalFree(hGlobal);
        return NULL;
    }

    ::CopyMemory(pvImageBuff, pDataBuff, dwDataSize);
    ::GlobalUnlock(hGlobal);

    IStream*  pStream  = NULL;
    IPicture* pPicture = NULL;

    // <> TRUE: hGlobal will be automatically freed
    if (::CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
    {
        HRESULT hr = ::OleLoadPicture(pStream, dwDataSize, FALSE, IID_IPicture, (void**)&pPicture);
        if (SUCCEEDED(hr))
            pStream->Release();
        else
            pPicture = NULL;
    }

    return pPicture;
}

///////////////////////////////////////////////////////////////////////////
// [Function]: BOOL AxGetResource(LPCTSTR, LPCTSTR, WORD, HMODULE, void*, DWORD&)
// [Comment ]: get resource data according to the resource name and type
// [Input   ]: <> pszResName  - [in] Specific the name of the resource, could 
//                                   use macro MAKEINTRESOURCE()
//             <> pszResType  - [in] Specific the resource type
//             <> wResLang    - [in] Specifies the language of the resource
//             <> hModule     - [in] Specific handle to the module whose executable
//                                   file contains the resource
//             <> pvResBuff   - [out] Pointer to receive buffer to hold resource data
//                                    if NULL, to get resource size only
//             <> rdwBuffSize - [in,out] Specific receive buffer size
//                                       must >= actual data size 
// [Output  ]: TRUE - success ; FALSE - fail
// [Others  ]: global
//
BOOL AxGetResource(LPCTSTR pszResName, LPCTSTR pszResType, WORD wResLang, 
                   HMODULE hModule, void* pvResBuff, DWORD& rdwBuffSize)
{
    AP_ASSERT(pszResName != NULL && ::lstrlen(pszResName) > 0);
    AP_ASSERT(pszResType != NULL && ::lstrlen(pszResType) > 0);
    
    HRSRC hResInfo = ::FindResourceEx(hModule, pszResName, pszResType, wResLang);
    if (hResInfo == NULL)   return FALSE;
    
    HGLOBAL hResGlobal = ::LoadResource(hModule, hResInfo);
    if (hResGlobal == NULL) return FALSE;
        
    void* pvResData = ::LockResource(hResGlobal);
    if (pvResData == NULL)  return FALSE;

    DWORD dwResSize = ::SizeofResource(hModule, hResInfo);
    if (pvResBuff == NULL) 
    {
        rdwBuffSize = dwResSize;        
        return TRUE;
    }
    if (rdwBuffSize < dwResSize)
    {
        rdwBuffSize = dwResSize;
        return FALSE;        
    }

    ::CopyMemory(pvResBuff, pvResData, dwResSize);
    rdwBuffSize = dwResSize;        // return actual resource size

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// [Function]: BOOL AxIsWndClassMatch(HWND, LPCTSTR)
// [Comment ]: judge if the window class of the hWnd is eque to pszWndClass
// [Input   ]: <> hWnd        - [in] Specific the window handle to judge
//             <> pszWndClass - [in] Specific the window class name to judge
// [Output  ]: TRUE - match; FALSE - not match
// [Others  ]: global
//
BOOL AxIsWndClassMatch(HWND hWnd, LPCTSTR pszWndClass)
{
    AP_ASSERT(hWnd != NULL);
    AP_ASSERT(pszWndClass != NULL && ::lstrlen(pszWndClass) > 0);

    static TCHAR szWndClassName[80] = { 0, };

    ::GetClassName(hWnd, szWndClassName, sizeof(szWndClassName));
    BOOL bRetValue = (::lstrcmpi(pszWndClass, szWndClassName) == 0);

    return bRetValue;	
}

///////////////////////////////////////////////////////////////////////////
// [Function]: LONG AxGetWidth(const RECT&)
// [Comment ]: get the width of specific rectangle
// [Input   ]: <> rRect - [in] Reference to a rectangle
// [Output  ]:
// [Others  ]: global
//
LONG AxGetWidth(const RECT& rRect)
{
    return (rRect.right - rRect.left);
}

///////////////////////////////////////////////////////////////////////////
// [Function]: LONG AxGetHeight(const RECT&)
// [Comment ]: get the height of specific rectangle
// [Input   ]: <> rRect - [in] Reference to a rectangle
// [Output  ]:
// [Others  ]: global
//
LONG AxGetHeight(const RECT& rRect)
{
    return (rRect.bottom - rRect.top);
}

///////////////////////////////////////////////////////////////////////////
// [Function]: void AxReboot()
// [Comment ]: reboot the system immediately
// [Input   ]:
// [Output  ]:
// [Others  ]: global
//
void AxReboot()
{
    OSVERSIONINFO oi = { 0, };
    oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&oi);

    if (oi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
    {
        // <> if NT/2000, must do
        HANDLE hTokenHandle = NULL;
        TOKEN_PRIVILEGES tkp;

        ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                           &hTokenHandle);
        ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

        tkp.PrivilegeCount = 1;     // one privilege to set
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

        ::AdjustTokenPrivileges(hTokenHandle, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    }

    // <> Shut down the system and force all applications to close. 
    ::ExitWindowsEx(EWX_FORCE | EWX_REBOOT, 0);
}

// 将一个字节的数字转换成两个ASCII字符
WORD Hex2A(BYTE data)
{
    BYTE hi = ((data & 0xF0) >> 4) + '0';
    if (hi > '9') hi += 7;
    
    BYTE lo = (data & 0x0F) + '0';
    if (lo > '9') lo += 7;
    
//    return MAKEWORD(lo, hi);
    return MAKEWORD(hi, lo);    
}

// 将两个ASCII字符转换成一个字节的Hex数字
BYTE A2Hex(WORD asc)
{
    BYTE hi = HIBYTE(asc);
    hi -= '0';
    if (hi > 9) hi -= 7;
    hi = hi & 0x0F;
    
    BYTE lo = LOBYTE(asc);
    lo -= '0';
    if (lo > 9) lo -= 7;
    lo = lo & 0x0F;
    
    BYTE result = (hi << 4) | lo;
    
    return result;
}
