#include "stdafx.h"
#include "SimpleTabCtrl.h"

CSimpleTabCtrl::CSimpleTabCtrl():ImageList(NULL),CurSel(-1),m_nNextID(ID_RANGEFROM),RectHeadButtonOffset(0,0,0,0)
{
}

CSimpleTabCtrl::~CSimpleTabCtrl()
{
	for(int i=0; i<Items.GetCount(); i++)
	{
		structItem* pItem = Items.GetAt(i);
		delete pItem->tabItem;
		delete pItem;
	}
}

void CSimpleTabCtrl::Create(CWnd* parent, const CRect& rc, DWORD tabDirection)
{
	Parent = parent;
	Rect = rc;
	TabItemsDirection = tabDirection;
	
	m_builder.SetDataLength(10240);
	m_builder.BeginTemplate(WS_CHILD|WS_VISIBLE|DS_CONTROL|WS_TABSTOP, &rc, _T(""));
	CreateIndirect(m_builder.GetTemplate(),Parent);
	MoveWindow(rc);
}

void CSimpleTabCtrl::SetImageList(CBitmap* pBitmap, int iSmallWidth, int iSmallHeight, enumImageListDirection mainImageDirection,
								  int subImageIndexNormal, int subImageIndexMouseOver, int subImageIndexSelected, int subImageIndexFocused)
{
	ImageList = pBitmap;
	SmallWidth = iSmallWidth;
	SmallHeight = iSmallHeight;
	ImageListDirection = mainImageDirection;
	SubImageIndexes[SII_NORMAL] = subImageIndexNormal;
	SubImageIndexes[SII_OVER] = subImageIndexMouseOver;
	SubImageIndexes[SII_SELECTED] = subImageIndexSelected;
	SubImageIndexes[SII_FOCUSED] = subImageIndexFocused;
}
int CSimpleTabCtrl::AddTab(LPCTSTR text, int itemImageIndex, CWnd* pControl, bool bVisible, bool bUsingScreenPos)
{
	return AddTab(text, itemImageIndex, pControl, bVisible, bUsingScreenPos?ControlOwner_Top:ControlOwner_Same);
}
int CSimpleTabCtrl::AddTab(LPCTSTR text, CBitmap* itemImage, CWnd* pControl, bool bVisible, bool bUsingScreenPos)
{
	return AddTab(text, itemImage, pControl, bVisible, bUsingScreenPos?ControlOwner_Top:ControlOwner_Same);
}
int CSimpleTabCtrl::AddTab(LPCTSTR text, int itemImageIndex, CWnd* pControl, bool bVisible, enumControlOwner ControlOwner, int vi)
{
	structItem* pItem = new structItem;
	pItem->bVisible = bVisible;
	pItem->ControlOwner = ControlOwner;
	pItem->vi = vi;
	pItem->tabItem = new CSimpleButton;
	switch(ImageListDirection)
	{
	case ILD_LEFT2RIGHT:
		pItem->tabItem->SetOneBitmap(ImageList, SmallWidth, SmallHeight,
			itemImageIndex, SubImageIndexes[SII_NORMAL], 
			itemImageIndex, SubImageIndexes[SII_SELECTED], 
			itemImageIndex, SubImageIndexes[SII_FOCUSED], 
			itemImageIndex, SubImageIndexes[SII_OVER], 
			itemImageIndex, SubImageIndexes[SII_NORMAL]);
		break;
	case ILD_TOP2BOTTOM:
		pItem->tabItem->SetOneBitmap(ImageList, SmallWidth, SmallHeight,
			SubImageIndexes[SII_NORMAL], itemImageIndex,
			SubImageIndexes[SII_SELECTED], itemImageIndex,
			SubImageIndexes[SII_FOCUSED], itemImageIndex,
			SubImageIndexes[SII_OVER], itemImageIndex,
			SubImageIndexes[SII_NORMAL], itemImageIndex);
		break;
	}

	pItem->tabItem->SetCustomButton(true);
	pItem->tabItem->DrawType_FG.SetBasic(CSimpleDrawType::STRETCHX|CSimpleDrawType::STRETCHY);
	DWORD textDT = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	pItem->tabItem->SetTextDrawType(textDT, textDT, textDT, textDT, textDT);
	pItem->tabItem->Create(text, WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_OWNERDRAW, CRect(0,0,0,0), this, GetItemID());

	int ret = Items.Add(pItem);
	AddControl(ret, pControl);
	if(bVisible) RefreshPosition();
	return ret;
}
int CSimpleTabCtrl::AddTab(LPCTSTR text, CBitmap* itemImage, CWnd* pControl, bool bVisible,enumControlOwner ControlOwner, int vi)
{
	structItem* pItem = new structItem;
	pItem->bVisible = bVisible;
	pItem->ControlOwner = ControlOwner;
	pItem->vi = vi;
	pItem->tabItem = new CSimpleButton;
	pItem->tabItem->SetBitmap(itemImage, itemImage, itemImage, itemImage, itemImage);

	pItem->tabItem->SetCustomButton(true);
	pItem->tabItem->DrawType_FG.SetBasic(CSimpleDrawType::STRETCHX|CSimpleDrawType::STRETCHY);
	DWORD textDT = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
	pItem->tabItem->SetTextDrawType(textDT, textDT, textDT, textDT, textDT);
	pItem->tabItem->Create(text, WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_OWNERDRAW, CRect(0,0,0,0), this, GetItemID());

	int ret = Items.Add(pItem);
	AddControl(ret, pControl);
	if(bVisible) RefreshPosition();
	return ret;
}
int CSimpleTabCtrl::AddControl(int itemIndex, CWnd* pControl)
{
	if(itemIndex >= Items.GetCount()) return -1;
	CArray<CWnd*, CWnd*>& ctrls = Items.GetAt(itemIndex)->Controls;
	if(pControl)
	{
		CRect rcClient;
		GetTabClient(rcClient);
		pControl->ShowWindow(SW_HIDE);
		pControl->MoveWindow(rcClient);
	}
	return ctrls.Add(pControl);
}
void CSimpleTabCtrl::ShowTab(int itemIndex, bool bVisible, bool bRedraw)
{
	if(itemIndex<0 || itemIndex>=Items.GetCount()) return;
	Items.GetAt(itemIndex)->bVisible = bVisible;
	if(bRedraw) RefreshPosition();
}
void CSimpleTabCtrl::SetCurSel(int itemIndex)
{
	if(!CanChange(CurSel, itemIndex)) return;
	if(CurSel>-1 && CurSel<Items.GetCount())
	{
		if(!CanUnselect(CurSel)) return;
		Items.GetAt(CurSel)->tabItem->EnableWindow(true);
		OnActivating(CurSel, false);
		//隐藏itemIndex对应的窗口
		CArray<CWnd*, CWnd*>& ctrls = Items.GetAt(CurSel)->Controls;
		for(int i=0; i<ctrls.GetCount(); i++)
		{
			CWnd* pWnd = ctrls.GetAt(i);
			if(pWnd) pWnd->ShowWindow(SW_HIDE);
		}
		OnActivated(CurSel, false);
	}
	if(CanSelect(itemIndex))
	{
		CurSel = itemIndex;
		if(CurSel>-1 && CurSel<Items.GetCount())
		{
			Items.GetAt(CurSel)->bVisible = true;
			Items.GetAt(CurSel)->tabItem->EnableWindow(false);
			OnActivating(CurSel, true);
			//显示itemIndex对应的窗口
			CArray<CWnd*, CWnd*>& ctrls = Items.GetAt(CurSel)->Controls;
			for(int i=0; i<ctrls.GetCount(); i++)
			{
				CWnd* pWnd = ctrls.GetAt(i);
				if(pWnd) pWnd->ShowWindow(SW_SHOW);
			}
			OnActivated(CurSel, true);
		}
	}
	RefreshPosition();
}

void CSimpleTabCtrl::GetTabClient(RECT& rc) const
{
	rc = Rect;
	switch(TabItemsDirection & TID_MASK_SIDE)
	{
	case TID_LEFT:rc.left += SmallWidth*m_nRows;break;
	case TID_TOP:rc.top += SmallHeight*m_nRows;break;
	case TID_RIGHT:rc.right -= SmallWidth*m_nRows;break;
	case TID_BOTTOM:rc.bottom -= SmallHeight*m_nRows;break;
	}
}
void CSimpleTabCtrl::RefreshPosition()
{
	CRect rc;
	GetClientRect(rc);
	if(RectHeadButtonOffset.left) rc.left += RectHeadButtonOffset.left;
	if(RectHeadButtonOffset.top) rc.top += RectHeadButtonOffset.top;
	if(RectHeadButtonOffset.right) rc.left -= RectHeadButtonOffset.right;
	if(RectHeadButtonOffset.bottom) rc.top -= RectHeadButtonOffset.bottom;

	if(TabItemsDirection & TID_SINGLEROW)
	{
		m_nRows = 1;//设置tab条目
		int vi = 0;
		for(int i=0; i<Items.GetCount(); i++)
		{
			structItem* pItem = Items.GetAt(i);
			if(!pItem->bVisible) continue;
			int x, y, w, h;
			switch(TabItemsDirection & TID_MASK_SIDE)
			{
			case TID_LEFT:
				w = SmallWidth;
				h = pItem->vi;
				x = rc.left;
				break;
			case TID_TOP:
				w = pItem->vi;
				h = SmallHeight;
				y = rc.top;
				break;
			case TID_RIGHT:
				w = SmallWidth;
				h = pItem->vi;
				x = rc.right - w;
				break;
			case TID_BOTTOM:
				w = pItem->vi;
				h = SmallHeight;
				y = rc.bottom - h;
				break;
			}
			switch(TabItemsDirection & TID_MASK_DIRECTION)
			{
			case TID_LEFT2RIGHT:x = rc.left + vi;break;
			case TID_TOP2BOTTOM:y = rc.top + vi;break;
			case TID_RIGHT2LEFT:x = rc.right - vi - w;break;
			case TID_BOTTOM2TOP:y = rc.bottom - vi - h;break;
			}
			pItem->tabItem->MoveWindow(x, y, w, h);
			vi += pItem->vi;
		}
	}
	else
	{
		DWORD Count = 0;
		DWORD CurSelCount = 0;
		for(int i=0; i<Items.GetCount(); i++)
		{
			if(!Items.GetAt(i)->bVisible) continue;
			if(CurSel>-1 && i<=CurSel) CurSelCount++;
			Count++;
		}
		int nCols = 0;
		switch(TabItemsDirection & TID_MASK_SIDE)
		{
		case TID_LEFT:
		case TID_RIGHT:nCols = rc.Height()/SmallHeight;break;
		case TID_TOP:
		case TID_BOTTOM:nCols = rc.Width()/SmallWidth;break;
		}
		if(nCols == 0) nCols = 1;
		m_nRows = Count/nCols;
		if(Count%nCols) m_nRows++;
		int icurselrow = CurSelCount/nCols;

		//设置tab条目
		int index = 0;
		for(int i=0; i<Items.GetCount(); i++)
		{
			if(!Items.GetAt(i)->bVisible) continue;
			int icol,irow;
			icol = index%nCols;
			irow = index/nCols;
			if(irow > icurselrow) irow--;
			else if(irow == icurselrow) irow = m_nRows-1;
			int x, y;
			switch(TabItemsDirection & TID_MASK_SIDE)
			{
			case TID_LEFT:x = rc.left + irow*SmallWidth;break;
			case TID_TOP:y = rc.top + irow*SmallHeight;break;
			case TID_RIGHT:x = rc.right - (irow+1)*SmallWidth;break;
			case TID_BOTTOM:y = rc.bottom - (irow+1)*SmallHeight;break;
			}
			switch(TabItemsDirection & TID_MASK_DIRECTION)
			{
			case TID_LEFT2RIGHT:x = rc.left + icol * SmallWidth;break;
			case TID_TOP2BOTTOM:y = rc.top + icol * SmallHeight;break;
			case TID_RIGHT2LEFT:x = rc.right - (icol + 1) * SmallWidth;break;
			case TID_BOTTOM2TOP:y = rc.bottom - (icol + 1) * SmallHeight;break;
			}

			Items.GetAt(i)->tabItem->MoveWindow(x, y, SmallWidth, SmallHeight);
			index++;
		}
	}
	//设置各tab条目对应的控件
	CRect rcSameClient;
	GetTabClient(rcSameClient);
	CRect rcScreen(rcSameClient);
	CRect rcThisClient(rcSameClient);
	if(Parent)
	{
		Parent->ClientToScreen(rcScreen);
		//rcThisClient = rcScreen;
		//ScreenToClient(rcThisClient);//本窗口没有边框和标题，默认的计算方法有错误
		rcThisClient = CRect(0,0,rcSameClient.Width(),rcSameClient.Height());
		switch(TabItemsDirection & TID_MASK_SIDE)
		{
		case TID_LEFT:rcThisClient.left += SmallWidth*m_nRows;break;
		case TID_TOP:rcThisClient.top += SmallHeight*m_nRows;break;
		case TID_RIGHT:rcThisClient.right -= SmallWidth*m_nRows;break;
		case TID_BOTTOM:rcThisClient.bottom -= SmallHeight*m_nRows;break;
		}
	}
	for(int i=0; i<Items.GetCount(); i++)
	{
		CArray<CWnd*, CWnd*>& ctrls = Items.GetAt(i)->Controls;
		for(int j=0; j<ctrls.GetCount(); j++)
		{
			CWnd* pWnd = ctrls.GetAt(j);
			if(pWnd)
			{
				switch(Items.GetAt(i)->ControlOwner)
				{
				case ControlOwner_Same:pWnd->MoveWindow(rcSameClient);break;
				case ControlOwner_This:pWnd->MoveWindow(rcThisClient);break;
				case ControlOwner_Top:pWnd->MoveWindow(rcScreen);break;
				}
			}
		}
	}
	OnPositionRefreshed();
}
UINT CSimpleTabCtrl::GetItemID()
{
	if(m_nNextID == -1) return 0;
	return m_nNextID++;
}
IMPLEMENT_DYNAMIC(CSimpleTabCtrl, CDialog)

BEGIN_MESSAGE_MAP(CSimpleTabCtrl, CDialog)
	ON_COMMAND_RANGE(CSimpleTabCtrl::ID_RANGEFROM, CSimpleTabCtrl::ID_RANGETO, &CSimpleTabCtrl::OnCommandRange)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CSimpleTabCtrl::OnCommandRange(UINT id)
{
	SetCurSel(id - ID_RANGEFROM);
}
void CSimpleTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	Rect.right = Rect.left + cx;
	Rect.bottom = Rect.top + cy;
	RefreshPosition();
}
BOOL CSimpleTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}