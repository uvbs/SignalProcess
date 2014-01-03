// SignalProcessDlg.h : 头文件
#pragma once
#include "afxwin.h"
#include <vector>
#include <list>
#include <utility>
#include "afxcmn.h"
#include "Peak.h"
#include "PeakSeak.h"
//#include "RealTime.h"
//#include "EditListCtrl.h"

// CSignalProcessDlg 对话框
class CSignalProcessDlg : public CDialog
{
// 构造
	enum SignalProStatus 
	{ 
		LOADFILE = 0,
		FILTER = 1,
		PEAKSEEK = 2,
	};
public:
	CSignalProcessDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSignalProcessDlg();

// 对话框数据
	enum { IDD = IDD_SIGNALPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoadCsvFile();
	bool CStringToDouble(CStringArray &DataArr); //字符转换成double型
	void vect2pointer(const std::vector<std::pair<double,double> > &Data); //vector数据转存入指针
	void GetPeakValue(double x[], double y[], int n, std::vector<std::pair<double,double> > &PeakData); //获得峰值

protected:
	SignalProStatus m_SignalProcessStatus;

private:
//----------------------------------------------------------------
// 新增CPeak类作为信号量类型
	std::vector<CSema> m_OrigSema; //原始数据
	std::vector<CPeak> m_Peak;     //峰的容器
	std::vector<CSema> m_TagPoint; //基线点的上下标线
//----------------------------------------------------------------

	
	CSuperChartCtrl m_ChcarCtrlPeakSeek; 
	CString m_CsvFilePathName;

	CStringArray m_data;
	CChartLineSerie* m_pLine;				   //原始波曲线
	std::vector<CChartLineSerie*> m_phemline;  //曲线底边线
	std::list<CChartLineSerie*> m_pTagline;    //标记线
	std::list<CChartLineSerie*> m_pDividePeakBaseLine;    //垂直分峰后连接线
	CChartPointsSerie *m_pPoint; //波起始点和终点
	CChartPointsSerie *m_pPeak;//波起始点和终点
	CChartLineSerie* m_pLineFilter;  //滤波后曲线
	CChartLineSerie* m_pLinePeakSeek;//寻峰后曲线
	double *pX; //原始数据
	double *pY;

	std::vector<std::pair<double,double> > m_OriginalData; //原始数据
	std::vector<std::pair<double,double> > m_ReshapeData;  //整形数据
	std::vector<std::pair<double,double> > m_WaveletData;  //小波数据
	std::vector<std::pair<double,double> > m_PeakData;     //峰数据

	double *pDWTX; //小波后数据
	double *pDWTY;
	double dXMinVal;
	double dXMaxVal;
	double dYMinVal;
	double dYMaxVal;

	CRect m_rcChcarCtrl;          //1原始图控件矩形
	//CRect m_rcChcarCtrlFilter;	  //2滤波图控件矩形
	//CRect m_rcChcarCtrlPeakSeek;  //3寻峰图控件矩形
	

public:
	afx_msg void OnBnClickedFilterButton();
	afx_msg void OnBnClickedPeakseekButton();

private:
	CEdit m_MovingAvgTimes;

/*
   2010.05.26测试小波算法用
*/
public:
	double *m_pdSaveArray;

	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSignalprocess();
	afx_msg void OnEnChangeMovingaveragetimes();
	afx_msg void OnRClickDatareport(NMHDR *pNMHDR, LRESULT *pResult); //报表控件右击
	afx_msg void OnDblclkDatareport(NMHDR *pNMHDR, LRESULT *pResult); //报表控件双击
private:
	CEdit m_HalfPeakWidth;
	CEdit m_Slope;
	CEdit m_SlopeFactor;
	CEdit m_PeakArea;
	CEdit m_RunTime;
	CListCtrl m_Report;  
	CListCtrl m_Config;  //配置参数列表
	CComboBox m_avgN;	 //内部几点平均，滤除杂波用
	CComboBox m_Series;  //连续几点判断，处理平头峰


public:
	void ClearLineOn3rdCtrl();
	void DrawLineOn3rdCtrl();
	void DrawBaseLineOn3rdCtrl(bool isDividePeak);
	void DrawDividePeakBaseLine(bool isDividePeak);
	void DrawTagLineOn3rdCtrl();
	void OutputReport();
	void OutputConfigArgs();// !new
	void OutputResultConfigArgs(); //new!
	void AddStringsOn3rdCtrl();

public:
	VOID AddNewPeak();	   //参数增峰
	VOID DelOnePeak();	   //参数删峰
	VOID SaveConfigParameter(CString &opt, const Para &para); //保存配置参数
	VOID LoadConfigParameter(/*CString &opt, const Para &para*/); //读入配置参数
	int  GetMoreAvgPara(); //设置更多内部平均点数
	int m_OptTimes;  //配置参数的操作次数
	int m_iavgN;


//接口封装测试
	CWnd hControl; //当前窗口内绘图控件句柄
	CPeakSeak m_PeakSeak;
	CPeakDraw m_PeakDraw;
	//CRealTime m_RealTime;

//配置参数结构
	deque<ConfigArgument> m_ConfigArgs;

public:
	afx_msg void OnBnClickedStatrfinal();
	afx_msg void OnBnClickedStatrfinal2();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeMoreAvgPara();
	//afx_msg void OnBnClickedFunctest();
	//afx_msg void OnBnClickedDateLoadTest();
	afx_msg void OnBnClickedReadshow();
	//afx_msg void OnBnClickedDeltest();
	//afx_msg void OnBnClickedColtest();
	//afx_msg void OnBnClickedShowhidetest();
	//afx_msg void OnRealTimeTest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnConfigArgsTest();

	
	afx_msg void OnConfigArgsSelchange();
	
	
	afx_msg void OnPreprocessConfigArgs();
	afx_msg void OnFirstScan();
	afx_msg void OnSecondScan();
	afx_msg void OnThirdScan();
	afx_msg void OnFourthScan();
	
	afx_msg void OnFifthScan();
	afx_msg void OnSixthScan();
	afx_msg void OnCbnDropdownSeries();
	afx_msg void OnBnClickedClearResult();
	afx_msg void OnBnClickedProcessPort1();
	
	afx_msg void OnBnClickedPreprocess();
};
