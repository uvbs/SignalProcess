#pragma once

#include <string>
#include <vector>
#include <deque>
using std::string;
using std::vector;
using std::deque;
using std::pair;


class ConfigArgument  
{
public:
	CString ArgName;	    //参数名称
	double BeginTime;       //开始时间
	double EndTime;         //结束时间
	double InputValue;      //值
	ConfigArgument();
	ConfigArgument(CString argname, double begint);
	ConfigArgument(CString argname, double begint, double endt);
	ConfigArgument(CString argname, double begint, double endt, double val);
	ConfigArgument(double begint, double val);

	bool operator== (const ConfigArgument &rhs);
};

class strict_weak_ordering 
{
public:
	inline bool operator()(const std::pair<int, ConfigArgument> a, const std::pair<int, ConfigArgument> b) 
	{ 
		return a.first < b.first; 
	} 
};

class CSema;
class CPeak;
class AnalysisConfigArguments //解析峰处理配置参数
{
public:
	struct PeakSepaArg { //分峰参数
		unsigned StartPeakNo;
		unsigned FinishPeakNo;
		int SepaType; //分峰类型：0=峰谷基线，1=垂直分峰，2=正切分峰，3=反切分峰
		explicit PeakSepaArg(unsigned, unsigned, int);
	};
	//构造函数
	AnalysisConfigArguments();
	explicit AnalysisConfigArguments(double, double, double, double);

	//外部峰处理参数 --> 内部峰处理参数
	void TransformArgs(deque<ConfigArgument>& args);

	//传入配置参数
	void SetArgs(const deque<ConfigArgument>& args);

	//逐个添加配置参数
	void addArgs(const ConfigArgument& configArg);


//--------------------------------------------------------------------------------------------
//                                           预处理部分                                           
//--------------------------------------------------------------------------------------------
	//配置参数预处理
	void PreprocessConfigArgs();

	//设置分段噪声阀门参数默认值
	void SetPeakSearchArgs(int size, double defaultSlope);

	//设置分段负峰检测参数默认值
	void SetMinusCheckArgs(int size);

	//设置分段最小负峰值参数默认值
	void SetMinPeakWidthArgs(int size, double defaultVal);

	//设置分段最小峰面积参数默认值
	void SetMinPeakAreaArgs(int size, double defaultVal);

	//设置分段最小峰高参数默认值
	void SetMinPeakHeightArgs(int size, double defaultVal);
//--------------------------------------------------------------------------------------------
//                                           解析部分  
//--------------------------------------------------------------------------------------------
	//解析寻峰需要的噪声门限
	VOID Analysis_PeakSearchSlopeBased(const vector<CSema> &Data);

	//解析寻峰需要的负峰检测
	VOID Analysis_PeakSearchMinusCheck(const vector<CSema> &Data);

	//解析禁止积分需要的参数
	VOID Analysis_BanPeak(const vector<CPeak> &Peak);

	//解析最小半峰宽需要的参数
	VOID Analysis_MinHalfPeakWidth(const vector<CPeak> &Peak);

	//解析切线分峰需要的参数
	void Analysis_TangentDividPeak(const vector<CPeak> &Peak, deque<PeakSepaArg> &dequeIdx);

	//解析重叠峰处理需要的参数
	VOID Analysis_PeakSeparation(const vector<CPeak> &Peak, vector<PeakSepaArg> &psArgs);

	//解析手工分峰需要的参数
	VOID Analysis_HandPeakSeparation(const vector<CPeak> &Peak, deque<double> &HandPeakArgs);
	//解析手工分峰需要的一个参数，返回峰容器内对应的峰下标。-1表示找不到符合条件的峰下标
	int  Analysis_OneHandPeakSepar(const vector<CPeak> &Peak, double starttime);

	//解析手工调峰需要的参数，分别装入3个容器
	VOID Analysis_HandAdjustPeak(const vector<CPeak> &Peak);
	VOID Analysis_HandworkBaseLine(const vector<CPeak> &Peak); //自动判断是调整起点还是落点

	//解析合并峰需要的参数
	VOID Analysis_MergePeak(const vector<CPeak> &Peak);

	//解析分段最小峰面积
	VOID Analysis_MinPeakArea(const vector<CPeak> &Peak);

	//解析分段最小峰高
	VOID Analysis_MinPeakHeight(const vector<CPeak> &Peak);

	//解析手工封顶（增加峰）需要的参数 （2011.07.21追加）
	VOID Analysis_HandAddPeak(const vector<CPeak> &Peak);

	//解析删除峰必须参数
	VOID Analysis_HandDelPeak(const vector<CPeak> &Peak);

	//获得增加峰和删除峰的前后顺序参数
	VOID Analysis_AddelPeak(const vector<CPeak> &Peak);

	BOOL IsOverlapping(double dA, double dB, double dC, double dD);

//----------------------------------------------------------------------------------------------
//                                           返回所需的参数                                           
//----------------------------------------------------------------------------------------------
	vector<double>& GetPeakSearchArgs();
	deque<bool>& GetMinusCheckArgs();
	vector<unsigned>& GetBanPeakArgs();
	vector<double>& GetMinHalfPeakWidthArgs();
	vector<double>& GetMinPeakAreaArgs();
	vector<double>& GetMinPeakHeightArgs();
	vector<double>& GetHandAdjustPeakArgs();
	vector<pair<unsigned, double> >& GetHandAdjustStartArgs();
	vector<pair<unsigned, double> >& GetHandAdjustFinishArgs();
	vector<pair<double, double> >& GetMergePeakArgs(); //重构结束后删除
	vector<pair<unsigned, unsigned> >& GetMergePeakNo();
	deque<ConfigArgument>& GetAddPeakArgs();
	deque<ConfigArgument>& GetDelPeakArgs();
	deque<ConfigArgument>& GetAddDelPeakArgs();
	bool GetMinusCheakStatus(double point);
	deque<ConfigArgument>& GetAllArguments();
	deque<ConfigArgument>& GetResultArgument();
	double& GetDefaultPeak2Valley();
	bool IsPeak2Valley();

private:
	void Pre_PeakSearchSlopeBased(); //预处理分段门限参数
	void Pre_PeakSearchMinusCheck(); //预处理分段负峰检测
	void Pre_HalfPeakWidth();        //预处理分段最小半峰宽
	void Pre_PeakSeparation();       //预处理分峰参数
	void Pre_Peak2ValleyRatio();     //预处理峰谷比参数

public:
	deque<ConfigArgument> _ConfigArgument; //全部的配置参数列表
	deque<ConfigArgument> _ResultArgument; //输出全部正确的参数

	vector<double> _SlopeBased;	  //分段噪声阀门
	vector<unsigned> _BanPeakNo;  //分段禁止积分峰号
	deque<bool> _MinusCheck;	  //分段负峰检测
	vector<double> _MinHalfWidth; //分段最小半峰宽，容器内元素个数与波峰个数一致
	vector<double> _MinPeakArea;  //分段最小峰面积
	vector<double> _MinPeakHeight; //分段最小峰高
	vector<double> _HandPeakSpra; //手工分峰起始时间

	vector<pair<unsigned,double> > _AdjustStart;  //调整起点
	vector<double> _AdjustPeak;   //调整峰谷
	vector<pair<unsigned,double> > _AdjustFinish; //调整终点
	vector<pair<double, double> > _MergePeak; //合并峰参数
	vector<pair<unsigned, unsigned> > _MergePeakNo; //合并峰号

	deque<ConfigArgument> _AddPeak;   //增加峰参数(手工封顶)
	deque<ConfigArgument> _DelPeak;   //删除峰峰号
	deque<ConfigArgument> _AddelPeak; //增加峰和删除峰的混合数据
	
	//vector<unsigned> _DelPeak;		  //删除峰峰号

	double _defaultSlope;	  //默认门限
	double _defaultHalfWidth; //默认最小半峰宽
	double _defaultPeakArea;  //默认最小峰面积
	double _defaultPeakHeight; //默认最小峰高

//新增参数：峰谷比
	double _defaultPeak2Valley; //默认峰谷比
	
};

