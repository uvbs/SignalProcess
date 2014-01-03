#include "stdafx.h"
#include "WndCtrlPos.h"

CWndCtrlPos::CWndCtrlPos()
{
	m_hWnd = NULL;
	m_ptWndCtrlInfo = NULL;
	m_iCtrlNum = 0;
	m_iCtrlMaxNum = 0;
	m_tMinWndSize.x = 0;
}
CWndCtrlPos::~CWndCtrlPos()
{
	if (m_ptWndCtrlInfo) delete []m_ptWndCtrlInfo;
}
void CWndCtrlPos::bindWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}
BOOL CWndCtrlPos::initCtrlNum(int iNum)
{
	if (!iNum) return FALSE;
	if (m_ptWndCtrlInfo) delete []m_ptWndCtrlInfo;
	m_iCtrlNum = 0;
	if ((m_ptWndCtrlInfo = new WND_CTRL_INFO[iNum]) == NULL) return FALSE;
	m_iCtrlMaxNum = iNum;
	return TRUE;
}
BOOL CWndCtrlPos::addCtrl(HWND hCtrl, DWORD dwXFlags, DWORD dwYFlags, LPPOINT ptMore)
{
	if (hCtrl == NULL) return FALSE;
	if (m_iCtrlNum >= m_iCtrlMaxNum) return FALSE;
	m_ptWndCtrlInfo[m_iCtrlNum].hCtrl = hCtrl;
	m_ptWndCtrlInfo[m_iCtrlNum].bActived = FALSE;
	m_ptWndCtrlInfo[m_iCtrlNum].tXInfo.dwFlags = dwXFlags;
	m_ptWndCtrlInfo[m_iCtrlNum].tYInfo.dwFlags = dwYFlags;
	if (ptMore) {
		m_ptWndCtrlInfo[m_iCtrlNum].tXInfo.iMore = ptMore->x;
		m_ptWndCtrlInfo[m_iCtrlNum].tYInfo.iMore = ptMore->y;
	}
	m_iCtrlNum++;
	return TRUE;
}
BOOL CWndCtrlPos::addCtrlID(INT iID, DWORD dwXFlags, DWORD dwYFlags, LPPOINT ptMore)
{
	if (m_hWnd == NULL) return FALSE;
	return addCtrl(GetDlgItem(m_hWnd, iID), dwXFlags, dwYFlags, ptMore);
}

void CWndCtrlPos::makeAlignInfo(P_WND_CTRL_ALIGN_INFO pwcai, int iStart, int iSize, int iTotal)
{
	pwcai->iMargin = iStart;
	pwcai->iMargin2 = iTotal - (iStart + iSize);
	pwcai->iSize = iSize;
}

void CWndCtrlPos::translateAlignInfo(P_WND_CTRL_ALIGN_INFO pwcai, int iTotal, int &iStart, int &iSize)
{
	if (pwcai->dwFlags & WND_CTRL_SIZE_PERCENT) {
		iSize = iTotal * pwcai->iMore / 100;
	}
	else {
		iSize = pwcai->iSize;
	}
	switch(pwcai->dwFlags & WND_CTRL_ALIGN_MASK) {
	case WND_CTRL_ALIGN_LEFT:
		iStart = pwcai->iMargin;
		break;
	case WND_CTRL_ALIGN_CENTER:
		iStart = (iTotal - iSize) / 2;
		break;
	case WND_CTRL_ALIGN_RIGHT:
		iStart = iTotal - pwcai->iMargin2 - iSize;
		break;
	case WND_CTRL_ALIGN_JUSTIFY:
		iStart = pwcai->iMargin;
		iSize = iTotal - pwcai->iMargin2 - pwcai->iMargin;
		break;
	}
}

void CWndCtrlPos::keepWndSize(LPPOINT ptClientXY)
{
	RECT r;
	POINT p;
	int i;
	if (m_hWnd == NULL) return;
	GetWindowRect(m_hWnd, &r);
	m_tMinWndSize.x = r.right - r.left, m_tMinWndSize.y = r.bottom - r.top;
	if (ptClientXY) {
		m_tMinClientSize.x = ptClientXY->x, m_tMinClientSize.y = ptClientXY->y;
	}
	else{
		GetClientRect(m_hWnd, &r);
		m_tMinClientSize.x = r.right, m_tMinClientSize.y = r.bottom;
	}
	for (i = 0; i < m_iCtrlNum; i++) {
		GetWindowRect(m_ptWndCtrlInfo[i].hCtrl, &r);
		p.x = r.left, p.y = r.top;
		ScreenToClient(m_hWnd, &p);
		makeAlignInfo(&m_ptWndCtrlInfo[i].tXInfo, p.x, r.right - r.left, m_tMinClientSize.x);
		makeAlignInfo(&m_ptWndCtrlInfo[i].tYInfo, p.y, r.bottom - r.top, m_tMinClientSize.y);
		m_ptWndCtrlInfo[i].bActived = TRUE;
	}
}

void CWndCtrlPos::onSize(int cx, int cy)
{
	int i,x,y,w,h;
	if (m_hWnd == NULL) return;
	for (i = 0; i < m_iCtrlNum; i++) {
		if (!m_ptWndCtrlInfo[i].bActived) continue;
		if (cx < m_tMinClientSize.x || cy < m_tMinClientSize.y) continue;
		translateAlignInfo(&m_ptWndCtrlInfo[i].tXInfo, cx, x, w);
		translateAlignInfo(&m_ptWndCtrlInfo[i].tYInfo, cy, y, h);
		SetWindowPos(m_ptWndCtrlInfo[i].hCtrl, NULL, x, y, w, h, SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void CWndCtrlPos::onGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_tMinWndSize.x) lpMMI->ptMinTrackSize = m_tMinWndSize;
}
