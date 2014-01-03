
#include "stdafx.h"
#include "CFileSystemEx.h"

///////////////////////////////
bool CFileSystemEx::GetOpenFile(CString&fpn, LPCTSTR fileType, LPCTSTR text, CWnd*parent)
{
	CString type;
	CString tmp(fileType);
	if(tmp.Find(_T('|'))>-1) type.Format(_T("%s||"),fileType);
	else type.Format(_T("%s文件|*.%s||"),fileType,fileType);
	CFileDialog mFileDlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_EXPLORER,type,parent);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000)// Windows NT
		mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else// if (dwWindowsMajorVersion < 4)// Win32s
		//else// Windows Me/98/95
		mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle=text;
	if(mFileDlg.DoModal()==IDOK)
	{
		fpn=mFileDlg.GetPathName();
		return true;
	}
	return false;
}
bool CFileSystemEx::GetSaveFile(CString&fpn, LPCTSTR fileType, LPCTSTR text, LPCTSTR defaultExt, LPCTSTR defaultFN, CWnd*parent)
{
	CString type;
	CString tmp(fileType);
	if(tmp.Find(_T('|'))>-1) type.Format(_T("%s||"),fileType);
	else type.Format(_T("%s文件|*.%s||"),fileType,fileType);
	CFileDialog mFileDlg(FALSE,defaultExt,defaultFN,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,type,parent);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000) mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle=text;
	if(mFileDlg.DoModal()==IDOK)
	{
		fpn=mFileDlg.GetPathName();
		return true;
	}
	return false;
}