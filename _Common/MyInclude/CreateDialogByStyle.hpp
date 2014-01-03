BOOL CreateDialogByStyle(CDialog* pDlg, UINT nID, CWnd* pParentWnd, DWORD dwStyle, int iStyleLevel, HINSTANCE hInst)
{
	HRSRC hResource;
	HGLOBAL hTemplate;
	DWORD dwSize;
	LPCDLGTEMPLATE lpctDialogTemplate;
	LPDLGTEMPLATE lptDialogTemplate;
	LPDWORD lpdwStyle;
	BOOL bResult;
	hResource = FindResource(hInst, MAKEINTRESOURCE(nID), RT_DIALOG);
	dwSize = SizeofResource(hInst, hResource);
	hTemplate = LoadResource(hInst, hResource);
	lpctDialogTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
	lptDialogTemplate = (LPDLGTEMPLATE)new BYTE[dwSize];
	CopyMemory(lptDialogTemplate, lpctDialogTemplate, dwSize);
	UnlockResource(hTemplate);
	FreeResource(hTemplate);
	if (lptDialogTemplate->style == 0xFFFF0001) {
		lpdwStyle = (LPDWORD)&lptDialogTemplate->y;
	}
	else {
		lpdwStyle = (LPDWORD)&lptDialogTemplate->style;
	}
	switch (iStyleLevel) {
	case -1:
		*lpdwStyle = dwStyle;
		break;
	case 1:
		*lpdwStyle = (*lpdwStyle & 0x0000FFFF) | (dwStyle & 0xFFFF0000);
		break;
	}
	bResult = pDlg->CreateIndirect((LPCDLGTEMPLATE)lptDialogTemplate, pParentWnd);
	delete[](LPBYTE)lptDialogTemplate;
	return bResult;
}
