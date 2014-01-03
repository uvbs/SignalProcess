
#include "stdafx.h"
#include "LabelEx.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleButton
///////////////////////////////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNAMIC(CLabelEx, CLabel)

CLabelEx::CLabelEx()
{
	m_ToolTip.m_hWnd = NULL;
}

BEGIN_MESSAGE_MAP(CLabelEx, CLabel)
END_MESSAGE_MAP()

void CLabelEx::PreSubclassWindow() 
{
	CLabel::PreSubclassWindow();

	ModifyStyle(NULL, SS_NOTIFY);
}
BOOL CLabelEx::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	if(pMsg->message == WM_MOUSEMOVE) m_ToolTip.RelayEvent(pMsg);

	return CLabel::PreTranslateMessage(pMsg);
}

void CLabelEx::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_BALLOON|TTS_NOPREFIX);
		m_ToolTip.Activate(FALSE);
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
		m_ToolTip.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}
void CLabelEx::SetToolTip(LPCTSTR tooltip,BOOL bActivate)
{
	if(tooltip == NULL) return;
	InitToolTip();
	if(m_ToolTip.GetToolCount()<1)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, tooltip, rectBtn, 1);
	}
	m_ToolTip.UpdateTipText(tooltip, this, 1);
	m_ToolTip.Activate(bActivate);
}
void CLabelEx::ActivateTooltip(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount()<1) return;
	m_ToolTip.Activate(bActivate);
}