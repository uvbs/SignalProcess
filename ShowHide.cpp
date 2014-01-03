// ShowHide.cpp : 实现文件
//

#include "stdafx.h"
#include "SignalProcess.h"
#include "ShowHide.h"


// CShowHide 对话框

IMPLEMENT_DYNAMIC(CShowHide, CDialog)

CShowHide::CShowHide(CWnd* pParent /*=NULL*/)
	: CDialog(CShowHide::IDD, pParent)
{
	g1_IsShow = true;
	g2_IsShow = true;
}

CShowHide::~CShowHide()
{
}

void CShowHide::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, m_G1show);
	DDX_Control(pDX, IDC_RADIO3, m_G2show);
}


BEGIN_MESSAGE_MAP(CShowHide, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, &CShowHide::OnBnClickedCancle)
	ON_BN_CLICKED(IDC_BUTTON1, &CShowHide::OnBnClickedOK)
END_MESSAGE_MAP()


// CShowHide 消息处理程序

void CShowHide::OnBnClickedCancle()
{
	CShowHide::OnCancel();
}

void CShowHide::OnBnClickedOK()
{
	if (m_G1show.GetCheck()!=BST_CHECKED)
		g1_IsShow = false;
	else
		g1_IsShow = true;
	if (m_G2show.GetCheck()!=BST_CHECKED)
		g2_IsShow = false;
	else
		g2_IsShow = true;
	CShowHide::OnOK();
}

BOOL CShowHide::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (g1_IsShow) 
		static_cast<CButton*>(GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
	else
		static_cast<CButton*>(GetDlgItem(IDC_RADIO2))->SetCheck(BST_CHECKED);

	if (g2_IsShow)
		static_cast<CButton*>(GetDlgItem(IDC_RADIO3))->SetCheck(BST_CHECKED);
	else
		static_cast<CButton*>(GetDlgItem(IDC_RADIO4))->SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
