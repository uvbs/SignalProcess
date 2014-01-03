// ParaAdjust.cpp : 实现文件
//

#include "stdafx.h"
#include "SignalProcess.h"
#include "ParaAdjust.h"

// CParaAdjust 对话框

IMPLEMENT_DYNAMIC(CParaAdjust, CDialog)

CParaAdjust::CParaAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CParaAdjust::IDD, pParent)
{
	m_PeakNo = 0;
	m_StartSema = 0.0;
	m_FinSema = 0.0;
	m_DlgStyle = 0;
}

CParaAdjust::~CParaAdjust()
{
}

void CParaAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_iPeakNo);
	DDX_Control(pDX, IDC_EDIT2, m_dStartSema);
	DDX_Control(pDX, IDC_EDIT4, m_dFinSema);
	DDX_Control(pDX, IDC_ADJUSTMENT, m_Button);
}


BEGIN_MESSAGE_MAP(CParaAdjust, CDialog)
	ON_BN_CLICKED(IDC_ADJUSTMENT, &CParaAdjust::OnBnClickedAdjustment)
	ON_BN_CLICKED(IDC_BUTTON1, &CParaAdjust::OnClickedCancle)
END_MESSAGE_MAP()


// CParaAdjust 消息处理程序

void CParaAdjust::OnBnClickedAdjustment()
{
	CString szPeakNo;    //输入调整波峰编号
	m_iPeakNo.GetWindowText(szPeakNo);
	m_PeakNo = _wtoi(szPeakNo);

	CString szStartSema;    //输入调整波起点
	m_dStartSema.GetWindowText(szStartSema);
	m_StartSema = _wtof(szStartSema);

	CString szFinSema;    //输入调整波终点
	m_dFinSema.GetWindowText(szFinSema);
	m_FinSema = _wtof(szFinSema);

	CParaAdjust::OnOK();
}

BOOL CParaAdjust::OnInitDialog()
{

	CDialog::OnInitDialog();
	CString szPeakNo;
	szPeakNo.Format(_T("%d"), m_PeakNo);
	m_iPeakNo.SetWindowText(szPeakNo);

	CString szStartT;
	szStartT.Format(_T("%0.6f"), m_StartSema);
	m_dStartSema.SetWindowText(szStartT);

	CString szFinT;
	szFinT.Format(_T("%0.6f"), m_FinSema);
	m_dFinSema.SetWindowText(szFinT);

	if (m_DlgStyle==2)
	{
		m_Button.SetWindowText(_T("增加峰"));
		m_iPeakNo.SetReadOnly();
		m_iPeakNo.SetWindowText(_T(""));
	}
	else if (m_DlgStyle==3)
	{
		m_Button.SetWindowText(_T("删除峰"));
		m_iPeakNo.SetReadOnly();
		m_iPeakNo.SetWindowText(_T(""));
	}
	else
	{
		m_Button.SetWindowText(_T("基线调整"));
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParaAdjust::OnClickedCancle()
{
	CDialog::OnCancel();
}
