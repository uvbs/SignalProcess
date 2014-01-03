#pragma once
#include "afxwin.h"


// CSetAvgDlg 对话框

class CSetAvgDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetAvgDlg)

public:
	CSetAvgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetAvgDlg();

// 对话框数据
	enum { IDD = IDD_SET_AVGPOINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancle();

public:
	CEdit m_ParaAvgTimes; 
	int m_iParaAvgTimes;
};
