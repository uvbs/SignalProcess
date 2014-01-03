// FileSystem.h: interface for the CFileSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FileSystem_H__70E77DA0_B048_4A33_A60C_42D4B6A251D0__INCLUDED_)
#define AFX_FileSystem_H__70E77DA0_B048_4A33_A60C_42D4B6A251D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <shlobj.h>

class CFolderDialog  
{
friend static int CALLBACK BrowseDirectoryCallback(
				HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

public:
	CFolderDialog(	LPCTSTR title = _T("ÇëÑ¡ÔñÂ·¾¶"),
					CWnd* pParentWnd = NULL,
					LPCTSTR lpszInitialFolder = NULL, 
					DWORD dwFlags = BIF_RETURNONLYFSDIRS
					);
	virtual ~CFolderDialog();
	virtual int DoModal();
	CString GetPathName() const;

protected:
	virtual void OnInitDialog();
	virtual void OnSelChanged(ITEMIDLIST* pIdl);
	virtual void CallbackFunction(HWND hWnd, UINT uMsg,	LPARAM lParam);

	void EnableOK(BOOL bEnable = TRUE);
	void SetSelection(LPCTSTR pszSelection);
	void SetSelection(ITEMIDLIST* pIdl);
	void SetStatusText(LPCTSTR pszStatusText);
	CString ShortName(const CString& strName);

public:
	BROWSEINFO m_bi;

protected:
	CString m_strInitialFolderName;
	CString m_strFinalFolderName;

	TCHAR m_szDisplayName[MAX_PATH];
	TCHAR m_szPath[MAX_PATH];

	HWND m_hDialogBox;
};

#endif // !defined(AFX_FileSystem_H__70E77DA0_B048_4A33_A60C_42D4B6A251D0__INCLUDED_)
