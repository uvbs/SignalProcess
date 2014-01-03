#pragma once

#define WND_CTRL_ALIGN_MASK			0x03
#define WND_CTRL_ALIGN_LEFT			0x00
#define WND_CTRL_ALIGN_TOP			WND_CTRL_ALIGN_LEFT
#define WND_CTRL_ALIGN_CENTER		0x01
#define WND_CTRL_ALIGN_MIDDLE		WND_CTRL_ALIGN_CENTER
#define WND_CTRL_ALIGN_RIGHT		0x02
#define WND_CTRL_ALIGN_BOTTOM		WND_CTRL_ALIGN_RIGHT
#define WND_CTRL_ALIGN_JUSTIFY		0x03
#define WND_CTRL_SIZE_PERCENT		0x10

typedef struct tagWndCtrlAlignInfo {
	DWORD dwFlags;
	int iMargin;
	int iMargin2;
	int iSize;
	int iMore;
}	WND_CTRL_ALIGN_INFO, *P_WND_CTRL_ALIGN_INFO;

typedef struct tagWndCtrlInfo {
	HWND hCtrl;
	BOOL bActived;
	WND_CTRL_ALIGN_INFO tXInfo;
	WND_CTRL_ALIGN_INFO tYInfo;
}	WND_CTRL_INFO, *P_WND_CTRL_INFO;

class CWndCtrlPos {
public:
	CWndCtrlPos();
	virtual ~CWndCtrlPos();
	void bindWnd(HWND hWnd);
	BOOL initCtrlNum(int iNum);
	BOOL addCtrl(HWND hCtrl, DWORD dwXFlags, DWORD dwYFlags, LPPOINT ptMore = NULL);
	BOOL addCtrlID(INT iID, DWORD dwXFlags, DWORD dwYFlags, LPPOINT ptMore = NULL);
	void keepWndSize(LPPOINT ptClientXY = NULL);
	void onSize(int cx, int cy);
	void onGetMinMaxInfo(MINMAXINFO* lpMMI);
private:
	HWND m_hWnd;
	POINT m_tMinWndSize;
	POINT m_tMinClientSize;
	P_WND_CTRL_INFO m_ptWndCtrlInfo;
	int m_iCtrlNum;
	int m_iCtrlMaxNum;
	static void makeAlignInfo(P_WND_CTRL_ALIGN_INFO pwcai, int iStart, int iSize, int iTotal);
	static void translateAlignInfo(P_WND_CTRL_ALIGN_INFO pwcai, int iTotal, int &iStart, int &iSize);
};
