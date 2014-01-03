// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: axRectEx.h
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang, John Xu, Cloud Ju
// [Version ]: R0.01
// [Release ]: 2004/01/30
// 
// [Contains]:
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   01/30/04  Leo     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_AXRECTEX_INCLUDE_
#define _INCLUDE_AXRECTEX_INCLUDE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAxRectTracker
{
public:
    // [] Style flags
    //
    enum StyleFlags
    {
        solidLine     = 1,
        dottedLine    = 2,
        hatchedBorder = 4,
        resizeInside  = 8,
        resizeOutside = 16,
        hatchInside   = 32
    };
    
    // [] Hit-Test codes
    //
    enum TrackerHit
    {
        hitNothing     = -1,
        hitTopLeft     = 0,
        hitTopRight    = 1,
        hitBottomRight = 2,
        hitBottomLeft  = 3,
        hitTop         = 4,
        hitRight       = 5,
        hitBottom      = 6,
        hitLeft        = 7,
        hitMiddle      = 8
    };

public:
    CAxRectTracker();
    CAxRectTracker(const RECT* pSrcRect, UINT nStyle);
    virtual ~CAxRectTracker();
    
//----------------------------
// Operations
//----------------------------  
public:
    void Draw(CDC* pDC) const;
    void GetTrueRect(RECT* pTrueRect) const;
    BOOL SetCursor(CWnd* pWnd, UINT nHitTest) const;
    int  HitTest(CPoint point) const;
    int  NormalizeHit(int nHandle) const;
    BOOL Track(CWnd* pWnd, CPoint point, BOOL bAllowInvert = FALSE, CWnd* pWndClipTo = NULL);
    BOOL TrackRubberBand(CWnd* pWnd, CPoint point, BOOL bAllowInvert = TRUE);
   
//----------------------------
// Overridables
//----------------------------      
public:
    virtual void DrawTrackerRect(const RECT* pRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd);
    virtual void AdjustRect(int nHandle, RECT* pRect);
    virtual void OnChangedRect(const CRect& rectOld);
    virtual UINT GetHandleMask() const;
    
//----------------------------
// Helpers
//----------------------------    
protected:
    int HitTestHandles(CPoint point) const;
    void GetHandleRect(int nHandle, CRect* pHandleRect) const;
    void GetModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py);
    virtual int GetHandleSize(const RECT* pRect = NULL) const;
    BOOL TrackHandle(int nHandle, CWnd* pWnd, CPoint point, CWnd* pWndClipTo);
    void Construct();
   
public:    
    // Attributes
    UINT    m_nStyle;       // current state
    CRect   m_rect;         // current position (always in pixels)
    CSize   m_sizeMin;      // minimum X and Y size during track operation
    int     m_nHandleSize;  // size of resize handles (default from WIN.INI)
    
protected:
    BOOL    m_bAllowInvert;    // flag passed to Track or TrackRubberBand
    CRect   m_rectLast;
    CSize   m_sizeLast;
    BOOL    m_bErase;          // TRUE if DrawTrackerRect is called for erasing
    BOOL    m_bFinalErase;     // TRUE if DragTrackerRect called for final erase    
};

//class CAxRectTracker
//{
//public:
//    // Style Flags
//    enum StyleFlags
//    {
//        solidLine = 1, dottedLine = 2, hatchedBorder = 4,
//        resizeInside = 8, resizeOutside = 16, hatchInside = 32,
//    };
//    
//    // Hit-Test codes
//    enum TrackerHit
//    {
//        hitNothing = -1,
//        hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
//        hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
//    };
//    
//public:
//    // Constructors
//    CAxRectTracker();
//    CAxRectTracker(LPCRECT lpSrcRect, UINT nStyle);
//      
//    virtual ~CAxRectTracker();
//    
//public:    
//    // Attributes
//    UINT    m_nStyle;      // current state
//    CRect   m_rect;       // current position (always in pixels)
//    CSize   m_sizeMin;    // minimum X and Y size during track operation
//    int     m_nHandleSize;  // size of resize handles (default from WIN.INI)
//    
//public:
//    // Operations
//    void Draw(CDC* pDC) const;
//    void GetTrueRect(LPRECT lpTrueRect) const;
//    BOOL SetCursor(CWnd* pWnd, UINT nHitTest) const;
//    BOOL Track(CWnd* pWnd, CPoint point, BOOL bAllowInvert = FALSE, CWnd* pWndClipTo = NULL);
//    BOOL TrackRubberBand(CWnd* pWnd, CPoint point, BOOL bAllowInvert = TRUE);
//    int HitTest(CPoint point) const;
//    int NormalizeHit(int nHandle) const;
//    
//    // Overridables
//    virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
//        CDC* pDC, CWnd* pWnd);
//    virtual void AdjustRect(int nHandle, LPRECT lpRect);
//    virtual void OnChangedRect(const CRect& rectOld);
//    virtual UINT GetHandleMask() const;
//        
//protected:
//    BOOL    m_bAllowInvert;    // flag passed to Track or TrackRubberBand
//    CRect   m_rectLast;
//    CSize   m_sizeLast;
//    BOOL    m_bErase;          // TRUE if DrawTrackerRect is called for erasing
//    BOOL    m_bFinalErase;     // TRUE if DragTrackerRect called for final erase
//    
//protected:
//    // implementation helpers
//    int HitTestHandles(CPoint point) const;
//    void GetHandleRect(int nHandle, CRect* pHandleRect) const;
//    void GetModifyPointers(int nHandle, int**ppx, int**ppy, int* px, int*py);
//    virtual int GetHandleSize(LPCRECT lpRect = NULL) const;
//    BOOL TrackHandle(int nHandle, CWnd* pWnd, CPoint point, CWnd* pWndClipTo);
//    void Construct();
//};



///////////////////////////////////////////////////////////////////////////////

#endif // _INCLUDE_AXRECTEX_INCLUDE_