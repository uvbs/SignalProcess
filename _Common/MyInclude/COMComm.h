#pragma once

class cCOMComm {
public:
	enum enumDebugError
	{
		DE_READFAILED,
		DE_DATA,
		DE_OK,
	};

private:
	HANDLE m_hCOM;
	BOOL m_bAttachFlag;
protected:
	DWORD m_dwLastTimeout;
	BYTE m_dbByteSize;
	BYTE m_dbStopBits;
public:
	cCOMComm();
	virtual ~cCOMComm();
	BOOL Open(int iComIdx, int iBPS, int iTimeout);
	void Close();
	BOOL IsOpen() const;
	HANDLE GetSafeHandle() const {return m_hCOM;}
	BOOL Attach(HANDLE hCOM);
	HANDLE Detach();
	BOOL IsAttachHandle() const {return m_bAttachFlag;}
	BOOL PutText(LPCSTR lpcszCmd) const;
	BOOL GetText(LPSTR lpszText, char cEnd = 13) const;
	BOOL GetText_s(LPSTR lpszText, UINT uLimit, char cEnd = 13) const;
	BOOL PutData(LPVOID lpData, DWORD dwLength) const;
	BOOL GetData(LPVOID lpData, DWORD dwLength) const;
	enumDebugError GetData_Debug(LPVOID lpData, DWORD dwLength) const;
	void SetTotalTimeout(int iTimeout = 0);
};

enum COMCOMOfs{StartCOMNo = 1, EndCOMNo = 256};
