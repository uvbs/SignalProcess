#include "StdAfx.h"
#include "AppContext.h"


////////////////////////
VOID CAppContext::GetAppPath(LPTSTR pszAppPath, PWORD pwAppPathLen)
{
    TCHAR   *pszSearch;
    TCHAR   szAppPath[MAX_PATH+1];

    pszSearch = szAppPath;
    memset(szAppPath, 0, sizeof(szAppPath));
    ::GetModuleFileName(NULL, szAppPath, sizeof(szAppPath));

    while ((_tcschr(pszSearch, '\\') != NULL))
    {
        pszSearch = _tcschr(pszSearch, '\\') + 1;
    }
    pszSearch[-1] = '\0';

    _tcscpy(pszAppPath, szAppPath);
    *pwAppPathLen = (WORD)_tcslen(szAppPath);
}

////////////////////////
LPCTSTR CAppContext::GetAppPath()
{
    TCHAR   *pszSearch;
    static  TCHAR   szAppPath[MAX_PATH+1];

    pszSearch = szAppPath;
    memset(szAppPath, 0, sizeof(szAppPath));
    ::GetModuleFileName(NULL, szAppPath, sizeof(szAppPath));

    while ((_tcschr(pszSearch, '\\') != NULL))
    {
        pszSearch = _tcschr(pszSearch, '\\') + 1;
    }
    pszSearch[-1] = '\0';

    return szAppPath;
}

////////////////////////
LPCTSTR CAppContext::GetTempDir()
{
    static TCHAR szTempPath[MAX_PATH+1];
    
    memset(szTempPath, 0, sizeof(szTempPath));
    ::GetTempPath(sizeof(szTempPath), szTempPath);
	szTempPath[_tcslen(szTempPath)-1] = 0;
    return szTempPath;
}