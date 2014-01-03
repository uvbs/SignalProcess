// Copyright (c) 2003, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axGlobal.h
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang, John Xu, Cloud Ju
// [Version ]: R0.01
// [Release ]: 2003/12/10
// 
// [Contains]:
// <> Macro:    SAFE_DELETE(p)
// <> Macro:    SAFE_DELETE_AR(p)
// <> Defines:  all the windows class(WNDCLASS) registered by Window or MFC
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

#ifndef _INCLUDE_AXGLOBAL_H_
#define _INCLUDE_AXGLOBAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct IPicture;

///////////////////////////////////////////////////////////////////////////////
// [Macro  ]: SAFE_DELETE(p)
// [Comment]: free the dynamically allocated memory specified by 'p' and set
//            its value to NULL. 
//
#define SAFE_DELETE(p)      if(p) { delete p; p = NULL; }

///////////////////////////////////////////////////////////////////////////////
// [Macro  ]: SAFE_DELETE_AR(p)
// [Comment]: free the dynamically allocated memory for an array specified by
//            'p' and set its value to NULL.
//
#define SAFE_DELETE_AR(p)   if(p) { delete [] p; p = NULL; }

///////////////////////////////////////////////////////////////////////////
// [Function]: IPicture* AxLoadPictureFromBuffer(const BYTE*, DWORD)
// [Comment ]: create IPicture* from a buffer which contains image data
// [Input   ]: <> pDataBuff  - [in] Pointer to a buffer which contains image data
//             <> dwDataSize - [in] Specific buffer size
// [Output  ]: NULL - fail ; other - success
// [Others  ]: global
//    
IPicture* AxLoadPictureFromBuffer(const BYTE* pDataBuff, DWORD dwDataSize);

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
                   HMODULE hModule, void* pvResBuff, DWORD& rdwBuffSize);

///////////////////////////////////////////////////////////////////////////////
// [Defines]: 
// [Comment]: all the windows class(WNDCLASS) registered by Window or MFC internally
//
#define AX_WC_BUTTON            _T("Button")
#define AX_WC_STATIC            _T("Static")
#define AX_WC_EDIT              _T("Edit")
#define AX_WC_COMBOBOX          _T("ComboBox")
#define AX_WC_COMBOLBOX         _T("ComboLBox")
#define AX_WC_LISTBOX           _T("ListBox")
#define AX_WC_SCROLLBAR         _T("Scrollbar")
#define AX_WC_TOOLBAR           _T("toolbarwindow32")
#define AX_WC_SPIN              _T("msctls_updown32")
#define AX_WC_PROGRESS          _T("msctls_progress32")
#define AX_WC_SLIDER            _T("msctls_trackbar32")
#define AX_WC_HOTKEY            _T("msctls_hotkey32")
#define AX_WC_SHELLDLLDEFVIEW   _T("SHELLDLL_DefView")
#define AX_WC_STATUSBAR         _T("msctls_statusbar32")
#define AX_WC_ANIMATE           _T("SysAnimate32")
#define AX_WC_RICHEDIT          _T("Richedit")
#define AX_WC_RICHEDIT20        _T("RichEdit20W")
#define AX_WC_DATETIMEPICK      _T("SysDateTimePick32")
#define AX_WC_MONTHCAL          _T("SysMonthCal32")
#define AX_WC_REBAR             _T("ReBarWindow32")
#define AX_WC_DIALOGBOX         _T("#32770")
#define AX_WC_MDICLIENT         _T("MDIClient")
#define AX_WC_TOOLTIPS          _T("tooltips_class32")
#define AX_WC_THUMBNAILVIEW     _T("ThumbnailVwExtWnd32")
#define AX_WC_IE                _T("Internet Explorer_Server")
#define AX_WC_SHDOCOBJVW        _T("Shell DocObject View")
#define AX_WC_SHELLEMBEDDING    _T("Shell Embedding")
#define AX_WC_CONTROLBAR        _T("AfxControlBar")
#define AX_WC_MFCDIALOGBAR      _T("MfcDialogBar")
#define AX_WC_MFCVIEW           _T("MfcView")
#define AX_WC_MFCMDICHILD       _T("MfcFrameMDIChild")
#define AX_WC_MFCMDIFRAME       _T("MfcFrameMDI")
#define AX_WC_MFCSPLITTER       _T("MfcSplitter")
#define AX_WC_MFCFRAME          _T("MfcFrame")
#define AX_WC_MFCMINIDOCKFRAME  _T("MfcFrameMiniDock")
#define AX_WC_MFCMINIFRAME      _T("MfcFrameMini")
#define AX_WC_MFCWND            _T("MfcWnd")
#define AX_WC_MENU              _T("#32768")

///////////////////////////////////////////////////////////////////////////
// [Function]: BOOL AxIsWndClassMatch(HWND, LPCTSTR)
// [Comment ]: judge if the window class of the hWnd is eque to pszWndClass
// [Input   ]: <> hWnd        - [in] Specific the window handle to judge
//             <> pszWndClass - [in] Specific the window class name to judge
// [Output  ]: TRUE - match; FALSE - not match
// [Others  ]: global
//
BOOL AxIsWndClassMatch(HWND hWnd, LPCTSTR pszWndClass);

///////////////////////////////////////////////////////////////////////////
// [Function]: LONG AxGetWidth(const RECT&)
// [Comment ]: get the width of specific rectangle
// [Input   ]: <> rRect - [in] Reference to a rectangle
// [Output  ]:
// [Others  ]: global
//
LONG AxGetWidth(const RECT& rRect);

///////////////////////////////////////////////////////////////////////////
// [Function]: LONG AxGetHeight(const RECT&)
// [Comment ]: get the height of specific rectangle
// [Input   ]: <> rRect - [in] Reference to a rectangle
// [Output  ]:
// [Others  ]: global
//
LONG AxGetHeight(const RECT& rRect);

///////////////////////////////////////////////////////////////////////////
// [Function]: void AxReboot()
// [Comment ]: reboot the system immediately
// [Input   ]:
// [Output  ]:
// [Others  ]: global
//
void AxReboot();

// 将一个字节的数字转换成两个ASCII字符
WORD Hex2A(BYTE data);

// 将两个ASCII字符转换成一个字节的Hex数字
BYTE A2Hex(WORD asc);

///////////////////////////////////////////////////////////////////////////////

#endif // _INCLUDE_AXGLOBAL_INCLUDE_