#include "stdafx.h"
#include "SyncWindowPos.h"

CSyncWindowPosMaster::CSyncWindowPosMaster() : m_hSlaveWnd(NULL), m_hMasterWnd(NULL), m_hLocateWnd(NULL), m_bLockFlag(FALSE)
{
	m_uBaseMsg = WM_USER;
}
void CSyncWindowPosMaster::init(HWND hMasterWnd, HWND hLocateWnd, LPCTSTR lpcszClassName, LPCTSTR lpcszTitleName)
{
	m_hMasterWnd = hMasterWnd;
	m_hLocateWnd = hLocateWnd;
	if (m_hSlaveWnd = FindWindow(lpcszClassName, lpcszTitleName)) {
		notifySlave(WM_SYNCWINDOWPOS_BIND, (WPARAM)hMasterWnd);
	}
}
void CSyncWindowPosMaster::onDestroy()
{
	notifySlave(WM_SYNCWINDOWPOS_BIND, NULL);
}
void CSyncWindowPosMaster::onLock(BOOL bLock)
{
	m_bLockFlag = bLock;
	notifySlave(WM_SYNCWINDOWPOS_LOCK, bLock);
}
void CSyncWindowPosMaster::onMove()
{
	RECT r;
	if (m_bLockFlag && m_hLocateWnd && IsWindow(m_hLocateWnd)) {
		GetWindowRect(m_hLocateWnd, &r);
		notifySlave(WM_SYNCWINDOWPOS_MOVE, (WPARAM)r.left, (LPARAM)r.top);
	}
}
void CSyncWindowPosMaster::onSize(BOOL bMin)
{
	if (m_bLockFlag) {
		notifySlave(WM_SYNCWINDOWPOS_HIDE, bMin);
		if (!bMin) onMove();
	}
}
void CSyncWindowPosMaster::onActive(BOOL bActived, HWND hOtherWnd)
{
	if (m_bLockFlag && m_hSlaveWnd && IsWindow(m_hSlaveWnd)) {
		if (bActived) {
			SetWindowPos(m_hSlaveWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
		}
		else {
			if (hOtherWnd != m_hSlaveWnd) {
				SetWindowPos(m_hSlaveWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
				SetWindowPos(m_hMasterWnd, m_hSlaveWnd, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
			}
		}
	}
}
BOOL CSyncWindowPosMaster::onWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message) {
	case WM_ACTIVATE:
		onActive(LOWORD(wParam), (HWND)lParam);
		break;
	case WM_DESTROY:
		onDestroy();
		break;
	default:
		switch(message - m_uBaseMsg) {
		case WM_SYNCWINDOWPOS_BIND:
			m_hSlaveWnd = (HWND)wParam;
			return TRUE;
		case WM_SYNCWINDOWPOS_HIDE:
			if (!wParam) onMove();
			return TRUE;
		}
	}
	return FALSE;
}
void CSyncWindowPosMaster::notifySlave(UINT uMsgIdx, WPARAM wParam, LPARAM lParam)
{
	if (m_hSlaveWnd && IsWindow(m_hSlaveWnd)) {
		PostMessage(m_hSlaveWnd, m_uBaseMsg + uMsgIdx, wParam, lParam);
	}
}


CSyncWindowPosSlave::CSyncWindowPosSlave() : m_hMasterWnd(NULL), m_hSlaveWnd(NULL), m_bLockFlag(FALSE)
{
	m_uBaseMsg = WM_USER;
}
void CSyncWindowPosSlave::init(HWND hSlaveWnd)
{
	m_hSlaveWnd = hSlaveWnd;
}
BOOL CSyncWindowPosSlave::onWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message) {
	case WM_SIZE:
		if (wParam == SIZE_RESTORED && m_bLockFlag) {
			notifyMaster(WM_SYNCWINDOWPOS_HIDE, FALSE);
		}
		break;
	case WM_NCLBUTTONDOWN:
		if (m_bLockFlag) {
			if (wParam == HTCAPTION) {
				*pResult = 0;
				return TRUE;
			}
		}
		break;
	case WM_CLOSE:
		if (m_bLockFlag) {
			*pResult = 0;
			return TRUE;
		}
		break;
	case WM_DESTROY:
		notifyMaster(WM_SYNCWINDOWPOS_BIND, NULL);
		break;
	default:
		switch(message - m_uBaseMsg) {
		case WM_SYNCWINDOWPOS_BIND:
			if ((m_hMasterWnd = (HWND)wParam) == NULL) {
				m_bLockFlag = FALSE;
			}
			return TRUE;
		case WM_SYNCWINDOWPOS_LOCK:
			if (m_bLockFlag = (BOOL)wParam) {
				notifyMaster(WM_SYNCWINDOWPOS_HIDE, FALSE);
			}
			return TRUE;
		case WM_SYNCWINDOWPOS_MOVE:
			if (m_bLockFlag) {
				if (m_hSlaveWnd) {
					SetWindowPos(m_hSlaveWnd, NULL, (int)wParam, (int)lParam, 0, 0, SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER);
				}
			}
			return TRUE;
		case WM_SYNCWINDOWPOS_HIDE:
			if (m_bLockFlag) {
				if (m_hSlaveWnd) {
					if (wParam)
						ShowWindow(m_hSlaveWnd, SW_MINIMIZE);
					else
						PostMessage(m_hSlaveWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
void CSyncWindowPosSlave::notifyMaster(UINT uMsgIdx, WPARAM wParam, LPARAM lParam)
{
	if (m_hMasterWnd && IsWindow(m_hMasterWnd)) {
		PostMessage(m_hMasterWnd, m_uBaseMsg + uMsgIdx, wParam, lParam);
	}
}
