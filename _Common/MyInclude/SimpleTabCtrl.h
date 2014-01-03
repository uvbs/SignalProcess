#pragma once

#include "SimpleDraw.h"
#include "CTemplateDlg.h"

class CSimpleTabCtrl : public CDialog
{
	enum enumID
	{
		ID_RANGEFROM = 10000,
		ID_RANGETO = 20000,
	};
public:
	enum enumImageListDirection
	{
		ILD_LEFT2RIGHT,
		ILD_TOP2BOTTOM,
	};
	enum enumTabItemsDirection
	{
		TID_LEFT2RIGHT = 0x00000001,
		TID_TOP2BOTTOM = 0x00000002,
		TID_RIGHT2LEFT = 0x00000003,
		TID_BOTTOM2TOP = 0x00000004,

		TID_LEFT		= 0x00010000,
		TID_TOP			= 0x00020000,
		TID_RIGHT		= 0x00030000,
		TID_BOTTOM		= 0x00040000,

		TID_SINGLEROW	= 0x10000000,

		TID_MASK_DIRECTION	= 0x0000000F,
		TID_MASK_SIDE	= 0x000F0000,
	};

	enum enumSubImageIndex
	{
		SII_NORMAL		=0X00,
		SII_OVER,
		SII_SELECTED,
		SII_FOCUSED,

		SII_COUNT,
	};

	enum enumControlOwner
	{
		ControlOwner_Same,//AddTab中加入的控件和本tab具有相同的父窗口
		ControlOwner_This,//AddTab中加入的控件是本tab的子窗口
		ControlOwner_Top,//AddTab中加入的控件是顶层窗口
	};
public:
	CSimpleTabCtrl();
	~CSimpleTabCtrl();

public:
	void Create(CWnd* parent, const CRect& rc, DWORD tabDirection);
	void SetImageList(CBitmap* pBitmap, int iSmallWidth, int iSmallHeight, enumImageListDirection mainImageDirection, int subImageIndexNormal, int subImageIndexMouseOver, int subImageIndexSelected, int subImageIndexFocused);
	int AddTab(LPCTSTR text, int itemImageIndex, CWnd* pControl, bool bVisible, bool bUsingScreenPos = false);
	int AddTab(LPCTSTR text, CBitmap* itemImage, CWnd* pControl, bool bVisible, bool bUsingScreenPos = false);
	int AddTab(LPCTSTR text, int itemImageIndex, CWnd* pControl, bool bVisible, enumControlOwner ControlOwner, int vi = -1);//vi:TID_SINGLEROW时指定条目的高度或者宽度
	int AddTab(LPCTSTR text, CBitmap* itemImage, CWnd* pControl, bool bVisible, enumControlOwner ControlOwner, int vi = -1);//vi:TID_SINGLEROW时指定条目的高度或者宽度
	int AddControl(int itemIndex, CWnd* pControl);
	void ShowTab(int itemIndex, bool bVisible, bool bRedraw = true);
	void SetCurSel(int itemIndex);
	int GetCurSel() const{return CurSel;}
	int GetItemCount() const{return Items.GetCount();}
	void RefreshPosition();

protected:
	void GetTabClient(RECT& rc) const;
	UINT GetItemID();
	virtual void OnActivating(long index, bool bActive){}
	virtual void OnActivated(long index, bool bActive){}
	virtual void OnPositionRefreshed(){}
	virtual bool CanChange(long indexFrom, long indexTo){return true;}
	virtual bool CanSelect(long index){return true;}
	virtual bool CanUnselect(long index){return true;}
	CSimpleButton* GetHeadButton(long index) const{if(Items.GetCount()<=index) return NULL; return Items.GetAt(index)->tabItem;}

protected:
	DECLARE_DYNAMIC(CSimpleTabCtrl)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCommandRange(UINT id);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG*   pMsg)
	{
		if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
		{
			switch(pMsg->wParam)
			{
			case VK_CANCEL:return TRUE;
			case VK_RETURN:if(pMsg->message == WM_KEYDOWN) NextDlgCtrl();return TRUE;
			}
		} 
		return CDialog::PreTranslateMessage(pMsg);
	}
	afx_msg void OnOK(){}
	afx_msg void OnCancel(){}

protected:
	UINT m_nNextID;
	UINT m_nRows;
	CDlgTemplateBuilder m_builder;

public:
	struct structItem
	{
		int vi;
		CSimpleButton* tabItem;
		CArray<CWnd*, CWnd*> Controls;
		bool bVisible;
		enumControlOwner ControlOwner;
	};
	CWnd* Parent;
	CRect Rect;
	DWORD TabItemsDirection;
	CBitmap* ImageList;
	int SmallWidth, SmallHeight;
	enumImageListDirection ImageListDirection;
	int SubImageIndexes[SII_COUNT];
	CArray<structItem*, structItem*> Items;
	int CurSel;
	CRect RectHeadButtonOffset;//按钮四边缩进
};
