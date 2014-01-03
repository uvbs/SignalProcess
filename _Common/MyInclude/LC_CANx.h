#pragma once

//总线设备枚举 -
enum LCDevCanId {
	lcDC_Detect = 0x205,
	lcDC_PumpA = 0x301,
	lcDC_PumpB = 0x302,
	lcDC_PumpC = 0x303,
	lcDC_PumpD = 0x304,
	lcDC_AllPump = 0xFFF,
	lcDC_All = 0x1FFF
};

//错误返回 -
enum LCReturnCode {
	lc_COMErr = -1,						//CAN通讯错误或超时 -
	lc_OK = 0,							//成功完成 -
	lc_IsRunning = 1,					//已经开始运行，但操作未结束 -
	lc_UnknowCmd = 2,					//不支持的指令 -
	lc_DeviceTypeErr = 3,				//设备类型错误 -
	lc_SystemBusy = 4,					//系统忙 -
	lc_StartSample = 5,					//开始采样 -
	lc_InvalidParam = 6,				//传入参数出错 -
	lc_FixedCanId = 7,					//不能更改CAN节点地址 -
	lc_WaveGradsBegin = 8,				//波长梯度该段开始 -
	lc_WaveGradsEnd = 9					//波长梯度该段结束 -
};

//命令定义 -
enum LCType {
	lcT_query = 0x51,
	lcT_set = 0x52,
	lcT_broadcast = 0x53
};

//子命令定义 -
enum LCSubCmd {
	lcsc_All_Can = 1,					//CAN
	lcsc_All_LogicIndex = 2,			//逻辑编号
	lcsc_All_Time = 3,					//时间
	lcsc_All_RealtimeData = 5,			//实时数据
	lcsc_All_Warning = 6,				//报警
	lcsc_All_Rest = 9,					//待机
	lcsc_UV_Status = 10,				//状态	查询
	lcsc_UV_DeuteriumLamp = 11,			//氘灯	查询、归零
	lcsc_UV_Wavelength = 12,			//波长	查询、设定
	lcsc_UV_WavelengthScan = 13,		//		波长扫描查询、执行
	lcsc_UV_WavelengthTable_Get = 14,	//波长程序表	查询
	lcsc_UV_WavelengthTable_Cls = 15,	//		清除
	lcsc_UV_WavelengthTable_Add = 16,	//		增加
	lcsc_UV_WavelengthTable_Del = 17,	//		删除
	lcsc_UV_WavelengthAdjust_Zero = 18,	//波长校正	波长调零
	lcsc_UV_WavelengthAdjust_Adj = 19,	//		波长校准
	lcsc_UV_WavelengthAdjust_Chk = 20,	//		自检
	lcsc_UV_WavelengthAdjust = 21,		//		波长校正值设定、查询
	lcsc_UV_RunCmd = 22,				//紫外命令执行	多选择命令
	lcsc_UV_Run_WavelengthScan = 23,	//		执行波长扫描
	lcsc_UV_Run_DeuteriumLamp = 24,		//		执行氘灯开关
	lcsc_UV_Run_WavelengthTable = 25,	//		执行程序波长
	lcsc_UV_Run_Zero = 26,				//		执行调零
	lcsc_UV_WorkData = 27,				//实时工作数据	查询
	lcsc_UV_WorkMultiData = 28,			//		查询多帧
	lcsc_Pump_Status = 30,				//状态	查询 -
	lcsc_Pump_Speed = 31,				//流速	设置、查询 -
	lcsc_Pump_Pressure = 32,			//压力	范围设定 -
	lcsc_Pump_PressureZero = 33,		//		设置、查询零位显示 -
	lcsc_Pump_Ripple = 34,				//		脉动设定 -
	lcsc_Pump_GradsTable_Get = 35,		//压力梯度	查询 -
	lcsc_Pump_GradsTable_Cls = 36,		//		清除 -
	lcsc_Pump_GradsTable_Add = 37,		//		增加/修改 -
	lcsc_Pump_GradsTable_Del = 38,		//		删除 -
	lcsc_Pump_Wash = 39,				//洗脱	设置、查询 -
	lcsc_Pump_RunCmd = 40,				//泵命令执行	多命令执行 -
	lcsc_Pump_Gland = 41,				//密封圈	查询、回零 -
	lcsc_Pump_Switch_Run = 42,			//		执行泵开关 -
	lcsc_Pump_Switch_Wash = 43,			//		执行泵冲洗 -
	lcsc_Pump_Switch_GradsReady = 44,	//		执行梯度准备 -
	lcsc_Pump_Switch_Grads = 45,		//		执行梯度 -
	lcsc_Pump_Time = 46,				//泵时间	泵运行时间 -
	//离子色谱新加功能 -
	lcsc_Temp_BPS = 60,					//温控	通讯速率 -
	lcsc_Temp_Temp = 61,				//		温度 -
	lcsc_CCS_Vol = 70,					//恒流源	测量电流 -
	lcsc_CCS_Temp = 71,					//		测量温度 -
	lcsc_CCS_TSw = 72,					//		电导池温度开关 -
	lcsc_CCS_Range = 73,				//		量程开关 -
	lcsc_CCS_DASw = 74,					//		电流开关 -
	lcsc_CCS_DAVal = 75					//		电流值 -
};

//紫外运行命令枚举(此命令集已被底层废弃) -
// enum LCUVCmd {
// 	lcC_UV_Sleep = 1,					//0x01：泵休眠 -
// 	lcC_UV_WakeUp = 0,					//0x00: 泵唤醒 -
// 	lcC_UV_StopWave = 2,				//0x02: 波长编辑程序停止 -
// 	lcC_UV_RunWave = 3,					//0x03: 波长编辑程序运行 -
// 	lcC_UV_CloseDeuterium = 4,			//0x04: 氘灯关 -
// 	lcC_UV_OpenDeuterium = 5,			//0x05: 氘灯开 -
// 	lcC_UV_NoZero = 6,					//0x06: 不调零 -
// 	lcC_UV_DoZero = 7					//0x07: 调零 -
// };

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

//恒流源开关枚举 -
enum CCS_TSW_Val {
	T_SW_40 = 1, T_SW_50 = 2, T_SW_60 = 3
};
enum CCS_RANGE_Val {
	RANGE_uS = 1, RANGE_mS = 2
};

#pragma pack(push, 1)
//CAN节点属性 -
typedef struct tagCanInfo {
	BYTE	dbType;						//设备类型 -
	int		iCanID;						//设备地址 -
	int		iDeviceIdx;					//设备序号 -
}	CAN_INFO, *P_CAN_INFO;
//报警信息 -
typedef struct tagWarningInfo {
	BYTE	dbType;						//报警类型 -
	BYTE	dbIdx;						//报警序号 -
	DWORD	dwSetParam;					//设定参数 -
	DWORD	dwNowParam;					//当前参数 -
	DWORD	dwTime;						//报警时间 -
	WORD	wIdx;						//报警编号 -
}	WARNING_INFO, *P_WARNING_INFO;
//实时数据 -
typedef struct tagSampleResult {
	DWORD	dwTime;						//时间 -
	float	fValue;						//数据 -
}	SAMPLE_RESULT, *P_SAMPLE_RESULT;
//紫外工作数据 -
typedef struct tagUVWorkData {
	WORD	wWavelength;				//当前波长 -
	LONG	lAbsorbency;				//吸光度 -
	DWORD	dwSampleValue;				//样品值 -
	DWORD	dwCompareValue;				//参比值 -
}	UV_WORK_DATA, *P_UV_WORK_DATA;
//波长程序表 -
typedef struct tagWavelengthTable {
	DWORD	dwTime;						//时间 -
	WORD	wWavelength;				//波长 -
	BYTE	dbSampleSpeed;				//速率 -
}	WAVELENGTH_TABLE, *P_WAVELENGTH_TABLE;
//梯度表 -
typedef struct tagGradsTable {
	DWORD	dwTime;						//时间 -
	WORD	wScaleA;					//A比例 -
	WORD	wScaleB;					//B比例 -
	WORD	wScaleC;					//C比例 -
	WORD	wScaleD;					//D比例 -
	WORD	wVelocity;					//流速 -
}	GRADS_TABLE, *P_GRADS_TABLE;
#pragma pack(pop)

#define MAX_LC_BUFFER_SIZE				260	//LC协议包最大长度 -
#define DEVICE_INFO_MAX_NUM				32	//仪器信息一次最多返回个数 -
#define WARNING_DATA_MAX_NUM			15	//警告一次最多返回个数 -
#define REALTIME_DATA_MAX_NUM			31	//实时数据一次最多返回个数 -
#define UV_WORK_DATA_MAX_NUM			17	//紫外工作数据一次最多返回个数 -


//设备配置连接函数 -
extern BOOL WINAPI LC_initDLL();//初始化DLL函数 -
extern void WINAPI LC_exitDLL();//退出前调用 -
extern void WINAPI LC_setVCOM2CANFindOrder(int iDevType, int iLevel);//设置串口转CAN型设备的查找顺序 -
extern BOOL WINAPI LC_Connect(int &iComIdx);//连接设备 -
extern void WINAPI LC_Close();//关闭设备 -
extern void WINAPI LC_setWindowMessage(HWND hWnd, UINT uMsg);//设置窗口句柄和消息 -
extern void WINAPI LC_setCallbackFunction(BOOL (CALLBACK* pCallback)(int, LPBYTE, LPVOID), LPVOID lpParam);//设置回调函数 -
extern void WINAPI LC_setWaitTimeout(DWORD dwWaitTimeout = 3000);//设置超时 -

////////////////////////////////////////////////////////////////////////////////
//异步命令区
//设备异步命令(发送)
extern int WINAPI LC_p_GetCurrentTime(int iCanID);//查询当前时间 -
extern int WINAPI LC_p_SetCurrentTime(int iCanID, DWORD dwTime);//设置当前时间 -
extern int WINAPI LC_p_SetCanID(int iCanID, int iNewCanID);//设定设备CAN地址 -
extern int WINAPI LC_p_GetCanInfo(int iCanID);//询问CAN节点属性 -
extern int WINAPI LC_p_RunWake(int iCanID, BOOL bOnFlag);//执行唤醒 -
extern int WINAPI LC_p_GetWarning(int iCanID, BYTE dbType, BYTE dbIdx);//查询报警 -
extern int WINAPI LC_p_SetWarning(int iCanID, BYTE dbType, BYTE dbIdx, WORD wIdx);//报警复位 -
extern int WINAPI LC_p_GetRealtimeData(int iCanID);//实时数据 -
extern int WINAPI LC_p_SetLogicIndex(int iCanID, WORD wLogicIndex);//设置设备的逻辑编号 -
//紫外异步命令(发送)
extern int WINAPI LC_p_GetUVStatus(int iCanID);//查询紫外状态 -
extern int WINAPI LC_p_GetUVWorkData(int iCanID, BOOL bMultiData);//查询紫外工作数据 -
extern int WINAPI LC_p_GetUVDeuteriumLampTime(int iCanID);//查询氘灯累计运行时 -
extern int WINAPI LC_p_GetUVWavelength(int iCanID);//查询设定波长 -
extern int WINAPI LC_p_GetUVWavelengthTable(int iCanID, BYTE dbLine);//查询波长程序表
extern int WINAPI LC_p_GetUVWavelengthScanRange(int iCanID);//查询波长扫描范围 -
extern int WINAPI LC_p_SetUVWavelengthSmooth(int iCanID, WORD wWavelength, BYTE dbSmoothValue);//设定波长和平滑指数 -
extern int WINAPI LC_p_SetUVWavelengthScan(int iCanID, BYTE dbWavelengthScan, WORD wStartWavelength, WORD wEndWavelength);//设定(执行)波长扫描 -
extern int WINAPI LC_p_ClsUVWavelengthTable(int iCanID);//清除波长程序表 -
extern int WINAPI LC_p_SetUVWavelengthTable(int iCanID, P_WAVELENGTH_TABLE pwt);//增加/修改波长程序表 -
extern int WINAPI LC_p_DelUVWavelengthTable(int iCanID, DWORD dwTime);//删除波长程序表行 -
extern int WINAPI LC_p_SetUVWavelengthZero(int iCanID, BOOL bZeroFlag);//更改调零设置 -
extern int WINAPI LC_p_AdjUVWavelength(int iCanID, WORD wParam);//校准波长 -
extern int WINAPI LC_p_SetUVDeuteriumLampTime(int iCanID);//归零氘灯时间 -
extern int WINAPI LC_p_RunUVWavelengthScan(int iCanID, BOOL bOnFlag);//执行波长扫描 -
extern int WINAPI LC_p_RunUVDeuteriumLamp(int iCanID, BOOL bOnFlag);//执行氘灯开关 -
extern int WINAPI LC_p_RunUVWavelengthTable(int iCanID, BOOL bOnFlag);//执行程序波长 -
extern int WINAPI LC_p_RunUVWavelengthZero(int iCanID);//执行调零 -
extern int WINAPI LC_p_RunUVWavelengthSelfCheck(int iCanID);//波长自检 -
extern int WINAPI LC_p_SetUVWavelengthAdjust(int iCanID, short sAdjustValue);//设定波长校正值 -
extern int WINAPI LC_p_GetUVWavelengthAdjust(int iCanID);//查询波长校正值 -
//泵异步命令(发送)
extern int WINAPI LC_p_GetPumpConstantSpeed(int iCanID);//查询恒流速度 -
extern int WINAPI LC_p_GetPumpGland(int iCanID);//查询密封圈 -
extern int WINAPI LC_p_SetPumpGlandZero(int iCanID);//密封圈次数回零 -
extern int WINAPI LC_p_GetPumpPressure(int iCanID);//查询泵状态 -
extern int WINAPI LC_p_SetPumpConstantSpeed(int iCanID, WORD wConstantSpeed);//设置恒流速度 -
extern int WINAPI LC_p_SetPumpPressure(int iCanID, WORD wPressureHigh, WORD wPressureLow, BYTE dbUnit);//设置压力值范围 -
extern int WINAPI LC_p_SetPumpWashParam(int iCanID, WORD wVelocity, WORD wTime);//设置洗脱参数 -
extern int WINAPI LC_p_GetPumpWashParam(int iCanID);//查询洗脱参数 -
extern int WINAPI LC_p_SetPumpPressureZero(int iCanID, WORD wPressureZero);//设置压力显示零位 -
extern int WINAPI LC_p_SetPumpRippleCompress(int iCanID, WORD wRippleCompress);//设置脉动压缩 -
extern int WINAPI LC_p_ClsPumpGradsTable(int iCanID);//清除压力梯度表 -
extern int WINAPI LC_p_SetPumpGradsTable(int iCanID, P_GRADS_TABLE pgt);//增加/修改压力梯度表 -
extern int WINAPI LC_p_DelPumpGradsTable(int iCanID, DWORD dwTime);//删除压力梯度表行 -
extern int WINAPI LC_p_GetPumpGradsTable(int iCanID, BYTE dbLine);//查询压力梯度表 -
extern int WINAPI LC_p_GetPumpPressureZero(int iCanID);//查询泵压力零位值 -
extern int WINAPI LC_p_RunPumpCmd(int iCanID, enum LCPumpCmd eCmd);//泵执行命令 -
extern int WINAPI LC_p_RunPumpSwitch(int iCanID, BOOL bOnFlag);//执行泵开关 -
extern int WINAPI LC_p_RunPumpWash(int iCanID, BOOL bOnFlag);//执行泵冲洗 -
extern int WINAPI LC_p_RunPumpGradsReady(int iCanID, BOOL bOnFlag);//执行梯度准备 -
extern int WINAPI LC_p_RunPumpGrads(int iCanID, BOOL bOnFlag);//执行梯度 -
extern int WINAPI LC_p_GetPumpTime(int iCanID);//查询泵运行时间 -
//色谱异步命令(发送)
extern int WINAPI LC_p_RunGCSample(int iCanID, BYTE dbChannel);//开始采样 -
extern int WINAPI LC_p_GetGCRealtimeData(int iCanID, BYTE dbChannel);//实时数据 -
//温控异步命令(发送)
extern int WINAPI LC_p_GetTempBPS(int iCanID);//获取温控通讯速率 -
extern int WINAPI LC_p_SetTempBPS(int iCanID, WORD wBPS);//设定温控通讯速率 -
extern int WINAPI LC_p_GetTempTemp(int iCanID, BYTE dbAddr);//查询设定温度 -
extern int WINAPI LC_p_SetTempTemp(int iCanID, BYTE dbAddr, WORD wTemp);//设定温度 -
//恒流源异步命令(发送)
extern int WINAPI LC_p_GetCCSVol(int iCanID);//查询恒流源电流 -
extern int WINAPI LC_p_GetCCSTemp(int iCanID);//查询恒流源温度 -
extern int WINAPI LC_p_GetCCSTSw(int iCanID);//查询电导池温度 -
extern int WINAPI LC_p_SetCCSTSw(int iCanID, BYTE eVal);//设置电导池温度 -
extern int WINAPI LC_p_GetCCSRange(int iCanID);//查询量程 -
extern int WINAPI LC_p_SetCCSRange(int iCanID, BYTE eVal);//设置量程 -
extern int WINAPI LC_p_GetCCSDASw(int iCanID);//查询恒流开关 -
extern int WINAPI LC_p_RunCCSDASw(int iCanID, BYTE bOnFlag);//设置恒流开关 -
extern int WINAPI LC_p_GetCCSDAVal(int iCanID);//查询恒流电流 -
extern int WINAPI LC_p_SetCCSDAVal(int iCanID, BYTE dbVal);//设置恒流电流 -
////////////////////////////////////////
//设备异步命令(接收)
extern int WINAPI LC_r_GetCurrentTime(int iCanID, DWORD &dwTime);//查询当前时间 -
extern int WINAPI LC_r_SetCurrentTime(int iCanID);//设置当前时间 -
extern int WINAPI LC_r_SetCanID(int iCanID);//设定设备CAN地址 -
extern int WINAPI LC_r_GetCanInfo(int iCanID, P_CAN_INFO ptCanInfo);//询问CAN节点属性 -
extern int WINAPI LC_r_RunWake(int iCanID);//执行唤醒 -
extern int WINAPI LC_r_GetWarning(int iCanID, P_WARNING_INFO ptWarningInfo, int &iNum);//查询报警 -
extern int WINAPI LC_r_SetWarning(int iCanID);//报警复位 -
extern int WINAPI LC_r_GetRealtimeData(int iCanID, P_SAMPLE_RESULT ptSample, int &iNum, LCReturnCode &eStatus, DWORD &dwTime, DWORD &dwWaveLength);//实时数据 -
extern int WINAPI LC_r_SetLogicIndex(int iCanID);//设置设备的逻辑编号 -
//紫外异步命令(接收)
extern int WINAPI LC_r_GetUVStatus(int iCanID, BYTE &dbSysStatus, BYTE &dbWorkStatus);//查询紫外状态 -
extern int WINAPI LC_r_GetUVWorkData(int iCanID, P_UV_WORK_DATA ptUVWorkData, int &iNum, BOOL bMultiData);//查询紫外工作数据 -
extern int WINAPI LC_r_GetUVDeuteriumLampTime(int iCanID, WORD &wDeuteriumTime);//查询氘灯累计运行时 -
extern int WINAPI LC_r_GetUVWavelength(int iCanID, WORD &wWavelength);//查询设定波长 -
extern int WINAPI LC_r_GetUVWavelengthTable(int iCanID, BYTE &dbTotalLines, BYTE &dbNowLine, P_WAVELENGTH_TABLE pwt);//查询波长程序表
extern int WINAPI LC_r_GetUVWavelengthScanRange(int iCanID, WORD &wStartWavelength, WORD &wEndWavelength);//查询波长扫描范围 -
extern int WINAPI LC_r_SetUVWavelengthSmooth(int iCanID);//设定波长和平滑指数 -
extern int WINAPI LC_r_SetUVWavelengthScan(int iCanID);//设定(执行)波长扫描 -
extern int WINAPI LC_r_ClsUVWavelengthTable(int iCanID);//清除波长程序表 -
extern int WINAPI LC_r_SetUVWavelengthTable(int iCanID);//增加/修改波长程序表 -
extern int WINAPI LC_r_DelUVWavelengthTable(int iCanID);//删除波长程序表行 -
extern int WINAPI LC_r_SetUVWavelengthZero(int iCanID);//更改调零设置 -
extern int WINAPI LC_r_AdjUVWavelength(int iCanID);//校准波长 -
extern int WINAPI LC_r_SetUVDeuteriumLampTime(int iCanID);//归零氘灯时间 -
extern int WINAPI LC_r_RunUVWavelengthScan(int iCanID);//执行波长扫描 -
extern int WINAPI LC_r_RunUVDeuteriumLamp(int iCanID);//执行氘灯开关 -
extern int WINAPI LC_r_RunUVWavelengthTable(int iCanID);//执行程序波长 -
extern int WINAPI LC_r_RunUVWavelengthZero(int iCanID);//执行调零 -
extern int WINAPI LC_r_RunUVWavelengthSelfCheck(int iCanID);//波长自检 -
extern int WINAPI LC_r_SetUVWavelengthAdjust(int iCanID);//设定波长校正值 -
extern int WINAPI LC_r_GetUVWavelengthAdjust(int iCanID, short &sAdjustValue);//查询波长校正值 -
//泵异步命令(接收)
extern int WINAPI LC_r_GetPumpConstantSpeed(int iCanID, WORD &wConstantSpeed);//查询恒流速度 -
extern int WINAPI LC_r_GetPumpGland(int iCanID, DWORD &dwLeftGland, DWORD &dwRightGland);//查询密封圈 -
extern int WINAPI LC_r_SetPumpGlandZero(int iCanID);//密封圈次数回零 -
extern int WINAPI LC_r_GetPumpPressure(int iCanID, WORD &wPressure, BYTE &dbUnit, DWORD &dwPressureRange, WORD &wVelocity, BYTE &dbSysStatus, BYTE &dbWorkStatus);//查询泵状态 -
extern int WINAPI LC_r_SetPumpConstantSpeed(int iCanID);//设置恒流速度 -
extern int WINAPI LC_r_SetPumpPressure(int iCanID);//设置压力值范围 -
extern int WINAPI LC_r_SetPumpWashParam(int iCanID);//设置洗脱参数 -
extern int WINAPI LC_r_GetPumpWashParam(int iCanID, WORD &wVelocity, WORD &wTime);//查询洗脱参数 -
extern int WINAPI LC_r_SetPumpPressureZero(int iCanID);//设置压力显示零位 -
extern int WINAPI LC_r_SetPumpRippleCompress(int iCanID);//设置脉动压缩 -
extern int WINAPI LC_r_ClsPumpGradsTable(int iCanID);//清除压力梯度表 -
extern int WINAPI LC_r_SetPumpGradsTable(int iCanID);//增加/修改压力梯度表 -
extern int WINAPI LC_r_DelPumpGradsTable(int iCanID);//删除压力梯度表行 -
extern int WINAPI LC_r_GetPumpGradsTable(int iCanID, BYTE &dbTotalLines, BYTE &dbNowLine, P_GRADS_TABLE pgt);//查询压力梯度表 -
extern int WINAPI LC_r_GetPumpPressureZero(int iCanID, WORD &wPressureZero);//查询泵压力零位值 -
extern int WINAPI LC_r_RunPumpCmd(int iCanID);//泵执行命令 -
extern int WINAPI LC_r_RunPumpSwitch(int iCanID);//执行泵开关 -
extern int WINAPI LC_r_RunPumpWash(int iCanID);//执行泵冲洗 -
extern int WINAPI LC_r_RunPumpGradsReady(int iCanID);//执行梯度准备 -
extern int WINAPI LC_r_RunPumpGrads(int iCanID);//执行梯度 -
extern int WINAPI LC_r_GetPumpTime(int iCanID, DWORD &dwPumpTime);//查询泵运行时间 -
//色谱异步命令(接收)
extern int WINAPI LC_r_RunGCSample(int iCanID);//开始采样 -
extern int WINAPI LC_r_GetGCRealtimeData(int iCanID, P_SAMPLE_RESULT ptSample, int &iNum, BOOL &bStartFlag);//实时数据 -
//温控异步命令(接收)
extern int WINAPI LC_r_GetTempBPS(int iCanID, WORD &wBPS);//获取温控通讯速率 -
extern int WINAPI LC_r_SetTempBPS(int iCanID);//设定温控通讯速率 -
extern int WINAPI LC_r_GetTempTemp(int iCanID, BYTE &dbAddr, WORD &wTrueTemp, WORD &wTemp);//查询设定温度 -
extern int WINAPI LC_r_SetTempTemp(int iCanID);//设定温度 -
//恒流源异步命令(接收)
extern int WINAPI LC_r_GetCCSVol(int iCanID, WORD &wVol);//查询恒流源电流 -
extern int WINAPI LC_r_GetCCSTemp(int iCanID, WORD &wTemp);//查询电导池温度 -
extern int WINAPI LC_r_GetCCSTSw(int iCanID, BYTE &eVal);//查询电导池温度开关 -
extern int WINAPI LC_r_SetCCSTSw(int iCanID);//设置电导池温度开关 -
extern int WINAPI LC_r_GetCCSRange(int iCanID, BYTE &eVal);//查询量程开关 -
extern int WINAPI LC_r_SetCCSRange(int iCanID);//设置量程开关 -
extern int WINAPI LC_r_GetCCSDASw(int iCanID, BYTE &bOnFlag);//查询恒流开关 -
extern int WINAPI LC_r_RunCCSDASw(int iCanID);//设置恒流开关 -
extern int WINAPI LC_r_GetCCSDAVal(int iCanID, BYTE &dbVal, BYTE &dbNowVal);//查询恒流电流 -
extern int WINAPI LC_r_SetCCSDAVal(int iCanID);//设置恒流电流 -

////////////////////////////////////////////////////////////////////////////////
//同步命令区
//设备同步命令 -
extern int WINAPI LC_GetCurrentTime(int iCanID, DWORD &dwTime);//查询当前时间 -
extern int WINAPI LC_SetCurrentTime(int iCanID, DWORD dwTime);//设置当前时间 -
extern int WINAPI LC_SetCanID(int iCanID, int iNewCanID);//设定设备CAN地址 -
extern int WINAPI LC_GetCanInfo(int iCanID, P_CAN_INFO ptCanInfo, int &iNum);//询问CAN节点属性 -
extern int WINAPI LC_RunWake(int iCanID, BOOL bOnFlag);//执行唤醒 -
extern int WINAPI LC_GetWarning(int iCanID, BYTE dbType, BYTE dbIdx, P_WARNING_INFO ptWarningInfo, int &iNum);//查询报警 -
extern int WINAPI LC_SetWarning(int iCanID, BYTE dbType, BYTE dbIdx, WORD wIdx);//报警复位 -
extern int WINAPI LC_GetRealtimeData(int iCanID, P_SAMPLE_RESULT ptSample, int &iNum, LCReturnCode &eStatus, DWORD &dwTime, DWORD &dwWaveLength);//实时数据 -
extern int WINAPI LC_SetLogicIndex(int iCanID, WORD wLogicIndex);//设置设备的逻辑编号 -
//紫外同步命令 -
extern int WINAPI LC_GetUVStatus(int iCanID, BYTE &dbSysStatus, BYTE &dbWorkStatus);//查询紫外状态 -
extern int WINAPI LC_GetUVWorkData(int iCanID, P_UV_WORK_DATA ptUVWorkData, int &iNum, BOOL bMultiData = FALSE);//查询紫外工作数据 -
extern int WINAPI LC_GetUVDeuteriumLampTime(int iCanID, WORD &wDeuteriumTime);//查询氘灯累计运行时 -
extern int WINAPI LC_GetUVWavelength(int iCanID, WORD &wWavelength);//查询设定波长 -
extern int WINAPI LC_GetUVWavelengthTable(int iCanID, BYTE dbLine, BYTE &dbTotalLines, BYTE &dbNowLine, P_WAVELENGTH_TABLE pwt);//查询波长程序表
extern int WINAPI LC_GetUVWavelengthScanRange(int iCanID, WORD &wStartWavelength, WORD &wEndWavelength);//查询波长扫描范围 -
extern int WINAPI LC_SetUVWavelengthSmooth(int iCanID, WORD wWavelength, BYTE dbSmoothValue);//设定波长和平滑指数 -
extern int WINAPI LC_SetUVWavelengthScan(int iCanID, BYTE dbWavelengthScan, WORD wStartWavelength, WORD wEndWavelength);//设定(执行)波长扫描 -
extern int WINAPI LC_ClsUVWavelengthTable(int iCanID);//清除波长程序表 -
extern int WINAPI LC_SetUVWavelengthTable(int iCanID, P_WAVELENGTH_TABLE pwt);//增加/修改波长程序表 -
extern int WINAPI LC_DelUVWavelengthTable(int iCanID, DWORD dwTime);//删除波长程序表行 -
extern int WINAPI LC_SetUVWavelengthZero(int iCanID, BOOL bZeroFlag);//更改调零设置 -
extern int WINAPI LC_AdjUVWavelength(int iCanID, WORD wParam);//校准波长 -
extern int WINAPI LC_SetUVDeuteriumLampTime(int iCanID);//归零氘灯时间 -
extern int WINAPI LC_RunUVWavelengthScan(int iCanID, BOOL bOnFlag);//执行波长扫描 -
extern int WINAPI LC_RunUVDeuteriumLamp(int iCanID, BOOL bOnFlag);//执行氘灯开关 -
extern int WINAPI LC_RunUVWavelengthTable(int iCanID, BOOL bOnFlag);//执行程序波长 -
extern int WINAPI LC_RunUVWavelengthZero(int iCanID);//执行调零 -
extern int WINAPI LC_RunUVWavelengthSelfCheck(int iCanID);//波长自检 -
extern int WINAPI LC_SetUVWavelengthAdjust(int iCanID, short sAdjustValue);//设定波长校正值 -
extern int WINAPI LC_GetUVWavelengthAdjust(int iCanID, short &sAdjustValue);//查询波长校正值 -
//泵同步命令 -
extern int WINAPI LC_GetPumpConstantSpeed(int iCanID, WORD &wConstantSpeed);//查询恒流速度 -
extern int WINAPI LC_GetPumpGland(int iCanID, DWORD &dwLeftGland, DWORD &dwRightGland);//查询密封圈 -
extern int WINAPI LC_SetPumpGlandZero(int iCanID);//密封圈次数回零 -
extern int WINAPI LC_GetPumpPressure(int iCanID, WORD &wPressure, BYTE &dbUnit, DWORD &dwPressureRange, WORD &wVelocity, BYTE &dbSysStatus, BYTE &dbWorkStatus);//查询泵状态 -
extern int WINAPI LC_SetPumpConstantSpeed(int iCanID, WORD wConstantSpeed);//设置恒流速度 -
extern int WINAPI LC_SetPumpPressure(int iCanID, WORD wPressureHigh, WORD wPressureLow, BYTE dbUnit);//设置压力值范围 -
extern int WINAPI LC_SetPumpWashParam(int iCanID, WORD wVelocity, WORD wTime);//设置洗脱参数 -
extern int WINAPI LC_GetPumpWashParam(int iCanID, WORD &wVelocity, WORD &wTime);//查询洗脱参数 -
extern int WINAPI LC_SetPumpPressureZero(int iCanID, WORD wPressureZero);//设置压力显示零位 -
extern int WINAPI LC_SetPumpRippleCompress(int iCanID, WORD wRippleCompress);//设置脉动压缩 -
extern int WINAPI LC_ClsPumpGradsTable(int iCanID);//清除压力梯度表 -
extern int WINAPI LC_SetPumpGradsTable(int iCanID, P_GRADS_TABLE pgt);//增加/修改压力梯度表 -
extern int WINAPI LC_DelPumpGradsTable(int iCanID, DWORD dwTime);//删除压力梯度表行 -
extern int WINAPI LC_GetPumpGradsTable(int iCanID, BYTE dbLine, BYTE &dbTotalLines, BYTE &dbNowLine, P_GRADS_TABLE pgt);//查询压力梯度表 -
extern int WINAPI LC_GetPumpPressureZero(int iCanID, WORD &wPressureZero);//查询泵压力零位值 -
extern int WINAPI LC_RunPumpCmd(int iCanID, enum LCPumpCmd eCmd);//泵执行命令 -
extern int WINAPI LC_RunPumpSwitch(int iCanID, BOOL bOnFlag);//执行泵开关 -
extern int WINAPI LC_RunPumpWash(int iCanID, BOOL bOnFlag);//执行泵冲洗 -
extern int WINAPI LC_RunPumpGradsReady(int iCanID, BOOL bOnFlag);//执行梯度准备 -
extern int WINAPI LC_RunPumpGrads(int iCanID, BOOL bOnFlag);//执行梯度 -
extern int WINAPI LC_GetPumpTime(int iCanID, DWORD &dwPumpTime);//查询泵运行时间 -
//色谱同步命令 -
extern int WINAPI LC_RunGCSample(int iCanID, BYTE dbChannel);//开始采样 -
extern int WINAPI LC_GetGCRealtimeData(int iCanID, BYTE dbChannel, P_SAMPLE_RESULT ptSample, int &iNum, BOOL &bStartFlag);//实时数据 -
extern void WINAPI LC_TranslateGCRealtimeData(P_SAMPLE_RESULT ptDst, P_SAMPLE_RESULT ptSrc, int iNum, BYTE eVal);//根据量程转换结果 -
//温控同步命令 -
extern int WINAPI LC_GetTempBPS(int iCanID, WORD &wBPS);//获取温控通讯速率 -
extern int WINAPI LC_SetTempBPS(int iCanID, WORD wBPS);//设定温控通讯速率 -
extern int WINAPI LC_GetTempTemp(int iCanID, BYTE dbAddr, WORD &wTrueTemp, WORD &wTemp);//查询设定温度 -
extern int WINAPI LC_SetTempTemp(int iCanID, BYTE dbAddr, WORD wTemp);//设定温度 -
//恒流源同步命令 -
extern int WINAPI LC_GetCCSVol(int iCanID, WORD &wVol);//查询恒流源电流 -
extern int WINAPI LC_GetCCSTemp(int iCanID, WORD &wTemp);//查询电导池温度 -
extern int WINAPI LC_GetCCSTSw(int iCanID, BYTE &eVal);//查询电导池温度开关 -
extern int WINAPI LC_SetCCSTSw(int iCanID, BYTE eVal);//设置电导池温度开关 -
extern int WINAPI LC_GetCCSRange(int iCanID, BYTE &eVal);//查询量程开关 -
extern int WINAPI LC_SetCCSRange(int iCanID, BYTE eVal);//设置量程开关 -
extern int WINAPI LC_GetCCSDASw(int iCanID, BYTE &bOnFlag);//查询恒流开关 -
extern int WINAPI LC_RunCCSDASw(int iCanID, BYTE bOnFlag);//设置恒流开关 -
extern int WINAPI LC_GetCCSDAVal(int iCanID, BYTE &dbVal, BYTE &dbNowVal);//查询恒流电流 -
extern int WINAPI LC_SetCCSDAVal(int iCanID, BYTE dbVal);//设置恒流电流 -

//特殊的应答命令 -
extern int WINAPI LC_ResponseCanInfo(int iCanID);
