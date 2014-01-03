
#include "stdafx.h"
#include "SimpleDraw.h"

#pragma comment( lib, "Msimg32.lib" )

void CSimpleDrawType::GetResult(CRect& rcResult, CRect& rcResultImage, const CRect& rcSource, const CRect& rcImage) const
{
	rcResult = CRect(0,0,0,0);
	for(int i=0; i<ORDER_COUNT; i++)
	{
		switch(Orders[i])
		{
		case TYPE_NONE:
			break;
		case TYPE_SOURCE:
			rcResult = rcSource;
			break;
		case TYPE_OFFSET:
			if((DrawType_Offset & OFFSET_LEFT) == OFFSET_LEFT) rcResult.left += rcOffset.left;
			if((DrawType_Offset & OFFSET_TOP) == OFFSET_TOP) rcResult.top += rcOffset.top;
			if((DrawType_Offset & OFFSET_RIGHT) == OFFSET_RIGHT) rcResult.right -= rcOffset.right;
			if((DrawType_Offset & OFFSET_BOTTOM) == OFFSET_BOTTOM) rcResult.bottom -= rcOffset.bottom;
			break;
		case TYPE_BASIC:
			{
				int drawW = 0,drawH = 0;//绘图区域
				if((DrawType_Basic & CSimpleDrawType::LEFT) || (DrawType_Basic & CSimpleDrawType::RIGHT) || (DrawType_Basic & CSimpleDrawType::CENTER) || (DrawType_Basic & CSimpleDrawType::FITX)) drawW = rcImage.Width();
				else if((DrawType_Basic & CSimpleDrawType::STRETCHX) || (DrawType_Basic & CSimpleDrawType::REPEATX)) drawW = rcSource.Width();
				if((DrawType_Basic & CSimpleDrawType::TOP) || (DrawType_Basic & CSimpleDrawType::BOTTOM) || (DrawType_Basic & CSimpleDrawType::VCENTER) || (DrawType_Basic & CSimpleDrawType::FITY)) drawH = rcImage.Height();
				else if((DrawType_Basic & CSimpleDrawType::STRETCHY) || (DrawType_Basic & CSimpleDrawType::REPEATY)) drawH = rcSource.Height();

				//左上角坐标
				if((DrawType_Basic & CSimpleDrawType::LEFT) || (DrawType_Basic & CSimpleDrawType::REPEATX) || (DrawType_Basic & CSimpleDrawType::STRETCHX) || (DrawType_Basic & CSimpleDrawType::FITX)) rcResult.left = 0;
				else if(DrawType_Basic & CSimpleDrawType::RIGHT) rcResult.left = rcSource.Width() - drawW;
				else if(DrawType_Basic & CSimpleDrawType::CENTER) rcResult.left = (rcSource.Width() - drawW)/2;
				if((DrawType_Basic & CSimpleDrawType::TOP) || (DrawType_Basic & CSimpleDrawType::REPEATY) || (DrawType_Basic & CSimpleDrawType::STRETCHY) || (DrawType_Basic & CSimpleDrawType::FITY)) rcResult.top = 0;
				else if(DrawType_Basic & CSimpleDrawType::BOTTOM) rcResult.top = rcSource.Height() - drawH;
				else if(DrawType_Basic & CSimpleDrawType::VCENTER) rcResult.top = (rcSource.Height() - drawH)/2;

				rcResult.left += rcSource.left;
				rcResult.top += rcSource.top;
				rcResult.right = rcResult.left + drawW;
				rcResult.bottom = rcResult.top + drawH;
			}
			break;
		}
	}
}

void CSimpleDrawDC::Draw(const CSimpleElement& element)
{
	Draw(element, m_pSimpleDrawDC);
}
void CSimpleDrawDC::Draw(const CSimpleElement& element, CDC* pSimpleDrawDC)
{
	if(!pSimpleDrawDC) return;
	CDC dc;
	if(!dc.CreateCompatibleDC(pSimpleDrawDC)) return;
	CBitmap bmp;
	const CRect& rc = element.Rect;
	if(!bmp.CreateCompatibleBitmap(pSimpleDrawDC,rc.Width(),rc.Height())) return;
	CBitmap* pOldBitmap = dc.SelectObject(&bmp);

	DrawGround(element.EraseGround, element.Rect, element.DrawType, &dc);
	DrawGround(element.BackGround, element.Rect, element.DrawType, &dc);
	DrawGround(element.ForeGround, element.Rect, element.DrawType, &dc);
	DrawText(element.Text, element.Rect, element.DrawType, &dc);

	pSimpleDrawDC->BitBlt(rc.left,rc.top,rc.Width(),rc.Height(),&dc,0,0,SRCCOPY);

	dc.SelectObject(pOldBitmap);
}
void CSimpleDrawDC::Draw(const CSimpleGround* EraseGround, const CSimpleDrawType* DrawType_EG,
						 const CSimpleGround* BackGround, const CSimpleDrawType* DrawType_BG,
						 const CSimpleGround* ForeGround, const CSimpleDrawType* DrawType_FG,
						 const CSimpleText* Text, const CRect& Rect, CDC* pSimpleDrawDC)
{
	if(!pSimpleDrawDC) return;
	CDC dc;
	if(!dc.CreateCompatibleDC(pSimpleDrawDC)) return;
	CBitmap bmp;
	if(!bmp.CreateCompatibleBitmap(pSimpleDrawDC,Rect.Width(),Rect.Height())) return;
	CBitmap* pOldBitmap = dc.SelectObject(&bmp);

	if(EraseGround && DrawType_EG) DrawGround(*EraseGround, Rect, *DrawType_EG, &dc);
	if(BackGround && DrawType_BG) DrawGround(*BackGround, Rect, *DrawType_BG, &dc);
	if(ForeGround && DrawType_FG) DrawGround(*ForeGround, Rect, *DrawType_FG, &dc);
	if(Text && DrawType_FG) DrawText(*Text, Rect, *DrawType_FG, &dc);

	pSimpleDrawDC->BitBlt(Rect.left,Rect.top,Rect.Width(),Rect.Height(),&dc,0,0,SRCCOPY);

	dc.SelectObject(pOldBitmap);
}
void CSimpleDrawDC::DrawEx(const CSimpleGround* EraseGround, const CSimpleDrawType* DrawType_EG,
						 const CSimpleGround* BackGround, const CSimpleDrawType* DrawType_BG,
						 const CSimpleGround* ForeGround, const CSimpleDrawType* DrawType_FG,
						 const CRect& GroundRect,
						 const CSimpleText* Text,  const CRect& TextRect,
						 CDC* pSimpleDrawDC)
{
	if(!pSimpleDrawDC) return;
	CDC dc;
	if(!dc.CreateCompatibleDC(pSimpleDrawDC)) return;
	CBitmap bmp;
	if(!bmp.CreateCompatibleBitmap(pSimpleDrawDC,GroundRect.Width(),GroundRect.Height())) return;
	CBitmap* pOldBitmap = dc.SelectObject(&bmp);

	if(EraseGround && DrawType_EG) DrawGround(*EraseGround, GroundRect, *DrawType_EG, &dc);
	if(BackGround && DrawType_BG) DrawGround(*BackGround, GroundRect, *DrawType_BG, &dc);
	if(ForeGround && DrawType_FG) DrawGround(*ForeGround, GroundRect, *DrawType_FG, &dc);
	if(Text && DrawType_FG) DrawText(*Text, TextRect, *DrawType_FG, &dc);

	pSimpleDrawDC->BitBlt(GroundRect.left,GroundRect.top,GroundRect.Width(),GroundRect.Height(),&dc,0,0,SRCCOPY);

	dc.SelectObject(pOldBitmap);
}
void CSimpleDrawDC::DrawLine(CDC* pDC, COLORREF color, int lineWidth, int style, int x1, int y1, int x2, int y2)
{
	if(lineWidth < 1) return;
	CPen pen;
	pen.CreatePen(style, lineWidth, color);
	pDC->MoveTo(x1, y1);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->LineTo(x2, y2);
	pDC->SelectObject(pOldPen);
}
void CSimpleDrawDC::DrawGround(const CSimpleGround& Ground, const CRect& Rect, const CSimpleDrawType& DrawType, CDC* pDC)
{
	if(!pDC) return;
	CRect rc(Rect);
	if(Ground.Border.Left.Width && Ground.Border.Left.BorderOffset) rc.left += Ground.Border.Left.Width;
	if(Ground.Border.Top.Width && Ground.Border.Top.BorderOffset) rc.top += Ground.Border.Top.Width;
	if(Ground.Border.Right.Width && Ground.Border.Right.BorderOffset) rc.right -= Ground.Border.Right.Width;
	if(Ground.Border.Bottom.Width && Ground.Border.Bottom.BorderOffset) rc.bottom -= Ground.Border.Bottom.Width;
	DrawLine(pDC, Ground.Border.Left.Color, Ground.Border.Left.Width, PS_SOLID, Rect.left, Rect.top, Rect.left, Rect.bottom);
	DrawLine(pDC, Ground.Border.Top.Color, Ground.Border.Top.Width, PS_SOLID, Rect.left, Rect.top, Rect.right, Rect.top);
	DrawLine(pDC, Ground.Border.Right.Color, Ground.Border.Right.Width, PS_SOLID,  Rect.right - Ground.Border.Right.Width, Rect.top, Rect.right - Ground.Border.Right.Width, Rect.bottom);
	DrawLine(pDC, Ground.Border.Bottom.Color, Ground.Border.Bottom.Width, PS_SOLID, Rect.right, Rect.bottom - Ground.Border.Bottom.Width, Rect.left, Rect.bottom - Ground.Border.Bottom.Width);

	switch(Ground.Type)
	{
	case CSimpleGround::TYPE_NONE:break;
	case CSimpleGround::TYPE_COLOR:
		EraseRect(pDC, rc, Ground.Color);
		break;
	case CSimpleGround::TYPE_COLORS:
		if(Ground.Colors.Count < 1) break;
		switch(Ground.Colors.Type)
		{
		case CSimpleGround::CLT_ROW:
			{
				int h = rc.Height()/Ground.Colors.Count;
				CRect rcTemp(rc);
				for (DWORD i=0; i<Ground.Colors.Count-1; i++)
				{
					rcTemp.bottom = rcTemp.top + h;
					EraseRect(pDC, rcTemp, Ground.Colors.Colors[i]);
					rcTemp.top = rcTemp.bottom;
				}
				rcTemp.bottom = rc.bottom;
				EraseRect(pDC, rcTemp, Ground.Colors.Colors[Ground.Colors.Count-1]);
			}
			break;
		case CSimpleGround::CLT_COLUMN:
			{
				int w = rc.Width()/Ground.Colors.Count;
				CRect rcTemp(rc);
				for (DWORD i=0; i<Ground.Colors.Count-1; i++)
				{
					rcTemp.right = rcTemp.left + w;
					EraseRect(pDC, rcTemp, Ground.Colors.Colors[i]);
					rcTemp.left = rcTemp.right;
				}
				rcTemp.right = rc.right;
				EraseRect(pDC, rcTemp, Ground.Colors.Colors[Ground.Colors.Count-1]);
			}
			break;
		}
		break;
	case CSimpleGround::TYPE_IMAGE:
		DrawBitmap(pDC, rc, DrawType, Ground.Image.Bitmap, Ground.Image.Rect, Ground.Image.Transparent, Ground.Image.TransparentColor);
		break;
	}
}
void CSimpleDrawDC::DrawText(const CSimpleText& Text, const CRect& Rect, const CSimpleDrawType& DrawType, CDC* pDC)
{
	if(!pDC || Text.Text.IsEmpty()) return;

	if(Text.Font.Transparent) pDC->SetBkMode(TRANSPARENT);
	else
	{
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(Text.Font.BkColor);
	}
	CFont* pFont = Text.DefaultFont;
	CFont font;
	if(!pFont)
	{
		pFont = &font;
		font.CreateFont(
			Text.Font.Size,       // nHeight
			0,                         // nWidth
			Text.Font.Escapement, // nEscapement
			Text.Font.Orientation,// nOrientation
			Text.Font.Bold?FW_BOLD:FW_NORMAL,     // nWeight
			Text.Font.Italic,                     // bItalic
			Text.Font.Underline,  // bUnderline
			Text.Font.StrikeOut,  // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			Text.Font.Name);      // lpszFacename
	}
	CFont* pOldFont = pDC->SelectObject(pFont);
	pDC->SetTextColor(Text.Font.Color);
	CRect _rc(Rect);
	/*if(!(Text.DrawType & DT_SINGLELINE))
	{
	//计算区域
	pDC->DrawText(Text.Text, _rc, Text.DrawType|DT_CALCRECT);
	}//*/
	CRect rcTemp;
	Text.GetResult(rcTemp, _rc, pDC);
	pDC->DrawText(Text.Text, rcTemp, Text.DrawType);
	pDC->SelectObject(pOldFont);
}

void CSimpleDrawDC::DrawRepeatX(CDC* pDC, int t, int h, int l, int dcW, CBitmap* pBitmap, const CRect& rcImage)
{
	CDC dc;
	if(!dc.CreateCompatibleDC(pDC)) return;
	CBitmap* pOldBitmap = dc.SelectObject(pBitmap);
	pDC->SetStretchBltMode(COLORONCOLOR);
	//画整图
	int count = (int)(dcW/rcImage.Width());
	for(int i=0; i<count; i++)
	{
		pDC->StretchBlt(l+rcImage.Width()*i,t,rcImage.Width(),h,&dc,rcImage.left,rcImage.top,rcImage.Width(),rcImage.Height(),SRCCOPY);
	}
	//画剩余部分
	int w = dcW - rcImage.Width()*count;
	if(w > 0) pDC->StretchBlt(l+rcImage.Width()*count,t,w,h,&dc,rcImage.left,rcImage.top,w,rcImage.Height(),SRCCOPY);
	dc.SelectObject(pOldBitmap);
}
void CSimpleDrawDC::DrawRepeatY(CDC* pDC, int l, int w, int t, int dcH, CBitmap* pBitmap, const CRect& rcImage)
{
	CDC dc;
	if(!dc.CreateCompatibleDC(pDC)) return;
	CBitmap* pOldBitmap = dc.SelectObject(pBitmap);
	pDC->SetStretchBltMode(COLORONCOLOR);
	//画整图
	int count = (int)(dcH/rcImage.Height());
	for(int i=0; i<count; i++)
	{
		pDC->StretchBlt(l,t+rcImage.Height()*i,w,rcImage.Height(),&dc,rcImage.left,rcImage.top,rcImage.Width(),rcImage.Height(),SRCCOPY);
	}
	//画剩余部分
	int h = dcH - rcImage.Height()*count;
	if(h > 0) pDC->StretchBlt(l,t+rcImage.Height()*count,w,h,&dc,rcImage.left,rcImage.top,rcImage.Width(),h,SRCCOPY);
	dc.SelectObject(pOldBitmap);
}
void CSimpleDrawDC::DrawRepeatXY(CDC* pDC, const CRect& rc, CBitmap* pBitmap, const CRect& rcImage)
{
	//画整图
	int count = (int)(rc.Height()/rcImage.Height());
	for(int i=0; i<count; i++)
	{
		DrawRepeatX(pDC, rc.top+rcImage.Height()*i, rcImage.Height(), rc.left, rc.Width(), pBitmap, rcImage);
	}
	//画剩余部分
	int h = rc.Height() - rcImage.Height()*count;
	CRect _rcImage(rcImage);
	_rcImage.bottom = _rcImage.top + h;
	if(h > 0) DrawRepeatX(pDC, rc.top+rcImage.Height()*count, h, rc.left, rc.Width(), pBitmap, _rcImage);
}
void CSimpleDrawDC::DrawBitmap(CDC* pDC, const CRect& _rcDC, const CSimpleDrawType& _drawType, CBitmap* pBitmap, const CRect& _rcImage, bool bTransparent, COLORREF crTransparent)
{
	if(!pBitmap || !pDC) return;
	BITMAP bm;
	pBitmap->GetBitmap(&bm);
	CRect rcImage = _rcImage;
	if(rcImage.left < 0) rcImage.left = 0;
	if(rcImage.top < 0) rcImage.top = 0;
	if(rcImage.right < 0) rcImage.right = bm.bmWidth;
	if(rcImage.bottom < 0) rcImage.bottom = bm.bmHeight;

	CRect rcImageCopy(rcImage),rcDraw;
	_drawType.GetResult(rcDraw, rcImage, _rcDC, rcImageCopy);
	if(rcDraw.Width() == 0 || rcDraw.Height() == 0) return;

	/*
	if((drawType & CSimpleGround::FITX) || (drawType & CSimpleGround::FITX))
	{
	//改变大小
	}//*/
	DWORD drawType = _drawType.GetBasic();
	if((drawType & CSimpleDrawType::REPEATX) || (drawType & CSimpleDrawType::REPEATY))
	{
		if ((drawType & CSimpleDrawType::REPEATX) && (drawType & CSimpleDrawType::REPEATY)) CSimpleDrawDC::DrawRepeatXY(pDC, _rcDC, pBitmap, rcImage);
		else if(drawType & CSimpleDrawType::REPEATX) CSimpleDrawDC::DrawRepeatX(pDC, rcDraw.top, rcDraw.Height(), _rcDC.left, _rcDC.Width(), pBitmap, rcImage);
		else if(drawType & CSimpleDrawType::REPEATY) CSimpleDrawDC::DrawRepeatY(pDC, rcDraw.left, rcDraw.Width(), _rcDC.top, _rcDC.Height(), pBitmap, rcImage);
	}
	else
	{
		CDC dc;
		if(!dc.CreateCompatibleDC(pDC)) return;
		CBitmap*oldBitmap=dc.SelectObject(pBitmap);
		pDC->SetStretchBltMode(COLORONCOLOR);
		if(bTransparent) ::TransparentBlt(pDC->GetSafeHdc(),rcDraw.left,rcDraw.top,rcDraw.Width(),rcDraw.Height(),dc.GetSafeHdc(),rcImage.left,rcImage.top,rcImage.Width(),rcImage.Height(),crTransparent);
		else pDC->StretchBlt(rcDraw.left,rcDraw.top,rcDraw.Width(),rcDraw.Height(),&dc,rcImage.left,rcImage.top,rcImage.Width(),rcImage.Height(),SRCCOPY);
		dc.SelectObject(oldBitmap);
	}
}

void CSimpleDrawDC::EraseRect(CDC* pDC, const CRect& rc, COLORREF color)
{
	pDC->FillSolidRect(rc, color);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleButton
///////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSimpleButton, CButton)

#ifndef CSimpleButton_MultiInstance
	CPen CSimpleButton::m_BoundryPen;
	CPen CSimpleButton::m_InsideBoundryPenLeft;
	CPen CSimpleButton::m_InsideBoundryPenRight;
	CPen CSimpleButton::m_InsideBoundryPenTop;
	CPen CSimpleButton::m_InsideBoundryPenBottom;
	CBrush CSimpleButton::m_FillActive;
	CBrush CSimpleButton::m_FillInactive;
	CPen CSimpleButton::m_InsideBoundryPenLeftSel;
	CPen CSimpleButton::m_InsideBoundryPenTopSel;
	CPen CSimpleButton::m_InsideBoundryPenRightSel;
	CPen CSimpleButton::m_InsideBoundryPenBottomSel;
	BOOL CSimpleButton::m_bOver;
	BOOL CSimpleButton::m_bSelected;
	BOOL CSimpleButton::m_bFocus;
#endif

CSimpleButton::CSimpleButton():CSimpleDrawDC(NULL),m_bTracking(false),m_bDefautUI(true)//,m_bAuto3D(false)
,m_dwCurrentSwitch(0),m_dwSwitches(0),m_ppSwitches(NULL),XPButton(false),m_bDrawCheckBox(true),m_bDrawRadio(true)
{
	memset(m_pbStatus, 0, sizeof(bool)*CBS_COUNT);
	memset(m_pbUsingStatusText, 0, sizeof(bool)*CBS_COUNT);
	m_ToolTip.m_hWnd = NULL;
#ifndef CSimpleButton_MultiInstance
	static bool bFirst = true;
	if(bFirst)
	{
		bFirst = false;
#endif
		m_BoundryPen.CreatePen(PS_INSIDEFRAME | PS_SOLID, 1, RGB(0, 0, 0));
		m_InsideBoundryPenLeft.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(250, 196, 88)); 
		m_InsideBoundryPenRight.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(251, 202, 106));
		m_InsideBoundryPenTop.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(252, 210, 121));
		m_InsideBoundryPenBottom.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(229, 151, 0));

		m_FillActive.CreateSolidBrush(RGB(223, 222, 236));
		m_FillInactive.CreateSolidBrush(RGB(222, 223, 236));

		m_InsideBoundryPenLeftSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(153, 198, 252)); 
		m_InsideBoundryPenTopSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(162, 201, 255));
		m_InsideBoundryPenRightSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(162, 189, 252));
		m_InsideBoundryPenBottomSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(162, 201, 255));
#ifndef CSimpleButton_MultiInstance
	}
#endif
}

BEGIN_MESSAGE_MAP(CSimpleButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CSimpleButton 消息处理程序
void CSimpleButton::PreSubclassWindow() 
{
	DWORD style = GetStyle();
	switch(style & BS_TYPEMASK)
	{
	case BS_AUTOCHECKBOX:
	case BS_CHECKBOX:m_enumType = Type_CheckBox;break;
	case BS_AUTORADIOBUTTON:
	case BS_RADIOBUTTON:m_enumType = Type_Radio;break;
	case BS_AUTO3STATE:
	case BS_3STATE:m_enumType = Type_3State;break;
	case BS_GROUPBOX:m_enumType = Type_GroupBox;break;
	default:m_enumType = Type_Normal;
	}
	if(!m_bDefautUI) ModifyStyle(0, BS_OWNERDRAW);
	m_pSimpleDrawDC = GetDC();
	CButton::PreSubclassWindow();
}

void CSimpleButton::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
}
void CSimpleButton::OnMove(int x, int y) 
{
	CButton::OnMove(x, y);
}
void CSimpleButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_pbStatus[CBS_PRESSED] = true;
	if(!m_pbStatus[CBS_DISABLED]) InvalidateRect(NULL, FALSE);
	CButton::OnLButtonDown(nFlags,point);
}
void CSimpleButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_pbStatus[CBS_PRESSED] = false;
	if(!m_pbStatus[CBS_DISABLED]) InvalidateRect(NULL, FALSE);
	CButton::OnLButtonUp(nFlags,point);
}
void CSimpleButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking=_TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags,point);
}
LRESULT CSimpleButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_pbStatus[CBS_OVER] = false;
	if(GetFocus() == this) ::SetFocus(NULL);
	else
	{
		m_pbStatus[CBS_PRESSED] = false;
		m_pbStatus[CBS_FOCUS] = false;
	}
	m_bTracking=FALSE;
	if(!m_pbStatus[CBS_DISABLED]) InvalidateRect(NULL, FALSE);
	return 0;
}
LRESULT CSimpleButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	bool bOldOver = m_pbStatus[CBS_OVER];
	m_pbStatus[CBS_OVER] = true;
	if(!bOldOver && !m_pbStatus[CBS_DISABLED]) InvalidateRect(NULL, FALSE);
	return 0;
}
BOOL CSimpleButton::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	if(pMsg->message == WM_MOUSEMOVE) m_ToolTip.RelayEvent(pMsg);

	return CButton::PreTranslateMessage(pMsg);
}
//绘制按钮的底色
void CSimpleButton::DoGradientFill(CDC *pDC, CRect* rect)
{
	CBrush brBk[64];
	int nWidth = rect->Width();	
	int nHeight = rect->Height();
	CRect rct;

	for (int i = 0; i < 64; i ++)
	{
		if (m_bOver)
		{
			if (m_bFocus)
				brBk[i].CreateSolidBrush(RGB(255 - (i / 4), 255 - (i / 4), 255 - (i / 3)));
			else
				brBk[i].CreateSolidBrush(RGB(255 - (i / 4), 255 - (i / 4), 255 - (i / 5)));
		}
		else
		{
			if (m_bFocus)
				brBk[i].CreateSolidBrush(RGB(255 - (i / 3), 255 - (i / 3), 255 - (i / 4)));
			else
				brBk[i].CreateSolidBrush(RGB(255 - (i / 3), 255 - (i / 3), 255 - (i / 5)));
		}
	}

	for (int i = rect->top; i <= nHeight + 2; i ++) 
	{
		rct.SetRect(rect->left, i, nWidth + 2, i + 1);
		pDC->FillRect(&rct, &brBk[((i * 63) / nHeight)]);
	}

	for (int i = 0; i < 64; i ++) brBk[i].DeleteObject();
}
//绘制按钮的内边框
void CSimpleButton::DrawInsideBorder(CDC *pDC, CRect* rect)
{
	CPen *pLeft, *pRight, *pTop, *pBottom;

	if (m_bSelected && !m_bOver)
	{
		pLeft = & m_InsideBoundryPenLeftSel;
		pRight = &m_InsideBoundryPenRightSel;
		pTop = &m_InsideBoundryPenTopSel;
		pBottom = &m_InsideBoundryPenBottomSel;
	}
	else
	{
		pLeft = &m_InsideBoundryPenLeft;
		pRight = &m_InsideBoundryPenRight;
		pTop = &m_InsideBoundryPenTop;
		pBottom = &m_InsideBoundryPenBottom;
	}

	CPoint oldPoint = pDC->MoveTo(rect->left, rect->bottom - 1);
	CPen* pOldPen = pDC->SelectObject(pLeft);
	pDC->LineTo(rect->left, rect->top + 1);
	pDC->SelectObject(pRight);
	pDC->MoveTo(rect->right - 1, rect->bottom - 1);
	pDC->LineTo(rect->right - 1, rect->top);
	pDC->SelectObject(pTop);
	pDC->MoveTo(rect->left - 1, rect->top);
	pDC->LineTo(rect->right - 1, rect->top);
	pDC->SelectObject(pBottom);
	pDC->MoveTo(rect->left, rect->bottom);
	pDC->LineTo(rect->right - 1, rect->bottom);
	pDC->SelectObject(pOldPen);
	pDC->MoveTo(oldPoint);

	if (m_bSelected && !m_bOver) DrawFocusRect(pDC->m_hDC,rect);
}
void CSimpleButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(m_bDefautUI)
	{
		CButton::DrawItem(lpDrawItemStruct);
		return;
	}
	//从lpDrawItemStruct获取控件的相关信息
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc(lpDrawItemStruct->rcItem);
	//获取按钮的状态
	m_pbStatus[CBS_DISABLED] = (lpDrawItemStruct->itemState & ODS_DISABLED) ? true : false;
	m_pbStatus[CBS_FOCUS] = ((lpDrawItemStruct->itemState & ODS_FOCUS) ||( lpDrawItemStruct->itemState & ODS_SELECTED) || (lpDrawItemStruct->itemState & ODS_DEFAULT)) ? true : false;

	enumCustomButtonStatus cbs;

	if(m_pbStatus[CBS_DISABLED]) cbs=CBS_DISABLED;
	else if(m_pbStatus[CBS_PRESSED]) cbs=CBS_PRESSED;
	else if(m_pbStatus[CBS_OVER]) cbs=CBS_OVER;
	else if(m_pbStatus[CBS_FOCUS]) cbs=CBS_FOCUS;
	else cbs=CBS_NORMAL;
	//

	structStatus* pStatus = GetSwitch(m_dwCurrentSwitch);
	if(!pStatus) return;

	if(m_pbUsingStatusText[cbs] == false)
	{
		CString text;
		GetWindowText(text);
		pStatus[cbs].Text.Text = text;
	}
	if(XPButton)
	{
		CDC dc;
		CBitmap bmp;
		if(dc.CreateCompatibleDC(pDC) && bmp.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height()))
		{
			CBitmap* pOldBitmap = dc.SelectObject(&bmp);

			DrawGround(EraseGround, rc, DrawType_EG, &dc);
			//DrawXPButton Begin
			UINT state = lpDrawItemStruct->itemState;
			//获取按钮的状态
			if (state & ODS_FOCUS)
			{
				m_bFocus = TRUE;
				m_bSelected = TRUE;
			}
			else
			{
				m_bFocus = FALSE;
				m_bSelected = FALSE;
			}
			if (state & ODS_SELECTED || state & ODS_DEFAULT)
			{
				m_bFocus = TRUE;
			}
			m_bOver = cbs == CBS_OVER;
			//画按钮的外边框，它是一个半径为5的圆角矩形
			CPen* hOldPen = dc.SelectObject(&m_BoundryPen);
			POINT pt;
			pt.x = pt.y = 5;
			dc.RoundRect(&rc, pt);
			dc.SelectObject(hOldPen);

			CRect rcTemp(rc);
			rcTemp.DeflateRect(CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

			//根据按钮的状态填充按钮的底色
			CBrush* pOldBrush;
			if (m_bOver)
			{
				pOldBrush = dc.SelectObject(&m_FillActive);
				DoGradientFill(&dc, &rcTemp);
			}
			else
			{
				pOldBrush =dc.SelectObject(&m_FillInactive);
				DoGradientFill(&dc, &rcTemp);
			}

			//根据按钮的状态绘制内边框
			if (m_bOver || m_bSelected) DrawInsideBorder(&dc, &rcTemp);

			dc.SelectObject(pOldBrush);
			//DrawXPButton End
			DrawGround(pStatus[cbs].ForeGround, rc, DrawType_FG, &dc);
			DrawText(pStatus[cbs].Text, rc, DrawType_FG, &dc);

			pDC->BitBlt(rc.left,rc.top,rc.Width(),rc.Height(),&dc,0,0,SRCCOPY);

			dc.SelectObject(pOldBitmap);
		}
	}
	else
	{/*
		if(m_enumType == Type_CheckBox || m_enumType == Type_Radio || m_enumType == Type_3State)
		{
			CRect rcPrefix = rc;
			rcPrefix.right = rc.left + rc.Height();
			CRect rcText = rc;
			rcText.left = rcPrefix.right + 3;
			DrawEx(&EraseGround, &DrawType_EG, &BackGround, &DrawType_BG, &pStatus[cbs].ForeGround, &DrawType_FG, rc,
				&pStatus[cbs].Text, rcText, pDC);
			if(m_enumType == Type_CheckBox)
			{
				//DoGradientFill(pDC, &rcPrefix);
				switch(GetCheck())
				{
				case BST_CHECKED:
					break;
				case BST_UNCHECKED:
				case BST_INDETERMINATE://不支持，当作BST_UNCHECKED处理
					break;
				}
			}
			else if(m_enumType == Type_Radio) 
			{
				pDC->Ellipse(rcPrefix);
				switch(GetCheck())
				{
				case BST_CHECKED:
					{
						CRect rcDot = rcPrefix;
						int s = -rcPrefix.Width()/4;
						rcDot.InflateRect(s,s,s,s);

						CPen pen(PS_SOLID, 1, (COLORREF)NULL);
						CPen* pOldPen = pDC->SelectObject(&pen);
						CBrush brush((COLORREF)NULL);
						CBrush* pOldBrush = pDC->SelectObject(&brush);
						pDC->Ellipse(rcDot);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
					break;
				case BST_UNCHECKED:
				case BST_INDETERMINATE://不支持，当作BST_UNCHECKED处理
					break;
				}
			}
			else if(m_enumType == Type_3State)//不支持
			{
				ASSERT(false);
				switch(GetCheck())
				{
				case BST_CHECKED:
				case BST_UNCHECKED:
				case BST_INDETERMINATE://不支持，当作BST_UNCHECKED处理
				}
			}
		}
		else //*/
			Draw(&EraseGround, &DrawType_EG, &BackGround, &DrawType_BG, &pStatus[cbs].ForeGround, &DrawType_FG,
			&pStatus[cbs].Text, rc, pDC);
	}
}
BOOL CSimpleButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
BOOL CSimpleButton::OnClicked() 
{
	return FALSE;
}

void CSimpleButton::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CButton::OnRButtonUp(nFlags, point);
}

void CSimpleButton::SetText(LPCTSTR text)
{
	SetSwitchText(text, 0);
}
void CSimpleButton::SetBitmap(CBitmap* Normal, CBitmap* Disabled, CBitmap* Focus, CBitmap* Over, CBitmap* Pressed)
{
	SetSwitchBitmap(Normal, Disabled, Focus, Over, Pressed, 0);
}
void CSimpleButton::SetColor(COLORREF Normal, COLORREF Disabled, COLORREF Focus, COLORREF Over, COLORREF Pressed)
{
	SetSwitchColor(Normal, Disabled, Focus, Over, Pressed, 0);
}
void CSimpleButton::SetTextDrawType(DWORD Normal, DWORD Disabled, DWORD Focus, DWORD Over, DWORD Pressed)
{
	SetSwitchTextDrawType(Normal, Disabled, Focus, Over, Pressed, 0);
}
void CSimpleButton::SetTextOffset(int l, int t, int r, int b)
{
	SetSwitchTextOffset(l, t, r, b, 0);
}
void CSimpleButton::SetOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
								 const CPoint& indexNormal, const CPoint& indexDisabled, const CPoint& indexFocus, const CPoint& indexOver, const CPoint& indexPressed)
{
	SetOneBitmap(pBitmap, smallWidth, smallHeight,
		indexNormal.x, indexNormal.y,
		indexDisabled.x, indexDisabled.y,
		indexFocus.x, indexFocus.y,
		indexOver.x, indexOver.y,
		indexPressed.x, indexPressed.y);
}
void CSimpleButton::SetOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
								 UINT XNormal, UINT YNormal, 
								 UINT XDisabled, INT YDisabled, 
								 UINT XFocus, UINT YFocus, 
								 UINT XOver, UINT YOver, 
								 UINT XPressed, UINT YPressed)
{

	SetSwitchOneBitmap(pBitmap, smallWidth, smallHeight,
		XNormal, YNormal, 
		XDisabled, YDisabled, 
		XFocus, YFocus, 
		XOver, YOver, 
		XPressed, YPressed, 0);
}

void CSimpleButton::SetSwitchText(LPCTSTR text, DWORD dwSwitch)
{
	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;
	for(DWORD i=0; i<CBS_COUNT; i++) pStatus[i].Text.Text = text;
}
void CSimpleButton::SetSwitchBitmap(CBitmap* Normal, CBitmap* Disabled, CBitmap* Focus, CBitmap* Over, CBitmap* Pressed, DWORD dwSwitch)
{
	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;
	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Type = CSimpleGround::TYPE_IMAGE;
	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Image.Bitmap = Normal;

	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Type = CSimpleGround::TYPE_IMAGE;
	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Image.Bitmap = Disabled;

	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Type = CSimpleGround::TYPE_IMAGE;
	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Image.Bitmap = Focus;

	pStatus[CSimpleButton::CBS_OVER].ForeGround.Type = CSimpleGround::TYPE_IMAGE;
	pStatus[CSimpleButton::CBS_OVER].ForeGround.Image.Bitmap = Over;

	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Type = CSimpleGround::TYPE_IMAGE;
	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Image.Bitmap = Pressed;
}
void CSimpleButton::SetSwitchColor(COLORREF Normal, COLORREF Disabled, COLORREF Focus, COLORREF Over, COLORREF Pressed, DWORD dwSwitch)
{
	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;
	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Type = CSimpleGround::TYPE_COLOR;
	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Color = Normal;

	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Type = CSimpleGround::TYPE_COLOR;
	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Color = Disabled;

	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Type = CSimpleGround::TYPE_COLOR;
	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Color = Focus;

	pStatus[CSimpleButton::CBS_OVER].ForeGround.Type = CSimpleGround::TYPE_COLOR;
	pStatus[CSimpleButton::CBS_OVER].ForeGround.Color = Over;

	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Type = CSimpleGround::TYPE_COLOR;
	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Color = Pressed;
}
void CSimpleButton::SetSwitchTextDrawType(DWORD Normal, DWORD Disabled, DWORD Focus, DWORD Over, DWORD Pressed, DWORD dwSwitch)
{
	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;
	pStatus[CSimpleButton::CBS_NORMAL].Text.DrawType = Normal;

	pStatus[CSimpleButton::CBS_DISABLED].Text.DrawType = Disabled;

	pStatus[CSimpleButton::CBS_FOCUS].Text.DrawType = Focus;

	pStatus[CSimpleButton::CBS_OVER].Text.DrawType = Over;

	pStatus[CSimpleButton::CBS_PRESSED].Text.DrawType = Pressed;
}
void CSimpleButton::SetSwitchTextOffset(int l, int t, int r, int b, DWORD dwSwitch)
{
	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;
	pStatus[CSimpleButton::CBS_NORMAL].Text.SetOffset(CSimpleText::OFFSET_ALL, l, t, r, b);

	pStatus[CSimpleButton::CBS_DISABLED].Text.SetOffset(CSimpleText::OFFSET_ALL, l, t, r, b);

	pStatus[CSimpleButton::CBS_FOCUS].Text.SetOffset(CSimpleText::OFFSET_ALL, l, t, r, b);

	pStatus[CSimpleButton::CBS_OVER].Text.SetOffset(CSimpleText::OFFSET_ALL, l, t, r, b);

	pStatus[CSimpleButton::CBS_PRESSED].Text.SetOffset(CSimpleText::OFFSET_ALL, l, t, r, b);
}
void CSimpleButton::SetSwitchOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
									   const CPoint& indexNormal, const CPoint& indexDisabled, const CPoint& indexFocus, const CPoint& indexOver, const CPoint& indexPressed, DWORD dwSwitch)
{
	SetSwitchOneBitmap(pBitmap, smallWidth, smallHeight,
		indexNormal.x, indexNormal.y,
		indexDisabled.x, indexDisabled.y,
		indexFocus.x, indexFocus.y,
		indexOver.x, indexOver.y,
		indexPressed.x, indexPressed.y, dwSwitch);
}
void CSimpleButton::SetSwitchOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
									   UINT XNormal, UINT YNormal, 
									   UINT XDisabled, INT YDisabled, 
									   UINT XFocus, UINT YFocus, 
									   UINT XOver, UINT YOver, 
									   UINT XPressed, UINT YPressed, DWORD dwSwitch)
{
	SetSwitchBitmap(pBitmap, pBitmap, pBitmap, pBitmap, pBitmap, dwSwitch);

	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;

	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Image.Rect = CRect(smallWidth*XNormal,smallHeight*YNormal,smallWidth*(XNormal+1),smallHeight*(YNormal+1));

	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Image.Rect = CRect(smallWidth*XDisabled,smallHeight*YDisabled,smallWidth*(XDisabled+1),smallHeight*(YDisabled+1));

	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Image.Rect = CRect(smallWidth*XFocus,smallHeight*YFocus,smallWidth*(XFocus+1),smallHeight*(YFocus+1));

	pStatus[CSimpleButton::CBS_OVER].ForeGround.Image.Rect = CRect(smallWidth*XOver,smallHeight*YOver,smallWidth*(XOver+1),smallHeight*(YOver+1));

	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Image.Rect = CRect(smallWidth*XPressed,smallHeight*YPressed,smallWidth*(XPressed+1),smallHeight*(YPressed+1));
}
void CSimpleButton::SetSwitchTransparent(bool bTransparent, COLORREF crTransparent, DWORD dwSwitch)
{
	structStatus* pStatus = GetSwitch(dwSwitch);
	if(!pStatus) return;
	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Image.Transparent = bTransparent;
	pStatus[CSimpleButton::CBS_NORMAL].ForeGround.Image.TransparentColor = crTransparent;
	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Image.Transparent = bTransparent;
	pStatus[CSimpleButton::CBS_DISABLED].ForeGround.Image.TransparentColor = crTransparent;
	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Image.Transparent = bTransparent;
	pStatus[CSimpleButton::CBS_FOCUS].ForeGround.Image.TransparentColor = crTransparent;
	pStatus[CSimpleButton::CBS_OVER].ForeGround.Image.Transparent = bTransparent;
	pStatus[CSimpleButton::CBS_OVER].ForeGround.Image.TransparentColor = crTransparent;
	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Image.Transparent = bTransparent;
	pStatus[CSimpleButton::CBS_PRESSED].ForeGround.Image.TransparentColor = crTransparent;
}
CSimpleButton::structStatus* CSimpleButton::GetSwitch(DWORD dwSwitch)
{
	if(dwSwitch == -1) dwSwitch = m_dwCurrentSwitch;
	if(dwSwitch == 0) return Status;
	return (dwSwitch>m_dwSwitches)?NULL:m_ppSwitches[dwSwitch-1];
}
void CSimpleButton::SetSwitches(DWORD dwCount)
{
	ResetSwitch();
	m_dwSwitches = dwCount;
	if(m_dwSwitches)
	{
		m_ppSwitches = new structStatusPointer[m_dwSwitches];
		for(DWORD i=0; i<m_dwSwitches; i++) m_ppSwitches[i] = new structStatus[CBS_COUNT];
	}
}
void CSimpleButton::ResetSwitch()
{
	if(m_dwSwitches)
	{
		for(DWORD i=0; i<m_dwSwitches; i++) delete []m_ppSwitches[i];
		delete []m_ppSwitches;
		m_ppSwitches = NULL;
		m_dwSwitches = 0;
		m_dwCurrentSwitch = 0;
	}
}

void CSimpleButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_BALLOON|TTS_NOPREFIX);
		m_ToolTip.Activate(FALSE);
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
		//m_ToolTip.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		m_ToolTip.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}
void CSimpleButton::SetToolTip(LPCTSTR tooltip,BOOL bActivate)
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
void CSimpleButton::ActivateTooltip(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount()<1) return;
	m_ToolTip.Activate(bActivate);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CSIMPLEEDIT_TIMERID_DELAY		1

CSimpleEdit::CSimpleEdit():UsingCustomUI(true),m_OnContentChanged(NULL)
{
	m_bDisabled=FALSE;
	m_bReadOnly=FALSE;
	m_bFocused=FALSE;
	//
	SetTextFont(NULL);
	SetTextBkInfo(TRUE);
	SetTextColor(RGB(0,0,0),RGB(0,0,0),GetSysColor(COLOR_GRAYTEXT),GetSysColor(COLOR_GRAYTEXT));
	SetBkColor(RGB(255,255,255),RGB(255,255,255),GetSysColor(COLOR_BTNFACE),GetSysColor(COLOR_BTNFACE));
}
CSimpleEdit::~CSimpleEdit()
{
}

BEGIN_MESSAGE_MAP(CSimpleEdit, CEdit)
	//{{AFX_MSG_MAP(CSimpleEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_CHANGE, &CSimpleEdit::OnEnChange)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleEdit message handlers
void CSimpleEdit::SetTextFont(CFont*font,BOOL bRepaint)
{
	m_pTextFont=font;
	if(font) SetFont(font,FALSE);
	if(bRepaint) Invalidate();
}
////////////
void CSimpleEdit::EnableEdit(BOOL bEnable,BOOL bRepaint)
{
	m_bDisabled=!bEnable;
	EnableWindow(bEnable);
	if(bRepaint) Invalidate();
}
void CSimpleEdit::SetTextBkInfo(BOOL bTransparent,
								COLORREF crNormal,
								COLORREF crFocused,
								COLORREF crReadOnly,
								COLORREF crDisabled,
								BOOL bRepaint)
{
	m_bTransparent=bTransparent;
	m_crTextBkColor[CEI_Normal] = crNormal;
	m_crTextBkColor[CEI_Focused] = crFocused;
	m_crTextBkColor[CEI_ReadOnly] = crReadOnly;
	m_crTextBkColor[CEI_Disabled] = crDisabled;
	if(bRepaint) Invalidate();
}
void CSimpleEdit::SetTextColor(COLORREF crNormal,COLORREF crFocused,
							   COLORREF crReadOnly,COLORREF crDisabled,
							   BOOL bRepaint)
{
	m_crTextColor[CEI_Normal] = crNormal;
	m_crTextColor[CEI_Focused] = crFocused;
	m_crTextColor[CEI_ReadOnly] = crReadOnly;
	m_crTextColor[CEI_Disabled] = crDisabled;
	if(bRepaint) Invalidate();
}
void CSimpleEdit::SetBkColor(COLORREF crNormal,COLORREF crFocused,
							 COLORREF crReadOnly,COLORREF crDisabled,
							 BOOL bRepaint)
{
	//*
	m_crBkColor[CEI_Normal]=crNormal;
	m_crBkColor[CEI_Focused]=crFocused;
	m_crBkColor[CEI_ReadOnly]=crReadOnly;
	m_crBkColor[CEI_Disabled]=crDisabled;
	//*/
	m_BrushBk[CEI_Normal].DeleteObject();
	m_BrushBk[CEI_Focused].DeleteObject();
	m_BrushBk[CEI_ReadOnly].DeleteObject();
	m_BrushBk[CEI_Disabled].DeleteObject();
	m_BrushBk[CEI_Normal].CreateSolidBrush(crNormal);
	m_BrushBk[CEI_Focused].CreateSolidBrush(crFocused);
	m_BrushBk[CEI_ReadOnly].CreateSolidBrush(crReadOnly);
	m_BrushBk[CEI_Disabled].CreateSolidBrush(crDisabled);
	if(bRepaint) Invalidate();
}

BYTE CSimpleEdit::GetStatus()
{
	//GetStatus here
	DWORD dwStyle=GetStyle();
	BYTE nIndex=0;
	if(dwStyle & WS_DISABLED) nIndex=CEI_Disabled;
	else if(dwStyle & ES_READONLY) nIndex=CEI_ReadOnly;
	else
	{
		if(m_bDisabled) nIndex=CEI_Disabled;
		else if(m_bReadOnly) nIndex=CEI_ReadOnly;
		else if(m_bFocused) nIndex=CEI_Focused;
		else nIndex=CEI_Normal;
	}
	return nIndex;
}
void CSimpleEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	m_bFocused=TRUE;
	Invalidate();
}

HBRUSH CSimpleEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if(UsingCustomUI)
	{
		BYTE nIndex=GetStatus();
		pDC->SetTextColor(m_crTextColor[nIndex]);
		if(m_bTransparent) pDC->SetBkMode(TRANSPARENT);
		else pDC->SetBkColor(m_crTextBkColor[nIndex]);

		return (HBRUSH)m_BrushBk[nIndex];
	}
	else return NULL;
}

void CSimpleEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	m_bFocused=FALSE;
	Invalidate();
}

void CSimpleEdit::OnEnChange()
{
	if(m_OnContentChanged)
	{
		CString s;
		GetWindowText(s);
		if(s == m_oldText) return;
		m_oldText = s;
		KillTimer(CSIMPLEEDIT_TIMERID_DELAY);
		if(m_dwWaitingMilliseconds) SetTimer(CSIMPLEEDIT_TIMERID_DELAY, m_dwWaitingMilliseconds, NULL);
		else m_OnContentChanged(this, m_dwExtraData_OnContentChanged, m_dwFlagData_OnContentChanged);
	}
}
void CSimpleEdit::OnTimer(UINT timerID)
{
	if(timerID == CSIMPLEEDIT_TIMERID_DELAY)
	{
		KillTimer(CSIMPLEEDIT_TIMERID_DELAY);
		if(m_OnContentChanged) m_OnContentChanged(this, m_dwExtraData_OnContentChanged, m_dwFlagData_OnContentChanged);
	}
	CEdit::OnTimer(timerID);
}

/////////////////////////////////////////////////////////////////////////////
// CSimpleEdit2
/////////////////////////////////////////////////////////////////////////////
CSimpleEdit2::CSimpleEdit2():Switches(NULL),dwSwitches(NULL),OnContentChanged(NULL)
{
	SetSwitches(1);
	SetTextFont(0, NULL);
	SetTextBkInfo(0, TRUE);
	SetTextColor(0, RGB(0,0,0));
	SetBkInfo(0, false, RGB(255,255,255));
	SetCurrentSwitch(0, false);
}
CSimpleEdit2::~CSimpleEdit2()
{
	if(Switches) delete []Switches;
	Switches = NULL;
	dwSwitches = NULL;
}

BEGIN_MESSAGE_MAP(CSimpleEdit2, CEdit)
	//{{AFX_MSG_MAP(CSimpleEdit2)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_CHANGE, &CSimpleEdit2::OnEnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleEdit2 message handlers
void CSimpleEdit2::SetSwitches(DWORD _dwSwitches)
{
	structSwitch* pSwitch = new structSwitch[_dwSwitches];
	DWORD n = min(_dwSwitches, dwSwitches);
	for(DWORD i=0; i<n; i++)
	{
		pSwitch[i].m_pTextFont = Switches[i].m_pTextFont;
		pSwitch[i].m_bTextBkTransparent = Switches[i].m_bTextBkTransparent;
		pSwitch[i].m_crTextColor = Switches[i].m_crTextColor;
		pSwitch[i].m_crTextBkColor = Switches[i].m_crTextBkColor;
		pSwitch[i].m_bBkTransparent = Switches[i].m_bBkTransparent;
		pSwitch[i].m_crBkColor = Switches[i].m_crBkColor;
		pSwitch[i].m_BrushBk.DeleteObject();
		pSwitch[i].m_BrushBk.CreateSolidBrush(pSwitch[i].m_crBkColor);
	}
	delete []Switches;
	dwSwitches = _dwSwitches;
	Switches = pSwitch;
}
void CSimpleEdit2::SetCurrentSwitch(DWORD dwSwitch, bool bRepaint)
{
	if(dwSwitch >= dwSwitches) return;
	dwCurrentSwitch = dwSwitch;
	structSwitch* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch) if(pSwitch->m_pTextFont) SetFont(pSwitch->m_pTextFont,FALSE);
	if(bRepaint) RedrawWindow();
}
bool CSimpleEdit2::SetCurrentEditSwitch(DWORD dwSwitch)
{
	if(dwSwitch >= dwSwitches) return false;
	dwCurrentEditSwitch = dwSwitch;
	return true;
}

void CSimpleEdit2::SetTextFont(DWORD dwSwitch, CFont*font,BOOL bRepaint)
{
	structSwitch* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch)
	{
		pSwitch->m_pTextFont=font;
		if(bRepaint) RedrawWindow();
	}
}

void CSimpleEdit2::SetTextBkInfo(DWORD dwSwitch, BOOL bTransparent, COLORREF cr, BOOL bRepaint)
{
	structSwitch* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch)
	{
		pSwitch->m_bTextBkTransparent=bTransparent;
		pSwitch->m_crTextBkColor = cr;
		if(bRepaint) RedrawWindow();
	}
}
void CSimpleEdit2::SetTextColor(DWORD dwSwitch, COLORREF cr, BOOL bRepaint)
{
	structSwitch* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch)
	{
		pSwitch->m_crTextColor = cr;
		if(bRepaint) RedrawWindow();
	}
}
void CSimpleEdit2::SetBkInfo(DWORD dwSwitch, BOOL bTransparent, COLORREF cr, BOOL bRepaint)
{
	structSwitch* pSwitch = GetSwitch(dwSwitch);
	if(pSwitch)
	{
		pSwitch->m_bBkTransparent=bTransparent;
		pSwitch->m_crBkColor = cr;
		pSwitch->m_BrushBk.DeleteObject();
		pSwitch->m_BrushBk.CreateSolidBrush(pSwitch->m_crBkColor);
		if(bRepaint) RedrawWindow();
	}
}

HBRUSH CSimpleEdit2::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	structSwitch* pSwitch = GetSwitch(-1, false);
	if(pSwitch == NULL) return (HBRUSH)GetStockObject(NULL_BRUSH);
	pDC->SetTextColor(pSwitch->m_crTextColor);
	if(pSwitch->m_bTextBkTransparent) pDC->SetBkMode(TRANSPARENT);
	else pDC->SetBkColor(pSwitch->m_crTextBkColor);

	if(pSwitch->m_bBkTransparent) return (HBRUSH)GetStockObject(NULL_BRUSH);
	else return (HBRUSH)pSwitch->m_BrushBk;
}
void CSimpleEdit2::OnEnChange()
{
	if(OnContentChanged) OnContentChanged(this, dwExtraData_OnContentChanged, dwFlagData_OnContentChanged);
}
///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleDialog
///////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSimpleDialog, CDialog)

BEGIN_MESSAGE_MAP(CSimpleDialog, CDialog)
	ON_WM_EXITSIZEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CSimpleDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CSimpleDialog::OnExitSizeMove()
{
	Invalidate(true);
	CDialog::OnExitSizeMove();
}

BOOL CSimpleDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	DrawGround(EraseBkgnd, rc, DrawType_EG, pDC);
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// CSimplePropertyPage
///////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSimplePropertyPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CSimplePropertyPage, CPropertyPage)
	ON_WM_EXITSIZEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CSimplePropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;
}

void CSimplePropertyPage::OnExitSizeMove()
{
	Invalidate(true);
	CPropertyPage::OnExitSizeMove();
}

BOOL CSimplePropertyPage::OnEraseBkgnd(CDC* pDC)
{
	if(UsingCustomUI)
	{
		CRect rc;
		GetClientRect(rc);
		DrawGround(EraseBkgnd, rc, DrawType_EG, pDC);
		return TRUE;
	}
	else return CPropertyPage::OnEraseBkgnd(pDC);
}



///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleStatic
///////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSimpleStatic, CStatic)

BEGIN_MESSAGE_MAP(CSimpleStatic, CStatic)
END_MESSAGE_MAP()


void CSimpleStatic::PreSubclassWindow() 
{
	if(UsingCustomUI) ModifyStyle(0, SS_OWNERDRAW);
	m_pSimpleDrawDC = GetDC();
	CStatic::PreSubclassWindow();
}

void CSimpleStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(!UsingCustomUI)
	{
		CStatic::DrawItem(lpDrawItemStruct);
		return;
	}
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc(lpDrawItemStruct->rcItem);
	GetWindowText(Text.Text);
	Draw(&EraseBkgnd, &DrawType_EG,
		NULL, NULL,
		&ForeGround, &DrawType_FG,
		&Text, rc, pDC);
}



/////////////////////////////////////////////////////////////////////////////
// CSimpleStatic2
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSimpleStatic2, CStatic)

CSimpleStatic2::CSimpleStatic2()
{
	SetTextFont(NULL);
	SetTextBkInfo(TRUE);
	SetTextColor(RGB(0,0,0));
	SetBkInfo(false, GetSysColor(COLOR_BTNFACE));
}
CSimpleStatic2::~CSimpleStatic2()
{
}

BEGIN_MESSAGE_MAP(CSimpleStatic2, CStatic)
	//{{AFX_MSG_MAP(CSimpleStatic2)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleStatic2 message handlers
void CSimpleStatic2::SetTextFont(CFont*font,BOOL bRepaint)
{
	m_pTextFont=font;
	if(font) SetFont(font,FALSE);
	if(bRepaint) Invalidate();
}

void CSimpleStatic2::SetTextBkInfo(BOOL bTransparent, COLORREF cr, BOOL bRepaint)
{
	m_bTextBkTransparent=bTransparent;
	m_crTextBkColor = cr;
	if(bRepaint) Invalidate();
}
void CSimpleStatic2::SetTextColor(COLORREF cr, BOOL bRepaint)
{
	m_crTextColor = cr;
	if(bRepaint) Invalidate();
}
void CSimpleStatic2::SetBkInfo(BOOL bTransparent, COLORREF cr, BOOL bRepaint)
{
	m_bBkTransparent=bTransparent;
	m_crBkColor = cr;
	m_BrushBk.DeleteObject();
	m_BrushBk.CreateSolidBrush(m_crBkColor);
	if(bRepaint) Invalidate();
}

HBRUSH CSimpleStatic2::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_crTextColor);
	if(m_bTextBkTransparent) pDC->SetBkMode(TRANSPARENT);
	else pDC->SetBkColor(m_crTextBkColor);

	if(m_bBkTransparent) return (HBRUSH)GetStockObject(NULL_BRUSH);
	else return (HBRUSH)m_BrushBk;
}


/////////////////////////////////////////////////////////////////////////////
// CSimpleGroupBox
/////////////////////////////////////////////////////////////////////////////
CSimpleGroupBox::CSimpleGroupBox()
{
	SetTextFont(NULL);
	SetTextBkInfo(TRUE);
	SetTextColor(RGB(0,0,0));
	SetBkInfo(false, GetSysColor(COLOR_BTNFACE));
}
CSimpleGroupBox::~CSimpleGroupBox()
{
}

BEGIN_MESSAGE_MAP(CSimpleGroupBox, CButton)
	//{{AFX_MSG_MAP(CSimpleGroupBox)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleGroupBox message handlers
void CSimpleGroupBox::SetTextFont(CFont*font,BOOL bRepaint)
{
	m_pTextFont=font;
	if(font) SetFont(font,FALSE);
	if(bRepaint) Invalidate();
}

void CSimpleGroupBox::SetTextBkInfo(BOOL bTransparent, COLORREF cr, BOOL bRepaint)
{
	m_bTextBkTransparent=bTransparent;
	m_crTextBkColor = cr;
	if(bRepaint) Invalidate();
}
void CSimpleGroupBox::SetTextColor(COLORREF cr, BOOL bRepaint)
{
	m_crTextColor = cr;
	if(bRepaint) Invalidate();
}
void CSimpleGroupBox::SetBkInfo(BOOL bTransparent, COLORREF cr, BOOL bRepaint)
{
	m_bBkTransparent=bTransparent;
	m_crBkColor = cr;
	m_BrushBk.DeleteObject();
	m_BrushBk.CreateSolidBrush(m_crBkColor);
	if(bRepaint) Invalidate();
}

HBRUSH CSimpleGroupBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_crTextColor);
	if(m_bTextBkTransparent) pDC->SetBkMode(TRANSPARENT);
	else pDC->SetBkColor(m_crTextBkColor);

	if(m_bBkTransparent) return (HBRUSH)GetStockObject(NULL_BRUSH);
	else return (HBRUSH)m_BrushBk;
}


/////////////////////////////////////////////////////////////////////////////
// CTipEdit
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTipEdit, CEdit)
	//{{AFX_MSG_MAP(CTipEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipEdit message handlers
void CTipEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	if(IsTipping()) SetWindowText(_T(""));
}

void CTipEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	CString curText;
	GetWindowText(curText);
	if(curText.IsEmpty()) SetWindowText(m_strTip);
}