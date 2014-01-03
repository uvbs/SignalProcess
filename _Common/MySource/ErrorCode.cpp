#include "StdAfx.h"
#include "ErrorCode.h"



#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "Log_D.lib")
#else
#pragma comment(lib, "Log_R.lib")
#endif


///////////////////////////////////////////////////////////////////////////
// log相关
///////////////////////////////////////////////////////////////////////////
CErrorLog CPAInformation::log;
CMFCCaptionBar* CPAInformation::m_pcCaptionBar = NULL;
CString CPAInformation::m_strMsgboxtitle;

CStringArray& CPAInformation_DefaultButtons()
{
	static CStringArray sa;
	if(sa.IsEmpty())
	{
		//sa.Add(_T("确定"));
		sa.Add(_T("关闭"));
	}
	return sa;
}

CString CPAInformation_ToFormatFixedLength(const CString& s, enumTextFormat eFormat)
{
	static int length = 40;
	if(s.GetLength() < length) return s;

	CString t;
	int l = 0;
	for(int i = 0; i<s.GetLength(); ++i)
	{
#if defined(UNICODE) || defined(_UNICODE)
		TCHAR c = s.GetAt(i);
		if(c < 256) ++l;
		else l += 2;
		t += c;
		if(l >= length)
		{
			l = 0;
			if(i != s.GetLength()-1)
			{
				switch(eFormat)
				{
					case TEXTFORMAT_TEXT:
						t += _T("\r\n      ");break;
					case TEXTFORMAT_HTML:
						t += _T("<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");break;
				}
			}
		}
#endif
	}
	return t;
}
CString CPAInformation_ToFormat(enumTextFormat eFormat, LPCTSTR lpcszErrorText, LPCTSTR lpcszOperation, LPCTSTR lpcszWhere, LPCTSTR file, int line, LPCTSTR function)
{
	CString s, t;
	switch(eFormat)
	{
	case TEXTFORMAT_TEXT:
		if(lpcszErrorText){s += _T("信息：");s += CPAInformation_ToFormatFixedLength(lpcszErrorText, eFormat);s += _T("\r\n");}
		if(lpcszOperation){s += _T("操作：");s += CPAInformation_ToFormatFixedLength(lpcszOperation, eFormat);s += _T("\r\n");}
		if(lpcszWhere){s += _T("位置：");s += CPAInformation_ToFormatFixedLength(lpcszWhere, eFormat);s += _T("\r\n");}
		if(file){s += _T("文件：");s += CPAInformation_ToFormatFixedLength(file, eFormat);s += _T("\r\n");}
		if(line > -1){s += _T("行号：");t.Format(_T("%d"), line);s += t;s += _T("\r\n");}
		if(function){s += _T("函数：");s += function;s += _T("\r\n");}
		return s;
	case TEXTFORMAT_HTML:
		if(lpcszErrorText){s += _T("<font color='blue'>信息：</font>");s += CPAInformation_ToFormatFixedLength(lpcszErrorText, eFormat);s += _T("<br/>");}
		if(lpcszOperation){s += _T("<font color='blue'>操作：</font>");s += CPAInformation_ToFormatFixedLength(lpcszOperation, eFormat);s += _T("<br/>");}
		if(lpcszWhere){s += _T("<font color='blue'>位置：</font>");s += CPAInformation_ToFormatFixedLength(lpcszWhere, eFormat);s += _T("<br/>");}
		if(file){s += _T("<font color='blue'>文件：</font>");s += CPAInformation_ToFormatFixedLength(file, eFormat);s += _T("<br/>");}
		if(line > -1){s += _T("<font color='blue'>行号：</font>");t.Format(_T("%d"), line);s += t;s += _T("<br/>");}
		if(function){s += _T("<font color='blue'>函数：</font>");s += function;s += _T("<br/>");}
		return s;
	default:return _T("");
	}
}
///////////////////////////////////////////////////////////////////////////
CString CPAInformation::ErrorCode2String(ERRORCODE errorCode, void* pErrorArguments)
{
	CString ret;
	switch(errorCode)
	{
	case IAERROR_SUCCESS:ret.Format(_T("成功!"));break;
	case IAERROR_UNKNOWN:ret.Format(_T("未知错误!"));break;
	case IAERROR_SYSTEMERROR:ret.Format(_T("系统内部错误!"));break;

	case IAERROR_FILE_NOTFOUND:return(_T("未找到文件"));break;
	case IAERROR_FILE_FORMATERROR:return(_T("文件格式错误"));break;
	case IAERROR_FILE_SAVEFAILURE:return(_T("文件出盘出错"));break;

	case IAERROR_DB_NOTFOUND:return(_T("未找到数据库项"));break;

	case IAERROR_XML_INITERROR:return(_T("XML初始化错误"));break;
	case IAERROR_XML_NOTVALID:return(_T("XML数据不合法"));break;
	case IAERROR_XML_ITEMNOTFOUND:return(_T("XML项没有找到"));break;
	case IAERROR_XML_SYSERROR:return(_T("XML出错"));break;

	case IAERROR_PARAMETER_WRONGTYPE:return(_T("错误的数据类型"));break;

	case IAERROR_XML_NOTREADCHANNEL:return(_T("读取通道号错误"));break;
	case IAERROR_CONTENTNOTNULL:return(_T("表格内容不能为空"));break;

	//色谱错误信息
	case IAERROR_CG_ACQUIRING:return(_T("正在采样"));break;
	case IAERROR_CG_ACQUIRED:return(_T("采样已经结束"));break;
	case IAERROR_CG_ACQUIRER_NOTFOUND:return(_T("没发现仪器接口程序"));break;
	case IAERROR_CG_ACQUIRER_CANNOTRUN:return(_T("仪器接口程序无法正常运行"));break;
	case IAERROR_CG_ACQUIRER_NOTREADY:return(_T("仪器接口没有准备好"));break;

	case IAERROR_CG_SAMPLE_BUSY:return(_T("样品正忙，无法相应相关操作"));break;
	case IAERROR_CG_SAMPLE_RUNNING:return(_T("样品正在采样，无法相应相关操作"));break;
	case IAERROR_CG_SAMPLE_DATAEXIST:return(_T("样品数据已经存在，无法相应相关操作"));break;
	case IAERROR_CG_SAMPLE_NOTREADY	:return(_T("样品没有准备好"));break;
	case IAERROR_CG_SAMPLE_STATUSERROR:return(_T("样品状态错误"));break;
	case IAERROR_CG_INSTRUMENT_ERROR:return(_T("仪器错误"));break;
	case IAERROR_CG_INSTRUMENT_BUSY:return(_T("仪器正在运行参数,或已经被占用"));break;
	case IAERROR_SAMPLE_NODATA:return(_T("样品没有数据"));break;

	case IAERROR_INSTRUMENT_NOTCONNECT:return(_T("仪器没有连接"));break;
	case IAERROR_INSTRUMENT_TEMPNOTREADY:return(_T("仪器温度没达到"));break;
	case IAERROR_INSTRUMENT_CHANNELERROR:return(_T("仪器通道错误"));break;

	case IAERROR_INSTRUMENT_PARAMNOTREADY:return(_T("仪器参数没达到设定标准"));break;
	case IAERROR_INSTRUMENT_PARAMSETERROR:return(_T("无法设定仪器参数"));break;
	case IAERROR_INSTRUMENT_PARAMGETERROR:return(_T("无法获得仪器参数"));break;

	//仪器错误
	case IAERROR_VALVE_COMM	:return(_T("进样通讯错误"));break;
	default:ret.Format(_T("未知错误(%d)!"), errorCode);
	}
	return ret;
}

CString CPAInformation::ToTString(LPCSTR s)
{
	USES_CONVERSION;
	return A2T(s);
}

///////////////////////////////////////////////////////////////////////////
void CPAInformation::Init(LPCTSTR logfile, LPCTSTR msgboxtitle, CMFCRibbonBaseElement* pStatusBar, CMFCCaptionBar* pCaptionBar)
{
	if(msgboxtitle) m_strMsgboxtitle = msgboxtitle;
	log.SetTextFormat(TEXTFORMAT_TEXT);
	log.SetMessageParser(ErrorCode2String);
	log.SetFile(logfile);
	log.SetParent(AfxGetMainWnd(), msgboxtitle, &CPAInformation_DefaultButtons());
	log.SetRibbonStatusBar(pStatusBar);
	m_pcCaptionBar = pCaptionBar;
}

void CPAInformation::ErrorLog(CWnd* parent, DWORD dwTarget, enumErrorLevel eErrorLevel, LPCTSTR lpcszErrorText, LPCTSTR lpcszOperation, LPCTSTR lpcszWhere, LPCTSTR file, int line, LPCTSTR function)
{
	DWORD dwOldTarget = dwTarget;
	dwTarget &= ~LOGTARGET_MESSAGEBOX;
	if(dwTarget) log.Report(CPAInformation_ToFormat(TEXTFORMAT_TEXT, lpcszErrorText, lpcszOperation, lpcszWhere, file, line, function), 
			dwTarget, eErrorLevel, false);

	if(dwOldTarget && LOGTARGET_MESSAGEBOX)
	{
		CString s = CPAInformation_ToFormat(TEXTFORMAT_HTML, lpcszErrorText, lpcszOperation, lpcszWhere, file, line, function);
		if(parent) log.Report2MessageBox(s, parent, m_strMsgboxtitle, &CPAInformation_DefaultButtons(), eErrorLevel);
		else log.Report2MessageBox(s, eErrorLevel);
	}
}
void CPAInformation::Alert(CWnd* parent, enumHTMLMessageBoxIcon eIcon, LPCTSTR lpcszErrorText, LPCTSTR lpcszOperation, LPCTSTR lpcszWhere)
{
	CString s = CPAInformation_ToFormat(TEXTFORMAT_HTML, lpcszErrorText, lpcszOperation, lpcszWhere, NULL, -1, NULL);
	HTMLMessageBox(parent, s, m_strMsgboxtitle, &CPAInformation_DefaultButtons(), eIcon);
}

BOOL CPAInformation::PostInformation(LPCTSTR lpcszMessage, LPCTSTR lpcszButtonFace, UINT uiCmdUI, HBITMAP hBitmap, COLORREF clrTransparent)
{
	if(!m_pcCaptionBar) return FALSE;

	//如果有图标显示
	if(hBitmap!=0) {
		m_pcCaptionBar->SetBitmap(hBitmap, clrTransparent, FALSE, CMFCCaptionBar::ALIGN_LEFT);
	}

	//显示信息
	m_pcCaptionBar->SetText(lpcszMessage, CMFCCaptionBar::ALIGN_LEFT);


	//如果按钮没有名字，说明不需要显示按钮
	if (lpcszButtonFace == NULL || !*lpcszButtonFace) {
		m_pcCaptionBar->RemoveButton();
		m_pcCaptionBar->SetButton(_T(" "), 0, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	}
	//否则需要显示按钮
	else {
		//m_pcCaptionBar->RemoveButton();//  [12/17/2012 洪浩] 防止闪烁
		m_pcCaptionBar->SetButton(lpcszButtonFace, uiCmdUI, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	}

	return TRUE;
}