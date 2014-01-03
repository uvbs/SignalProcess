#pragma once

#include "CTemplateDlg.h"

class CTWenChartSerie;
class CTWenChartGroup;
class CTWenChartCtrl;

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartColor
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartColor
{
public:
	enum enumColorIndex
	{
		COLOR_BLACK = 0,
		COLOR_RED,
		COLOR_YELLOW,
		COLOR_LIME,
		COLOR_AQUA,
		COLOR_BLUE,
		COLOR_FUCHSIA,
		COLOR_GRAY,
		COLOR_SILVER,
		COLOR_MAROON,
		COLOR_OLIVE,
		COLOR_GREEN,
		COLOR_TEAL,
		COLOR_NAVY,
		COLOR_PURPLE,
		COLOR_WHITE,

		COLOR_COUNT,
	};

public:
	static COLORREF Colors[COLOR_COUNT];
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartFont
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartFont
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
	CTWenChartFont();

public:
	bool FromFont(CFont& font);
	bool ToFont(CFont& font) const;
	CFont* GetFont(CFont* pFont) const;
	bool FromLogFont(const LOGFONT& lf);
	void ToLogFont(LOGFONT& lf) const;

public:
	structFontArguments Arguments;
	CFont* DefaultFont;

	COLORREF Color;
	bool Transparent;
	COLORREF BkColor;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartPen
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartPen
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
	CTWenChartPen();

public:
	bool FromPen(CPen& pen);
	bool ToPen(CPen& pen) const;
	CPen* GetPen(CPen* pPen) const;

public:
	structPenArguments Arguments;
	CPen* DefaultPen;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartText
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartTextBase
{
protected:
	CTWenChartTextBase();

public:
	DWORD GetID() const{return ID;}

public:
	bool Visible;
	CString Text;
	CTWenChartFont Font;

protected:
	DWORD ID;
};
//
class CTWenChartText : public CTWenChartTextBase
{
public:
	double x, y;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartLegend
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartTitle : public CTWenChartTextBase
{
public:
	CTWenChartTitle();

public:
	int SpacingBorderAndText;
	int SpacingBorderAndText2;
	int Border;
	COLORREF Color_TopLeft, Color_BottomRight;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartLegend
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartLegend
{
public:
	CTWenChartLegend();

public:
	bool Visible;
	//Border
	//Background
	//Shadow
	CTWenChartFont Font;//Color项无效

public:
	int SpacingBorderAndText;
	int SpacingBorderAndImage;
	int VSpacing;
	int HSpacing;
	int ImageWidth;
	int Border;
	COLORREF Color_TopLeft, Color_BottomRight;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartGrid
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartGrid
{
public:
	enum enumPositionType_X
	{
		POSITION_TOP,
		POSITION_BOTTOM,
	};
	enum enumPositionType_Y
	{
		POSITION_LEFT,
		POSITION_RIGHT,
	};
	enum enumAxisType
	{
		AXISTYPE_ALL			= 0x0000001,//显示所有数据
		AXISTYPE_FIXED			= 0x0000002,//显示指定范围的数据
		AXISTYPE_LENGTH			= 0x0000004,//显示（Min -- Min+Length或者Max-Length -- Max）范围的数据

		AXISTYPE_NUMBER			= 0x00010000,//坐标轴是普通数字
		AXISTYPE_DATE			= 0x00020000,//坐标轴是日期
		AXISTYPE_TIME			= 0x00040000,//坐标轴是时间
		AXISTYPE_DATETIME		= 0x00080000,//坐标轴是日期时间

		AXISTYPE_STEPS			= 0x0100000,//自动改变步长，以使步数保持＋－1浮动，需要设置Steps，仅当AXISTYPE_ALL时有效
	};

protected:
	friend class CTWenChartCtrl;
	class CTWenChartScanLine//扫描线
	{
	public:
		CTWenChartScanLine();

	public:
		bool Visible;
		CTWenChartPen Pen;
	};
	class CTWenChartAxisBase//不负责任何逻辑
	{
	public:
		CTWenChartAxisBase();

	public:
		DWORD GetSteps() const{if(AxisType & AXISTYPE_STEPS) return Steps;else return (DWORD)((Max - Min)/Step);}
		double GetStep() const{if(AxisType & AXISTYPE_STEPS) return (Max - Min)/Steps;else return Step;}
		void SetStep(double step);
		void SetDateStep(int years, int monthes, int days);
		void SetTimeStep(int hours, int minutes, int seconds, int milliseconds);
		void SetDateTimeStep(int years, int monthes, int days, int hours, int minutes, int seconds, int milliseconds);
		double GetStepValue(DWORD dwStep) const{return Min + dwStep*GetStep();}
		double GetStepScaleValue(DWORD dwStep) const{return GetStepValue(dwStep)*Scale;}
		CString GetStepScaleValueString(DWORD dwStep) const;
		virtual CString GetScaleValueString() const;
		bool GetStepsValue(double** v, DWORD* count) const;
		bool GetStepsScaleValue(double** v, DWORD* count) const;
		static long GetScale_Base10(double scale);

	public:
		DWORD AxisType;
		/*
		坐标轴类型			数据单位
		AXISTYPE_NUMBER		无
		AXISTYPE_DATE		天
		AXISTYPE_TIME		毫秒
		AXISTYPE_DATETIME	毫秒
		//*/
		double Min,Max;//当前的坐标轴范围
		double Step;//步进值,当为日期时间等类型时，最好使用SetStep系列函数设置
		CString Text;//坐标轴名称
		double Scale;//缩放比例
		bool ScaleBase10;
		bool Visible;
		CTWenChartScanLine ScanLine;
		double ScanValue;//动态属性
		DWORD Steps;

	public:
		int Spacing;//坐标轴名称、数值、坐标轴刻度线的间隔距离
		int LineLength;//坐标轴刻度线长度
		int SpacingBorderAndText;//坐标轴文本（名称或者数值）和边框的间隔距离
	};
	class CTWenChartAxisX : public CTWenChartAxisBase
	{
	public:
		CTWenChartAxisX();

	public:
		enumPositionType_X Position;
	};
	struct CTWenChartAxisY : public CTWenChartAxisBase
	{
	public:
		CTWenChartAxisY();

		virtual CString GetScaleValueString() const;

	public:
		enumPositionType_Y Position;
	};

public:
	CTWenChartGrid(CTWenChartGroup* parent);

public:
	void SetGridArea(const CRect& rcGrid);
	CTWenChartGroup* GetParent() const{return Parent;}
	CString GetXDrawCation() const;
	CString GetYDrawCation() const;

public://坐标变换
	double ToLogical_X(double v) const;//像素坐标转换为逻辑坐标
	long ToPixel_X(double v) const;
	double ToLogical_Y(double v) const;
	long ToPixel_Y(double v) const;

	void ToLogical_X(DWORD count, double *v) const;
	void ToPixel_X(DWORD count, double *v) const;
	void ToLogical_Y(DWORD count, double *v) const;
	void ToPixel_Y(DWORD count, double *v) const;

protected:
	void OnPoint(double x, double y);
	friend class CTWenChartPoints;
	void OnValue(CTWenChartAxisBase* pAxis, double v);
	void OnAxisPixelxChanging(CTWenChartAxisBase* pAxis, double oldPixels, double newPixels);

public:
	bool Visible;
	CTWenChartAxisX AxisX;
	CTWenChartAxisY AxisY;
	COLORREF BGColor;
	//Shadow
	CTWenChartPen Pen;
	CTWenChartFont AxisTextFont;

public:
	int SpacingX, SpacingY;//离标题，图例，边框等的距离

protected:
	CTWenChartGroup* Parent;
	CRect Rect;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartSerie
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartPoints
{
public:
	enum enumDefine
	{
		DEFINE_GROWCOUNT = 10240,
	};
	enum enumRangeType
	{
		RT_NONE,//not supported
		RT_X,//default
		RT_Y,
		RT_XY,//not supported
	};
	struct CTWenChartPointLabel
	{
		CTWenChartPointLabel():Visible(true){}
		bool Visible;
		CString Text;
	};
	class CTWenChartPoint
	{
	public:
		CTWenChartPoint();

	public:
		double x;
		double y;
		CTWenChartPointLabel* label;
	};

public:
	CTWenChartPoints(CTWenChartSerie* parent);
	~CTWenChartPoints();

public:
	CTWenChartPoint* AddPoint(double x, double y);
	void AddPoints(DWORD dwCount, double* x, double* y);
	DWORD GetCount() const{return CurrentIndex;}
	CTWenChartSerie* GetParent() const{return Parent;}
	CTWenChartGrid* GetGrid() const;
	bool GetIndexRange(DWORD& from, DWORD& to) const;

public:
	CTWenChartPoint* Points;
	CTWenChartFont LabelFont;//颜色无效
	enumRangeType RangeType;

protected:
	DWORD MaxCount;
	DWORD CurrentIndex;
	CTWenChartSerie* Parent;
	friend class CTWenChartCtrl;
};
//
class CTWenChartSerie
{
public:
	enum enumSerieType
	{
		ST_NONE,

		ST_LINE,
		ST_POINT,
		ST_BAR,
		ST_SURFACE,
	};

protected:
	CTWenChartSerie(CTWenChartGroup* parent, enumSerieType serieType = ST_NONE);
	friend class CTWenChartGroup;

public:
	virtual ~CTWenChartSerie(){}

public:
	DWORD GetID() const{return ID;}
	CTWenChartGroup* GetParent() const{return Parent;}
	bool IsMainSerie() const;
	virtual bool GetIndexRange(DWORD& from, DWORD& to) const;

public:
	enumSerieType SerieType;
	bool Visible;
	COLORREF Color;
	CString Name;
	CTWenChartPoints Data;

protected:
	DWORD ID;
	CTWenChartGroup* Parent;
};
//
class CTWenChartLineSerie : public CTWenChartSerie
{
public:
	CTWenChartLineSerie(CTWenChartGroup* parent);

public:
	virtual bool GetIndexRange(DWORD& from, DWORD& to) const;

public:
	CTWenChartPen Pen;
};
//
class CTWenChartPointSerie : public CTWenChartSerie
{
public:
	CTWenChartPointSerie(CTWenChartGroup* parent);

public:
	int Radius;
};
//
class CTWenChartBarSerie : public CTWenChartSerie
{
public:
	CTWenChartBarSerie(CTWenChartGroup* parent);

public:
	int Width;
};
//
class CTWenChartSurfaceSerie : public CTWenChartSerie
{
public:
	CTWenChartSurfaceSerie(CTWenChartGroup* parent);

public:
	virtual bool GetIndexRange(DWORD& from, DWORD& to) const;

public:
	int Smooth;
};
//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartPiePiece
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartPiePiece
{
public:
	CTWenChartPiePiece(CTWenChartGroup* parent, LPCTSTR name = NULL, double data = 1.0);

public:
	DWORD GetID() const{return ID;}
	CTWenChartGroup* GetParent() const{return Parent;}

public:
	CString Name;
	COLORREF Color;
	double Data;

protected:
	DWORD ID;
	CTWenChartGroup* Parent;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartGroup
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartGroup
{
public:
	enum enumDataType
	{
		DATATYPE_SERIE,
		DATATYPE_PIE,
	};

public:
	CTWenChartGroup(CTWenChartCtrl* parent);
	~CTWenChartGroup();

public:
	DWORD GetID() const{return ID;}
	CTWenChartCtrl* GetParent() const{return Parent;}
	bool IsVisible() const{return Visible;}
	void SetMainSerieByID(DWORD dwSerieID);
	void SetMainSerieByIndex(DWORD dwSerieIndex);
	CTWenChartSerie* GetMainSerie() const{return MainSerie;}

public://Serie集合操作
	CTWenChartLineSerie* NewLineSerie();
	CTWenChartPointSerie* NewPointSerie();
	CTWenChartBarSerie* NewBarSerie();
	CTWenChartSurfaceSerie* NewSurfaceSerie();
	void DeleteSerieByID(DWORD dwSerieID);
	void DeleteSerieByIndex(DWORD dwSerieIndex);
	CTWenChartSerie* GetSerieByID(DWORD dwSerieID) const;
	CTWenChartSerie* GetSerieByIndex(DWORD dwSerieIndex) const;
	void ShowSerieByID(DWORD dwSerieID, bool bVisible);
	void ShowSerieByIndex(DWORD dwSerieIndex, bool bVisible);
	DWORD GetSerieIndexByID(DWORD dwSerieID) const;
	void DeleteSeries();
protected:
	void AddSerie(CTWenChartSerie* pSerie);

public://PiePiece集合操作
	CTWenChartPiePiece* NewPiePiece(LPCTSTR name, double data);
	void DeletePiePieceByID(DWORD dwPiePieceID);
	void DeletePiePieceByIndex(DWORD dwPiePieceIndex);
	CTWenChartPiePiece* GetPiePieceByID(DWORD dwPiePieceID) const;
	CTWenChartPiePiece* GetPiePieceByIndex(DWORD dwPiePieceIndex) const;
	DWORD GetPiePieceIndexByID(DWORD dwPiePieceID) const;
	void DeletePiePieces();

public://Text集合操作
	CTWenChartText* NewText();
	void DeleteTextByID(DWORD dwTextID);
	void DeleteTextByIndex(DWORD dwTextIndex);
	CTWenChartText* GetTextByID(DWORD dwTextID) const;
	CTWenChartText* GetTextByIndex(DWORD dwTextIndex) const;
	void ShowTextByID(DWORD dwTextID, bool bVisible);
	void ShowTextByIndex(DWORD dwTextIndex, bool bVisible);
	DWORD GetTextIndexByID(DWORD dwTextID) const;
	void DeleteTexts();

public:
	enumDataType DataType;
	CTWenChartGrid Grid;
	CArray<CTWenChartSerie*, CTWenChartSerie*> Series;
	CArray<CTWenChartPiePiece*, CTWenChartPiePiece*> PiePieces;
	CArray<CTWenChartText*, CTWenChartText*> Texts;

protected:
	DWORD ID;
	CTWenChartCtrl* Parent;
	bool Visible;
	CTWenChartSerie* MainSerie;
	friend class CTWenChartCtrl;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartCtrl
//////////////////////////////////////////////////////////////////////////////////////////////
class CTWenChartCtrl : public CWnd
{
protected:
	enum enumArrowDefine
	{
		Arrow_Width = 8,
		Arrow_Height = 3,
	};
public:
	enum enumMenuIDs
	{
		ID_MENU_RANGE_FROM = WM_USER + 5000,

		ID_MENU_TITLE_SHOW,
		ID_MENU_TITLE_HIDE,
		ID_MENU_TITLE_FONT,

		ID_MENU_LEGEND_SHOW,
		ID_MENU_LEGEND_HIDE,
		ID_MENU_LEGEND_FONT,

		ID_MENU_AUTOAXISX_ENABLE,
		ID_MENU_AUTOAXISX_DISABLE,

		ID_MENU_AUTOAXISY_ENABLE,
		ID_MENU_AUTOAXISY_DISABLE,

		ID_MENU_AUTOAXIS_FONT,
		ID_MENU_AUTOAXIS_BGCOLOR,

		ID_MENU_CTRL_BGCOLOR,
		ID_MENU_SAVE,
		ID_MENU_LOAD,

		ID_MENU_GROUPS_NEW,
		ID_MENU_GROUPS_DATA_SAVE,
		ID_MENU_GROUPS_DATA_LOAD,
		ID_MENU_GROUPS_DATA_CLEAR,

		ID_MENU_CURRENTGROUP_NEW_TEXT,
		ID_MENU_CURRENTGROUP_NEW_LINESERIE,
		ID_MENU_CURRENTGROUP_NEW_POINTSERIE,
		ID_MENU_CURRENTGROUP_NEW_BARSERIE,
		ID_MENU_CURRENTGROUP_NEW_SURFACESERIE,
		ID_MENU_CURRENTGROUP_DATA_SAVE,
		ID_MENU_CURRENTGROUP_DATA_LOAD,
		ID_MENU_CURRENTGROUP_DATA_CLEAR,

		ID_MENU_GRID_BGCOLOR,

		ID_MENU_GRID_LINE_SHOW,
		ID_MENU_GRID_LINE_HIDE,

		ID_MENU_GRID_AXIS_FONT,

		ID_MENU_GRID_AXISX_TOP,
		ID_MENU_GRID_AXISX_BOTTOM,

		ID_MENU_GRID_AXISY_LEFT,
		ID_MENU_GRID_AXISY_RIGHT,

		ID_MENU_GRID_AXISX_BASESET,
		ID_MENU_GRID_AXISY_BASESET,

		ID_MENU_GRID_AXISX_SCANLINE_SHOW,
		ID_MENU_GRID_AXISX_SCANLINE_HIDE,

		ID_MENU_GRID_AXISY_SCANLINE_SHOW,
		ID_MENU_GRID_AXISY_SCANLINE_HIDE,

		ID_MENU_TEXT_UPDATE,
		ID_MENU_TEXT_DELETE,

		ID_MENU_SERIE_COLOR,
		ID_MENU_SERIE_TYPE,
		ID_MENU_SERIE_DATA_SAVE,
		ID_MENU_SERIE_DATA_LOAD,
		ID_MENU_SERIE_DATA_CLEAR,
		ID_MENU_SERIE_DATA_ADDPOINT,
		
		ID_MENU_GROUPFROM,
		ID_MENU_GROUPTO = ID_MENU_GROUPFROM + 1000,//最后1000个组

		ID_MENU_TEXTFROM,
		ID_MENU_TEXTTO = ID_MENU_TEXTFROM + 10000,//每组最多10000条线

		ID_MENU_SERIEORPIEPIECE,

		ID_MENU_RANGE_TO = ID_MENU_SERIEORPIEPIECE + 10000,//每组最多10000条线
	};

protected:
	struct structSelectChartFont
	{
		structSelectChartFont(CDC* pDC, const CTWenChartFont& ChartFont, CFont& font);
		~structSelectChartFont();

	private:
		CDC* m_pDC;
		CFont* m_pOldFont;
	};
	class CTWenChartTextDlg : public CTemplateDlg
	{
	public:
		CTWenChartTextDlg(CWnd* parent, CTWenChartText* pText, bool New);

		DECLARE_MESSAGE_MAP()
	
	protected:
		afx_msg void OnButtonFont();
		virtual void OnOK();

	public:
		CTWenChartText* Text;
	};

	DECLARE_DYNAMIC(CTWenChartCtrl)

public:
	CTWenChartCtrl();
	virtual ~CTWenChartCtrl();

public:
	void Create(CWnd* parent, const CRect& rc);
	void Redraw(bool bRebuilding);//刷新窗口显示，当有配置或者数据改动时，bRebuliding应当为true
	CTWenChartGroup* GetCurrentGroup() const{return CurrentGroup;}

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
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT msg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public://组操作,不会自动刷新显示
	CTWenChartGroup* NewGroup();
	void DeleteGroupByID(DWORD dwGroupID);
	void DeleteGroupByIndex(DWORD dwGroupIndex);
	CTWenChartGroup* GetGroupByID(DWORD dwGroupID) const;
	CTWenChartGroup* GetGroupByIndex(DWORD dwGroupIndex) const;
	DWORD GetGroupIndexByID(DWORD dwGroupID) const;
	void DeleteGroups();

public://会自动刷新显示
	void SetCurGroupByID(DWORD dwGroupID);
	void SetCurGroupByIndex(DWORD dwGroupIndex);

protected:
	void RebuildChart(CDC* pDC = NULL);
	void DrawAutoAxis(CDC* pDC, const CTWenChartGrid& Grid);
	static void DrawBackground(CDC* pDC, const CRect& rc, COLORREF bgColor);
	static void DrawTitle(CDC* pDC, const CTWenChartTitle& Title, const CRect& rc, CRect& rcTitle);
	static void DrawLegend(CDC* pDC, const CTWenChartLegend& Legend, const CTWenChartGroup* pGroup, const CRect& rc, CRect& rcLegend);
	static void DrawLegendImage(CDC* pDC, const CRect& rc, const CTWenChartSerie* pSerie);
	static void CalculateArea(CDC* pDC, const CTWenChartGroup* pGroup, const CRect& rc, CRect& rcGrid, CRect& rcAxisX, CRect& rcAxisY);
	static void DrawAxisX(CDC* pDC, const CTWenChartGrid& Grid, const CRect& rcAxisX, const CRect& rcAxisY);
	static void DrawAxisY(CDC* pDC, const CTWenChartGrid& Grid, const CRect& rcAxisX, const CRect& rcAxisY);
	static void DrawGrid(CDC* pDC, const CTWenChartGrid& Grid, const CRect& rcGrid);
	static void DrawSerie(CDC* pDC,const CTWenChartGrid& Grid, const CTWenChartSerie* pSerie);
	static void DrawPointLabel(CDC* pDC, const CTWenChartGrid& Grid, int x, int y, const CTWenChartFont& Font, const CTWenChartPoints::CTWenChartPointLabel* pLabel);
	static void DrawChartText(CDC* pDC, const CTWenChartGrid& Grid, const CTWenChartText* pText);
	static void DrawScanLine(CDC* pDC,const CTWenChartGrid& Grid);

public:
	static void TrimZero(CString& str);

protected:
	static bool ChartFont(CTWenChartFont& font, CWnd* parent);//显示ChartFont对话框，如果有所更改，返回true
	bool ChartColor(COLORREF& color);
	bool ChartAxisRange(CTWenChartGrid::CTWenChartAxisBase& axis);
	bool GetSaveFile(CFile& file, LPCTSTR DefaultFileTitle, LPCTSTR Caption);
	void SaveSerie(CFile& file, CTWenChartSerie* pSerie);
	bool GetLoadFile(CFile& file, CString& fileTitle, LPCTSTR Caption);
	void LoadSerie(CFile& file, CTWenChartSerie* pSerie);
	static bool Read(CFile& file,CString& content,char token);

public:
	COLORREF BGColor;
	CTWenChartTitle Title;
	CTWenChartLegend Legend;
	CArray<CTWenChartGroup*, CTWenChartGroup*> Groups;
	bool AutoAxisX, AutoAxisY;
	CTWenChartFont AutoAxisFont;

protected:
	CTWenChartGroup* CurrentGroup;
	CBitmap ChartImage;
	DWORD CurrentSelect;
	DWORD CurrentSelectText;
	HCURSOR Cursor;
	int Posiotion_AutoAxisX, Posiotion_AutoAxisY;
};