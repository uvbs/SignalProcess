// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Open GUI Library.
//
// [Filename]: GxCaptionBarST.h
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang 
// [Version ]: R0.01
// [Release ]: 2004/03/28
// 
// [Contains]:
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   03/28/04  Leo Y.J. Chang     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_GXCAPTIONBARST_H_
#define _INCLUDE_GXCAPTIONBARST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCaptionBarST : public CStatic
{
    // Construction
public:
    CCaptionBarST();
    
    // Attributes
public:
    
    // Operations
public:
    CString     m_csFontName;
    
    void SetConstantText(const char *pText)	{m_csConstantText = pText;}
    
    int m_fontSize, m_fontWeight;
    BOOL m_grayText;
    COLORREF m_textClr;
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCaptionBarST)
    //}}AFX_VIRTUAL
    
    // Implementation
public:
    virtual ~CCaptionBarST();
    
protected:
    CFont m_captionFont, m_nameFont;
    
    CBitmap m_bm;
    
    CString m_csConstantText;
    
    
    void MakeCaptionBitmap();
    
    // Generated message map functions
protected:
    //{{AFX_MSG(CCaptionBarST)
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//    afx_msg BOOL OnSetText (WPARAM wParam, LPARAM lParam); PHH
    afx_msg LRESULT OnSetText (WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////

#endif // _INCLUDE_GXCAPTIONBARST_H_