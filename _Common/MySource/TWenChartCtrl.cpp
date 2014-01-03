// TWenChartCtrl.cpp : 实现文件
//
#ifdef _AFX_NO_RICHEDIT_SUPPORT
#undef _AFX_NO_RICHEDIT_SUPPORT
#endif

#include "stdafx.h"
#include "TWenChartCtrl.h"
#include "CStringExt.h"
#include "CStringConvert.h" 

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartColor
//////////////////////////////////////////////////////////////////////////////////////////////
COLORREF CTWenChartColor::Colors[COLOR_COUNT] =
{
	RGB(0x00,0x00,0x00),//Black
	RGB(0xff,0x00,0x00),//Red
	RGB(0xff,0xff,0x00),//Yellow
	RGB(0x00,0xff,0x00),//Lime
	RGB(0x00,0xff,0xff),//Aqua
	RGB(0x00,0x00,0xff),//Blue
	RGB(0xff,0x00,0xff),//Fuchsia
	RGB(0x80,0x80,0x80),//Gray
	RGB(0xc0,0xc0,0xc0),//Silver
	RGB(0x80,0x00,0x00),//Maroon
	RGB(0x80,0x80,0x00),//Olive
	RGB(0x00,0x80,0x00),//Green
	RGB(0x00,0x80,0x80),//Teal
	RGB(0x00,0x00,0x80),//Navy
	RGB(0x80,0x00,0x80),//Purple
	RGB(0xff,0xff,0xff)//White
};

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartFont
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartFont::CTWenChartFont():DefaultFont(NULL),Color(NULL),Transparent(true),BkColor(NULL)
{
}

bool CTWenChartFont::FromFont(CFont& font)
{
	LOGFONT lf;
	if(font.GetLogFont(&lf) == FALSE) return false;
	return FromLogFont(lf);
}
bool CTWenChartFont::ToFont(CFont& font) const
{
	return font.CreateFont(
		Arguments.Size,       // nHeight
		0,                         // nWidth
		Arguments.Escapement, // nEscapement
		Arguments.Orientation,// nOrientation
		Arguments.Bold?FW_BOLD:FW_NORMAL,     // nWeight
		Arguments.Italic,                     // bItalic
		Arguments.Underline,  // bUnderline
		Arguments.StrikeOut,  // cStrikeOut
		DEFAULT_CHARSET,           // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		Arguments.Name) != FALSE;      // lpszFacename
}
CFont* CTWenChartFont::GetFont(CFont* pFont) const
{
	if(DefaultFont) return DefaultFont;
	if(pFont) ToFont(*pFont);
	return pFont;
}
bool CTWenChartFont::FromLogFont(const LOGFONT& lf)
{
	Arguments.Size = lf.lfHeight;
	Arguments.Escapement = lf.lfEscapement;
	Arguments.Orientation = lf.lfOrientation;
	Arguments.Bold = (lf.lfWeight == FW_BOLD)?true:false;
	Arguments.Italic = lf.lfItalic?true:false;
	Arguments.Underline = lf.lfUnderline?true:false;
	Arguments.StrikeOut = lf.lfStrikeOut?true:false;
	Arguments.Name = lf.lfFaceName;
	return true;
}
void CTWenChartFont::ToLogFont(LOGFONT& lf) const
{
	if(DefaultFont) DefaultFont->GetLogFont(&lf);
	else
	{
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfEscapement = Arguments.Escapement;
		_tcscpy(lf.lfFaceName, Arguments.Name);
		lf.lfHeight = Arguments.Size * 20;
		lf.lfItalic = Arguments.Italic;
		lf.lfOrientation = Arguments.Orientation;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfStrikeOut = Arguments.StrikeOut;
		lf.lfUnderline = Arguments.Underline;
		lf.lfWeight = Arguments.Bold?FW_BOLD:FW_NORMAL;
		lf.lfWidth = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartPen
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartPen::CTWenChartPen():DefaultPen(NULL)
{
}

bool CTWenChartPen::FromPen(CPen& pen)
{
	LOGPEN lp;
	if(pen.GetLogPen(&lp) == FALSE) return false;
	Arguments.Color = lp.lopnColor;
	Arguments.Style = lp.lopnStyle;
	Arguments.Width = lp.lopnWidth.x;
	return true;
}
bool CTWenChartPen::ToPen(CPen& pen) const
{
	return pen.CreatePen(Arguments.Style, Arguments.Width, Arguments.Color) != FALSE;
}
CPen* CTWenChartPen::GetPen(CPen* pPen) const
{
	if(DefaultPen) return DefaultPen;
	if(pPen) ToPen(*pPen);
	return pPen;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartText
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartTextBase::CTWenChartTextBase():Visible(true)
{
	static DWORD TextID = 0;
	TextID++;
	ID = TextID;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartLegend
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartTitle::CTWenChartTitle():SpacingBorderAndText(2),SpacingBorderAndText2(2),
Border(true),Color_TopLeft(CTWenChartColor::Colors[CTWenChartColor::COLOR_SILVER]),Color_BottomRight(CTWenChartColor::Colors[CTWenChartColor::COLOR_GRAY])
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartLegend
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartLegend::CTWenChartLegend():Visible(true),SpacingBorderAndText(2),SpacingBorderAndImage(2),VSpacing(2),HSpacing(2),ImageWidth(12),
	Border(true),Color_TopLeft(CTWenChartColor::Colors[CTWenChartColor::COLOR_SILVER]),Color_BottomRight(CTWenChartColor::Colors[CTWenChartColor::COLOR_GRAY])
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartGrid
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartGrid::CTWenChartScanLine::CTWenChartScanLine():Visible(false)
{
	Pen.Arguments.Style = PS_SOLID;
	Pen.Arguments.Color = CTWenChartColor::Colors[CTWenChartColor::COLOR_RED];
	Pen.Arguments.Width = 2;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartGrid::CTWenChartAxisBase::CTWenChartAxisBase()
:AxisType(CTWenChartGrid::AXISTYPE_NUMBER),Spacing(2),SpacingBorderAndText(2),Scale(1.0),Step(1.0),Min(0.0),Max(0.0),ScaleBase10(false),Visible(true),
LineLength(5),Steps(10)
{
}

void CTWenChartGrid::CTWenChartAxisBase::SetStep(double step)
{
	Step = step;
}
void CTWenChartGrid::CTWenChartAxisBase::SetDateStep(int years, int monthes, int days)
{
	SetDateTimeStep(years, monthes, days, 0, 0, 0, 0);
}
void CTWenChartGrid::CTWenChartAxisBase::SetTimeStep(int hours, int minutes, int seconds, int milliseconds)
{
	SetDateTimeStep(0, 0, 0, hours, minutes, seconds, milliseconds);
}
void CTWenChartGrid::CTWenChartAxisBase::SetDateTimeStep(int years, int monthes, int days, int hours, int minutes, int seconds, int milliseconds)
{
	if(milliseconds >= 1000){seconds+=milliseconds/1000;milliseconds%=1000;}
	if(seconds >= 60){minutes+=seconds/60;seconds%=60;}
	if(minutes >= 60){hours+=minutes/60;minutes%=60;}
	if(hours >= 24){days+=hours/24;hours%=24;}
	//每个月中的天数不确定，不能精确计算
	if(days > 999999) days %= 1000000;
	if(monthes >= 12){years+=monthes/12;monthes%=12;}
	//[xy][mm][dddddd].[hh][mm][ss][mmm]
	Step = years * 100000000 + monthes * 1000000 + days + hours * 0.01 + minutes * 0.0001 + seconds * 0.000001 + milliseconds * 0.000000001;
}

CString CTWenChartGrid::CTWenChartAxisBase::GetStepScaleValueString(DWORD dwStep) const
{
	CString ret;
	ret.Format(_T("%.1f"), GetStepScaleValue(dwStep));
	CTWenChartCtrl::TrimZero(ret);
	return ret;
}
CString CTWenChartGrid::CTWenChartAxisBase::GetScaleValueString() const
{
	CString ret;
	if(ScaleBase10)
	{
		long l = GetScale_Base10(Scale);
		switch(l)
		{
		case 0:ret = _T("1");break;
		case 1:ret = _T("10");break;
		default:ret.Format(_T("10(%d)"), l);
		}
	}
	else
	{
		ret.Format(_T("%f"), Scale);
		CTWenChartCtrl::TrimZero(ret);
	}
	return ret;
}
bool CTWenChartGrid::CTWenChartAxisBase::GetStepsValue(double** v, DWORD* count) const
{
	if(!v) return false;
	DWORD steps = GetSteps();
	*v = new double[steps+1];
	double bv = Min;
	for(DWORD i=0; i<=steps; i++)
	{
		(*v)[i] = bv;
		bv += GetStep();
	}
	if(count) *count = steps;
	return true;
}
bool CTWenChartGrid::CTWenChartAxisBase::GetStepsScaleValue(double** v, DWORD* count) const
{
	if(!v) return false;
	DWORD steps = GetSteps();
	*v = new double[steps];
	double bv = Min * Scale;
	double step = GetStep() * Scale;
	for(DWORD i=0; i<=steps; i++)
	{
		(*v)[i] = bv;
		bv += step;
	}
	if(count) *count = steps;
	return true;
}
long CTWenChartGrid::CTWenChartAxisBase::GetScale_Base10(double scale)
{
	long ret = 0;
	//scale必须是10的n次方
	if(scale > 1)
	{
		while(scale > 1)
		{
			ret++;
			scale /= 10;
		}
	}
	else
	{

		while(scale < 1)
		{
			ret--;
			scale *= 10;
		}
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartGrid::CTWenChartAxisX::CTWenChartAxisX():CTWenChartAxisBase(),Position(CTWenChartGrid::POSITION_BOTTOM)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartGrid::CTWenChartAxisY::CTWenChartAxisY():CTWenChartAxisBase(),Position(CTWenChartGrid::POSITION_LEFT)
{
}
CString CTWenChartGrid::CTWenChartAxisY::GetScaleValueString() const
{
	CString ret;
	if(ScaleBase10)
	{
		long l = GetScale_Base10(Scale);
		switch(l)
		{
		case 0:ret = _T("1");break;
		case 1:ret = _T("10");break;
		default:ret.Format(_T("10∧%d∨"), l);
		}
	}
	else
	{
		ret.Format(_T("%f"), Scale);
		CTWenChartCtrl::TrimZero(ret);
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartGrid::CTWenChartGrid(CTWenChartGroup* parent):Parent(parent),Visible(true),Rect(0,0,0,0),SpacingX(16),SpacingY(16),
BGColor(CTWenChartColor::Colors[CTWenChartColor::COLOR_WHITE])
{
	Pen.Arguments.Style = PS_DOT;
}


void CTWenChartGrid::SetGridArea(const CRect& rcGrid)
{
	OnAxisPixelxChanging(&AxisX, Rect.Width(), rcGrid.Width());
	OnAxisPixelxChanging(&AxisY, Rect.Height(), rcGrid.Height());
	Rect = rcGrid;
}
CString CTWenChartGrid::GetXDrawCation() const
{
	return AxisX.Text + _T("＜比例:") + AxisX.GetScaleValueString() + _T("＞");
}
CString CTWenChartGrid::GetYDrawCation() const
{
	return AxisY.Text + _T("∧比例|") + AxisY.GetScaleValueString() + _T("∨");
}

void CTWenChartGrid::OnPoint(double x, double y)
{
	//设置xy轴的范围
	OnValue(&AxisX, x);
	OnValue(&AxisY, y);
}
void CTWenChartGrid::OnValue(CTWenChartGrid::CTWenChartAxisBase* pAxis, double v)
{
	pAxis->ScanValue = v;
	if(pAxis->AxisType & AXISTYPE_ALL)
	{
		if(pAxis->Min > v) pAxis->Min = v;
		else if(pAxis->Max < v) pAxis->Max = v;
		if(pAxis->AxisType & AXISTYPE_STEPS)
		{
			if(pAxis->Max == v) pAxis->Max = pAxis->Max + (pAxis->Max - pAxis->Min) * 0.1;
			pAxis->Step = (pAxis->Max - pAxis->Min)/pAxis->Steps;
		}
	}
	else if(pAxis->AxisType & AXISTYPE_LENGTH)
	{
		double FixedLength = pAxis->Max - pAxis->Min;
		if(pAxis->Min > v)
		{
			pAxis->Min = v;
			pAxis->Max = pAxis->Min + FixedLength;
		}
		else if(pAxis->Max < v)
		{
			pAxis->Max = v;
			pAxis->Min =pAxis-> Max - FixedLength;
		}
	}
	//else if(axis->AxisType &  AXISTYPE_FIXED){}//不会改变坐标范围
	//OnAxisPixelxChanging(&AxisX, Rect.Width(), Rect.Width());
	//OnAxisPixelxChanging(&AxisY, Rect.Height(), Rect.Height());
}
void CTWenChartGrid::OnAxisPixelxChanging(CTWenChartGrid::CTWenChartAxisBase* pAxis, double oldPixels, double newPixels)
{
	if(oldPixels < 1 || newPixels < 1) return;
	pAxis->Step = oldPixels * pAxis->Step / newPixels;
}

//坐标变换
double CTWenChartGrid::ToLogical_X(double v) const
{
	ToLogical_X(1, &v);
	return v;
}
long CTWenChartGrid::ToPixel_X(double v) const
{
	ToPixel_X(1, &v);
	return (long)v;
}
double CTWenChartGrid::ToLogical_Y(double v) const
{
	ToLogical_Y(1, &v);
	return v;
}
long CTWenChartGrid::ToPixel_Y(double v) const
{
	ToPixel_Y(1, &v);
	return (long)v;
}

void CTWenChartGrid::ToLogical_X(DWORD count, double *v) const
{
	switch(AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT://从左向右
		{
			double x = (AxisX.Max - AxisX.Min)/Rect.Width();
			for(DWORD i=0; i<count; i++)
			{
				v[i] = x * (v[i] - Rect.left) + AxisX.Min;
			}
		}
		break;
	case CTWenChartGrid::POSITION_RIGHT://从右向左
		{
			double x = (AxisX.Max - AxisX.Min)/Rect.Width();
			for(DWORD i=0; i<count; i++)
			{
				v[i] = x * (Rect.right - v[i]) + AxisX.Min;
			}
		}
		break;
	}
}
void CTWenChartGrid::ToPixel_X(DWORD count, double *v) const
{
	switch(AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT://从左向右
		{
			double x = Rect.Width()/(AxisX.Max - AxisX.Min);
			for(DWORD i=0; i<count; i++)
			{
				v[i] = x * (v[i] - AxisX.Min) + Rect.left;
			}
		}
		break;
	case CTWenChartGrid::POSITION_RIGHT://从右向左
		{
			double x = Rect.Width()/(AxisX.Max - AxisX.Min);
			for(DWORD i=0; i<count; i++)
			{
				v[i] = Rect.right - x * (v[i] - AxisX.Min);
			}
		}
		break;
	}
}
void CTWenChartGrid::ToLogical_Y(DWORD count, double *v) const
{
	switch(AxisX.Position)
	{
	case CTWenChartGrid::POSITION_TOP:
		{
			double x = (AxisY.Max - AxisY.Min)/Rect.Height();
			for(DWORD i=0; i<count; i++)
			{
				v[i] = x * (v[i] - Rect.top) + AxisY.Min;
			}
		}
		break;
	case CTWenChartGrid::POSITION_BOTTOM:
		{
			double x = (AxisY.Max - AxisY.Min)/Rect.Height();
			for(DWORD i=0; i<count; i++)
			{
				v[i] = x * (Rect.bottom - v[i]) + AxisY.Min;
			}
		}
		break;
	}
}
void CTWenChartGrid::ToPixel_Y(DWORD count, double *v) const
{
	switch(AxisX.Position)
	{
	case CTWenChartGrid::POSITION_LEFT://从左向右
		{
			double x = Rect.Height()/(AxisY.Max - AxisY.Min);
			for(DWORD i=0; i<count; i++)
			{
				v[i] = x * (v[i] - AxisY.Min) + Rect.top;
			}
		}
		break;
	case CTWenChartGrid::POSITION_RIGHT://从右向左
		{
			double x = Rect.Height()/(AxisY.Max - AxisY.Min);
			for(DWORD i=0; i<count; i++)
			{
				v[i] = Rect.bottom - x * (v[i] - AxisY.Min);
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartSerie
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartPoints::CTWenChartPoint::CTWenChartPoint():label(NULL)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartPoints::CTWenChartPoints(CTWenChartSerie* parent):Parent(parent),Points(NULL),MaxCount(0),CurrentIndex(0),RangeType(CTWenChartPoints::RT_X)
{
}
CTWenChartPoints::~CTWenChartPoints()
{
	if(Points)
	{
		for(DWORD i=0; i<CurrentIndex; i++)
		{
			CTWenChartPoint& point = Points[i];
			if(point.label) delete point.label;
		}
		delete []Points;
		Points=NULL;
	}
	MaxCount = 0;
	CurrentIndex = 0;
}

CTWenChartPoints::CTWenChartPoint* CTWenChartPoints::AddPoint(double x, double y)
{
	if(CurrentIndex == MaxCount)
	{
		MaxCount += DEFINE_GROWCOUNT;
		CTWenChartPoint* newPoints = new CTWenChartPoint[MaxCount];
		if(CurrentIndex) memcpy(newPoints, Points, sizeof(CTWenChartPoint)*CurrentIndex);
		if(Points) delete []Points;
		Points = newPoints;
	}
	CTWenChartPoint& point = Points[CurrentIndex++];
	point.x = x;
	point.y = y;
	//根据需要，自动设置坐标轴
	if(Parent && Parent->Visible && Parent->IsMainSerie())
	{
		CTWenChartGroup* pGroup = Parent->GetParent();
		if(pGroup) pGroup->Grid.OnPoint(x, y);
	}
	return &point;
}
void CTWenChartPoints::AddPoints(DWORD dwCount, double* x, double* y)
{
	for(DWORD i=0; i<dwCount; i++) AddPoint(x[i], y[i]);
}

CTWenChartGrid* CTWenChartPoints::GetGrid() const
{
	if(!Parent) return NULL;
	CTWenChartGroup* pGroup = Parent->GetParent();
	if(!pGroup) return NULL;
	return &(pGroup->Grid);
}
bool CTWenChartPoints::GetIndexRange(DWORD& from, DWORD& to) const
{
	if(GetCount() == 0) return false;
	from = 0;
	to = GetCount() - 1;
	//获取
	CTWenChartGrid* pGrid = GetGrid();
	if(!pGrid) return false;
	double min, max;
	switch(RangeType)
	{
	case CTWenChartPoints::RT_NONE:ASSERT(false);break;
	case CTWenChartPoints::RT_X:
		if(pGrid->AxisX.AxisType & CTWenChartGrid::AXISTYPE_ALL) return true;
		min = pGrid->AxisX.Min;
		max = pGrid->AxisX.Max;
		for(DWORD i=GetCount()-1; ; i--)
		{
			if(Points[i].x <= max)
			{
				to = i;
				break;
			}
			if(i == 0) return false;
		}
		for(DWORD i=0; i<=to; i++)
		{
			if(Points[i].x >= min)
			{
				from = i;
				break;
			}
			if(i == to) return false;
		}
		break;
	case CTWenChartPoints::RT_Y:
		if(pGrid->AxisY.AxisType & CTWenChartGrid::AXISTYPE_ALL) return true;
		min = pGrid->AxisY.Min;
		max = pGrid->AxisY.Max;
		for(DWORD i=GetCount()-1; ; i--)
		{
			if(Points[i].y <= max)
			{
				to = i;
				break;
			}
			if(i == 0) return false;
		}
		for(DWORD i=0; i<=to; i++)
		{
			if(Points[i].y >= min)
			{
				from = i;
				break;
			}
			if(i == to) return false;
		}
		break;
	case CTWenChartPoints::RT_XY:ASSERT(false);break;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartSerie::CTWenChartSerie(CTWenChartGroup* parent, CTWenChartSerie::enumSerieType serieType):Parent(parent),SerieType(serieType),Visible(true),Color(NULL),Data(this)
{
	static DWORD SerieID = 0;
	SerieID++;
	ID = SerieID;
	Color = CTWenChartColor::Colors[ID % CTWenChartColor::COLOR_COUNT];
}

bool CTWenChartSerie::IsMainSerie() const
{
	if(!Parent) return false;
	return Parent->GetMainSerie() == this;
}
bool CTWenChartSerie::GetIndexRange(DWORD& from, DWORD& to) const
{
	return Data.GetIndexRange(from, to);
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartLineSerie::CTWenChartLineSerie(CTWenChartGroup* parent):CTWenChartSerie(parent, CTWenChartSerie::ST_LINE)
{
}

bool CTWenChartLineSerie::GetIndexRange(DWORD& from, DWORD& to) const
{
	if(!Data.GetIndexRange(from, to)) return false;
	if(from > 0) from--;
	if(to < Data.GetCount() - 1) to++;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartPointSerie::CTWenChartPointSerie(CTWenChartGroup* parent):CTWenChartSerie(parent, CTWenChartSerie::ST_POINT),Radius(5)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartBarSerie::CTWenChartBarSerie(CTWenChartGroup* parent):CTWenChartSerie(parent, CTWenChartSerie::ST_BAR),Width(10)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartSurfaceSerie::CTWenChartSurfaceSerie(CTWenChartGroup* parent):CTWenChartSerie(parent, CTWenChartSerie::ST_SURFACE),Smooth(3)
{
}

bool CTWenChartSurfaceSerie::GetIndexRange(DWORD& from, DWORD& to) const
{
	if(!Data.GetIndexRange(from, to)) return false;
	if(from > 0) from--;
	if(to < Data.GetCount() - 1) to++;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartPiePiece
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartPiePiece::CTWenChartPiePiece(CTWenChartGroup* parent, LPCTSTR name, double data):Parent(parent),Name(name?name:_T("")),Data(data),Color(NULL)
{
	static DWORD PiePieceID = 0;
	PiePieceID++;
	ID = PiePieceID;
	Color = CTWenChartColor::Colors[ID % CTWenChartColor::COLOR_COUNT];
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartGroup
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartGroup::CTWenChartGroup(CTWenChartCtrl* parent):Parent(parent),DataType(DATATYPE_SERIE),Visible(false),MainSerie(NULL),Grid(this)
{
	static DWORD GroupID = 0;
	GroupID++;
	ID = GroupID;
}
CTWenChartGroup::~CTWenChartGroup()
{
	DeleteSeries();
	DeleteTexts();
	DeletePiePieces();
	MainSerie = NULL;
}

void CTWenChartGroup::SetMainSerieByID(DWORD dwSerieID)
{
	MainSerie = GetSerieByID(dwSerieID);
}
void CTWenChartGroup::SetMainSerieByIndex(DWORD dwSerieIndex)
{
	MainSerie = GetSerieByIndex(dwSerieIndex);
}

//Serie集合操作
CTWenChartLineSerie* CTWenChartGroup::NewLineSerie()
{
	CTWenChartLineSerie* pSerie = new CTWenChartLineSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenChartPointSerie* CTWenChartGroup::NewPointSerie()
{
	CTWenChartPointSerie* pSerie = new CTWenChartPointSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenChartBarSerie* CTWenChartGroup::NewBarSerie()
{
	CTWenChartBarSerie* pSerie = new CTWenChartBarSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenChartSurfaceSerie* CTWenChartGroup::NewSurfaceSerie()
{
	CTWenChartSurfaceSerie* pSerie = new CTWenChartSurfaceSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
void CTWenChartGroup::DeleteSerieByID(DWORD dwSerieID)
{
	DeleteSerieByIndex(GetSerieIndexByID(dwSerieID));
}
void CTWenChartGroup::DeleteSerieByIndex(DWORD dwSerieIndex)
{
	CTWenChartSerie* Serie = GetSerieByIndex(dwSerieIndex);
	if(Serie)
	{
		delete Serie;
		Series.RemoveAt(dwSerieIndex);
		if(Serie == MainSerie)
		{
			if(Series.GetCount() > 0) MainSerie = Series.GetAt(0);//是主线，则自动设置0线为主线
			else MainSerie = NULL;
		}
	}
}
CTWenChartSerie* CTWenChartGroup::GetSerieByID(DWORD dwSerieID) const
{
	return GetSerieByIndex(GetSerieIndexByID(dwSerieID));
}
CTWenChartSerie* CTWenChartGroup::GetSerieByIndex(DWORD dwSerieIndex) const
{
	if(dwSerieIndex >= (DWORD)Series.GetCount()) return NULL;
	return Series.GetAt(dwSerieIndex);
}
void CTWenChartGroup::ShowSerieByID(DWORD dwSerieID, bool bVisible)
{
	ShowSerieByIndex(GetSerieIndexByID(dwSerieID), bVisible);
}
void CTWenChartGroup::ShowSerieByIndex(DWORD dwSerieIndex, bool bVisible)
{
	CTWenChartSerie* Serie = GetSerieByIndex(dwSerieIndex);
	if(!Serie) return;
	Serie->Visible = bVisible;
}
DWORD  CTWenChartGroup::GetSerieIndexByID(DWORD dwSerieID) const
{
	for(int i=0; i<Series.GetCount(); i++)
	{
		if(Series.GetAt(i)->GetID() == dwSerieID) return (DWORD)i;
	}
	return -1;
}
void CTWenChartGroup::DeleteSeries()
{
	for(int i=0; i<Series.GetCount(); i++)
	{
		delete Series.GetAt(i);
	}
	Series.RemoveAll();
}
void CTWenChartGroup::AddSerie(CTWenChartSerie* pSerie)
{
	Series.Add(pSerie);
	if(Series.GetCount() == 1) MainSerie = pSerie;//是唯一的一条线，则自动设置为主线
}

//PiePiece集合操作
CTWenChartPiePiece* CTWenChartGroup::NewPiePiece(LPCTSTR name, double data)
{
	CTWenChartPiePiece* PiePiece = new CTWenChartPiePiece(this, name, data);
	PiePieces.Add(PiePiece);
	return PiePiece;
}
void CTWenChartGroup::DeletePiePieceByID(DWORD dwPiePieceID)
{
	DeletePiePieceByIndex(GetPiePieceIndexByID(dwPiePieceID));
}
void CTWenChartGroup::DeletePiePieceByIndex(DWORD dwPiePieceIndex)
{
	CTWenChartPiePiece* PiePiece = GetPiePieceByIndex(dwPiePieceIndex);
	if(PiePiece)
	{
		delete PiePiece;
		PiePieces.RemoveAt(dwPiePieceIndex);
	}
}
CTWenChartPiePiece* CTWenChartGroup::GetPiePieceByID(DWORD dwPiePieceID) const
{
	return GetPiePieceByIndex(GetPiePieceIndexByID(dwPiePieceID));
}
CTWenChartPiePiece* CTWenChartGroup::GetPiePieceByIndex(DWORD dwPiePieceIndex) const
{
	if(dwPiePieceIndex >= (DWORD)PiePieces.GetCount()) return NULL;
	return PiePieces.GetAt(dwPiePieceIndex);
}
DWORD  CTWenChartGroup::GetPiePieceIndexByID(DWORD dwPiePieceID) const
{
	for(int i=0; i<PiePieces.GetCount(); i++)
	{
		if(PiePieces.GetAt(i)->GetID() == dwPiePieceID) return (DWORD)i;
	}
	return -1;
}
void CTWenChartGroup::DeletePiePieces()
{
	for(int i=0; i<PiePieces.GetCount(); i++)
	{
		delete PiePieces.GetAt(i);
	}
	PiePieces.RemoveAll();
}

//Text集合操作
CTWenChartText* CTWenChartGroup::NewText()
{
	CTWenChartText* Text = new CTWenChartText();
	Texts.Add(Text);
	return Text;
}
void CTWenChartGroup::DeleteTextByID(DWORD dwTextID)
{
	DeleteTextByIndex(GetTextIndexByID(dwTextID));
}
void CTWenChartGroup::DeleteTextByIndex(DWORD dwTextIndex)
{
	CTWenChartText* Text = GetTextByIndex(dwTextIndex);
	if(Text)
	{
		delete Text;
		Texts.RemoveAt(dwTextIndex);
	}
}
CTWenChartText* CTWenChartGroup::GetTextByID(DWORD dwTextID) const
{
	return GetTextByIndex(GetTextIndexByID(dwTextID));
}
CTWenChartText* CTWenChartGroup::GetTextByIndex(DWORD dwTextIndex) const
{
	if(dwTextIndex >= (DWORD)Texts.GetCount()) return NULL;
	return Texts.GetAt(dwTextIndex);
}
void CTWenChartGroup::ShowTextByID(DWORD dwTextID, bool bVisible)
{
	ShowTextByIndex(GetTextIndexByID(dwTextID), bVisible);
}
void CTWenChartGroup::ShowTextByIndex(DWORD dwTextIndex, bool bVisible)
{
	CTWenChartText* Text = GetTextByIndex(dwTextIndex);
	if(!Text) return;
	Text->Visible = bVisible;
}
DWORD  CTWenChartGroup::GetTextIndexByID(DWORD dwTextID) const
{
	for(int i=0; i<Texts.GetCount(); i++)
	{
		if(Texts.GetAt(i)->GetID() == dwTextID) return (DWORD)i;
	}
	return -1;
}
void CTWenChartGroup::DeleteTexts()
{
	for(int i=0; i<Texts.GetCount(); i++)
	{
		delete Texts.GetAt(i);
	}
	Texts.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenChartCtrl
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartCtrl::structSelectChartFont::structSelectChartFont(CDC* pDC, const CTWenChartFont& ChartFont, CFont& font):m_pDC(pDC),m_pOldFont(NULL)
{
	if(m_pDC)
	{
		m_pOldFont = m_pDC->SelectObject(ChartFont.GetFont(&font));
		m_pDC->SetTextColor(ChartFont.Color);
		if(ChartFont.Transparent) m_pDC->SetBkMode(TRANSPARENT);
		else
		{
			m_pDC->SetBkMode(OPAQUE);
			m_pDC->SetBkColor(ChartFont.BkColor);
		}
	}
}
CTWenChartCtrl::structSelectChartFont::~structSelectChartFont()
{
	if(m_pDC) m_pDC->SelectObject(m_pOldFont);
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenChartCtrl::CTWenChartTextDlg::CTWenChartTextDlg(CWnd* parent, CTWenChartText* pText, bool New):CTemplateDlg(parent),Text(pText)
{
	SetTitle(_T("添加文本"));
	CString sText, sX, sY, sShow(_T("1"));
	if(!New && pText)
	{
		sText = Text->Text;
		sX.Format(_T("%.f"), Text->x);
		sY.Format(_T("%.f"), Text->y);
		sShow = Text->Visible ? _T("1") : _T("0");
	}
	SetCtrlInfo(5,
		_T("文本"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Text|CTemplateDlg::VT_NoZero,sText,
		_T("x:"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Double,sX,
		_T("y:"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Double,sY,
		_T(""),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,sShow+_T("|可见"),
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置字体|1234567")
		);
}

BEGIN_MESSAGE_MAP(CTWenChartCtrl::CTWenChartTextDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenChartCtrl::CTWenChartTextDlg::OnButtonFont)
END_MESSAGE_MAP()

void CTWenChartCtrl::CTWenChartTextDlg::OnButtonFont()
{
	if(!Text) return;
	CTWenChartCtrl::ChartFont(Text->Font, this);
}
void CTWenChartCtrl::CTWenChartTextDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Text) return;
	CString v[5];
	CStringExt::Split(m_strValue,m_strSep, 5, v);
	Text->Text = v[0];
	Text->x = _tstof(v[1]);
	Text->y = _tstof(v[2]);
	Text->Visible = _tstol(v[3])?true:false;
}
//////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTWenChartCtrl, CWnd)

CTWenChartCtrl::CTWenChartCtrl():CurrentGroup(NULL),BGColor(CTWenChartColor::Colors[CTWenChartColor::COLOR_WHITE]),
AutoAxisX(true), AutoAxisY(true),CurrentSelect(-1),CurrentSelectText(-1),Cursor(NULL),Posiotion_AutoAxisX(-1),Posiotion_AutoAxisY(-1)
{
	AutoAxisFont.Color = CTWenChartColor::Colors[CTWenChartColor::COLOR_WHITE];
	AutoAxisFont.BkColor = CTWenChartColor::Colors[CTWenChartColor::COLOR_BLACK];
	AutoAxisFont.Transparent = false;
}

CTWenChartCtrl::~CTWenChartCtrl()
{
	DeleteGroups();
}

void CTWenChartCtrl::Create(CWnd* parent, const CRect& rc)
{
	CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rc, parent, NULL);
	Cursor = LoadCursor(NULL,IDC_CROSS);
	CurrentGroup = NewGroup();
	CurrentGroup->Visible = true;
}
void CTWenChartCtrl::Redraw(bool bRebuilding)
{
	if(bRebuilding) RebuildChart();
	Invalidate();
}

void CTWenChartCtrl::OnInitMenu(CMenu& Menu)
{
	CMenu menuTemp;

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(Title.Visible?MF_CHECKED:NULL), ID_MENU_TITLE_SHOW, _T("(&S)显示"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(Title.Visible?NULL:MF_CHECKED), ID_MENU_TITLE_HIDE, _T("(&H)隐藏"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_TITLE_FONT, _T("(&F)字体"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&T)标题"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(Legend.Visible?MF_CHECKED:NULL), ID_MENU_LEGEND_SHOW, _T("(&S)显示"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(Legend.Visible?NULL:MF_CHECKED), ID_MENU_LEGEND_HIDE, _T("(&H)隐藏"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_LEGEND_FONT, _T("(&F)字体"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&L)图例"));
	menuTemp.DestroyMenu();

	Menu.AppendMenu(MF_SEPARATOR);

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(AutoAxisX?MF_CHECKED:NULL), ID_MENU_AUTOAXISX_ENABLE, _T("(&O)打开"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(AutoAxisX?NULL:MF_CHECKED), ID_MENU_AUTOAXISX_DISABLE, _T("(&C)关闭"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&X)自动浏览轴X"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(AutoAxisY?MF_CHECKED:NULL), ID_MENU_AUTOAXISY_ENABLE, _T("(&O)打开"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(AutoAxisY?NULL:MF_CHECKED), ID_MENU_AUTOAXISY_DISABLE, _T("(&C)关闭"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&Y)自动浏览轴Y"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_AUTOAXIS_FONT, _T("(&F)字体"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_AUTOAXIS_BGCOLOR, _T("(&B)背景色"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&A)自动浏览轴"));
	menuTemp.DestroyMenu();

	Menu.AppendMenu(MF_SEPARATOR);

	DWORD MF_CG = CurrentGroup?MF_ENABLED:MF_GRAYED|MF_DISABLED;
	CMenu menuTempCG;
	menuTempCG.CreateMenu();
	DWORD gCount = (DWORD)Groups.GetCount();
	if(gCount > 0)
	{
		for(DWORD i=0; i<gCount; i++)
		{
			CTWenChartGroup* pGroup = Groups.GetAt(i);
			CString t;
			t.Format(_T("组ID:%u"), pGroup->GetID());
			menuTempCG.AppendMenu(MF_STRING|MF_ENABLED|(pGroup == CurrentGroup ? MF_CHECKED : NULL), ID_MENU_GROUPFROM + i, t);
		}
		menuTempCG.AppendMenu(MF_SEPARATOR);
	}

	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_NEW_TEXT, _T("(&T)添加文本"));
	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_NEW_LINESERIE, _T("(&L)添加曲线图"));
	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_NEW_POINTSERIE, _T("(&P)添加散点图"));
	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_NEW_BARSERIE, _T("(&B)添加柱状图"));
	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_NEW_SURFACESERIE, _T("(&S)添加曲面图"));

	menuTempCG.AppendMenu(MF_SEPARATOR);

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_GRID_AXISX_BASESET, _T("(&X)X轴"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_GRID_AXISY_BASESET, _T("(&Y)Y轴"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&R)坐标轴基值"));
	menuTemp.DestroyMenu();

	//当前文本
	bool bCurText = false;
	DWORD cCountText = 0;
	if(CurrentGroup)
	{
		cCountText = (DWORD)CurrentGroup->Texts.GetCount();
		bCurText = CurrentSelectText < cCountText;
	}
	menuTemp.CreateMenu();
	if(CurrentGroup)
	{
		for(DWORD i=0; i<cCountText; i++)
		{
			menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(CurrentSelectText == i ? MF_CHECKED : NULL), ID_MENU_TEXTFROM + i, CurrentGroup->Texts.GetAt(i)->Text);
		}
		if(cCountText > 0) menuTemp.AppendMenu(MF_SEPARATOR);
	}
	menuTemp.AppendMenu(MF_STRING|(bCurText?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_TEXT_UPDATE, _T("(&U)修改"));
	menuTemp.AppendMenu(MF_STRING|(bCurText?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_TEXT_DELETE, _T("(&D)删除"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&T)当前文本"));
	//当前组再添加到当前组之前以便快捷操作
	Menu.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&T)当前文本"));
	menuTemp.DestroyMenu();

	//当前曲线
	bool bCurCurve = false;
	DWORD cCount = 0;
	if(CurrentGroup)
	{
		switch(CurrentGroup->DataType)
		{
		case CTWenChartGroup::DATATYPE_SERIE:cCount = (DWORD)CurrentGroup->Series.GetCount();break;
		case CTWenChartGroup::DATATYPE_PIE:cCount = (DWORD)CurrentGroup->PiePieces.GetCount();break;
		}
		bCurCurve = CurrentSelect < cCount;
	}
	menuTemp.CreateMenu();
	if(CurrentGroup)
	{
		switch(CurrentGroup->DataType)
		{
		case CTWenChartGroup::DATATYPE_SERIE:
			for(DWORD i=0; i<cCount; i++)
			{
				menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(CurrentSelect == i ? MF_CHECKED : NULL), ID_MENU_SERIEORPIEPIECE + i, CurrentGroup->Series.GetAt(i)->Name);
			}
			break;
		case CTWenChartGroup::DATATYPE_PIE:
			for(DWORD i=0; i<cCount; i++)
			{
				menuTemp.AppendMenu(MF_STRING|MF_ENABLED|(CurrentSelect == i ? MF_CHECKED : NULL), ID_MENU_SERIEORPIEPIECE + i, CurrentGroup->PiePieces.GetAt(i)->Name);
			}
			break;
		}
		if(cCount > 0) menuTemp.AppendMenu(MF_SEPARATOR);
	}
	menuTemp.AppendMenu(MF_STRING|(bCurCurve?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_SERIE_COLOR, _T("(&C)颜色"));
	menuTemp.AppendMenu(MF_STRING|MF_GRAYED|(bCurCurve?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_SERIE_TYPE, _T("(&T)类型"));
	menuTemp.AppendMenu(MF_STRING|(bCurCurve?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_SERIE_DATA_SAVE, _T("(&S)保存"));
	menuTemp.AppendMenu(MF_STRING|(bCurCurve?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_SERIE_DATA_LOAD, _T("(&L)加载"));
	menuTemp.AppendMenu(MF_STRING|(bCurCurve?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_SERIE_DATA_CLEAR, _T("(&E)清除"));
	menuTemp.AppendMenu(MF_SEPARATOR);
	menuTemp.AppendMenu(MF_STRING|(bCurCurve?MF_ENABLED:MF_GRAYED|MF_DISABLED), ID_MENU_SERIE_DATA_ADDPOINT, _T("(&A)加点"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&C)当前曲线"));
	//当前组再添加到当前组之前以便快捷操作
	Menu.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&S)当前曲线"));
	menuTemp.DestroyMenu();

	menuTempCG.AppendMenu(MF_SEPARATOR);

	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_GRID_AXIS_FONT, _T("(&F)坐标轴字体"));

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.AxisX.ScanLine.Visible)?MF_CHECKED:NULL), ID_MENU_GRID_AXISX_SCANLINE_SHOW, _T("(&S)显示"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && !CurrentGroup->Grid.AxisX.ScanLine.Visible)?MF_CHECKED:NULL), ID_MENU_GRID_AXISX_SCANLINE_HIDE, _T("(&H)隐藏"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&X)X轴扫描线"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.AxisY.ScanLine.Visible)?MF_CHECKED:NULL), ID_MENU_GRID_AXISY_SCANLINE_SHOW, _T("(&S)显示"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && !CurrentGroup->Grid.AxisY.ScanLine.Visible)?MF_CHECKED:NULL), ID_MENU_GRID_AXISY_SCANLINE_HIDE, _T("(&H)隐藏"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&Y)Y轴扫描线"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.AxisX.Position == CTWenChartGrid::POSITION_TOP)?MF_CHECKED:NULL), ID_MENU_GRID_AXISX_TOP, _T("(&T)顶部"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.AxisX.Position == CTWenChartGrid::POSITION_BOTTOM)?MF_CHECKED:NULL), ID_MENU_GRID_AXISX_BOTTOM, _T("(&B)底部"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&X)X轴位置"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.AxisY.Position == CTWenChartGrid::POSITION_LEFT)?MF_CHECKED:NULL), ID_MENU_GRID_AXISY_LEFT, _T("(&L)左侧"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.AxisY.Position == CTWenChartGrid::POSITION_RIGHT)?MF_CHECKED:NULL), ID_MENU_GRID_AXISY_RIGHT, _T("(&R)右侧"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&Y)Y轴位置"));
	menuTemp.DestroyMenu();

	menuTempCG.AppendMenu(MF_SEPARATOR);

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_CG, ID_MENU_GRID_BGCOLOR, _T("(&B)背景色"));
	menuTemp.AppendMenu(MF_SEPARATOR);
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && CurrentGroup->Grid.Visible)?MF_CHECKED:NULL), ID_MENU_GRID_LINE_SHOW, _T("(&S)显示"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|((CurrentGroup && !CurrentGroup->Grid.Visible)?MF_CHECKED:NULL), ID_MENU_GRID_LINE_HIDE, _T("(&H)隐藏"));
	menuTempCG.AppendMenu(MF_POPUP|MF_CG, (UINT_PTR)menuTemp.m_hMenu, _T("(&G)网格"));
	menuTemp.DestroyMenu();

	menuTempCG.AppendMenu(MF_SEPARATOR);

	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_DATA_SAVE, _T("(&S)保存数据"));
	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_DATA_LOAD, _T("(&L)加载数据"));
	menuTempCG.AppendMenu(MF_STRING|MF_CG, ID_MENU_CURRENTGROUP_DATA_CLEAR, _T("(&C)清除数据"));

	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTempCG.m_hMenu, _T("(&C)当前组"));
	menuTempCG.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_GROUPS_NEW, _T("(&N)新建组"));
	menuTemp.AppendMenu(MF_SEPARATOR);
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|MF_GRAYED, ID_MENU_GROUPS_DATA_SAVE, _T("(&S)保存数据"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED|MF_GRAYED, ID_MENU_GROUPS_DATA_LOAD, _T("(&L)加载数据"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_GROUPS_DATA_CLEAR, _T("(&C)清除数据"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&G)所有组"));
	menuTemp.DestroyMenu();

	Menu.AppendMenu(MF_SEPARATOR);
	
	Menu.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_CTRL_BGCOLOR, _T("(&B)背  景  色"));
	Menu.AppendMenu(MF_STRING|MF_ENABLED|MF_GRAYED, ID_MENU_SAVE, _T("(&S)保存图表"));
	Menu.AppendMenu(MF_STRING|MF_ENABLED|MF_GRAYED, ID_MENU_LOAD, _T("(&L)加载图表"));
}
void CTWenChartCtrl::OnMenu(DWORD dwMenuID)
{
	if(dwMenuID >= ID_MENU_GROUPFROM && dwMenuID <= ID_MENU_GROUPTO)
	{
		CurrentSelect = -1;
		CurrentSelectText = -1;
		SetCurGroupByIndex(dwMenuID - ID_MENU_GROUPFROM);
		return;
	}
	if(dwMenuID >= ID_MENU_TEXTFROM && dwMenuID <= ID_MENU_TEXTTO)
	{
		CurrentSelectText = dwMenuID - ID_MENU_TEXTFROM;
		return;
	}
	if(dwMenuID >= ID_MENU_SERIEORPIEPIECE && dwMenuID <= ID_MENU_RANGE_TO)
	{
		CurrentSelect = dwMenuID - ID_MENU_SERIEORPIEPIECE;
		if(CurrentGroup) CurrentGroup->SetMainSerieByIndex(CurrentSelect);
		return;
	}
	switch(dwMenuID)
	{
	case ID_MENU_TITLE_SHOW:Title.Visible = true;break;
	case ID_MENU_TITLE_HIDE:Title.Visible = false;break;
	case ID_MENU_TITLE_FONT:if(!ChartFont(Title.Font, this)) return;break;

	case ID_MENU_LEGEND_SHOW:Legend.Visible = true;break;
	case ID_MENU_LEGEND_HIDE:Legend.Visible = false;break;
	case ID_MENU_LEGEND_FONT:if(!ChartFont(Legend.Font, this)) return;break;

	case ID_MENU_AUTOAXISX_ENABLE:AutoAxisX = true;break;
	case ID_MENU_AUTOAXISX_DISABLE:AutoAxisX = false;break;

	case ID_MENU_AUTOAXISY_ENABLE:AutoAxisY = true;break;
	case ID_MENU_AUTOAXISY_DISABLE:AutoAxisY = false;break;

	case ID_MENU_AUTOAXIS_FONT:
		if(!ChartFont(AutoAxisFont, this)) return;
		break;
	case ID_MENU_AUTOAXIS_BGCOLOR:
		if(!ChartColor(AutoAxisFont.BkColor)) return;
		break;

	case ID_MENU_CTRL_BGCOLOR:if(!ChartColor(BGColor)) return;break;
	case ID_MENU_SAVE:
		TRACE("Not Supported:ID_MENU_SAVE.\n");
		break;
	case ID_MENU_LOAD:
		TRACE("Not Supported:ID_MENU_LOAD.\n");
		break;

	case ID_MENU_GROUPS_NEW:
		{
			CTWenChartGroup* pGroup = NewGroup();
			if(Groups.GetCount() == 1)
			{
				CurrentGroup = pGroup;
				CurrentGroup->Visible = true;
			}
		}
		break;
	case ID_MENU_GROUPS_DATA_SAVE:
		TRACE("Not Supported:ID_MENU_GROUPS_DATA_SAVE.\n");
		break;
	case ID_MENU_GROUPS_DATA_LOAD:
		TRACE("Not Supported:ID_MENU_GROUPS_DATA_LOAD.\n");
		break;
	case ID_MENU_GROUPS_DATA_CLEAR:
		DeleteGroups();
		CurrentGroup = NULL;
		break;

	case ID_MENU_CURRENTGROUP_NEW_TEXT:
		if(CurrentGroup)
		{
			CTWenChartTextDlg dlg(this, new CTWenChartText, true);
			if(dlg.DoModal() !=  IDOK)
			{
				delete dlg.Text;
				return;
			}
			CurrentGroup->Texts.Add(dlg.Text);
		}
		break;
	case ID_MENU_CURRENTGROUP_NEW_LINESERIE:
		if(CurrentGroup)
		{
			CTWenChartSerie* pSerie = CurrentGroup->NewLineSerie();
			pSerie->Name.Format(_T("Line%u"), pSerie->GetID());
		}
		break;
	case ID_MENU_CURRENTGROUP_NEW_POINTSERIE:
		if(CurrentGroup)
		{
			CTWenChartSerie* pSerie = CurrentGroup->NewPointSerie();
			pSerie->Name.Format(_T("Points%u"), pSerie->GetID());
		}
		break;
	case ID_MENU_CURRENTGROUP_NEW_BARSERIE:
		if(CurrentGroup)
		{
			CTWenChartSerie* pSerie = CurrentGroup->NewBarSerie();
			pSerie->Name.Format(_T("Bar%u"), pSerie->GetID());
		}
		break;
	case ID_MENU_CURRENTGROUP_NEW_SURFACESERIE:
		if(CurrentGroup)
		{
			CTWenChartSerie* pSerie = CurrentGroup->NewSurfaceSerie();
			pSerie->Name.Format(_T("Surface%u"), pSerie->GetID());
		}
		break;
	case ID_MENU_CURRENTGROUP_DATA_SAVE:
		if(CurrentGroup)
		{
			CString s;
			s.Format(_T("Group%u"), CurrentGroup->GetID());
			switch (CurrentGroup->DataType)
			{
			case CTWenChartGroup::DATATYPE_SERIE:
				{
					CFile file;
					if(!GetSaveFile(file, s, _T("保存到文件"))) return;
					for(int i=0; i<CurrentGroup->Series.GetCount(); i++) SaveSerie(file, CurrentGroup->Series.GetAt(i));
					MessageBox(_T("组数据已保存到文件[")+file.GetFilePath()+_T("]"));
					file.Close();
				}
				break;
			case CTWenChartGroup::DATATYPE_PIE:
				ASSERT(false);
				break;
			}
		}
		return;
		break;
	case ID_MENU_CURRENTGROUP_DATA_LOAD:
		if(CurrentGroup)
		{
			CString s;
			switch (CurrentGroup->DataType)
			{
			case CTWenChartGroup::DATATYPE_SERIE:
				{
					CFile file;
					if(!GetLoadFile(file, s, _T("从文件加载"))) return;
					for(int i=0; i<CurrentGroup->Series.GetCount(); i++) LoadSerie(file, CurrentGroup->Series.GetAt(i));
					MessageBox(_T("已从文件[")+file.GetFilePath()+_T("加载组数据]"));
					file.Close();
				}
				break;
			case CTWenChartGroup::DATATYPE_PIE:
				ASSERT(false);
				break;
			}
		}
		break;
	case ID_MENU_CURRENTGROUP_DATA_CLEAR:
		if(CurrentGroup){CurrentGroup->DeletePiePieces();CurrentGroup->DeleteSeries();}
		break;

	case ID_MENU_GRID_BGCOLOR:if(!CurrentGroup || !ChartColor(CurrentGroup->Grid.BGColor)) return;break;

	case ID_MENU_GRID_LINE_SHOW:if(CurrentGroup) CurrentGroup->Grid.Visible = true;break;
	case ID_MENU_GRID_LINE_HIDE:if(CurrentGroup) CurrentGroup->Grid.Visible = false;break;

	case ID_MENU_GRID_AXIS_FONT:
		if(!CurrentGroup) return;
		if(!ChartFont(CurrentGroup->Grid.AxisTextFont, this)) return;
		break;

	case ID_MENU_GRID_AXISX_TOP:if(CurrentGroup) CurrentGroup->Grid.AxisX.Position = CTWenChartGrid::POSITION_TOP;break;
	case ID_MENU_GRID_AXISX_BOTTOM:if(CurrentGroup) CurrentGroup->Grid.AxisX.Position = CTWenChartGrid::POSITION_BOTTOM;break;

	case ID_MENU_GRID_AXISY_LEFT:if(CurrentGroup) CurrentGroup->Grid.AxisY.Position = CTWenChartGrid::POSITION_LEFT;break;
	case ID_MENU_GRID_AXISY_RIGHT:if(CurrentGroup) CurrentGroup->Grid.AxisY.Position = CTWenChartGrid::POSITION_RIGHT;break;

	case ID_MENU_GRID_AXISX_BASESET:
		if(!CurrentGroup) return;
		if(!ChartAxisRange(CurrentGroup->Grid.AxisX)) return;
		break;
	case ID_MENU_GRID_AXISY_BASESET:
		if(!CurrentGroup) return;
		if(!ChartAxisRange(CurrentGroup->Grid.AxisY)) return;
		break;


	case ID_MENU_GRID_AXISX_SCANLINE_SHOW:if(CurrentGroup) CurrentGroup->Grid.AxisX.ScanLine.Visible = true;break;
	case ID_MENU_GRID_AXISX_SCANLINE_HIDE:if(CurrentGroup) CurrentGroup->Grid.AxisX.ScanLine.Visible = false;break;

	case ID_MENU_GRID_AXISY_SCANLINE_SHOW:if(CurrentGroup) CurrentGroup->Grid.AxisY.ScanLine.Visible = true;break;
	case ID_MENU_GRID_AXISY_SCANLINE_HIDE:if(CurrentGroup) CurrentGroup->Grid.AxisY.ScanLine.Visible = false;break;

	case ID_MENU_TEXT_UPDATE:
		if(CurrentGroup)
		{
			CTWenChartTextDlg dlg(this, CurrentGroup->GetTextByIndex(CurrentSelectText), false);
			if(dlg.DoModal() !=  IDOK) return;
		}
		break;
	case ID_MENU_TEXT_DELETE:
		if(CurrentGroup)
		{
			CurrentGroup->DeleteTextByIndex(CurrentSelectText);
			CurrentSelectText = -1;
		}
		break;

	case ID_MENU_SERIE_COLOR:
		{
			DWORD * pCr = NULL;
			if(CurrentGroup)
			{
				switch (CurrentGroup->DataType)
				{
				case CTWenChartGroup::DATATYPE_SERIE:if(CurrentSelect < (DWORD)CurrentGroup->Series.GetCount()) pCr = &(CurrentGroup->Series.GetAt(CurrentSelect)->Color);break;
				case CTWenChartGroup::DATATYPE_PIE:if(CurrentSelect < (DWORD)CurrentGroup->PiePieces.GetCount()) pCr = &(CurrentGroup->PiePieces.GetAt(CurrentSelect)->Color);break;
				}
			}
			if(!pCr || !ChartColor(*pCr)) return;
		}
		break;
	case ID_MENU_SERIE_TYPE:
		TRACE("Not Supported:ID_MENU_SERIE_DATA_SAVE.\n");
		break;
	case ID_MENU_SERIE_DATA_SAVE:
		if(CurrentGroup)
		{
			switch (CurrentGroup->DataType)
			{
			case CTWenChartGroup::DATATYPE_SERIE:
				if(CurrentSelect < (DWORD)CurrentGroup->Series.GetCount())
				{
					CFile file;
					if(!GetSaveFile(file, CurrentGroup->Series.GetAt(CurrentSelect)->Name, _T("保存到文件"))) return;
					SaveSerie(file, CurrentGroup->Series.GetAt(CurrentSelect));
					MessageBox(_T("数据已保存到文件[")+file.GetFilePath()+_T("]"));
					file.Close();
				}
				break;
			case CTWenChartGroup::DATATYPE_PIE:
				if(CurrentSelect < (DWORD)CurrentGroup->PiePieces.GetCount())
				{
					CTWenChartPiePiece* pPiece = CurrentGroup->PiePieces.GetAt(CurrentSelect);
					ASSERT(false);
				}
				break;
			}
		}
		return;
		break;
	case ID_MENU_SERIE_DATA_LOAD:
		if(CurrentGroup)
		{
			switch (CurrentGroup->DataType)
			{
			case CTWenChartGroup::DATATYPE_SERIE:
				if(CurrentSelect < (DWORD)CurrentGroup->Series.GetCount())
				{
					CFile file;
					if(!GetLoadFile(file, CurrentGroup->Series.GetAt(CurrentSelect)->Name, _T("从文件加载"))) return;
					LoadSerie(file, CurrentGroup->Series.GetAt(CurrentSelect));
					MessageBox(_T("已从文件[")+file.GetFilePath()+_T("加载数据]"));
					file.Close();
				}
				break;
			case CTWenChartGroup::DATATYPE_PIE:
				if(CurrentSelect < (DWORD)CurrentGroup->PiePieces.GetCount())
				{
					CTWenChartPiePiece* pPiece = CurrentGroup->PiePieces.GetAt(CurrentSelect);
					ASSERT(false);
				}
				break;
			}
		}
		break;
	case ID_MENU_SERIE_DATA_CLEAR:
		if(CurrentGroup)
		{
			switch (CurrentGroup->DataType)
			{
			case CTWenChartGroup::DATATYPE_SERIE:
				if(CurrentSelect < (DWORD)CurrentGroup->Series.GetCount()) CurrentGroup->Series.GetAt(CurrentSelect)->Data.CurrentIndex = 0;
				break;
			case CTWenChartGroup::DATATYPE_PIE:
				if(CurrentSelect < (DWORD)CurrentGroup->PiePieces.GetCount())
				{
					CTWenChartPiePiece* pPiece = CurrentGroup->PiePieces.GetAt(CurrentSelect);
					ASSERT(false);
				}
				break;
			}
		}
		break;
	case ID_MENU_SERIE_DATA_ADDPOINT:
		if(CurrentGroup)
		{
			switch (CurrentGroup->DataType)
			{
			case CTWenChartGroup::DATATYPE_SERIE:
				if(CurrentSelect < (DWORD)CurrentGroup->Series.GetCount())
				{
					CTemplateDlg dlg(this);
					dlg.SetTitle(_T("添加点"));
					dlg.SetCtrlInfo(3,
						_T("x:"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Double,_T(""),
						_T("y:"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Double,_T(""),
						_T("标签"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Text,_T("")
						);
					if(dlg.DoModal() != IDOK) return;
					CString x, y, label;
					CStringExt::Split(dlg.m_strValue,dlg.m_strSep, 3, &x, &y, &label);
					CTWenChartPoints::CTWenChartPoint* pPoint = CurrentGroup->Series.GetAt(CurrentSelect)->Data.AddPoint(_tstof(x), _tstof(y));
					if(!label.IsEmpty())
					{
						pPoint->label = new CTWenChartPoints::CTWenChartPointLabel;
						pPoint->label->Text = label;
					}
				}
				break;
			case CTWenChartGroup::DATATYPE_PIE:
				if(CurrentSelect < (DWORD)CurrentGroup->PiePieces.GetCount())
				{
					CTWenChartPiePiece* pPiece = CurrentGroup->PiePieces.GetAt(CurrentSelect);
					ASSERT(false);
				}
				break;
			}
		}
		break;
	default:return;
	}
	Redraw(true);
}

BEGIN_MESSAGE_MAP(CTWenChartCtrl, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND_RANGE(ID_MENU_RANGE_FROM, ID_MENU_RANGE_TO, &CTWenChartCtrl::OnMenuCommandRange)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CTWenChartCtrl 消息处理程序
void CTWenChartCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Redraw(true);
}
BOOL CTWenChartCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&ChartImage);
	BITMAP bm;
	ChartImage.GetBitmap(&bm);
	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0,0,rc.Width(),rc.Height(),&dc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	dc.SelectObject(pOldBitmap);
	return TRUE;
}
void CTWenChartCtrl::OnMenuCommandRange(UINT id)
{
	OnMenu(id);
}
void CTWenChartCtrl::OnMouseMove(UINT nFlags, CPoint point)
{/*
	Posiotion_AutoAxisX = point.x;
	Posiotion_AutoAxisY = point.y;
	if(AutoAxisX || AutoAxisY) Redraw(true);//*/
	CWnd::OnMouseMove(nFlags, point);
}
void CTWenChartCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	OnInitMenu(menu);
	CPoint pt(point);
	ClientToScreen(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();

	CWnd::OnRButtonDown(nFlags, point);
}
void CTWenChartCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	Posiotion_AutoAxisX = point.x;
	Posiotion_AutoAxisY = point.y;
	if(AutoAxisX || AutoAxisY) Redraw(true);
	CWnd::OnLButtonDown(nFlags, point);
}
BOOL CTWenChartCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT msg) 
{
	::SetCursor(Cursor);
	return TRUE;
}
BOOL CTWenChartCtrl::PreTranslateMessage(MSG* pMsg)
{
	static bool shiftDown = false;
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SHIFT) shiftDown = true;
	if(pMsg->message == WM_KEYUP && pMsg->wParam == VK_SHIFT) shiftDown = false;

	/*
	static bool ctrlDown = false;
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CONTROL) ctrlDown = true;
	if(pMsg->message == WM_KEYUP && pMsg->wParam == VK_CONTROL) ctrlDown = false;
	//*/

	if(pMsg->message == WM_KEYDOWN)
	{
		int pixels = shiftDown?5:1;
		switch(pMsg->wParam)
		{
		case VK_LEFT:
			Posiotion_AutoAxisX -= pixels;
			if(AutoAxisX || AutoAxisY) Redraw(true);break;
		case VK_UP:
			Posiotion_AutoAxisY -= pixels;
			if(AutoAxisX || AutoAxisY) Redraw(true);break;
		case VK_RIGHT:
			Posiotion_AutoAxisX += pixels;
			if(AutoAxisX || AutoAxisY) Redraw(true);break;
		case VK_DOWN:
			Posiotion_AutoAxisY += pixels;
			if(AutoAxisX || AutoAxisY) Redraw(true);break;
		}
	} 

	return CWnd::PreTranslateMessage(pMsg);
}
//组操作
CTWenChartGroup* CTWenChartCtrl::NewGroup()
{
	CTWenChartGroup* Group = new CTWenChartGroup(this);
	Groups.Add(Group);
	return Group;
}
void CTWenChartCtrl::DeleteGroupByID(DWORD dwGroupID)
{
	DeleteGroupByIndex(GetGroupIndexByID(dwGroupID));
}
void CTWenChartCtrl::DeleteGroupByIndex(DWORD dwGroupIndex)
{
	CTWenChartGroup* Group = GetGroupByIndex(dwGroupIndex);
	if(Group)
	{
		delete Group;
		Groups.RemoveAt(dwGroupIndex);
		if(CurrentGroup == Group) CurrentGroup = NULL;
	}
}
CTWenChartGroup* CTWenChartCtrl::GetGroupByID(DWORD dwGroupID) const
{
	return GetGroupByIndex(GetGroupIndexByID(dwGroupID));
}
CTWenChartGroup* CTWenChartCtrl::GetGroupByIndex(DWORD dwGroupIndex) const
{
	if(dwGroupIndex >= (DWORD)Groups.GetCount()) return NULL;
	return Groups.GetAt(dwGroupIndex);
}
DWORD  CTWenChartCtrl::GetGroupIndexByID(DWORD dwGroupID) const
{
	for(int i=0; i<Groups.GetCount(); i++)
	{
		if(Groups.GetAt(i)->GetID() == dwGroupID) return (DWORD)i;
	}
	return -1;
}
void CTWenChartCtrl::DeleteGroups()
{
	for(int i=0; i<Groups.GetCount(); i++)
	{
		delete Groups.GetAt(i);
	}
	Groups.RemoveAll();
}

void CTWenChartCtrl::SetCurGroupByID(DWORD dwGroupID)
{
	SetCurGroupByIndex(GetGroupIndexByID(dwGroupID));
}
void CTWenChartCtrl::SetCurGroupByIndex(DWORD dwGroupIndex)
{
	if(CurrentGroup) CurrentGroup->Visible = false;
	CurrentGroup = GetGroupByIndex(dwGroupIndex);
	if(CurrentGroup)
	{
		CurrentGroup->Visible = true;
	}
	Redraw(true);
}
//
void CTWenChartCtrl::RebuildChart(CDC* pDC)
{
	CRect rcTitle, rcLegend, rcGrid, rcAxisX, rcAxisY;
	if(ChartImage.m_hObject) ChartImage.DeleteObject();

	CRect rc;
	GetClientRect(rc);
	if(pDC == NULL) pDC = GetDC();
	CRect rcClient(rc);

	ChartImage.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&ChartImage);

	//对&dc画图

	DrawBackground(&dc, rc, BGColor);//先画背景

	//画图表
	DrawTitle(&dc, Title, rc, rcTitle);//标题

	rc.top = rcTitle.bottom;
	//Legend
	DrawLegend(&dc, Legend, CurrentGroup, rc, rcLegend);

	rc.right = rcLegend.left;
	if(CurrentGroup)
	{
		//Grid,AxisX,AxisY
		CalculateArea(&dc, CurrentGroup, rc, rcGrid, rcAxisX, rcAxisY);

		//设置网格在ChartCtrl窗口中的区域，并设置各坐标信息
		CurrentGroup->Grid.SetGridArea(rcGrid);

		DrawAxisX(&dc, CurrentGroup->Grid, rcAxisX, rcAxisY);//画x轴
		DrawAxisY(&dc, CurrentGroup->Grid, rcAxisX, rcAxisY);//画y轴

		CBitmap bmpData;
		bmpData.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		CDC dcGrid;
		dcGrid.CreateCompatibleDC(&dc);
		CBitmap* pOldBitmapData = dcGrid.SelectObject(&bmpData);

		DrawBackground(&dcGrid, rcGrid, CurrentGroup->Grid.BGColor);//先画背景
		DrawGrid(&dcGrid, CurrentGroup->Grid, rcGrid);

		//画数据
		switch(CurrentGroup->DataType)
		{
		case CTWenChartGroup::DATATYPE_SERIE:
			{
				int count = CurrentGroup->Series.GetCount();
				for(int i=0; i<count; i++) DrawSerie(&dcGrid, CurrentGroup->Grid, CurrentGroup->GetSerieByIndex(i));
			}
			break;
		case CTWenChartGroup::DATATYPE_PIE://暂不支持
			ASSERT(false);
			break;
		}

		//画文本
		for(int i=0; i<CurrentGroup->Texts.GetCount(); i++) DrawChartText(&dcGrid, CurrentGroup->Grid, CurrentGroup->Texts.GetAt(i));

		//自动浏览轴
		DrawAutoAxis(&dcGrid, CurrentGroup->Grid);

		//画扫描线
		DrawScanLine(&dcGrid, CurrentGroup->Grid);

		dc.BitBlt(rcGrid.left, rcGrid.top, rcGrid.Width(), rcGrid.Height(), &dcGrid, rcGrid.left, rcGrid.top, SRCCOPY);
		dcGrid.SelectObject(&pOldBitmapData);
	}

	dc.SelectObject(&pOldBitmap);
}
void CTWenChartCtrl::DrawAutoAxis(CDC* pDC, const CTWenChartGrid& Grid)
{
	CString s;
	if(AutoAxisX) s.Format(_T("x:%f"), Grid.ToLogical_X(Posiotion_AutoAxisX));
	if(AutoAxisY)
	{
		if(AutoAxisX) s += _T("\n");
		CString t;
		t.Format(_T("y:%f"), Grid.ToLogical_Y(Posiotion_AutoAxisY));
		s += t;
	}
	if(!s.IsEmpty())
	{
		CFont font;
		structSelectChartFont _fontTemp(pDC, AutoAxisFont, font);
		CRect rc(0,0,0,0);
		pDC->DrawText(s, rc, DT_CALCRECT);
		int x = Posiotion_AutoAxisX+2;
		int y = Posiotion_AutoAxisY+2;
		if(x+rc.Width()>Grid.Rect.right) x = x - 2 - rc.Width();
		if(y+rc.Height()>Grid.Rect.bottom) y = y -2 - rc.Height();
		rc = CRect(x, y, x+rc.Width(), y+rc.Height());
		if(!AutoAxisFont.Transparent) DrawBackground(pDC, rc, AutoAxisFont.BkColor);
		pDC->DrawText(s, rc, DT_LEFT|DT_TOP);
	}
}
void CTWenChartCtrl::DrawBackground(CDC* pDC, const CRect& rc, COLORREF bgColor)
{
	pDC->FillSolidRect(rc, bgColor);
}
void CTWenChartCtrl::DrawTitle(CDC* pDC, const CTWenChartTitle& Title, const CRect& rc, CRect& rcTitle)
{
	rcTitle = CRect(rc.left, rc.top, rc.right, rc.top);

	if(!Title.Visible) return;

	rcTitle.bottom += Title.SpacingBorderAndText;
	CFont font;
	structSelectChartFont _fontTemp(pDC, Title.Font, font);
	CRect rcTemp;
	pDC->DrawText(Title.Text, rcTemp, DT_CALCRECT);
	rcTitle.bottom += rcTemp.Height();

	CRect rcTitleDraw(rcTitle);
	rcTitleDraw.top = rcTitleDraw.bottom - rcTemp.Height();
	pDC->DrawText(Title.Text, rcTitleDraw, DT_CENTER|DT_TOP);

	//画边框
	if(Title.Border)
	{
		rcTitle.bottom += Title.SpacingBorderAndText2;
		pDC->Draw3dRect(rcTitle, Title.Color_TopLeft, Title.Color_BottomRight);
	}
}
void  CTWenChartCtrl::DrawLegend(CDC* pDC, const CTWenChartLegend& Legend, const CTWenChartGroup* pGroup, const CRect& rc, CRect& rcLegend)
{
	//计算Legend的区域并绘制
	rcLegend = CRect(rc.right, rc.top, rc.right, rc.bottom);
	if(!Legend.Visible) return;
	rcLegend.left -= Legend.SpacingBorderAndText;
	if(!pGroup) return;

	switch(pGroup->DataType)
	{
	case CTWenChartGroup::DATATYPE_SERIE:
		{
			int count = pGroup->Series.GetCount();
			//查找最长字符串
			CString maxString;
			for(int i=0; i<count; i++)
			{
				CTWenChartSerie* pSerie = pGroup->GetSerieByIndex(i);
				if(maxString.GetLength() < pSerie->Name.GetLength()) maxString = pSerie->Name;
			}

			CFont font;
			structSelectChartFont _fontTemp(pDC, Legend.Font, font);
			CRect rcTemp;
			pDC->DrawText(maxString, rcTemp, DT_CALCRECT);
			rcLegend.left -= rcTemp.Width();//文本宽度
			CRect rcLegendText = rcLegend;
			rcLegend.left -= Legend.HSpacing;
			rcLegend.left -= Legend.ImageWidth;
			CRect rcLegendImage(rcLegend.left, 0, rcLegend.left + Legend.ImageWidth, 0);
			int lineHeight = rcTemp.Height();
			//画图例和文本
			int top_begin = rcLegend.top + (rcLegend.Height() - count*lineHeight - (count - 1)*Legend.VSpacing)/2;
			rcLegendImage.top = rcLegendText.top = top_begin;
			int vspacing = lineHeight + Legend.VSpacing;
			for(int i=0; i<count; i++)
			{
				rcLegendImage.bottom = rcLegendImage.top + lineHeight;
				CTWenChartSerie* pSerie = pGroup->GetSerieByIndex(i);
				DrawLegendImage(pDC, rcLegendImage, pSerie);
				pDC->DrawText(pSerie->Name, rcLegendText, DT_LEFT|DT_TOP);
				rcLegendImage.top += vspacing;
				rcLegendText.top += vspacing;
			}
		}
		break;
	case CTWenChartGroup::DATATYPE_PIE://暂不支持
		ASSERT(false);
		break;
	}
	//画边框
	if(Legend.Border)
	{
		rcLegend.left -= Legend.SpacingBorderAndImage;
		pDC->Draw3dRect(rcLegend, Legend.Color_TopLeft, Legend.Color_BottomRight);
	}
}
void CTWenChartCtrl::DrawLegendImage(CDC* pDC, const CRect& rc, const CTWenChartSerie* pSerie)
{
	switch(pSerie->SerieType)
	{
	case CTWenChartSerie::ST_NONE:break;
	case CTWenChartSerie::ST_LINE:
		{
			const CTWenChartLineSerie* pLine = (const CTWenChartLineSerie*)pSerie;
			//画2像素的线
			static int lineWidth = 1;
			int t = rc.top + (rc.Height() - lineWidth)/2;
			CPen pen;
			pen.CreatePen(PS_SOLID, lineWidth, pLine->Color);
			CPen* pOldPen = pDC->SelectObject(&pen);
			pDC->MoveTo(rc.left, t);
			pDC->LineTo(rc.right, t);
			pDC->SelectObject(pOldPen);
		}
		break;
	case CTWenChartSerie::ST_POINT:
		{
			const CTWenChartPointSerie* pPoint = (const CTWenChartPointSerie*)pSerie;
			CBrush brush,*pOldBrush;
			brush.CreateSolidBrush(pPoint->Color);
			pOldBrush = pDC->SelectObject(&brush);
			int x = rc.Width()/2;
			int y = rc.Height()/2;
			int r = min(x,y);//半径
			x += rc.left;
			y += rc.top;//圆心
			if(r%2==0) r--;
			pDC->Ellipse(x-r, y-r, x+r, y+r);
			pDC->SelectObject(pOldBrush); 
		}
		break;
	case CTWenChartSerie::ST_BAR:
		{
			const CTWenChartBarSerie* pBar = (const CTWenChartBarSerie*)pSerie;
			pDC->FillSolidRect(rc, pBar->Color);
		}
		break;
	case CTWenChartSerie::ST_SURFACE:
		{
			const CTWenChartSurfaceSerie* pSurface = (const CTWenChartSurfaceSerie*)pSerie;
			int w = rc.Width()/3;
			int h = rc.Height()/3;
			POINT p[5] =
			{
				{rc.left,rc.bottom},
				{rc.left+w+2,rc.bottom-h},
				{rc.left+w*2+2,rc.bottom-h},
				{rc.right,rc.bottom-2},
				{rc.right,rc.bottom}
			};
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, pSurface->Color);
			CPen* pOldPen = pDC->SelectObject(&pen);
			pDC->BeginPath();
			pDC->MoveTo(p[0]);
			for(int i=1; i<5; i++) pDC->LineTo(p[i]);
			pDC->EndPath();
			pDC->SelectObject(pOldPen);

			//pDC->SetPolyFillMode();
			CBrush brush,*pOldBrush;
			brush.CreateSolidBrush(pSurface->Color);
			pOldBrush = pDC->SelectObject(&brush);
			pDC->FillPath();

			pDC->SelectObject(pOldBrush); 
		}
		break;
	}
}
void CTWenChartCtrl::CalculateArea(CDC* pDC, const CTWenChartGroup* pGroup, const CRect& rc, CRect& rcGrid, CRect& rcAxisX, CRect& rcAxisY)
//最终的rcAxisX, rcAxisY包含坐标轴区域的相交部分
{
	rcAxisX = CRect(rc.left, -1, rc.right, -1);
	rcAxisY = CRect(-1, rc.top, -1, rc.bottom);
	rcGrid = rc;
	if(!pGroup) return;
	
	const CTWenChartGrid& Grid = pGroup->Grid;
	CFont font;
	structSelectChartFont _fontTemp(pDC, Grid.AxisTextFont, font);

	CString testText(_T("MM"));
	CRect testRc(0,0,0,0);
	pDC->DrawText(testText, testRc, DT_CALCRECT);

	//计算x轴
	if(Grid.AxisX.Visible)
	{
		int h = 0;
		CRect rcXText(0,0,0,0);//x轴文本高度和宽度
		pDC->DrawText(Grid.GetXDrawCation(), rcXText, DT_CALCRECT);
		h += Grid.AxisX.SpacingBorderAndText + rcXText.Height();

		h += Grid.AxisX.Spacing + testRc.Height() + Grid.AxisX.Spacing + Grid.AxisX.LineLength;

		switch(Grid.AxisX.Position)
		{
		case CTWenChartGrid::POSITION_TOP:
			rcAxisX.top = rc.top;
			rcAxisX.bottom = rcAxisX.top + h;
			rcGrid.top = rcAxisX.bottom;
			break;
		case CTWenChartGrid::POSITION_BOTTOM:
			rcAxisX.bottom = rc.bottom;
			rcAxisX.top = rcAxisX.bottom - h;
			rcGrid .bottom = rcAxisX.top;
			break;
		}
	}

	//计算y轴
	if(Grid.AxisY.Visible)
	{
		int w = 0;
		CString caption = Grid.GetYDrawCation();
		if(!caption.IsEmpty())
		{
			w += Grid.AxisY.SpacingBorderAndText + testRc.Width();
		}

		//获取最长数值文本
		CString maxLenghtValue = _T("");
		CString textTemp;
		for(DWORD i=0; i<=Grid.AxisY.GetSteps(); i++)
		{
			textTemp = Grid.AxisY.GetStepScaleValueString(i);
			if(textTemp.GetLength() > maxLenghtValue.GetLength()) maxLenghtValue = textTemp;
		}

		CRect rcLenghtValue;
		pDC->DrawText(maxLenghtValue, rcLenghtValue, DT_CALCRECT);
		w += Grid.AxisY.Spacing + rcLenghtValue.Width() + Grid.AxisY.Spacing + Grid.AxisY.LineLength;

		switch(Grid.AxisY.Position)
		{
		case CTWenChartGrid::POSITION_LEFT:
			rcAxisY.left = rc.left;
			rcAxisY.right = rcAxisY.left + w;
			rcGrid.left = rcAxisY.right;
			break;
		case CTWenChartGrid::POSITION_RIGHT:
			rcAxisY.right = rc.right;
			rcAxisY.left = rcAxisY.right - w;
			rcGrid.right = rcAxisY.left;
			break;
		}
	}

	//设置rcGrid
	switch(Grid.AxisX.Position)
	{
	case CTWenChartGrid::POSITION_TOP:rcGrid.bottom -= pGroup->Grid.SpacingY;break;
	case CTWenChartGrid::POSITION_BOTTOM:rcGrid.top += pGroup->Grid.SpacingY;break;
	}
	switch(Grid.AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT:rcGrid.right -= pGroup->Grid.SpacingX;break;
	case CTWenChartGrid::POSITION_RIGHT:rcGrid.left += pGroup->Grid.SpacingX;break;
	}
}
void CTWenChartCtrl::DrawAxisX(CDC* pDC, const CTWenChartGrid& Grid, const CRect& rcAxisX, const CRect& rcAxisY)
{
	CFont font;
	structSelectChartFont _fontTemp(pDC, Grid.AxisTextFont, font);

	CString testText(_T("MM"));
	CRect testRc(0,0,0,0);
	pDC->DrawText(testText, testRc, DT_CALCRECT);

	CRect rcText(rcAxisX);//文本绘画区域
	CRect rcStepsString(rcAxisX);//刻度值绘画区域
	CRect rcSteps(rcAxisX);//刻度线绘画区域

	int hXText = rcAxisX.Height() - (Grid.AxisX.SpacingBorderAndText + Grid.AxisX.Spacing + testRc.Height() + Grid.AxisX.Spacing + Grid.AxisX.LineLength);

	switch(Grid.AxisX.Position)
	{
	case CTWenChartGrid::POSITION_TOP:
		//坐标轴文字（如果不为空）在坐标轴上方
		rcText.top += Grid.AxisX.SpacingBorderAndText;
		rcText.bottom = rcText.top + hXText;

		rcStepsString.bottom -= (Grid.AxisX.Spacing + Grid.AxisX.LineLength);
		rcStepsString.top = rcStepsString.bottom - testRc.Height();

		rcSteps.top = rcSteps.bottom - Grid.AxisX.LineLength;
		break;
	case CTWenChartGrid::POSITION_BOTTOM:
		//坐标轴文字（如果不为空）在坐标轴下方
		rcText.bottom -= Grid.AxisX.SpacingBorderAndText;
		rcText.top = rcText.bottom - hXText;

		rcStepsString.top += (Grid.AxisX.Spacing + Grid.AxisX.LineLength);
		rcStepsString.bottom = rcStepsString.top + testRc.Height();

		rcSteps.bottom = rcSteps.top + Grid.AxisX.LineLength;
		break;
	}

	pDC->DrawText(Grid.GetXDrawCation(), rcText, DT_CENTER|DT_TOP);//画文本

	//画刻度值和刻度线
	int offset = testRc.Width()/2;
	double *xV = NULL;
	DWORD xSteps = 0;
	Grid.AxisX.GetStepsValue(&xV, &xSteps);
	Grid.ToPixel_X(xSteps+1, xV);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CTWenChartColor::Colors[CTWenChartColor::COLOR_BLACK]);
	CPen* pOldPen = pDC->SelectObject(&pen);//画轴线和刻度线的画笔

	POINT p[3];

	switch(Grid.AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT://从左向右画
		for(DWORD i=0; i<=xSteps; i++)
		{
			rcStepsString.left = (long)(xV[i] - offset);
			pDC->DrawText(Grid.AxisX.GetStepScaleValueString(i), rcStepsString, DT_LEFT|DT_TOP);
		}
		//找出箭头顶点
		switch (Grid.AxisX.Position)
		{
		case CTWenChartGrid::POSITION_TOP:p[1] = CPoint(rcAxisX.right, rcAxisX.bottom);break;
		case CTWenChartGrid::POSITION_BOTTOM:p[1] = CPoint(rcAxisX.right, rcAxisX.top);break;
		}
		p[0].x = p[2].x = p[1].x - Arrow_Width;
		break;
	case CTWenChartGrid::POSITION_RIGHT://从右向左画
		for(DWORD i=0; i<=xSteps; i++)
		{
			rcStepsString.right = (long)(xV[i] + offset);
			pDC->DrawText(Grid.AxisX.GetStepScaleValueString(i), rcStepsString, DT_RIGHT|DT_TOP);
		}
		//找出箭头顶点
		switch (Grid.AxisX.Position)
		{
		case CTWenChartGrid::POSITION_TOP:p[1] = CPoint(rcAxisX.left, rcAxisX.bottom);break;
		case CTWenChartGrid::POSITION_BOTTOM:p[1] = CPoint(rcAxisX.left, rcAxisX.top);break;
		}
		p[0].x = p[2].x = p[1].x + Arrow_Width;
		break;
	}

	//画箭头
	p[0].y = p[1].y - Arrow_Height;
	p[2].y = p[1].y + Arrow_Height;
	pDC->MoveTo(p[0]);
	pDC->LineTo(p[1]);
	pDC->LineTo(p[2]);

	//画轴线
	int y = -1;
	switch (Grid.AxisX.Position)
	{
	case CTWenChartGrid::POSITION_TOP:y = rcAxisX.bottom;break;
	case CTWenChartGrid::POSITION_BOTTOM:y = rcAxisX.top;break;
	}
	int l = rcAxisX.left, r = rcAxisX.right;
	switch(Grid.AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT:l = rcAxisY.right;break;
	case CTWenChartGrid::POSITION_RIGHT:r = rcAxisY.left;break;
	}
	pDC->MoveTo(l, y);
	pDC->LineTo(r, y);

	//画刻度线
	for(DWORD i=0; i<=xSteps; i++)
	{
		pDC->MoveTo((int)xV[i], rcSteps.top);
		pDC->LineTo((int)xV[i], rcSteps.bottom);
	}

	pDC->SelectObject(&pOldPen);

	delete []xV;
}
void CTWenChartCtrl::DrawAxisY(CDC* pDC, const CTWenChartGrid& Grid, const CRect& rcAxisX, const CRect& rcAxisY)
{
	CFont font;
	structSelectChartFont _fontTemp(pDC, Grid.AxisTextFont, font);

	CString testText(_T("MM"));
	CRect testRc(0,0,0,0);
	pDC->DrawText(testText, testRc, DT_CALCRECT);

	CRect rcText(rcAxisY);//文本绘画区域
	CRect rcStepsString(rcAxisY);//刻度值绘画区域
	CRect rcSteps(rcAxisY);//刻度线绘画区域

	int wYStepString = rcAxisY.Width() - (Grid.AxisX.SpacingBorderAndText + Grid.AxisX.Spacing + testRc.Width() + Grid.AxisX.Spacing + Grid.AxisX.LineLength);

	switch(Grid.AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT:
		//坐标轴文字（如果不为空）在坐标轴左方
		rcText.left = rcAxisY.left + Grid.AxisY.SpacingBorderAndText;
		rcText.right = rcText.left + testRc.Width();

		rcStepsString.left = rcText.right + Grid.AxisY.Spacing;
		rcStepsString.right = rcStepsString.left + wYStepString;

		rcSteps.left = rcStepsString.right + Grid.AxisY.Spacing;
		break;
	case CTWenChartGrid::POSITION_RIGHT:
		//坐标轴文字（如果不为空）在坐标轴右方
		rcText.right = rcAxisY.right - Grid.AxisY.SpacingBorderAndText;
		rcText.left = rcText.right - testRc.Width();

		rcStepsString.right = rcText.left - Grid.AxisY.Spacing;
		rcStepsString.left = rcStepsString.right - wYStepString;

		rcSteps.right = rcStepsString.left - Grid.AxisY.Spacing;
	}

	//画文本
	CString YText;
	CString caption = Grid.GetYDrawCation();
	if(!caption.IsEmpty())
	{
#ifdef UNICODE
		YText = caption.GetAt(0);
		for(int i=1; i<caption.GetLength(); i++) YText = YText +_T("\n") + caption.GetAt(i);
#else
		bool bFirst = true;
		for(int i=0; i<caption.GetLength(); i++)
		{
			if(!bFirst) YText += '\n';
			char c = caption.GetAt(i);
			YText += c;
			if(c & 0x80){i++;YText += caption.GetAt(i);}
			bFirst = false;
		}
#endif
	}
	if(!YText.IsEmpty())
	{
		CRect rcTemp(0,0,0,0);
		pDC->DrawText(YText, rcTemp, DT_CALCRECT);
		rcText.top += (rcAxisY.Height() - rcTemp.Height())/2;
		rcText.right = rcText.left + rcTemp.Width();
		pDC->DrawText(YText, rcText, DT_CENTER|DT_TOP);
	}

	//画刻度值和刻度线
	int offset = testRc.Height()/2;
	double *yV = NULL;
	DWORD ySteps = 0;
	Grid.AxisY.GetStepsValue(&yV, &ySteps);
	Grid.ToPixel_Y(ySteps+1, yV);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CTWenChartColor::Colors[CTWenChartColor::COLOR_BLACK]);
	CPen* pOldPen = pDC->SelectObject(&pen);//画轴线和刻度线的画笔

	POINT p[3];

	DWORD xD = 0;
	switch(Grid.AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT:xD = DT_RIGHT;break;
	case CTWenChartGrid::POSITION_RIGHT:xD = DT_LEFT;break;
	}
	switch(Grid.AxisX.Position)
	{
	case CTWenChartGrid::POSITION_TOP://从上向下画
		for(DWORD i=0; i<=ySteps; i++)
		{
			rcStepsString.top = (long)(yV[i] - offset);
			pDC->DrawText(Grid.AxisY.GetStepScaleValueString(i), rcStepsString, DT_TOP|xD);
		}
		//找出箭头顶点
		switch (Grid.AxisY.Position)
		{
		case CTWenChartGrid::POSITION_LEFT:p[1] = CPoint(rcAxisY.right, rcAxisY.bottom);break;
		case CTWenChartGrid::POSITION_RIGHT:p[1] = CPoint(rcAxisY.left, rcAxisY.bottom);break;
		}
		p[0].y = p[2].y = p[1].y - Arrow_Width;
		break;
	case CTWenChartGrid::POSITION_BOTTOM://从下向上画
		for(DWORD i=0; i<=ySteps; i++)
		{
			rcStepsString.bottom = (long)(yV[i] + offset);
			pDC->DrawText(Grid.AxisY.GetStepScaleValueString(i), rcStepsString, DT_SINGLELINE|DT_BOTTOM|xD);
		}
		//找出箭头顶点
		switch (Grid.AxisY.Position)
		{
		case CTWenChartGrid::POSITION_LEFT:p[1] = CPoint(rcAxisY.right, rcAxisY.top);break;
		case CTWenChartGrid::POSITION_RIGHT:p[1] = CPoint(rcAxisY.left, rcAxisY.top);break;
		}
		p[0].y = p[2].y = p[1].y + Arrow_Width;
		break;
	}

	//画箭头
	p[0].x = p[1].x - Arrow_Height;
	p[2].x = p[1].x + Arrow_Height;
	pDC->MoveTo(p[0]);
	pDC->LineTo(p[1]);
	pDC->LineTo(p[2]);

	//画轴线
	int x = -1;
	switch (Grid.AxisY.Position)
	{
	case CTWenChartGrid::POSITION_LEFT:x = rcAxisY.right;break;
	case CTWenChartGrid::POSITION_RIGHT:x = rcAxisY.left;break;
	}
	int t = rcAxisY.top, b = rcAxisY.bottom;
	switch(Grid.AxisX.Position)
	{
	case CTWenChartGrid::POSITION_TOP:t = rcAxisX.bottom;break;
	case CTWenChartGrid::POSITION_BOTTOM:b = rcAxisX.top;break;
	}
	pDC->MoveTo(x, t);
	pDC->LineTo(x, b);

	//画刻度线
	for(DWORD i=0; i<=ySteps; i++)
	{
		pDC->MoveTo(rcSteps.left, (int)yV[i]);
		pDC->LineTo(rcSteps.right, (int)yV[i]);
	}

	pDC->SelectObject(&pOldPen);

	delete []yV;
}
void CTWenChartCtrl::DrawGrid(CDC* pDC, const CTWenChartGrid& Grid, const CRect& rcGrid)
{
	if(!Grid.Visible) return;

	DWORD xSteps = 0;
	DWORD ySteps = 0;
	double *xV = NULL;
	double *yV = NULL;
	Grid.AxisX.GetStepsValue(&xV, &xSteps);
	Grid.AxisY.GetStepsValue(&yV, &ySteps);
	Grid.ToPixel_X(xSteps+1, xV);
	Grid.ToPixel_Y(ySteps+1, yV);
	CPen pen;
	CPen* pPen = Grid.Pen.GetPen(&pen);
	CPen* pOldPen = pDC->SelectObject(pPen);
	for(DWORD i=0; i<=xSteps; i++)
	{
		//pDC->MoveTo((int)xV[i], (int)yV[0]);
		//pDC->LineTo((int)xV[i], (int)yV[ySteps]);
		pDC->MoveTo((int)xV[i], Grid.Rect.top);
		pDC->LineTo((int)xV[i], Grid.Rect.bottom);
	}
	for(DWORD i=0; i<=ySteps; i++)
	{
		//pDC->MoveTo((int)xV[0], (int)yV[i]);
		//pDC->LineTo((int)xV[xSteps], (int)yV[i]);
		pDC->MoveTo(Grid.Rect.left, (int)yV[i]);
		pDC->LineTo(Grid.Rect.right, (int)yV[i]);
	}
	delete []xV;
	delete []yV;
	pDC->SelectObject(pOldPen);
}
void CTWenChartCtrl::DrawSerie(CDC* pDC,const CTWenChartGrid& Grid, const CTWenChartSerie* pSerie)
{
	if(!pSerie) return;
	const CTWenChartPoints& pts = pSerie->Data;
	DWORD from, to;
	if(!pts.GetIndexRange(from, to)) return;
	CTWenChartFont Font = pts.LabelFont;
	Font.Color = pSerie->Color;
	switch(pSerie->SerieType)
	{
	case CTWenChartSerie::ST_NONE:break;
	case CTWenChartSerie::ST_LINE:
		{
			//应当首先计算from点是否在坐标轴以外，如果是，将起始点设为from和from＋1点和y轴或者x轴的交点！！！！！！！！！！！！！
			//同样，对于其他超出范围的点，也可能导致在范围部分绘画丢失
			//解决办法就是建立一个在范围内的足够大的空间，把所有点画全，然后拷贝相关的部分
			CTWenChartLineSerie* pLine = (CTWenChartLineSerie*)pSerie;
			CPen pen;
			pLine->Pen.Arguments.Color = pSerie->Color;
			CPen* pOldPen = pDC->SelectObject(pLine->Pen.GetPen(&pen));
			int x = Grid.ToPixel_X(pts.Points[from].x);
			int y = Grid.ToPixel_Y(pts.Points[from].y);
			DrawPointLabel(pDC, Grid, x, y, Font, pts.Points[from].label);
			pDC->MoveTo(x, y);
			for(DWORD i=from+1; i<=to; i++)
			{
				x = Grid.ToPixel_X(pts.Points[i].x);
				y = Grid.ToPixel_Y(pts.Points[i].y);
				DrawPointLabel(pDC, Grid, x, y, Font, pts.Points[i].label);
				pDC->LineTo(x, y);
			}
			pDC->SelectObject(pOldPen);
		}
		break;
	case CTWenChartSerie::ST_POINT:
		{
			const CTWenChartPointSerie* pPoint = (const CTWenChartPointSerie*)pSerie;
			CBrush brush,*pOldBrush;
			brush.CreateSolidBrush(pPoint->Color);
			pOldBrush = pDC->SelectObject(&brush);
			int r = pPoint->Radius;
			if(r%2==0) r--;
			if(r<1) r = 1;
			int x, y;
			for(DWORD i=from; i<=to; i++)
			{
				x = Grid.ToPixel_X(pts.Points[i].x);
				y = Grid.ToPixel_Y(pts.Points[i].y);
				DrawPointLabel(pDC, Grid, x, y, Font, pts.Points[i].label);
				pDC->Ellipse(x-r, y-r, x+r, y+r);
			}			
			pDC->SelectObject(pOldBrush); 
		}
		break;
	case CTWenChartSerie::ST_BAR:
		{
			const CTWenChartBarSerie* pBar = (const CTWenChartBarSerie*)pSerie;
			int x, y;
			for(DWORD i=from; i<=to; i++)
			{
				x = Grid.ToPixel_X(pts.Points[i].x);
				y = Grid.ToPixel_Y(pts.Points[i].y);
				DrawPointLabel(pDC, Grid, x, y, Font, pts.Points[i].label);
				switch(Grid.AxisX.Position)
				{
				case CTWenChartGrid::POSITION_TOP:
					pDC->FillSolidRect(x-pBar->Width/2, Grid.Rect.top, pBar->Width, y-Grid.Rect.top, pBar->Color);
					break;
				case CTWenChartGrid::POSITION_BOTTOM:
					pDC->FillSolidRect(x-pBar->Width/2, y, pBar->Width, Grid.Rect.bottom-y+1, pBar->Color);
					break;
				}
			}
		}
		break;
	case CTWenChartSerie::ST_SURFACE:
		{
			//应当首先计算from点是否在坐标轴以外，如果是，将起始点设为from和from＋1点和y轴或者x轴的交点！！！！！！！！！！！！！
			//同样，对于其他超出范围的点，也可能导致在范围部分绘画丢失
			//解决办法就是建立一个在范围内的足够大的空间，把所有点画全，然后拷贝相关的部分
			const CTWenChartSurfaceSerie* pSurface = (const CTWenChartSurfaceSerie*)pSerie;
			
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, pSurface->Color);
			CPen* pOldPen = pDC->SelectObject(&pen);
			//
			DWORD count = to - from + 1;
			POINT* ppt = new POINT[count+2];
			ppt[0].x = Grid.ToPixel_X(pts.Points[from].x);
			ppt[0].y = Grid.ToPixel_Y(Grid.AxisY.GetStepValue(0));
			for(DWORD i=from; i<=to; i++)
			{
				int index = i+1-from;
				ppt[index].x = Grid.ToPixel_X(pts.Points[i].x);
				ppt[index].y = Grid.ToPixel_Y(pts.Points[i].y);
				DrawPointLabel(pDC, Grid, ppt[index].x, ppt[index].y, Font, pts.Points[i].label);
			}
			ppt[count+1].x = ppt[count].x;
			ppt[count+1].y = ppt[0].y;
			//
			pDC->BeginPath();
			pDC->Polyline(ppt, count+2);
			//pDC->PolyBezierTo(ppt, count+2);
			//pDC->PolyBezier(ppt, count+2);
			pDC->EndPath();
			pDC->SelectObject(pOldPen);

			delete []ppt;

			//pDC->SetPolyFillMode(WINDING);
			CBrush brush,*pOldBrush;
			brush.CreateSolidBrush(pSurface->Color);
			pOldBrush = pDC->SelectObject(&brush);
			pDC->FillPath();

			pDC->SelectObject(pOldBrush); 
		}
		break;
	}
}
void CTWenChartCtrl::DrawPointLabel(CDC* pDC, const CTWenChartGrid& Grid, int x, int y, const CTWenChartFont& Font, const CTWenChartPoints::CTWenChartPointLabel* pLabel)
{
	if(!pLabel) return;
	CFont font;
	structSelectChartFont _SelectFont(pDC, Font, font);
	CRect rcText(0,0,0,0);
	pDC->DrawText(pLabel->Text, rcText, DT_CALCRECT);
	pDC->DrawText(pLabel->Text, CRect(x-rcText.Width()/2, y-rcText.Height()/2, x+rcText.Width()/2, y+rcText.Height()/2), DT_CENTER|DT_TOP);
}
void CTWenChartCtrl::DrawChartText(CDC* pDC, const CTWenChartGrid& Grid, const CTWenChartText* pText)
{
	if(!pText) return;
	CFont font;
	structSelectChartFont _SelectFont(pDC, pText->Font, font);
	CRect rcText(0,0,0,0);
	pDC->DrawText(pText->Text, rcText, DT_CALCRECT);
	int x = Grid.ToPixel_X(pText->x);
	int y = Grid.ToPixel_Y(pText->y);
	pDC->DrawText(pText->Text, CRect(x-rcText.Width()/2, y-rcText.Height()/2, x+rcText.Width()/2, y+rcText.Height()/2), DT_CENTER|DT_TOP);
}
void CTWenChartCtrl::DrawScanLine(CDC* pDC,const CTWenChartGrid& Grid)
{
	if(Grid.AxisX.ScanLine.Visible)
	{
		//x轴的扫描线
		CPen pen;
		CPen* pOldPen = pDC->SelectObject(Grid.AxisX.ScanLine.Pen.GetPen(&pen));
		int x = Grid.ToPixel_X(Grid.AxisX.ScanValue);
		pDC->MoveTo(x, Grid.Rect.top);
		pDC->LineTo(x, Grid.Rect.bottom);
		pDC->SelectObject(pOldPen);
	}
	if(Grid.AxisY.ScanLine.Visible)
	{
		//y轴的扫描线
		CPen pen;
		CPen* pOldPen = pDC->SelectObject(Grid.AxisY.ScanLine.Pen.GetPen(&pen));
		int y = Grid.ToPixel_Y(Grid.AxisY.ScanValue);
		pDC->MoveTo(Grid.Rect.left, y);
		pDC->LineTo(Grid.Rect.right, y);
		pDC->SelectObject(pOldPen);
	}
}
//
void CTWenChartCtrl::TrimZero(CString& str)
{
	if(str.Find('.') < 0) return;
	while(true)
	{
		if(str.GetAt(str.GetLength() - 1) == '0')
		{
			str.Delete(str.GetLength() - 1);
			if(str.GetAt(str.GetLength() - 1) == '.')
			{
				str.Delete(str.GetLength() - 1);
				break;
			}
		}
		else break;
	}
}
//
bool CTWenChartCtrl::ChartFont(CTWenChartFont& font, CWnd* parent)
{
	CHARFORMAT cf;
	cf.bCharSet = DEFAULT_CHARSET;
	cf.bPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.crTextColor = font.Color;
	cf.dwEffects = NULL;
	if(font.Arguments.Bold) cf.dwEffects |= CFE_BOLD;
	if(font.Arguments.Italic) cf.dwEffects |= CFE_ITALIC;
	if(font.Arguments.StrikeOut) cf.dwEffects |= CFE_STRIKEOUT;
	if(font.Arguments.Underline) cf.dwEffects |= CFE_UNDERLINE;
	cf.dwMask = CFM_BOLD|CFM_CHARSET|CFM_COLOR|CFM_FACE|CFM_ITALIC|CFM_OFFSET|CFM_SIZE|CFM_STRIKEOUT|CFM_UNDERLINE;
	_tcscpy(cf.szFaceName, font.Arguments.Name);
	cf.yHeight = font.Arguments.Size*20;
	cf.yOffset = font.Arguments.YOffset;
	CFontDialog dlg(cf, 1, NULL, parent);
	if(dlg.DoModal() != IDOK) return false;
	dlg.GetCharFormat(cf);
	font.Color = cf.crTextColor;
	font.Arguments.Bold = (cf.dwEffects & CFE_BOLD)?true:false;
	font.Arguments.Italic = (cf.dwEffects & CFE_ITALIC)?true:false;
	font.Arguments.StrikeOut = (cf.dwEffects & CFE_STRIKEOUT)?true:false;
	font.Arguments.Underline = (cf.dwEffects & CFE_UNDERLINE)?true:false;
	font.Arguments.Name = cf.szFaceName;
	font.Arguments.Size = cf.yHeight/20;
	font.Arguments.YOffset = cf.yOffset;
	return true;
}
bool CTWenChartCtrl::ChartColor(COLORREF& color)
{
	CColorDialog dlg(color, 0, this);
	if(dlg.DoModal() != IDOK) return false;
	color = dlg.GetColor();
	return true;
}
bool CTWenChartCtrl::ChartAxisRange(CTWenChartGrid::CTWenChartAxisBase& axis)
{
	CString rangeType, valueType, sMin, sMax, sStep, sScale, sBase10;
	rangeType.Format(_T("%u|所有范围:%u,指定大小值范围:%u,指定大小值的固定长度范围:%u"), axis.AxisType & 0x0000FFFF, CTWenChartGrid::AXISTYPE_ALL,CTWenChartGrid:: AXISTYPE_FIXED, CTWenChartGrid::AXISTYPE_LENGTH);
	valueType.Format(_T("%u|普通数字:%u,日期:%u,时间:%u,日期时间:%u"), axis.AxisType & 0xFFFF0000,
		CTWenChartGrid::AXISTYPE_NUMBER, CTWenChartGrid:: AXISTYPE_DATE, CTWenChartGrid::AXISTYPE_TIME, CTWenChartGrid::AXISTYPE_DATETIME);
	sMin.Format(_T("%f"), axis.Min);
	sMax.Format(_T("%f"), axis.Max);
	sStep.Format(_T("%f"), axis.Step);
	sScale.Format(_T("%f"), axis.Scale);
	sBase10.Format(_T("%s|缩放值是10的次方"), axis.ScaleBase10?_T("1"):_T("0"));

	CTemplateDlg dlg(this);
	dlg.SetCtrlInfo(8,
		_T("范围类型"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,rangeType,
		_T("数值类型"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,valueType,
		_T("文本"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Text|CTemplateDlg::VT_NoZero,axis.Text,
		_T("Min )"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Double, sMin,
		_T("Max )"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Double, sMax,
		_T("步进 )"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Double, sStep,
		_T("缩放 )"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Double, sScale,
		_T(""),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,sBase10
		);
	if(dlg.DoModal() != IDOK) return false;
	CString v[8];
	CStringExt::Split(dlg.m_strValue, dlg.m_strSep, 8, v);
	axis.AxisType = ((DWORD)_tstol(v[0])) | ((DWORD)_tstol(v[1]));
	axis.Text = v[2];
	axis.Min = _tstof(v[3]);
	axis.Max = _tstof(v[4]);
	axis.Step = _tstof(v[5]);
	axis.Scale = _tstof(v[6]);
	axis.ScaleBase10 = _tstol(v[7]) ? true : false;
	return true;
}
bool CTWenChartCtrl::GetSaveFile(CFile& file, LPCTSTR DefaultFileTitle, LPCTSTR Caption)
{
	CFileDialog mFileDlg(FALSE,_T("csv"),DefaultFileTitle,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("csv文件|*.csv||"),this);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000) mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle= Caption;
	if(mFileDlg.DoModal() != IDOK ) return false;

	if(file.Open(mFileDlg.GetPathName(),CFile::modeCreate | CFile::modeWrite)) return true;
	MessageBox(_T("打开文件失败"));
	return false;
}
void CTWenChartCtrl::SaveSerie(CFile& file, CTWenChartSerie* pSerie)
{
	CString strSep = _T(",");
	CString line;
	//首先保存曲线名称
	{
		line.Format(_T("%s%s%s\n"), pSerie->Name, strSep, strSep);
		TWen_USES_CONVERSION;
		LPCSTR s=TWen_CT2A(line);
		file.Write(s,strlen(s));
	}
	for(unsigned j=0; j<pSerie->Data.GetCount(); j++)
	{
		CTWenChartPoints::CTWenChartPoint& p = pSerie->Data.Points[j];
		line.Format(_T("%.5f%s%.5f%s%s\n"), p.x, strSep, p.y, strSep, p.label?(LPCTSTR)p.label->Text:_T(""));
		TWen_USES_CONVERSION;
		LPCSTR s=TWen_CT2A(line);
		file.Write(s,strlen(s));
	}
	{
		line.Format(_T(",,\n,,\n,,\n"));
		TWen_USES_CONVERSION;
		LPCSTR s=TWen_CT2A(line);
		file.Write(s,strlen(s));
	}
}
bool CTWenChartCtrl::GetLoadFile(CFile& file, CString& fileTitle, LPCTSTR Caption)
{
	CFileDialog mFileDlg(TRUE,_T("csv"),NULL,OFN_FILEMUSTEXIST|OFN_EXPLORER,_T("csv文件|*.csv||"),this);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000) mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle= Caption;
	if(mFileDlg.DoModal() != IDOK ) return false;

	fileTitle = mFileDlg.GetFileTitle();
	if(file.Open(mFileDlg.GetPathName(), CFile::modeRead)) return true;

	MessageBox(_T("打开文件失败"));
	return false;
}
void CTWenChartCtrl::LoadSerie(CFile& file, CTWenChartSerie* pSerie)
{
	if(!Read(file, pSerie->Name, ',')) return;
	CString x, y, label;
	if(!Read(file, y, ',')) return;
	if(!Read(file, label, '\n')) return;

	pSerie->Data.CurrentIndex = 0;

	DWORD emps = 0;

	while(true)
	{
		if(!Read(file, x, ',')) return;
		if(!Read(file, y, ',')) return;
		if(!Read(file, label, '\n')) return;
		if(x.IsEmpty() && y.IsEmpty() && label.IsEmpty())
		{
			emps++;
			if(emps >= 3) break;
			else continue;
		}
		CTWenChartPoints::CTWenChartPoint* p = pSerie->Data.AddPoint(_tstof(x), _tstof(y));
		if(!label.IsEmpty())
		{
			p->label = new CTWenChartPoints::CTWenChartPointLabel;
			p->label->Text = label;
		}
	}
}
bool CTWenChartCtrl::Read(CFile& file,CString& content,char token)
{
	char c;
	content = _T("");
	char maxline[102400];
	int count = 0;
	while(true)
	{
		UINT uc = file.Read(&c, sizeof(char));
		if(uc < sizeof(char)) return false;
		if(token == c) break;
		else maxline[count++]=c;
	}
	if(count == 0) return true;
	maxline[count]=0;
	TWen_USES_CONVERSION;
	content = TWen_CA2T(maxline);
	return true;
}