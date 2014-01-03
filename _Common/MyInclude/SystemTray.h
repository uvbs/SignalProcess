
/*
	CSystemTray st;
	st.Add(this->m_hWnd,this->m_hIcon,IDR_MAINFRAME,WM_SYSTEMTRAY,"Tip");
	st.Modify(NULL,NULL,NULL,NULL,"Tip2");
	st.Delete();
//*/

#ifndef TWEN_SDK_CSYSTEMTRAY
#define TWEN_SDK_CSYSTEMTRAY

#include<shellapi.h>

class CSystemTray
{
public:
	CSystemTray();
	~CSystemTray();
	void Add(HWND hWnd,HICON hIcon,UINT IDIcon,DWORD wm_systemtray,LPCTSTR strTip);
	void Delete();
	void Modify(HWND hWnd,HICON hIcon,UINT IDIcon,DWORD wm_systemtray,LPCTSTR strTip);
	void ReAdd();
private:
	NOTIFYICONDATA nid;
	bool bCanUse;
};

#endif