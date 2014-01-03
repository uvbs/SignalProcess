
#include "stdafx.h"
#include "CSimpleAniButton.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAnimation_Bitmap
///////////////////////////////////////////////////////////////////////////////////////////////
void CSimpleAnimation_Bitmap::AddIndex(long row, long col)
{
	m_indexes.Add(CPoint(row, col));
}
void CSimpleAnimation_Bitmap::AddRows(long col, long rowFrom, long rowTo)
{
	for(long i=rowFrom; i<=rowTo; i++) m_indexes.Add(CPoint(i, col));
}
void CSimpleAnimation_Bitmap::AddColumns(long row, long colFrom, long colTo)
{
	for(long i=colFrom; i<=colTo; i++) m_indexes.Add(CPoint(row, i));
}
void CSimpleAnimation_Bitmap::InsertIndex(long row, long col, long index)
{
	m_indexes.InsertAt(index, CPoint(row, col));
}
void CSimpleAnimation_Bitmap::InsertRows(long col, long rowFrom, long rowTo, long index)
{
	for(long i=rowFrom; i<=rowTo; i++) m_indexes.InsertAt(index+i-rowFrom,CPoint(i, col));
}
void CSimpleAnimation_Bitmap::InsertColumns(long row, long colFrom, long colTo, long index)
{
	for(long i=colFrom; i<=colTo; i++) m_indexes.InsertAt(index+i-colFrom,CPoint(row, i));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAnimation_ImageList
///////////////////////////////////////////////////////////////////////////////////////////////
void CSimpleAnimation_ImageList::AddIndex(long imageIndex)
{
	m_indexes.Add(imageIndex);
}
void CSimpleAnimation_ImageList::AddIndexes(long imageIndexFrom, long imageIndexTo)
{
	for(long i=imageIndexFrom; i<=imageIndexTo; i++) m_indexes.Add(i);
}
void CSimpleAnimation_ImageList::InsertIndex(long imageIndex, long index)
{
	m_indexes.InsertAt(index,imageIndex);
}
void CSimpleAnimation_ImageList::InsertIndexes(long imageIndexFrom, long imageIndexTo, long index)
{
	for(long i=imageIndexFrom; i<=imageIndexTo; i++) m_indexes.InsertAt(index+i-imageIndexFrom,i);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAnimation_Bitmaps
///////////////////////////////////////////////////////////////////////////////////////////////
void CSimpleAnimation_Bitmaps::SetBitmaps(const CArray<CBitmap*, CBitmap*>* pBitmaps)
{
	m_bitmaps.RemoveAll();
	m_bitmaps.Append(*pBitmaps);
}
void CSimpleAnimation_Bitmaps::AppendBitmap(CBitmap* pBitmap)
{
	m_bitmaps.Add(pBitmap);
}
void CSimpleAnimation_Bitmaps::InsertBitmap(CBitmap* pBitmap, long lIndex)
{
	m_bitmaps.InsertAt(lIndex, pBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAnimation_Files
///////////////////////////////////////////////////////////////////////////////////////////////
void CSimpleAnimation_Files::SetFiles(const CArray<CString, CString&>* pFiles)
{
	m_files.RemoveAll();
	m_files.Append(*pFiles);
}
void CSimpleAnimation_Files::AppendFile(LPCTSTR file)
{
	m_files.Add(CString(file));
}
void CSimpleAnimation_Files::InsertFile(LPCTSTR file, long lIndex)
{
	m_files.InsertAt(lIndex, CString(file));
}
void CSimpleAnimation_Files::AppendFiles(LPCTSTR format, long from, long to)
{
	for(long i=from; i<=to; i++)
	{
		CString t;
		t.Format(format, i);
		m_files.Add(t);
	}
}
void CSimpleAnimation_Files::InsertFiles(LPCTSTR format, long from, long to, long lIndex)
{
	for(long i=from; i<=to; i++)
	{
		CString t;
		t.Format(format, i);
		m_files.InsertAt(lIndex+i-from, t);
	}
}
void CSimpleAnimation_Files::AppendFiles(LPCTSTR format, TCHAR from, TCHAR to)
{
	for(TCHAR i=from; i<=to; i++)
	{
		CString t;
		t.Format(format, i);
		m_files.Add(t);
	}
}
void CSimpleAnimation_Files::InsertFiles(LPCTSTR format, TCHAR from, TCHAR to, long lIndex)
{
	for(TCHAR i=from; i<=to; i++)
	{
		CString t;
		t.Format(format, i);
		m_files.InsertAt(lIndex+i-from, t);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAniButton
///////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSimpleAniButton, CButton)

CSimpleAniButton::CSimpleAniButton():CSimpleDrawDC(NULL),m_dwCurrentSwitch(0),m_dwSwitches(0),m_pSwitches(NULL),m_bUsingSwitchText(false)
{
	m_ToolTip.m_hWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSimpleAniButton, CButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CSimpleAniButton 消息处理程序
void CSimpleAniButton::PreSubclassWindow() 
{
	ModifyStyle(0, BS_OWNERDRAW);
	m_pSimpleDrawDC = GetDC();
	CButton::PreSubclassWindow();
}
BOOL CSimpleAniButton::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	if(pMsg->message == WM_MOUSEMOVE) m_ToolTip.RelayEvent(pMsg);

	return CButton::PreTranslateMessage(pMsg);
}

void CSimpleAniButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//从lpDrawItemStruct获取控件的相关信息
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc(lpDrawItemStruct->rcItem);
	//获取按钮的状态
	structSwitchItem* pSwitch = GetSwitch(m_dwCurrentSwitch);
	if(!pSwitch) return;
	CString strSwitch;
	if(m_bUsingSwitchText == false)
	{
		CString text;
		GetWindowText(text);
		strSwitch = pSwitch->Text.Text;
		pSwitch->Text.Text = text;
	}
	this->Draw(&EraseGround, &DrawType_EG, &BackGround, &DrawType_BG, NULL, &DrawType_FG,
		NULL, rc, pDC);
	//画动画图片
	CBitmap* pBitmap = NULL;
	CRect imageRect;
	switch (pSwitch->Animation.Type)
	{
	case CSimpleAnimation::TYPE_NONE:break;
	case CSimpleAnimation::TYPE_BITMAP:
		pBitmap = pSwitch->Animation.Bitmap.m_pBitmap;
		pSwitch->Animation.Bitmap.GetCurrentRect(imageRect);
		break;
	case CSimpleAnimation::TYPE_IMAGELIST:ASSERT(false);break;
	case CSimpleAnimation::TYPE_BITMAPS:
		pBitmap = pSwitch->Animation.Bitmaps.GetCurrentBitmap();
		{
			BITMAP bm;
			pBitmap->GetBitmap(&bm);
			imageRect = CRect(0,0,bm.bmWidth,bm.bmHeight);
		}
		break;
	case CSimpleAnimation::TYPE_FILES:
		pBitmap = pSwitch->Animation.Files.GetCurrentBitmap();
		{
			BITMAP bm;
			pBitmap->GetBitmap(&bm);
			imageRect = CRect(0,0,bm.bmWidth,bm.bmHeight);
		}
		break;
	}
	DrawBitmap(pDC, rc, DrawType_FG, pBitmap, imageRect, pSwitch->Animation.Transparent, pSwitch->Animation.TransparentColor);
	DrawText(pSwitch->Text, rc, DrawType_FG, pDC);
	if(m_bUsingSwitchText == false) pSwitch->Text.Text = strSwitch;
}

void CSimpleAniButton::SetSwitchText(LPCTSTR text, DWORD dwSwitch)
{
	structSwitchItem* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch) pSwitch->Text.Text = text;
}
void CSimpleAniButton::SetSwitchTextDrawType(DWORD DrawType, DWORD dwSwitch)
{
	structSwitchItem* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch) pSwitch->Text.DrawType = DrawType;
}
void CSimpleAniButton::SetSwitchAnimateType(CSimpleAnimation::enumAnimationType type, DWORD dwSwitch)
{
	structSwitchItem* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch) pSwitch->Animation.Type = type;
}

CSimpleAniButton::structSwitchItem* CSimpleAniButton::GetSwitch(DWORD dwSwitch)
{
	if(dwSwitch == -1) dwSwitch = m_dwCurrentSwitch;
	if(dwSwitch == 0) return &DefaultForeGround;
	return (dwSwitch>m_dwSwitches)?NULL:&m_pSwitches[dwSwitch-1];
}
void CSimpleAniButton::SetSwitches(DWORD dwCount)
{
	ResetSwitch();
	m_dwSwitches = dwCount;
	if(m_dwSwitches)
	{
		m_pSwitches = new structSwitchItem[m_dwSwitches];
	}
}
void CSimpleAniButton::ResetSwitch()
{
	if(m_dwSwitches)
	{
		delete []m_pSwitches;
		m_pSwitches = NULL;
		m_dwSwitches = 0;
		m_dwCurrentSwitch = 0;
	}
}
void CSimpleAniButton::StopAnimateWith(DWORD index)
{
	KillTimer(TIMER_ID_ANIMATION);
	for(DWORD i=0; i<=m_dwSwitches; i++)
	{
		structSwitchItem* pItem = GetSwitch(i);
		if(!pItem) continue;
		pItem->Animation.ForceIndex(index);
	}
}
void CSimpleAniButton::EnsureAnimate()
{
	KillTimer(TIMER_ID_ANIMATION);
	structSwitchItem* pSwitch = GetSwitch(m_dwCurrentSwitch);
	if(!pSwitch) return;
	SetTimer(TIMER_ID_ANIMATION, pSwitch->Animation.Milliseconds, NULL);
}
void CSimpleAniButton::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case TIMER_ID_ANIMATION:
		{
			structSwitchItem* pSwitch = GetSwitch(m_dwCurrentSwitch);
			if(pSwitch)
			{
				pSwitch->Animation.OnFrameNext();
				Invalidate(false);
			}
		}
		break;
	}
	CButton::OnTimer(nIDEvent);
}

void CSimpleAniButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_BALLOON|TTS_NOPREFIX);
		m_ToolTip.Activate(FALSE);
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
		//m_ToolTip.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}
void CSimpleAniButton::SetToolTip(LPCTSTR tooltip,BOOL bActivate)
{
	if(tooltip == NULL) return;
	InitToolTip();
	if(m_ToolTip.GetToolCount()<1)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, tooltip, rectBtn, 1);
	}
	m_ToolTip.UpdateTipText(tooltip, this, 1);
	m_ToolTip.Activate(bActivate);
}
void CSimpleAniButton::ActivateTooltip(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount()<1) return;
	m_ToolTip.Activate(bActivate);
}