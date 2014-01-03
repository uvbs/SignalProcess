#pragma once

enum GCType {
	gcT_get = 0x55,
	gcT_set = 0x5A,
	gcT_sample = 0x51
};

enum GCFlag {
	gcF_NULL,
	gcF_PillarArkTemp,					//柱箱的设定温度(单位0.01度)
	gcF_ForeInputTemp,					//前进样器的设定温度
	gcF_BackInputTemp,					//后进样器的设定温度
	gcF_ForeDetectTemp,					//前检测器的设定温度
	gcF_BackDetectTemp,					//后检测器的设定温度
	gcF_AssistantTemp,					//辅助的设定温度
	gcF_PillarArkK,						//柱箱的加热开关
	gcF_ForeInputK,						//前进样器的加热开关
	gcF_BackInputK,						//后进样器的加热开关
	gcF_ForeDetectK,					//前检测器的加热开关
	gcF_BackDetectK,					//后检测器的加热开关
	gcF_AssistantK,						//辅助的加热开关
	gcF_HotUpKeepTime = 0x10,			//程序升温度的保持时间(0 base)(单位0.1分)
	gcF_HotUpSpeed,						//程序升温升温速率(1 base)(单位0.1度/分)
	gcF_HotUpTemp,						//程序升温温度(1 base)(单位0.01度)
	gcF_HotUpK = 0x26,					//程序升温启动命令
	gcF_FIDSensitiveSW = 0x30,			//FID灵敏度切换命令
	gcF_FIDPolaritySW,					//FID极性切换
	gcF_FIDAFire,						//FIDA路点火
	gcF_FIDBFire,						//FIDB路点火
	gcF_TCDCurrent,						//TCD的桥电流
	gcF_TCDPolaritySW,					//TCD极性切换
	gcF_HotUpSpeed8 = 0x40,				//程序升温升温速率(8 base)(单位0.1度/分)
	gcF_HotUpTemp8,						//程序升温温度(8 base)(单位0.01度)
	gcF_HotUpKeepTime8,					//程序升温度的保持时间(8 base)(单位0.1分)
	gcF_ECDCurrentSel = 0x4A,			//ECD电流选择
	gcF_ECDMeasureSel,					//ECD量程选择
	gcF_FPDMeasureSel = 0x50,			//FPD量程选择
	gcF_FID2SensitiveSW = 0x55,			//FID1灵敏度切换命令
	gcF_FID2PolaritySW,					//FID1极性切换
	gcF_FID2AFire,						//FID1A路点火
	gcF_FID2BFire						//FID1B路点火
};

enum GCSampleFlag {
	gcsF_Cycle = 1,
	gcsF_Run = 2,
	gcsF_Data = 5
};

#define TEMP_MAX_VALUE					40000	//最高温度(单位0.01度)
#define HOT_UP_MAX_VALUE				400	//最高升温(单位0.1度/分)
#define HOT_UP_SPACE_SIZE				3	//程序升温命令间隔
#define HOT_UP_LEVEL1					7	//升温阶数
#define HOT_UP_LEVEL2					10	//升温阶数(扩展后的总数)
#define TCD_CURRENT_MAX_VALUE			255	//最大TCD桥电流
#define GC_TABLE1_SIZE					30	//表1:长度
#define GC_TABLE2_SIZE					28	//表2:长度
#define TABLE1_TEMP_SPACE_SIZE			4	//表1:温度间隔
#define TABLE1_SET_TEMP_OFS				0	//表1:设置温度偏移量
#define TABLE1_TRUE_TEMP_OFS			2	//表1:实际温度偏移量
#define TABLE1_TEMP_SWITCH_OFS			24	//表1:温度开关偏移量
#define TABLE1_TEMP_SWITCH_NUM			6	//表1:温度开关个数
#define TABLE2_FID_POLARITY_OFS			0	//表2:FID极性偏移量
#define TABLE2_FID_SENSITIVE_OFS		1	//表2:FID灵敏度偏移量
#define TABLE2_TCD_CURRENT_OFS			2	//表2:TCD桥电流
#define TABLE2_TCD_POLARITY_OFS			3	//表2:TCD极性
#define TABLE2_FPD_MEASURE_OFS			4	//表2:FPD量程选择
#define TABLE2_ECD_CURRENT_OFS			5	//表2:ECD电流选择
#define TABLE2_ECD_MEASURE_OFS			6	//表2:ECD量程选择
#define TABLE2_FID2_SENSITIVE_OFS		7	//表2:FID2灵敏度偏移量
#define TABLE2_FID2_POLARITY_OFS		8	//表2:FID2极性偏移量
#define TABLE2_FLUX_SPACE_SIZE			2	//表2:流量间隔
#define TABLE2_FLUX_OFS					12	//表2:流量偏移量
#define TABLE2_FLUX_NUM					6	//表2:流量个数

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

enum GCFIDSensitive {
	FIDs_10_10, FIDs_10_9, FIDs_10_8, FIDs_10_7
};

enum GCReturnCode {
	gc_FearfulErr = -2, gc_COMErr = -1, gc_OK = 0, gc_Invalid, gc_Failed
};

#ifdef GC_EXPORTS
#define GC_DLL	__declspec(dllexport)
#else
#define GC_DLL	__declspec(dllimport)
#endif

extern GC_DLL const BYTE* WINAPI GC_getTable(int iTable, UINT &uSize);//取内部数据表 -
//表1字段 -
extern GC_DLL int WINAPI GC_getSettedTemp(const BYTE* pcdbTable, UINT uFlag, UINT &uValue);//从表1中取设置的温度 -
extern GC_DLL int WINAPI GC_getTrueTemp(const BYTE* pcdbTable, UINT uFlag, UINT &uValue);//从表1中取实际温度 -
extern GC_DLL int WINAPI GC_getTempSwitch(const BYTE* pcdbTable, UINT uFlag, BOOL &bValue);//从表1中取温度开关状态 -
//表2字段 -
extern GC_DLL int WINAPI GC_getFIDValue(const BYTE* pcdbTable, UINT uFlag, UINT &uValue);//从表2中取FID值 -
extern GC_DLL int WINAPI GC_getTCDValue(const BYTE* pcdbTable, UINT uFlag, UINT &uValue);//从表2中取TCD值 -
extern GC_DLL int WINAPI GC_getFPDValue(const BYTE* pcdbTable, UINT uFlag, UINT &uValue);//从表2中取FPD值 -
extern GC_DLL int WINAPI GC_getECDValue(const BYTE* pcdbTable, UINT uFlag, UINT &uValue);//从表2中取ECD值 -
extern GC_DLL int WINAPI GC_getFlux(const BYTE* pcdbTable, UINT uIdx, UINT &uValue);//从表2中取流量 -
//基本函数 -
extern GC_DLL BOOL WINAPI GC_Connect(int iComIdx = 0, int* piComIdx = NULL, int iBPS = 19200, int iTimeout = 3000);
extern GC_DLL void WINAPI GC_Close();
extern GC_DLL int WINAPI GC_Send(UINT uType, UINT uAddr, UINT uflag, LPBYTE lpdbData, UINT uLen, LPBYTE lpdbOut, UINT &uOutLen, BOOL bOutCntOnly);
//温度控制 -
extern GC_DLL int WINAPI GC_getData(UINT uFlag, UINT &uResult);//读取数据(同时刷新表1) -
extern GC_DLL int WINAPI GC_setData(UINT uFlag, UINT uData, UINT &uResult);//设置数据(同时刷新表2) -
extern GC_DLL int WINAPI GC_setHotUpData(UINT uFlag, UINT uIdx, UINT uData, UINT &uResult);//设置升温 -
//采样 -
extern GC_DLL void WINAPI GC_initSampleTimeCount(UINT uChannel);//初始化计数器 -
extern GC_DLL int WINAPI GC_getSampleCycle(UINT uChannel, UINT &uCycle);//取得采样周期 -
extern GC_DLL int WINAPI GC_runSample(UINT uChannel, BOOL &bSuccessd);//开始采样命令 -
extern GC_DLL int WINAPI GC_getRealtimeData(UINT uChannel, UINT uCycle, P_SAMPLE_RESULT ptsr, int &iNum, BOOL &bStartFlag);//取数据 -
//#define GC_USE_OLD_SAMPLE	//使用老的采样数据格式 -
