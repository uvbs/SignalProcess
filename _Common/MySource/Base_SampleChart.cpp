#include "stdafx.h"
#include "Base_SampleChart.h"
#include "Base_AnalysisSample.h"
#include <deque>
#include "PeakSeak.h"
#include "CStringConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
CParamCommon::CParamCommon(void)
{
}


CParamCommon::~CParamCommon(void)
{
}

const CString& CParamCommon::ThresholdString(double dblValue)
{
	static CString ret;
	ret.Format(_T("%.3f"), dblValue);
	return ret;
}
const CString& CParamCommon::MinHalfPeakWidthString(double dblValue)
{
	static CString ret;
	ret.Format(_T("%.3f"), dblValue);
	return ret;
}
const CString& CParamCommon::MinPeakAreaString(double dblValue)
{
	static CString ret;
	ret.Format(_T("%.2f"), dblValue);
	return ret;
}
const CString& CParamCommon::MinPeakHeightString(double dblValue)
{
	static CString ret;
	ret.Format(_T("%.2f"), dblValue);
	return ret;
}

const CString& CParamCommon::ConcentrationString(double dblValue)
{
	static CString ret;
	ret.Format(_T("%.1f"), dblValue);
	return ret;
}
//*/

CString CParamCommon::TimeString(double dblValue)
{
	CString ret;
	ret.Format(_T("%.3f"), dblValue);
	return ret;
}



IMPLEMENT_DYNAMIC(CBase_SampleChart, CSuperChartCtrl)

//ON_COMMAND：CSuperChartCtrl附加的命令标识从50000开始
BEGIN_MESSAGE_MAP(CBase_SampleChart, CSuperChartCtrl)
	//{{AFX_MSG_MAP(CBase_SampleChart)
	ON_COMMAND(ID_MENU_EXPORT_DATA, &CBase_SampleChart::OnExportData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CBase_SampleChart::OnExportData()
{
	CString file = _T("C:/Points.csv");

	CFileDialog mFileDlg(FALSE,_T("csv"),_T("Points"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("csv文件|*.csv||"),this);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000) mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle= _T("保存到文件");
	if(mFileDlg.DoModal() !=IDOK ) return;
	file = mFileDlg.GetPathName();

	CString strSep = _T(",");
	CFile f;
	BOOL ret = f.Open(file,CFile::modeCreate | CFile::modeReadWrite);
	if(!ret)
	{
		MessageBox(_T("打开文件失败，可能是文件为只读属性或被占用"));
		return;
	}
	CString line(_T("时间"));
	//
	CArray<CChartSerie*, CChartSerie*> series;
	series.Add(m_pcCurveMain);
	unsigned maxCount = 0;
	CChartSerie* pSerieOfMaxCount = NULL;
	for(int i=0; i<=series.GetUpperBound(); ++i)
	{
		CChartSerie* pS = series.GetAt(i);
		unsigned c = pS->GetPointsCount();
		if(c > maxCount)
		{
			maxCount = c;
			pSerieOfMaxCount = pS;
		}
		line = line + strSep + pS->GetName().c_str();
	}
	//标题列
	{
		TWen_USES_CONVERSION;
		LPCSTR s=TWen_CT2A(line + _T("\r\n"));
		f.Write(s,strlen(s));
	}
	for(size_t i=0; i<maxCount; i++)
	{
		line.Format(_T("%.15f"), pSerieOfMaxCount->m_vPoints.m_pPoints[i].x);
		for(int j=0; j<=series.GetUpperBound(); ++j)
		{
			CChartSerie* pS = GetSerieByIndex(j);
			unsigned c = pS->GetPointsCount();
			CString v;
			if(c > i) v.Format(_T("%.15f"), pS->m_vPoints.m_pPoints[i].y);
			line = line + strSep + v;
		}
		{
			TWen_USES_CONVERSION;
			LPCSTR s=TWen_CT2A(line + _T("\r\n"));
			f.Write(s,strlen(s));
		}
	}
	f.Close();
	MessageBox(_T("数据已保存到文件[")+file+_T("]"));
}
CBase_SampleChart::enumMenu CBase_SampleChart::OnPrepareMenu(CMenu& defaultMenu, CMenu& customMenu)
{
	customMenu.AppendMenu(MF_STRING, ID_MENU_AXIS_RANGE, _T("设置坐标范围"));

	{
		CMenu m;
		m.CreatePopupMenu();
		m.AppendMenu(MF_STRING, ID_MENU_GRID_COLOR, _T("设置网格颜色"));
		m.AppendMenu(MF_STRING, ID_MENU_BACKGROUND_COLOR, _T("设置背景颜色"));
		m.AppendMenu(MF_STRING, ID_MENU_SERIE_COLOR, _T("设置曲线颜色"));
		m.AppendMenu(MF_STRING, ID_MENU_BORDER_COLOR, _T("设置边界颜色"));

		customMenu.AppendMenu(MF_POPUP, (UINT_PTR)m.m_hMenu, _T("颜色设置"));
	}

	{
		CMenu m;
		m.CreatePopupMenu();
		m.AppendMenu(MF_STRING, ID_MENU_ADD_STRING, _T("添加文本"));
		//m.AppendMenu(MF_STRING, ID_MENU_ADD_FILLXY, _T("添加填充(指定中间点)"));
		m.AppendMenu(MF_STRING, ID_MENU_ADD_FILLPOINTS, _T("添加填充(指定区域)"));
		m.AppendMenu(MF_STRING, ID_MENU_ADD_PAN, _T("添加矩形区域"));
		m.AppendMenu(MF_SEPARATOR);
		m.AppendMenu(MF_STRING, ID_MENU_BZ_MANAGE, _T("管理标注"));

		customMenu.AppendMenu(MF_POPUP, (UINT_PTR)m.m_hMenu, _T("标注"));
	}

	{
		CMenu m;
		m.CreatePopupMenu();
		m.AppendMenu(MF_STRING, ID_MENU_ENABLEAUTOAXIS, _T("启用"));
		m.AppendMenu(MF_STRING, ID_MENU_DISABLEAUTOAXIS, _T("禁用"));

		customMenu.AppendMenu(MF_POPUP, (UINT_PTR)m.m_hMenu, _T("浏览轴X"));
	}

	{
		CMenu m;
		m.CreatePopupMenu();
		m.AppendMenu(MF_STRING, ID_MENU_ENABLEAUTOAXISY, _T("启用"));
		m.AppendMenu(MF_STRING, ID_MENU_DISABLEAUTOAXISY, _T("禁用"));

		customMenu.AppendMenu(MF_POPUP, (UINT_PTR)m.m_hMenu, _T("浏览轴Y"));
	}

	customMenu.AppendMenu(MF_STRING, ID_MENU_EXPORT_DATA, _T("导出数据"));

	customMenu.AppendMenu(MF_STRING, ID_MENU_SAVE_ASIMAGE, _T("保存到图片"));

	customMenu.AppendMenu(MF_STRING, ID_MENU_DP_ZSCD, _T("噪声测定"));

	{
		CMenu m;
		m.CreatePopupMenu();
		m.AppendMenu(MF_STRING, ID_MENU_DISPLAY_GRID, _T("显示/隐藏网格"));
		m.AppendMenu(MF_STRING, ID_MENU_DISPLAY_LEGEND, _T("显示/隐藏图例"));

		customMenu.AppendMenu(MF_POPUP, (UINT_PTR)m.m_hMenu, _T("其他"));
	}

	return MENU_CUSTOM;
}
CBase_SampleChart::CBase_SampleChart():/*m_pStartSerie(NULL),m_pStopSerie(NULL),*/
m_pcCurveMain(NULL),m_crBaseLines(NULL), m_crDivideBaseLines(NULL), m_crAssistants(NULL)
	,m_pXLabel(NULL),m_pYLabel(NULL),m_pXLabelBk(NULL),m_pYLabelBk(NULL)
	,m_RealtimeDataMonitor(NULL),m_ma(7)
{
	CSuperChartCtrl::SetAxisMinMaxFormat(_T("%.3f"), _T("%.3f"), _T("%.3f"), _T("%.3f"));
	CreatNewGroup(true);
	m_nTemporaryGroupID = CreatNewGroup(false);
}
CBase_SampleChart::~CBase_SampleChart()
{
	if(m_pYLabel) TWenDeleteChartPan(m_pYLabel);
	if(m_pXLabel) TWenDeleteChartPan(m_pXLabel);
	if(m_pYLabelBk) TWenDeleteChartPan(m_pYLabelBk);
	if(m_pXLabelBk) TWenDeleteChartPan(m_pXLabelBk);
}
CChartSerie* CBase_SampleChart::CreateSerie(CChartSerie::enumSerieType eType)
{
	switch(eType)
	{
	case CChartSerie::stLineSerie:return CreateLineSerie();break;
	case CChartSerie::stPointsSerie:return CreatePointsSerie();break;
	case CChartSerie::stSurfaceSerie:return CreateSurfaceSerie();break;
	case CChartSerie::stBarSerie:return CreateBarSerie();break;
	default:return NULL;
	}
}
CChartSerie* CBase_SampleChart::CreateSerie()
{
	return CreateSerie(m_eSerieType);
}
CChartSerie* CBase_SampleChart::CreateCurve2DSerie(const CString& group, const CString& serie)
{
	void* pV = NULL;
	if(m_mapCurve2DSeries.Lookup(group + _T("_") + serie, pV))
	{
		if(((CChartSerie*)pV)->SerieType == m_eSerieType) return (CChartSerie*)pV;
		else RemoveSerie(((CChartSerie*)pV)->GetSerieId()); 
	}

	CChartSerie* pSerie = CreateSerie();
	if(pSerie) pSerie->SetName((LPCTSTR)serie);
	m_mapCurve2DSeries.SetAt(group + _T("_") + serie, pSerie);
	return pSerie;
}

CChartSerie* CBase_SampleChart::CreateCurve2DMain(void)
{
	if(m_pcCurveMain)
		delete m_pcCurveMain;

	m_pcCurveMain = CreateSerie();
	if(m_pcCurveMain) m_pcCurveMain->SetName(_T("信号值"));//MainCurve
	return m_pcCurveMain;

}
/*
CChartSerie* CBase_SampleChart::GetMainSerie()
{
	if(m_pSample)
	{
		//主曲线
		CString mainGroup, mainSerie;
		if(m_pSample->GetIADataCurve2D_MainInfo(&mainGroup, &mainSerie))
		{
			void* pV = NULL;
			if(m_mapCurve2DSeries.Lookup(mainGroup + _T("_") + mainSerie, pV)) return (CChartSerie*)pV;
		}
	}
	return NULL;
}
const CChartSerie* CBase_SampleChart::GetMainSerie() const
{
	if(m_pSample)
	{
		//主曲线
		CString mainGroup, mainSerie;
		if(m_pSample->GetIADataCurve2D_MainInfo(&mainGroup, &mainSerie))
		{
			void* pV = NULL;
			if(m_mapCurve2DSeries.Lookup(mainGroup + _T("_") + mainSerie, pV)) return (CChartSerie*)pV;
		}
	}
	return NULL;
}
*/
void CBase_SampleChart::Init(CChartSerie::enumSerieType eType)
{
//	m_pSample = pSample;
	m_eSerieType = eType;

	RemoveAllSeries();
	m_mapCurve2DVisibilites.RemoveAll();
	m_mapCurve2DSeries.RemoveAll();
	
// 	m_pStartSerie = CreateSerie();
// 	m_pStartSerie->SetName(_T("开始"));
// 
// 	m_pStopSerie = CreateSerie();
// 	m_pStopSerie->SetName(_T("停止"));

	CreateCurve2DMain();
/*
	if(m_pSample)
	{
		//主曲线
		CString mainGroup, mainSerie;
		if(m_pSample->GetIADataCurve2D_MainInfo(&mainGroup, &mainSerie))
		{
			CreateCurve2DSerie(mainGroup, mainSerie);
		}
	}
*/
// 	m_pBeginPoints = CreatePointsSerie();
// 	m_pBeginPoints->SetName(_T("起点"));
// 
// 	m_pPeakPoints = CreatePointsSerie();
// 	m_pPeakPoints->SetName(_T("峰谷"));
// 	m_pPeakPoints->SetPointType(CChartPointsSerie::ptTriangle);//三角标记
// 
// 	m_pEndPoints = CreatePointsSerie();
// 	m_pEndPoints->SetName(_T("终点"));

	ResetAxis();

	bool bYX = (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_LC1620A) == 0);
	bool bQX = (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC_PX) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC1120) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC2400) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC1100W) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC1100P) == 0);
	bool bIC = (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_IC1800) == 0);

	if(bYX || bQX || bIC)
	{
		int w = 25;
		int h = 16;

		m_pXLabelBk = TWenAddChartPan(false, false);
		m_pXLabelBk->SetMustInPlottingRect(false);
		m_pXLabelBk->m_dwPositionFlag = CTWenChartPan::PT_CUSTOM;
		m_pXLabelBk->m_CustomValues.MinX.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_RIGHT;
		m_pXLabelBk->m_CustomValues.MinX.value = w+3;
		m_pXLabelBk->m_CustomValues.MaxX.eType = CTWenChartPan::CVT_OFFSETCTRLPIXEL_RIGHT;
		m_pXLabelBk->m_CustomValues.MaxX.value = 3;
		m_pXLabelBk->m_CustomValues.MinY.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_BOTTOM;
		m_pXLabelBk->m_CustomValues.MinY.value = -2;
		m_pXLabelBk->m_CustomValues.MaxY.eType = CTWenChartPan::CVT_OFFSETCTRLPIXEL_BOTTOM;
		m_pXLabelBk->m_CustomValues.MaxY.value = 3;
		//m_pXLabelBk->SetColor(RGB(255, 0, 0));
		m_pXLabelBk->SetAlpha(255);
		m_pXLabelBk->SetVisible(true);
		m_pXLabelBk->m_dwChartObjectFlag = CTWenChartObject::CHARTOBJECT_NOTINBZMANAGE;

		m_pXLabel = TWenAddChartPan(false, false);
		m_pXLabel->SetMustInPlottingRect(false);
		m_pXLabel->m_dwPositionFlag = CTWenChartPan::PT_CUSTOM;
		m_pXLabel->m_CustomValues.MinX.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_RIGHT;
		m_pXLabel->m_CustomValues.MinX.value = w+3;
		m_pXLabel->m_CustomValues.MaxX.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_RIGHT;
		m_pXLabel->m_CustomValues.MaxX.value = 3;
		m_pXLabel->m_CustomValues.MinY.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_BOTTOM;
		m_pXLabel->m_CustomValues.MinY.value = -2;
		m_pXLabel->m_CustomValues.MaxY.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_BOTTOM;
		m_pXLabel->m_CustomValues.MaxY.value = -h;
		m_pXLabel->SetAlpha(255);
		//m_pXLabel->SetColor(RGB(128, 128, 128));
		//m_pXLabel->SetColor(RGB(255, 255, 255));
		m_pXLabel->SetVisible(true);
		m_pXLabel->SetContent(_T("min"));
		m_pXLabel->Font.Color = RGB(0, 0, 0);
		m_pXLabel->m_dwChartObjectFlag = CTWenChartObject::CHARTOBJECT_NOTINBZMANAGE;


		w = bYX?25:20;
		h = 16;

		m_pYLabelBk = TWenAddChartPan(false, false);
		m_pYLabelBk->SetMustInPlottingRect(false);
		m_pYLabelBk->m_dwPositionFlag = CTWenChartPan::PT_CUSTOM;
		m_pYLabelBk->m_CustomValues.MinX.eType = CTWenChartPan::CVT_OFFSETCTRLPIXEL_LEFT;
		m_pYLabelBk->m_CustomValues.MinX.value = -3;
		m_pYLabelBk->m_CustomValues.MaxX.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_LEFT;
		m_pYLabelBk->m_CustomValues.MaxX.value = 2;
		m_pYLabelBk->m_CustomValues.MinY.eType = CTWenChartPan::CVT_OFFSETCTRLPIXEL_TOP;
		m_pYLabelBk->m_CustomValues.MinY.value = -3;
		m_pYLabelBk->m_CustomValues.MaxY.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_TOP;
		m_pYLabelBk->m_CustomValues.MaxY.value = -h;
		m_pYLabelBk->SetAlpha(255);
		m_pYLabelBk->SetVisible(true);
		m_pYLabelBk->m_dwChartObjectFlag = CTWenChartObject::CHARTOBJECT_NOTINBZMANAGE;

		m_pYLabel = TWenAddChartPan(false, false);
		m_pYLabel->SetMustInPlottingRect(false);
		m_pYLabel->m_dwPositionFlag = CTWenChartPan::PT_CUSTOM;
		m_pYLabel->m_CustomValues.MinX.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_LEFT;
		m_pYLabel->m_CustomValues.MinX.value = w;
		m_pYLabel->m_CustomValues.MaxX.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_LEFT;
		m_pYLabel->m_CustomValues.MaxX.value = 2;
		m_pYLabel->m_CustomValues.MinY.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_TOP;
		m_pYLabel->m_CustomValues.MinY.value = 0;
		m_pYLabel->m_CustomValues.MaxY.eType = CTWenChartPan::CVT_OFFSETPLOTTINGPIXEL_TOP;
		m_pYLabel->m_CustomValues.MaxY.value = -h;
		m_pYLabel->SetAlpha(255);
		//m_pYLabel->SetColor(RGB(128, 128, 128));
		//m_pYLabel->SetColor(RGB(255, 0, 0));
		//m_pYLabel->SetColor(RGB(255, 255, 255));
		m_pYLabel->SetVisible(true);
		if(bYX) m_pYLabel->SetContent(_T("mAU"));
		else if(bQX) m_pYLabel->SetContent(_T("mV"));
		else if(bIC) m_pYLabel->SetContent(_T("μS"));
		m_pYLabel->Font.Color = RGB(0, 0, 0);
		m_pYLabel->m_dwChartObjectFlag = CTWenChartObject::CHARTOBJECT_NOTINBZMANAGE;
	}
}
void CBase_SampleChart::SetDrawMask(DWORD dMask)
{
	m_drawMask = dMask;
}

void CBase_SampleChart::OnBackColorChanged()
{
	if(m_pXLabel) m_pXLabel->SetColor(m_BackColor);
	if(m_pYLabel) m_pYLabel->SetColor(m_BackColor);
	if(m_pXLabelBk) m_pXLabelBk->SetColor(m_BackColor);
	if(m_pYLabelBk) m_pYLabelBk->SetColor(m_BackColor);
}
void CBase_SampleChart::ResetAxis()
{
	GetBottomAxis()->SetCoordinate(0, 5, COORDINATE_SET);
	GetLeftAxis()->SetCoordinate(-10, 500, COORDINATE_SET);

	GetBottomAxis()->GetGrid()->SetVisible(false);
	GetLeftAxis()->GetGrid()->SetVisible(false);

	if((g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC_PX) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC1120) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC2400) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC1100W) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_GC1100P) == 0)) 
		 GetLeftAxis()->SetZoomLimit(0.01);
	else if((g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_LC1620A) == 0) || (g_cConfigIni.CurrentSystemName().Compare(PASYSTEM_NAME_IC1800) == 0)) 
		 GetLeftAxis()->SetZoomLimit(0.1);
	else GetLeftAxis()->SetZoomLimit(0.1);
	GetTopAxis()->SetZoomLimit(0.1);
	GetRightAxis()->SetZoomLimit(0.1);
	GetBottomAxis()->SetZoomLimit(0.1);
}
void CBase_SampleChart::LoadSerieFromFile(structMinMaxInfo&mmi, CChartSerie* pSerie, LPCTSTR filename)
{
	CIAFileCurve2D file;
	if(!file.Open(filename)) return;
	file.BeginEnum();
	DWORD pointCount = file.m_Ext.GetCount();
	pSerie->m_vPoints.InitPoints(pointCount+2);
	double x, y;
	CTime t;
	while(file.GetNext(x, y, t))
	{
		pSerie->m_vPoints.AddPoint(x, y);
		if(mmi.minx > x) mmi.minx = x;
		if(mmi.maxx < x) mmi.maxx = x;
		if(mmi.miny > y) mmi.miny = y;
		if(mmi.maxy < y) mmi.maxy = y;
	}
	file.Close();
}
void CBase_SampleChart::RefreshSample_Curve2DSeries(CBase_AnalysisSample* pSample ,const CString& dataFolder, bool bRefreshAxisRange)
{
	if(pSample)
	{
		CIADataCurve2D c2d;
		if(!PASUCCESS(pSample->GetIADataCurve2D(c2d))) return;
		structMinMaxInfo mmi;
		mmi.minx=0,mmi.miny=0,mmi.maxx=0,mmi.maxy=0;
		CString type;
		CIADataCurve2DSingle single;
		CString strMainGroup,strMainSerie;
		pSample->GetIADataCurve2D_MainInfo(&strMainGroup, &strMainSerie);
		long pos;
		c2d.BeginEnum(pos);
		while(c2d.GetNext(pos, type, single))
		{
			CString name,filename;
			single.BeginEnum();
			while(single.GetNext(name, filename))
			{
				void* pv = NULL;
				m_mapCurve2DVisibilites.Lookup(name, pv);
				void* pLine = NULL;
				bool bIsMain = strMainGroup == type && strMainSerie == name;
				if(bIsMain) continue;//主曲线由积分结果负责
				if(bIsMain) pLine = m_pcCurveMain;
				else m_mapCurve2DSeries.Lookup(name, pLine);
				if(pv)//需要绘画
				{
					if(bIsMain)
					{
						if(m_pcCurveMain)
						{
							m_pcCurveMain->ClearSerie();
							LoadSerieFromFile(mmi, m_pcCurveMain, dataFolder + filename);
							m_pcCurveMain->SetVisible(true);
						}
					}
					else
					{
						if(pLine == NULL)
						{
							CChartSerie* line = CreateCurve2DSerie(type, name);
							line->ClearSerie();
							LoadSerieFromFile(mmi, line, dataFolder + filename);
						}
						else ((CChartSerie*)pLine)->SetVisible(true);
					}
				}
				else
				{
					if(pLine) ((CChartSerie*)pLine)->SetVisible(false);
				}
			}
		}
		if(bRefreshAxisRange)
		{
			GetBottomAxis()->SetCoordinate(mmi.minx, mmi.maxx, COORDINATE_SET);
			GetLeftAxis()->SetCoordinate(mmi.miny, mmi.maxy, COORDINATE_SET);
		}
	}
}

void CBase_SampleChart::ClearCompSeries()
{
	POSITION pos = m_mapCompSeries.GetStartPosition();
	void* pv = NULL;
	CString key;
	while(pos)
	{
		m_mapCompSeries.GetNextAssoc(pos, key, pv);
		CChartSerie* pSerie = (CChartSerie*)pv;
		if(pSerie) RemoveSerie(pSerie->GetSerieId());
	}
	m_mapCompSeries.RemoveAll();
}
void CBase_SampleChart::ClearCurve2DSeries()
{
	POSITION pos = m_mapCurve2DSeries.GetStartPosition();
	void* pv = NULL;
	CString key;
	while(pos)
	{
		m_mapCurve2DSeries.GetNextAssoc(pos, key, pv);
		CChartSerie* pSerie = (CChartSerie*)pv;
		if(pSerie) RemoveSerie(pSerie->GetSerieId());
	}
	m_mapCurve2DSeries.RemoveAll();
}
void CBase_SampleChart::ClearMarkLines()
{
// 	if(m_pStartSerie) m_pStartSerie->ClearSerie();
// 	if(m_pStopSerie) m_pStopSerie->ClearSerie();
}
void CBase_SampleChart::ClearTemporary()
{
	DeleteGroup(m_nTemporaryGroupID, false, false);
}
const CPeakStandard* GetPSByTime(const CPeakList& list, double dblTime)
{
	CString strTime = CParamCommon::TimeString(dblTime);
	for(int i=0; i<list.m_cArrayData.GetCount(); ++i)
	{
		const CPeakStandard& ps = list.m_cArrayData.GetAt(i);
		if(CParamCommon::TimeString(ps.m_dReserveTime) == strTime) return &ps;
	}
	return NULL;
}
void CBase_SampleChart::RedrawPeak(const CBase_AnalysisSample* pSample, DWORD dwFlag, CBase_SampleChart::enumMode eMode, bool bRefresh)
{
	if(!pSample) return;
	EnableRefresh(false);
	if(dwFlag & FLAG_DATA_SOURCE)
	{
		CChartSerie* pSerie = GetMainSerie();
		if(pSerie)
		{
			pSerie->ClearSerie();
			const CArray<CCurveDataSingle>& m_srcData = pSample->m_dataMainCurve.m_arrayData;
			if(m_srcData.GetUpperBound() > -1)
			{
				pSerie->m_vPoints.InitPoints(m_srcData.GetUpperBound()+1);
				structMinMaxInfo mmi;
				GetBottomAxis()->GetMinMax(mmi.minx, mmi.maxx);
				GetLeftAxis()->GetMinMax(mmi.miny, mmi.maxy);
				bool bNeed = mmi.minx==0 && mmi.miny == 0 && mmi.maxx == 0 && mmi.maxy == 0;
				for (int i=0; i<=m_srcData.GetUpperBound(); ++i)
				{
					double x = m_srcData[i].dX;
					double y = m_srcData[i].dY;
					pSerie->m_vPoints.AddPoint(x, y);					
					if(mmi.minx > x) mmi.minx = x;
					if(mmi.maxx < x) mmi.maxx = x;
					if(mmi.miny > y) mmi.miny = y;
					if(mmi.maxy < y) mmi.maxy = y;
				}
				if(bNeed)
				{
					GetBottomAxis()->SetCoordinate(mmi.minx, mmi.maxx, COORDINATE_SET);
					GetLeftAxis()->SetCoordinate(mmi.miny, mmi.maxy, COORDINATE_SET);
				}
			}
		}
	}
	//const CPeakList* pPS = &pSample->m_cResultList;
	const CPeakList* pPS = &pSample->m_cPeakList;
	const CPeakList* pRT = &pSample->m_cAllRealTimePeakList; 
	/**********************************************************************
	if(dwFlag & FLAG_POINTS_BEGIN)
	{
		if(m_pBeginPoints)
		{
			if(eMode == MODE_SETALL) m_pBeginPoints->ClearSerie();
			if(pPS->m_cArrayData.GetUpperBound() > -1)
			{
				if(eMode == MODE_SETALL) m_pBeginPoints->m_vPoints.InitPoints(pPS->m_cArrayData.GetUpperBound()+1);
				for (int i=0; i<=pPS->m_cArrayData.GetUpperBound(); ++i)
				{
					m_pBeginPoints->m_vPoints.AddPoint(pPS->m_cArrayData[i].m_dataBegin.dX, pPS->m_cArrayData[i].m_dataBegin.dY);
				}
			}
		}
	}
	if(dwFlag & FLAG_POINTS_PEAK)
	{
		if(m_pPeakPoints)
		{
			if(eMode == MODE_SETALL) m_pPeakPoints->ClearSerie();
			if(pPS->m_cArrayData.GetUpperBound() > -1)
			{
				if(eMode == MODE_SETALL) m_pPeakPoints->m_vPoints.InitPoints(pPS->m_cArrayData.GetUpperBound()+1);
				for (int i=0; i<=pPS->m_cArrayData.GetUpperBound(); ++i)
				{
					m_pPeakPoints->m_vPoints.AddPoint(pPS->m_cArrayData[i].m_dataTop.dX, pPS->m_cArrayData[i].m_dataTop.dY);
				}
			}
		}
	}
	if(dwFlag & FLAG_POINTS_END)
	{
		if(m_pEndPoints)
		{
			if(eMode == MODE_SETALL) m_pEndPoints->ClearSerie();
			if(pPS->m_cArrayData.GetUpperBound() > -1)
			{
				if(eMode == MODE_SETALL) m_pEndPoints->m_vPoints.InitPoints(pPS->m_cArrayData.GetUpperBound()+1);
				for (int i=0; i<=pPS->m_cArrayData.GetUpperBound(); ++i)
				{
					m_pEndPoints->m_vPoints.AddPoint(pPS->m_cArrayData[i].m_dataEnd.dX, pPS->m_cArrayData[i].m_dataEnd.dY);
				}
			}
		}
	}//*******************************************************************/
	if(dwFlag & FLAG_LINES_BASE)
	{
		//首先删除全部，再重新绘画
		if(eMode == MODE_SETALL) ClearPeak(pSample, FLAG_LINES_BASE);

		CChartLineSerie* pLine = NULL;
		for (int i=0; i<=pPS->m_cArrayData.GetUpperBound(); ++i)
		{
			pLine = CreateLineSerie();
			pLine->SetColor(m_crBaseLines);
			pLine->m_vPoints.AddPoint(pPS->m_cArrayData[i].m_dataBegin.dX, pPS->m_cArrayData[i].m_dataBegin.dY);
			pLine->m_vPoints.AddPoint(pPS->m_cArrayData[i].m_dataEnd.dX, pPS->m_cArrayData[i].m_dataEnd.dY);
			pLine->m_dwDPFlag = NULL;//不能在数据处理的曲线选择列表框中出现
			m_arrBaseLines.Add(pLine);
		}
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		CChartLineSerie* pLine = NULL;
		//首先删除全部，再重新绘画
		if(eMode == MODE_SETALL) ClearPeak(pSample, FLAG_LINES_DIVIDEBASE);

		for (int i=0; i<=pSample->m_arrayVLines.GetUpperBound(); ++i)
		{
			const CBase_AnalysisSample::structLine& line = pSample->m_arrayVLines.GetAt(i);
			pLine = CreateLineSerie();
			pLine->SetColor(m_crBaseLines);
			pLine->m_vPoints.AddPoint(line.begin.x, line.begin.y);
			pLine->m_vPoints.AddPoint(line.end.x, line.end.y);
			pLine->m_dwDPFlag = NULL;//不能在数据处理的曲线选择列表框中出现
			m_arrDivideBaseLines.Add(pLine);
		}
	}

	if((dwFlag & FLAG_STRINGS_RESERVEDTIME) || (dwFlag & FLAG_STRINGS_CONCENTRATION) || (dwFlag & FLAG_STRINGS_COMPONENTNAME) || (dwFlag & FLAG_STRINGS_FMJ) || (dwFlag & FLAG_STRINGS_FG))
	{
		//首先删除全部，再重新绘画
		if(eMode == MODE_SETALL) ClearPeak(pSample, FLAGMASK_STRINGS_ASSISTANT);
		

		CString t;
		int i_upper_bound = pPS->m_cArrayData.GetUpperBound();
		for (int i=0; i<=i_upper_bound; ++i)
		{
			CString ass;
			const CPeakStandard& ps = pPS->m_cArrayData[i];
			if(dwFlag & FLAG_STRINGS_RESERVEDTIME) ass += CParamCommon::TimeString(ps.m_dReserveTime) + _T("/");
			if(dwFlag & FLAG_STRINGS_FMJ)
			{
				t.Format(_T("%.3f/"), ps.m_dArea);
				ass += t;
			}
			if(dwFlag & FLAG_STRINGS_FG)
			{
				t.Format(_T("%.3f/"), ps.m_dHeight);
				ass += t;
			}

			const CPeakStandard* pPSResult = GetPSByTime(pSample->m_cResultList, ps.m_dReserveTime);
			if(pPSResult)
			{
				if(dwFlag & FLAG_STRINGS_CONCENTRATION)
				{
					t.Format(_T("%.1f/"), pPSResult->m_dResultConcentration);
					ass += t;
				}
				if(dwFlag & FLAG_STRINGS_COMPONENTNAME)
				{
					ass += pPSResult->m_cStrComponentName;
					ass += _T('/');
				}
			}

			while(ass.Right(1) == _T("/")) ass.Delete(ass.GetLength()-1);
			//ass = _T("-->") + ass;
			CSuperChartString* pSerie = AddChartString(ps.m_dataTop.dX, ps.m_dataTop.dY, ass);
			pSerie->SetMustInPlottingRect(true);
			pSerie->Font.Color = m_crAssistants;
			pSerie->Font.SetSize(100);
			pSerie->Font.SetBold(550);
			pSerie->Font.SetAngle(600);
			pSerie->SetScreenOffset(-6, -8);
			pSerie->m_dwChartObjectFlag = CTWenChartObject::CHARTOBJECT_NOTINBZMANAGE;
			m_arrAssistants.Add(pSerie);
		}
	}
	EnableRefresh(true);
	if(bRefresh) RefreshCtrl();
}

void CBase_SampleChart::RedrawPeakOnRealTime(const CBase_AnalysisSample* pSample, DWORD dwFlag, CBase_SampleChart::enumMode eMode, bool bRefresh)
{
	if(!pSample) return;
	EnableRefresh(false);

	if(dwFlag & FLAG_DATA_SOURCE)
	{
		CChartSerie* pSerie = GetMainSerie();
		if(pSerie)
		{
			pSerie->ClearSerie();
			const CArray<CCurveDataSingle>& m_srcData = pSample->m_dataMainCurve.m_arrayData;
			if(m_srcData.GetUpperBound() > -1)
			{
				pSerie->m_vPoints.InitPoints(m_srcData.GetUpperBound()+1);
				structMinMaxInfo mmi;
				GetBottomAxis()->GetMinMax(mmi.minx, mmi.maxx);
				GetLeftAxis()->GetMinMax(mmi.miny, mmi.maxy);
				bool bNeed = mmi.minx==0 && mmi.miny == 0 && mmi.maxx == 0 && mmi.maxy == 0;
				for (int i=0; i<=m_srcData.GetUpperBound(); ++i)
				{
					double x = m_srcData[i].dX;
					double y = m_srcData[i].dY;
					pSerie->m_vPoints.AddPoint(x, y);					
					if(mmi.minx > x) mmi.minx = x;
					if(mmi.maxx < x) mmi.maxx = x;
					if(mmi.miny > y) mmi.miny = y;
					if(mmi.maxy < y) mmi.maxy = y;
				}
				if(bNeed)
				{
					GetBottomAxis()->SetCoordinate(mmi.minx, mmi.maxx, COORDINATE_SET);
					GetLeftAxis()->SetCoordinate(mmi.miny, mmi.maxy, COORDINATE_SET);
				}
			}
		}
	}
	//const CPeakList* pPS = &pSample->m_cResultList;
	const CPeakList* pPS = &pSample->m_cPeakList;
	const CPeakList* pRT = &pSample->m_cAllRealTimePeakList; 
	
	if(dwFlag & FLAG_LINES_BASE)
	{
		//首先删除全部，再重新绘画
		if(eMode == MODE_SETALL) ClearPeak(pSample, FLAG_LINES_BASE);

		CChartLineSerie* pLine = NULL;
		for (int i=0; i<=pRT->m_cArrayData.GetUpperBound(); ++i)
		{
			pLine = CreateLineSerie();
			pLine->SetColor(m_crBaseLines);
			pLine->m_vPoints.AddPoint(pRT->m_cArrayData[i].m_dataBegin.dX, pRT->m_cArrayData[i].m_dataBegin.dY);
			pLine->m_vPoints.AddPoint(pRT->m_cArrayData[i].m_dataEnd.dX, pRT->m_cArrayData[i].m_dataEnd.dY);
			pLine->m_dwDPFlag = NULL;//不能在数据处理的曲线选择列表框中出现
			m_arrBaseLines.Add(pLine);
		}
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		CChartLineSerie* pLine = NULL;
		//首先删除全部，再重新绘画
		if(eMode == MODE_SETALL) ClearPeak(pSample, FLAG_LINES_DIVIDEBASE);

		for (int i=0; i<=pSample->m_arrayVLines.GetUpperBound(); ++i)
		{
			const CBase_AnalysisSample::structLine& line = pSample->m_arrayVLines.GetAt(i);
			pLine = CreateLineSerie();
			pLine->SetColor(m_crBaseLines);
			pLine->m_vPoints.AddPoint(line.begin.x, line.begin.y);
			pLine->m_vPoints.AddPoint(line.end.x, line.end.y);
			pLine->m_dwDPFlag = NULL;//不能在数据处理的曲线选择列表框中出现
			m_arrDivideBaseLines.Add(pLine);
		}
	}


	if((dwFlag & FLAG_STRINGS_RESERVEDTIME) || (dwFlag & FLAG_STRINGS_CONCENTRATION) || (dwFlag & FLAG_STRINGS_COMPONENTNAME) || (dwFlag & FLAG_STRINGS_FMJ) || (dwFlag & FLAG_STRINGS_FG))
	{
		
		//不管。首先删除全部，再重新绘画
		if(eMode == MODE_SETALL) ClearPeak(pSample, FLAGMASK_STRINGS_ASSISTANT);

		CString t;
		int i_upper_bound = pRT->m_cArrayData.GetUpperBound();
		for (int i=0; i<=i_upper_bound; ++i)
		{
			CString ass;
			const CPeakStandard& ps = pRT->m_cArrayData[i];
			if(dwFlag & FLAG_STRINGS_RESERVEDTIME) ass += CParamCommon::TimeString(ps.m_dReserveTime) + _T("/");
			if(dwFlag & FLAG_STRINGS_FMJ)
			{
				t.Format(_T("%.3f/"), ps.m_dArea);
				ass += t;
			}
			if(dwFlag & FLAG_STRINGS_FG)
			{
				t.Format(_T("%.3f/"), ps.m_dHeight);
				ass += t;
			}

			const CPeakStandard* pPSResult = GetPSByTime(pSample->m_cResultList, ps.m_dReserveTime);
			if(pPSResult)
			{
				if(dwFlag & FLAG_STRINGS_CONCENTRATION)
				{
					t.Format(_T("%.1f/"), pPSResult->m_dResultConcentration);
					ass += t;
				}
				if(dwFlag & FLAG_STRINGS_COMPONENTNAME)
				{
					ass += pPSResult->m_cStrComponentName;
					ass += _T('/');
				}
			}

			while(ass.Right(1) == _T("/")) ass.Delete(ass.GetLength()-1);
			//ass = _T("-->") + ass;
			CSuperChartString* pSerie = AddChartString(ps.m_dataTop.dX, ps.m_dataTop.dY, ass);
			pSerie->SetMustInPlottingRect(true);
			pSerie->Font.Color = m_crAssistants;
			pSerie->Font.SetSize(100);
			pSerie->Font.SetBold(550);
			pSerie->Font.SetAngle(600);
			pSerie->SetScreenOffset(-6, -8);
			pSerie->m_dwChartObjectFlag = CTWenChartObject::CHARTOBJECT_NOTINBZMANAGE;
			m_arrAssistants.Add(pSerie);
		}
	}
	EnableRefresh(true);
	if(bRefresh) RefreshCtrl();
}


void CBase_SampleChart::ClearPeak(const CBase_AnalysisSample* pSample, DWORD dwFlag)
{
	EnableRefresh(false);
	if(dwFlag & FLAG_DATA_SOURCE)
	{
		CChartSerie* pSerie = GetMainSerie();
		if(pSerie) pSerie->ClearSerie();
	}
// 	if(dwFlag & FLAG_POINTS_BEGIN)
// 	{
// 		if(m_pBeginPoints) m_pBeginPoints->ClearSerie();
// 	}
// 	if(dwFlag & FLAG_POINTS_PEAK)
// 	{
// 		if(m_pPeakPoints) m_pPeakPoints->ClearSerie();
// 	}
// 	if(dwFlag & FLAG_POINTS_END)
// 	{
// 		if(m_pEndPoints) m_pEndPoints->ClearSerie();
// 	}
	if(dwFlag & FLAG_LINES_BASE)
	{
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrBaseLines.GetAt(i);
			if(pLine) RemoveSerie(pLine->GetSerieId());
		}
		m_arrBaseLines.RemoveAll();
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrDivideBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrDivideBaseLines.GetAt(i);
			if(pLine) RemoveSerie(pLine->GetSerieId());
		}
		m_arrDivideBaseLines.RemoveAll();
	}
	DWORD dwFMask = dwFlag & FLAGMASK_STRINGS_ASSISTANT;
	if(dwFMask == FLAGMASK_STRINGS_ASSISTANT)
	{
		//同时都删除,才清空		
		CSuperChartString* pSerie = NULL;
		for(int i=0; i<=m_arrAssistants.GetUpperBound(); i++)
		{
			pSerie = m_arrAssistants.GetAt(i);
			if(pSerie) RemoveChartString(pSerie);
		}
		m_arrAssistants.RemoveAll();
	}
	else if(dwFMask)
	{
		//否则,重画
		RedrawPeak(pSample, FLAGMASK_STRINGS_ASSISTANT & ~dwFMask);
	}
	EnableRefresh(true);
	RefreshCtrl();
}
void CBase_SampleChart::ShowPeak(const CBase_AnalysisSample* pSample, DWORD dwFlag)
{
	EnableRefresh(false);
	if(dwFlag & FLAG_DATA_SOURCE)
	{
		CChartSerie* pSerie = GetMainSerie();
		if(pSerie) pSerie->SetVisible(true);
	}
// 	if(dwFlag & FLAG_POINTS_BEGIN)
// 	{
// 		if(m_pBeginPoints) m_pBeginPoints->SetVisible(true);
// 	}
// 	if(dwFlag & FLAG_POINTS_PEAK)
// 	{
// 		if(m_pPeakPoints) m_pPeakPoints->SetVisible(true);
// 	}
// 	if(dwFlag & FLAG_POINTS_END)
// 	{
// 		if(m_pEndPoints) m_pEndPoints->SetVisible(true);
// 	}
	if(dwFlag & FLAG_LINES_BASE)
	{
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrBaseLines.GetAt(i);
			if(pLine) pLine->SetVisible(true);
		}
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrDivideBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrDivideBaseLines.GetAt(i);
			if(pLine) pLine->SetVisible(true);
		}
	}
	DWORD dwFMask = dwFlag & FLAGMASK_STRINGS_ASSISTANT;
	if(dwFMask == FLAGMASK_STRINGS_ASSISTANT)
	{
		//同时都显示,才显示		
		CSuperChartString* pSerie = NULL;
		for(int i=0; i<=m_arrAssistants.GetUpperBound(); i++)
		{
			pSerie = m_arrAssistants.GetAt(i);
			if(pSerie) pSerie->SetVisible(true);
		}
	}
	else if(dwFMask)
	{
		//否则,重画
		RedrawPeak(pSample, FLAGMASK_STRINGS_ASSISTANT & ~dwFMask);
	}
	EnableRefresh(true);
	RefreshCtrl();	
}
void CBase_SampleChart::HidePeak(const CBase_AnalysisSample* pSample, DWORD dwFlag)
{
	EnableRefresh(false);
	if(dwFlag & FLAG_DATA_SOURCE)
	{
		CChartSerie* pSerie = GetMainSerie();
		if(pSerie) pSerie->SetVisible(false);
	}
// 	if(dwFlag & FLAG_POINTS_BEGIN)
// 	{
// 		if(m_pBeginPoints) m_pBeginPoints->SetVisible(false);
// 	}
// 	if(dwFlag & FLAG_POINTS_PEAK)
// 	{
// 		if(m_pPeakPoints) m_pPeakPoints->SetVisible(false);
// 	}
// 	if(dwFlag & FLAG_POINTS_END)
// 	{
// 		if(m_pEndPoints) m_pEndPoints->SetVisible(false);
// 	}
	if(dwFlag & FLAG_LINES_BASE)
	{
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrBaseLines.GetAt(i);
			if(pLine) pLine->SetVisible(false);
		}
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrDivideBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrDivideBaseLines.GetAt(i);
			if(pLine) pLine->SetVisible(false);
		}
	}
	DWORD dwFMask = dwFlag & FLAGMASK_STRINGS_ASSISTANT;
	if(dwFMask == FLAGMASK_STRINGS_ASSISTANT)
	{
		//同时都显示,才显示		
		CSuperChartString* pSerie = NULL;
		for(int i=0; i<=m_arrAssistants.GetUpperBound(); i++)
		{
			pSerie = m_arrAssistants.GetAt(i);
			if(pSerie) pSerie->SetVisible(false);
		}
	}
	else if(dwFMask)
	{
		//否则,重画
		RedrawPeak(pSample, FLAGMASK_STRINGS_ASSISTANT & ~dwFMask);
	}
	EnableRefresh(true);
	RefreshCtrl();	
}

void CBase_SampleChart::SetColor(DWORD dwFlag, COLORREF color)
{
	EnableRefresh(false);
	if(dwFlag & FLAG_DATA_SOURCE)
	{
		CChartSerie* pSerie = GetMainSerie();
		if(pSerie) pSerie->SetColor(color);
	}
// 	if(dwFlag & FLAG_POINTS_BEGIN)
// 	{
// 		if(m_pBeginPoints) m_pBeginPoints->SetColor(color);
// 	}
// 	if(dwFlag & FLAG_POINTS_PEAK)
// 	{
// 		if(m_pPeakPoints) m_pPeakPoints->SetColor(color);
// 	}
// 	if(dwFlag & FLAG_POINTS_END)
// 	{
// 		if(m_pEndPoints) m_pEndPoints->SetColor(color);
// 	}
	if(dwFlag & FLAG_LINES_BASE)
	{
		m_crBaseLines = color;
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrBaseLines.GetAt(i);
			if(pLine) pLine->SetColor(color);
		}
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		m_crDivideBaseLines = color;
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrDivideBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrDivideBaseLines.GetAt(i);
			if(pLine) pLine->SetColor(color);
		}
	}
	if(dwFlag & FLAG_LINES_DIVIDEBASE)
	{
		m_crDivideBaseLines = color;
		CChartLineSerie* pLine = NULL;
		for(int i=0; i<=m_arrDivideBaseLines.GetUpperBound(); i++)
		{
			pLine = m_arrDivideBaseLines.GetAt(i);
			if(pLine) pLine->SetColor(color);
		}
	}
	DWORD dwFMask = dwFlag & FLAGMASK_STRINGS_ASSISTANT;
	if(dwFMask == FLAGMASK_STRINGS_ASSISTANT)
	{
		//同时都更改,才更改		
		m_crAssistants = color;
		CSuperChartString* pSerie = NULL;
		for(int i=0; i<=m_arrAssistants.GetUpperBound(); i++)
		{
			pSerie = m_arrAssistants.GetAt(i);
			if(pSerie) pSerie->Font.Color = m_crAssistants;
		}
	}//否则,无效
	EnableRefresh(true);
	RefreshCtrl();	
}
COLORREF CBase_SampleChart::GetColor(enumPeakFlag eFlag) const
{
	switch(eFlag)
	{
	case FLAG_DATA_SOURCE:
		{
			const CChartSerie* pSerie = GetMainSerie();
			if(pSerie) return pSerie->GetColor();
			return NULL;
		}
		break;
// 	case FLAG_POINTS_BEGIN:
// 		if(m_pBeginPoints) return m_pBeginPoints->GetColor();
// 		else return NULL;
// 		break;
// 	case FLAG_POINTS_PEAK:
// 		if(m_pPeakPoints) return m_pPeakPoints->GetColor();
// 		else return NULL;
// 		break;
// 	case FLAG_POINTS_END:
// 		if(m_pEndPoints) return m_pEndPoints->GetColor();
// 		else return NULL;
// 		break;
	case FLAG_LINES_BASE:return m_crBaseLines;
	case FLAG_LINES_DIVIDEBASE:return m_crDivideBaseLines;
	case FLAG_STRINGS_RESERVEDTIME:
	case FLAG_STRINGS_CONCENTRATION:
	case FLAG_STRINGS_COMPONENTNAME:
	case FLAG_STRINGS_FMJ:
	case FLAG_STRINGS_FG:
		return m_crAssistants;
	default:return NULL;
	}
}
CTWenChartPan* CBase_SampleChart::ShowRangeFlag(double fromX, double toX, COLORREF cr, size_t nGroupID)
{
	CTWenChartPan* pan = TWenAddChartPan(false, false);
	pan->SetMustInPlottingRect(true);
	pan->m_dwPositionFlag = CTWenChartPan::PT_CUSTOM;
	pan->m_CustomValues.MinX.eType = CTWenChartPan::CVT_SPECIFIEDVALUE;
	pan->m_CustomValues.MinX.value = fromX;
	pan->m_CustomValues.MaxX.eType = CTWenChartPan::CVT_SPECIFIEDVALUE;
	pan->m_CustomValues.MaxX.value = toX;
	pan->m_CustomValues.MinY.eType = CTWenChartPan::CVT_CURAXISVALUE;
	pan->m_CustomValues.MaxY.eType = CTWenChartPan::CVT_CURAXISVALUE;
	pan->SetAlpha(128);
	//pan->SetColor(RGB(128, 128, 128));
	//pan->SetColor(m_crAssistants);
	pan->SetColor(cr);
	pan->SetVisible(true);
	if(nGroupID == -1) nGroupID = m_nTemporaryGroupID;
	pan->SetGroupID(nGroupID);
	return pan;
}
CChartFill* CBase_SampleChart::ShowPeakFill(const CChartPointsArray& arrPoints, COLORREF cr, size_t nGroupID)
{
	if(nGroupID == -1) nGroupID = m_nTemporaryGroupID;
	return AddChartFill(arrPoints, cr, nGroupID);
}

BOOL CBase_SampleChart::AddRealTimeCurveDataMain(double xSrc[], double ySrc[], int nSrc)
{
	//移动平均MovingAverage
	//const double* yProcessed = m_ma.Process(ySrc, nSrc);
	if(m_RealtimeDataMonitor) m_RealtimeDataMonitor->OnRealtimeData(NULL, xSrc, ySrc, nSrc, xSrc, ySrc, nSrc);

	if(!m_pcCurveMain) return FALSE;

	m_pcCurveMain->AddPoints(xSrc, (double*)ySrc, nSrc);

	return TRUE;

}

BOOL CBase_SampleChart::AddRealTimeCurveDataMainWithProcess(CBase_AnalysisSample* pSample, bool isProcess, double xSrc[], double ySrc[], int nSrc)
{	//实时曲线积分功能
	if (pSample->IsStartRealtimeProcess() && isProcess)
	{
		CAnalysisData cAnalysisData;
		CString cStrPeakProcess;

		if((pSample->m_data).GetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, cStrPeakProcess)==IAERROR_SUCCESS)
		{
			if (cAnalysisData.LoadFromDocument(cStrPeakProcess)==IAERROR_SUCCESS)
			{ 
				pSample->m_RealTime.RealTimeProcess(&cAnalysisData, xSrc, ySrc, nSrc, pSample->m_cPeakList, pSample->m_cAllRealTimePeakList);
				
				if(pSample->m_cPeakList.m_cArrayData.GetCount() > 0)
				{ //当实时积分积出新的峰后则刷新画布，这里需要得到显示的配置参数

					//RedrawPeak(pSample, CBase_SampleChart::FLAGMASK_BASELINE|CBase_SampleChart::FLAG_STRINGS_RESERVEDTIME, CBase_SampleChart::MODE_APPENDONLY, false);

					RedrawPeakOnRealTime(pSample, m_drawMask, CBase_SampleChart::MODE_SETALL, false);
				}
			}
		}
	}

	if(m_RealtimeDataMonitor) 
	{
		m_RealtimeDataMonitor->OnRealtimeData(NULL, xSrc, ySrc, nSrc, xSrc, ySrc, nSrc);
	}

	if(!m_pcCurveMain) 
	{
		return FALSE;
	}

	m_pcCurveMain->AddPoints(xSrc, (double*)ySrc, nSrc);

	return TRUE;

}

BOOL CBase_SampleChart::AddRealTimeCurveDataMainWithProcess(CBase_AnalysisSample* pSample, bool isProcess, const CCurveData &curveData, double dScaleX, double dScaleY, double xOffset, double yOffset)
{
	double *x=NULL;
	double *y=NULL;
	int n = curveData.ConvertData(x, y, dScaleX, dScaleY, xOffset, yOffset);

	if(n<1) return FALSE;

	BOOL bRet = AddRealTimeCurveDataMainWithProcess(pSample, isProcess, x, y, n);
	delete []x;
	delete []y;

	return bRet;
}

void CBase_SampleChart::AddMarkLine(int bStart)
{
// 	if(!m_pcCurveMain) return;
// 	unsigned c = m_pcCurveMain->GetPointsCount();
// 	ASSERT(c > 0);
// 	if(c > 0)
// 	{
// 		double  x = m_pcCurveMain->GetXPointValue(c-1);
// 		double  y = m_pcCurveMain->GetYPointValue(c-1);
// 		switch(bStart)
// 		{
// 		case 1:
// 			ASSERT(m_pStartSerie);
// 			if(m_pStartSerie)
// 			{
// 				m_pStartSerie->ClearSerie();
// 				m_pStartSerie->AddPoint(x, y-2);
// 				m_pStartSerie->AddPoint(x, y+2);
// 			}
// 			break;
// 		case 2:
// 			ASSERT(m_pStopSerie);
// 			if(m_pStopSerie)
// 			{
// 				m_pStopSerie->ClearSerie();
// 				m_pStopSerie->AddPoint(x, y-2);
// 				m_pStopSerie->AddPoint(x, y+2);
// 			}
// 			break;
// 		default:
// 			ASSERT(false);
// 		}
// 	}
}