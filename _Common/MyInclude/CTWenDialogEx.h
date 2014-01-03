// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTWenDialogEx dialog

class CTWenDialogEx : public CDialog
{
	DECLARE_DYNAMIC(CTWenDialogEx)

// Construction
public:
	CTWenDialogEx();
	CTWenDialogEx(UINT nIDTemplate, CWnd *pParent = NULL);
	CTWenDialogEx(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);

protected:
	void CommonConstruct();

// Attributes:
public:
	enum BackgroundLocation
	{
		BACKGR_TILE,
		BACKGR_TOPLEFT,
		BACKGR_TOPRIGHT,
		BACKGR_BOTTOMLEFT,
		BACKGR_BOTTOMRIGHT,
	};

protected:
	HBITMAP            m_hBkgrBitmap;
	CSize              m_sizeBkgrBitmap;
	CBrush             m_brBkgr;
	BackgroundLocation m_BkgrLocation;
	BOOL               m_bAutoDestroyBmp;

// Operations:
public:
	void SetBackgroundColor(COLORREF color, BOOL bRepaint = TRUE);
	void SetBackgroundImage(HBITMAP hBitmap, BackgroundLocation location = BACKGR_TILE, BOOL bAutoDestroy = TRUE, BOOL bRepaint = TRUE);
	BOOL SetBackgroundImage(UINT uiBmpResId, BackgroundLocation location = BACKGR_TILE, BOOL bRepaint = TRUE);

// Implementation
protected:
	//{{AFX_MSG(CTWenDialogEx)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};