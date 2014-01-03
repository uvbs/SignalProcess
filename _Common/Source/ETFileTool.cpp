#include "stdafx.h"
#include "etfiletool.h"

#pragma once 

/*CETFileTool::CETFileTool()
{
}

CETFileTool::~CETFileTool()
{
}
*/
bool CETFileTool::VerifyFilePath(LPCTSTR filefullpath)
{
	CString szTmp,szFile = filefullpath;
	WIN32_FIND_DATA finddata;

	int index = szFile.Find('\\',3); //默认前三个为"$:\"
	while(index!=-1)
	{
		szTmp = szFile.Mid(0,index);
		if(::FindFirstFile(szTmp,&finddata)==INVALID_HANDLE_VALUE)
		{
			if(!::CreateDirectory(szTmp,NULL))
				return false;
		}
		index = szFile.Find('\\',index+1);
	}
	
	return true;
}

