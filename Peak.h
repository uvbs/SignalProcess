#pragma once
#include "ConfigArgument.h"

//#include "Common\MyInclude\Base_SampleChart.h"
//#include "Common\MyInclude\Base_AnalysisSample.h"

#include <deque>
using std::deque;

class CCurveData;
class CPeakList;



struct  Para
{
	int pn;    //波峰编号
	double st; //起点时间
	double ft; //终点时间
	Para(double s, double f) : pn(0), st(s), ft(f) {  }
	Para(int n, double s, double f) : pn(n), st(s), ft(f) {  }
};

class CSema {
public:
	unsigned  idx;  //该点信号在整个信号量中的索引
	double x;		//时间变量，x轴					
	double y;		//信号量，y轴					
public:
	CSema();
	CSema(int index, double time, double val);
	bool operator<(const CSema &s);
	bool operator==(const CSema &rhs);
	bool operator!=(const CSema &rhs);
	virtual ~CSema();
};

// ===================================================================
// --2010.12.09--
// 新增一个实时点，记录每个送样点的当前状态（起点，上升，波峰点，波谷点，下降，终点）
class CRealTimeSema : public CSema
{
public: 
	enum RealTimeStates{
		//未知状态
		UNKNOWN,		  //初始化
		HANDLED,          //已处理，但无法判断处于何种状态
		//正峰情况
		PLUS_START_POINT, //起点
		PLUS_RAISE_POINT, //上升
		PLUS_PEAK_POINT,  //波峰点
		PLUS_DROP_POINT,  //下降
		PLUS_FINISH_POINT, //终点
		PLUS_CONTINUOUS_WAVE, //连续峰
		//负峰情况
		MINUS_START_POINT, //起点
		MINUS_DROP_POINT,  //下降
		MINUS_TROUGH_POINT,//波谷点
		MINUS_RAISE_POINT, //上升
		MINUS_FINISH_POINT, //终点
		MINUS_CONTINUOUS_WAVE //连续峰
	};
	CRealTimeSema(int index, double time, double val);
	CRealTimeSema(int index, double time, double val, RealTimeStates rts);
	CRealTimeSema(const CSema& sema);

public:
	RealTimeStates pts; //该点的当前状态
}; 

// --------------------------------------------------------------------------
class CPeak
{
public:
	enum DivideType {
		PEAKVALLEY, //峰谷基线（初始化）
		VERTDIVIDE, //垂直分峰
		PLUSTANGET, //正切分峰
		MINUSTANGET //反切分峰
	};
	union CPeakType 
	{
		CPeakType() : all(0){ }
		struct {
			unsigned short negative:			1;	//负峰
			unsigned short single :				1;	//单峰
			unsigned short overlap_pl:			1;	//重叠峰
			unsigned short overlap_ls:			1;	//重叠峰(左肩)
			unsigned short overlap_rs:			1;	//重叠峰(右肩)
			unsigned short overlap_vs:			1;	//重叠峰(垂直分峰)
			unsigned short tail:				1;	//拖尾
			//------------------------
			unsigned short reserve2future:		9;	//以后用				
		};
		unsigned short all;
	
	//---------------------------
	void CleanMark(void){ all=0; }
	//获得峰类型的文字描述
	CString ConvertToTypeName(void); 
	void	GetFromTypeName(CString cStrName);
	};
public: 
	CPeakType PeakType;		//峰类型,跟CPeakStandard匹配
	int    Type;			//峰类型,1=波峰; -1=波谷; >1重叠波峰; <-1重叠波谷
	DivideType dType;		//重叠峰分峰类型
	CSema  StartSema;		//峰起点
	CSema  PeakSema;		//峰，谷
	CSema  FinSema;			//峰落点
	double PeakWidth;		//峰宽
	double PeakHeight;		//峰高
	CSema  MidStartSema;	//半峰起点
	CSema  MidPeakSema;		//半峰，谷
	CSema  MidFinSema;		//半峰落点
	double HalfPeakWidth;	//半峰宽
	double PeakArea;		//峰面积
	double Disfactor;		//畸变因子
	double Tailing;			//拖尾因子 
	double PeakAreaPercent; //峰面积%
	double PeakHighPercent; //峰高%
	double ColumnPlate;     //理论塔板数
	double Resolution;      //分离度
public:
	CPeak(void);
	CPeak(CSema startSema, CSema peakSema, CSema finSema);
	CPeak(int sgn,CSema startSema, CSema peakSema, CSema finSema);
	CPeak(int sgn, CSema startSema, CSema peakSema, CSema finSema, double pw, double ph,
			CSema MidStartSema, CSema MidPeakSema, CSema MidFinSema,double hpw, double pa, double df, double tg);
	bool operator==(const CPeak &rhs);
	bool operator==(int value);
	bool operator!=(const CPeak &rhs);
	bool operator< (const CPeak &rhs) const;
	CPeak& operator= (const CPeak& rhs);
	~CPeak(void);
};

//----------------------------------------------------------------------------------------------------------------------
// 寻峰用的一系列算法
//----------------------------------------------------------------------------------------------------------------------
void new_Averaging(const std::vector<CSema> &handleSema, std::vector<CSema> &avgData, unsigned avg);	
void new_GetAvgDataSlope(const std::vector<CSema> &avgData, std::vector<double> &firstOrder, double &maxSlope);										         
void new_GetpeakSearch(const std::vector<CSema> &avgData, const std::vector<double> &firstOrder, std::vector<CPeak> &Peak,   
						const double slopeBased, bool hasNagetiveSpike, bool isTwiceJudge); 
void new_RestorePeakData(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak,const int avg, const double slopeBased); 
void new_DividePeak(std::vector<CPeak> &Peak);
void new_BaselineAdjust(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak,bool hasNagetiveSpike);
void new_BaselineCheak(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak,bool);
void new_PeakTypeChange(std::vector<CPeak> &Peak);
void new_GetPeakWidth(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak,bool);
void new_GetPeakArea(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak);
void new_ErasePeakByHalfWidth(std::vector<CPeak> &Peak, const double halfPeakWidth);
void new_ErasePeakByPeakArea(std::vector<CPeak> &Peak,const double peakArea);
void new_GetTagPoint(const std::vector<CPeak> &Peak, std::vector<CSema> &TagPoint);

//----------------------------------------------------------------------------------------------------------------------
// 测试用的算法
//----------------------------------------------------------------------------------------------------------------------
VOID test_GetDividePeakPoint(std::vector<CPeak> &Peak, unsigned bg, unsigned ed);
VOID test_DividePeak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak);
VOID test_ErasePeakByNextto(std::vector<CPeak> &Peak);
VOID new_GetPeakWidth_v2(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak,bool);
VOID test_GetpeakSearch(const std::vector<CSema> &avgData, const std::vector<double> &firstOrder, std::vector<CPeak> &Peak,   
						const double slopeBased, bool hasNagetiveSpike, bool isTwiceJudge);
VOID test_GetpeakSearch_Series(const std::vector<CSema> &avgData,   //平均化好的信号量
							   const std::vector<double> &firstOrder,//平均化好的信号量的斜率
							   std::vector<CPeak> &Peak,    //(输出)起点容器
							   double slopeBased, //门限斜率
							   bool hasNagetiveSpike,//输入变量，考虑是否有负峰情况
							   const int series);
//----------------------------------------------------------------------------------------------------------------------
// 追加3项新功能  
// 1. 基线调整
// 2. 给定参数删除峰
// 3. 给定参数增加峰
//----------------------------------------------------------------------------------------------------------------------
int  BinarySearch(const std::vector<CSema> &handleSema, double x); //二分查找算法
VOID one_BaselineCheak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, const int PeakNo, bool hasNagetiveSpike);
int  one_GetPeakWidth( const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, const int PeakNo, bool isDividePeak);
VOID one_GetPeakArea(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak, const int PeakNo);
VOID neo_BaseLineAdj(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, Para& p); // 1. 基线调整
VOID neo_DelOnePeak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, Para& p); // 2. 给定参数删除峰
VOID neo_AddNewPeak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, Para& p);	// 3. 给定参数增加峰

//----------------------------------------------------------------------------------------------------------------------
// 增加实时功能
// 更改寻峰接口，功能为对新增点的状态判定

void RealTime_Averaging(const std::vector<CRealTimeSema> &InputSema, //传入:的信号量
						std::vector<CRealTimeSema> &OutputSema,		//传出:平均化好的信号量
						std::vector<CRealTimeSema> &TotalAvgSema,   //总的平均化好的信号量
						std::vector<CRealTimeSema> &TotalSema,      //总的信号量
						unsigned avg );	//平均次数

void RealTime_GetAvgDataSlope(  std::vector<CRealTimeSema> &InputSema, //平均化好的信号量
								std::vector<CRealTimeSema> &TotalAvgSema,
								std::vector<double> &firstOrder,	//斜率存放容器
								double &maxSlope);					//最大斜率 

void RealTime_GetpeakSearch(std::vector<CRealTimeSema> &InputSema, //平均化好的信号量
	const std::vector<double> &firstOrder,		//平均化好的信号量的斜率
	std::vector<CRealTimeSema> &TotAvgSema,
	std::vector<CRealTimeSema> &RealTimeSema,   //(输出)起点容器
	const double slopeBased, bool hasNagetiveSpike, bool isTwiceJudge);

void RealTime_GetpeakSearch_v2(std::vector<CRealTimeSema> &InputSema, //平均化好的信号量
	const std::vector<double> &firstOrder,		//平均化好的信号量的斜率
	std::vector<CRealTimeSema> &TotAvgSema,
	std::vector<CRealTimeSema> &RealTimeSema,   //(输出)起点容器
	const double slopeBased, bool hasNagetiveSpike, bool isTwiceJudge,
	const int series);

void RealTime_RestorePeakData(std::vector<CRealTimeSema> &InputSema, //最初传入的信号量，传出结果
	std::vector<CRealTimeSema> &TotalSema,      //总的信号量
	std::vector<CRealTimeSema> &RealTimeSema,   //起点波峰波谷终点容器
	const int avg,
	const double slopeBased);  

//----------------------------------------------------------------------------------------------------------------------
// 增加配置参数算法功能   
//----------------------------------------------------------------------------------------------------------------------
bool TwoLineIsIntersect(double x0, double y0, 
	double x1, double y1, 
	double x2, double y2, 
	double x3, double y3/*, float &InterX, float &InterY*/);

//
// 第一次扫描用
//
VOID CA_GetMinMaxSlope(const std::vector<CSema> &avgData,
	const std::vector<double> &firstOrder,
	double &MinSlope, 
	double &MaxSlope,
	int N);


VOID CA_GetpeakSearch_Series(const std::vector<CSema> &avgData,   //平均化好的信号量
	const std::vector<double> &firstOrder,//平均化好的信号量的斜率
	std::vector<CPeak> &Peak,    //(输出)起点容器
	std::vector<double> &slopeBased, //！门限斜率容器
	std::deque<bool> &hasNagetiveSpike,//输入变量，考虑是否有负峰情况
	const int series);

VOID CA_RestorePeakData(const std::vector<CSema> &handleSema, //待还原的信号量
	std::vector<CPeak> &Peak,
	const int avg,
	const std::vector<double> &slopeBased); //门限斜率容器) 

//
//第二次扫描用
//
VOID CA_BanPeak(std::vector<CPeak> &Peak,
	vector<unsigned> &BanPeakNo);

VOID CA_GetHalfPeakWidth(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak);

VOID CA_GetAllHPW(std::vector<CPeak> &Peak, CArray<double> &cArray);

VOID CA_EraseByHalfPeakWidth(std::vector<CPeak> &Peak,
	vector<double> &MinHalfWidth);

//
//第三次扫描用
//
VOID CA_TangentDividPeak(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	vector<AnalysisConfigArguments::PeakSepaArg> &dequeIdx);

VOID CA_DividePeak_legacy(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<AnalysisConfigArguments::PeakSepaArg> &PeakSepaArgs);

VOID CA_DividePeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<AnalysisConfigArguments::PeakSepaArg> &PeakSepaArgs);
//
//第四次扫描用
//
bool CA_HandPeakSpration(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	const int index,      //分峰下标
	const double Spra_t); //分峰时刻

 //中止！   VOID CA_HandAddPeak(const std::vector<CSema> &handleSema,
 //中止！	std::vector<CPeak> &Peak,
 //中止！	vector<pair<double, double> >&addPeakArgs);  
VOID CA_HandAdjustStart(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<double> &Arg);

VOID CA_HandAdjustFinish(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<double> &Arg);

VOID CA_HandAdjustPeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<double> &Arg);

VOID CA_MergePeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<pair<double,double> > &Arg,
	AnalysisConfigArguments* pCA);

VOID CA_AddPeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	deque<ConfigArgument> &AddArgs);
//
//第五次扫描用
//
VOID CA_GetVertDivideBaseLineArgs(const std::vector<CPeak> &Peak,
	deque<pair<unsigned, unsigned> > &Arg); //throw

VOID CA_GetBaseLineArgs(CPeak::DivideType divType,
	const std::vector<CPeak> &Peak,
	deque<pair<unsigned, unsigned> > &Arg);

VOID CA_VertDivideBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end);

BOOL GetLeftPoint(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end,
	unsigned LeftIdx);

BOOL GetRightPoint(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end,
	unsigned RightIdx);

VOID CA_NewPeakValleyBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end);

VOID CA_PeakValleyBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end);

VOID CA_PlusTangetBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end);

VOID CA_MinusTangetBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end);

//对比峰谷比系数
VOID CA_CheckPeakToValley(
	std::vector<CPeak> &Peak,
	const double p2vRatio,
	deque<pair<unsigned, unsigned> > &DividePeakId);


//
//第六次扫描用
//
VOID CA_GetOtherPeakArgs(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak);

VOID CA_GetPeakArea(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak);

VOID CA_GetAllMPA(std::vector<CPeak> &Peak, CArray<double> &cArray);
VOID CA_GetAllMH(std::vector<CPeak> &Peak, CArray<double> &cArray);

VOID CA_EraseByPeakArea(std::vector<CPeak> &Peak,
	vector<double> &PeakArea);

VOID CA_EraseByPeakHeight(std::vector<CPeak> &Peak,
	vector<double> &PeakHeight);

VOID CA_PeakTypeChange(std::vector<CPeak> &Peak);

//add
VOID CA_GetPeakPercent(std::vector<CPeak> &Peak);

//add 获得垂直分峰上的垂线
//VOID CA_GetValleyLine(const CCurveData &curveData, const CPeakList &peakList,  CArrayPALine &PALineResult);
	
VOID CA_SetPeakType(std::vector<CPeak> &Peak);