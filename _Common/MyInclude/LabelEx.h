
#ifndef TWEN_SDN_CLabelEx
#define TWEN_SDN_CLabelEx

#include "Label.h"

class CLabelEx : public CLabel
{
//	DECLARE_DYNAMIC(CLabelEx)

public:
	CLabelEx();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabelEx)
public:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CLabelEx)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetToolTip(LPCTSTR tooltip=NULL,BOOL bActivate=TRUE);
	void ActivateTooltip(BOOL bActivate=TRUE);

protected:
	void InitToolTip();
	CToolTipCtrl m_ToolTip;
};

#endif