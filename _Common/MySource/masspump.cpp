#include "stdafx.h"
#include "masspump.h"

CMassPumpCmd::CMassPumpCmd() : cCOMComm()
{
	m_iComIdx = 0, m_iComBps = 0,m_iComTimeout = 0;
	m_dwErrorCount = 0;
}

BOOL CMassPumpCmd::init(int iComIdx, int iBPS, int iTimeout)
{
	int iValue;
	if (Open(iComIdx, iBPS, iTimeout)) {
		if (GetPumpRev(iValue) == pump_OK && iValue >= 0) {
			m_iComIdx = iComIdx, m_iComBps = iBPS, m_iComTimeout = iTimeout;
			m_dwErrorCount = 0;
			return TRUE;
		}
		Close();
	}
	return FALSE;
}
int CMassPumpCmd::search(int iBPS, int iTimeout)
{
	int i;
	for (i = StartCOMNo; i <= EndCOMNo; i++) {
		if (init(i, iBPS, iTimeout)) return i;
	}
	return 0;
}

void CMassPumpCmd::exit()
{
	Close();
}

const char CMassPumpCmd::l_szCmdFmt[] = {"001003%02d02=?%03d\r\n"};

int CMassPumpCmd::exec(int iCmd, int iQuery, int &iValue)
{
	char szCmdBuf[Buf__Size];
	char szRstBuf[Buf__Size];
#if __STDC_WANT_SECURE_LIB__
	sprintf_s(szCmdBuf, Buf__Size, l_szCmdFmt, iCmd, iQuery);
#else
	sprintf(szCmdBuf, l_szCmdFmt, iCmd, iQuery);
#endif
	if (PutData(szCmdBuf, Cmd__Size) && GetData(szRstBuf, Rst__Size)) {
		if (szRstBuf[Rst__Size - 1] == 13 && CheckRstValid(szCmdBuf, szRstBuf)) {
			szRstBuf[Rst__Ofs + Rst__Len] = 0;
			iValue = atoi(&szRstBuf[Rst__Ofs]);
			m_dwErrorCount = 0;
			return pump_OK;
		}
	}
	m_dwErrorCount++;
	if (m_dwErrorCount > 3 && !IsAttachHandle()) {//连续3次以上错误且不是外部句柄.
		//重新尝试连接串口，如成功则清除错误计数.
		init(m_iComIdx, m_iComBps, m_iComTimeout);
	}
	return pump_COMErr;
}

BOOL CMassPumpCmd::CheckRstValid(const char szCmd[Cmd__Size], const char szRst[Rst__Size])
{
	int i,iSum;
	if (szRst[RetCode1_Ofs] != '1' || szRst[RetCode2_Ofs] != '6') return FALSE;
	for (iSum = 0, i = 0; i < ChkSum__Ofs; i++) {
		iSum += (int)(szRst[i] - '0');
	}
	return (iSum == atoi(&szRst[ChkSum__Ofs])) ? TRUE : FALSE;
}
