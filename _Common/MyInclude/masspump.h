#pragma once

#include "COMComm.h"

enum PumpReturnCode {
	pump_COMErr = -1, pump_OK = 0
};

class CMassPumpCmd : public cCOMComm {
private:
	int m_iComIdx,m_iComBps,m_iComTimeout;
	DWORD m_dwErrorCount;
public:
	CMassPumpCmd();
	BOOL init(int iComIdx, int iBPS, int iTimeout);
	int search(int iBPS, int iTimeout);
	void exit();
protected:
	int exec(int iCmd, int iQuery, int &iValue);
public:
	int GetPumpRev(int &iValue) {return exec(98,115,iValue);}//转速(rpm)
	int GetPumpFrequency(int &iValue) {return exec(9,107,iValue);}//频率(Hz)
	int GetPumpPower(int &iValue) {return exec(16,105,iValue);}//功率(W)
	int GetPumpTemp(int &iValue) {return exec(46,108,iValue);}//温度(℃)
	int GetPumpVoltage(int &iValue) {return exec(13,102,iValue);}//电压(mV)
	int GetPumpElectricity(int &iValue) {return exec(10,99,iValue);}//电流(mA)
protected:
	enum {Cmd__Size = 17, Rst__Size = 20, RetCode1_Ofs = 3, RetCode2_Ofs = 9, Rst__Ofs = 10, Rst__Len = 6, ChkSum__Ofs = 16, Buf__Size = 32};
	static BOOL CheckRstValid(const char szCmd[Cmd__Size], const char szRst[Rst__Size]);
	static const char l_szCmdFmt[];
};
