// SignalProcessDlg.cpp : 实现文件
#include "stdafx.h"
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

#include "SignalProcess.h"
#include "SignalProcessDlg.h"
#include "CSVFile.h"
#include "WaveletTransform.h"
#include "SetAvgDlg.h"
#include "PeakSeak.h"
#include "ShowHide.h"
#include "ConfigArgument.h"

//#include "AnalysisData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSignalProcessDlg 对话框
CSignalProcessDlg::CSignalProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSignalProcessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pX = NULL;
	pY = NULL;
	pDWTX = NULL;
	pDWTY = NULL;
	dXMinVal = -0.01;
	dXMaxVal = 0.0;
	dYMinVal = -0.01;
	dYMaxVal = 0.0;
	m_OptTimes = 1;
	m_iavgN = 5; //默认内部平均点数为5
}

CSignalProcessDlg::~CSignalProcessDlg()
{
	if (pX!=NULL || pY!=NULL)
	{
		delete [] pX;
		delete [] pY;
	}
}

void CSignalProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MovingAverageTimes, m_MovingAvgTimes);
	DDX_Control(pDX, IDC_HALFPEAKWIDTH, m_HalfPeakWidth);
	DDX_Control(pDX, IDC_SLOPE, m_Slope);
	DDX_Control(pDX, IDC_DATAREPORT, m_Report);
	DDX_Control(pDX, IDC_COMBO1, m_avgN);
	DDX_Control(pDX, IDC_PEAKAREA, m_PeakArea);
	DDX_Control(pDX, IDC_EDIT2, m_RunTime);
	DDX_Control(pDX, IDC_SERIES, m_Series);
	DDX_Control(pDX, IDC_LIST3, m_Config);
	DDX_Control(pDX, IDC_EDIT1, m_SlopeFactor);
}

BEGIN_MESSAGE_MAP(CSignalProcessDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(NM_RCLICK, IDC_DATAREPORT, &CSignalProcessDlg::OnRClickDatareport)
	ON_NOTIFY(NM_DBLCLK, IDC_DATAREPORT, &CSignalProcessDlg::OnDblclkDatareport)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSignalProcessDlg::OnCbnSelchangeMoreAvgPara)
	ON_BN_CLICKED(IDC_READSHOW, &CSignalProcessDlg::OnBnClickedReadshow)
	ON_BN_CLICKED(IDC_FIRSTSCAN, &CSignalProcessDlg::OnFirstScan)
	ON_BN_CLICKED(IDC_SECONDSCAN, &CSignalProcessDlg::OnSecondScan)
	ON_BN_CLICKED(IDC_THIRDSCAN, &CSignalProcessDlg::OnThirdScan)
	ON_BN_CLICKED(IDC_FOURTHSCAN, &CSignalProcessDlg::OnFourthScan)
	ON_BN_CLICKED(IDC_FIFTHSCAN, &CSignalProcessDlg::OnFifthScan)
	ON_BN_CLICKED(IDC_SIXTHSCAN, &CSignalProcessDlg::OnSixthScan)
	ON_CBN_DROPDOWN(IDC_SERIES, &CSignalProcessDlg::OnCbnDropdownSeries)
	ON_BN_CLICKED(IDC_CLEARRESULT, &CSignalProcessDlg::OnBnClickedClearResult)
	ON_BN_CLICKED(IDC_PROCESSPORT1, &CSignalProcessDlg::OnBnClickedProcessPort1)
	ON_BN_CLICKED(IDC_PREPROCESS, &CSignalProcessDlg::OnBnClickedPreprocess)
END_MESSAGE_MAP()


// CSignalProcessDlg 消息处理程序
BOOL CSignalProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE); // 设置大图标
	SetIcon(m_hIcon, FALSE); // 设置小图标
	
	CRect rcPeakSeek;
	GetDlgItem(IDC_PEAKSEEK)->GetWindowRect(rcPeakSeek);
	ScreenToClient(rcPeakSeek);
	m_ChcarCtrlPeakSeek.Create(this, rcPeakSeek, 0, WS_CHILD|WS_VISIBLE);
	m_pLinePeakSeek = m_ChcarCtrlPeakSeek.CreateLineSerie(); //创建寻峰曲线
	m_pPoint = m_ChcarCtrlPeakSeek.CreatePointsSerie(); //创建起点终点标点
	m_pPeak  = m_ChcarCtrlPeakSeek.CreatePointsSerie(); //创建波峰标点

	m_pdSaveArray = new double[32];
	for( int i = 0; i < 32; i++ )
	{
		m_pdSaveArray[i] = 0.0;
	}

	m_HalfPeakWidth.SetWindowText(_T("0.00"));
	m_Slope.SetWindowText(_T("5000"));
	m_PeakArea.SetWindowText(_T("无限大"));
	m_avgN.SetCurSel(4);
	m_Series.SetCurSel(2);
	m_Report.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_Report.SetBkColor(RGB(205,205,205));
	m_Report.SetTextBkColor(RGB(205,205,205));
	m_Report.SetTextColor(RGB(45,45,45));
	m_Report.InsertColumn(0, _T("No."),	   LVCFMT_LEFT,38);
	m_Report.InsertColumn(1, _T("保留时间"), LVCFMT_LEFT,68);
	m_Report.InsertColumn(2, _T("峰面积"),   LVCFMT_LEFT,68);
	m_Report.InsertColumn(3, _T("峰高"),     LVCFMT_LEFT,68);
	m_Report.InsertColumn(4, _T("峰宽"),     LVCFMT_LEFT,68); //new
	m_Report.InsertColumn(5, _T("半峰宽"),   LVCFMT_LEFT,68);
	m_Report.InsertColumn(6, _T("峰类型"),   LVCFMT_LEFT,74);
	m_Report.InsertColumn(7, _T("畸变因子"), LVCFMT_LEFT,68);
	m_Report.InsertColumn(8, _T("拖尾因子"), LVCFMT_LEFT,68);
	m_Config.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_Config.SetBkColor(RGB(205,205,205));
	m_Config.SetTextBkColor(RGB(205,205,205));
	m_Config.SetTextColor(RGB(45,45,45));
	m_Config.InsertColumn(0, _T("参数名称"),  LVCFMT_LEFT, 100);
	m_Config.InsertColumn(1, _T("开始时间"),  LVCFMT_LEFT, 85);
	m_Config.InsertColumn(2, _T("结束时间"),  LVCFMT_LEFT, 85);
	m_Config.InsertColumn(3, _T("值"),	    LVCFMT_LEFT, 90);
	
	return TRUE;  
}

void CSignalProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CSignalProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSignalProcessDlg::OnClose()
{
	delete[] m_pdSaveArray;
	CDialog::OnClose();
}

void CSignalProcessDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);
}

//同步比例视图机能
void CSignalProcessDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CChartAxis* pAxisSelectX;
	//CChartAxis* pAxisSelectY;
	//double tempX_MIN;
	//double tempX_MAX; 
	//double tempY_MIN;
	//double tempY_MAX; 

	//if ( 1 /*m_rcChcarCtrl.PtInRect(point)*/ )
	//{
	//	//CSuperChartCtrl* pCtrl     = &m_ChcarCtrl;
	//	//CSuperChartCtrl* pFilter   = &m_ChcarCtrlFilter;
	//	CSuperChartCtrl* pPeakSeek = &m_ChcarCtrlPeakSeek;
	//	pAxisSelectY = pCtrl->GetLeftAxis();   //设置Y坐标
	//	pAxisSelectY->GetMinMax( tempY_MIN, tempY_MAX );
	//	pAxisSelectX = pCtrl->GetBottomAxis(); //设置X坐标
	//	pAxisSelectX->GetMinMax( tempX_MIN, tempX_MAX );
	//	
	//	if (tempY_MAX!=dYMaxVal || tempY_MIN!=dYMinVal || tempX_MAX!=dXMaxVal || tempX_MIN!=dXMinVal)
	//	{	//同步滤波控件视图
	//		pAxisSelectY = pFilter->GetLeftAxis();   //设置滤波控件Y坐标
	//		pAxisSelectY->SetMinMax( tempY_MIN, tempY_MAX );
	//		pAxisSelectX = pFilter->GetBottomAxis(); //设置滤波控件X坐标
	//		pAxisSelectX->SetMinMax( tempX_MIN, tempX_MAX );
	//		//同步寻峰控件视图
	//		pAxisSelectY = pPeakSeek->GetLeftAxis();   //设置寻峰控件Y坐标
	//		pAxisSelectY->SetMinMax( tempY_MIN, tempY_MAX );
	//		pAxisSelectX = pPeakSeek->GetBottomAxis(); //设置寻峰控件X坐标
	//		pAxisSelectX->SetMinMax( tempX_MIN, tempX_MAX );
	//		dYMaxVal = tempY_MAX;
	//		dYMinVal = tempY_MIN; 
	//		dXMaxVal = tempX_MAX;
	//		dXMinVal = tempX_MIN;
	//	}
	//}
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CSignalProcessDlg::PreTranslateMessage(MSG* pMsg)
{	
	return CDialog::PreTranslateMessage(pMsg);
}

void CSignalProcessDlg::DrawDividePeakBaseLine(bool isDividePeak)
{
	if (!isDividePeak)
	{
		return;
	}

	m_ChcarCtrlPeakSeek.EnableRefresh(false);

	const unsigned PeakSize = m_Peak.size();
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (abs(m_Peak[i].Type)!=1)
		{
			int PeakType = m_Peak[i].Type;
			while (m_Peak[i].Type == PeakType && m_Peak[i+1].Type == PeakType )
			{
				CChartLineSerie* line;  
				line = m_ChcarCtrlPeakSeek.CreateLineSerie(); //创建一根基线的连接线
				line->SetColor(RGB(210, 0, 0)); //设置基线颜色
				m_pDividePeakBaseLine.push_back(line);
				line->m_vPoints.InitPoints(4);
				double dx[] = { m_Peak[i].FinSema.x, m_Peak[i].FinSema.x };
				double dy[] = { m_OrigSema[m_Peak[i].FinSema.idx].y, m_Peak[i].FinSema.y };
				line->m_vPoints.AddPoints(dx, dy, 2);
				i++;
			}
		}
	}
	m_ChcarCtrlPeakSeek.EnableRefresh(true);

}
//void CSignalProcessDlg::DrawTagLineOn3rdCtrl()
//{	
	////画标记线
	//const unsigned TagPointSize = m_TagPoint.size();
	//for (unsigned i=0; i<TagPointSize/2; ++i)
	//{//创建画标记线指针
	//	CChartLineSerie* line; 
	//	line = m_ChcarCtrlPeakSeek.CreateLineSerie(); 
	//	line->SetColor(RGB(74, 165, 255)); //设置标记线颜色
	//	m_pTagline.push_back(line);
	//}
	//const unsigned m_pTaglineSize = m_pTagline.size();
	//typedef std::list<CChartLineSerie*>::iterator LI;
	//unsigned i = 0;
	//for (LI iter=m_pTagline.begin(); iter!=m_pTagline.end(); iter++)
	//{//连接标记线
	//	double dx[] = { m_TagPoint[i].x,  m_TagPoint[i+1].x };
	//	double dy[] = { m_TagPoint[i].y,  m_TagPoint[i+1].y };
	//	i+=2;
	//	(*iter)->AddPoints(dx, dy, 2);   //标记线
	//}
	//m_ChcarCtrlPeakSeek.RefreshCtrl();	
//}
void CSignalProcessDlg::OutputReport()
{
	// 填充表格
	m_Report.DeleteAllItems();  
	const int PeakSize = m_Peak.size();
	for (int i=0; i<PeakSize; ++i)
	{
		CString szRepo[9]; //szNo,szT,szArea,szHeight,szHalf,szType,szRatio,szTF;
		szRepo[0].Format(_T("%d"), i+1);
		szRepo[1].Format(_T("%0.4f"),m_Peak[i].PeakSema.x);       //峰的保留时间
		szRepo[2].Format(_T("%0.4f"),m_Peak[i].PeakArea);		  //峰面积
		szRepo[3].Format(_T("%0.4f"),m_Peak[i].PeakHeight);		  //峰高
		szRepo[4].Format(_T("%0.3f"),m_Peak[i].PeakWidth);        //峰宽
		szRepo[5].Format(_T("%0.4f"),m_Peak[i].HalfPeakWidth);    //半峰宽

		CString szPeakType;
		szPeakType.Format(_T("%d"), m_Peak[i].Type);
		if (m_Peak[i].Type==1 )	//峰类型
			szRepo[6] = _T("单独峰") + szPeakType + _T("(+)");
		else if (m_Peak[i].Type==-1)
			szRepo[6] = _T("单独峰") + szPeakType + _T("(-)");
		else if (m_Peak[i].Type>1)
			szRepo[6] = _T("连续峰") + szPeakType + _T("(+)");
		else if (m_Peak[i].Type<-1)
			szRepo[6] = _T("连续峰") + szPeakType + _T("(-)");
		else
			szRepo[6] = _T("人工峰");

		szRepo[7].Format(_T("%0.4f"),m_Peak[i].Disfactor);			//畸变因子
		szRepo[8].Format(_T("%0.4f"),m_Peak[i].Tailing);			//拖尾因子

		m_Report.InsertItem(i, szRepo[0], i); //增加峰序号
		for (int j=1; j<9; ++j)
		{
			m_Report.SetItemText(i, j, szRepo[j]);
		}
	}
}
void CSignalProcessDlg::OutputConfigArgs()
{
	m_Config.DeleteAllItems(); //全清空一次
	const unsigned ArgsItemSize = m_ConfigArgs.size();
	for (unsigned i=0; i<ArgsItemSize; ++i)
	{
		CString szConfigArgs[4]; 
		szConfigArgs[0] = m_ConfigArgs[i].ArgName;  //参数名称
		szConfigArgs[1].Format(_T("%0.4f"),m_ConfigArgs[i].BeginTime);  //起始时间
		szConfigArgs[2].Format(_T("%0.4f"),m_ConfigArgs[i].EndTime);	//结束时间
		szConfigArgs[3].Format(_T("%0.2f"),m_ConfigArgs[i].InputValue); //值

		if (m_Config.InsertItem(i, szConfigArgs[0]) != -1)
		{
			for (int j=1; j<4; ++j)
				if (!m_Config.SetItemText(i, j, szConfigArgs[j]))
					TRACE(_T("m_Config.SetItemText false"));
		}
		else
			TRACE(_T("m_Config.InsertItem false"));
	}
}
void CSignalProcessDlg::OutputResultConfigArgs() //返回正确的配置参数
{
	const unsigned ListCtrlCount = m_Config.GetItemCount(); //获得配置列表行数
	const unsigned ArgsItemSize = m_ConfigArgs.size();
	CString szLabel(_T("----结果----"));
	m_Config.InsertItem(ListCtrlCount, szLabel);

	for (unsigned i=0; i<ArgsItemSize; ++i)
	{
		CString szConfigArgs[4]; 
		szConfigArgs[0] = m_ConfigArgs[i].ArgName;  //参数名称
		szConfigArgs[1].Format(_T("%0.4f"),m_ConfigArgs[i].BeginTime);  //起始时间
		szConfigArgs[2].Format(_T("%0.4f"),m_ConfigArgs[i].EndTime);	//结束时间
		if (szConfigArgs[0] == _T("禁止积分"))
		{
			szConfigArgs[3] = _T("--");
		}
		if (szConfigArgs[0] == _T("手工分峰"))
		{
			szConfigArgs[2] = _T("--");
			szConfigArgs[3] = _T("--");
		}
		if (szConfigArgs[0] == _T("合并峰"))
		{
			szConfigArgs[3] = _T("--");
		}
		if (szConfigArgs[0] == _T("重叠峰处理"))
		{
			if (m_ConfigArgs[i].InputValue == 1)
				szConfigArgs[3] = _T("垂直分峰");
			else if (m_ConfigArgs[i].InputValue == 2)
				szConfigArgs[3] = _T("正切分峰");
			else if (m_ConfigArgs[i].InputValue == 3)
				szConfigArgs[3] = _T("反切分峰");
			else
				szConfigArgs[3] = _T("峰谷基线");
		}
		else if(szConfigArgs[0] == _T("手工调峰"))
		{
			if (m_ConfigArgs[i].InputValue == 11)
				szConfigArgs[3] = _T("起点");
			else if (m_ConfigArgs[i].InputValue == 22)
				szConfigArgs[3] = _T("峰谷");
			else if(m_ConfigArgs[i].InputValue == 33)
				szConfigArgs[3] = _T("终点");

			szConfigArgs[2] = _T("--");
		}
		else
		{
			szConfigArgs[3].Format(_T("%0.2f"),m_ConfigArgs[i].InputValue); //值
		}
		

		if (m_Config.InsertItem(i+ListCtrlCount+1, szConfigArgs[0]) != -1)
		{
			for (int j=1; j<4; ++j)
				if (!m_Config.SetItemText(i+ListCtrlCount+1, j, szConfigArgs[j]))
					TRACE(_T("m_Config.SetItemText false"));
		}
		else
			TRACE(_T("m_Config.InsertItem false"));
	}
}
void CSignalProcessDlg::AddStringsOn3rdCtrl()
{
//	const unsigned PeakSize = m_Peak.size();
//	for (unsigned i=0; i<PeakSize; ++i)
//	{//标出各点并连接直线
//		std::wstring szPeakInfo(L"");
//		wchar_t szNo[10];   //峰序号
//		_itow_s(i+1, szNo, 10, 10);
//		szPeakInfo += szNo;
//		wchar_t szTime[20]; //保留时间
//		double peakTime = m_Peak[i].PeakSema.x;
//		swprintf(szTime, 20, L"%.4lf", peakTime); 
//		szPeakInfo += L"（t";
//		szPeakInfo += szTime;
//		wchar_t szPeakH[20]; //峰高
//		double peakH = m_Peak[i].PeakHeight;
//		swprintf(szPeakH, 20, L"%.4lf", peakH); 
//		szPeakInfo += L"，h";
//		szPeakInfo += szPeakH;
//		szPeakInfo += L"）";
////		m_ChcarCtrlPeakSeek.AddString(m_Peak[i].PeakSema.x,m_Peak[i].PeakSema.y, szPeakInfo);  //设置字符串
//	}
}
void CSignalProcessDlg::OnRClickDatareport(NMHDR *pNMHDR, LRESULT *pResult)
{
	;
}
void CSignalProcessDlg::OnDblclkDatareport(NMHDR *pNMHDR, LRESULT *pResult)  
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	POSITION pos = m_Report.GetFirstSelectedItemPosition(); //获得当前选中报表内的数据
	if(pos!=NULL)
	{
		;
	}
	
	*pResult = 0;
}

VOID CSignalProcessDlg::SaveConfigParameter(CString &opt, const Para &para)
{
	//CString szConfig[5]; 
	//szConfig[0].Format(_T("%d"), m_OptTimes);
	//szConfig[1] = opt;       //操作类型
	//szConfig[2].Format(_T("%d"),para.pn);		  //峰号
	//szConfig[3].Format(_T("%0.4f"),para.st);		  //起点时间
	//szConfig[4].Format(_T("%0.4f"),para.ft);    //终点时间

	//m_Config.InsertItem(m_OptTimes, szConfig[0], 1); //增加峰序号
	//for (int j=1; j<5; ++j)
	//{
	//	m_Config.SetItemText(m_OptTimes-1, j, szConfig[j]);
	//}
	//m_OptTimes++;

}
VOID CSignalProcessDlg::LoadConfigParameter()
{
	//int line = m_Config.GetItemCount();
	//for (int i=1; i<=line; ++i)
	//{
	//	CString szOpation = m_Config.GetItemText(i-1, 1); //第i行，第2列 （0开始计数）
	//	CString szPeakNo = m_Config.GetItemText(i-1, 2);
	//	CString szSTime = m_Config.GetItemText(i-1, 3);
	//	CString szFTime = m_Config.GetItemText(i-1, 4);
	//	int PeakNo = _wtoi(szPeakNo);
	//	double StartSema = _wtof(szSTime);
	//	double FinSema = _wtof(szFTime);
	//	Para p(PeakNo, StartSema, FinSema);

	//	if (szOpation=="modify")
	//		neo_BaseLineAdj(m_OrigSema, m_Peak, p);
	//	else if (szOpation=="add")
	//		neo_AddNewPeak(m_OrigSema, m_Peak, p);
	//	else if (szOpation=="delete")
	//		neo_DelOnePeak(m_OrigSema, m_Peak, p);
	//}
}
int  CSignalProcessDlg::GetMoreAvgPara()
{
	int avg = 5;
	CSetAvgDlg SetAvgDlg;
	if (SetAvgDlg.DoModal()==IDOK)
	{
		avg = SetAvgDlg.m_iParaAvgTimes;
	}
	m_avgN.UpdateData(FALSE);
	CString szContent(_T("内部")), szParaAvgTimes;//内部1点平均
	szParaAvgTimes.Format(_T("%d"), avg);
	szContent += szParaAvgTimes + _T("点平均");
	m_avgN.InsertString(m_avgN.GetCount()-1, szContent);
	m_avgN.SetCurSel(m_avgN.GetCount()-2);
	return avg;
}
void CSignalProcessDlg::OnCbnSelchangeMoreAvgPara()
{
	m_iavgN = m_avgN.GetCurSel()+1;  //几点平均，内部调节参数

	if (m_avgN.GetCount()!=21)
	{
		m_avgN.DeleteString(20);
		m_iavgN--;
	}
	if (m_iavgN==m_avgN.GetCount()) //更多...
	{
		m_iavgN = GetMoreAvgPara();
	}
}
void CSignalProcessDlg::OnCbnDropdownSeries() //内部连续判点的CCombox下拉时事件
{
	if (m_Series.GetCount()>40)
	{
		m_Series.DeleteString(m_Series.GetCount()-1);
	}
}
	
void CSignalProcessDlg::OnTimer(UINT_PTR nIDEvent)
{

	CDialog::OnTimer(nIDEvent);
}

// 界面主要测试功能

void CSignalProcessDlg::OnBnClickedReadshow() 
{ //数据读入
	CFileDialog flDlg(TRUE,_T("csv"),_T(""),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("csv格式色谱信号文件|*.csv||"));
	if (flDlg.DoModal()==IDOK)
	{
		m_PeakSeak.ClearSemaDate();
		m_CsvFilePathName = flDlg.GetPathName();
	}
	else
		return;

	DWORD StartTime = ::GetTickCount();
	m_PeakDraw.ClearLine();	
	m_PeakSeak.SetSemaDate(m_CsvFilePathName);		// 输入信号量
	m_PeakDraw.SetChartCtrl(&m_ChcarCtrlPeakSeek);	// 传入控件指针
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.DrawCurve();

	DWORD FinishTime = ::GetTickCount();
	DWORD  RunTime = FinishTime - StartTime;
	CString szRunTime;
	szRunTime.Format(_T("%u"), RunTime);
	m_RunTime.SetWindowText(szRunTime);

	GetDlgItem(IDC_PREPROCESS)->EnableWindow(TRUE); //按键active
}

//参数预处理
void CSignalProcessDlg::OnPreprocessConfigArgs()
{
	if (m_PeakSeak.m_OrigSema.size()==0)
		return;

	bool isDividePeak = false; //是否开启垂直分峰
	CButton *pBtnVDP = static_cast<CButton*>(GetDlgItem(IDC_VDIVIDEPEAK));
	if (pBtnVDP!=NULL)
	{
		if (pBtnVDP->GetCheck()==BST_CHECKED)
			isDividePeak = true;  //输入变量，是否开启垂直分峰
		else
			isDividePeak = false;
	}
	else
		return;
	bool hasNagetiveSpike = true;     //考虑是否有负峰情况
	CButton *pBtnHNS = static_cast<CButton*>(GetDlgItem(IDC_NAGETIVESPIKE));
	if (pBtnHNS!=NULL)
	{
		if (pBtnHNS->GetCheck()==BST_CHECKED)
			hasNagetiveSpike = true;
		else 
			hasNagetiveSpike = false;
	}
	else
		return;
	CString szSlope;    //要求输入门限斜率
	m_Slope.GetWindowText(szSlope);
	double threshold = _wtof(szSlope);
	if (szSlope==_T("5000")||szSlope.IsEmpty())
		threshold = 5000.0f;      //默认门限斜率

	CString szSlopeFactor;
	m_SlopeFactor.GetWindowText(szSlopeFactor);
	double thresholdfactor = _wtof(szSlopeFactor);
	if (szSlopeFactor.IsEmpty())
		thresholdfactor = 1.0f;

	CString szPeakArea;		   //要求输入峰面积
	m_PeakArea.GetWindowText(szPeakArea);
	double peakArea = _wtof(szPeakArea);
	if (szPeakArea==_T("无限大")||szPeakArea.IsEmpty())
		peakArea = 0.0;         //默认最小峰面积
	CString szHalfPeakWidth;    //要求输入半峰宽
	m_HalfPeakWidth.GetWindowText(szHalfPeakWidth);
	double halfPeakWidth = _wtof(szHalfPeakWidth);
	if (szHalfPeakWidth==_T("0.00")||szHalfPeakWidth.IsEmpty())
		halfPeakWidth = 0.00;  //默认半峰宽
	//根据输入的默认最小半峰宽来调节2个内部参数（连续判点，内部平均点数）
	//连续判点范围[1,10] 内部平均点数范围[1,20]
	//一般默认连续判点=3，内部平均点数=5
	//之前得出的结论是：【峰宽的3/4处为半峰宽，峰宽*100=内部平均点数】
	//而连续判点则不变化，固定于某一个数值
	//经过测试后发现利用峰宽*100=连续判点点数能达到更佳的效果
	int series = 0;
	if (halfPeakWidth == 0.00)
	{
		series = m_Series.GetCurSel()+1;  //几点平均，内部调节参数
		if (series == 0) series = 3;
	}
	else
	{
		series = static_cast<int>(halfPeakWidth * 200);
		CString szContent(_T("自动连续"));
		CString szSeries;
		szSeries.Format(_T("%d"), series);
		szContent += szSeries;
		m_Series.InsertString(m_Series.GetCount(), szContent);
		m_Series.SetCurSel(m_Series.GetCount()-1);
	} 
	m_PeakSeak.SetParameter(isDividePeak, hasNagetiveSpike, threshold, thresholdfactor, halfPeakWidth, peakArea, 0.0f, series, m_iavgN);	// 设置寻峰参数	
	m_PeakDraw.SetChartCtrl(&m_ChcarCtrlPeakSeek);
	
	//ConfigArgument testCA(_T("手工分峰"), 2.980000);   //////////////////////////////////////////////////
	//m_ConfigArgs.push_back(testCA);
	//从txt文件中读取配置参数
//	CReadSaveArgs ReadSaveArgs;
//	ReadSaveArgs.ReadFromTxt(m_ConfigArgs); // E:\\PWB\\chem2009\\SignalProcess\\ConfigArgument.txt



	m_PeakSeak.m_AnalysisConfigArg.SetArgs(m_ConfigArgs);
	m_PeakSeak.m_AnalysisConfigArg.PreprocessConfigArgs(); //配置参数预处理
	OutputConfigArgs();

	//double threshold,minHalfPeakWidth,minPeakArea,minPeakHeight;
	//CString szAnalysisData = pSample_Parameter_PeakProcess->GetXMLString();
	//if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_TRD, threshold)!=IAERROR_SUCCESS)  return FALSE;
	//if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPW, minHalfPeakWidth)!=IAERROR_SUCCESS) return FALSE;
	//if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPA, minPeakArea)!=IAERROR_SUCCESS) return FALSE;
	//if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_HEIGHT, minPeakHeight)!=IAERROR_SUCCESS) return FALSE;

	//// 设置寻峰参数 (默认series, m_iavgN需要修改
	////第1-2个参数：是否开启垂直分峰，是否有负峰情况
	////第3-5个参数：门限斜率，最小半峰宽，最小峰面积，最小峰高
	//SetParameter(true, false, threshold, minHalfPeakWidth, minPeakArea, minPeakHeight);	

	//// 读取设置峰处理参数表
	//CIADataTable cPPTimeTable; 
	//if (pSample_Parameter_PeakProcess->GetValue_Table(XMLPATH_PEAKPROC_PPT, cPPTimeTable) != IAERROR_SUCCESS) return FALSE;;
	//const long ConfigArgumentNum = cPPTimeTable.GetRowsNumber(); //获得列数，即参数个数

	//// 需要传入的参数表，由pAnalysisData解析后传入
	//deque<ConfigArgument> cArgs;
	//for (long i=0; i<ConfigArgumentNum; ++i)
	//{
	//	//判断是否开关
	//	CString szState;
	//	if(!cPPTimeTable.GetCellValue(i, _T("State"), szState)) return FALSE;
	//	if (szState == _T("关")) continue;

	//	CString szName, szBegainTime, szEndTime, szValue;
	//	if(!cPPTimeTable.GetCellValue(i, _T("Name"), szName)) return FALSE; 
	//	if(!cPPTimeTable.GetCellValue(i, _T("BegainTime"), szBegainTime)) return FALSE;
	//	if(!cPPTimeTable.GetCellValue(i, _T("EndTime"), szEndTime)) return FALSE;
	//	if(!cPPTimeTable.GetCellValue(i, _T("Value"), szValue)) return FALSE;

	//	double BegainTime = _tstof(szBegainTime);
	//	double EndTime = _tstof(szEndTime);
	//	double Value = _tstof(szValue);

	//	if (szValue==_T("前切"))
	//		Value = -1.0f;
	//	if (szValue==_T("后切"))
	//		Value = 1.0f;

	//	ConfigArgument cArg(szName, BegainTime, EndTime,  Value);
	//	cArgs.push_back(cArg);
	//}
	////这里需要做个参数转化，把XML上的字段转化成峰处理参数

	//m_AnalysisConfigArg.TransformArgs(cArgs);
	////.........................................................

	//m_AnalysisConfigArg.SetArgs(cArgs);
	//m_AnalysisConfigArg.PreprocessConfigArgs(); //配置参数预处理



	GetDlgItem(IDC_FIRSTSCAN)->EnableWindow(TRUE); //第一次扫描按键active
	GetDlgItem(IDC_PREPROCESS)->EnableWindow(FALSE); //当前按键inactive
	GetDlgItem(IDC_READSHOW)->EnableWindow(FALSE);
}

//第一次扫描：寻峰关键点
void CSignalProcessDlg::OnFirstScan()
{
	m_PeakDraw.ClearLine();	
	//m_PeakSeak.ConfigArgs_FirstScan(); //(接口)信号处理:寻峰关键点
	m_PeakSeak.SP_FirstScan();
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.SignalDisplay();
	GetDlgItem(IDC_SECONDSCAN)->EnableWindow(TRUE); //第二次扫描按键active
	GetDlgItem(IDC_FIRSTSCAN)->EnableWindow(FALSE); //第一次扫描按键inactive
}

//第二次扫描：去除不要的峰
void CSignalProcessDlg::OnSecondScan()
{
	m_PeakDraw.ClearLine();	
	//m_PeakSeak.ConfigArgs_SecondScan(); //(接口)信号处理:去除不要的峰
	m_PeakSeak.SP_SecondScan();
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.SignalDisplay();

	m_PeakSeak.GetPeakResult(m_Peak);
	OutputReport();
	GetDlgItem(IDC_THIRDSCAN)->EnableWindow(TRUE);  //第三次按键active
	GetDlgItem(IDC_SECONDSCAN)->EnableWindow(FALSE); //第二次扫描按键inactive
}

//第三次扫描：重叠峰处理
void CSignalProcessDlg::OnThirdScan()
{
	m_PeakDraw.ClearLine();
	//(接口)信号处理:重叠峰处理(垂直分峰，峰谷连线，切线分峰
	m_PeakSeak.SP_ThirdScan();
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.SignalDisplay();
	
	GetDlgItem(IDC_FOURTHSCAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_THIRDSCAN)->EnableWindow(FALSE);  //第三次按键inactive
	m_PeakSeak.GetPeakResult(m_Peak);
	OutputReport();
}

//第4次扫描：基线的校正
void CSignalProcessDlg::OnFourthScan()
{
	m_PeakDraw.ClearLine();
    //(接口)信号处理:基线校正
	m_PeakSeak.SP_FourthScan();
	m_PeakSeak.SP_FourthScan(); // 1 more time
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.SignalDisplay();

	GetDlgItem(IDC_FIFTHSCAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_FOURTHSCAN)->EnableWindow(FALSE);  //第四次按键inactive
	m_PeakSeak.GetPeakResult(m_Peak);
	OutputReport();
}

//第五次扫描：手工调整峰
void CSignalProcessDlg::OnFifthScan()
{
	m_PeakDraw.ClearLine();
	//m_PeakSeak.ConfigArgs_FourthScan(); //(接口)信号处理:手工调整峰（合并峰、手工分峰、手工基线、手工封顶
	m_PeakSeak.SP_FifthScan();
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.SignalDisplay();

	GetDlgItem(IDC_SIXTHSCAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_FIFTHSCAN)->EnableWindow(FALSE);  //第五次按键inactive
	
	m_PeakSeak.GetPeakResult(m_Peak);
	OutputReport();
}

//第六次扫描：积分并整理
void CSignalProcessDlg::OnSixthScan()
{
	m_PeakDraw.ClearLine();
	//m_PeakSeak.ConfigArgs_SixthScan(); //(接口)信号处理:最终整理
	m_PeakSeak.SP_SixthScan();
	m_PeakDraw.SetCPeakSeakData(m_PeakSeak);
	m_PeakDraw.SignalDisplay();
	
	m_Peak.clear();
	m_PeakSeak.GetPeakResult(m_Peak);
	OutputReport();
	m_ConfigArgs.clear();
	m_ConfigArgs = m_PeakSeak.m_AnalysisConfigArg.GetResultArgument();

	//把最终结果配置列表保存到txt文件中去
//	CReadSaveArgs ReadSaveArgs;
//	ReadSaveArgs.Write2Txt(&m_ConfigArgs);

	OutputResultConfigArgs(); //输入最终结果配置列表

	GetDlgItem(IDC_SIXTHSCAN)->EnableWindow(FALSE);  //第六次按键inactive

	CString szMsg;
	m_PeakSeak.GetPeakTypeStr(szMsg);
	MessageBox(szMsg);

}

//清除结果
void CSignalProcessDlg::OnBnClickedClearResult() 
{
	GetDlgItem(IDC_READSHOW)  ->EnableWindow(TRUE);
	GetDlgItem(IDC_PREPROCESS)->EnableWindow(FALSE);  //参数预处理inactive
	GetDlgItem(IDC_FIRSTSCAN) ->EnableWindow(FALSE);  //第一次按键inactive
	GetDlgItem(IDC_SECONDSCAN)->EnableWindow(FALSE);  //第二次按键inactive
	GetDlgItem(IDC_THIRDSCAN) ->EnableWindow(FALSE);  //第三次按键inactive
	GetDlgItem(IDC_FOURTHSCAN)->EnableWindow(FALSE);  //第四次按键inactive
	GetDlgItem(IDC_FIFTHSCAN) ->EnableWindow(FALSE);  //第五次按键inactive
	GetDlgItem(IDC_SIXTHSCAN) ->EnableWindow(FALSE);  //第六次按键inactive

	m_PeakSeak.ClearSemaDate();
	m_PeakDraw.ClearLine();
}


void CSignalProcessDlg::OnBnClickedProcessPort1()  //积分接口1
{
	//读取XML样品文件内的积分参数
	CFileDialog flDlg(TRUE,_T("cg.smp"),_T(""),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("样品文件|*.cg.smp||"));
	CString XMLFilePath;
	if (flDlg.DoModal()==IDOK)
		XMLFilePath = flDlg.GetPathName();
	else
		return;

//	CAnalysisData analysisData;
//	analysisData.LoadFromFile(XMLFilePath);
//	CString cAllXML = analysisData.GetXMLString();
//	CString cRoot = analysisData.GetXMLPathString();
//	CString cPath = analysisData.m_strXMLPath;
//	CIADataTable cPeakListTable(_T("Sample"), _T("<PeakListTable/>"));
//	
//	m_PeakDraw.SetChartCtrl(&m_ChcarCtrlPeakSeek);
//	
//	//m_PeakSeak.SignalProcess(&analysisData, &cPeakListTable); //积分接口1
//	//m_PeakSeak.GetPeakResult(m_Peak);
//
//	//m_Peak =  m_PeakSeak.SignalProcessResult(&analysisData);//积分接口2
//
////	m_PeakSeak.ClearLine();	
////	m_PeakSeak.CA_SignalDisplay();
//	
//	OutputReport();
//
//	CString Observer = cPeakListTable.GetXMLString();

}


void CSignalProcessDlg::OnBnClickedPreprocess()
{
	// TODO: 在此添加控件通知处理程序代码
}
