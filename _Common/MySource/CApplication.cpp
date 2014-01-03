
#include "stdafx.h"
#include "CApplication.h"

CString CApplication::GetApplicationPath()
{
	CString sPath;
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	sPath.ReleaseBuffer();
	int i=sPath.GetLength()-1;
	while(i>0)
	{
		if(sPath.GetAt(i)=='/' || sPath.GetAt(i)=='\\') break;
		i--;
	}
	if(i<0) return _T("");
	if(i==0) return _T("./");
	return sPath.Left(i);
}