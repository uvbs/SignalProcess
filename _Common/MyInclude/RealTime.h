#pragma once
#include "CSVFile.h"
#include "Peak.h"
#include <deque>



class CPeakList;
class CRealTimeSema;

class CRealTime
{
public:
	CRealTime(void);
	CRealTime(CSuperChartCtrl*);
	~CRealTime(void);

public:
	//信号量的数据结构
	std::vector<CRealTimeSema> m_InputSema;
	std::vector<CRealTimeSema> m_OutputSema;
	std::vector<CRealTimeSema> m_RTTotalAvgSema; //总的平均化信号量
	std::vector<CRealTimeSema> m_RTTotalSema;    //总的信号量
	// 输出结果
	int m_prevprocess_peak_nums;
	std::vector<CRealTimeSema> m_RTPeak;   //结果容器
	std::vector<CPeak> m_Peak;			   //峰的容器

	//寻峰需要的变量
	std::vector<double> m_firstOrder;	//斜率存放容器
	double m_maxSlope;
	int m_id;//静态（m_firstOrder下标）

	//传入参数
	bool m_isRealTimeParametersSet; //积分传参完毕标志位
	int m_InputNums; //进样量，每50个点处理一次
	bool m_IsDividePeak; //是否开启垂直分峰
	bool m_HasNagetiveSpike; //是否有负峰情况
	int m_Avgs; //内部平均点数.默认5
	int m_Series; //内部连续点判断。默认3
	double m_Threshold; //门限斜率
	double m_HalfPeakWidth; //最小半峰宽
	double m_PeakArea; //最小峰面积

	//把原本全局变量改成成员变量
	std::deque<CRealTimeSema> dequetmpSema;// 全局静态队列作为平均化缓存
	std::deque<CRealTimeSema> dequeBufSema;// 全局静态队列作为寻峰缓存


public:
	// !-----------------------2013.04.19 -------------------------------
	// !-----------------------实时曲线积分功能接入工作站--------------------
	//对外接口
	//VOID CRealTime::RealTimeProcess(CAnalysisData* pSample_Parameter_PeakProcess, 
	//					const CCurveData &curveData, 
	//					CPeakList &cPeakList);

	VOID CRealTime::RealTimeProcess(CAnalysisData* pSample_Parameter_PeakProcess, 
						double xSrc[], 
						double ySrc[], 
						int nSrc, 
						CPeakList &cPeakList,
						CPeakList &cAllPeakList);

	BOOL CRealTime::addRealTimePoints(const CCurveData &curveData); //准备废弃
	VOID CRealTime::RT_AddRealTimePoints(double xSrc[], double ySrc[], int nSrc);
	VOID CRealTime::RT_SetProcessParameter(CAnalysisData* pSample_Parameter_PeakProcess);
	VOID CRealTime::RT_SetProcessParameter(bool,bool,int,int,double,double,double);
	VOID CRealTime::RT_Averaging();
	VOID CRealTime::RT_AvgDataSlope();
	VOID CRealTime::RT_PeakSearch();
	VOID CRealTime::RT_PeakReturn();

	VOID CRealTime::RT_RealTimeSema2Peak(); //+ 把结果容器中数据结构转换
	VOID CRealTime::RT_GetPeakArea(); //+
	VOID CRealTime::RT_GetPeakHeight(); //+
	VOID CRealTime::RT_Peak2PeakList(CPeakList &cPeakList, CPeakList &cAllPeakList); //+
	VOID CRealTime::RT_Save2PeakList(CPeakList &cPeakList);

	// !-----------------------2010.12.09 -------------------------------
	VOID SetRealTimeData(int begin, int end);
	VOID ClearRealTimeCurve(); // 清空实时曲线
	VOID DrawRealTimeCurve(); //画实时曲线
	VOID DrawRealTimeResult(); //画最后结果（线，点）
	VOID SetSemaDate(CString filePathName); //获得待处理信号量数据
	VOID SetSemaDate(double x[], double y[], int n);
	VOID ClearSemaDate(); //清空信号量数据
	VOID SetParameter(bool, bool, double, double, double, int, int);  //设置寻峰参数
	VOID SetChartCtrl(CSuperChartCtrl* p);  //设置绘图控件指针
	VOID SetShowHide(const bool g1, const bool g2); //设置控件分组隐藏显示 
	VOID GetShowHide(bool &g1, bool &g2); //获得控件分组隐藏显示状态
	VOID SignalProc(); //接口信号处理部分
	VOID SignalDisplay(); //接口信号显示部分
	VOID GetPeakResult(std::vector<CPeak> &Peak);
	VOID ClearLine();  //清除上次绘图的东西
	VOID DrawBaseLine();  //画基线
	VOID DrawDividePeakBaseLine();  //画垂直分峰的基线
	VOID DrawResultLine(); //画最后结果（线，点）
	VOID DrawCurve();  //画原来曲线

	// !-----------------------------------------------------------------
	// !
	// ! 脚手架程序测试修改后接口的功能
	// !
	// !-----------------------2010.11.04 -------------------------------
	VOID ClearBaseLine();				// !(new)清除所有基线
	VOID ClearStartFinishPoints();	    // !(new)清除起点，落点，波峰点
	VOID ClearOrigCurve();				// !(new)清空原曲线
	VOID SetBaseLineColor(COLORREF cr); // !(new)设置当前和之后基线的颜色
	// !-----------------------2010.11.05 -------------------------------
	//VOID DisplayBaseLine();				// !(new)显示基线
	//VOID HideBaseLine();				// !(new)隐藏基线
	//VOID DisplayStartFinishPoints();	// !(new)显示起落点
	//VOID HideStartFinishPoints();		// !(new)隐藏起落点
	//VOID DisplayOrigCurve();			// !(new)显示元曲线
	//VOID HideOrigCurve();				// !(new)隐藏元曲线

	////////////////////////////////////////////////////////////////////////
	//计数器
	int counter;
	//基线颜色
	COLORREF m_BaseLineColor;
	std::vector<CSema> m_OrigSema; //原始数据
	std::vector<CSema> m_TagPoint; //基线点的上下标线

	//控件坐标轴范围
	double dXMin;
	double dXMax;
	double dYMin;
	double dYMax;

	//控件
	CSuperChartCtrl* m_pCtrl; 

	//控件内画线
	std::vector<CChartLineSerie*> m_phemline;  //曲线底边线
	std::list<CChartLineSerie*> m_pTagline;    //标记线
	std::list<CChartLineSerie*> m_pDividePeakBaseLine;    //垂直分峰后连接线
	CChartPointsSerie *m_pPoint; //波起始点和终点
	CChartPointsSerie *m_pPeak;//波起始点和终点
	CChartLineSerie* m_pDrawCurve; //元曲线 
	CChartLineSerie* m_pLinePeakSeek;//寻峰后曲线 

	////控件内分组
	//static int GROUPID_FIRST;
	//static int GROUPID_SECOND;
	//static int GROUPID_THREE; // !new
	//bool m_G1_Show;
	//bool m_G2_Show;
	////////////////////////////////////////////////////////////////////////
};