// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axStaticEx.cpp
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang, John Xu, Cloud Ju
// [Version ]: R0.01
// [Release ]: 2004/01/29
// 
// [Contains]:
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   01/29/04  Leo     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "axStaticEx.h"
#include "TP_Core.h"
#include "IDCardCollMainDlg.h"

//--------------PHH--------------------
#include "resource.h"		// main symbols
#include "PhotoProcessDlg.h"
#include ".\axstaticex.h"
#include "dlgadjustphoto.h"
#include ".\dlgpassword.h"
//^^^^^^^^^^^^^^^PHH^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////
// [Class  ]: CAxCanvasST
// [Comment]: 
// [Feature]:  
// [Usage  ]:
// [Related]: 
// [Others ]: 
//

BYTE CAxImageCanvasST::m_ArrayPoint[PHOTO_ORI_POINTS_X][PHOTO_ORI_POINTS_Y];


CAxCanvasST::CAxCanvasST()
: m_pBufDC(NULL), m_pBufBmp(NULL), m_bIsDblBuffer(TRUE), m_emBorderStyle(BDS_LOW),
  m_bIfDrawBackgnd(TRUE), m_bIfDrawBorder(TRUE), m_bHilited(FALSE)
{
    // <> Set the background color
//    m_clrBackgnd = RGB(255,255,255);//::GetSysColor(COLOR_3DFACE);
    m_clrBackgnd = ::GetSysColor(COLOR_BTNFACE);
}

CAxCanvasST::~CAxCanvasST()
{
    Term();
}

// <> Initialize the double buffer
void CAxCanvasST::Init()
{		
    // [] Get the Client rectangle
    //
    CRect rcClient;
    GetClientRect(rcClient);
        
    CClientDC dc(this);
    
    // [] Create the bitmap
    //
    if (m_pBufBmp == NULL)
    {
        m_pBufBmp = new CBitmap;
        m_pBufBmp->CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    }
    
    // [] Create the double buffer
    //
    if (m_pBufDC == NULL)
    {
        m_pBufDC = new CDC;
        m_pBufDC->CreateCompatibleDC(&dc);
        m_pBufDC->SelectObject(m_pBufBmp);
    }
    
    // [] Init the buffer
    //
    Flush();
}

void CAxCanvasST::Term()
{
    if (m_pBufDC != NULL)  SAFE_DELETE(m_pBufDC);
    if (m_pBufBmp != NULL) SAFE_DELETE(m_pBufBmp);
}

BEGIN_MESSAGE_MAP(CAxCanvasST, CStatic)
//{{AFX_MSG_MAP(CAxCanvasST)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAxCanvasST::Initialise(UINT nID, CWnd* pParent)
{
    return SubclassDlgItem(nID, pParent);
}

void CAxCanvasST::Update()
{
    if (m_bIsDblBuffer)
    {
        Flush();
    }
    else
    {
        Invalidate();        
    }    
}

void CAxCanvasST::Flush()
{
    CRect rcClient;
    GetClientRect(rcClient);

    // <> Draw the background
    if (m_bIfDrawBackgnd)
    {
        CBrush brush(m_clrBackgnd);
        m_pBufDC->FillRect(rcClient, &brush);
    }
        
    // <> Call the paint method directly for non double buffered
    OnDraw(m_pBufDC);
        
    // <> Draw a border
    if (m_bIfDrawBorder)
    {
        OnDrawBorder(m_pBufDC);
    }
        
    // <> Copy the double buffer to the front
    CClientDC dc(this);    
    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), m_pBufDC, 0, 0,SRCCOPY);
}

void CAxCanvasST::HiliteBorderColor(BOOL bHilited)
{
    m_bHilited = bHilited;
    Update();
}


/*
// [] Modify background color 
//
void CAxCanvasST::SetBackgroundBrush(int nSysColorIdx)
{
    if (nSysColorIdx == 0)
        m_backBrush = (HBRUSH) ::GetStockObject(HOLLOW_BRUSH);
    else
        m_backBrush = (HBRUSH) (sysColorIndex+1);
    Invalidate();
}

void CAxCanvasST::SetBackgroundBrush(CBrush & brush)
{

}

void CAxCanvasST::SetBackgroundBrush(HBRUSH brush)
{

}*/


// <> OnPaint
void CAxCanvasST::OnPaint() 
{
    CPaintDC dc(this);
    
    CRect rcClient;
    GetClientRect(rcClient);
    
    // [] Check is double buffered
    //
    if (m_bIsDblBuffer)
    {
        // <> Copy back image to dc
        dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), m_pBufDC, 0, 0, SRCCOPY);
    }
    else
    {
        // <> Set the clipping region
        CRgn rgnClip;
        rgnClip.CreateRectRgnIndirect(rcClient);
        dc.SelectClipRgn(&rgnClip);
        
        // <> Draw the background
        if (m_bIfDrawBackgnd)
        {
            CBrush brush(m_clrBackgnd);
            dc.FillRect(rcClient, &brush);
        }
        
        // <> Call the paint method directly for non double buffered
        OnDraw(&dc);
        
        // <> Draw a border
        if (m_bIfDrawBorder)
        {
            OnDrawBorder(&dc);
        }
    }
}

BOOL CAxCanvasST::OnEraseBkgnd(CDC* pDC) 
{
    // <> We are managing the background
    return TRUE;
}

// <> Called after the CCanvas is created, before it is visible
void CAxCanvasST::PreSubclassWindow() 
{	
    CStatic::PreSubclassWindow();
    
    // <> Init double buffer
    Init();
}

// <> Draw a border
void CAxCanvasST::OnDrawBorder(CDC* pDC)
{
    CRect rcClient;
    GetClientRect(rcClient);
    
    switch (m_emBorderStyle)
    {
    case BDS_SUNKEN:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DLIGHT));
        break;
        
    case BDS_HIGH:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DLIGHT), GetSysColor(COLOR_3DSHADOW));
        break;
        
    case BDS_FLAT:
        if (!m_bHilited)
        {
            DrawRect(pDC, rcClient, GetSysColor(COLOR_3DDKSHADOW));
            rcClient.DeflateRect(1, 1, 1, 1);
            DrawRect(pDC, rcClient, GetSysColor(COLOR_3DDKSHADOW));            
        }
        else
        {
//            CPen  BorderPen;
//            BorderPen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
//            CPen* pOldPen = pDC->SelectObject(&BorderPen);
            
            DrawRect(pDC, rcClient, RGB(0, 255, 0));
            rcClient.DeflateRect(1, 1, 1, 1);
            DrawRect(pDC, rcClient, RGB(0, 255, 0));            
        }
        break;
        
    case BDS_LOW:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
        break;
        
    case BDS_UP:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
        break;
        
    case BDS_SIMPLE:
        DrawRect(pDC, rcClient, GetSysColor(COLOR_3DDKSHADOW));
        break;
        
    case BDS_FRAME:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
        rcClient.DeflateRect(1, 1, 1, 1);
        DrawRect(pDC, rcClient, GetSysColor(COLOR_3DFACE));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT));
        break;
        
    case BDS_THIN_FRAME:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT));
        break;
        
    case BDS_LIGHT_FRAME:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
        rcClient.DeflateRect(1, 1, 1, 1);
        DrawRect(pDC, rcClient, GetSysColor(COLOR_3DFACE));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
        break;
        
    case BDS_THIN_LIGHT_FRAME:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
        break;
        
    case BDS_ETCHED:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
        break;
        
    case BDS_ETCHED2:
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
        rcClient.DeflateRect(1, 1, 1, 1);
        DrawRect(pDC, rcClient, GetSysColor(COLOR_3DDKSHADOW));
        rcClient.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(rcClient, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
        break;
        
    default:
        break;
    }
}

// [] Draw a rectangle
//
void CAxCanvasST::DrawRect(CDC* pDC, RECT rect)
{
    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.right - 1, rect.top);
    pDC->LineTo(rect.right - 1, rect.bottom - 1);
    pDC->LineTo(rect.left, rect.bottom-1);
    pDC->LineTo(rect.left, rect.top);
}

void CAxCanvasST::DrawRect(CDC* pDC, RECT rect, COLORREF color)
{
    CPen pen(PS_SOLID, 1, color);
    CPen* oldPen = pDC->SelectObject(&pen);
    
    DrawRect(pDC, rect);
    
    pDC->SelectObject(oldPen);
}

void CAxCanvasST::DrawRect(CDC* pDC, int x, int y, int w, int h)
{
    pDC->MoveTo(x, y);
    pDC->LineTo(x + w,y);
    pDC->LineTo(x + w, y + h);
    pDC->LineTo(x, y + h);
    pDC->LineTo(x, y);
}

void CAxCanvasST::DrawRect(CDC* pDC, int x, int y, int w, int h, COLORREF color)
{
    CPen pen(PS_SOLID, 1, color);
    CPen* oldPen = pDC->SelectObject(&pen);
    oldPen;    
    DrawRect(pDC, x, y, w, h);
}

// Draw a circle
void CAxCanvasST::DrawCircle(CDC* pDC, int x, int y, int radius)
{
    pDC->Arc(x - radius, y - radius, x + radius, y + radius, x, y - radius, x, y - radius);
}

void CAxCanvasST::DrawCircle(CDC* pDC, int x, int y, int radius, COLORREF color)
{
    CPen pen(PS_SOLID, 1, color);
    CPen* oldPen = pDC->SelectObject(&pen);
    
    DrawCircle(pDC, x, y, radius);
    
    pDC->SelectObject(oldPen);
}

// Draw a line
void CAxCanvasST::DrawLine(CDC* pDC, CPoint& start, CPoint& end)
{
    pDC->MoveTo(start);
    pDC->LineTo(end);
}

void CAxCanvasST::DrawLine(CDC* pDC, CPoint& start, CPoint& end, COLORREF color)
{
    CPen pen(PS_SOLID, 1, color);
    CPen* oldPen = pDC->SelectObject(&pen);
    
    DrawLine(pDC, start, end);
    
    pDC->SelectObject(oldPen);
}

void CAxCanvasST::DrawLine(CDC* pDC, int xStart, int yStart, int xEnd, int yEnd)
{
    pDC->MoveTo(xStart, yStart);
    pDC->LineTo(xEnd, yEnd);
}

void CAxCanvasST::DrawLine(CDC* pDC, int xStart, int yStart, int xEnd, int yEnd, COLORREF color)
{
    CPen pen(PS_SOLID,1,color);
    CPen* oldPen = pDC->SelectObject(&pen);
    
    DrawLine(pDC, xStart, yStart, xEnd, yEnd);
    
    pDC->SelectObject(oldPen);
}

///////////////////////////////////////////////////////////////////////////////
// [Class  ]: CAxImageCanvasST
// [Comment]: 
// [Feature]:  
// [Usage  ]:
// [Related]: 
// [Others ]: 
//

CAxImageCanvasST::CAxImageCanvasST()
 : m_pxImgInner(NULL), m_bIsDragging(FALSE) //----PHH----
{
    Reset();
}

CAxImageCanvasST::~CAxImageCanvasST()
{
}

BOOL CAxImageCanvasST::LoadImage(LPCTSTR pszImagePath)
{
    ASSERT(pszImagePath != NULL && ::lstrlen(pszImagePath) > 0);
    
    if (m_pxImgInner == NULL) return FALSE;

    Reset();
    
    BOOL bRetValue = m_pxImgInner->Load(pszImagePath, 0);
        
    CalcFactor();
    
    return bRetValue;
}

BOOL CAxImageCanvasST::SetImage(HBITMAP hBitmap)
{
    ASSERT(hBitmap != NULL);

    if (m_pxImgInner == NULL) return FALSE;    
    
    Reset();
    
    m_pxImgInner->CreateFromHBITMAP(hBitmap);
    
    CalcFactor();
    
    return TRUE;
}

BOOL CAxImageCanvasST::SetImage(CxImage* pImage)
{
    ASSERT(pImage != NULL);

    Reset();
    
    m_pxImgInner = pImage;

    CalcFactor();
    
    return TRUE;
}

CxImage* CAxImageCanvasST::GetImage() const
{
    return m_pxImgInner;
}

void CAxImageCanvasST::Reset()
{
    m_nImagePos = POSITION_NORMAL;
    m_nAlignX   = ALIGN_LEFT;
    m_nAlignY   = ALIGN_TOP;
    
    m_ptOrigin.x = m_ptOrigin.y = 0;
    m_dblZoomX = m_dblZoomY = 1.0;

    m_rectDraw.SetRectEmpty();    
}

void CAxImageCanvasST::Update()
{
    CalcFactor();
    CAxCanvasST::Update();
}

void CAxImageCanvasST::CalcFactor()
{
    if (m_pxImgInner == NULL || !m_pxImgInner->IsValid()) return;
    
    CRect rectClient;
    GetClientRect(&rectClient);
    
    CRect rectImage;
    rectImage.SetRect(0, 0, m_pxImgInner->GetWidth(), m_pxImgInner->GetHeight());
    
    m_rectDraw.left = m_rectDraw.top = 0;
//    m_dblZoomX = m_dblZoomY = 1.0;

    switch (m_nImagePos)
    {
    case POSITION_NORMAL:           // Left upper corner
        m_rectDraw.right  = rectImage.Width();
        m_rectDraw.bottom = rectImage.Height();
        break;
        
    case POSITION_STRETCH:          // Stretch to fit entire frame
        m_rectDraw.right  = rectClient.Width();
        m_rectDraw.bottom = rectClient.Height();
        m_dblZoomX = ((double)m_rectDraw.right) / rectImage.Width();
        m_dblZoomY = ((double)m_rectDraw.bottom) / rectImage.Height();
        break;
        
    case POSITION_CUSTOM:           // Custom source, zoom
        m_rectDraw.right  = (int)(rectImage.Width()  * m_dblZoomX);
        m_rectDraw.bottom = (int)(rectImage.Height() * m_dblZoomY);
        break;
        
    case POSITION_FITX:
    case POSITION_FITY:
    case POSITION_FITXY:
    case POSITION_FITSMALL:
        {
            double dblZoom  = 1;
            double dblZoomX = ((double)rectClient.Width())  / rectImage.Width(); 
            double dblZoomY = ((double)rectClient.Height()) / rectImage.Height();
            if (m_nImagePos == POSITION_FITX)               // Stretch to fit X coordinate
            {
                dblZoom = dblZoomX;
            }
            else if (m_nImagePos == POSITION_FITY)          // Stretch to fit Y cooddinate
            {
                dblZoom = dblZoomY;
            }
            else
            {
                // for stretchXY take smaller, for stretchSm take larger;
                dblZoom = ((m_nImagePos == POSITION_FITXY) ^ (dblZoomX > dblZoomY)) ? dblZoomX : dblZoomY;
            }
            
            m_dblZoomX = m_dblZoomY = dblZoom;  
            m_rectDraw.right  = (int)(rectImage.Width()  * dblZoom);
            m_rectDraw.bottom = (int)(rectImage.Height() * dblZoom);
            
            break;
        }
        
    case POSITION_TILE:
        break;
    }
    
    switch (m_nAlignX)
    {
    case ALIGN_CENTER: m_ptOrigin.x = (rectClient.Width() - m_rectDraw.Width()) / 2; break;
    case ALIGN_RIGHT:  m_ptOrigin.x = rectClient.Width() - m_rectDraw.Width();       break;
    case ALIGN_LEFT:   m_ptOrigin.x = 0;                                             break;
    }
    m_rectDraw.left  += m_ptOrigin.x;
    m_rectDraw.right += m_ptOrigin.x;
    
    switch (m_nAlignY)
    {
    case ALIGN_CENTER: m_ptOrigin.y = (rectClient.Height() - m_rectDraw.Height()) /2; break;
    case ALIGN_BOTTOM: m_ptOrigin.y = rectClient.Height() - m_rectDraw.Height();      break;
    case ALIGN_TOP:    m_ptOrigin.y = 0;                                              break;
    }
    m_rectDraw.top    += m_ptOrigin.y;
    m_rectDraw.bottom += m_ptOrigin.y;
    
    if (m_nImagePos == POSITION_TILE) 
    {
        m_rectDraw.SetRect(0, 0, rectClient.Width(), rectClient.Height());
    }

//    CRect rectScrean(m_rectDraw);
//    ClientToScreen(rectScrean);
//    ClipCursor(rectScrean);
}


void CAxImageCanvasST::OnDraw(CDC* pDC)
{
    if (m_pxImgInner == NULL || !m_pxImgInner->IsValid()) return;
    
    if (m_nImagePos != POSITION_TILE)
    {
        m_pxImgInner->Draw(pDC->GetSafeHdc(), m_rectDraw.left, m_rectDraw.top, m_rectDraw.Width(), m_rectDraw.Height());        
    }
    else
    {
        CRect   rectClient;
        GetClientRect(&rectClient);
        
        CRect   rectImage;
        rectImage.SetRect(0, 0, m_pxImgInner->GetWidth(), m_pxImgInner->GetHeight());
        
        for (int x = m_ptOrigin.x; x < rectClient.right; x += rectImage.Width())
        {
            if (x > -rectClient.right)
            {
                for (int y = m_ptOrigin.y; y < rectClient.bottom; y += rectImage.right)
                {
                    if (y > -rectClient.bottom)
                    {
                        m_pxImgInner->Draw(pDC->GetSafeHdc(), x, y, rectImage.Width(), rectImage.Height());
                    }
                }
            }
        }
        
    }
}

void CAxImageCanvasST::SetPositionMode(int nPosition)
{
    if (nPosition < 0 || nPosition > _POSITION_MAX_) nPosition = POSITION_NORMAL;

    if (nPosition == m_nImagePos) return;

    m_nImagePos = nPosition;

    m_dblZoomX = m_dblZoomY = 1.0;
    
    CalcFactor();
}

void CAxImageCanvasST::SetAlignX(int nAlignX)
{
    if (nAlignX < 0 || nAlignX > _ALIGN_MAX_) nAlignX = ALIGN_LEFT;

    if (nAlignX == m_nAlignX) return;

    m_nAlignX = nAlignX;
    
    CalcFactor();    
}

void CAxImageCanvasST::SetAlignY(int nAlignY)
{
    if (nAlignY < 0 || nAlignY > _ALIGN_MAX_) nAlignY = ALIGN_TOP;
    
    if (nAlignY == m_nAlignY) return;
    
    m_nAlignY = nAlignY;
    
    CalcFactor();    
}

void CAxImageCanvasST::SetZoomX(double dblZoomX)
{
    m_dblZoomX  = dblZoomX;
    m_nImagePos = POSITION_CUSTOM;
    
    CalcFactor();    
}

void CAxImageCanvasST::SetZoomY(double dblZoomY)
{
    m_dblZoomY  = dblZoomY;
    m_nImagePos = POSITION_CUSTOM;
    
    CalcFactor();    
}

void CAxImageCanvasST::SetOriginX(int nOrigX)
{
    m_ptOrigin.x = nOrigX;
    m_nAlignX    = ALIGN_CUSTOM;

    CalcFactor();
}

void CAxImageCanvasST::SetOriginY(int nOrigY)
{
    m_ptOrigin.y = nOrigY;
    m_nAlignY    = ALIGN_CUSTOM;
    
    CalcFactor();    
}

///////////////////////////////////////////////////////////////////////////////
// [Class  ]: CAxTrackImageCanvasST
// [Comment]: 
// [Feature]:  
// [Usage  ]:
// [Related]: 
// [Others ]: 
//
CAxTrackImageCanvasST::CAxTrackImageCanvasST()
: m_pTracker(NULL), m_bIfShowRect(FALSE)
{
    
}

CAxTrackImageCanvasST::~CAxTrackImageCanvasST()
{
    
}

BEGIN_MESSAGE_MAP(CAxTrackImageCanvasST, CAxImageCanvasST)
//{{AFX_MSG_MAP(CAxTrackImageCanvasST)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP() 
    ON_WM_SETCURSOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAxTrackImageCanvasST::SetTrackerRect(CAxRectTracker* pTracker)
{
    ASSERT(pTracker != NULL);

    m_pTracker = pTracker;
}

BOOL CAxTrackImageCanvasST::ShowTrackerRect(BOOL bShow)
{
    if (m_pTracker == NULL) return FALSE;

    m_bIfShowRect = bShow;

//    Update();

    return TRUE;
}

void CAxTrackImageCanvasST::OnDraw(CDC* pDC)
{
    CAxImageCanvasST::OnDraw(pDC);

    if (m_pTracker != NULL && m_bIfShowRect)
        m_pTracker->Draw(pDC);
}

void CAxTrackImageCanvasST::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if (m_pTracker != NULL && m_bIfShowRect)
    {
        m_pTracker->Track(this, point);
        Update();
    }

    CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////
void CAxTrackImageCanvasST::OnMouseMove(UINT nFlags, CPoint point) 
{   
    CStatic::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
void CAxTrackImageCanvasST::OnLButtonUp(UINT nFlags, CPoint point) 
{
    CStatic::OnLButtonUp(nFlags, point);
}

BOOL CAxTrackImageCanvasST::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if (m_pTracker != NULL && m_bIfShowRect)
    {        
        // If we are in the client area change the cursor
        if (pWnd == this && m_pTracker->SetCursor(this, nHitTest))
            return TRUE;
    }
    
    return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

//-----------------------PHH--------------------------------------------
BEGIN_MESSAGE_MAP(CAxImageCanvasST, CAxCanvasST)
//{{AFX_MSG_MAP(CAxImageCanvasST)
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
//ON_WM_MOUSEWHEEL()
ON_WM_SETCURSOR()
//}}AFX_MSG_MAP
ON_WM_LBUTTONDBLCLK()
ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

void CAxImageCanvasST::OnMouseMove(UINT nFlags, CPoint point)
{
    
    CPhotoProcessDlg *pcWndParent = (CPhotoProcessDlg *)(this->GetParent());
    
    if(&pcWndParent->m_ctrlProcessST==this) {//只有大窗口
        if(m_bIsDragging) {
            pcWndParent->MovePhoto(point.x-m_cPointDragBegin.x,point.y-m_cPointDragBegin.y);
            m_cPointDragBegin = point;
        }
        else
        {
            m_hCurCursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
            
            ::SetCursor(m_hCurCursor);
			pcWndParent->DumpPhotoFactor(point);

        }
        
    }
    
    CAxCanvasST::OnMouseMove(nFlags, point);
}

void CAxImageCanvasST::OnLButtonUp(UINT nFlags, CPoint point)
{
    CPhotoProcessDlg *pcWndParent = (CPhotoProcessDlg *)(this->GetParent());
    
    if(&pcWndParent->m_ctrlProcessST==this) {//只有大窗口
        //推出拖动状态
        m_bIsDragging = FALSE;
        //不用监禁鼠标
        ClipCursor(NULL);
        
        m_hCurCursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        
        ::SetCursor(m_hCurCursor);
    }
    
    CAxCanvasST::OnLButtonUp(nFlags, point);
}

void CAxImageCanvasST::OnLButtonDown(UINT nFlags, CPoint point)
{
    //	AfxMessageBox("OnLButtonDown");
    
    
    CPhotoProcessDlg *pcWndParent = (CPhotoProcessDlg *)(this->GetParent());
    
    if(&pcWndParent->m_ctrlProcessST==this) {//只有大窗口
        
        //进入拖动状态
        m_bIsDragging = TRUE;
        m_cPointDragBegin = point;
        m_hCurCursor=AfxGetApp()->LoadStandardCursor(IDC_HAND);	//PHH加入
        
        {//把鼠标关起来
            CRect r;
            GetClientRect(&r);
            ClientToScreen(&r);
            ClipCursor(&r);
        }
        
        ::SetCursor(m_hCurCursor);
    }
    
    
    CAxCanvasST::OnLButtonDown(nFlags, point);
}


BOOL CAxImageCanvasST::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPhotoProcessDlg *pcWndParent = (CPhotoProcessDlg *)(this->GetParent());
    
    if(&pcWndParent->m_ctrlProcessST==this) {//只有大窗口
        ::SetCursor(m_hCurCursor);
        return FALSE;
    }
    else
    {
        //::SetCursor(m_hCurCursor);
        return CAxCanvasST::OnSetCursor(pWnd, nHitTest, message);
    }
}

//^^^^^^^^^^^^PHH^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void CAxImageCanvasST::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	
	CDlgPassword dlgPW;

	if(dlgPW.DoModal()!=IDOK) return;



	CDlgAdjustPhoto cDlgAdjust;

	cDlgAdjust.m_nAdjustStep = 3;//注意这个是初始化调整步长参数，如果想调整效果明显，加大它！

	//----------PHH NOTICE 1---------------------
	//
	//这里要将以前储存的调整参数（3个色差，1个亮度，1个对比度参数）调入，并且按以下方法调整！
	//	cDlgAdjust.m_nAdjust_Blue = blueLoad;
	//	cDlgAdjust.m_nAdjust_Green = greenLoad;
	//	cDlgAdjust.m_nAdjust_Red = redLoad;
	//	cDlgAdjust.m_nAdjust_Light = lightLoad;
	//	cDlgAdjust.m_nAdjust_Contrast = contrastLoad
	CString strAppPath  = CAppContext::GetAppPath();        // 从"Favorite.dat"中取得参数
	//CString strFavorite = strAppPath + _T("\\favorite.dat");    
	CString strFavorite = _T("TP_Core.ini");    
	CIni ini;
	ini.SetAppName("TP_Core");    
	ini.m_bFromRegistry = TRUE;
	ini.SetFileName(strFavorite);    
	cDlgAdjust.m_nAdjust_Red = ini.GetInt(_T("PA_TooRed"),0 , _T("Adjust Setting"));
	cDlgAdjust.m_nAdjust_Blue = ini.GetInt(_T("PA_TooBlue"),0 , _T("Adjust Setting"));
	cDlgAdjust.m_nAdjust_Green = ini.GetInt(_T("PA_TooGreen"),0 , _T("Adjust Setting"));
	cDlgAdjust.m_nAdjust_Contrast = ini.GetInt(_T("PA_Contrast"),0 , _T("Adjust Setting"));
	cDlgAdjust.m_nAdjust_Light = ini.GetInt(_T("PA_Light"),0 , _T("Adjust Setting"));

	cDlgAdjust.m_nEditSensitive = ini.GetInt(_T("DeBkgdSen"),20 , _T("Adjust Setting"));
	cDlgAdjust.m_bCheckAuto = ini.GetInt(_T("DeBkgdAuto"),1 , _T("Adjust Setting"));
	cDlgAdjust.m_bCheckDoAll = ini.GetInt(_T("DeBkgdAll"),0 , _T("Adjust Setting"));


	cDlgAdjust.SetImage(m_pxImgInner);
	if(cDlgAdjust.DoModal()==IDCANCEL) {
		CAxCanvasST::OnLButtonDblClk(nFlags, point);
		return;
	}


	CPhotoProcessDlg *pcDlg = (CPhotoProcessDlg *)GetParent();
/*
	//按照设定参数修改原始图形
	pcDlg->GetCurrentImage()->ShiftRGB(
		cDlgAdjust.m_nAdjust_Blue+cDlgAdjust.m_nAdjust_Green,
		cDlgAdjust.m_nAdjust_Blue+cDlgAdjust.m_nAdjust_Red,
		cDlgAdjust.m_nAdjust_Green+cDlgAdjust.m_nAdjust_Red);
	pcDlg->GetCurrentImage()->Light(
		cDlgAdjust.m_nAdjust_Light, 
		cDlgAdjust.m_nAdjust_Contrast);
*/
	if(AfxMessageBox("是否以后的照片都按照刚才调整的参数进行自动处理？",MB_YESNO)==IDYES) {
		//-------------------------------------------------------------
		//PHH NOTICE 2
		//
		//此处应该将这些参数存盘:cDlgAdjust.m_nAdjust_Blue,m_nAdjust_Green...等5个参数
		//以便下次拍摄动作后马上用这些参数进行调整。
		CString strAppPath  = CAppContext::GetAppPath();        // 从"Favorite.dat"中取得参数
		//CString strFavorite = strAppPath + _T("\\favorite.dat");    
		CString strFavorite = _T("TP_Core.ini");    
		CIni ini;
		ini.SetAppName("TP_Core");    
		ini.m_bFromRegistry = TRUE;

		ini.SetFileName(strFavorite);    
		ini.WriteInt(_T("PA_TooRed"), cDlgAdjust.m_nAdjust_Red, _T("Adjust Setting"));
		ini.WriteInt(_T("PA_TooBlue"), cDlgAdjust.m_nAdjust_Blue, _T("Adjust Setting"));
		ini.WriteInt(_T("PA_TooGreen"), cDlgAdjust.m_nAdjust_Green, _T("Adjust Setting"));
		ini.WriteInt(_T("PA_Contrast"), cDlgAdjust.m_nAdjust_Contrast, _T("Adjust Setting"));
		ini.WriteInt(_T("PA_Light"), cDlgAdjust.m_nAdjust_Light, _T("Adjust Setting"));

		ini.WriteInt(_T("DeBkgdSen"), cDlgAdjust.m_nEditSensitive, _T("Adjust Setting"));
		ini.WriteInt(_T("DeBkgdAuto"), cDlgAdjust.m_bCheckAuto, _T("Adjust Setting"));
		ini.WriteInt(_T("DeBkgdAll"), cDlgAdjust.m_bCheckDoAll, _T("Adjust Setting"));

	}

	//-------------------------------------------------------------------
	//PHH NOTICE
	//
	//注意：如果选择了多个调整参数，以上函数的效果是不是和显示的效果一样？？？
	//所以此处最好将修改好的原始图形再显示出来
	//.....
	//pcDlg->m_ctrlProcessST.Update();   

	//PHH IDII
	pcDlg->DisplayTemplatePhoto();
	pcDlg->DumpPhotoFactor();

	CAxCanvasST::OnLButtonDblClk(nFlags, point);
}


void CAxImageCanvasST::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDlgPassword dlgPW;

	if(dlgPW.DoModal()!=IDOK) return;



	this->DeBackground(point);
	CAxCanvasST::OnRButtonUp(nFlags, point);
}
void CAxImageCanvasST::DeBackground(CPoint point)

{


	CString strAppPath  = CAppContext::GetAppPath();        // 从"Favorite.dat"中取得参数
	//CString strFavorite = strAppPath + _T("\\favorite.dat");    
	CString strFavorite = _T("TP_Core.ini");    
	CIni ini;
	ini.SetFileName(strFavorite);    
	ini.SetAppName("TP_Core");    
	ini.m_bFromRegistry = TRUE;

	//获得敏感度数值
	m_nChange = ini.GetInt(_T("DeBkgdSen"),0 , _T("Adjust Setting"));


	CRect rectWindow;

    //-----------------------------------------------------
    // 1. 准备图片信息
    //-----------------------------------------------------
	CPhotoProcessDlg *pcWndParent = (CPhotoProcessDlg *)(this->GetParent());
    //CIDCardCollMainDlg *pMainDlg = (CIDCardCollMainDlg*)GetParent();
    CPreProcessImage& curPrecessImage = pcWndParent->m_PrePrcssImages[pcWndParent->m_nCurPreImage];

    
    //-----------------------------------------------------
    // 2. 取得模板图片的位置信息
    //-----------------------------------------------------
    m_rectTracker =  pcWndParent->m_PrePrcssImages[0].m_rectOriDraw;   // m_PreviewImages[0] - 模板图片
    // <> 根据当前图片位置, 调整取景框范围
    double dblZoomX   = curPrecessImage.m_dblNewZoom; 
    double dblZoomY   = curPrecessImage.m_dblNewZoom; 
/*
	// <> 调整原点
    int nOriginX = (int)(curPrecessImage.m_nNewOriginX / dblZoomX);
    int nOriginY = (int)(curPrecessImage.m_nNewOriginY / dblZoomY);
    // <> 调整取景框位置    
    m_rectTracker.left   = (int)(m_rectTracker.left   / dblZoomX) - nOriginX;
    m_rectTracker.top    = (int)(m_rectTracker.top    / dblZoomY) - nOriginY;
    m_rectTracker.right  = (int)(m_rectTracker.right  / dblZoomX) - nOriginX;
    m_rectTracker.bottom = (int)(m_rectTracker.bottom / dblZoomY) - nOriginY;
*/

	m_rectTracker.left   = (int)((m_rectTracker.left-curPrecessImage.m_nNewOriginX)/ dblZoomX);
	m_rectTracker.top    = (int)((m_rectTracker.top-curPrecessImage.m_nNewOriginY) / dblZoomY);
	m_rectTracker.right  = (int)((m_rectTracker.right-curPrecessImage.m_nNewOriginX)  / dblZoomX);
	m_rectTracker.bottom = (int)((m_rectTracker.bottom-curPrecessImage.m_nNewOriginY) / dblZoomY);



	m_rectTracker.top = m_pxImgInner->GetHeight()-1-m_rectTracker.top;
	m_rectTracker.bottom = m_pxImgInner->GetHeight()-1-m_rectTracker.bottom;



	CRect rectClient;
	this->GetClientRect(rectClient); 
	CPoint pointImage;
	pointImage.x = m_rectTracker.left + point.x/dblZoomX;
	pointImage.y = m_rectTracker.bottom + (rectClient.bottom-point.y)/dblZoomY;

	//获得当前点的颜色
	m_rgbOld = m_pxImgInner->GetPixelColor(pointImage.x, pointImage.y);
	//CString cStrTemp;
	//cStrTemp.Format("该点颜色:R%d G%d B%d",m_rgbOld.rgbRed,m_rgbOld.rgbGreen,m_rgbOld.rgbBlue);
	//AfxMessageBox(cStrTemp);


	//-----------------------------------------------------------------------
	//如果是全图漂白
	if(ini.GetInt(_T("DeBkgdAll"),0 , _T("Adjust Setting"))) {
		RGBQUAD rgb;

		for(DWORD i=0;i<m_pxImgInner->GetWidth(); ++i) {
			for(DWORD j=0; j<m_pxImgInner->GetHeight(); ++j) {
				rgb = m_pxImgInner->GetPixelColor(i, j);
				if((m_rgbOld.rgbBlue>=rgb.rgbBlue&&m_rgbOld.rgbBlue-rgb.rgbBlue<m_nChange) ||( m_rgbOld.rgbBlue<=rgb.rgbBlue&&rgb.rgbBlue-m_rgbOld.rgbBlue<m_nChange))
					if((m_rgbOld.rgbRed>=rgb.rgbRed&&m_rgbOld.rgbRed-rgb.rgbRed<m_nChange) || (m_rgbOld.rgbRed<=rgb.rgbRed&&rgb.rgbRed-m_rgbOld.rgbRed<m_nChange))
						if((m_rgbOld.rgbGreen>=rgb.rgbGreen&&m_rgbOld.rgbGreen-rgb.rgbGreen<m_nChange) || (m_rgbOld.rgbGreen<=rgb.rgbGreen&&rgb.rgbGreen-m_rgbOld.rgbGreen<m_nChange)) {
							m_pxImgInner->SetPixelColor(i,j,RGB(255,255,255));
						}
			}
		}

		this->Update(); 
		return;
	}
	//--------------------------------------------------------------------------


//	CString cStrTemp;
//	cStrTemp.Format("left: %d right: %d top:%d bottom:%d\npointX=%d,pointY=%d\n x=%d, y=%d",m_rectTracker.left,m_rectTracker.right,m_rectTracker.top,m_rectTracker.bottom,point.x,point.y,pointImage.x, pointImage.y);
//	AfxMessageBox(cStrTemp);

	if(m_rectTracker.Width()>=PHOTO_ORI_POINTS_X||m_rectTracker.Height()>=PHOTO_ORI_POINTS_Y) {
		AfxMessageBox(_T("请先进行构图（放大图形）后再进行此操作！"));
		return;
	}

	for(int i=0;i<PHOTO_ORI_POINTS_X;++i)
		for(int j=0; j<PHOTO_ORI_POINTS_Y; ++j)
			m_ArrayPoint[i][j] = 0;

	m_ArrayPoint[pointImage.x-m_rectTracker.left][pointImage.y-m_rectTracker.bottom] = 1;
	//m_pxImgInner->SetPixelColor(pointImage.x, pointImage.y, RGB(255,255,255));
	BOOL bEnd = TRUE;

//	CString cStrTemp;
//	cStrTemp.Format("left=%d right=%d top=%d bottum=%d",m_rectTracker.left,m_rectTracker.right,m_rectTracker.top,m_rectTracker.bottom);
//	AfxMessageBox(cStrTemp);

	for(;;) {
		bEnd = TRUE;
		for(int i=m_rectTracker.left;i<=m_rectTracker.right; ++i) {
			for(int j=m_rectTracker.bottom;j<=m_rectTracker.top;++j) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}
//------Add
		for(int i=m_rectTracker.left;i<=m_rectTracker.right; ++i) {
			for(int j=m_rectTracker.top;j>=m_rectTracker.bottom;--j) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}




		for(int i=m_rectTracker.right;i>=m_rectTracker.left; --i) {
			for(int j=m_rectTracker.top;j>=m_rectTracker.bottom;--j) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}
//----Add
		for(int i=m_rectTracker.right;i>=m_rectTracker.left; --i) {
			for(int j=m_rectTracker.bottom;j<=m_rectTracker.top;++j) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}




		for(int j=m_rectTracker.bottom;j<=m_rectTracker.top;++j) {
			for(int i=m_rectTracker.left;i<=m_rectTracker.right; ++i) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色
					//cStrTemp.Format("该点颜色:i=%d j=%d R%d G%d B%d",i,j,m_rgbOld.rgbRed,m_rgbOld.rgbGreen,m_rgbOld.rgbBlue);
					//AfxMessageBox(cStrTemp);

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}

//------ADD

		for(int j=m_rectTracker.bottom;j<=m_rectTracker.top;++j) {
			for(int i=m_rectTracker.right;i>=m_rectTracker.left; --i) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色
					//cStrTemp.Format("该点颜色:i=%d j=%d R%d G%d B%d",i,j,m_rgbOld.rgbRed,m_rgbOld.rgbGreen,m_rgbOld.rgbBlue);
					//AfxMessageBox(cStrTemp);

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}




		for(int j=m_rectTracker.top;j>=m_rectTracker.bottom;--j) {
			for(int i=m_rectTracker.right;i>=m_rectTracker.left; --i) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}

	//Add------
		for(int j=m_rectTracker.top;j>=m_rectTracker.bottom;--j) {
			for(int i=m_rectTracker.left;i<=m_rectTracker.right; ++i) {
				//这一点符合要求
				if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
					//以这一点的颜色作为标准能够对付渐变的背景
					m_rgbOld= m_pxImgInner->GetPixelColor(i, j);//获得颜色

					if(IsFit(i+1, j))	bEnd=FALSE;
					if(IsFit(i+1, j+1)) bEnd=FALSE;
					if(IsFit(i+1, j-1)) bEnd=FALSE;
					if(IsFit(i-1, j))	bEnd=FALSE;
					if(IsFit(i-1, j+1)) bEnd=FALSE;
					if(IsFit(i-1, j-1)) bEnd=FALSE;
					if(IsFit(i, j+1))	bEnd=FALSE;
					if(IsFit(i, j-1))	bEnd=FALSE;
				}
			}
		}


		this->Update(); 
		if(bEnd) break;
	}

	//this->SearchPoint(pointImage.x, pointImage.y);
	//是背景的都变白
	for(int i=m_rectTracker.left+1;i<=m_rectTracker.right-1; ++i) {
		for(int j=m_rectTracker.bottom+1;j<=m_rectTracker.top-1;++j) {
			//这一点符合要求
			if(m_ArrayPoint[i-m_rectTracker.left][j-m_rectTracker.bottom] == 1) {
				m_pxImgInner->SetPixelColor(i, j, RGB(255,255,255));
			}
		}
	}

    this->Update(); 

}
BOOL CAxImageCanvasST::IsFit(_int16 x, _int16 y)
{
	//已经搞过了
	if(m_ArrayPoint[x-m_rectTracker.left][y-m_rectTracker.bottom] == 1) return FALSE;

	RGBQUAD rgbGet;
	rgbGet = m_pxImgInner->GetPixelColor(x, y);//获得颜色
	if((rgbGet.rgbBlue>=m_rgbOld.rgbBlue&&rgbGet.rgbBlue-m_rgbOld.rgbBlue<m_nChange) ||( rgbGet.rgbBlue<=m_rgbOld.rgbBlue&&m_rgbOld.rgbBlue-rgbGet.rgbBlue<m_nChange))
		if((rgbGet.rgbRed>=m_rgbOld.rgbRed&&rgbGet.rgbRed-m_rgbOld.rgbRed<m_nChange) || (rgbGet.rgbRed<=m_rgbOld.rgbRed&&m_rgbOld.rgbRed-rgbGet.rgbRed<m_nChange))
			if((rgbGet.rgbGreen>=m_rgbOld.rgbGreen&&rgbGet.rgbGreen-m_rgbOld.rgbGreen<m_nChange) || (rgbGet.rgbGreen<=m_rgbOld.rgbGreen&&m_rgbOld.rgbGreen-rgbGet.rgbGreen<m_nChange)) {
//				m_pxImgInner->SetPixelColor(x, y, RGB(255,255,255));
				m_ArrayPoint[x-m_rectTracker.left][y-m_rectTracker.bottom] = 1;
				return TRUE;
			}

	return FALSE;
}

void CAxImageCanvasST::SearchPoint(_int16 x, _int16 y)
{
	RGBQUAD rgbGet;

	if(x-m_rectTracker.left>=PHOTO_ORI_POINTS_X) {
		ASSERT(FALSE);
		return;
	}
	if(y-m_rectTracker.bottom>=PHOTO_ORI_POINTS_Y) {
		ASSERT(FALSE);
		return;
	}

	if(x<m_rectTracker.left||x>m_rectTracker.right-1) return;
	if(y<m_rectTracker.bottom||y>m_rectTracker.top-1) return;
	if(m_ArrayPoint[x-m_rectTracker.left][y-m_rectTracker.bottom] == 1) return;

	rgbGet = m_pxImgInner->GetPixelColor(x, y);//获得颜色
	if((rgbGet.rgbBlue>=m_rgbOld.rgbBlue&&rgbGet.rgbBlue-m_rgbOld.rgbBlue<m_nChange) ||( rgbGet.rgbBlue<=m_rgbOld.rgbBlue&&m_rgbOld.rgbBlue-rgbGet.rgbBlue<m_nChange))
		if((rgbGet.rgbRed>=m_rgbOld.rgbRed&&rgbGet.rgbRed-m_rgbOld.rgbRed<m_nChange) || (rgbGet.rgbRed<=m_rgbOld.rgbRed&&m_rgbOld.rgbRed-rgbGet.rgbRed<m_nChange))
			if((rgbGet.rgbGreen>=m_rgbOld.rgbGreen&&rgbGet.rgbGreen-m_rgbOld.rgbGreen<m_nChange) || (rgbGet.rgbGreen<=m_rgbOld.rgbGreen&&m_rgbOld.rgbGreen-rgbGet.rgbGreen<m_nChange)) {
				m_pxImgInner->SetPixelColor(x, y, RGB(255,255,255));
				m_ArrayPoint[x-m_rectTracker.left][y-m_rectTracker.bottom] = 1;
				SearchPoint(x+1, y);//右
				SearchPoint(x-1, y);//左
				SearchPoint(x+1, y+1);//右上
				SearchPoint(x+1, y-1);//右下
				SearchPoint(x-1, y+1);//左上 
				SearchPoint(x-1, y-1);//左下
				SearchPoint(x, y+1);//上
				SearchPoint(x, y-1);//下
			}
	return;


}
