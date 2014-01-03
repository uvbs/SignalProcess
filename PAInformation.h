#pragma once

////////////////////////////////////////////////////////////////////////////////
//	错误等级
enum PAErrorLevel {
	pael_WriteDebugWindow=0,
	pael_WriteStatusBar,
	pael_WriteStatusWindow,
	pael_WriteLogFile,
	pael_PopMessage,
	pael_NeedRestart
};

////////////////////////////////////////////////////////////////////////////////
//	错误结构
typedef struct tagPAError {
	int iErrorCode;
	int iErrorLevel;
	LPCTSTR lpcszErrorText;
}	PA_ERROR, *P_PA_ERROR;
typedef const PA_ERROR *PC_PA_ERROR;

//
//--------系统返回错误代码-------------------------------
//
#define IAERROR_SUCCESS					0
#define IAERROR_UNKNOWN					255	//未知错误


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


//仪器错误
#define IAERROR_VALVE_COMM				0x87FE	//进样通讯错误
#define IAERROR_RGA_COMM				0x87FF	//检测器通讯错误
#define IAERROR_RGA_BASE				0x8800
#define IAERROR_RGA_ERRCNT_11			(IAERROR_RGA_BASE|11)	//"Bad command received"
#define IAERROR_RGA_ERRCNT_12			(IAERROR_RGA_BASE|12)	//"Bad Parameter received"
#define IAERROR_RGA_ERRCNT_13			(IAERROR_RGA_BASE|13)	//"Command-too-long"
#define IAERROR_RGA_ERRCNT_14			(IAERROR_RGA_BASE|14)	//"OVERWRITE in receiving"
#define IAERROR_RGA_ERRCNT_15			(IAERROR_RGA_BASE|15)	//"Transmit buffer overwrite"
#define IAERROR_RGA_ERRCNT_16			(IAERROR_RGA_BASE|16)	//"Jumper protection violation"
#define IAERROR_RGA_ERRCNT_17			(IAERROR_RGA_BASE|17)	//"Parameter conflict"
#define IAERROR_RGA_ERRCNT_21			(IAERROR_RGA_BASE|21)	//"Single filament operation"
#define IAERROR_RGA_ERRCNT_26			(IAERROR_RGA_BASE|26)	//"Vacuum Chamber pressure too high"
#define IAERROR_RGA_ERRCNT_27			(IAERROR_RGA_BASE|27)	//"Unable to set the requested emission current"
#define IAERROR_RGA_ERRCNT_28			(IAERROR_RGA_BASE|28)	//"No filament detected"
#define IAERROR_RGA_ERRCNT_48			(IAERROR_RGA_BASE|48)	//"No Electron Multiplier Option installed"
#define IAERROR_RGA_ERRCNT_55			(IAERROR_RGA_BASE|55)	//"Power supply in current limited mode"
#define IAERROR_RGA_ERRCNT_57			(IAERROR_RGA_BASE|57)	//"Primary current exceeds 2.0A"
#define IAERROR_RGA_ERRCNT_58			(IAERROR_RGA_BASE|58)	//"RF_CT exceeds (V_EXT- 2V) at M_MAX"
#define IAERROR_RGA_ERRCNT_62			(IAERROR_RGA_BASE|62)	//"OP-AMP Input Offset Voltage out of range"
#define IAERROR_RGA_ERRCNT_64			(IAERROR_RGA_BASE|64)	//"COMPENSATE fails to read -5nA input current"
#define IAERROR_RGA_ERRCNT_65			(IAERROR_RGA_BASE|65)	//"COMPENSATE fails to read +5nA input current"
#define IAERROR_RGA_ERRCNT_66			(IAERROR_RGA_BASE|66)	//"DETECT fails to read -5nA input current"
#define IAERROR_RGA_ERRCNT_67			(IAERROR_RGA_BASE|67)	//"DETECT fails to read +5nA input current"
#define IAERROR_RGA_ERRCNT_68			(IAERROR_RGA_BASE|68)	//"ADC16 Test failure"
#define IAERROR_RGA_ERRCNT_77			(IAERROR_RGA_BASE|77)	//"Voltage <22V"
#define IAERROR_RGA_ERRCNT_78			(IAERROR_RGA_BASE|78)	//"Voltage >26V"

//基于全局变量的宏定义，方便使用
#define DISPifERROR(errCode,strWhere)	g_pcInformation->reportError(errCode,strWhere, 4)
#define PASUCCESS(nRet) (nRet==IAERROR_SUCCESS)
#define RETURNifERROR(errCode)	{int nRet =errCode;if(nRet!=IAERROR_SUCCESS) return nRet;}

//此函数仅照顾质谱里的几次调用，以后要去掉！请勿使用
#define DisplayIfError(cStrWhere,nErrorCode)	g_pcInformation->reportError(nErrorCode,cStrWhere)


class AFX_EXT_CLASS CPASystemInformation
{
public:
	CPASystemInformation();
	virtual ~CPASystemInformation();

	void bindParentWindow(HWND hWnd) {m_hParentWnd = hWnd;}
	void SetStatusBar(CMFCRibbonBaseElement* pCWndStatusElement) {m_pCWndStatusElement = pCWndStatusElement;}
	void InitCaptionBar(CMFCCaptionBar	*pcCaptionBar){m_pcCaptionBar=pcCaptionBar;}
	void setLogPath(LPCTSTR lpcszPath) {StrCpy(m_szLogPath, lpcszPath);}
public:
	//CString GetErrorString (int nErrorCode){void* pcRet;m_cMapErrMsg.Lookup(nErrorCode, pcRet);return (PTCHAR)pcRet;}

	//该函数弹出对话框显示nErrorCode对应的错误信息,格式如下:
	// (...cStrWhere...)出错: (...错误信息...)
	//注意当nErrorCode==IAERROR_SUCCESS 不弹出对话框!
	//函数返回值等于nErrorCode
	//比较方便的用法如下: 比如你想在YourFunction()返回错误的时候显示错误信息
	//
	//   DisplayInformationIfError (YourFunction(), _T("出错地方描述"));
	//
	//int	DisplayInformationIfError(int nErrorCode, CString cStrWhere=_T(""));

// 	BOOL	PostInformation(int nErrorCode,					//错误代码定义,IAERROR_...
// 							CString cStrButtonFace=_T(""),	//如果要加入按钮,设置按钮的显示字符
// 							UINT uiCmdUI=0,					//CMaimFrame中响应的命令代码
// 							HBITMAP hBitmap=0,				//如果要显示图标,图标的句柄
// 							COLORREF clrTransparent=RGB(255,255,255)	//图标中的透明色
// 							);
	//和上面的函数一样,只是直接传入错误信息字符串cStrMessage,不用错误代码
	BOOL PostInformation(LPCTSTR lpcszMessage, LPCTSTR lpcszButtonFace = NULL, UINT uiCmdUI = 0, HBITMAP hBitmap = 0, COLORREF clrTransparent = RGB(255,255,255));
	void SetMsgBoxInfo(HINSTANCE hInstance, LPCTSTR lpszIcon);//AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME)

private:
	HINSTANCE m_hInstance_MsgBoxInfo;
	LPCTSTR m_lpszIcon_MsgBoxInfo;
	//CMapWordToPtr	m_cMapErrMsg;


public:
	void SetErrorInfo(PA_ERROR	*pcErrorInf, int nNumber){m_nErrorNumber=nNumber;m_pcErrorInf=pcErrorInf;}
	int reportError(int iErrorCode, LPCTSTR lpcszWhere = NULL, int iErrorLevel = 4, LPCTSTR lpcszFilename = NULL, int iFileLine = -1, UINT uCmdID = 0, LPCTSTR lpcszCmdText = NULL);
	void reportSpecialError(LPCTSTR lpcszErrorText, LPCTSTR lpcszWhere = NULL, PAErrorLevel eErrorLevel = pael_WriteDebugWindow, LPCTSTR lpcszFilename = NULL, int iFileLine = -1, UINT uCmdID = 0, LPCTSTR lpcszCmdText = NULL);
	int reportSysError(int iSysErrorCode, LPCTSTR lpcszWhere = NULL, PAErrorLevel eErrorLevel = pael_WriteDebugWindow, LPCTSTR lpcszFilename = NULL, int iFileLine = -1, UINT uCmdID = 0, LPCTSTR lpcszCmdText = NULL);
protected:
	enum {UserType, SpecialType, SysType};
	void processError(int iErrorType, int iErrorCode, LPCTSTR lpcszWhere, int iErrorLevel, LPCTSTR lpcszFilename, int iFileLine, UINT uCmdID, LPCTSTR lpcszCmdText);
	void saveLog(LPSYSTEMTIME lptNowTime, LPCTSTR lpcszContent);
private:
	HWND m_hParentWnd;		//父窗口
	CMFCRibbonBaseElement* m_pCWndStatusElement;//状态窗口元素
	CMFCCaptionBar* m_pcCaptionBar;//消息窗口
	TCHAR m_szLogPath[MAX_PATH];	//日志目录
public:
	//static PC_PA_ERROR findError(int iErrorCode);
	PC_PA_ERROR findError(int iErrorCode);

	//这个不用了，替换为下面那个更通用版本
	//static const PA_ERROR l_tErrorInfo[];

	PA_ERROR	*m_pcErrorInf;
	int			m_nErrorNumber;

};


