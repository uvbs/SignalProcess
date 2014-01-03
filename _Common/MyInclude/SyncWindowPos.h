#pragma once

#define WM_SYNCWINDOWPOS_BIND			1	//WPARAM:HWND
#define WM_SYNCWINDOWPOS_LOCK			2	//WPARAM:LockFlag
#define WM_SYNCWINDOWPOS_MOVE			3	//WPARAM:cx;LPARAM:cy
#define WM_SYNCWINDOWPOS_HIDE			4	//WPARAM:HideFlag

class CSyncWindowPosMaster {
private:
	HWND m_hSlaveWnd;
	HWND m_hMasterWnd;
	HWND m_hLocateWnd;
	BOOL m_bLockFlag;
	UINT m_uBaseMsg;
public:
	CSyncWindowPosMaster();
	void setBaseMsg(UINT uMsg) {m_uBaseMsg = uMsg;}
	void init(HWND hMasterWnd, HWND hLocateWnd, LPCTSTR lpcszClassName, LPCTSTR lpcszTitleName);
	void onLock(BOOL bLock);
	void onMove();
	void onSize(BOOL bMin);
	BOOL onWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
protected:
	void onDestroy();
	void onActive(BOOL bActived, HWND hOtherWnd);
	void notifySlave(UINT uMsgIdx, WPARAM wParam = 0, LPARAM lParam = 0);
};

class CSyncWindowPosSlave {
private:
	HWND m_hMasterWnd;
	HWND m_hSlaveWnd;
	BOOL m_bLockFlag;
	UINT m_uBaseMsg;
public:
	CSyncWindowPosSlave();
	void setBaseMsg(UINT uMsg) {m_uBaseMsg = uMsg;}
	void init(HWND hSlaveWnd);
	BOOL onWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
protected:
	void notifyMaster(UINT uMsgIdx, WPARAM wParam = 0, LPARAM lParam = 0);
};
