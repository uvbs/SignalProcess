
#ifndef SDK_TWenBMEditorCtrl
#define SDK_TWenBMEditorCtrl


#include "CTemplateDlg.h"

class CTWenBMEditorCtrl;

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenFont
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenFont
{
public:
	struct structFontArguments
	{
		structFontArguments():Size(12),Name(_T("宋体")),Bold(false),Italic(false),Underline(false),StrikeOut(false),Escapement(0),Orientation(0),YOffset(0){}

		int Size;//字体大小
		CString Name;//字体名称
		bool Bold;
		bool Italic;
		bool Underline;
		bool StrikeOut;
		int Escapement;
		int Orientation;
		int YOffset;//Character offset, in twips, from the baseline. If the value of this member is positive, the character is a superscript; if it is negative, the character is a subscript. 
	};

public:
	CTWenFont();

public:
	bool FromFont(CFont& font);
	bool ToFont(CFont& font) const;
	CFont* GetFont(CFont* pFont) const;
	bool FromLogFont(const LOGFONT& lf);
	void ToLogFont(LOGFONT& lf) const;

public://持久化
	void Save(CFile& file) const;
	void Load(CFile& file);

public:
	structFontArguments Arguments;
	CFont* DefaultFont;

	COLORREF Color;
	bool Transparent;
	COLORREF BkColor;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenPen
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenPen
{
public:
	struct structPenArguments
	{
		structPenArguments():Color(NULL),Width(1),Style(PS_SOLID){}

		COLORREF Color;
		long Width;
		UINT Style;//Pen Styles
	};

public:
	CTWenPen();

public:
	bool FromPen(CPen& pen);
	bool ToPen(CPen& pen) const;
	CPen* GetPen(CPen* pPen) const;

public://持久化
	void Save(CFile& file) const;
	void Load(CFile& file);

public:
	structPenArguments Arguments;
	CPen* DefaultPen;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenBrush
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenBrush
{
public:
	struct structBrushArguments
	{
		structBrushArguments():Color(NULL),Hatch(NULL),Style(BS_SOLID){}

		UINT     Style; 
		COLORREF Color; 
		LONG     Hatch;
	};

public:
	CTWenBrush();

public:
	bool FromBrush(CBrush& Brush);
	bool ToBrush(CBrush& Brush) const;
	CBrush* GetBrush(CBrush* Brush) const;

public://持久化
	void Save(CFile& file) const;
	void Load(CFile& file);

public:
	structBrushArguments Arguments;
	CBrush* DefaultBrush;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// structSelectDCFont
//////////////////////////////////////////////////////////////////////////////////////////////
struct structSelectDCFont
{
	structSelectDCFont(CDC* pDC, const CTWenFont& ChartFont, CFont& font);
	~structSelectDCFont();

private:
	CDC* m_pDC;
	CFont* m_pOldFont;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenGraphicsSerie
//////////////////////////////////////////////////////////////////////////////////////////////
//
class CTWenGraphicsSerie
{
public:
	enum enumSerieType
	{
		ST_NONE,

		ST_TEXT,
		ST_RECT,
		ST_ROUNDRECT,
		ST_ELLIPSE,
		ST_ARROW,
	};
	enum enumFillType
	{
		FT_FILL,//实心
		FT_NOTFILL,//空心
	};
	enum enumPositionType
	{
		POSITION_LEFTTOP,//大小改变时，以左上坐标为基准
		POSITION_CENTER,//大小改变时，以中心点坐标为基准
	};

protected:
	CTWenGraphicsSerie(CTWenBMEditorCtrl* parent, enumSerieType serieType = ST_NONE);
	friend class CTWenBMEditorCtrl;

public:
	virtual ~CTWenGraphicsSerie(){}

public:
	CTWenBMEditorCtrl* GetParent() const{return Parent;}
	DWORD GetID() const{return ID;}
	enumSerieType GetSerieType() const{return SerieType;}
	LPCTSTR GetSerieTypeName() const;
	void SetRect(const RECT& rc);
	void SetCenterPoint(long x, long y);
	void SetCenterPoint(const POINT& point){SetCenterPoint(point.x, point.y);}
	const CRect& GetRect() const{return Rect;}
	const CPoint& GetCenterPoint(){return CenterPoint;}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	CString Name;
	bool Visible;
	COLORREF Color;
	enumFillType FillType;
	enumPositionType PositionType;

protected:
	DWORD ID;
	enumSerieType SerieType;
	CTWenBMEditorCtrl* Parent;
	CRect Rect;
	CPoint CenterPoint;
};
//
class CTWenTextSerie : public CTWenGraphicsSerie
{
public:
	CTWenTextSerie(CTWenBMEditorCtrl* parent):CTWenGraphicsSerie(parent, CTWenGraphicsSerie::ST_TEXT),DrawType(DT_CENTER|DT_TOP)
	{}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	DWORD DrawType;
	CString Text;
	CTWenFont Font;
};
//
class CTWenRectSerie : public CTWenGraphicsSerie
{
public:
	CTWenRectSerie(CTWenBMEditorCtrl* parent):CTWenGraphicsSerie(parent, CTWenGraphicsSerie::ST_RECT){}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	CTWenPen BorderPen;
};
//
class CTWenRoundRectSerie : public CTWenGraphicsSerie
{
public:
	CTWenRoundRectSerie(CTWenBMEditorCtrl* parent):CTWenGraphicsSerie(parent, CTWenGraphicsSerie::ST_ROUNDRECT),Round(5,4){}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	CTWenPen Pen;
	CTWenBrush Brush;
	CPoint Round;
};
//
class CTWenEllipseSerie : public CTWenGraphicsSerie
{
public:
	CTWenEllipseSerie(CTWenBMEditorCtrl* parent):CTWenGraphicsSerie(parent, CTWenGraphicsSerie::ST_ELLIPSE){}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	CTWenPen Pen;
	CTWenBrush Brush;
};
//
class CTWenArrowSerie : public CTWenGraphicsSerie
{
public:
	enum enumArrowDirection
	{
		DIRECTION_LEFT,
		DIRECTION_UP,
		DIRECTION_RIGHT,
		DIRECTION_DOWN,
	};
public:
	CTWenArrowSerie(CTWenBMEditorCtrl* parent):CTWenGraphicsSerie(parent, CTWenGraphicsSerie::ST_ARROW),Direction(CTWenArrowSerie::DIRECTION_UP),Length(6)
	{}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	enumArrowDirection Direction;
	CTWenPen Pen;
	long Length;
};
/*
class CTWenBtimapSerie : public CTWenGraphicsSerie
{
public:
	CTWenBtimapSerie(CTWenBMEditorCtrl* parent):CTWenGraphicsSerie(parent, CTWenGraphicsSerie::ST_ARROW),Direction(CTWenArrowSerie::DIRECTION_UP),Length(6)
	{}

public://持久化
	virtual void Save(CFile& file) const;
	virtual void Load(CFile& file);

public:
	CString Bitmap;
	DWORD Rop;
	DWORD DrawType;
	bool Transparent;
	COLORREF TransparentColor;
};//*/

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenBMEditorCtrl
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenBMEditorCtrl : public CWnd
{
public:
	enum enumMenuIDs
	{
		ID_MENU_RANGE_FROM = WM_USER + 5000,

		//添加
		ID_MENU_SERIE_NEWTEXT,
		ID_MENU_SERIE_NEWRECT,
		ID_MENU_SERIE_NEWROUNDRECT,
		ID_MENU_SERIE_NEWELLIPSE,
		ID_MENU_SERIE_NEWARROW,

		//
		ID_MENU_SERIE_DELETE,

		//加载
		ID_MENU_LOAD_BITMAP,
		ID_MENU_LOAD_EDITORINDEX,
		ID_MENU_LOAD_EDITORDOCUMENT,

		//保存
		ID_MENU_SAVE_BITMAP,
		ID_MENU_SAVE_EDITORINDEX,
		ID_MENU_SAVE_EDITORDOCUMENT,

		ID_MENU_PROPERTY_BASE,
		ID_MENU_PROPERTY_SPECIAL,

		ID_MENU_PROPERTY_FITIMAGE,

		//SERIE
		ID_MENU_SERIEFROM,
		ID_MENU_SERIETO = ID_MENU_SERIEFROM + 10000,//最多10000个元素

		ID_MENU_RANGE_TO
	};

public:
	class CTWenPropertyDlg : public CTemplateDlg
	{
	public:
		CTWenPropertyDlg(CWnd* parent, CTWenGraphicsSerie* pSerie);

		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnButtonColor();
		virtual void OnOK();

	public:
		CTWenGraphicsSerie* Serie;
		COLORREF Color;
	};
	//
	class CTWenTextDlg : public CTemplateDlg
	{
	public:
		CTWenTextDlg(CWnd* parent, CTWenTextSerie* pText);

		DECLARE_MESSAGE_MAP()
	
	protected:
		afx_msg void OnButtonFont();
		virtual void OnOK();

	public:
		CTWenTextSerie* Text;
		bool AutoRect;
	};
	//
	class CTWenPenDlg : public CTemplateDlg
	{
	public:
		CTWenPenDlg(CWnd* parent, CTWenPen* pPen);

		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnButtonColor();
		virtual void OnOK();

	public:
		CTWenPen* Pen;
		COLORREF Color;
	};
	//
	class CTWenBrushDlg : public CTemplateDlg
	{
	public:
		CTWenBrushDlg(CWnd* parent, CTWenBrush* pBrush);

		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnButtonColor();
		virtual void OnOK();

	public:
		CTWenBrush* Brush;
		COLORREF Color;
	};
	//
	class CTWenRoundRectDlg : public CTemplateDlg
	{
	public:
		CTWenRoundRectDlg(CWnd* parent, CTWenRoundRectSerie* pSerie);

		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnButtonPen();
		afx_msg void OnButtonBrush();
		virtual void OnOK();

	public:
		CTWenRoundRectSerie* Serie;
		CTWenPen Pen;
		CTWenBrush Brush;
	};
	//
	class CTWenEllipseDlg : public CTemplateDlg
	{
	public:
		CTWenEllipseDlg(CWnd* parent, CTWenEllipseSerie* pSerie);

		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnButtonPen();
		afx_msg void OnButtonBrush();
		virtual void OnOK();

	public:
		CTWenEllipseSerie* Serie;
		CTWenPen Pen;
		CTWenBrush Brush;
	};
	//
	class CTWenArrowDlg : public CTemplateDlg
	{
	public:
		CTWenArrowDlg(CWnd* parent, CTWenArrowSerie* pSerie);

		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnButtonPen();
		afx_msg void OnButtonBrush();
		virtual void OnOK();

	public:
		CTWenArrowSerie* Serie;
		CTWenPen Pen;
	};

	DECLARE_DYNAMIC(CTWenBMEditorCtrl)

public:
	CTWenBMEditorCtrl();
	virtual ~CTWenBMEditorCtrl();

public:
	void Create(CWnd* parent, const CRect& rc);
	void Redraw(bool bRebuilding);//刷新窗口显示，当有配置或者数据改动时，bRebuliding应当为true
	CTWenGraphicsSerie* GetCurrentSelect() const{return CurrentSelect;}

protected:
	virtual void OnInitMenu(CMenu& Menu);
	virtual void OnMenu(DWORD dwMenuID);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMenuCommandRange(UINT id);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	CTWenTextSerie* NewTextSerie();
	CTWenRectSerie* NewRectSerie();
	CTWenRoundRectSerie* NewoundRectSerie();
	CTWenEllipseSerie* NewEllipseSerie();
	CTWenArrowSerie* NewArrowSerie();

	void DeleteSerieByID(DWORD dwSerieID);
	void DeleteSerieByIndex(DWORD dwSerieIndex);
	CTWenGraphicsSerie* GetSerieByID(DWORD dwSerieID) const;
	CTWenGraphicsSerie* GetSerieByIndex(DWORD dwSerieIndex) const;
	DWORD GetSerieIndexByID(DWORD dwSerieID) const;
	void DeleteSeries();

	void SetCurrentSelectByIndex(DWORD dwSerieIndex);
	void SetCurrentSelectByID(DWORD dwSerieID);

	CTWenGraphicsSerie* FromPosition(const CPoint& point) const;

protected:
	void Rebuild(CDC* pDC = NULL);
	void AddSerie(CTWenGraphicsSerie* pSerie);
	void DrawBackground(CDC* pDC, const CRect& rc);
	void DrawTextSerie(CDC* pDC, CTWenTextSerie* pSerie, const CRect& rc);
	void DrawRectSerie(CDC* pDC,CTWenRectSerie* pSerie, const CRect& rc);
	void DrawRoundRectSerie(CDC* pDC,CTWenRoundRectSerie* pSerie, const CRect& rc);
	void DrawEllipseSerie(CDC* pDC,CTWenEllipseSerie* pSerie, const CRect& rc);
	void DrawArrowSerie(CDC* pDC,CTWenArrowSerie* pSerie, const CRect& rc);

	bool SpecialProperty_TextSerie(CTWenTextSerie* pSerie);
	bool SpecialProperty_RectSerie(CTWenRectSerie* pSerie);
	bool SpecialProperty_RoundRectSerie(CTWenRoundRectSerie* pSerie);
	bool SpecialProperty_EllipseSerie(CTWenEllipseSerie* pSerie);
	bool SpecialProperty_ArrowSerie(CTWenArrowSerie* pSerie);

	void SaveIndex(CFile& file) const;
	void LoadIndex(CFile& file);
	void RefreshSrcImage();

public:
	static bool ChartFont(CTWenFont& font, CWnd* parent);//显示ChartFont对话框，如果有所更改，返回true
	static bool ChartColor(COLORREF& color, CWnd* parent);

public:
	CArray<CTWenGraphicsSerie*, CTWenGraphicsSerie*> Series;

protected:
	CString SrcImageFile;
	CBitmap SrcImage;
	CBitmap ResultImage;
	CTWenGraphicsSerie* CurrentSelect;
	CPoint StartPoint;
};

#endif