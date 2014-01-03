#ifndef _INCLUDE_APPCONTEXT_H_
#define _INCLUDE_APPCONTEXT_H_

class CAppContext
{
public:
    static VOID   GetAppPath(LPTSTR pszAppPath, PWORD pwAppPathLen);
    static LPCTSTR GetAppPath();

    static LPCTSTR GetTempDir();
};

#endif//_INCLUDE_APPCONTEXT_H_
