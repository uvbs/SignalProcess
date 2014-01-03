// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axLogfile.cpp
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

#include "StdAfx.h"
#include "AxLogfile.h"


CAxLogFile::CAxLogFile()
	: m_bIsFileOpened(FALSE), m_pLogFile(NULL)
{}

CAxLogFile::~CAxLogFile()
{
	if (m_pLogFile) 
    {
        fclose(m_pLogFile);
        m_pLogFile = NULL;
    }
}

BOOL CAxLogFile::Open(LPCTSTR pszLogFilePath)
{
    if (m_pLogFile != NULL) Close();

    m_pLogFile = fopen(pszLogFilePath, "w+");
    if (m_pLogFile == NULL) return FALSE;

    m_bIsFileOpened = TRUE;
    return TRUE;
}

void CAxLogFile::Close()
{
    if (m_pLogFile != NULL)
    {
        fclose(m_pLogFile);
        m_pLogFile      = NULL;
        m_bIsFileOpened = FALSE;
    }
}

BOOL CAxLogFile::AddLine(LPCTSTR pszLine)
{
	if (!m_bIsFileOpened) return FALSE;

    fprintf(m_pLogFile, "%s\n", pszLine);

	return TRUE;	
}

BOOL CAxLogFile::AddString(LPCTSTR pszString)
{
    if (!m_bIsFileOpened) return FALSE;
    
    fprintf(m_pLogFile, "%s", pszString);
    
    return TRUE;	
}

void CAxLogFile::Flush()
{
    if (m_bIsFileOpened)
    {
        fflush(m_pLogFile);
    }
}