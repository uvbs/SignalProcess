#pragma once
#include "CSVFile.h"
#include "Peak.h"
#include "ConfigArgument.h"
#include "AnalysisData.h"
#include "Base_AnalysisSample.h"

const double TAILING = 1.000000; //拖尾因子，判别峰类型是否带/拖尾

class CPeakList;

class CPeakSeak 
{
public:
	//自定义存放信号量的数据结构
	std::vector<CSema> m_OrigSema; //原始数据
	std::vector<CSema> m_OrigSema_backup; // 移动平均前的原始数据
	std::vector<CPeak> m_Peak;     //峰的容器
	//std::vector<CSema> m_TagPoint; //基线点的上下标线

	//配置参数结构
	AnalysisConfigArguments m_AnalysisConfigArg;

	//传入参数
	bool m_IsDividePeak; //是否开启垂直分峰
	bool m_HasNagetiveSpike; //是否有负峰情况
	int m_Avgs; //内部平均点数.默认5
	int m_Series; //内部连续点判断。默认3
	double m_Threshold; //门限斜率
	double m_thresholdFactor; //门限因子
	double m_HalfPeakWidth; //最小半峰宽
	double m_PeakArea; //最小峰面积
	double m_PeakHeight; //最小峰高

	double m_Magnification; //按仪器类型的放大倍数

	DWORD GetPeakCount() const { return static_cast<DWORD>(m_Peak.size()); }//获取峰个数
	void AutoSetPeakArea(DWORD dwSmallPeakCount){ASSERT(false);}//需要去除dwSmallPeakCount个小峰
	DWORD GetPeakAreaCount(double dblPeakArea){ASSERT(false);return 0;}//获取面积以dblPeakArea为分界的小峰个数

	//控件坐标轴范围
	double dXMin;
	double dXMax;
	double dYMin;
	double dYMax;

	//门限斜率的最值
	double m_dMaxSlope; 
	double m_dMinSlope;

	//峰处理过程中需要的中间变量
	CArray<double> m_cArrayHPW; //最小半峰宽（改：在进行最小半峰宽删峰之后）
	CArray<double> m_cArrayMPA; //最小峰面积（改：在进行最小峰面积删峰之后）
	CArray<double> m_cArrayMH; //最小峰高（改：在进行最小峰高删峰之后）

	int m_MinHalfPeakWidthEarseNum;
	int m_MinPeakAreaEarseNum;
	int m_MinPeakHeightEarseNum;

	typedef CBase_AnalysisSample::structLine structLine;
	CArray<structLine> m_cArrayVLine; //垂直分峰线
	
public:
	CPeakSeak(void);
	~CPeakSeak(void);
	CPeakSeak& operator= (const CPeakSeak& rhs);

	//积分接口
	BOOL SignalProcess(CAnalysisData* pSample_Parameter_PeakProcess, 
		 CPeakList &cPeakList, 
		 CArray<double> *pcArrayHPW=NULL, 
		 CArray<double> *pcArrayMPA=NULL,
		 CArray<double> *pcArrayMH=NULL,
		 int *pMinHalfPeakWidthEarseNum=NULL,
		 int *pMinPeakAreaEarseNum=NULL,
		 int *pMinPeakHeightEarseNum=NULL,
		 CArray<structLine> *pcArrayVLine=NULL,
		 double *pdMinSlope=NULL,
		 double *pdMaxSlope=NULL); 


	VOID SetSemaDate(CString filePathName); //传入待处理信号量数据
	VOID SetSemaDate(double x[], double y[], int n);
	VOID SetSemaDate(const CChartPointsArray& vPoints);
	VOID SetSemaDate(const CChartSerie* pSerie);
	VOID ClearSemaDate(); //清空信号量数据
	VOID SetParameter(bool isDividePeak, bool hasNagetiveSpike, double threshold, double thresholdFactor,
			double halfPeakWidth, double peakArea, double peakHeight, int series=3, int avgs=5);  //设置寻峰参数
	//VOID SetNoChangeCoordinate(); //设置原来的坐标范围;  new！
	VOID GetPeakResult(std::vector<CPeak> &Peak);

	// !-----------------------2010.12.17 ----------------------------------
	// !-----------------带配置参数的峰处理算法测试---------------------------
	VOID ConfigArgs_SignalProc();
	VOID ConfigArgs_FirstScan();
	VOID ConfigArgs_SecondScan();
	VOID ConfigArgs_ThirdScan();
	VOID ConfigArgs_FourthScan();
	VOID ConfigArgs_FifthScan();
	VOID ConfigArgs_SixthScan();

	VOID GetPeakTypeStr(CString &str);

public:
	void SP_FirstScan(); 
	void SP_SecondScan(); 
	void SP_ThirdScan(); 
	void SP_FourthScan(); 
	void SP_FifthScan(); 
	void SP_SixthScan(); 
private:
	//第1次扫描 
	void CPeakSeak::SP_MovingAverage(vector<CSema> &OrigData, int N);
	void CPeakSeak::SP_Averaging(std::vector<CSema> &avgData);
	void CPeakSeak::SP_GetAvgDataSlope(const vector<CSema> &avgData, vector<double> &firstOrder, double &maxSlope);
	void CPeakSeak::SP_GetMinMaxSlope(/*const vector<CSema> &avgData, */const vector<double> &firstOrder,int N);
	void CPeakSeak::SP_GetpeakSearch_Series(const vector<CSema> &avgData, const vector<double> &firstOrder, vector<double> &slopeBased, deque<bool> &hasNagetiveSpike);
	void CPeakSeak::SP_GetpeakSearch_Series_plus(const vector<CSema> &avgData, const vector<double> &firstOrder, vector<double> &slopeBased, deque<bool> &hasNagetiveSpike, double thresholdFactor=0.7f);
	void CPeakSeak::SP_RestorePeakData(const vector<double> &slopeBased);
	//第2次扫描 
	void CPeakSeak::SP_BanPeak(vector<unsigned> BanPeakNo);
	void CPeakSeak::SP_PeakTypeChange();
	void CPeakSeak::SP_GetHalfPeakWidth();
	void CPeakSeak::SP_GetAllHPW();
	void CPeakSeak::SP_EraseByHalfPeakWidth(vector<double> &MinHalfWidth);
	//第3次扫描
	void CPeakSeak::SP_TangentDividPeak(vector<AnalysisConfigArguments::PeakSepaArg> &PeakSepaArgs);
	void CPeakSeak::SP_GetSeriesPeakDeque(deque<pair<unsigned, unsigned> > &seriesPeak);
	void CPeakSeak::SP_CheckPeakToValley(const double p2vRatio, deque<pair<unsigned, unsigned> > &DividePeakId);
	void CPeakSeak::SP_SinglePeakValleyBaseLineCheck(unsigned no); //单峰谷连线校正
	void CPeakSeak::SP_SingleVertDivideBaseLineCheck(unsigned no); //单垂线分峰校正
	void CPeakSeak::SP_PeakValleyBaseLineCheck(unsigned begin, unsigned end);
	void CPeakSeak::SP_DividePeak(vector<AnalysisConfigArguments::PeakSepaArg> &SepaArgs); 
	void CPeakSeak::SP_CheckOverlapPoint(); //重叠峰追加判定
	//第4次扫描
	bool CPeakSeak::SP_HandPeakSpration(const int index,const double Spra_t); //准备废弃
	void CPeakSeak::SP_SplitPeak(const int index,const double Splitime);
	void CPeakSeak::SP_SplitPeak_Left(const int index,const unsigned left);
	void CPeakSeak::SP_SplitPeak_Right(const int index,const unsigned right);
	void CPeakSeak::SP_HandAdjustStart (vector<pair<unsigned, double> > &Arg);
	void CPeakSeak::SP_HandAdjustFinish(vector<pair<unsigned, double> > &Arg);
	void CPeakSeak::SP_HandAdjustPeak(vector<double> &Arg);
	void CPeakSeak::SP_MergePeak(vector<pair<unsigned,unsigned> > &Arg);
	//第5次扫描
	bool CPeakSeak::SP_TwoLineIsIntersect(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);
	void CPeakSeak::SP_GetBaseLineArgs(CPeak::DivideType divType, deque<pair<unsigned, unsigned> > &Arg);
	void CPeakSeak::SP_VertDivideBaseLineCheck(unsigned begin, unsigned end);
	void CPeakSeak::SP_PlusTangetBaseLineCheck(unsigned begin, unsigned end);
	void CPeakSeak::SP_MinusTangetBaseLineCheck(unsigned begin,unsigned end);
	void CPeakSeak::SP_AddPeak(deque<ConfigArgument> &AddArgs);
	void CPeakSeak::SP_AddPeak_One(ConfigArgument &AddArg);
	//第6次扫描
	void CPeakSeak::SP_GetOtherPeakArgs();
	void CPeakSeak::SP_GetTailFactor();
	void CPeakSeak::SP_CalcResolution(); // +2013.12.27 计算分离度（//必须放在半峰宽计算之后）
	void CPeakSeak::SP_GetPeakArea();
	void CPeakSeak::SP_SetPeakAreaUnit(double factor);
	void CPeakSeak::SP_GetAllMPA();
	void CPeakSeak::SP_GetAllMH();
	void CPeakSeak::SP_EraseByPeakArea(vector<double> &PeakArea);
	void CPeakSeak::SP_EraseByPeakHeight(vector<double> &PeakHeight);
	void CPeakSeak::SP_DelPeak(deque<ConfigArgument> &delArgs);
	void CPeakSeak::SP_DelPeak(ConfigArgument &delArg);
	void CPeakSeak::SP_DelPeak_One(ConfigArgument &delArg);
	void CPeakSeak::SP_AdjustStartandEndPoint();
	void CPeakSeak::SP_GetPeakPercent();
	void CPeakSeak::SP_CheckPeakType();
	void CPeakSeak::SP_SetPeakType();
	void CPeakSeak::SP_GetDividePeakLine();
	//最后的扫描
	void CPeakSeak::SP_RestoreOriginalData(); //还原至移动平均前的数据
};



// !----------------------- 2011.04.19 -------------------------------
//CHEM2009项目不需要用到的类
class CPeakDraw 
{
public:
	CPeakDraw();

public:
	CPeakSeak m_PeakSeak;
	VOID SetCPeakSeakData(const CPeakSeak &PeakSeak);

	//计数器
	int counter;

	//控件
	CSuperChartCtrl* m_pCtrl;
	
	//基线颜色
	COLORREF m_BaseLineColor;

	//控件内画线
	std::vector<CChartLineSerie* > m_phemline;    //曲线底边线             
	std::list<CChartLineSerie* >   m_pTagline;    //标记线                 
	std::list<CChartLineSerie* >   m_pDividePeakBaseLine; //垂直分峰后连接线
	CChartPointsSerie* m_pPoint;             //波起始点和终点                    
	CChartPointsSerie* m_pPeak;              //波起始点和终点                    
	CChartLineSerie*   m_pDrawCurve;		 //元曲线                         
	CChartLineSerie*   m_pLinePeakSeek;	     //寻峰后曲线                      

	//控件内分组
	static int GROUPID_FIRST;  
	static int GROUPID_SECOND; 
	static int GROUPID_THREE;   

public:
	VOID SignalDisplay(); //接口信号显示部分
	VOID DrawBaseLine();  //画基线
	VOID DrawDividePeakBaseLine();  //画垂直分峰的基线
	VOID CA_SignalDisplay();
	VOID SetChartCtrl(CSuperChartCtrl* p);  //设置绘图控件指针
	VOID ClearLine();  //清除上次绘图的东西
	VOID DrawCurve();  //画原来曲线
	VOID CA_DrawBaseLine();
	VOID DrawResultLine(); //画最后结果（线，点）
	
};