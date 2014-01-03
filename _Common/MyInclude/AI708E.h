#pragma once

enum AIType {
	aiT_get = 0x52,
	aiT_set = 0x43
};

enum AIIdx {
	aiIdx_SV = 0x00,   //给定值 -
	aiIdx_HIAL = 0x01, //上限报警 -
	aiIdx_LoAL = 0x02, //下限报警 -
	aiIdx_dHAL = 0x03, //正偏差报警 -
	aiIdx_dLAL = 0x04, //负偏差报警 -
	aiIdx_dF = 0x05,   //回差 -
	aiIdx_CtrL = 0x06, //控制方式 -
	aiIdx_M50 = 0x07,  //保持参数 -
	aiIdx_P = 0x08,    //速率参数 -
	aiIdx_t = 0x09,    //滞后参数 -
	aiIdx_CtI = 0x0A,  //控制周期 -
	aiIdx_Sn = 0x0B,   //输入规格 -
	aiIdx_dIP = 0x0C,  //小数点位置 -
	aiIdx_dIL = 0x0D,  //下限显示值 -
	aiIdx_dIH = 0x0E,  //上限显示值 -
	aiIdx_CJC = 0x0F,  //冷端补偿 (面板中没有)
	aiIdx_Sc = 0x10,   //传感器修正 -
	aiIdx_oP1 = 0x11,  //输出方式 -
	aiIdx_oPL = 0x12,  //输出下限 -
	aiIdx_oPH = 0x13,  //输出上限 -
	aiIdx_CF = 0x14,   //系统功能选择 -
	aiIdx_Baud = 0x15, //波特率 (不能改，否则要重新连接)
	aiIdx_Addr = 0x16, //通讯地址 (不能改，否则要重新连接)
	aiIdx_Soft = 0x17, //数字滤波 -
	aiIdx_run = 0x18,  //运行参数 -
	aiIdx_Loc = 0x19,   //参数封锁 -

	//以下，需要开启程序升温功能
	aiIdx_PT_1Temperature = 0x1A, // 程序升温第一段温度
	aiIdx_PT_1Time, // 程序升温第一段时间
	aiIdx_PT_2Temperature ,
	aiIdx_PT_2Time,
	aiIdx_PT_3Temperature ,
	aiIdx_PT_3Time,
	aiIdx_PT_4Temperature ,
	aiIdx_PT_4Time,
	aiIdx_PT_5Temperature ,
	aiIdx_PT_5Time,
	aiIdx_PT_6Temperature ,
	aiIdx_PT_6Time,
	aiIdx_PT_7Temperature ,
	aiIdx_PT_7Time,
	aiIdx_PT_8Temperature ,
	aiIdx_PT_8Time,
	aiIdx_PT_9Temperature ,
	aiIdx_PT_9Time,
	aiIdx_PT_10Temperature ,
	aiIdx_PT_10Time,
	aiIdx_PT_11Temperature ,
	aiIdx_PT_11Time,
	aiIdx_PT_12Temperature ,
	aiIdx_PT_12Time,
	aiIdx_PT_13Temperature ,
	aiIdx_PT_13Time,
	aiIdx_PT_14Temperature ,
	aiIdx_PT_14Time,
	aiIdx_PT_15Temperature ,
	aiIdx_PT_15Time,
	aiIdx_PT_16Temperature ,
	aiIdx_PT_16Time,
	aiIdx_PT_17Temperature ,
	aiIdx_PT_17Time,
	aiIdx_PT_18Temperature ,
	aiIdx_PT_18Time,
	aiIdx_PT_19Temperature ,
	aiIdx_PT_19Time,
	aiIdx_PT_20Temperature ,
	aiIdx_PT_20Time,
	aiIdx_PT_21Temperature ,
	aiIdx_PT_21Time,
	aiIdx_PT_22Temperature ,
	aiIdx_PT_22Time,
	aiIdx_PT_23Temperature ,
	aiIdx_PT_23Time,
	aiIdx_PT_24Temperature ,
	aiIdx_PT_24Time,
	aiIdx_PT_25Temperature ,
	aiIdx_PT_25Time,
	aiIdx_PT_26Temperature ,
	aiIdx_PT_26Time,
	aiIdx_PT_27Temperature ,
	aiIdx_PT_27Time,
	aiIdx_PT_28Temperature ,
	aiIdx_PT_28Time,
	aiIdx_PT_29Temperature ,
	aiIdx_PT_29Time,
	aiIdx_PT_30Temperature = 0x54, // 程序升温第三十段温度
	aiIdx_PT_30Time, // 程序升温第三十段时间
	aiIdx_PT_CPTime = 0x56, // 程序升温当前段已运行的时间

	AIIdx_Num,
};

enum AIReturnCode {
	ai_FearfulErr = -2, ai_COMErr = -1, ai_OK = 0, ai_Invalid, ai_Failed
};

#ifdef AI708E_EXPORTS
#define AI708E_DLL	__declspec(dllexport)
#else
#define AI708E_DLL	__declspec(dllimport)
#endif

//基本函数 -
extern AI708E_DLL HANDLE WINAPI AI708E_Create(int iComIdx, int iAddr, int iBPS, int iTimeout = 500);
extern AI708E_DLL void WINAPI AI708E_Close(HANDLE h);
extern AI708E_DLL int WINAPI AI708E_Get(HANDLE h, int iAddr, AIIdx eIdx, short* lpsPara = NULL, short* lpsPV = NULL, short* lpsSV = NULL, LPBYTE lpdbMV = NULL, LPBYTE lpdbWarning = NULL);
extern AI708E_DLL int WINAPI AI708E_Set(HANDLE h, int iAddr, AIIdx eIdx, short sPara);
extern AI708E_DLL int WINAPI AI708E_GetSendRecvInfo(HANDLE h, LPBYTE lpdbData);

extern AI708E_DLL void WINAPI AI708E_EnablePT(HANDLE h, bool bEnabled);//是否开启程序升温功能

