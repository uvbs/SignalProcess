#pragma once

enum NPDReturnCode {
	npd_FearfulErr = -2, npd_COMErr = -1, npd_OK = 0, npd_Invalid, npd_Failed
};

enum NPDRangeType {
	npdRange_10_10, npdRange_10_9, npdRange_10_8
};

#define GC_NPD_MAX_CURRENT_INT			600

#ifdef GC_NPD_EXPORTS
#define GC_NPD_DLL	__declspec(dllexport)
#else
#define GC_NPD_DLL	__declspec(dllimport)
#endif
 
//基本函数 -
extern GC_NPD_DLL HANDLE WINAPI NPD_Create(int iComIdx, BOOL bNeedCheck = TRUE, int iBPS = 19200, int iTimeout = 1000);
extern GC_NPD_DLL void WINAPI NPD_Close(HANDLE h);
extern GC_NPD_DLL int WINAPI NPD_GetComIndex(HANDLE h);
//NPD相关 -
extern GC_NPD_DLL int WINAPI NPD_getCurrent(HANDLE h, int &iCurrent);//取得NPD电流 -
extern GC_NPD_DLL int WINAPI NPD_setCurrent(HANDLE h, int iCurrent);//设定NPD电流 -
extern GC_NPD_DLL int WINAPI NPD_getRange(HANDLE h, int &iRange);//取得NPD量程 -
extern GC_NPD_DLL int WINAPI NPD_setRange(HANDLE h, int iRange);//设定NPD量程 -
extern GC_NPD_DLL int WINAPI NPD_reversePolarity(HANDLE h);//NPD极性反转 -

extern GC_NPD_DLL int WINAPI NPD_getPolarity(HANDLE h, int &iPolarity);//N取得PD极性 -
extern GC_NPD_DLL int WINAPI NPD_setPolarity(HANDLE h, int iPolarity);//设定NPD极性 -
//取得设置NPD电流的模板
template <class T>
int WINAPI getNPDCurrent(HANDLE h, T &tCurrent)
{
	int iCurrent,iRet;
	if ((iRet = NPD_getCurrent(h, iCurrent)) == npd_OK) {
		tCurrent = (T)iCurrent * (T)0.01;
	}
	return iRet;
}
template <class T>
int WINAPI setNPDCurrent(HANDLE h, T tCurrent)
{
	return NPD_setCurrent(h, (int)(tCurrent * (T)100.0));
}
//NPD采样测试 -
extern GC_NPD_DLL int WINAPI NPD_getSampleVoltage(HANDLE h, BYTE &dbRange, WORD &wVoltage);//获取NPD采样信号电压 -
template <class T>
int WINAPI getNPDSampleVoltage(HANDLE h, T &tVoltage)
{
	int iRet;
	WORD wVoltage;
	BYTE dbRange;
	if ((iRet = NPD_getSampleVoltage(h, dbRange, wVoltage)) == npd_OK) {
		tVoltage = (T)2500000 * (T)wVoltage / (T)65535 / (T)(1 << (dbRange & 7));
	}
	return iRet;
}

////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_NPD_SAMPLE_CYCLE		66	//采样周期(单位ms)
#define MAX_NPD_SAMPLE_RESULT_NUM		41	//一次最多获取结果数
#pragma pack(push, 1)
typedef struct tagNPDSampleResult {
	DWORD dwTime;
	union {
		LONG lValue;
		float fValue;
	};
}	NPD_SAMPLE_RESULT, *P_NPD_SAMPLE_RESULT;
#pragma pack(pop)
//采样相关 -
extern GC_NPD_DLL int WINAPI NPD_sendGC(HANDLE h, BYTE dbType, BYTE dbAddr, BYTE dbFlag, LPBYTE lpdbParam, UINT uParamLen, LPBYTE lpdbRecv, UINT &uRecvLen, BOOL bRecvContentOnly);//发送命令(GC相关)
extern GC_NPD_DLL int WINAPI NPD_runSample(HANDLE h, UINT uChannel, BOOL &bSuccessd);//开始采样命令 -
extern GC_NPD_DLL int WINAPI NPD_getRealtimeData(HANDLE h, UINT uChannel, P_NPD_SAMPLE_RESULT ptsr, int &iNum, BOOL &bStartFlag);//取数据 -
