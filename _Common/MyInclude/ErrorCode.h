#pragma once



///////////////////////////////////////////////////////////////////////////
// 错误码
///////////////////////////////////////////////////////////////////////////

#define IAERROR_SUCCESS					0
#define IAERROR_UNKNOWN					255	//未知错误
#define IAERROR_SYSTEMERROR				254	//系统内部错误


#define IAERROR_FILE_NOTFOUND			1	//未找到文件
#define IAERROR_FILE_FORMATERROR		2	//文件格式错误
#define IAERROR_FILE_SAVEFAILURE		3	//文件出盘出错

#define IAERROR_DB_NOTFOUND				21	//未找到数据库项

#define IAERROR_XML_INITERROR			40	//XML初始化错误
#define IAERROR_XML_NOTVALID			41	//XML数据不合法
#define IAERROR_XML_ITEMNOTFOUND		42	//XML项没有找到
#define IAERROR_XML_SYSERROR			43	//XML出错

#define IAERROR_PARAMETER_WRONGTYPE		61	//错误的数据类型

#define IAERROR_XML_NOTREADCHANNEL      100 //读取通道号错误
#define IAERROR_CONTENTNOTNULL          101 //表格内容不能为空

//色谱错误信息
#define IAERROR_CG_ACQUIRING			201 //正在采样
#define IAERROR_CG_ACQUIRED				202 //采样已经结束
#define IAERROR_CG_ACQUIRER_NOTFOUND	203 //没发现仪器接口程序
#define IAERROR_CG_ACQUIRER_CANNOTRUN	204 //仪器接口程序无法正常运行
#define IAERROR_CG_ACQUIRER_NOTREADY	205 //仪器接口没有准备好

#define IAERROR_CG_SAMPLE_BUSY			206 //样品正忙，无法相应相关操作
#define IAERROR_CG_SAMPLE_RUNNING		207 //样品正在采样，无法相应相关操作
#define	IAERROR_CG_SAMPLE_DATAEXIST		208	//样品数据已经存在，无法相应相关操作
#define	IAERROR_CG_SAMPLE_NOTREADY		209	//样品没有准备好
#define	IAERROR_CG_SAMPLE_STATUSERROR	210	//样品状态错误
#define	IAERROR_CG_INSTRUMENT_ERROR		211	//仪器错误
#define	IAERROR_CG_INSTRUMENT_BUSY		212	//仪器正在运行参数,或已经被占用
#define	IAERROR_SAMPLE_NODATA			213	//样品没有数据

#define	IAERROR_INSTRUMENT_NOTCONNECT	300	//仪器没有连接
#define	IAERROR_INSTRUMENT_TEMPNOTREADY	301	//仪器温度没达到
#define	IAERROR_INSTRUMENT_CHANNELERROR	302	//仪器通道错误

#define	IAERROR_INSTRUMENT_PARAMNOTREADY 303	//仪器参数没达到设定标准
#define	IAERROR_INSTRUMENT_PARAMSETERROR 304	//无法设定仪器参数
#define	IAERROR_INSTRUMENT_PARAMGETERROR 305	//无法获得仪器参数


//仪器错误
#define IAERROR_VALVE_COMM				0x87FE	//进样通讯错误





///////////////////////////////////////////////////////////////////////////
// log相关
///////////////////////////////////////////////////////////////////////////

#define SHP_DLL_LOG_RIBBON

#include <Log/Log.h>

SHP_DLL_LOG_NAMESPACE_USE;

// 注意查看<Log/Log.h>文件中的enumLogTarget枚举量，以获得Log的完整输出目标
// 对于已提供的输出目标，具体工程中就不能出现原生态的同功能语句，比如TRACE及其变体、MessageBox、AfxMessageBox、File、SetWindowText等


//在公用库中，尽量使用AfxMessageBox替代MessageBox
//在具体的工程中，必须使用HTMLMessageBox或ToHTMLMessageBox(ALERT系列宏)替代MessageBox和AfxMessageBox
//在具体的工程中，作为Log的信息必须使用DEBUGLOG、LOG、DEBUGLOGTHIS、LOGTHIS
// DEBUGLOGTHIS、LOGTHIS是在DEBUGLOG、LOG的基础上增加了当前窗口指针参数，该参数仅用于LOGTARGET_MESSAGEBOX
// DEBUGLOG、DEBUGLOGTHIS是在LOG、LOGTHIS的基础上增加了当前源文件、行号和函数名的输出
// 由用户交互直接产生的操作，传入为lpcszOperation；其他传入为lpcszWhere
// 
class CPAInformation
{
public:
	static CString ErrorCode2String(ERRORCODE errorCode, void* pErrorArguments);
	static CString ToTString(LPCSTR s);

public:
	static void Init(LPCTSTR logfile, LPCTSTR msgboxtitle, CMFCRibbonBaseElement* pStatusBar, CMFCCaptionBar* pCaptionBar);

	static void ErrorLog(CWnd* parent, DWORD dwTarget, enumErrorLevel eErrorLevel, LPCTSTR lpcszErrorText, LPCTSTR lpcszOperation, LPCTSTR lpcszWhere, LPCTSTR file, int line, LPCTSTR function);
	static void Alert(CWnd* parent, enumHTMLMessageBoxIcon eIcon, LPCTSTR lpcszErrorText, LPCTSTR lpcszOperation, LPCTSTR lpcszWhere);

	static BOOL PostInformation(LPCTSTR lpcszMessage, LPCTSTR lpcszButtonFace = NULL, UINT uiCmdUI = 0, HBITMAP hBitmap = 0, COLORREF clrTransparent = RGB(255,255,255));

public:
	static CErrorLog log;

private:
	static CMFCCaptionBar* m_pcCaptionBar;
	static CString m_strMsgboxtitle;
};


#define DEBUGLOG(dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere) CPAInformation::ErrorLog(NULL, dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere, CPAInformation::ToTString(__FILE__), __LINE__, CPAInformation::ToTString(__FUNCDNAME__))
#define DEBUGLOGTHIS(dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere) CPAInformation::ErrorLog(this, dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere, CPAInformation::ToTString(__FILE__), __LINE__, CPAInformation::ToTString(__FUNCDNAME__))

#define LOG(dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere) CPAInformation::ErrorLog(NULL, dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere, NULL, -1, NULL)
#define LOGTHIS(dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere) CPAInformation::ErrorLog(this, dwTarget, eErrorLevel, lpcszErrorText, lpcszOperation, lpcszWhere, NULL, -1, NULL)

#define ALERT(eIcon, lpcszErrorText, lpcszOperation, lpcszWhere) CPAInformation::Alert(NULL, eIcon, lpcszErrorText, lpcszOperation, lpcszWhere)
#define ALERTTHIS(eIcon, lpcszErrorText, lpcszOperation, lpcszWhere) CPAInformation::Alert(this, eIcon, lpcszErrorText, lpcszOperation, lpcszWhere)

#define DISPifERROR(errCode, lpcszOperation, lpcszWhere)	{int __nRet = (errCode);if(__nRet!=IAERROR_SUCCESS) ALERT(ICON_CRITICAL, CPAInformation::ErrorCode2String(__nRet, NULL), lpcszOperation, lpcszWhere);}
#define DISPifERRORTHIS(errCode, lpcszOperation, lpcszWhere)	{int __nRet = (errCode);if(__nRet!=IAERROR_SUCCESS) ALERTTHIS(ICON_CRITICAL, CPAInformation::ErrorCode2String(__nRet, NULL), lpcszOperation, lpcszWhere);}
#define PASUCCESS(nRet) (nRet==IAERROR_SUCCESS)
#define RETURNifERROR(errCode)	{int __nRet = (errCode);if(__nRet!=IAERROR_SUCCESS) return __nRet;}