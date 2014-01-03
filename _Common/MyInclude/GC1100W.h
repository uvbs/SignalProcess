#pragma once

enum GCType {
	gcT_sample = 0x51
};

enum GCSampleFlag {
	gcsF_Cycle = 1,
	gcsF_Run = 2,
	gcsF_Data = 5
};

#define DEFAULT_SAMPLE_CYCLE			66	//采样周期(单位ms)
#define MAX_SAMPLE_RESULT_NUM			41	//一次最多获取结果数

#pragma pack(push, 1)
typedef struct tagSampleResult {
	DWORD dwTime;
	union {
		LONG lValue;
		float fValue;
	};
}	SAMPLE_RESULT, *P_SAMPLE_RESULT;
#pragma pack(pop)

enum GCReturnCode {
	gc_FearfulErr = -2, gc_COMErr = -1, gc_OK = 0, gc_Invalid, gc_Failed
};

#ifdef GC1100W_EXPORTS
#define GC1100W_DLL	__declspec(dllexport)
#else
#define GC1100W_DLL	__declspec(dllimport)
#endif

//基本函数 -
extern GC1100W_DLL HANDLE WINAPI GC_Create(int iComIdx, int iBPS = 9600, int iTimeout = 500);
extern GC1100W_DLL void WINAPI GC_Close(HANDLE h);
//温度控制 -
extern GC1100W_DLL int WINAPI GC_getPillarArk1Temp(HANDLE h, int &iTemp);//取得柱箱1温度 -
extern GC1100W_DLL int WINAPI GC_setPillarArk1Temp(HANDLE h, int iTemp);//设定柱箱1温度 -
extern GC1100W_DLL int WINAPI GC_getInputDetectTemp(HANDLE h, int &iTemp);//取得进样检测温度 -
extern GC1100W_DLL int WINAPI GC_setInputDetectTemp(HANDLE h, int iTemp);//设定进样检测温度 -
extern GC1100W_DLL int WINAPI GC_startHotUp(HANDLE h);//开始加热 -
extern GC1100W_DLL int WINAPI GC_stopHotUp(HANDLE h);//停止加热 -
extern GC1100W_DLL int WINAPI GC_doFire(HANDLE h);//点火 -
extern GC1100W_DLL int WINAPI GC_getStatus(HANDLE h, int &iStatus);//读状态 -
//采样 -
extern GC1100W_DLL int WINAPI GC_initSampleTimeCount(HANDLE h, UINT uChannel);//初始化计数器 -
extern GC1100W_DLL int WINAPI GC_getSampleCycle(HANDLE h, UINT uChannel, UINT &uCycle);//取得采样周期 -
extern GC1100W_DLL int WINAPI GC_runSample(HANDLE h, UINT uChannel, BOOL &bSuccessd);//开始采样命令 -
extern GC1100W_DLL int WINAPI GC_getRealtimeData(HANDLE h, UINT uChannel, UINT uCycle, P_SAMPLE_RESULT ptsr, int &iNum, BOOL &bStartFlag);//取数据 -
//#define GC_USE_OLD_SAMPLE	//使用老的采样数据格式 -
