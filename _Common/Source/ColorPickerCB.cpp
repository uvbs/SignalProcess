// ColorPickerCB.cpp
//
// http://www.codeguru.com/combobox//combo_color_picker3.shtml
//
// (c) 1998 James R. Twine
//
// Based On Code That Was Found On www.codeguru.com, And Was 
// ?1997 Baldvin Hansson
//
///////////////////////////////////////////////////////////////////////////////
// Modified by Hans Dietrich  hdietrich2@hotmail.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorPickerCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// NOTE - following table must be kept in sync with XListBox.cpp

SColorAndName CColorPickerCB::ms_pColors[CCB_MAX_COLORS] =
{
	SColorAndName( RGB( 0x00, 0x00, 0x00 ),	"Black" ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xFF ),	"White" ),
	SColorAndName( RGB( 0x80, 0x00, 0x00 ),	"Maroon" ),
	SColorAndName( RGB( 0x00, 0x80, 0x00 ),	"Green" ),
	SColorAndName( RGB( 0x80, 0x80, 0x00 ),	"Olive" ),
	SColorAndName( RGB( 0x00, 0x00, 0x80 ),	"Navy" ),
	SColorAndName( RGB( 0x80, 0x00, 0x80 ),	"Purple" ),
	SColorAndName( RGB( 0x00, 0x80, 0x80 ),	"Teal" ),
	SColorAndName( RGB( 0xC0, 0xC0, 0xC0 ),	"Silver" ),
	SColorAndName( RGB( 0x80, 0x80, 0x80 ),	"Gray" ),
	SColorAndName( RGB( 0xFF, 0x00, 0x00 ),	"Red" ),
	SColorAndName( RGB( 0x00, 0xFF, 0x00 ),	"Lime" ),
	SColorAndName( RGB( 0xFF, 0xFF, 0x00 ),	"Yellow" ),
	SColorAndName( RGB( 0x00, 0x00, 0xFF ),	"Blue" ),
	SColorAndName( RGB( 0xFF, 0x00, 0xFF ),	"Fuchsia" ),
	SColorAndName( RGB( 0x00, 0xFF, 0xFF ),	"Aqua" )
};

CColorPickerCB::CColorPickerCB()
{
}

CColorPickerCB::~CColorPickerCB()
{
}

BEGIN_MESSAGE_MAP(CColorPickerCB, CComboBox)
	//{{AFX_MSG_MAP(CColorPickerCB)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerCB message handlers

int CColorPickerCB::OnCreate(LPCREATESTRUCT pCStruct)
{
	if (CComboBox::OnCreate(pCStruct) == -1)				// If Create Failed
		return -1;											// Return Failure

	Initialize();											// Initialize Contents
	SetCurSel(0);											// Select First Item By Default

	return 0;												// Done!
}

void CColorPickerCB::PreSubclassWindow()
{
	Initialize();											// Initialize Contents

	CComboBox::PreSubclassWindow();							// Subclass Control

	SetCurSel(0);											// Select First Item By Default
}

void CColorPickerCB::Initialize()
{
	int	iAddedItem = -1;

	ResetContent();

	for (int iColor = 0; iColor < CCB_MAX_COLORS; iColor++)			// For All Colors
	{
		iAddedItem = AddString(	ms_pColors[iColor].m_cColor);		// Set Color Name/Text
		if (iAddedItem == CB_ERRSPACE)								// If Not Added
			break;													// Stop
		else														// If Added Successfully
			SetItemData(iAddedItem, ms_pColors[iColor].m_crColor);	// Set Color RGB Value
	}
}

void CColorPickerCB::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	CDC			dcContext;
	CRect		rItemRect(pDIStruct -> rcItem);
	CRect		rBlockRect(rItemRect);
	CRect		rTextRect(rBlockRect);
	int			iColorWidth = (rBlockRect.Width() * 5)/ 12;
	int			iItem = pDIStruct -> itemID;
	int			iState = pDIStruct -> itemState;
	CBrush		brFrameBrush;
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor(COLOR_WINDOW);
	COLORREF	crSelected = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF	crText = GetSysColor(COLOR_WINDOWTEXT);

	if (!dcContext.Attach(pDIStruct -> hDC))				// Attach CDC Object
		return;												// Stop If Attach Failed

	brFrameBrush.CreateStockObject(BLACK_BRUSH);			// Create Black Brush

	if (iState & ODS_SELECTED)								// If Selected
	{														// Set Selected Attributes
		dcContext.SetTextColor((0x00FFFFFF & ~(crText)));	// Set Inverted Text Color (With Mask)
		dcContext.SetBkColor(crSelected);					// Set BG To Highlight Color
		dcContext.FillSolidRect(&rBlockRect, crSelected);	// Erase Item
	}
	else													// If Not Selected
	{														// Set Standard Attributes
		dcContext.SetTextColor(crText);						// Set Text Color
		dcContext.SetBkColor(crNormal);						// Set BG Color
		dcContext.FillSolidRect(&rBlockRect, crNormal);		// Erase Item
	}
	if (iState & ODS_FOCUS)									// If Item Has The Focus
		dcContext.DrawFocusRect(&rItemRect);				// Draw Focus Rect

	//
	//	Calculate Text Area
	//
	rTextRect.left += iColorWidth + 2;						// Set Start Of Text
	rTextRect.top += 2;										// Offset A Bit

	//
	//	Calculate Color Block Area
	//
	rBlockRect.DeflateRect(CSize(2, 2));					// Reduce Color Block Size
	rBlockRect.right = iColorWidth;							// Set Width Of Color Block

	//
	//	Draw Color Text And Block
	//
	if (iItem != -1)										// If Not An Empty Item
	{
		CString	sColor;
		GetLBText(iItem, sColor);							// Get Color Text
		if (iState & ODS_DISABLED)							// If Disabled
		{
			crColor = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
			dcContext.SetTextColor(crColor);				// Set Text Color
		}
		else												// If Normal
			crColor = GetItemData(iItem);					// Get Color Value

		dcContext.SetBkMode(TRANSPARENT);					// Transparent Background
		dcContext.TextOut(rTextRect.left, rTextRect.top, sColor);	// Draw Color Name

		dcContext.FillSolidRect(&rBlockRect, crColor);		// Draw Color

		dcContext.FrameRect(&rBlockRect, &brFrameBrush);	// Draw Frame
	}
	dcContext.Detach();										// Detach DC From Object
}

COLORREF CColorPickerCB::GetSelectedColorValue()
{
	int	iSelectedItem = GetCurSel();						// Get Selected Item

	if (iSelectedItem == CB_ERR)							// If Nothing Selected
		return (RGB(0, 0, 0));								// Return Black

	return (GetItemData(iSelectedItem));					// Return Selected Color
}

UINT CColorPickerCB::GetSelectedColorIndex()
{
	UINT nSelectedItem = GetCurSel();						// Get Selected Item

	if (nSelectedItem == CB_ERR)							// If Nothing Selected
		return 0;
	else
		return nSelectedItem;
}

CString CColorPickerCB::GetSelectedColorName()
{
	int	iSelectedItem = GetCurSel();						// Get Selected Item

	if (iSelectedItem == CB_ERR)							// If Nothing Selected
//		return (m_sColorName = afxEmptyString);				// Return Nothing (Not "Black!")
		return (m_sColorName = "");				//PHH Return Nothing (Not "Black!")

	GetLBText(iSelectedItem, m_sColorName);					// Store Name Of Color

	return m_sColorName;									// Return Selected Color Name
}

void CColorPickerCB::SetSelectedColorValue(COLORREF crClr)
{
	int iItems = GetCount();

	for (int iItem = 0; iItem < iItems; iItem++)
	{
		if (crClr == GetItemData(iItem))					// If Match Found
			SetCurSel(iItem);								// Select It
	}
}

void CColorPickerCB::SetSelectedColorIndex(UINT nIndex)
{
	ASSERT(nIndex >= 0 && nIndex <= 15);
	SetCurSel(nIndex);
}

void CColorPickerCB::SetSelectedColorName(LPCTSTR cpColor)
{
	int		iItems = GetCount();
	CString	sColorName;

	for (int iItem = 0; iItem < iItems; iItem++)
	{
		GetLBText(iItem, sColorName);						// Get Color Name

		if (!sColorName.CompareNoCase(cpColor))				// If Match Found
			SetCurSel(iItem);								// Select It
	}
}

BOOL CColorPickerCB::RemoveColor(LPCTSTR cpColor)
{
	int		iItems = GetCount();
	CString	sColor;
	BOOL	bRemoved = FALSE;

	for (int iItem = 0; iItem < iItems; iItem++)
	{
		GetLBText(iItem, sColor);							// Get Color Name
		if (!sColor.CompareNoCase(cpColor))					// If Match Found
		{
			DeleteString(iItem);							// Remove It
			bRemoved = TRUE;								// Set Flag
			break;											// Stop Checking
		}
	}
	return bRemoved;										// Done!
}

BOOL CColorPickerCB::RemoveColor(COLORREF crClr)
{
	int		iItems = GetCount();
	BOOL	bRemoved = FALSE;

	for (int iItem = 0; iItem < iItems; iItem++)
	{
		if (crClr == GetItemData(iItem))					// If Desired Color Found
		{
			DeleteString(iItem);							// Remove It
			bRemoved = TRUE;								// Set Flag
			break;											// Stop Checking
		}
	}
	return bRemoved;										// Done!
}

int	CColorPickerCB::AddColor(LPCTSTR cpName, COLORREF crColor)
{
	int		iItem = -1;

	iItem = InsertString(-1, cpName);						// Insert String
	if (iItem != LB_ERR)									// If Insert Good
		SetItemData(iItem, crColor);						// Set Color Value

	return iItem;											// Done! Return Location
}

void DDX_ColorPicker(CDataExchange* pDX, int nIDC_, COLORREF& crColor)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC_);

	ASSERT(hWndCtrl);

	CColorPickerCB* pPicker = (CColorPickerCB*) CWnd::FromHandle(hWndCtrl);
	ASSERT(pPicker);

	if (!pDX->m_bSaveAndValidate)
	{
		pPicker->SetSelectedColorValue(crColor);
	}
	else
	{
		crColor = pPicker->GetSelectedColorValue();
	}
}

void DDX_ColorPicker(CDataExchange* pDX, int nIDC_, CString& strName)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC_);
	ASSERT(hWndCtrl);

	CColorPickerCB* pPicker = (CColorPickerCB*) CWnd::FromHandle(hWndCtrl);
	ASSERT(pPicker);

	if (!pDX->m_bSaveAndValidate)
	{
		pPicker->SetSelectedColorName(strName);
	}
	else
	{
		strName = pPicker->GetSelectedColorName();
	}
}
