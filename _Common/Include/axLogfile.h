// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axLogfile.h
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang 
// [Version ]: R0.01
// [Release ]: 2004/02/10
// 
// [Contains]:
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   02/10/04  Leo     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_AXLOGFILE_H_
#define _INCLUDE_AXLOGFILE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAxLogFile
{
public:
    CAxLogFile();
    ~CAxLogFile();
    
public:
    BOOL Open(LPCTSTR pszLogFilePath);
    void Close();
    
	BOOL AddLine(LPCTSTR pszLine);
	BOOL AddString(LPCTSTR pszString);

    void Flush();

private:
    FILE*   m_pLogFile;
	BOOL	m_bIsFileOpened;
	
};

#endif // _INCLUDE_AXLOGFILE_H_
