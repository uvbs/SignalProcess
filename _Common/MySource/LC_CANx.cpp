#include "stdafx.h"

#define LC_CAN_DLL_PROC_ENTITY
#include "LC_CAN.h"

#include "LC_CANx.h"

#define LC_LENGTH_OFS	1
#define LC_PARAM_OFS	7
#define LC_TYPE_OFS		4
#define LC_ADDR_OFS		5
#define LC_SUBCMD_OFS	6
#define LC_RETURN_ERRCODE(s)	(s[LC_TYPE_OFS]>=0x60)
#define PC_CAN_ID		0

////////////////////////////////////////
//局部函数::协议包装
static void WINAPI _xor_Data(LPBYTE p, int iStart, int iEnd)
{
	int i;
	BYTE dbXor = 0;
	for (i = iStart; i < iEnd; i++) {
		dbXor ^= p[i];
	}
	p[i] = dbXor;
}
static void WINAPI _make_LCCmd(BYTE dbLCCmd[MAX_LC_BUFFER_SIZE], enum LCType eType, enum LCSubCmd eSubCmd, int iParaLen = 0, LPBYTE lpdbPara = NULL)
{
	dbLCCmd[0] = dbLCCmd[3] = 0x68;
	dbLCCmd[1] = (BYTE)(iParaLen + 5);
	dbLCCmd[2] = ~dbLCCmd[1];
	dbLCCmd[4] = (BYTE)eType;
	dbLCCmd[5] = 0;
	dbLCCmd[6] = (BYTE)eSubCmd;
	if (lpdbPara) CopyMemory(&dbLCCmd[LC_PARAM_OFS], lpdbPara, iParaLen);
	_xor_Data(dbLCCmd, 4, iParaLen + LC_PARAM_OFS);
	dbLCCmd[iParaLen + LC_PARAM_OFS + 1] = 0x16;
}
////////////////////////////////////////
//局部函数::数据转换
__declspec(naked) void __cdecl LSB2MSB16(LPVOID lpMSB, WORD wLSB)
{
	__asm {
		push	ebp
		mov		ebp, esp
		mov		edx, lpMSB
		mov		ax, wLSB
		xchg	al, ah
		mov		[edx], ax
		leave
		ret
	}
}
__declspec(naked) void __cdecl LSB2MSB32(LPVOID lpMSB, DWORD dwLSB)
{
	__asm {
		push	ebp
		mov		ebp, esp
		mov		edx, lpMSB
		mov		eax, dwLSB
		xchg	al, ah
		ror		eax, 16
		xchg	al, ah
		mov		[edx], eax
		leave
		ret
	}
}
__declspec(naked) WORD __cdecl MSB2LSB16(LPCVOID lpMSB)
{
	__asm {
		push	ebp
		mov		ebp, esp
		mov		edx, lpMSB
		mov		ax, [edx]
		xchg	al, ah
		leave
		ret
	}
}
__declspec(naked) DWORD __cdecl MSB2LSB32(LPCVOID lpMSB)
{
	__asm {
		push	ebp
		mov		ebp, esp
		mov		edx, lpMSB
		mov		eax, [edx]
		xchg	al, ah
		ror		eax, 16
		xchg	al, ah
		leave
		ret
	}
}
int WINAPI BCD2INT(LPVOID lpBCD, int iLen)
{
	int i,iValue;
	LPBYTE p = (LPBYTE)lpBCD;
	for (iValue = 0, i = 1; i < iLen; i++) {
		iValue *= 10;
		if (i & 1) {
			iValue += (int)(*p & 0x0F);
			p++;
		}
		else {
			iValue += (int)(*p >> 4);
		}
	}
	if (*(LPBYTE)lpBCD & 0x80) {
		iValue = -iValue;
	}
	return iValue;
}
int WINAPI INT2BCD(LPVOID lpBCD, int iInt, int iLen)
{
	int i,j;
	BYTE dbTmp[32];
	LPBYTE p = (LPBYTE)lpBCD;
	if (iInt < 0) {
		*p = 0x80;
		iInt = -iInt;

	}
	else {
		*p = 0;
	}
	for (i = 1; i < iLen; i++) {
		dbTmp[i] = (BYTE)(iInt % 10);
		iInt /= 10;
	}
	for (j = 1; i > 1; j++) {
		i--;
		if (j & 1) {
			*p |= dbTmp[i];
			p++;
		}
		else {
			*p = dbTmp[i] << 4;
		}
		
	}
	return j;
}

////////////////////////////////////////
//设备配置连接函数 -
static CRITICAL_SECTION l_hCriticalSection;//临界区用于防止多线程同时发送 -
static BOOL l_bCriticalSectionActiveFlag = FALSE;//临界区是否有效 -
BOOL WINAPI LC_initDLL()//初始化DLL函数 -
{
	BOOL bRet;
	bRet = LC_CAN_DLL_INIT(NULL);
	if (!l_bCriticalSectionActiveFlag) {
		InitializeCriticalSection(&l_hCriticalSection);
		l_bCriticalSectionActiveFlag = TRUE;
	}
	return bRet;
}
void WINAPI LC_exitDLL()
{
	if (l_bCriticalSectionActiveFlag) {
		l_bCriticalSectionActiveFlag = FALSE;
		DeleteCriticalSection(&l_hCriticalSection);
	}
}
typedef struct {
	int iDevType;
	int iLevel;
}	VCOM2CAN_FIND_ORDER;
#define VCOM2CAN_DEV_NUM	2
static VCOM2CAN_FIND_ORDER l_tVCom2CanFindOrder[VCOM2CAN_DEV_NUM] = {
	{SHP_USB2CAN, 0},
	{RBT_USB2CAN, 1},
};
void WINAPI LC_setVCOM2CANFindOrder(int iDevType, int iLevel)//设置串口转CAN型设备的查找顺序 -
{
	int i;
	if (iLevel > VCOM2CAN_DEV_NUM) iLevel = VCOM2CAN_DEV_NUM;
	for (i = 0; i < VCOM2CAN_DEV_NUM; i++) {
		if (l_tVCom2CanFindOrder[i].iDevType == iDevType) l_tVCom2CanFindOrder[i].iLevel = iLevel;
	}
}
BOOL WINAPI LC_Connect(int &iComIdx)//连接设备 -
{
	BOOL bRet;
	int i,j,iParas[3] = {0, 0, 0};
	if (l_bCriticalSectionActiveFlag) EnterCriticalSection(&l_hCriticalSection);
	if (!iComIdx) {//周立功USBCAN-I设备 -
		bRet = LC_CAN_init(PC_CAN_ID, VCI_USBCAN1, 0, iParas);
	}
	else {//各类串口转CAN的设备 -
		for (bRet = FALSE, i = 0; i <= VCOM2CAN_DEV_NUM; i++) {
			for (j = 0; j < VCOM2CAN_DEV_NUM; j++) {
				if (l_tVCom2CanFindOrder[j].iLevel == i) {
					iParas[0] = iComIdx;
					bRet = LC_CAN_init(PC_CAN_ID, l_tVCom2CanFindOrder[j].iDevType, 0, iParas);
					if (bRet) {//成功找到设备 -
						if (iComIdx < 0) iComIdx = iParas[0];//如果是自动查找串口号的场合，返回实际串口号 -
						i = VCOM2CAN_DEV_NUM;
						break;
					}
				}
			}
		}
	}
	if (l_bCriticalSectionActiveFlag) LeaveCriticalSection(&l_hCriticalSection);
	return bRet;
}
void WINAPI LC_Close()//关闭设备 -
{
	if (l_bCriticalSectionActiveFlag) EnterCriticalSection(&l_hCriticalSection);
	LC_CAN_exit();
	if (l_bCriticalSectionActiveFlag) LeaveCriticalSection(&l_hCriticalSection);
}
void WINAPI LC_setWindowMessage(HWND hWnd, UINT uMsg)//设置窗口句柄和消息 -
{
	LC_CAN_setWindowMessage(hWnd, uMsg);
}
void WINAPI LC_setCallbackFunction(BOOL (CALLBACK* pCallback)(int, LPBYTE, LPVOID), LPVOID lpParam)//设置回调函数 -
{
	LC_CAN_setCallbackFunction(pCallback, lpParam);
}

#define WAIT_TIME_SPACE	50
static DWORD volatile l_dwWaitTimeout = 3000;
void WINAPI LC_setWaitTimeout(DWORD dwWaitTimeout)
{
	l_dwWaitTimeout = dwWaitTimeout;
}

////////////////////////////////////////
//通讯包装 -
extern void g_TraceWithTime(LPCTSTR s);
static int WINAPI _lc_can_send(int iCanID, LPBYTE lpdbData)
{
#ifdef TWEN_TRACE_COMMAND
	CString cmd;
	cmd.Format(_T("地址:%x;命令:"), iCanID);
	CString x;
	for(int i=0; i<20; i++)
	{
		x.Format(_T("%x"), lpdbData[i]);
		cmd+=x;
	}
	g_TraceWithTime(_T("[开始]") + cmd + _T("\n"));
#endif
	BOOL bRet;
	if (l_bCriticalSectionActiveFlag) EnterCriticalSection(&l_hCriticalSection);
	bRet = LC_CAN_send(iCanID, lpdbData);
	if (l_bCriticalSectionActiveFlag) LeaveCriticalSection(&l_hCriticalSection);
#ifdef TWEN_TRACE_COMMAND
	g_TraceWithTime(_T("[结束]") + cmd + _T("\n"));
#endif
	return (bRet) ? 0 : -1;
}
static int WINAPI _lc_can_recvSpecial(LPBYTE lpdbData, int iCanID, enum LCType eType, enum LCSubCmd eSubCmd)
{
#ifdef TWEN_TRACE_COMMAND
	CString cmd;
	cmd.Format(_T("地址:%x;LCType:%x;LCSubCmd:%x;命令:"), iCanID, eType, eSubCmd);
	CString x;
	for(int i=0; i<20; i++)
	{
		x.Format(_T("%x"), lpdbData[i]);
		cmd+=x;
	}
	g_TraceWithTime(_T("[开始]") + cmd + _T("\n"));
#endif
	DWORD i;
	BOOL bRet;
	for (i = 0; i <= l_dwWaitTimeout; i += WAIT_TIME_SPACE) {
		if (l_bCriticalSectionActiveFlag) EnterCriticalSection(&l_hCriticalSection);
		bRet = LC_CAN_recvSpecial(lpdbData, iCanID, (BYTE)eType, (BYTE)eSubCmd);
		if (l_bCriticalSectionActiveFlag) LeaveCriticalSection(&l_hCriticalSection);
		if (bRet)
		{
#ifdef TWEN_TRACE_COMMAND
			g_TraceWithTime(_T("[结束]") + cmd + _T("\n"));
#endif
			return 0;
		}
		Sleep(WAIT_TIME_SPACE);
	}
#ifdef TWEN_TRACE_COMMAND
	g_TraceWithTime(_T("[超时]") + cmd + _T("\n"));
#endif
	return -1;
}
static int WINAPI _lc_can_listResponseID(int iCanID, int* piID)
{
	int iIDType;
	BOOL bRet;
	switch(iCanID) {
	case lcDC_All:
		iIDType = -1;
		break;
	case lcDC_AllPump:
		iIDType = 0x300;
		break;
	default:
		iIDType = iCanID;
	}
	Sleep(3000);//采用固定等待时间
	//Sleep(l_dwWaitTimeout);
	if (l_bCriticalSectionActiveFlag) EnterCriticalSection(&l_hCriticalSection);
	bRet = LC_CAN_listResponseID(iIDType, piID);
	if (l_bCriticalSectionActiveFlag) LeaveCriticalSection(&l_hCriticalSection);
	return bRet;
}

////////////////////////////////////////
//异步命令区
//设备异步命令(发送)
int WINAPI LC_p_GetCurrentTime(int iCanID)//查询当前时间 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_All_Time);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetCurrentTime(int iCanID, DWORD dwTime)//设置当前时间 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS], dwTime);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_All_Time, 4);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetCanID(int iCanID, int iNewCanID)//设定设备CAN地址 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS], iNewCanID);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_All_Can, 4);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetCanInfo(int iCanID)//询问CAN节点属性 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_broadcast, lcsc_All_Can);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunWake(int iCanID, BOOL bOnFlag)//执行休眠 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_All_Rest, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetWarning(int iCanID, BYTE dbType, BYTE dbIdx)//查询报警 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbType;
	dbLCCmd[LC_PARAM_OFS + 1] = dbIdx;
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS + 2], 0);
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS + 6], 0);
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_All_Warning, 10);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetWarning(int iCanID, BYTE dbType, BYTE dbIdx, WORD wIdx)//报警复位 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbType;
	dbLCCmd[LC_PARAM_OFS + 1] = dbIdx;
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 2], wIdx);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_All_Warning, 4);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetRealtimeData(int iCanID)//实时数据 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_All_RealtimeData);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetLogicIndex(int iCanID, WORD wLogicIndex)//设置设备的逻辑编号 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wLogicIndex);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_All_LogicIndex, 2);
	return _lc_can_send(iCanID, dbLCCmd);
};
//紫外异步命令(发送)
int WINAPI LC_p_GetUVStatus(int iCanID)//查询紫外状态 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_UV_Status);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetUVWorkData(int iCanID, BOOL bMultiData)//查询紫外工作数据 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, (bMultiData) ? lcsc_UV_WorkMultiData : lcsc_UV_WorkData);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetUVDeuteriumLampTime(int iCanID)//查询氘灯累计运行时 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_UV_DeuteriumLamp);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetUVWavelength(int iCanID)//查询设定波长 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_UV_Wavelength);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetUVWavelengthTable(int iCanID, BYTE dbLine)//查询波长程序表
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_UV_WavelengthTable_Get);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetUVWavelengthScanRange(int iCanID)//查询波长扫描范围 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_UV_WavelengthScan);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetUVWavelengthSmooth(int iCanID, WORD wWavelength, BYTE dbSmoothValue)//设定波长和平滑指数 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wWavelength);
	dbLCCmd[LC_PARAM_OFS + 2] = dbSmoothValue;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_Wavelength, 3);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetUVWavelengthScan(int iCanID, BYTE dbWavelengthScan, WORD wStartWavelength, WORD wEndWavelength)//设定(执行)波长扫描 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbWavelengthScan;
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 1], wStartWavelength);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 3], wEndWavelength);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthScan, 5);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_ClsUVWavelengthTable(int iCanID)//清除波长程序表 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthTable_Cls);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetUVWavelengthTable(int iCanID, P_WAVELENGTH_TABLE pwt)//增加/修改波长程序表 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS], pwt->dwTime);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 4], pwt->wWavelength);
	dbLCCmd[LC_PARAM_OFS + 6] = pwt->dbSampleSpeed;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthTable_Add, 7);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_DelUVWavelengthTable(int iCanID, DWORD dwTime)//删除波长程序表行 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS], dwTime);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthTable_Del, 4);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetUVWavelengthZero(int iCanID, BOOL bZeroFlag)//更改调零设置 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bZeroFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthAdjust_Zero, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_AdjUVWavelength(int iCanID, WORD wParam)//校准波长 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wParam);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthAdjust_Adj, 2);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetUVDeuteriumLampTime(int iCanID)//归零氘灯时间 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = 1;//执行代码	必须为1
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_DeuteriumLamp, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunUVWavelengthScan(int iCanID, BOOL bOnFlag)//执行波长扫描 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_Run_WavelengthScan, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunUVDeuteriumLamp(int iCanID, BOOL bOnFlag)//执行氘灯开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_Run_DeuteriumLamp, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunUVWavelengthTable(int iCanID, BOOL bOnFlag)//执行程序波长 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_Run_WavelengthTable, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunUVWavelengthZero(int iCanID)//执行调零 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = 1;//执行动作	必须为1
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_Run_Zero, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunUVWavelengthSelfCheck(int iCanID)//波长自检 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthAdjust_Chk);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetUVWavelengthAdjust(int iCanID, short sAdjustValue)//设定波长校正值 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	WORD wValue;
	//波长校正值需要特殊处理 -
	if (sAdjustValue < 0)
		wValue = (-sAdjustValue & 0xFF) | 0x0100;
	else
		wValue = sAdjustValue & 0xFF;
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wValue);

	_make_LCCmd(dbLCCmd, lcT_set, lcsc_UV_WavelengthAdjust, 2);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetUVWavelengthAdjust(int iCanID)//查询波长校正值 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_UV_WavelengthAdjust);
	return _lc_can_send(iCanID, dbLCCmd);
}
//泵异步命令(发送)
int WINAPI LC_p_GetPumpConstantSpeed(int iCanID)//查询恒流速度 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_Speed);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetPumpGland(int iCanID)//查询密封圈 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_Gland);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpGlandZero(int iCanID)//密封圈次数回零 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Gland);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetPumpPressure(int iCanID)//查询泵状态 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_Status);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpConstantSpeed(int iCanID, WORD wConstantSpeed)//设置恒流速度 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wConstantSpeed);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Speed, 2);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpPressure(int iCanID, WORD wPressureHigh, WORD wPressureLow, BYTE dbUnit)//设置压力值范围 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wPressureHigh);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 2], wPressureLow);
	dbLCCmd[LC_PARAM_OFS + 4] = dbUnit;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Pressure, 5);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpWashParam(int iCanID, WORD wVelocity, WORD wTime)//设置洗脱参数 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wVelocity);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 2], wTime);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Wash, 4);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetPumpWashParam(int iCanID)//查询洗脱参数 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_Wash);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpPressureZero(int iCanID, WORD wPressureZero)//设置压力显示零位 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wPressureZero);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_PressureZero, 2);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpRippleCompress(int iCanID, WORD wRippleCompress)//设置脉动压缩 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wRippleCompress);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Ripple, 2);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_ClsPumpGradsTable(int iCanID)//清除压力梯度表 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_GradsTable_Cls);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetPumpGradsTable(int iCanID, P_GRADS_TABLE pgt)//增加/修改压力梯度表 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS], pgt->dwTime);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 4], pgt->wScaleA);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 6], pgt->wScaleB);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 8], pgt->wScaleC);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 10], pgt->wScaleD);
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 12], pgt->wVelocity);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_GradsTable_Add, 14);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_DelPumpGradsTable(int iCanID, DWORD dwTime)//删除压力梯度表行 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS], dwTime);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_GradsTable_Del, 4);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetPumpGradsTable(int iCanID, BYTE dbLine)//查询压力梯度表 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbLine;
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_GradsTable_Get, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetPumpPressureZero(int iCanID)//查询泵压力零位值 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_PressureZero);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunPumpCmd(int iCanID, enum LCPumpCmd eCmd)//泵执行命令 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)eCmd;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_RunCmd, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunPumpSwitch(int iCanID, BOOL bOnFlag)//执行泵开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Switch_Run, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunPumpWash(int iCanID, BOOL bOnFlag)//执行泵冲洗 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Switch_Wash, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunPumpGradsReady(int iCanID, BOOL bOnFlag)//执行梯度准备 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Switch_GradsReady, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunPumpGrads(int iCanID, BOOL bOnFlag)//执行梯度 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = (BYTE)bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Pump_Switch_Grads, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetPumpTime(int iCanID)//查询泵运行时间 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Pump_Time);
	return _lc_can_send(iCanID, dbLCCmd);
}
//色谱异步命令(发送)
int WINAPI LC_p_RunGCSample(int iCanID, BYTE dbChannel)//开始采样 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbChannel;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_All_RealtimeData, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetGCRealtimeData(int iCanID, BYTE dbChannel)//实时数据 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbChannel;
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_All_RealtimeData, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
//温控异步命令(发送)
int WINAPI LC_p_GetTempBPS(int iCanID)//查询温控通讯速率 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Temp_BPS);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetTempBPS(int iCanID, WORD wBPS)//设定温控通讯速率 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS], wBPS);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Temp_BPS, 2);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetTempTemp(int iCanID, BYTE dbAddr)//查询设定温度 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbAddr;
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_Temp_Temp, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetTempTemp(int iCanID, BYTE dbAddr, WORD wTemp)//设定温度 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbAddr;
	LSB2MSB16(&dbLCCmd[LC_PARAM_OFS + 1], wTemp);
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_Temp_Temp, 3);
	return _lc_can_send(iCanID, dbLCCmd);
}
//恒流源异步命令(发送)
int WINAPI LC_p_GetCCSVol(int iCanID)//查询恒流源电流 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_CCS_Vol);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetCCSTemp(int iCanID)//查询电导池温度 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_CCS_Temp);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetCCSTSw(int iCanID)//查询电导池温度开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_CCS_TSw);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetCCSTSw(int iCanID, BYTE eVal)//设置电导池温度开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = eVal;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_CCS_TSw, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetCCSRange(int iCanID)//查询量程开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_CCS_Range);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetCCSRange(int iCanID, BYTE eVal)//设置量程开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = eVal;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_CCS_Range, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetCCSDASw(int iCanID)//查询恒流开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_CCS_DASw);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_RunCCSDASw(int iCanID, BYTE bOnFlag)//设置恒流开关 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = bOnFlag;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_CCS_DASw, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_GetCCSDAVal(int iCanID)//查询恒流电流 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	_make_LCCmd(dbLCCmd, lcT_query, lcsc_CCS_DAVal);
	return _lc_can_send(iCanID, dbLCCmd);
}
int WINAPI LC_p_SetCCSDAVal(int iCanID, BYTE dbVal)//设置恒流电流 -
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = dbVal;
	_make_LCCmd(dbLCCmd, lcT_set, lcsc_CCS_DAVal, 1);
	return _lc_can_send(iCanID, dbLCCmd);
}

////////////////////////////////////////
//无参数类的共用 -
static int WINAPI _LC_r_Set_(int iCanID, enum LCType eType, enum LCSubCmd eSubCmd)
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, eType, eSubCmd)) == 0) {
		iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
	}
	return iRet;
}
//设备异步命令(接收)
int WINAPI LC_r_GetCurrentTime(int iCanID, DWORD &dwTime)//查询当前时间 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_All_Time)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dwTime = MSB2LSB32(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_SetCurrentTime(int iCanID)//设置当前时间 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_All_Time);
}
int WINAPI LC_r_SetCanID(int iCanID)//设定设备CAN地址 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_All_Can);
}
int WINAPI LC_r_GetCanInfo(int iCanID, P_CAN_INFO ptCanInfo)//询问CAN节点属性 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_broadcast, lcsc_All_Can)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			ptCanInfo->dbType = dbLCData[LC_PARAM_OFS];
			ptCanInfo->iCanID = (int)MSB2LSB32(&dbLCData[LC_PARAM_OFS + 1]);
			ptCanInfo->iDeviceIdx = (int)MSB2LSB32(&dbLCData[LC_PARAM_OFS + 5]);
		}
	}
	return iRet;
}
int WINAPI LC_r_RunWake(int iCanID)//执行休眠 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_All_Rest);
}
int WINAPI LC_r_GetWarning(int iCanID, P_WARNING_INFO ptWarningInfo, int &iNum)//查询报警 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int i,iRet;
	LPBYTE p;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_All_Warning)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			iNum = (int)(dbLCData[LC_LENGTH_OFS] - 5) / sizeof(WARNING_INFO);
			for (p = &dbLCData[LC_PARAM_OFS], i = 0; i < iNum; i++, p += sizeof(WARNING_INFO)) {
				ptWarningInfo[i].dbType = p[0];
				ptWarningInfo[i].dbIdx = p[1];
				ptWarningInfo[i].dwSetParam = MSB2LSB32(&p[2]);
				ptWarningInfo[i].dwNowParam = MSB2LSB32(&p[6]);
				ptWarningInfo[i].dwTime = MSB2LSB32(&p[10]);
				ptWarningInfo[i].wIdx = MSB2LSB16(&p[14]);
			}
		}
	}
	return iRet;
}
int WINAPI LC_r_SetWarning(int iCanID)//报警复位 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_All_Warning);
}
#ifdef _DEBUG
static void _saveErrorData(LPCTSTR lpcszFilename, LPVOID lpdbData, DWORD dwLen)
{
	HANDLE hf;
	DWORD dwSize;
	SYSTEMTIME st;
	CString szFilename;
	if (lpcszFilename == NULL) {
		GetLocalTime(&st);
		szFilename.Format(_T("E:\\yx.log\\%u%02u%02u_%02u%02u%02u.LC"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		lpcszFilename = (LPCTSTR)szFilename;
	}
	if ((hf = CreateFile(lpcszFilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL)) == INVALID_HANDLE_VALUE) return;
	WriteFile(hf, lpdbData, dwLen, &dwSize, NULL);
	CloseHandle(hf);
}
#endif
int WINAPI LC_r_GetRealtimeData(int iCanID, P_SAMPLE_RESULT ptSample, int &iNum, LCReturnCode &eStatus, DWORD &dwTime, DWORD &dwWaveLength)//实时数据 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int i,j,iRet;
	DWORD dwThisTime,dwLastTime;
	LPBYTE p;
	union {
		LONG lValue;
		float fValue;
	} tValue;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_All_RealtimeData)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) {
			iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
			switch(iRet) {
			case 5://特殊返回：采样开始!
			case 8://特殊返回：梯度开始!
			case 9://特殊返回：梯度结束!
				iNum = 0;
				eStatus = (LCReturnCode)iRet;
				ptSample->dwTime = dwTime = MSB2LSB32(&dbLCData[LC_PARAM_OFS + 2]);
				if (iRet != 5) dwWaveLength = MSB2LSB32(&dbLCData[LC_PARAM_OFS + 6]);
				iRet = lc_OK;
				break;
			}
		}
		else {
			iNum = (int)(dbLCData[LC_LENGTH_OFS] - 5) / sizeof(SAMPLE_RESULT);
			for (p = &dbLCData[LC_PARAM_OFS], j = 0, i = 0; i < iNum; i++, p += sizeof(SAMPLE_RESULT)) {
				dwThisTime = MSB2LSB32(p);
				tValue.lValue = (LONG)MSB2LSB32(&p[4]);//新协议：返回值是FLOAT类型!!!
				if (dwThisTime % 50 || (i && dwThisTime - dwLastTime > 200) || tValue.fValue < -2.0f || tValue.fValue > 2.0f) {
					continue;
				}
				ptSample[j].dwTime = dwThisTime;
				ptSample[j].fValue = tValue.fValue;
				j++;
				dwLastTime = dwThisTime;
			}
			iNum = j;
#ifdef _DEBUG
			if (i != j) {
				_saveErrorData(NULL, dbLCData, MAX_LC_BUFFER_SIZE);
			}
#endif
			eStatus = lc_OK;
		}
	}
	return iRet;
}
int WINAPI LC_r_SetLogicIndex(int iCanID)//设置设备的逻辑编号 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_All_LogicIndex);
}
//紫外异步命令(接收)
int WINAPI LC_r_GetUVStatus(int iCanID, BYTE &dbSysStatus, BYTE &dbWorkStatus)//查询紫外状态 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_UV_Status)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dbSysStatus = dbLCData[LC_PARAM_OFS];
			dbWorkStatus = dbLCData[LC_PARAM_OFS + 1];
		}
	}
	return iRet;
}
int WINAPI LC_r_GetUVWorkData(int iCanID, P_UV_WORK_DATA ptUVWorkData, int &iNum, BOOL bMultiData)//查询紫外工作数据 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int i,iRet;
	DWORD dwValue;
	LPBYTE p;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, (bMultiData) ? lcsc_UV_WorkMultiData : lcsc_UV_WorkData)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			iNum = (int)(dbLCData[LC_LENGTH_OFS] - 5) / sizeof(UV_WORK_DATA);
			for (p = &dbLCData[LC_PARAM_OFS], i = 0; i < iNum; i++, p += sizeof(UV_WORK_DATA)) {
				ptUVWorkData[i].wWavelength = MSB2LSB16(p);
				//吸光度需要特殊处理 -
				dwValue = MSB2LSB32(&p[2]);
				ptUVWorkData[i].lAbsorbency = (LONG)(dwValue & 0x00FFFFFF);
				if (dwValue >> 24) ptUVWorkData[i].lAbsorbency = -ptUVWorkData[i].lAbsorbency;

				ptUVWorkData[i].dwSampleValue = MSB2LSB32(&p[6]);
				ptUVWorkData[i].dwCompareValue = MSB2LSB32(&p[10]);
			}
		}
	}
	return iRet;
}
int WINAPI LC_r_GetUVDeuteriumLampTime(int iCanID, WORD &wDeuteriumTime)//查询氘灯累计运行时 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_UV_DeuteriumLamp)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wDeuteriumTime = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_GetUVWavelength(int iCanID, WORD &wWavelength)//查询设定波长 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_UV_Wavelength)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wWavelength = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_GetUVWavelengthTable(int iCanID, BYTE &dbTotalLines, BYTE &dbNowLine, P_WAVELENGTH_TABLE pwt)//查询波长程序表
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_UV_WavelengthTable_Get)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dbTotalLines = dbLCData[LC_PARAM_OFS];
			dbNowLine = dbLCData[LC_PARAM_OFS + 1];
			pwt->dwTime = MSB2LSB32(&dbLCData[LC_PARAM_OFS + 2]);
			pwt->wWavelength = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 6]);
			pwt->dbSampleSpeed = dbLCData[LC_PARAM_OFS + 8];
		}
	}
	return iRet;
}
int WINAPI LC_r_GetUVWavelengthScanRange(int iCanID, WORD &wStartWavelength, WORD &wEndWavelength)//查询波长扫描范围 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_UV_WavelengthScan)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wStartWavelength = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
			wEndWavelength = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 2]);
		}
	}
	return iRet;
}
int WINAPI LC_r_SetUVWavelengthSmooth(int iCanID)//设定波长和平滑指数 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_Wavelength);
}
int WINAPI LC_r_SetUVWavelengthScan(int iCanID)//设定(执行)波长扫描 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthScan);
}
int WINAPI LC_r_ClsUVWavelengthTable(int iCanID)//清除波长程序表 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthTable_Cls);
}
int WINAPI LC_r_SetUVWavelengthTable(int iCanID)//增加/修改波长程序表 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthTable_Add);
}
int WINAPI LC_r_DelUVWavelengthTable(int iCanID)//删除波长程序表行 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthTable_Del);
}
int WINAPI LC_r_SetUVWavelengthZero(int iCanID)//更改调零设置 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthAdjust_Zero);
}
int WINAPI LC_r_AdjUVWavelength(int iCanID)//校准波长 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthAdjust_Adj);
}
int WINAPI LC_r_SetUVDeuteriumLampTime(int iCanID)//归零氘灯时间 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_DeuteriumLamp);
}
int WINAPI LC_r_RunUVWavelengthScan(int iCanID)//执行波长扫描 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_Run_WavelengthScan);
}
int WINAPI LC_r_RunUVDeuteriumLamp(int iCanID)//执行氘灯开关 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_Run_DeuteriumLamp);
}
int WINAPI LC_r_RunUVWavelengthTable(int iCanID)//执行程序波长 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_Run_WavelengthTable);
}
int WINAPI LC_r_RunUVWavelengthZero(int iCanID)//执行调零 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_Run_Zero);
}
int WINAPI LC_r_RunUVWavelengthSelfCheck(int iCanID)//波长自检 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthAdjust_Chk);
}
int WINAPI LC_r_SetUVWavelengthAdjust(int iCanID)//设定波长校正值 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_UV_WavelengthAdjust);
}
int WINAPI LC_r_GetUVWavelengthAdjust(int iCanID, short &sAdjustValue)//查询波长校正值 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	WORD wValue;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_UV_WavelengthAdjust)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			//波长校正值需要特殊处理 -
			wValue = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
			sAdjustValue = (short)(wValue & 0x00FF);
			if (wValue >> 8) sAdjustValue = -sAdjustValue;
		}
	}
	return iRet;
}
//泵异步命令(接收)
int WINAPI LC_r_GetPumpConstantSpeed(int iCanID, WORD &wConstantSpeed)//查询恒流速度 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_Speed)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wConstantSpeed = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_GetPumpGland(int iCanID, DWORD &dwLeftGland, DWORD &dwRightGland)//查询密封圈 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_Gland)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dwLeftGland = MSB2LSB32(&dbLCData[LC_PARAM_OFS]);
			dwRightGland = MSB2LSB32(&dbLCData[LC_PARAM_OFS + 4]);
		}
	}
	return iRet;
}
int WINAPI LC_r_SetPumpGlandZero(int iCanID)//密封圈次数回零 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Gland);
}
int WINAPI LC_r_GetPumpPressure(int iCanID, WORD &wPressure, BYTE &dbUnit, DWORD &dwPressureRange, WORD &wVelocity, BYTE &dbSysStatus, BYTE &dbWorkStatus)//查询泵状态 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_Status)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wPressure = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
			dbUnit = dbLCData[LC_PARAM_OFS + 2];
			dwPressureRange = MSB2LSB32(&dbLCData[LC_PARAM_OFS + 3]);
			wVelocity = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 7]);
			dbSysStatus = dbLCData[LC_PARAM_OFS + 9];
			dbWorkStatus = dbLCData[LC_PARAM_OFS + 10];
		}
	}
	return iRet;
}
int WINAPI LC_r_SetPumpConstantSpeed(int iCanID)//设置恒流速度 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Speed);
}
int WINAPI LC_r_SetPumpPressure(int iCanID)//设置压力值范围 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Pressure);
}
int WINAPI LC_r_SetPumpWashParam(int iCanID)//设置洗脱参数 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Wash);
}
int WINAPI LC_r_GetPumpWashParam(int iCanID, WORD &wVelocity, WORD &wTime)//查询洗脱参数 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_Wash)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wVelocity = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
			wTime = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 2]);
		}
	}
	return iRet;
}
int WINAPI LC_r_SetPumpPressureZero(int iCanID)//设置压力显示零位 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_PressureZero);
}
int WINAPI LC_r_SetPumpRippleCompress(int iCanID)//设置脉动压缩 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Ripple);
}
int WINAPI LC_r_ClsPumpGradsTable(int iCanID)//清除压力梯度表 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_GradsTable_Cls);
}
int WINAPI LC_r_SetPumpGradsTable(int iCanID)//增加/修改压力梯度表 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_GradsTable_Add);
}
int WINAPI LC_r_DelPumpGradsTable(int iCanID)//删除压力梯度表行 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_GradsTable_Del);
}
int WINAPI LC_r_GetPumpGradsTable(int iCanID, BYTE &dbTotalLines, BYTE &dbNowLine, P_GRADS_TABLE pgt)//查询压力梯度表 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_GradsTable_Get)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dbTotalLines = dbLCData[LC_PARAM_OFS];
			if (dbNowLine = dbLCData[LC_PARAM_OFS + 1]) {
				pgt->dwTime = MSB2LSB32(&dbLCData[LC_PARAM_OFS + 2]);
				pgt->wScaleA = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 6]);
				pgt->wScaleB = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 8]);
				pgt->wScaleC = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 10]);
				pgt->wScaleD = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 12]);
				pgt->wVelocity = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 14]);
			}
		}
	}
	return iRet;
}
int WINAPI LC_r_GetPumpPressureZero(int iCanID, WORD &wPressureZero)//查询泵压力零位值 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_PressureZero)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wPressureZero = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_RunPumpCmd(int iCanID)//泵执行命令 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_RunCmd);
}
int WINAPI LC_r_RunPumpSwitch(int iCanID)//执行泵开关 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Switch_Run);
}
int WINAPI LC_r_RunPumpWash(int iCanID)//执行泵冲洗 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Switch_Wash);
}
int WINAPI LC_r_RunPumpGradsReady(int iCanID)//执行梯度准备 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Switch_GradsReady);
}
int WINAPI LC_r_RunPumpGrads(int iCanID)//执行梯度 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Pump_Switch_Grads);
}
int WINAPI LC_r_GetPumpTime(int iCanID, DWORD &dwPumpTime)//查询泵运行时间 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Pump_Time)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dwPumpTime = MSB2LSB32(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
//色谱异步命令(接收)
int WINAPI LC_r_RunGCSample(int iCanID)//开始采样 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_All_RealtimeData);
}
int WINAPI LC_r_GetGCRealtimeData(int iCanID, P_SAMPLE_RESULT ptSample, int &iNum, BOOL &bStartFlag)//实时数据 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int i,j,iRet;
	DWORD dwThisTime,dwLastTime;
	LPBYTE p;
	union {
		LONG lValue;
		float fValue;
	} tValue;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_All_RealtimeData)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			iNum = (int)(dbLCData[LC_LENGTH_OFS] - 5) / sizeof(SAMPLE_RESULT);
			bStartFlag = (dbLCData[LC_ADDR_OFS] & 0x80) ? TRUE : FALSE;
			for (p = &dbLCData[LC_PARAM_OFS], j = 0, i = 0; i < iNum; i++, p += sizeof(SAMPLE_RESULT)) {
				dwThisTime = MSB2LSB32(p);
				tValue.lValue = (LONG)MSB2LSB32(&p[4]);//新协议：返回值是FLOAT类型!!!
				ptSample[j].dwTime = dwThisTime;
				ptSample[j].fValue = tValue.fValue;
				j++;
				dwLastTime = dwThisTime;
			}
			iNum = j;
		}
	}
	return iRet;
}
//温控异步命令(接收)
int WINAPI LC_r_GetTempBPS(int iCanID, WORD &wBPS)//查询温控通讯速率 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Temp_BPS)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wBPS = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_SetTempBPS(int iCanID)//设定温控通讯速率 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Temp_BPS);
}
int WINAPI LC_r_GetTempTemp(int iCanID, BYTE &dbAddr, WORD &wTrueTemp, WORD &wTemp)//查询设定温度 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_Temp_Temp)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dbAddr = dbLCData[LC_PARAM_OFS];
			wTrueTemp = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 1]);
			wTemp = MSB2LSB16(&dbLCData[LC_PARAM_OFS + 3]);
		}
	}
	return iRet;
}
int WINAPI LC_r_SetTempTemp(int iCanID)//设定温度 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_Temp_Temp);
}
//恒流源异步命令(接收)
int WINAPI LC_r_GetCCSVol(int iCanID, WORD &wVol)//查询恒流源电流 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_CCS_Vol)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wVol = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_GetCCSTemp(int iCanID, WORD &wTemp)//查询电导池温度 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_CCS_Temp)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			wTemp = MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		}
	}
	return iRet;
}
int WINAPI LC_r_GetCCSTSw(int iCanID, BYTE &eVal)//查询电导池温度开关 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_CCS_TSw)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			eVal = dbLCData[LC_PARAM_OFS];
		}
	}
	return iRet;
}
int WINAPI LC_r_SetCCSTSw(int iCanID)//设置电导池温度开关 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_CCS_TSw);
}
int WINAPI LC_r_GetCCSRange(int iCanID, BYTE &eVal)//查询量程开关 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_CCS_Range)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			eVal = dbLCData[LC_PARAM_OFS];
		}
	}
	return iRet;
}
int WINAPI LC_r_SetCCSRange(int iCanID)//设置量程开关 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_CCS_Range);
}
int WINAPI LC_r_GetCCSDASw(int iCanID, BYTE &bOnFlag)//查询恒流开关 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_CCS_DASw)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			bOnFlag = dbLCData[LC_PARAM_OFS];
		}
	}
	return iRet;
}
int WINAPI LC_r_RunCCSDASw(int iCanID)//设置恒流开关 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_CCS_DASw);
}
int WINAPI LC_r_GetCCSDAVal(int iCanID, BYTE &dbVal, BYTE &dbNowVal)//查询恒流电流 -
{
	BYTE dbLCData[MAX_LC_BUFFER_SIZE];
	int iRet;
	if ((iRet = _lc_can_recvSpecial(dbLCData, iCanID, lcT_query, lcsc_CCS_DAVal)) == 0) {
		if (LC_RETURN_ERRCODE(dbLCData)) iRet = (int)MSB2LSB16(&dbLCData[LC_PARAM_OFS]);
		else {
			dbVal = dbLCData[LC_PARAM_OFS];
			dbNowVal = dbLCData[LC_PARAM_OFS + 1];
		}
	}
	return iRet;
}
int WINAPI LC_r_SetCCSDAVal(int iCanID)//设置恒流电流 -
{
	return _LC_r_Set_(iCanID, lcT_set, lcsc_CCS_DAVal);
}

////////////////////////////////////////
//同步命令区
//设备同步命令 -
int WINAPI LC_GetCurrentTime(int iCanID, DWORD &dwTime)//查询当前时间 -
{
	int iRet;
	if ((iRet = LC_p_GetCurrentTime(iCanID)) == 0)
		iRet = LC_r_GetCurrentTime(iCanID, dwTime);
	return iRet;
}
int WINAPI LC_SetCurrentTime(int iCanID, DWORD dwTime)//设置当前时间 -
{
	int iRet;
	if ((iRet = LC_p_SetCurrentTime(iCanID, dwTime)) == 0)
		iRet = LC_r_SetCurrentTime(iCanID);
	return iRet;
}
int WINAPI LC_SetCanID(int iCanID, int iNewCanID)//设定设备CAN地址 -
{
	int iRet;
	if ((iRet = LC_p_SetCanID(iCanID, iNewCanID)) == 0)
		iRet = LC_r_SetCanID(iCanID);
	return iRet;
}
int WINAPI LC_GetCanInfo(int iCanID, P_CAN_INFO ptCanInfo, int &iNum)//询问CAN节点属性 -
{
	int i,j,iTotal,iRet,iCanIDs[32];
	if ((iRet = LC_p_GetCanInfo(iCanID)) == 0) {
		iTotal = _lc_can_listResponseID(iCanID, iCanIDs);
		for (j = i = 0; i < iTotal; i++) {
			if (LC_r_GetCanInfo(iCanIDs[i], &ptCanInfo[j]) == 0) {
				j++;
			}
		}
		iNum = j;
	}
	return iRet;
}
int WINAPI LC_RunWake(int iCanID, BOOL bOnFlag)//执行休眠 -
{
	int iRet;
	if ((iRet = LC_p_RunWake(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunWake(iCanID);
	return iRet;
}
int WINAPI LC_GetWarning(int iCanID, BYTE dbType, BYTE dbIdx, P_WARNING_INFO ptWarningInfo, int &iNum)//查询报警 -
{
	int iRet;
	if ((iRet = LC_p_GetWarning(iCanID, dbType, dbIdx)) == 0)
		iRet = LC_r_GetWarning(iCanID, ptWarningInfo, iNum);
	return iRet;
}
int WINAPI LC_SetWarning(int iCanID, BYTE dbType, BYTE dbIdx, WORD wIdx)//报警复位 -
{
	int iRet;
	if ((iRet = LC_p_SetWarning(iCanID, dbType, dbIdx, wIdx)) == 0)
		iRet = LC_r_SetWarning(iCanID);
	return iRet;
}
int WINAPI LC_GetRealtimeData(int iCanID, P_SAMPLE_RESULT ptSample, int &iNum, LCReturnCode &eStatus, DWORD &dwTime, DWORD &dwWaveLength)//实时数据 -
{
	int iRet;
	if ((iRet = LC_p_GetRealtimeData(iCanID)) == 0)
		iRet = LC_r_GetRealtimeData(iCanID, ptSample, iNum, eStatus, dwTime, dwWaveLength);
	return iRet;
}
int WINAPI LC_SetLogicIndex(int iCanID, WORD wLogicIndex)//设置设备的逻辑编号 -
{
	int iRet;
	if ((iRet = LC_p_SetLogicIndex(iCanID, wLogicIndex)) == 0)
		iRet = LC_r_SetLogicIndex(iCanID);
	return iRet;
}
//紫外同步命令 -
int WINAPI LC_GetUVStatus(int iCanID, BYTE &dbSysStatus, BYTE &dbWorkStatus)//查询紫外状态 -
{
	int iRet;
	if ((iRet = LC_p_GetUVStatus(iCanID)) == 0)
		iRet = LC_r_GetUVStatus(iCanID, dbSysStatus, dbWorkStatus);
	return iRet;
}
int WINAPI LC_GetUVWorkData(int iCanID, P_UV_WORK_DATA ptUVWorkData, int &iNum, BOOL bMultiData)//查询紫外工作数据 -
{
	int iRet;
	if ((iRet = LC_p_GetUVWorkData(iCanID, bMultiData)) == 0)
		iRet = LC_r_GetUVWorkData(iCanID, ptUVWorkData, iNum, bMultiData);
	return iRet;
}
int WINAPI LC_GetUVDeuteriumLampTime(int iCanID, WORD &wDeuteriumTime)//查询氘灯累计运行时 -
{
	int iRet;
	if ((iRet = LC_p_GetUVDeuteriumLampTime(iCanID)) == 0)
		iRet = LC_r_GetUVDeuteriumLampTime(iCanID, wDeuteriumTime);
	return iRet;
}
int WINAPI LC_GetUVWavelength(int iCanID, WORD &wWavelength)//查询设定波长 -
{
	int iRet;
	if ((iRet = LC_p_GetUVWavelength(iCanID)) == 0)
		iRet = LC_r_GetUVWavelength(iCanID, wWavelength);
	return iRet;
}
int WINAPI LC_GetUVWavelengthTable(int iCanID, BYTE dbLine, BYTE &dbTotalLines, BYTE &dbNowLine, P_WAVELENGTH_TABLE pwt)//查询波长程序表
{
	int iRet;
	if ((iRet = LC_p_GetUVWavelengthTable(iCanID, dbLine)) == 0)
		iRet = LC_r_GetUVWavelengthTable(iCanID, dbTotalLines, dbNowLine, pwt);
	return iRet;
}
int WINAPI LC_GetUVWavelengthScanRange(int iCanID, WORD &wStartWavelength, WORD &wEndWavelength)//查询波长扫描范围 -
{
	int iRet;
	if ((iRet = LC_p_GetUVWavelengthScanRange(iCanID)) == 0)
		iRet = LC_r_GetUVWavelengthScanRange(iCanID, wStartWavelength, wEndWavelength);
	return iRet;
}
int WINAPI LC_SetUVWavelengthSmooth(int iCanID, WORD wWavelength, BYTE dbSmoothValue)//设定波长和平滑指数 -
{
	int iRet;
	if ((iRet = LC_p_SetUVWavelengthSmooth(iCanID, wWavelength, dbSmoothValue)) == 0)
		iRet = LC_r_SetUVWavelengthSmooth(iCanID);
	return iRet;
}
int WINAPI LC_SetUVWavelengthScan(int iCanID, BYTE dbWavelengthScan, WORD wStartWavelength, WORD wEndWavelength)//设定(执行)波长扫描 -
{
	int iRet;
	if ((iRet = LC_p_SetUVWavelengthScan(iCanID, dbWavelengthScan, wStartWavelength, wEndWavelength)) == 0)
		iRet = LC_r_SetUVWavelengthScan(iCanID);
	return iRet;
}
int WINAPI LC_ClsUVWavelengthTable(int iCanID)//清除波长程序表 -
{
	int iRet;
	if ((iRet = LC_p_ClsUVWavelengthTable(iCanID)) == 0)
		iRet = LC_r_ClsUVWavelengthTable(iCanID);
	return iRet;
}
int WINAPI LC_SetUVWavelengthTable(int iCanID, P_WAVELENGTH_TABLE pwt)//增加/修改波长程序表 -
{
	int iRet;
	if ((iRet = LC_p_SetUVWavelengthTable(iCanID, pwt)) == 0)
		iRet = LC_r_SetUVWavelengthTable(iCanID);
	return iRet;
}
int WINAPI LC_DelUVWavelengthTable(int iCanID, DWORD dwTime)//删除波长程序表行 -
{
	int iRet;
	if ((iRet = LC_p_DelUVWavelengthTable(iCanID, dwTime)) == 0)
		iRet = LC_r_DelUVWavelengthTable(iCanID);
	return iRet;
}
int WINAPI LC_SetUVWavelengthZero(int iCanID, BOOL bZeroFlag)//更改调零设置 -
{
	int iRet;
	if ((iRet = LC_p_SetUVWavelengthZero(iCanID, bZeroFlag)) == 0)
		iRet = LC_r_SetUVWavelengthZero(iCanID);
	return iRet;
}
int WINAPI LC_AdjUVWavelength(int iCanID, WORD wParam)//校准波长 -
{
	int iRet;
	if ((iRet = LC_p_AdjUVWavelength(iCanID, wParam)) == 0)
		iRet = LC_r_AdjUVWavelength(iCanID);
	return iRet;
}
int WINAPI LC_SetUVDeuteriumLampTime(int iCanID)//归零氘灯时间 -
{
	int iRet;
	if ((iRet = LC_p_SetUVDeuteriumLampTime(iCanID)) == 0)
		iRet = LC_r_SetUVDeuteriumLampTime(iCanID);
	return iRet;
}
int WINAPI LC_RunUVWavelengthScan(int iCanID, BOOL bOnFlag)//执行波长扫描 -
{
	int iRet;
	if ((iRet = LC_p_RunUVWavelengthScan(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunUVWavelengthScan(iCanID);
	return iRet;
}
int WINAPI LC_RunUVDeuteriumLamp(int iCanID, BOOL bOnFlag)//执行氘灯开关 -
{
	int iRet;
	if ((iRet = LC_p_RunUVDeuteriumLamp(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunUVDeuteriumLamp(iCanID);
	return iRet;
}
int WINAPI LC_RunUVWavelengthTable(int iCanID, BOOL bOnFlag)//执行程序波长 -
{
	int iRet;
	if ((iRet = LC_p_RunUVWavelengthTable(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunUVWavelengthTable(iCanID);
	return iRet;
}
int WINAPI LC_RunUVWavelengthZero(int iCanID)//执行调零 -
{
	int iRet;
	if ((iRet = LC_p_RunUVWavelengthZero(iCanID)) == 0)
		iRet = LC_r_RunUVWavelengthZero(iCanID);
	return iRet;
}
int WINAPI LC_RunUVWavelengthSelfCheck(int iCanID)//波长自检 -
{
	int iRet;
	if ((iRet = LC_p_RunUVWavelengthSelfCheck(iCanID)) == 0)
		iRet = LC_r_RunUVWavelengthSelfCheck(iCanID);
	return iRet;
}
int WINAPI LC_SetUVWavelengthAdjust(int iCanID, short sAdjustValue)//设定波长校正值 -
{
	int iRet;
	if ((iRet = LC_p_SetUVWavelengthAdjust(iCanID, sAdjustValue)) == 0)
		iRet = LC_r_SetUVWavelengthAdjust(iCanID);
	return iRet;
}
int WINAPI LC_GetUVWavelengthAdjust(int iCanID, short &sAdjustValue)//查询波长校正值 -
{
	int iRet;
	if ((iRet = LC_p_GetUVWavelengthAdjust(iCanID)) == 0)
		iRet = LC_r_GetUVWavelengthAdjust(iCanID, sAdjustValue);
	return iRet;
}
//泵同步命令 -
int WINAPI LC_GetPumpConstantSpeed(int iCanID, WORD &wConstantSpeed)//查询恒流速度 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpConstantSpeed(iCanID)) == 0)
		iRet = LC_r_GetPumpConstantSpeed(iCanID, wConstantSpeed);
	return iRet;
}
int WINAPI LC_GetPumpGland(int iCanID, DWORD &dwLeftGland, DWORD &dwRightGland)//查询密封圈 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpGland(iCanID)) == 0)
		iRet = LC_r_GetPumpGland(iCanID, dwLeftGland, dwRightGland);
	return iRet;
}
int WINAPI LC_SetPumpGlandZero(int iCanID)//密封圈次数回零 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpGlandZero(iCanID)) == 0)
		iRet = LC_r_SetPumpGlandZero(iCanID);
	return iRet;
}
int WINAPI LC_GetPumpPressure(int iCanID, WORD &wPressure, BYTE &dbUnit, DWORD &dwPressureRange, WORD &wVelocity, BYTE &dbSysStatus, BYTE &dbWorkStatus)//查询泵状态 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpPressure(iCanID)) == 0)
		iRet = LC_r_GetPumpPressure(iCanID, wPressure, dbUnit, dwPressureRange, wVelocity, dbSysStatus, dbWorkStatus);
	return iRet;
}
int WINAPI LC_SetPumpConstantSpeed(int iCanID, WORD wConstantSpeed)//设置恒流速度 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpConstantSpeed(iCanID, wConstantSpeed)) == 0)
		iRet = LC_r_SetPumpConstantSpeed(iCanID);
	return iRet;
}
int WINAPI LC_SetPumpPressure(int iCanID, WORD wPressureHigh, WORD wPressureLow, BYTE dbUnit)//设置压力值范围 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpPressure(iCanID, wPressureHigh, wPressureLow, dbUnit)) == 0)
		iRet = LC_r_SetPumpPressure(iCanID);
	return iRet;
}
int WINAPI LC_SetPumpWashParam(int iCanID, WORD wVelocity, WORD wTime)//设置洗脱参数 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpWashParam(iCanID, wVelocity, wTime)) == 0)
		iRet = LC_r_SetPumpWashParam(iCanID);
	return iRet;
}
int WINAPI LC_GetPumpWashParam(int iCanID, WORD &wVelocity, WORD &wTime)//查询洗脱参数 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpWashParam(iCanID)) == 0)
		iRet = LC_r_GetPumpWashParam(iCanID, wVelocity, wTime);
	return iRet;
}
int WINAPI LC_SetPumpPressureZero(int iCanID, WORD wPressureZero)//设置压力显示零位 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpPressureZero(iCanID, wPressureZero)) == 0)
		iRet = LC_r_SetPumpPressureZero(iCanID);
	return iRet;
}
int WINAPI LC_SetPumpRippleCompress(int iCanID, WORD wRippleCompress)//设置脉动压缩 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpRippleCompress(iCanID, wRippleCompress)) == 0)
		iRet = LC_r_SetPumpRippleCompress(iCanID);
	return iRet;
}
int WINAPI LC_ClsPumpGradsTable(int iCanID)//清除压力梯度表 -
{
	int iRet;
	if ((iRet = LC_p_ClsPumpGradsTable(iCanID)) == 0)
		iRet = LC_r_ClsPumpGradsTable(iCanID);
	return iRet;
}
int WINAPI LC_SetPumpGradsTable(int iCanID, P_GRADS_TABLE pgt)//增加/修改压力梯度表 -
{
	int iRet;
	if ((iRet = LC_p_SetPumpGradsTable(iCanID, pgt)) == 0)
		iRet = LC_r_SetPumpGradsTable(iCanID);
	return iRet;
}
int WINAPI LC_DelPumpGradsTable(int iCanID, DWORD dwTime)//删除压力梯度表行 -
{
	int iRet;
	if ((iRet = LC_p_DelPumpGradsTable(iCanID, dwTime)) == 0)
		iRet = LC_r_DelPumpGradsTable(iCanID);
	return iRet;
}
int WINAPI LC_GetPumpGradsTable(int iCanID, BYTE dbLine, BYTE &dbTotalLines, BYTE &dbNowLine, P_GRADS_TABLE pgt)//查询压力梯度表 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpGradsTable(iCanID, dbLine)) == 0)
		iRet = LC_r_GetPumpGradsTable(iCanID, dbTotalLines, dbNowLine, pgt);
	return iRet;
}
int WINAPI LC_GetPumpPressureZero(int iCanID, WORD &wPressureZero)//查询泵压力零位值 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpPressureZero(iCanID)) == 0)
		iRet = LC_r_GetPumpPressureZero(iCanID, wPressureZero);
	return iRet;
}
int WINAPI LC_RunPumpCmd(int iCanID, enum LCPumpCmd eCmd)//泵执行命令 -
{
	int iRet;
	if ((iRet = LC_p_RunPumpCmd(iCanID, eCmd)) == 0)
		iRet = LC_r_RunPumpCmd(iCanID);
	return iRet;
}
int WINAPI LC_RunPumpSwitch(int iCanID, BOOL bOnFlag)//执行泵开关 -
{
	int iRet;
	if ((iRet = LC_p_RunPumpSwitch(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunPumpSwitch(iCanID);
	return iRet;
}
int WINAPI LC_RunPumpWash(int iCanID, BOOL bOnFlag)//执行泵冲洗 -
{
	int iRet;
	if ((iRet = LC_p_RunPumpWash(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunPumpWash(iCanID);
	return iRet;
}
int WINAPI LC_RunPumpGradsReady(int iCanID, BOOL bOnFlag)//执行梯度准备 -
{
	int iRet;
	if ((iRet = LC_p_RunPumpGradsReady(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunPumpGradsReady(iCanID);
	return iRet;
}
int WINAPI LC_RunPumpGrads(int iCanID, BOOL bOnFlag)//执行梯度 -
{
	int iRet;
	if ((iRet = LC_p_RunPumpGrads(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunPumpGrads(iCanID);
	return iRet;
}
int WINAPI LC_GetPumpTime(int iCanID, DWORD &dwPumpTime)//查询泵压力零位值 -
{
	int iRet;
	if ((iRet = LC_p_GetPumpTime(iCanID)) == 0)
		iRet = LC_r_GetPumpTime(iCanID, dwPumpTime);
	return iRet;
}
//色谱同步命令 -
int WINAPI LC_RunGCSample(int iCanID, BYTE dbChannel)//开始采样 -
{
	int iRet;
	if ((iRet = LC_p_RunGCSample(iCanID, dbChannel)) == 0)
		iRet = LC_r_RunGCSample(iCanID);
	return iRet;
}
int WINAPI LC_GetGCRealtimeData(int iCanID, BYTE dbChannel, P_SAMPLE_RESULT ptSample, int &iNum, BOOL &bStartFlag)//实时数据 -
{
	int iRet;
	if ((iRet = LC_p_GetGCRealtimeData(iCanID, dbChannel)) == 0)
		iRet = LC_r_GetGCRealtimeData(iCanID, ptSample, iNum, bStartFlag);
	return iRet;
}
void WINAPI LC_TranslateGCRealtimeData(P_SAMPLE_RESULT ptDst, P_SAMPLE_RESULT ptSrc, int iNum, BYTE eVal)//根据量程转换结果 -
{
	int i;
	float fMul,fMax;
	switch(eVal) {
	case RANGE_uS:
		fMul = 1000.0f;
		fMax = 1024.0f;
		break;
	case RANGE_mS:
		fMul = 10000.0f;
		fMax = 4000.0f;
		break;
	default:
		return;
	}
	for (i = 0; i < iNum; i++) {
		if ((ptDst[i].fValue = ptSrc[i].fValue * fMul) > fMax) ptDst[i].fValue = fMax;
	}
}
//温控同步命令 -
int WINAPI LC_GetTempBPS(int iCanID, WORD &wBPS)//查询温控通讯速率 -
{
	int iRet;
	if ((iRet = LC_p_GetTempBPS(iCanID)) == 0)
		iRet = LC_r_GetTempBPS(iCanID, wBPS);
	return iRet;
}
int WINAPI LC_SetTempBPS(int iCanID, WORD wBPS)//设定温控通讯速率 -
{
	int iRet;
	if ((iRet = LC_p_SetTempBPS(iCanID, wBPS)) == 0)
		iRet = LC_r_SetTempBPS(iCanID);
	return iRet;
}
int WINAPI LC_GetTempTemp(int iCanID, BYTE dbAddr, WORD &wTrueTemp, WORD &wTemp)//查询设定温度 -
{
	int iRet;
	if ((iRet = LC_p_GetTempTemp(iCanID, dbAddr)) == 0)
		iRet = LC_r_GetTempTemp(iCanID, dbAddr, wTrueTemp, wTemp);
	return iRet;
}
int WINAPI LC_SetTempTemp(int iCanID, BYTE dbAddr, WORD wTemp)//设定温度 -
{
	int iRet;
	if ((iRet = LC_p_SetTempTemp(iCanID, dbAddr, wTemp)) == 0)
		iRet = LC_r_SetTempTemp(iCanID);
	return iRet;
}
//恒流源同步命令 -
int WINAPI LC_GetCCSVol(int iCanID, WORD &wVol)//查询恒流源电流 -
{
	int iRet;
	if ((iRet = LC_p_GetCCSVol(iCanID)) == 0)
		iRet = LC_r_GetCCSVol(iCanID, wVol);
	return iRet;
}
int WINAPI LC_GetCCSTemp(int iCanID, WORD &wTemp)//查询电导池温度 -
{
	int iRet;
	if ((iRet = LC_p_GetCCSTemp(iCanID)) == 0)
		iRet = LC_r_GetCCSTemp(iCanID, wTemp);
	return iRet;
}
int WINAPI LC_GetCCSTSw(int iCanID, BYTE &eVal)//查询电导池温度开关 -
{
	int iRet;
	if ((iRet = LC_p_GetCCSTSw(iCanID)) == 0)
		iRet = LC_r_GetCCSTSw(iCanID, eVal);
	return iRet;
}
int WINAPI LC_SetCCSTSw(int iCanID, BYTE eVal)//设置电导池温度开关 -
{
	int iRet;
	if ((iRet = LC_p_SetCCSTSw(iCanID, eVal)) == 0)
		iRet = LC_r_SetCCSTSw(iCanID);
	return iRet;
}
int WINAPI LC_GetCCSRange(int iCanID, BYTE &eVal)//查询量程开关 -
{
	int iRet;
	if ((iRet = LC_p_GetCCSRange(iCanID)) == 0)
		iRet = LC_r_GetCCSRange(iCanID, eVal);
	return iRet;
}
int WINAPI LC_SetCCSRange(int iCanID, BYTE eVal)//设置量程开关 -
{
	int iRet;
	if ((iRet = LC_p_SetCCSRange(iCanID, eVal)) == 0)
		iRet = LC_r_SetCCSRange(iCanID);
	return iRet;
}
int WINAPI LC_GetCCSDASw(int iCanID, BYTE &bOnFlag)//查询恒流开关 -
{
	int iRet;
	if ((iRet = LC_p_GetCCSDASw(iCanID)) == 0)
		iRet = LC_r_GetCCSDASw(iCanID, bOnFlag);
	return iRet;
}
int WINAPI LC_RunCCSDASw(int iCanID, BYTE bOnFlag)//设置恒流开关 -
{
	int iRet;
	if ((iRet = LC_p_RunCCSDASw(iCanID, bOnFlag)) == 0)
		iRet = LC_r_RunCCSDASw(iCanID);
	return iRet;
}
int WINAPI LC_GetCCSDAVal(int iCanID, BYTE &dbVal, BYTE &dbNowVal)//查询恒流电流 -
{
	int iRet;
	if ((iRet = LC_p_GetCCSDAVal(iCanID)) == 0)
		iRet = LC_r_GetCCSDAVal(iCanID, dbVal, dbNowVal);
	return iRet;
}
int WINAPI LC_SetCCSDAVal(int iCanID, BYTE dbVal)//设置恒流电流 -
{
	int iRet;
	if ((iRet = LC_p_SetCCSDAVal(iCanID, dbVal)) == 0)
		iRet = LC_r_SetCCSDAVal(iCanID);
	return iRet;
}

//特殊的应答命令 -
int WINAPI LC_ResponseCanInfo(int iCanID)
{
	BYTE dbLCCmd[MAX_LC_BUFFER_SIZE];
	dbLCCmd[LC_PARAM_OFS] = 1;
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS + 1], PC_CAN_ID);
	LSB2MSB32(&dbLCCmd[LC_PARAM_OFS + 4], 1);
	_make_LCCmd(dbLCCmd, lcT_broadcast, lcsc_All_Can, 9);
	return _lc_can_send(iCanID, dbLCCmd);
}
