// SetAvgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SignalProcess.h"
#include "SetAvgDlg.h"


// CSetAvgDlg 对话框

IMPLEMENT_DYNAMIC(CSetAvgDlg, CDialog)

CSetAvgDlg::CSetAvgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetAvgDlg::IDD, pParent)
{

}

CSetAvgDlg::~CSetAvgDlg()
{
}

void CSetAvgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ParaAvgTimes);
}


BEGIN_MESSAGE_MAP(CSetAvgDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSetAvgDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDC_BUTTON2, &CSetAvgDlg::OnBnClickedCancle)
END_MESSAGE_MAP()


// CSetAvgDlg 消息处理程序

void CSetAvgDlg::OnBnClickedOK()
{
	CString szParaAvgTimes;    //输入内部平均点数
	m_ParaAvgTimes.GetWindowText(szParaAvgTimes);
	m_iParaAvgTimes = _wtoi(szParaAvgTimes);
	CSetAvgDlg::OnOK();
}

void CSetAvgDlg::OnBnClickedCancle()
{
	CSetAvgDlg::OnCancel();
}
