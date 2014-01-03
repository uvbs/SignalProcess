// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axStaticEx.h
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

#ifndef _INCLUDE_AXSTATICEX_INCLUDE_
#define _INCLUDE_AXSTATICEX_INCLUDE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "xImage.h"
#include "axRectEx.h"


///////////////////////////////////////////////////////////////////////////////
// [Class  ]: CAxCanvasST
// [Comment]: 
// [Feature]:  
// [Usage  ]:
// [Related]: 
// [Others ]: 
//
class CAxCanvasST : public CStatic
{
public:
    // [] Canvas border style
    //
    enum BORDER_STYLE
    {
        BDS_SUNKEN,                 // Sunken (CLIENT_EDGE)
        BDS_LOW,                    // Low sunken (STATIC_EDGE)
        BDS_UP,                     // Up (not too high)
        BDS_HIGH,                   // High (MODAL_FRAME)
        BDS_FLAT,                   // Flat (2 pixels large Border DKSHADOW)
        BDS_FRAME,				    // Raised frame
        BDS_LIGHT_FRAME,		    // Light raised frame
        BDS_THIN_FRAME,			    // Thin raised frame
        BDS_THIN_LIGHT_FRAME,	    // Thin light raised frame
        BDS_ETCHED,				    // Etched
        BDS_ETCHED2,			    // Etched 2
        BDS_SIMPLE				    // Simple rectangle DKSHADOW
    };

public:
    CAxCanvasST();
    virtual ~CAxCanvasST();

public:

    // [] Use to subclass any window object
    BOOL Initialise(UINT nID, CWnd* pParent);
    
    // []
    //
    void SetBorderStyle(BORDER_STYLE emStyle) { m_emBorderStyle = emStyle; };
    BORDER_STYLE GetBorderStyle() const { return m_emBorderStyle; };    

    // [] Modify background color 
    //
//    void SetBackgroundBrush(int nSysColorIdx);
//    void SetBackgroundBrush(CBrush & brush);
//    void SetBackgroundBrush(HBRUSH brush);

    // []
    //
    virtual void Update();
    
    // [] Draw a rectangle
    //
    void DrawRect(CDC* pDC, RECT rect);
    void DrawRect(CDC* pDC, int x, int y, int w, int h);
    void DrawRect(CDC* pDC, RECT rect, COLORREF color);
    void DrawRect(CDC* pDC, int x, int y, int w, int h, COLORREF color);
    
    // [] Draw a circle
    //
    void DrawCircle(CDC* pDC, int x, int y, int radius);
    void DrawCircle(CDC* pDC, int x, int y, int radius, COLORREF color);
    
    // [] Draw a line
    //
    void DrawLine(CDC* pDC, CPoint& start, CPoint& end);
    void DrawLine(CDC* pDC, int xStart, int yStart, int xEnd, int yEnd);
    void DrawLine(CDC* pDC, CPoint& start, CPoint& end, COLORREF color);
    void DrawLine(CDC* pDC, int xStart, int yStart, int xEnd, int yEnd, COLORREF color);
    
    void HiliteBorderColor(BOOL bHilited = FALSE);

protected:
    // <> Draw border
    virtual void OnDrawBorder(CDC* pDC);

    virtual void OnDraw(CDC* pDC) {};   // Overridden to draw this view
    
    void Flush();                       // Flush the bitmap buffer to the client dc
    
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAxCanvasST)
protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL
    
protected:
    // Generated message map functions
    //{{AFX_MSG(CAxCanvasST)
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()        

private:
    // [] Initialize and finalization
    //
    void Init();
    void Term();

protected:
    BORDER_STYLE    m_emBorderStyle;           // Border Style

    BOOL            m_bIfDrawBackgnd;          // If draw the background
    BOOL            m_bIfDrawBorder;           // If draw the border
    COLORREF        m_clrBackgnd;              // Background color
        
    CDC*            m_pBufDC;                  // Buffer
    CBitmap*        m_pBufBmp;                 // Bitmap associated to m_buffer
    BOOL            m_bIsDblBuffer;            // Double buffer or Simple buffer  
    
    BOOL            m_bHilited;
};

///////////////////////////////////////////////////////////////////////////////
// [Class  ]: CAxImageCanvasST
// [Comment]: 
// [Feature]:  
// [Usage  ]:
// [Related]: 
// [Others ]: 
//

#define PHOTO_ORI_POINTS_Y	1800	//1500//1323	//441*3
#define PHOTO_ORI_POINTS_X	1500	//1200//1074	//358*3


class CAxImageCanvasST : public CAxCanvasST
{
public:
    enum IMAGE_ALIGN 
    {
        ALIGN_CUSTOM    = 0,                // Custom alignment
        ALIGN_LEFT      = 1,                // Align to the left/top
        ALIGN_TOP       = 1,                // Align to the left/top
        ALIGN_CENTER    = 2,                // Align to the center
        ALIGN_RIGHT     = 3,                // Align to the right/bottom
        ALIGN_BOTTOM    = 3,                // Align to the right/bottom
        _ALIGN_MAX_     = 3
    };

    enum IMAGE_POSITION
    {
        POSITION_CUSTOM     = 0,            // Custom source, zoom
        POSITION_NORMAL     = 1,            // Left upper corner
        POSITION_STRETCH    = 2,            // Stretch to fit entire frame
        POSITION_FITX       = 3,            // Stretch to fit X coordinate
        POSITION_FITY       = 4,            // Stretch to fit Y cooddinate
        POSITION_FITXY      = 5,            // Stretch to fit, but keep aspect ratio
        POSITION_FITSMALL   = 6,            // Stretch to fit smaller (larger clipped, keeps a/r)    
        POSITION_TILE       = 7,            // Tile
        _POSITION_MAX_      = 7
    };
  
//----------------------------
// Constructor and Destructor
//----------------------------      
public:
    CAxImageCanvasST();
    virtual ~CAxImageCanvasST();

//----------------------------
// Operations
//----------------------------  
public:
    BOOL LoadImage(LPCTSTR pszImagePath);
    
    BOOL SetImage(HBITMAP hBitmap);
    BOOL SetImage(CxImage* pImage);

    CxImage* GetImage() const;

    virtual void Update();
    
//    void SetTrackerRect(int x, int y, int w, int h);
//    void SetTrackerRectStyle(UINT uStyle);

//----------------------------
// Properties
//----------------------------  
public:
    void SetPositionMode(int nPosition);  
    int  GetPositionMode() const { return m_nImagePos; }
    
    void SetAlignX(int nAlignX);                         
    void SetAlignY(int nAlignY);                         
    int  GetAlignX() const { return m_nAlignX; }
    int  GetAlignY() const { return m_nAlignY; }
    

    void   SetZoomX(double dblZoomX);        
    void   SetZoomY(double dblZoomY);        
    double GetZoomX() const { return m_dblZoomX; }
    double GetZoomY() const { return m_dblZoomY; }
    
    void SetOriginX(int nOrigX);
    void SetOriginY(int nOrigY);
    int  GetOriginX() const { return m_ptOrigin.x; }
    int  GetOriginY() const { return m_ptOrigin.y; }
            
protected:
    virtual void OnDraw(CDC* pDC);

protected:
    inline void Reset();               // Reset the Zoom, Origin information
    void CalcFactor();

//-----------PHH----------------------
//----------------------------
// Message Handlers
//----------------------------  
protected:
    // Generated message map functions
    //{{AFX_MSG(CAxImageCanvasST)
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        
    BOOL	m_bIsDragging;
    HCURSOR m_hCurCursor;
    CPoint	m_cPointDragBegin;
    
    //^^^^^^^^^^^^PHH^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

public:
    CxImage*          m_pxImgInner;         // Image to display

    int               m_nImagePos;          // Image position

    int               m_nAlignX;            // X alignment
    int               m_nAlignY;            // Y alignment

    double            m_dblZoomX;           // Custom X zoom factor
    double            m_dblZoomY;           // Custom Y zoom factor
   
    CPoint            m_ptOrigin;           // Custom origin (in CAxImageCanvasST coordinates, may be < 0)

    CRect             m_rectDraw;           // Rectangle used to draw the image
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

public://用于漂白背景
	void DeBackground(CPoint point);
private://用于漂白背景
	CRect m_rectTracker;
	static BYTE m_ArrayPoint[PHOTO_ORI_POINTS_X][PHOTO_ORI_POINTS_Y];
	RGBQUAD m_rgbOld;
	int m_nChange;	//敏感度
	void SearchPoint(_int16 x, _int16 y);
	BOOL IsFit(_int16 x, _int16 y);


};

///////////////////////////////////////////////////////////////////////////////
// [Class  ]: CAxTrackImageCanvasST
// [Comment]: 
// [Feature]:  
// [Usage  ]:
// [Related]: 
// [Others ]: 
//
class CAxTrackImageCanvasST : public CAxImageCanvasST
{
public:
    CAxTrackImageCanvasST();
    virtual ~CAxTrackImageCanvasST();

public:
    void SetTrackerRect(CAxRectTracker* pTracker);

    BOOL ShowTrackerRect(BOOL bShow = TRUE);

protected:
    virtual void OnDraw(CDC* pDC);
    
//----------------------------
// Message Handlers
//----------------------------  
protected:
    // Generated message map functions
    //{{AFX_MSG(CAxTrackImageCanvasST)
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);    
    //}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()        
        
private:
    CAxRectTracker*     m_pTracker;
    BOOL                m_bIfShowRect;
};

///////////////////////////////////////////////////////////////////////////////

#endif // _INCLUDE_AXSTATICEX_INCLUDE_