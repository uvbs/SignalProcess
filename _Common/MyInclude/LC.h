#pragma once

#define MSB_DEVICE

enum LCType {
	lcT_query = 0x51,
	lcT_cmd,
	lcT_run,
	lcT_sample = 0x51
};

enum LCAddr {
	lcA_PumpA = 1,
	lcA_PumpB,
	lcA_PumpC,
	lcA_PumpD,
	lcA_Detect,
};

enum LCFlag {
	lcF_D_qDeuteriumTime = 1,			//询问氘灯累计时 -
	lcF_D_qIP,							//询问IP地址 -
	lcF_D_qWavelength,					//询问当前波长执行情况 -
	lcF_D_qWarning,						//报警查询 -
	lcF_D_qWarningResponse,				//报警查询的应答(不能发)
	lcF_D_qFeedback = 0x10,				//询问命令信息反馈 -

	lcsF_Cycle = 1,
	lcsF_Run = 2,
	lcsF_Data = 5,

	lcF_D_cWavelength = 1,				//更改设定波长，采样速率 -
	lcF_D_cWavelengthScan,				//设定波长扫描 -
	lcF_D_cWavelengthTable,				//更改波长程序表 -
	lcF_D_cZero,						//更改调零设置 -
	lcF_D_cIP,							//更改IP地址 -
	lcF_D_cPWD,							//更改密码 -
	lcF_D_cAdjustWavelength,			//波长校准 -
	lcF_D_cDeuteriumTime,				//氘灯时间归零 -
	lcF_D_cResetWarning,				//报警复位 -
	lcF_D_cFeedback = 0x10,				//命令信息反馈 -

	lcF_D_rCmd = 1,						//运行命令（氘灯开启，程序波长运行，调零，休眠） -
	lcF_D_rFeedback = 0x10,				//开关命令信息反馈 -

	lcF_P_qIdxNumber = 1,				//询问A泵和泵数量 -
	lcF_P_qConstantSpeed,				//询问恒流速度 -
	lcF_P_qIP,							//询问IP地址 -
	lcF_P_qGland,						//询问密封圈 -
	lcF_P_qPressure,					//询问当前压力值、压力单位、流速、报警 -
	lcF_P_qWarning,						//报警查询 -
	lcF_P_qWarningResponse,				//报警查询的应答(不能发)
	lcF_P_qFeedback = 0x10,				//询问命令信息反馈 -

	lcF_P_cIdxNumber = 1,				//更改泵号和泵数量 -
	lcF_P_cConstantSpeed,				//更改恒流速度 -
	lcF_P_cPressure,					//更改压力值、压力单位 -
	lcF_P_cWashParam,					//更改洗脱参数(2字节流速，1字节时间） -
	lcF_P_cPressureZero,				//设置压力调零 -
	lcF_P_cRippleCompress,				//更改脉动压缩 -
	lcF_P_cIP,							//更改IP地址 -
	lcF_P_cGradsTable,					//更改梯度表 -
	lcF_P_cPWD,							//更改密码 -
	lcF_P_cPressureAdjust,				//压力调整 -
	lcF_P_cVelocityAdjust,				//流速调整 -
	lcF_P_cResetWarning,				//报警复位 -
	lcF_P_cFeedback = 0x10,				//命令信息反馈 -

	lcF_P_rCmd = 1,						//运行命令（泵开启，梯度运行，冲洗，休眠） -
	lcF_P_rFeedback = 0x10				//开关命令信息反馈 -
};

//检测器运行命令枚举 -
enum LCDetectCmd {
	lcC_Detect_Sleep = 1,				//0x01：检测器休眠 -
	lcC_Detect_WakeUp = 0,				//0x00: 检测器唤醒 -
	lcC_Detect_StopWave = 2,			//0x02：波长编辑程序停止 -
	lcC_Detect_StartWave = 3,			//0x03: 波长编辑程序运行 -
	lcC_Detect_CloseDeuterium = 4,		//0x04：氘灯关 -
	lcC_Detect_OpenDeuterium = 5,		//0x05: 氘灯开 -
	lcC_Detect_NoZero = 6,				//0x06：不调零 -
	lcC_Detect_DoZero = 7				//0x07: 调零 -
};

//泵运行命令枚举 -
enum LCPumpCmd {
	lcC_Pump_Sleep = 1,					//0x01：泵休眠 -
	lcC_Pump_WakeUp = 0,				//0x00: 泵唤醒 -
	lcC_Pump_StopGrads = 2,				//0x02: 梯度停止 -
	lcC_Pump_ReadyGrads = 3,			//0x03: 梯度准备 -
	lcC_Pump_RunGrads = 4,				//0x04: 梯度运行 -
	lcC_Pump_Stop = 5,					//0x05: 泵停止 -
	lcC_Pump_Run = 6,					//0x06: 泵运行 -
	lcC_Pump_StopWash = 7,				//0x07: 泵冲洗停止 -
	lcC_Pump_RunWash = 8				//0x08: 泵冲洗运行 -
};

#pragma pack(push, 1)
typedef struct tagGetDetectWavelength {
	WORD	wWavelength;				//当前波长 -
	DWORD	dwAbsorbency;				//吸光度 -
	DWORD	dwSampleValue;				//样品值 -
	DWORD	dwCompareValue;				//参比值 -
	BYTE	dbWorkStatus;				//工作状态 -
	BYTE	dbSysStatus;				//系统情况 -
}	GET_DETECT_WAVE_LENGTH, *P_GET_DETECT_WAVE_LENGTH;

typedef struct tagGetDetectWarning {
	BYTE	dbType;						//报警类型 -
	BYTE	dbWarning;					//报警序号 -
	DWORD	dwSettingParam;				//设定参数 -
	DWORD	dwNowParam;					//当前参数 -
}	GET_DETECT_WARNING, *P_GET_DETECT_WARNING;

typedef struct tagSetDetectWavelength {
	WORD	wWavelength;				//设定波长 -
	BYTE	dbSampleSpeed;				//采样速率 -
}	SET_DETECT_WAVE_LENGTH, *P_SET_DETECT_WAVE_LENGTH;

typedef struct tagSetDetectWavelengthScan {
	BYTE	dbWavelengthScan;			//波长扫描 -
	WORD	wStartWavelength;			//起始波长 -
	WORD	wEndWavelength;				//结束波长 -
}	SET_DETECT_WAVE_LENGTH_SCAN, *P_SET_DETECT_WAVE_LENGTH_SCAN;

typedef struct tagWavelengthTable{
	DWORD	dwTime;						//时间 -
	WORD	wWavelength;				//波长 -
	BYTE	dbSampleSpeed;				//速率 -
}	WAVELENGTH_TABLE, *P_WAVELENGTH_TABLE;

typedef struct tagSetDetectWavelengthTable{
	BYTE	dbTotalNum;					//表数量 -
	BYTE	dbNow;						//当前数 -
	WAVELENGTH_TABLE tWavelengthTable;	//波长程序表 -
}	SET_DETECT_WAVELENGTH_TABLE, *P_SET_DETECT_WAVELENGTH_TABLE;

typedef struct tagGetPumpGland {
	DWORD	dwLeftGland;				//左密封圈 -
	DWORD	dwRightGland;				//右密封圈 -
}	GET_PUMP_GLAND, *P_GET_PUMP_GLAND;

typedef struct tagGetPumpPressure {
	WORD	wPressure;					//压力值 -
	BYTE	dbUnit;						//压力单位 -
	DWORD	dwPressureRange;			//压力上下限 -
	WORD	wVelocity;					//当前流速 -
	BYTE	dbSysStatus;				//系统情况 -
	BYTE	dbWorkStatus;				//工作状态 -
}	GET_PUMP_PRESSURE, *P_GET_PUMP_PRESSURE;

typedef struct tagGetPumpWarning {
	BYTE	dbType;						//报警类型 -
	BYTE	dbWarning;					//报警序号 -
	DWORD	dwSettingParam;				//设定参数 -
	DWORD	dwNowParam;					//当前参数 -
}	GET_PUMP_WARNING, *P_GET_PUMP_WARNING;

typedef struct tagSetPumpPressure {
	WORD	wPressureHigh;				//压力值(上限) -
	WORD	wPressureLow;				//压力值(下限) -
	BYTE	dbUnit;						//压力单位 -
}	SET_PUMP_PRESSURE, *P_SET_PUMP_PRESSURE;

typedef struct tagSetPumpWashParam {
	WORD	wVelocity;					//流速 -
	BYTE	dbTime;						//时间 -
}	SET_PUMP_WASH_PARAM, *P_SET_PUMP_WASH_PARAM;

typedef struct tagGradsTable{
	DWORD	dwTime;						//时间 -
	WORD	wScaleA;					//A比例 -
	WORD	wScaleB;					//B比例 -
	WORD	wScaleC;					//C比例 -
	WORD	wScaleD;					//D比例 -
	WORD	wVelocity;					//流速 -
}	GRADS_TABLE, *P_GRADS_TABLE;

typedef struct tagSetPumpGradsTable{
	BYTE	dbTotalNum;					//表数量 -
	BYTE	dbNow;						//当前数 -
	GRADS_TABLE tGradsTable;			//梯度表 -
}	SET_PUMP_GRADS_TABLE, *P_SET_PUMP_GRADS_TABLE;

typedef struct tagSampleResult {
	DWORD dwTime;
	LONG lValue;
}	SAMPLE_RESULT, *P_SAMPLE_RESULT;
#pragma pack(pop)

#define LC_IP_LEN						4
#define LC_PWD_LEN						6
#define LC_FEEDBACK_LEN					1

enum LCReturnCode {
	lc_COMErr = -1, lc_OK = 0, lc_Invalid, lc_Failed
};

//反馈码 -
enum LCFeedbackCode {
	lcfb_OK, lcfb_Err1, lcfb_Err2, lcfb_Err3
};

//检测器系统情况 -
#define LC_DETECT_SYS_VALVE_OPEN		0x80	//进样阀开 -
#define LC_DETECT_SYS_PC_CONTROL		0x40	//PC控制 -
#define LC_DETECT_SYS_WAVELENGTH_COME	0x20	//波长到位 -
#define LC_DETECT_SYS_HAS_WARNING		0x10	//是否有报警  -

//检测器工作状态 -
enum LCDetectWorkStatus {
	lcDS_UVSleep,						//0：UV休眠 -
	lcDS_DeuteriumClose,				//1：氘灯关 -
	lcDS_DeuteriumOpen,					//2：基线监视（氘灯开） -
	lcDS_SpecialWavelengthSample,		//3：定波长采样 -
	lcDS_ProgramWavelengthSample,		//4：程序波长采样 -
	lcDS_WavelengthScan					//5：波长扫描 -
};

//泵系统情况说明 -
#define LC_PUMP_SYS_PC_CONTROL			0x40	//PC控制
#define LC_PUMP_SYS_HAS_WARNING			0x10	//是否有报警

//泵工作状态 -
enum LCPumpWorkStatus {
	lcPS_PumpSleep,						// 0: 泵休眠 -
	lcPS_PumpConstantSpeed,				// 1：恒流泵速 -
	lcPS_GradsReady,					// 2：梯度准备 -
	lcPS_GradsRun,						// 3：梯度运行 -
	lcPS_PumpWash,						// 4：泵冲洗 -
	lcPS_PumpStop						// 5：泵停止 -
};

//泵警告码 -
enum LCPumpWarningCode {
	lcPW_PumpPressureUpper = 1,			//压力上限报警 -
	lcPW_PumpPressureLower				//压力下限报警 -
};

#define MAX_WARNING_RECORD_NUM			24	//一次最多获取警告数 -
#define DEFAULT_SAMPLE_CYCLE			20	//采样周期(单位ms)
#define MAX_SAMPLE_RESULT_NUM			41	//一次最多获取结果数 -

#ifdef LC_EXPORTS
#define LC_DLL	__declspec(dllexport)
#else
#define LC_DLL	__declspec(dllimport)
#endif

//基本函数 -
extern LC_DLL BOOL WINAPI LC_Connect(int iComIdx = 0, int* piComIdx = NULL, int iBPS = 9600, int iTimeout = 500);
extern LC_DLL void WINAPI LC_Close();
extern LC_DLL int WINAPI LC_Send(UINT uType, UINT uAddr, UINT uflag, LPBYTE lpdbData, UINT uLen, LPBYTE lpdbOut, UINT &uOutLen, BOOL bOutCntOnly);

//特殊函数 -
extern LC_DLL P_GET_DETECT_WAVE_LENGTH WINAPI LC_GetWavelengthScanData(int &iNum, int &iErrorCode);
//extern LC_DLL BOOL WINAPI LC_GetWavelengthProgramData(P_GET_DETECT_WAVE_LENGTH p, int &iErrorCode, BOOL &bEndFlag);
extern LC_DLL BOOL WINAPI LC_GetRunCmdResult(int &iErrorCode, BOOL bWaitEnd = TRUE);

//检测器 -
extern LC_DLL int WINAPI LC_GetDetectDeuteriumTime(WORD &wDeuteriumTime);//询问氘灯累计时 -
extern LC_DLL int WINAPI LC_GetDetectIP(BYTE dbIP[LC_IP_LEN]);//询问IP地址 -
extern LC_DLL int WINAPI LC_GetDetectWavelength(WORD &wWavelength, DWORD &dwAbsorbency, DWORD &dwSampleValue, DWORD &dwCompareValue, BYTE &dbSysStatus, BYTE &dbWorkStatus);//询问当前波长执行情况 -
extern LC_DLL int WINAPI LC_GetDetectWarning(BYTE dbType, P_GET_DETECT_WARNING ptgdw, int &iNum);//报警查询 -
extern LC_DLL int WINAPI LC_GetDetectFeedback(BYTE &dbFeedback);//询问命令信息反馈 -

extern LC_DLL int WINAPI LC_SetDetectWavelength(WORD wWavelength, BYTE dbSampleSpeed);//更改设定波长，采样速率 -
extern LC_DLL int WINAPI LC_SetDetectWavelengthScan(BYTE dbWavelengthScan, WORD wStartWavelength, WORD wEndWavelength);//设定波长扫描 -
extern LC_DLL int WINAPI LC_SetDetectWavelengthTable(BYTE dbTotalNum, BYTE dbNow, P_WAVELENGTH_TABLE pwt);//更改波长程序表 -
extern LC_DLL int WINAPI LC_SetDetectZero(BYTE dbZero);//更改调零设置 -
extern LC_DLL int WINAPI LC_SetDetectIP(BYTE dbIP[LC_IP_LEN]);//更改IP地址 -
extern LC_DLL int WINAPI LC_SetDetectPWD(BYTE dbPWD[LC_PWD_LEN]);//更改密码 -
extern LC_DLL int WINAPI LC_SetDetectAdjustWavelength(WORD wAdjustWavelength);//波长校准 -
extern LC_DLL int WINAPI LC_SetDetectDeuteriumTime(BYTE dbDeuteriumTime);//氘灯时间归零 -
extern LC_DLL int WINAPI LC_SetDetectResetWarning(BYTE dbType, BYTE dbWarning);//报警复位 -
extern LC_DLL int WINAPI LC_SetDetectFeedback(BYTE &dbFeedback);//命令信息反馈 -

extern LC_DLL int WINAPI LC_RunDetectCmd(BYTE dbCmd, DWORD dwTimeOut = 0, BOOL bWaitEnd = TRUE);//运行命令（氘灯开启，程序波长运行，调零，休眠） -
extern LC_DLL int WINAPI LC_RunDetectFeedback(BYTE &dbFeedback);//开关命令信息反馈 -

//检测器::采样 -
extern LC_DLL void WINAPI LC_initSampleTimeCount();//初始化计数器 -
extern LC_DLL int WINAPI LC_getSampleCycle(UINT &uCycle);//取得采样周期 -
extern LC_DLL int WINAPI LC_runSample(BOOL &bSuccessd);//开始采样命令 -
extern LC_DLL int WINAPI LC_getRealtimeData(UINT uCycle, P_SAMPLE_RESULT ptsr, int &iNum, BOOL &bStartFlag);//取数据 -

//泵 -
extern LC_DLL int WINAPI LC_GetPumpIdxNumber(int idx, BYTE &dbPumpIdx, BYTE &dbPumpNum);//询问A泵和泵数量 -
extern LC_DLL int WINAPI LC_GetPumpConstantSpeed(int idx, WORD &wConstantSpeed);//询问恒流速度 -
extern LC_DLL int WINAPI LC_GetPumpIP(int idx, BYTE dbIP[LC_IP_LEN]);//询问IP地址 -
extern LC_DLL int WINAPI LC_GetPumpGland(int idx, DWORD &dwLeftGland, DWORD &dwRightGland);//询问密封圈 -
extern LC_DLL int WINAPI LC_GetPumpPressure(int idx, WORD &wPressure, BYTE &dbUnit, DWORD &dwPressureRange, WORD &wVelocity, BYTE &dbSysStatus, BYTE &dbWorkStatus);//询问当前压力值、压力单位、压力上下限、流速、报警 -
extern LC_DLL int WINAPI LC_GetPumpWarning(int idx, BYTE dbType, P_GET_PUMP_WARNING ptgpw, int &iNum);//报警查询 -
extern LC_DLL int WINAPI LC_GetPumpFeedback(int idx, BYTE &dbFeedback);//询问命令信息反馈 -

extern LC_DLL int WINAPI LC_SetPumpIdxNumber(int idx, BYTE dbPumpIdx, BYTE dbPumpNum);//更改泵号和泵数量 -
extern LC_DLL int WINAPI LC_SetPumpConstantSpeed(int idx, WORD wConstantSpeed);//更改恒流速度 -
extern LC_DLL int WINAPI LC_SetPumpPressure(int idx, WORD wPressureHigh, WORD wPressureLow, BYTE dbUnit);//更改压力值、压力单位 -
extern LC_DLL int WINAPI LC_SetPumpWashParam(int idx, WORD wVelocity, BYTE dbTime);//更改洗脱参数(2字节流速，1字节时间） -
extern LC_DLL int WINAPI LC_SetPumpPressureZero(int idx, BYTE dbPressureZero);//设置压力调零 -
extern LC_DLL int WINAPI LC_SetPumpRippleCompress(int idx, WORD wRippleCompress);//更改脉动压缩 -
extern LC_DLL int WINAPI LC_SetPumpIP(int idx, BYTE dbIP[LC_IP_LEN]);//更改IP地址 -
extern LC_DLL int WINAPI LC_SetPumpGradsTable(int idx, BYTE dbTotalNum, BYTE dbNow, P_GRADS_TABLE pgt);//更改梯度表 -
extern LC_DLL int WINAPI LC_SetPumpPWD(int idx, BYTE dbPWD[LC_PWD_LEN]);//更改密码 -
extern LC_DLL int WINAPI LC_SetPumpPressureAdjust(int idx, WORD wPressureAdjust);//压力调整 -
extern LC_DLL int WINAPI LC_SetPumpVelocityAdjust(int idx, WORD wVelocityAdjust);//流速调整 -
extern LC_DLL int WINAPI LC_SetPumpResetWarning(int idx, BYTE dbType, BYTE dbWarning);//报警复位 -
extern LC_DLL int WINAPI LC_SetPumpFeedback(int idx, BYTE &dbFeedback);//命令信息反馈 -
extern LC_DLL int WINAPI LC_RunPumpCmd(int idx, BYTE dbCmd, DWORD dwTimeOut = 0, BOOL bWaitEnd = TRUE);//运行命令（泵开启，梯度运行，冲洗，休眠） -
extern LC_DLL int WINAPI LC_RunPumpFeedback(int idx, BYTE &dbFeedback);//开关命令信息反馈 -
