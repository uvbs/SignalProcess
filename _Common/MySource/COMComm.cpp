#include "stdafx.h"
#include <winbase.h>
#include "COMComm.h"

#ifndef _T
#define _T(x)	__TEXT(x)
#endif

cCOMComm::cCOMComm()
{
	m_hCOM = INVALID_HANDLE_VALUE;
	m_bAttachFlag = FALSE;
	m_dwLastTimeout = 1000;
	m_dbByteSize = 8;
	m_dbStopBits = ONESTOPBIT;
}
cCOMComm::~cCOMComm()
{
	Close();
}
BOOL cCOMComm::Open(int iComIdx, int iBPS, int iTimeout)
{
	DCB dcb;
	COMMTIMEOUTS cto;
	int i;
	LPCTSTR lpcszComName;
	TCHAR szComName[12]={_T('\\'), _T('\\'), _T('.'), _T('\\'), _T('C'), _T('O'), _T('M')};
	Close();
	if (iComIdx < StartCOMNo || iComIdx > EndCOMNo) return FALSE;
	i = 7;
	if (iComIdx > 9) {
		lpcszComName = szComName;
		if (iComIdx > 99) {
			szComName[i++] = _T('0') + (TCHAR)(iComIdx / 100);
			iComIdx %= 100;
		}
		szComName[i++] = _T('0') + (TCHAR)(iComIdx / 10);
		iComIdx %= 10;
	}
	else {
		lpcszComName = &szComName[4];
	}
	szComName[i++] = _T('0') + (TCHAR)iComIdx;
	szComName[i] = 0;
	m_hCOM = CreateFile(lpcszComName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
	if (m_hCOM == INVALID_HANDLE_VALUE) return FALSE;
	SetupComm(m_hCOM, 0x8000, 0x000);
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = iTimeout;
	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = iTimeout;
	SetCommTimeouts(m_hCOM, &cto);
	GetCommState(m_hCOM, &dcb);
	dcb.BaudRate = iBPS;
	*((LPDWORD)&dcb.BaudRate + 1) = 0;
	dcb.fBinary = TRUE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.ByteSize = m_dbByteSize;
	dcb.Parity = 0;
	dcb.StopBits = m_dbStopBits;
	/*{
		HANDLE hf;
		DWORD dwSize;
		TCHAR szFilename[16] = {L"C:\\_dcb_com_"};
		szFilename[12] = szComName[7];
		szFilename[13] = szComName[8];
		szFilename[14] = szComName[9];
		hf = CreateFile(szFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hf, &dcb, sizeof(dcb), &dwSize, NULL);
		CloseHandle(hf);
	}*/
	SetCommState(m_hCOM, &dcb);
	PurgeComm(m_hCOM, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
	return TRUE;
}
void cCOMComm::Close()
{
	if (m_hCOM != INVALID_HANDLE_VALUE) {
		if (m_bAttachFlag) {
			m_bAttachFlag = FALSE;
		}
		else {
			CloseHandle(m_hCOM);
		}
		m_hCOM = INVALID_HANDLE_VALUE;
	}
}
BOOL cCOMComm::IsOpen() const
{
	return (m_hCOM == INVALID_HANDLE_VALUE) ? FALSE : TRUE;
}
BOOL cCOMComm::Attach(HANDLE hCOM)
{
	if (m_bAttachFlag) {
		m_bAttachFlag = TRUE;
	}
	else {
		if (m_hCOM != INVALID_HANDLE_VALUE) return FALSE;
	}
	m_hCOM = hCOM;
	return TRUE;
}
HANDLE cCOMComm::Detach()
{
	HANDLE hCOM = INVALID_HANDLE_VALUE;
	if (m_bAttachFlag) {
		m_bAttachFlag = FALSE;
		hCOM = m_hCOM;
		m_hCOM = INVALID_HANDLE_VALUE;
	}
	return hCOM;
}
BOOL cCOMComm::PutText(LPCSTR lpcszCmd) const
{
	DWORD dwSize;
	return WriteFile(m_hCOM, lpcszCmd, strlen(lpcszCmd), &dwSize, NULL);
}
BOOL cCOMComm::GetText(LPSTR lpszText, char cEnd) const
{
	LPSTR lpszTmp;
	DWORD dwSize;
	for (lpszTmp = lpszText; ;) {
		if (!ReadFile(m_hCOM, lpszTmp, 1, &dwSize, NULL)) return FALSE;
		if (dwSize != 1) return FALSE;
		if (*lpszTmp++ == cEnd) break;
	}
	*lpszTmp = 0;
	return TRUE;
}
BOOL cCOMComm::GetText_s(LPSTR lpszText, UINT uLimit, char cEnd) const
{
	LPSTR lpszTmp;
	DWORD dwSize;
	UINT i;
	for (lpszTmp = lpszText, i = 1; i < uLimit; i++) {
		if (!ReadFile(m_hCOM, lpszTmp, 1, &dwSize, NULL)) return FALSE;
		if (dwSize != 1) return FALSE;
		if (*lpszTmp++ == cEnd) break;
	}
	*lpszTmp = 0;
	return (i < uLimit) ? TRUE : FALSE;
}
BOOL cCOMComm::PutData(LPVOID lpData, DWORD dwLength) const
{
	DWORD dwSize;
	return WriteFile(m_hCOM, lpData, dwLength, &dwSize, NULL);
}
BOOL cCOMComm::GetData(LPVOID lpData, DWORD dwLength) const
{
	DWORD dwSize;
	if (!ReadFile(m_hCOM, lpData, dwLength, &dwSize, NULL)) return FALSE;
	if (dwSize != dwLength) return FALSE;
	return TRUE;
}
cCOMComm::enumDebugError cCOMComm::GetData_Debug(LPVOID lpData, DWORD dwLength) const
{
	DWORD dwSize;
	if (!ReadFile(m_hCOM, lpData, dwLength, &dwSize, NULL)) return DE_READFAILED;
	if (dwSize != dwLength) return DE_DATA ;
	return DE_OK;
}
void cCOMComm::SetTotalTimeout(int iTimeout)
{
	COMMTIMEOUTS cto;
	GetCommTimeouts(m_hCOM, &cto);
	if (iTimeout) {
		m_dwLastTimeout = cto.ReadTotalTimeoutConstant;
		cto.ReadTotalTimeoutConstant = iTimeout;
	}
	else {
		cto.ReadTotalTimeoutConstant = m_dwLastTimeout;
	}
	SetCommTimeouts(m_hCOM, &cto);
}
