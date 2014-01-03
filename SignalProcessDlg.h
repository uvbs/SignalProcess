// SignalProcessDlg.h : 头文件
#pragma once
#include <vector>
#include <list>
#include <utility>
#include "afxwin.h"
#include "afxcmn.h"
#include "Peak.h"
#include "PeakSeak.h"


// CSignalProcessDlg 对话框
class CSignalProcessDlg : public CDialog
{
public:
	CSignalProcessDlg(CWnd* pParent = NULL);
	~CSignalProcessDlg();

	enum { IDD = IDD_SIGNALPROCESS_DIALOG };

	//void ClearLineOn3rdCtrl();
	//void DrawLineOn3rdCtrl();
	//void DrawBaseLineOn3rdCtrl(bool isDividePeak);
	void DrawDividePeakBaseLine(bool isDividePeak);
	//void DrawTagLineOn3rdCtrl();
	void OutputReport();
	void OutputConfigArgs();
	void OutputResultConfigArgs(); 
	void AddStringsOn3rdCtrl();

	//VOID AddNewPeak();	   //参数增峰
	//VOID DelOnePeak();	   //参数删峰
	VOID SaveConfigParameter(CString &opt, const Para &para); //保存配置参数
	VOID LoadConfigParameter(); //读入配置参数
	int  GetMoreAvgPara(); //设置更多内部平均点数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	HICON m_hIcon;

	afx_msg void OnLoadCsvFile();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedFilterButton();
	afx_msg void OnBnClickedPeakseekButton();
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSignalprocess();
	afx_msg void OnEnChangeMovingaveragetimes();
	afx_msg void OnRClickDatareport(NMHDR *pNMHDR, LRESULT *pResult); //报表控件右击
	afx_msg void OnDblclkDatareport(NMHDR *pNMHDR, LRESULT *pResult); //报表控件双击
	afx_msg void OnBnClickedStatrfinal();
	afx_msg void OnBnClickedStatrfinal2();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeMoreAvgPara();
	afx_msg void OnBnClickedReadshow();
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

	DECLARE_MESSAGE_MAP()

public:
	bool CStringToDouble(CStringArray &DataArr); //字符转换成double型
	void vect2pointer(const std::vector<std::pair<double,double> > &Data); //vector数据转存入指针
	void GetPeakValue(double x[], double y[], int n, std::vector<std::pair<double,double> > &PeakData); //获得峰值

private:
	std::vector<CSema> m_OrigSema; //原始数据
	std::vector<CPeak> m_Peak;     //峰的容器
	std::vector<CSema> m_TagPoint; //基线点的上下标线
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

	CRect m_rcChcarCtrl; //1原始图控件矩形

	int m_OptTimes;  //配置参数的操作次数
	int m_iavgN;
	
	CEdit m_MovingAvgTimes;
	double *m_pdSaveArray;

	CEdit m_HalfPeakWidth;
	CEdit m_Slope;
	CEdit m_SlopeFactor;
	CEdit m_PeakArea;
	CEdit m_RunTime;
	CListCtrl m_Report;  
	CListCtrl m_Config;  //配置参数列表
	CComboBox m_avgN;	 //内部几点平均，滤除杂波用
	CComboBox m_Series;  //连续几点判断，处理平头峰

    //接口封装测试
	CWnd hControl; //当前窗口内绘图控件句柄
	CPeakSeak m_PeakSeak;
	CPeakDraw m_PeakDraw;

    //配置参数结构
	deque<ConfigArgument> m_ConfigArgs;
};
