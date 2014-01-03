// ColorPickerCB.h
//
// http://www.codeguru.com/combobox//combo_color_picker3.shtml
//
// (c) 1998 James R. Twine
//
// Based On Code That Was Found On www.codeguru.com, And Was 
// © 1997 Baldvin Hansson
//
///////////////////////////////////////////////////////////////////////////////
// Modified by Hans Dietrich  hdietrich2@hotmail.com
///////////////////////////////////////////////////////////////////////////////

#ifndef COLORPICKERCB_H
#define COLORPICKERCB_H

#define		CCB_MAX_COLORS			16						// Colors In List
#define		CCB_MAX_COLOR_NAME		16						// Max Chars For Color Name - 1

//
//	Internal Structure For Color/Name Storage...
//
struct	SColorAndName
{
	SColorAndName()
	{
		ZeroMemory(this, sizeof(SColorAndName));			// Init Structure
	};
	SColorAndName(COLORREF crColor, LPCTSTR cpColor)
	{
		ZeroMemory(this, sizeof(SColorAndName));			// Init Structure
		m_crColor = crColor;								// Set Color RGB Value
		strncpy(m_cColor, cpColor, CCB_MAX_COLOR_NAME);		// Set Color Name
	};
	COLORREF	m_crColor;									// Actual Color RGB Value
	char		m_cColor[CCB_MAX_COLOR_NAME];				// Actual Name For Color
};

// Gets/sets color value from/to COLORREF member variable
void DDX_ColorPicker(CDataExchange* pDX_, int nIDC_, COLORREF& crColor_);

// Gets/sets color name from/to CString member variable
void DDX_ColorPicker(CDataExchange* pDX_, int nIDC_, CString& strName_);

class CColorPickerCB : public CComboBox
{
// Construction
public:
	CColorPickerCB();
	virtual	~CColorPickerCB();

// Attributes
private:
	CString			m_sColorName;							// Name Of Selected Color

private:
	void			Initialize(void);						// Initialize Control/Colors

public:
	static
	SColorAndName	ms_pColors[CCB_MAX_COLORS];				// Array Of Colors And Names
	COLORREF		GetSelectedColorValue(void);			// Get Selected Color Value
	UINT			GetSelectedColorIndex(void);			// Get Selected Color Index
	CString			GetSelectedColorName( void );			// Get Selected Color Name

	void			SetSelectedColorValue(COLORREF crClr);	// Set Selected Color Value
	void			SetSelectedColorIndex(UINT nIndex);		// Set Selected Color Index
	void			SetSelectedColorName(LPCTSTR cpColor);	// Set Selected Color Name

	BOOL			RemoveColor(LPCTSTR cpColor);				// Remove Color From List
	BOOL			RemoveColor(COLORREF crClr);			// Remove Color From List
	int				AddColor(LPCTSTR cpNam, COLORREF crClr);	// Insert A New Color

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerCB)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPickerCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //COLORPICKERCB_H
