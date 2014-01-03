#pragma once

#include "KMDB_Error.h"


extern "C" void  PASCAL EXPORT GetSimpleChar(const TCHAR * wstr,char * & str);
extern "C" void  PASCAL EXPORT GetWideChar(const std::string & resstr,wchar_t * & wstr);
extern "C" BOOL  PASCAL EXPORT IsWellFormXML(const TCHAR * xmlStr);

std::string GetSimpleChar(const CString & wstr);
CString  GetWideChar(const std::string & resstr);

//将字节数转化为 "123,567 KB"的显示方式
extern "C" void PASCAL FormatTheSizeOfFile(ULONGLONG ulSize, CString &cStrDisp);

extern "C" int PASCAL FileAndFolderOperate(CString cStrFrom, CString cStrTo, UINT nType, FILEOP_FLAGS flags);
extern "C" void PASCAL SplitCommaString(CString cStrSource, CStringArray &cArrayStr, TCHAR chSeparator, BOOL bIsXML);
extern "C" void PASCAL SplitFile2Part(CString cStrSource, CString &cStrPath, CString &cStrFileTitle, CString &cStrPostfix);

extern "C" BOOL PASCAL IsInSystemFolder(CString cStrTheFileName, CString &cStrFolderName, int &nCSIDL, BOOL &bEqual);

extern "C" int PASCAL GetFolderInformation(
	CString cStrFolderName, 
	BOOL	bIncludeSubFolder,
	BOOL	bIncludeHTMLSubFolder,
	CStringArray &arrayFilesResult, 
	INT64 &nTotalSize, 
	int &nSubFolderNumber, 
	int &nFilesNumber);
