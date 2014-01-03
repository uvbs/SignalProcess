#pragma once
////////////////////
//周立功USBCAN_I
//接口卡类型定义 -
#define VCI_PCI5121		1
#define VCI_PCI9810		2
#define VCI_USBCAN1		3	//缺省 -
#define VCI_USBCAN2		4
#define VCI_PCI9820		5
#define VCI_CAN232		6
//参数索引(周立功)
#define INIT_PARA_OFS_VCI_IDEV	0
#define INIT_PARA_OFS_VCI_ICAN	1
#define INIT_PARA_OFS_VCI_LOF	2

////////////////////
//若比特USB2CAN
#define RBT_USB2CAN		0x2000
//参数索引(若比特)
#define INIT_PARA_OFS_RBT_VCOM	0
#define INIT_PARA_OFS_RBT_VBPS	1
#define INIT_PARA_OFS_RBT_AUTORECONN	2

////////////////////
//舜宇恒平USB2CAN
#define SHP_USB2CAN		0x4000
//参数索引(若比特)
#define INIT_PARA_OFS_SHP_VCOM	0
#define INIT_PARA_OFS_SHP_VBPS	1
#define INIT_PARA_OFS_SHP_AUTORECONN	2

#ifndef LC_CAN_EXPORTS
#ifdef LC_CAN_LIB	//静态链接DLL

#define LC_CAN_DLL	__declspec(dllimport)
extern LC_CAN_DLL BOOL WINAPI LC_CAN_init(INT32 iMyAddrID, INT iDevType, INT iBPS, LPINT piPara);
extern LC_CAN_DLL void WINAPI LC_CAN_exit();
extern LC_CAN_DLL void WINAPI LC_CAN_setWindowMessage(HWND hWnd, UINT uMsg);
extern LC_CAN_DLL void WINAPI LC_CAN_setCallbackFunction(BOOL (CALLBACK* pCallback)(INT32, LPBYTE, LPVOID), LPVOID lpParam);
extern LC_CAN_DLL BOOL WINAPI LC_CAN_send(INT32 iObjID, LPBYTE lpdbLC);
extern LC_CAN_DLL BOOL WINAPI LC_CAN_recv(LPBYTE lpdbLC, INT32 iSrcID);
extern LC_CAN_DLL BOOL WINAPI LC_CAN_recvSpecial(LPBYTE lpdbLC, INT32 iSrcID, BYTE dbType, BYTE dbCmd);
extern LC_CAN_DLL INT WINAPI LC_CAN_listResponseID(INT32 iIDType, PINT32 piResponseID);
extern LC_CAN_DLL void WINAPI LC_CAN_getErrCode(LPINT piSendErrCode, LPINT piRecvErrCode);
extern LC_CAN_DLL BOOL WINAPI LC_CAN_checkRunning();

#else	//动态链接DLL

#define DLL_PROC_DEFINE(n)	n##_PROC
typedef BOOL (WINAPI * DLL_PROC_DEFINE(LC_CAN_init))(INT32, INT, INT, LPINT);
typedef void (WINAPI * DLL_PROC_DEFINE(LC_CAN_exit))();
typedef void (WINAPI * DLL_PROC_DEFINE(LC_CAN_setWindowMessage))(HWND, UINT);
typedef void (WINAPI * DLL_PROC_DEFINE(LC_CAN_setCallbackFunction))(BOOL (CALLBACK*)(INT32, LPBYTE, LPVOID), LPVOID);
typedef BOOL (WINAPI * DLL_PROC_DEFINE(LC_CAN_send))(INT32, LPBYTE);
typedef BOOL (WINAPI * DLL_PROC_DEFINE(LC_CAN_recv))(LPBYTE, INT32);
typedef BOOL (WINAPI * DLL_PROC_DEFINE(LC_CAN_recvSpecial))(LPBYTE, INT32, BYTE, BYTE);
typedef INT (WINAPI * DLL_PROC_DEFINE(LC_CAN_listResponseID))(INT32, PINT32);
typedef void (WINAPI * DLL_PROC_DEFINE(LC_CAN_getErrCode))(LPINT, LPINT);
typedef BOOL (WINAPI * DLL_PROC_DEFINE(LC_CAN_checkRunning))();
#undef DLL_PROC_DEFINE

#ifdef LC_CAN_DLL_PROC_ENTITY
#define DLL_PROC_DEFINE(n)	n##_PROC n
#else
#define DLL_PROC_DEFINE(n)	extern n##_PROC n
#endif
DLL_PROC_DEFINE(LC_CAN_init);
DLL_PROC_DEFINE(LC_CAN_exit);
DLL_PROC_DEFINE(LC_CAN_setWindowMessage);
DLL_PROC_DEFINE(LC_CAN_setCallbackFunction);
DLL_PROC_DEFINE(LC_CAN_send);
DLL_PROC_DEFINE(LC_CAN_recv);
DLL_PROC_DEFINE(LC_CAN_recvSpecial);
DLL_PROC_DEFINE(LC_CAN_listResponseID);
DLL_PROC_DEFINE(LC_CAN_getErrCode);
DLL_PROC_DEFINE(LC_CAN_checkRunning);
#undef DLL_PROC_DEFINE

class CLC_CAN_DLL {
public:
	CLC_CAN_DLL();
	~CLC_CAN_DLL();
	BOOL init(LPCTSTR lpcszDLLName);
private:
	HINSTANCE m_hInst;
};

#ifdef LC_CAN_DLL_PROC_ENTITY
CLC_CAN_DLL::CLC_CAN_DLL()
{
	m_hInst = NULL;
}
CLC_CAN_DLL::~CLC_CAN_DLL()
{
	if (m_hInst) FreeLibrary(m_hInst);
}
#define IF_NOT_GET_PROC(n)	if((n=(n##_PROC)GetProcAddress(m_hInst,#n))==NULL) 
BOOL CLC_CAN_DLL::init(LPCTSTR lpcszDLLName)
{
	if (lpcszDLLName == NULL) {
#ifdef _UNICODE
#ifdef _DEBUG
		lpcszDLLName = L"LC_CAN_D.dll";
#else
		lpcszDLLName = L"LC_CAN_R.dll";
#endif
#else
#ifdef _DEBUG
		lpcszDLLName = "LC_CAN_D.dll";
#else
		lpcszDLLName = "LC_CAN_R.dll";
#endif
#endif
	}
	if (((m_hInst = LoadLibrary(lpcszDLLName)) == NULL)) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_init) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_exit) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_setWindowMessage) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_setCallbackFunction) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_send) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_recv) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_recvSpecial) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_listResponseID) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_getErrCode) return FALSE;
	IF_NOT_GET_PROC(LC_CAN_checkRunning) return FALSE;
	return TRUE;
}
#undef IF_NOT_GET_PROC
static CLC_CAN_DLL l_cLC_CAN_DLL;
#define LC_CAN_DLL_INIT(n)	l_cLC_CAN_DLL.init(n)
#else
#define LC_CAN_DLL_INIT(l,n)	l.init(n)
#endif

#endif
#endif
