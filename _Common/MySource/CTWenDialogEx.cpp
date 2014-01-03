// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#include "CTWenDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CTWenDialogEx, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CTWenDialogEx dialog

#pragma warning(disable : 4355)

CTWenDialogEx::CTWenDialogEx()
{
	CommonConstruct();
}

CTWenDialogEx::CTWenDialogEx(UINT nIDTemplate, CWnd *pParent/*= NULL*/) : CDialog(nIDTemplate, pParent)
{
	CommonConstruct();
}

CTWenDialogEx::CTWenDialogEx(LPCTSTR lpszTemplateName, CWnd *pParentWnd/*= NULL*/) : CDialog(lpszTemplateName, pParentWnd)
{
	CommonConstruct();
}

void CTWenDialogEx::CommonConstruct()
{
	m_hBkgrBitmap = NULL;
	m_sizeBkgrBitmap = CSize(0, 0);
	m_BkgrLocation = (BackgroundLocation) -1;
	m_bAutoDestroyBmp = FALSE;
}

void CTWenDialogEx::SetBackgroundColor(COLORREF color, BOOL bRepaint)
{
	if (m_brBkgr.GetSafeHandle() != NULL)
	{
		m_brBkgr.DeleteObject();
	}

	if (color != (COLORREF)-1)
	{
		m_brBkgr.CreateSolidBrush(color);
	}

	if (bRepaint && GetSafeHwnd() != NULL)
	{
		Invalidate();
		UpdateWindow();
	}
}

void CTWenDialogEx::SetBackgroundImage(HBITMAP hBitmap, BackgroundLocation location, BOOL bAutoDestroy, BOOL bRepaint)
{
	if (m_bAutoDestroyBmp && m_hBkgrBitmap != NULL)
	{
		::DeleteObject(m_hBkgrBitmap);
	}

	m_hBkgrBitmap = hBitmap;
	m_BkgrLocation = location;
	m_bAutoDestroyBmp = bAutoDestroy;

	if (hBitmap != NULL)
	{
		BITMAP bmp;
		::GetObject(hBitmap, sizeof(BITMAP), (LPVOID) &bmp);

		m_sizeBkgrBitmap = CSize(bmp.bmWidth, bmp.bmHeight);
	}
	else
	{
		m_sizeBkgrBitmap = CSize(0, 0);
	}

	if (bRepaint && GetSafeHwnd() != NULL)
	{
		Invalidate();
		UpdateWindow();
	}
}

BOOL CTWenDialogEx::SetBackgroundImage(UINT uiBmpResId, BackgroundLocation location, BOOL bRepaint)
{
	HBITMAP hBitmap = NULL;

	if (uiBmpResId != 0)
	{
		hBitmap = ::LoadBitmap(AfxFindResourceHandle(MAKEINTRESOURCE(uiBmpResId), RT_BITMAP), 
			MAKEINTRESOURCE(uiBmpResId));
		if (hBitmap == NULL)
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	SetBackgroundImage(hBitmap, location, TRUE /* Autodestroy */, bRepaint);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CTWenDialogEx, CDialog)
	//{{AFX_MSG_MAP(CTWenDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTWenDialogEx message handlers

BOOL CTWenDialogEx::OnEraseBkgnd(CDC* pDC)
{
	if (m_brBkgr.GetSafeHandle() == NULL && m_hBkgrBitmap == NULL)
	{
		return CDialog::OnEraseBkgnd(pDC);
	}

	ASSERT_VALID(pDC);

	CRect rectClient;
	GetClientRect(rectClient);

	if (m_BkgrLocation != BACKGR_TILE || m_hBkgrBitmap == NULL)
	{
		if (m_brBkgr.GetSafeHandle() != NULL)
		{
			pDC->FillRect(rectClient, &m_brBkgr);
		}
		else
		{
			CDialog::OnEraseBkgnd(pDC);
		}
	}

	if (m_hBkgrBitmap == NULL)
	{
		return TRUE;
	}

	ASSERT(m_sizeBkgrBitmap != CSize(0, 0));

	if (m_BkgrLocation != BACKGR_TILE)
	{
		CPoint ptImage = rectClient.TopLeft();

		switch (m_BkgrLocation)
		{
		case BACKGR_TOPLEFT:
			break;

		case BACKGR_TOPRIGHT:
			ptImage.x = rectClient.right - m_sizeBkgrBitmap.cx;
			break;

		case BACKGR_BOTTOMLEFT:
			ptImage.y = rectClient.bottom - m_sizeBkgrBitmap.cy;
			break;

		case BACKGR_BOTTOMRIGHT:
			ptImage.x = rectClient.right - m_sizeBkgrBitmap.cx;
			ptImage.y = rectClient.bottom - m_sizeBkgrBitmap.cy;
			break;
		}

		pDC->DrawState(ptImage, m_sizeBkgrBitmap, m_hBkgrBitmap, DSS_NORMAL);
	}
	else
	{
		// Tile background image:
		for (int x = rectClient.left; x < rectClient.Width(); x += m_sizeBkgrBitmap.cx)
		{
			for (int y = rectClient.top; y < rectClient.Height(); y += m_sizeBkgrBitmap.cy)
			{
				pDC->DrawState(CPoint(x, y), m_sizeBkgrBitmap, m_hBkgrBitmap, DSS_NORMAL);
			}
		}
	}

	return TRUE;
}

void CTWenDialogEx::OnDestroy()
{
	if (m_bAutoDestroyBmp && m_hBkgrBitmap != NULL)
	{
		::DeleteObject(m_hBkgrBitmap);
		m_hBkgrBitmap = NULL;
	}

	CDialog::OnDestroy();
}
