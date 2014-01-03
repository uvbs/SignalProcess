
#ifndef TWEN_SDN_CSimpleDraw
#define TWEN_SDN_CSimpleDraw

//如果在多个Instance中使用（如DLL中使用，然后在主工程中同时使用），需要在stdafx.h定义CSimpleButton_MultiInstance宏

class CSimpleElement;

class CSimpleDraw
{
public:
	virtual void Draw(const CSimpleElement& element)=0;
};

class CSimpleText
{
public:
	enum enumDrawType_Offset
	{
		OFFSET_LEFT	= 0X00000001,//rcOffset.left有效
		OFFSET_TOP	= 0X00000002,
		OFFSET_RIGHT	= 0X00000004,
		OFFSET_BOTTOM	= 0X00000008,

		OFFSET_ALL = OFFSET_LEFT | OFFSET_TOP | OFFSET_RIGHT | OFFSET_BOTTOM,
	};

public:
	CSimpleText():DrawType(DT_CENTER|DT_VCENTER),DrawType_Offset(NULL),DefaultFont(NULL){}
	
	void SetOffset(DWORD offset, const RECT& rc)
	{
		DrawType_Offset = offset;
		if((offset & OFFSET_LEFT) == OFFSET_LEFT) rcOffset.left = rc.left;
		if((offset & OFFSET_TOP) == OFFSET_TOP) rcOffset.top = rc.top;
		if((offset & OFFSET_RIGHT) == OFFSET_RIGHT) rcOffset.right = rc.right;
		if((offset & OFFSET_BOTTOM) == OFFSET_BOTTOM) rcOffset.bottom = rc.bottom;
	}
	void SetOffset(DWORD offset, int l, int t, int r, int b)
	{
		SetOffset(offset, CRect(l,t,r,b));
	}
	void GetResult(CRect& rcResult, const CRect& rcSource, CDC* pDC = NULL) const//暂时只支持offset
	{
		rcResult = rcSource;
		if((DrawType_Offset & OFFSET_LEFT) == OFFSET_LEFT) rcResult.left += rcOffset.left;
		if((DrawType_Offset & OFFSET_TOP) == OFFSET_TOP) rcResult.top += rcOffset.top;
		if((DrawType_Offset & OFFSET_RIGHT) == OFFSET_RIGHT) rcResult.right -= rcOffset.right;
		if((DrawType_Offset & OFFSET_BOTTOM) == OFFSET_BOTTOM) rcResult.bottom -= rcOffset.bottom;
	}

public:
	CString Text;
	DWORD DrawType;
	CFont*	DefaultFont;//当DefaultFont不空时，优先使用
	struct structTextFont
	{
		structTextFont():Size(12),Name(_T("宋体")),Bold(false),Italic(false),Underline(false),StrikeOut(false),Escapement(0),Orientation(0),
			Color(NULL),Transparent(true),BkColor(NULL){}
		int Size;//字体大小
		CString Name;//字体名称
		bool Bold;
		bool Italic;
		bool Underline;
		bool StrikeOut;
		int Escapement;
		int Orientation;

		COLORREF Color;
		bool Transparent;
		COLORREF BkColor;
	}Font;

private:
	CRect		rcOffset;
	DWORD		DrawType_Offset;
};

class CSimpleDrawType
{
public:
	enum enumOrder
	{
		ORDER_0 = 0,
		ORDER_1,
		ORDER_2,
		ORDER_3,
		ORDER_4,
		ORDER_5,

		ORDER_COUNT,
	};
	enum enumType
	{
		TYPE_NONE,	//目标区域无操作
		TYPE_SOURCE,//目标区域同源区域,rcResult=rcSource
		TYPE_OFFSET,//目标区域四边缩进rcResult.left+=rcOffset.left,rcResult.right-=rcOffset.right
		TYPE_BASIC,
	};

	enum enumDrawType_Offset
	{
		OFFSET_LEFT	= 0X00000001,//rcOffset.left有效
		OFFSET_TOP	= 0X00000002,
		OFFSET_RIGHT	= 0X00000004,
		OFFSET_BOTTOM	= 0X00000008,

		OFFSET_ALL = OFFSET_LEFT | OFFSET_TOP | OFFSET_RIGHT | OFFSET_BOTTOM,
	};

	enum enumDrawType_Basic
	{
		LEFT = 0X00000001,
		CENTER = 0X00000002,
		RIGHT = 0X00000004,
		STRETCHX = 0X00000008,
		REPEATX = 0X00000010,
		FITX = 0X00000020,//reserved

		X_MASK = 0X0000FFFF,

		TOP = 0X00010000,
		MIDDLE = 0X00020000,
		VCENTER = 0X00040000,
		BOTTOM = 0X00080000,
		STRETCHY = 0X00100000,
		REPEATY = 0X00200000,
		FITY = 0X00400000,//reserved

		Y_MASK = 0XFFFF0000,
	};
public:
	void SetOrder(enumOrder order, enumType type){Orders[order] = type;}
	void ClearOrder(){for(int i=0; i<ORDER_COUNT; i++) SetOrder((enumOrder)i, TYPE_NONE);}

	void SetOffset(DWORD offset, const RECT& rc)
	{
		DrawType_Offset = offset;
		if((offset & OFFSET_LEFT) == OFFSET_LEFT) rcOffset.left = rc.left;
		if((offset & OFFSET_TOP) == OFFSET_TOP) rcOffset.top = rc.top;
		if((offset & OFFSET_RIGHT) == OFFSET_RIGHT) rcOffset.right = rc.right;
		if((offset & OFFSET_BOTTOM) == OFFSET_BOTTOM) rcOffset.bottom = rc.bottom;
	}
	void SetOffset(DWORD offset, int l, int t, int r, int b)
	{
		SetOffset(offset, CRect(l,t,r,b));
	}
	DWORD GetOffset() const{return DrawType_Offset;}

	void SetBasic(DWORD basic){DrawType_Basic = basic;}
	DWORD GetBasic() const{return DrawType_Basic;}

	void GetResult(CRect& rcResult, CRect& rcResultImage, const CRect& rcSource, const CRect& rcImage) const;

private:
	enumType	Orders[ORDER_COUNT];
	CRect		rcOffset;
	DWORD		DrawType_Offset;
	DWORD		DrawType_Basic;

public:
	CSimpleDrawType():DrawType_Basic(CENTER|VCENTER),DrawType_Offset(NULL),rcOffset(0,0,0,0)
	{
		SetOrder(ORDER_0, TYPE_BASIC);
		SetOrder(ORDER_1, TYPE_OFFSET);
		for(int i=2; i<ORDER_COUNT; i++) SetOrder((enumOrder)i, TYPE_NONE);
	}
	CSimpleDrawType(const CSimpleDrawType& cdt){*this = cdt;}
	CSimpleDrawType& operator=(const CSimpleDrawType& cdt)
	{
		memcpy(Orders, cdt.Orders, sizeof(enumType)*ORDER_COUNT);
		rcOffset = cdt.rcOffset;
		DrawType_Offset = cdt.DrawType_Offset;
		DrawType_Basic = cdt.DrawType_Basic;

		return *this;
	}
};

class CSimpleBorder;
class CSimpleSingleBorder
{
private:
	CSimpleSingleBorder():Width(NULL),Color(NULL),BorderOffset(true){}
	friend class CSimpleBorder;

public:
	int Width;
	COLORREF Color;
	bool BorderOffset;
};

class CSimpleGround;
class CSimpleBorder
{
private:
	CSimpleBorder(){}
	friend class CSimpleGround;

public:
	void SetBorderWidth(int width)
	{
		Left.Width = width;
		Top.Width = width;
		Right.Width = width;
		Bottom.Width = width;
	}

	void SetBorderColor(COLORREF color)
	{
		Left.Color = color;
		Top.Color = color;
		Right.Color = color;
		Bottom.Color = color;
	}

	void SetBorderOffset(bool borderOffset)
	{
		Left.BorderOffset = borderOffset;
		Top.BorderOffset = borderOffset;
		Right.BorderOffset = borderOffset;
		Bottom.BorderOffset = borderOffset;
	}

public:
	CSimpleSingleBorder Left,Top,Right,Bottom;
};

class CSimpleGround
{
public:
	enum enumType
	{
		TYPE_NONE,
		TYPE_COLOR,
		TYPE_COLORS,
		TYPE_IMAGE,
	};

	enum enumColorsLayoutType
	{
		CLT_ROW,
		CLT_COLUMN,
	};
public:
	CSimpleGround():Type(TYPE_NONE){}

public:
	enumType Type;
	COLORREF Color;
	struct structSimpleGroundColors
	{
		structSimpleGroundColors():Count(0),Colors(NULL){}
		void SetColors(DWORD dwCount, enumColorsLayoutType clt = CLT_COLUMN, COLORREF* pColors = NULL)
		{
			Type = clt;
			Count = dwCount;
			if(Colors) delete Colors;
			Colors = new COLORREF[Count];
			if(pColors) memcpy(Colors, pColors, sizeof(COLORREF) * dwCount);
		}
		void SetColor(DWORD dwIndex, COLORREF color){if((Colors == NULL) || (dwIndex >= Count)) return;Colors[dwIndex] = color;}
		enumColorsLayoutType Type;
		DWORD Count;
		COLORREF* Colors;
	}Colors;
	struct structSimpleGroundImage
	{
		structSimpleGroundImage():Bitmap(NULL),Transparent(true),TransparentColor(NULL),Rect(-1,-1,-1,-1){}
		CBitmap* Bitmap;
		bool Transparent;
		COLORREF TransparentColor;
		CRect Rect;
	}Image;
	CSimpleBorder Border;
};

class CSimpleElement
{
public:
	CSimpleGround EraseGround;
	CSimpleGround BackGround;
	CSimpleGround ForeGround;
	CSimpleDrawType DrawType;
	CSimpleText Text;
	CRect Rect;
/*
public:
	void Draw(CSimpleDraw* pSimpleDraw)
	{
		if(!pSimpleDraw) return;
		pSimpleDraw->Draw(*this);
	}//*/
};

class CSimpleDrawDC : public CSimpleDraw
{
public:
	CSimpleDrawDC(CDC* pDC):m_pSimpleDrawDC(pDC){}

public:
	virtual void Draw(const CSimpleElement& element);
	void Draw(const CSimpleElement& element, CDC* pSimpleDrawDC);
	void Draw(const CSimpleGround* EraseGround, const CSimpleDrawType* DrawType_EG,
				const CSimpleGround* BackGround, const CSimpleDrawType* DrawType_BG,
				const CSimpleGround* ForeGround, const CSimpleDrawType* DrawType_FG,
				const CSimpleText* Text, const CRect& Rect, CDC* pSimpleDrawDC);
	void DrawEx(const CSimpleGround* EraseGround, const CSimpleDrawType* DrawType_EG,
		const CSimpleGround* BackGround, const CSimpleDrawType* DrawType_BG,
		const CSimpleGround* ForeGround, const CSimpleDrawType* DrawType_FG,
		const CRect& GroundRect,
		const CSimpleText* Text,  const CRect& TextRect,
		CDC* pSimpleDrawDC);
protected:
	CDC* m_pSimpleDrawDC;

public:
	static void DrawLine(CDC* pDC, COLORREF color, int lineWidth, int style, int x1, int y1, int x2, int y2);
	static void DrawGround(const CSimpleGround& Ground, const CRect& Rect, const CSimpleDrawType& DrawType, CDC* pDC);
	static void DrawText(const CSimpleText& Text, const CRect& Rect, const CSimpleDrawType& DrawType, CDC* pDC);

	static void DrawBitmap(CDC* pDC, const CRect& _rcDC, const CSimpleDrawType& _drawType, CBitmap* pBitmap, const CRect& _rcImage, bool bTransparent, COLORREF crTransparent);
	static void EraseRect(CDC* pDC, const CRect& rc, COLORREF color);
	static void DrawRepeatX(CDC* pDC, int t, int h, int l, int dcW, CBitmap* pBitmap, const CRect& rcImage);
	static void DrawRepeatY(CDC* pDC, int l, int w, int t, int dcH, CBitmap* pBitmap, const CRect& rcImage);
	static void DrawRepeatXY(CDC* pDC, const CRect& rc, CBitmap* pBitmap, const CRect& rcImage);
};

class CSimpleButton : public CButton, CSimpleDrawDC
{
	DECLARE_DYNAMIC(CSimpleButton)

	enum enumButtonType
	{
		Type_Normal,
		Type_CheckBox,//自绘按钮不支持默认的CheckBox，Radio，3State，GroupBox行为方式，暂不支持
		Type_Radio,
		Type_3State,
		Type_GroupBox,
	};
public:
	enum enumCustomButtonStatus
	{
		CBS_NORMAL		=0X00,
		//CBS_SELECTED,
		//CBS_GRAYED,
		CBS_DISABLED,
		//CBS_CHECKED,
		CBS_FOCUS,
		//CBS_DEFAULT,
		CBS_OVER,
		CBS_PRESSED,

		CBS_COUNT,
	};
	struct structStatus
	{
		CSimpleGround ForeGround;
		CSimpleText Text;
	};
	typedef structStatus* structStatusPointer;
public:
	CSimpleButton();
	virtual ~CSimpleButton(){ResetSwitch();}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSimpleButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnClicked();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetCustomButton(bool bCustonButton){m_bDefautUI = !bCustonButton;}
	void SetXPButton(bool bXPButton){XPButton = bXPButton;}

public://更详细的设置可以通过Values[CBS]进行
	void SetUsingStatusText(bool bUsingStatusText){for(DWORD i=0; i<CBS_COUNT; i++) m_pbUsingStatusText[i] = bUsingStatusText;}

public://开关0
	void SetText(LPCTSTR text);
	void SetBitmap(CBitmap* Normal, CBitmap* Disabled, CBitmap* Focus, CBitmap* Over, CBitmap* Pressed);
	void SetColor(COLORREF Normal, COLORREF Disabled, COLORREF Focus, COLORREF Over, COLORREF Pressed);
	void SetTextDrawType(DWORD Normal, DWORD Disabled, DWORD Focus, DWORD Over, DWORD Pressed);
	void SetTextOffset(int l, int t, int r, int b);
	void SetOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
		const CPoint& indexNormal, const CPoint& indexDisabled, const CPoint& indexFocus, const CPoint& indexOver, const CPoint& indexPressed);
	void SetOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
		UINT XNormal, UINT YNormal, 
		UINT XDisabled, INT YDisabled, 
		UINT XFocus, UINT YFocus, 
		UINT XOver, UINT YOver, 
		UINT XPressed, UINT YPressed);
//	void Set3Dborder(bool b3DBorder, COLORREF crLight, COLORREF crBlack){m_bAuto3D = b3DBorder;m_crLight = crLight;m_crBlack = crBlack;}

public://多开关控制，其他开关从1开始
	void SetSwitches(DWORD dwCount);//不包含开关0的开关个数
	void SetCurrentSwitch(DWORD dwSwitch){m_dwCurrentSwitch = dwSwitch;}
	DWORD GetCurrentSwitch() const { return m_dwCurrentSwitch;}

	void SetSwitchText(LPCTSTR text, DWORD dwSwitch = -1);
	void SetSwitchBitmap(CBitmap* Normal, CBitmap* Disabled, CBitmap* Focus, CBitmap* Over, CBitmap* Pressed, DWORD dwSwitch = -1);
	void SetSwitchColor(COLORREF Normal, COLORREF Disabled, COLORREF Focus, COLORREF Over, COLORREF Pressed, DWORD dwSwitch = -1);
	void SetSwitchTextDrawType(DWORD Normal, DWORD Disabled, DWORD Focus, DWORD Over, DWORD Pressed, DWORD dwSwitch = -1);
	void SetSwitchTextOffset(int l, int t, int r, int b, DWORD dwSwitch = -1);
	void SetSwitchOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
		const CPoint& indexNormal, const CPoint& indexDisabled, const CPoint& indexFocus, const CPoint& indexOver, const CPoint& indexPressed, DWORD dwSwitch = -1);
	void SetSwitchOneBitmap(CBitmap* pBitmap, long smallWidth, long smallHeight,
		UINT XNormal, UINT YNormal, 
		UINT XDisabled, INT YDisabled, 
		UINT XFocus, UINT YFocus, 
		UINT XOver, UINT YOver, 
		UINT XPressed, UINT YPressed, DWORD dwSwitch = -1);
	void SetSwitchTransparent(bool bTransparent, COLORREF crTransparent, DWORD dwSwitch = -1);
	structStatus* GetSwitch(DWORD dwSwitch);

public:
	void SetToolTip(LPCTSTR tooltip=NULL,BOOL bActivate=TRUE);
	void ActivateTooltip(BOOL bActivate=TRUE);
protected:
	void InitToolTip();
	CToolTipCtrl m_ToolTip;

public:
	structStatus Status[CBS_COUNT];
	CSimpleGround EraseGround;
	CSimpleGround BackGround;
	bool XPButton;//是否是xp风格按钮，如果是，则BackGround无效
	CSimpleDrawType DrawType_EG,DrawType_BG,DrawType_FG;

	bool m_pbUsingStatusText[CBS_COUNT];//true:Status[cbs]的文本画图；false：使用按钮的窗体文本画图

	bool m_bDefautUI;//是否使用系统的按钮行为

	bool m_bDrawCheckBox,m_bDrawRadio;

protected:
	enumButtonType m_enumType;
	BOOL m_bTracking;

	//按钮的状态
	bool m_pbStatus[CBS_COUNT];
//	bool m_bAuto3D;//显示3D边框
//	COLORREF m_crLight, m_crBlack;

	DWORD m_dwSwitches;//不包括开关0
	DWORD m_dwCurrentSwitch;
	structStatus** m_ppSwitches;

protected:
	void ResetSwitch();
#ifndef CSimpleButton_MultiInstance
	static CPen m_BoundryPen,m_InsideBoundryPenLeft,m_InsideBoundryPenRight,m_InsideBoundryPenTop,m_InsideBoundryPenBottom;
	static CBrush m_FillActive,m_FillInactive;
	static CPen m_InsideBoundryPenLeftSel,m_InsideBoundryPenTopSel,m_InsideBoundryPenRightSel,m_InsideBoundryPenBottomSel;
	static BOOL m_bOver,m_bSelected,m_bFocus;
#else
	CPen m_BoundryPen,m_InsideBoundryPenLeft,m_InsideBoundryPenRight,m_InsideBoundryPenTop,m_InsideBoundryPenBottom;
	CBrush m_FillActive,m_FillInactive;
	CPen m_InsideBoundryPenLeftSel,m_InsideBoundryPenTopSel,m_InsideBoundryPenRightSel,m_InsideBoundryPenBottomSel;
	BOOL m_bOver,m_bSelected,m_bFocus;
#endif
	void DoGradientFill(CDC *pDC, CRect* rect);
	void DrawInsideBorder(CDC *pDC, CRect* rect);
};

class CSimpleEdit : public CEdit
{
public:
	enum enumCustomEditIndex
	{
		CEI_Normal=0x00,
		CEI_Focused=0x01,
		CEI_ReadOnly=0x02,
		CEI_Disabled=0x03
	};
	typedef void (*CSimpleEdit_EventCallback)(CSimpleEdit* pEdit, DWORD dwExtraData, DWORD dwFlagData);

public:
	CSimpleEdit();
	virtual ~CSimpleEdit();

public:// Operations
	void SetTextFont(CFont*font,BOOL bRepaint=FALSE);
	///
	void SetTextBkInfo(BOOL bTransparent=TRUE,
		COLORREF crNormal=RGB(0,0,0),
		COLORREF crFocused=RGB(0,0,0),
		COLORREF crReadOnly=RGB(0,0,0),
		COLORREF crDisabled=RGB(0,0,0),
		BOOL bRepaint=FALSE);
	void EnableEdit(BOOL bEnable,BOOL bRepaint=FALSE);
	void SetTextColor(COLORREF crNormal,COLORREF crFocused,
		COLORREF crReadOnly,COLORREF crDisabled,
		BOOL bRepaint=FALSE);
	void SetBkColor(COLORREF crNormal,COLORREF crFocused,
		COLORREF crReadOnly,COLORREF crDisabled,
		BOOL bRepaint=FALSE);

	void SetEvent_OnContentChanged(DWORD dwWaitingMilliseconds, DWORD dwExtraData, DWORD dwFlagData, CSimpleEdit_EventCallback cb)
	{
		m_dwWaitingMilliseconds = dwWaitingMilliseconds;
		m_dwExtraData_OnContentChanged = dwExtraData;
		m_dwFlagData_OnContentChanged = dwFlagData;
		m_OnContentChanged = cb;
	}

public:// properties
	bool UsingCustomUI;

protected:
	BYTE GetStatus();

protected:
	//状态
	BOOL m_bDisabled;//并非状态变量
	BOOL m_bReadOnly;//并非状态变量
	BOOL m_bFocused;//并非状态变量

	CFont*m_pTextFont;
	///
	BOOL m_bTransparent;
	COLORREF m_crTextColor[4];
	COLORREF m_crTextBkColor[4];
	COLORREF m_crBkColor[4];
	CBrush m_BrushBk[4];

	DWORD m_dwWaitingMilliseconds;//0:表示不等待
	DWORD m_dwExtraData_OnContentChanged;
	DWORD m_dwFlagData_OnContentChanged;	
	CSimpleEdit_EventCallback m_OnContentChanged;
	CString m_oldText;

	//{{AFX_MSG(CSimpleEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEnChange();
	afx_msg void OnTimer(UINT timerID);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSimpleEdit2
/////////////////////////////////////////////////////////////////////////////
class CSimpleEdit2 : public CEdit
{
public:
	typedef void (*CSimpleEdit2_EventCallback)(CSimpleEdit2* pEdit, DWORD dwExtraData, DWORD dwFlagData);

public:
	CSimpleEdit2();
	virtual ~CSimpleEdit2();

public:
	void SetSwitches(DWORD _dwSwitches);
	void SetCurrentSwitch(DWORD dwSwitch, bool bRepaint=true);
	DWORD GetCurrentSwitch() const { return dwCurrentSwitch;}
	bool SetCurrentEditSwitch(DWORD dwSwitch);
	void SetEvent_OnContentChanged(DWORD dwExtraData, DWORD dwFlagData, CSimpleEdit2_EventCallback cb)
	{
		dwExtraData_OnContentChanged = dwExtraData;
		dwFlagData_OnContentChanged = dwFlagData;
		OnContentChanged = cb;
	}
	void GetEvent_OnContentChanged(DWORD& dwExtraData, DWORD& dwFlagData, CSimpleEdit2_EventCallback& cb)
	{
		dwExtraData = dwExtraData_OnContentChanged;
		dwFlagData = dwFlagData_OnContentChanged;
		cb = OnContentChanged;
	}

public:// Operations
	void SetTextFont(DWORD dwSwitch, CFont*font, BOOL bRepaint=FALSE);//dwSwitch:-1则CurrentEditSwitch
	void SetTextBkInfo(DWORD dwSwitch, BOOL bTransparent=TRUE, COLORREF cr=NULL,BOOL bRepaint=FALSE);//dwSwitch:-1则CurrentEditSwitch
	void SetTextColor(DWORD dwSwitch, COLORREF cr, BOOL bRepaint=FALSE);//dwSwitch:-1则CurrentEditSwitch
	void SetBkInfo(DWORD dwSwitch, BOOL bTransparent, COLORREF cr, BOOL bRepaint=FALSE);//dwSwitch:-1则CurrentEditSwitch

protected:
	DWORD dwSwitches;
	DWORD dwCurrentSwitch;
	DWORD dwCurrentEditSwitch;
	DWORD dwExtraData_OnContentChanged;
	DWORD dwFlagData_OnContentChanged;	
	CSimpleEdit2_EventCallback OnContentChanged;

	struct structSwitch
	{
		CFont*m_pTextFont;
		///
		BOOL m_bTextBkTransparent;
		COLORREF m_crTextColor;
		COLORREF m_crTextBkColor;
		BOOL m_bBkTransparent;
		COLORREF m_crBkColor;
		CBrush m_BrushBk;
	}*Switches;
	structSwitch* GetSwitch(DWORD dwSwitch, bool bEdit = true)
	{
		if(Switches == NULL) return NULL;
		if(dwSwitch == -1)
		{
			if(bEdit) dwSwitch = dwCurrentEditSwitch;
			else dwSwitch = dwCurrentSwitch;
		}
		if(dwSwitch >= dwSwitches) return NULL;
		return &Switches[dwSwitch];
	}

	//{{AFX_MSG(CSimpleEdit2)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnEnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleDialog
///////////////////////////////////////////////////////////////////////////////////////////////
class CSimpleDialog : public CDialog, public CSimpleDrawDC
{
	DECLARE_DYNAMIC(CSimpleDialog)

public:
	CSimpleDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL):CDialog(lpszTemplateName, pParentWnd),CSimpleDrawDC(NULL){}
	CSimpleDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL):CDialog(nIDTemplate, pParentWnd),CSimpleDrawDC(NULL){}

protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnExitSizeMove();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

public:
	CSimpleGround EraseBkgnd;
	CSimpleDrawType DrawType_EG;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimplePropertyPage
///////////////////////////////////////////////////////////////////////////////////////////////
class CSimplePropertyPage : public CPropertyPage, public CSimpleDrawDC
{
	DECLARE_DYNAMIC(CSimplePropertyPage)

public:
	CSimplePropertyPage(UINT id):CPropertyPage(id), CSimpleDrawDC(NULL),UsingCustomUI(false){}

protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnExitSizeMove();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG*   pMsg)
	{
		if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
		{
			switch(pMsg->wParam)
			{
			case VK_CANCEL:return TRUE;
			case VK_RETURN:if(pMsg->message == WM_KEYDOWN) NextDlgCtrl();return TRUE;
			}
		} 
		return CPropertyPage::PreTranslateMessage(pMsg);
	}
	afx_msg void OnOK(){}
	afx_msg void OnCancel(){}

	DECLARE_MESSAGE_MAP()

public:
	CSimpleGround EraseBkgnd;
	CSimpleDrawType DrawType_EG;
	bool UsingCustomUI;
};

/////////////////////////////////////////////////////////////////////////////
// CSimpleStatic
/////////////////////////////////////////////////////////////////////////////
class CSimpleStatic : public CStatic, public CSimpleDrawDC
{
	DECLARE_DYNAMIC(CSimpleStatic)

public:
	CSimpleStatic():CSimpleDrawDC(NULL),UsingCustomUI(true){}

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

public:
	CSimpleGround EraseBkgnd,ForeGround;
	CSimpleDrawType DrawType_EG,DrawType_FG;
	CSimpleText Text;
	bool UsingCustomUI;
};
/////////////////////////////////////////////////////////////////////////////
// CSimpleStatic2
/////////////////////////////////////////////////////////////////////////////
class CSimpleStatic2 : public CStatic
{
	DECLARE_DYNAMIC(CSimpleStatic2)
public:
	CSimpleStatic2();
	virtual ~CSimpleStatic2();

public:// Operations
	void SetTextFont(CFont*font,BOOL bRepaint=FALSE);
	//
	void SetTextBkInfo(BOOL bTransparent=TRUE, COLORREF cr=NULL,BOOL bRepaint=FALSE);
	void SetTextColor(COLORREF cr, BOOL bRepaint=FALSE);
	void SetBkInfo(BOOL bTransparent, COLORREF cr, BOOL bRepaint=FALSE);

protected:
	CFont*m_pTextFont;
	///
	BOOL m_bTextBkTransparent;
	COLORREF m_crTextColor;
	COLORREF m_crTextBkColor;
	BOOL m_bBkTransparent;
	COLORREF m_crBkColor;
	CBrush m_BrushBk;
	//{{AFX_MSG(CSimpleGroupBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSimpleGroupBox
/////////////////////////////////////////////////////////////////////////////
class CSimpleGroupBox : public CButton
{
public:
	CSimpleGroupBox();
	virtual ~CSimpleGroupBox();

public:// Operations
	void SetTextFont(CFont*font,BOOL bRepaint=FALSE);
	//
	void SetTextBkInfo(BOOL bTransparent=TRUE, COLORREF cr=NULL,BOOL bRepaint=FALSE);
	void SetTextColor(COLORREF cr, BOOL bRepaint=FALSE);
	void SetBkInfo(BOOL bTransparent, COLORREF cr, BOOL bRepaint=FALSE);

protected:
	CFont*m_pTextFont;
	///
	BOOL m_bTextBkTransparent;
	COLORREF m_crTextColor;
	COLORREF m_crTextBkColor;
	BOOL m_bBkTransparent;
	COLORREF m_crBkColor;
	CBrush m_BrushBk;
	//{{AFX_MSG(CSimpleGroupBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



class CTipEdit : public CEdit
{
public:
	CTipEdit(){}
	virtual ~CTipEdit(){}

public:
	void SetTip(LPCTSTR tip) 
	{
		if(IsTipping()) SetWindowText(tip);
		m_strTip = tip ? tip : _T("");
	}
	const CString& GetTip() const { return m_strTip; }
	bool IsTipping() const
	{
		CString curText;
		GetWindowText(curText);
		return (curText == m_strTip);
	}

protected:
	CString m_strTip;

	//{{AFX_MSG(CTipEdit)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#endif