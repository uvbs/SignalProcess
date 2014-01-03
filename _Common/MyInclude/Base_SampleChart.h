
#pragma once

#include "MovingAverage.h"



/*
//---------------------------采样数据定义--------------------------------
//
//		色谱中的定义：时间（毫秒）+ 采样值（微伏）
//
//-----------------------------------------------------------------------
class CHRO_PIC_DATA {
public:
	CHRO_PIC_DATA():dwNo(0),lVal(0){}
	~CHRO_PIC_DATA(){}

	void operator=(const CHRO_PIC_DATA &ori) {dwNo=ori.dwNo;lVal=ori.lVal;}

	DWORD  dwNo;
	long  lVal;
};

typedef CArray<CHRO_PIC_DATA> CDataArray;
*/

//---------------------------采样数据定义--------------------------------
//
//		色谱中的定义：时间（分钟）+ 采样值（毫伏）
//
//-----------------------------------------------------------------------
class CCurveDataSingle{
public:
	CCurveDataSingle():dX(0.0),dY(0.0){}
	CCurveDataSingle(double x, double y):dX(x),dY(y){}
	~CCurveDataSingle(){}

	void operator=(const CCurveDataSingle &ori) {dX=ori.dX;dY=ori.dY;}

	double  dX;
	double  dY;
};

class CCurveData {
public:
	CCurveData(){m_arrayData.SetSize(0, 20*60*5);}//以每秒20个数据来计算，增长5分钟数据}
	~CCurveData(){}

	void operator=(const CCurveData &ori) {m_arrayData.Copy(ori.m_arrayData);}
	void RemoveAll(void) {m_arrayData.RemoveAll();}
	void AppendData(double x, double y){m_arrayData.Add(CCurveDataSingle(x,y));}
	void AppendData(const CCurveData &data){m_arrayData.Append(data.m_arrayData);}
	//调用者释放x,y指针
	int  ConvertData(double *&x, double *&y, double dScaleX, double dScaleY, double xOffset, double yOffset) const {
		x=NULL;y=NULL;
		int nNum = m_arrayData.GetCount();
		if(nNum==0) return 0;
		int validCount = 0;
		x = new double[nNum]; y= new double[nNum];
		for(int i=0; i<nNum; ++i) {
			x[validCount]=m_arrayData[i].dX*dScaleX + xOffset;
			y[validCount]=m_arrayData[i].dY*dScaleY + yOffset;
			if(x[validCount] < 0) continue;
			++validCount;
		}
		return validCount;
		}


	CArray<CCurveDataSingle> m_arrayData;

};

class CPALine {
public:
	CCurveDataSingle m_point[2];
	//CPALine(CCurveDataSingle oneSingle, CCurveDataSingle twoSingle) 
	//{ m_point[0] = oneSingle; m_point[1] = twoSingle; }
};

typedef CArray<CPALine>  CArrayPALine;


//------------------------------------------------------------------------

class CBase_AnalysisSample;


#define ID_MENU_EXPORT_DATA		50001


class CBase_RealtimeDataMonitor
{
public:
	virtual void OnRealtimeData(CBase_AnalysisSample* pSample, const double xSrc[], const double ySrc[], int nSrc, const double xProcessed[], const double yProcessed[], int nProcessed) = 0;
	virtual void OnStartRun(CBase_AnalysisSample* pSample) = 0;
	virtual void OnEndRun(CBase_AnalysisSample* pSample) = 0;
};


class CBase_SampleChart : public CSuperChartCtrl
{
	DECLARE_DYNAMIC(CBase_SampleChart)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnExportData();

	struct structMinMaxInfo
	{
		double minx;
		double maxx;
		double miny;
		double maxy;
	};
	static void LoadSerieFromFile(structMinMaxInfo&mmi, CChartSerie* pSerie, LPCTSTR filename);

public:
	CBase_SampleChart();
	virtual ~CBase_SampleChart();

public:
	void RefreshSample_Curve2DSeries(CBase_AnalysisSample* pSample ,const CString& dataFolder, bool bRefreshAxisRange);

	void SetRealtimeDataMonitor(CBase_RealtimeDataMonitor* pMonitor){m_RealtimeDataMonitor = pMonitor;}

protected:
	CString m_cStrMainGroup, m_cStrMainSerie;

public:
	CChartSerie* GetMainSerie(){return m_pcCurveMain;}
	const CChartSerie* GetMainSerie() const{return m_pcCurveMain;}

protected:
	CChartSerie* CreateSerie(CChartSerie::enumSerieType eType);
	CChartSerie* CreateSerie();
	CChartSerie* CreateCurve2DSerie(const CString& group, const CString& serie);
	//建立主曲线
	CChartSerie* CreateCurve2DMain(void);

	virtual enumMenu OnPrepareMenu(CMenu& defaultMenu, CMenu& customMenu);

public://数据处理绘图
	enum enumPeakFlag
	{
		FLAG_DATA_SOURCE = 0X00000001,//原始数据曲线，即主曲线
// 		FLAG_POINTS_BEGIN = 0X00000002,//峰起点
// #ifdef _DEBUG
// 		FLAG_POINTS_PEAK = 0X00000004,//峰谷,
// #else
// 		FLAG_POINTS_PEAK = 0X00000000,//峰谷,
// #endif
// 		FLAG_POINTS_END = 0X00000008,//峰终点
		FLAG_LINES_BASE = 0X00000010,//基线
		FLAG_LINES_DIVIDEBASE = 0X00000020,//垂直分峰基线
		FLAG_STRINGS_RESERVEDTIME = 0X00000040,//保留时间
		FLAG_STRINGS_CONCENTRATION = 0X00000080,//浓度
		FLAG_STRINGS_COMPONENTNAME = 0X00000100,//组份名
		FLAG_STRINGS_FMJ = 0X00000200,//峰面积
		FLAG_STRINGS_FG = 0X00000400,//峰高

		FLAGMASK_BASELINE = /*FLAG_POINTS_BEGIN | FLAG_POINTS_PEAK | FLAG_POINTS_END | */FLAG_LINES_BASE | FLAG_LINES_DIVIDEBASE,
		FLAGMASK_STRINGS_ASSISTANT = FLAG_STRINGS_RESERVEDTIME|FLAG_STRINGS_CONCENTRATION|FLAG_STRINGS_COMPONENTNAME|FLAG_STRINGS_FMJ|FLAG_STRINGS_FG,
		FLAGMASK_ALL = 0XFFFFFFFF,
	};
	enum enumMode
	{
		MODE_SETALL,
		MODE_APPENDONLY,
	};

	//仅对指定的曲线进行处理，未指定的曲线保持当前状态
	void RedrawPeak(const CBase_AnalysisSample* pSample, DWORD dwFlag, enumMode eMode = MODE_SETALL, bool bRefresh = true);
	void RedrawPeakOnRealTime(const CBase_AnalysisSample* pSample, DWORD dwFlag, enumMode eMode = MODE_SETALL, bool bRefresh = true);

	void ClearPeak(const CBase_AnalysisSample* pSample, DWORD dwFlag);
	void ShowPeak(const CBase_AnalysisSample* pSample, DWORD dwFlag);
	void HidePeak(const CBase_AnalysisSample* pSample, DWORD dwFlag);

	void SetColor(DWORD dwFlag, COLORREF color);//可以是组合
	COLORREF GetColor(enumPeakFlag eFlag) const;//枚举

	void ClearCompSeries();
	void ClearCurve2DSeries();
	void ClearMarkLines();
	void ClearTemporary();

public:
	size_t GetTemporaryGroupID() const{return m_nTemporaryGroupID;}
	CTWenChartPan* ShowRangeFlag(double fromX, double toX, COLORREF cr, size_t nGroupID = -1);//-1表示添加到临时组
	CChartFill* ShowPeakFill(const CChartPointsArray& arrPoints, COLORREF cr, size_t nGroupID = -1);//-1表示添加到临时组

protected:
	friend class CBase_AnalysisSample;
	CBase_RealtimeDataMonitor* m_RealtimeDataMonitor;
	CMovingAverage m_ma;
//	CChartPointsSerie *m_pBeginPoints; //峰起点
//	CChartPointsSerie *m_pPeakPoints;//峰谷
//	CChartPointsSerie *m_pEndPoints; //峰终点
	CArray<CChartLineSerie*, CChartLineSerie*> m_arrBaseLines;//基线
	CArray<CChartLineSerie*, CChartLineSerie*> m_arrDivideBaseLines;//垂直分峰基线
	CArray<CSuperChartString*, CSuperChartString*> m_arrAssistants;//辅助显示中的保留时间、浓度、组份名
	COLORREF m_crBaseLines, m_crDivideBaseLines, m_crAssistants;
	size_t m_nTemporaryGroupID;
	DWORD m_drawMask; //绘图显示选项标记

public:
	CMapStringToPtr m_mapCompSeries;//对比曲线,注意，一定要以(CChartSerie*)方式添加
	CMapStringToPtr m_mapCurve2DVisibilites;
	//主曲线单独列出来，便于使用
	CChartSerie*	m_pcCurveMain;
	CMapStringToPtr m_mapCurve2DSeries;//样品中的所有曲线，不包括主组的主曲线,注意，一定要以CreateCurve2DSerie方式
//	CChartSerie* m_pStartSerie;
//	CChartSerie* m_pStopSerie;
	CTWenChartPan* m_pYLabel;
	CTWenChartPan* m_pXLabel;
	CTWenChartPan* m_pYLabelBk;
	CTWenChartPan* m_pXLabelBk;


protected:
	//CBase_AnalysisSample* m_pSample;该类不需要样品指针，而是直接被样品类驱动
	CChartSerie::enumSerieType m_eSerieType;
	void OnBackColorChanged();


	//
	//-------------------接口函数------------------------------------------
	//
public:
	//如果是TRUE，表示现在是实时数据采集状态
	// 可以接受实时数据，并显示
	// 否则不接受实时数据
	//BOOL	m_bRealtimeStatus;
	
	//
	//用样品数据初始化
	//判断该样品的状态，如果是...那么，状态转为m_bRealtimeStatus=FALSE，如果...状态转为m_bRealtimeStatus=TRUE
	//void Init(CBase_AnalysisSample* pSample, CChartSerie::enumSerieType eType = CChartSerie::stLineSerie);//最好在创建之后调用
	void Init(CChartSerie::enumSerieType eType = CChartSerie::stLineSerie);//最好在创建之后调用
	void SetDrawMask(DWORD dMask);
	void ResetAxis();
	
	
	// 刷新状态，根据bRealTimeStatus转换当前的m_bRealtimeStatus
	// 如果状态反生变化，图形相应地要变化：如果bRedraw为FALSE，那么仅在状态变化进行以下操作
	// 当m_bRealtimeStatus从TRUE变成FALSE的时候，清除当前图形所有元素，重画样品数据曲线
	// 当m_bRealtimeStatus从FALSE变成TRUE的时候，清除当前图形所有元素（等待实时数据）
	// 如果bRedraw为TRUE，那么无论如何都执行以上操作
	//void UpdateStatus(BOOL bRealTimeStatus, BOOL bRedraw=FALSE);

	//添加主曲线数据
	//在 m_bRealtimeStatus从TRUE 才进行操作，否则返回FALSE
	// 操作过程：
	// 1) 将点加入曲线中
	// 2) 调用 CRealTime::SetSemaDate(double x[], double y[], int n);
	// 3) 调用 CRealTime::GetPeakResult(std::vector<CPeak> &Peak),然后将Peak数据绘制在图形上
	BOOL AddRealTimeCurveDataMain(double x[], double y[], int n); 
	BOOL AddRealTimeCurveDataMainWithProcess(CBase_AnalysisSample* pSample, bool isProcess, double x[], double y[], int n); //里面包含了 实时曲线积分 功能
	//dScale表示输入的数值需要乘以多少倍数
	//Offset表示输入的数值被Scale之后要增加多少
	BOOL AddRealTimeCurveDataMainWithProcess(CBase_AnalysisSample* pSample, bool isProcess, const CCurveData &curveData, double dScaleX, double dScaleY, double xOffset, double yOffset);
	//添加其他曲线数据
	BOOL AddRealTimeCurveData(const CString& group, const CString& serie, double x[], double y[], int n);

	//绘制开始和结束线
	//在 m_bRealtimeStatus从TRUE 才进行操作，否则返回FALSE
	// 1是开始线，2结束线，3以后暂时没定义
	// 画线坐标，起始点：主曲线最后一点的横坐标；纵坐标：最后一点的纵坐标
	void AddMarkLine(int bStart);
};


class CParamCommon
{
public:
// 	CParamCommon(void);
// 	~CParamCommon(void);
// 
// public:
// 	static const CString& ThresholdString(double dblValue);
// 	static const CString& MinHalfPeakWidthString(double dblValue);
// 	static const CString& MinPeakAreaString(double dblValue);
// 	static const CString& MinPeakHeightString(double dblValue);
	static CString TimeString(double dblValue);
//	static const CString& ConcentrationString(double dblValue);
};