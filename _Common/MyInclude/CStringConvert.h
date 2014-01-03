
#ifndef TWEN_SDK_CSTRINGCONVERT_
#define TWEN_SDK_CSTRINGCONVERT_

/*
用法：
TWen_USES_CONVERSION;
CString x(_T("中文English"));
LPCSTR s=TWen_CT2A(x);
//*/

class TW_CA2W
{
public:
	TW_CA2W(LPCSTR ansiStr=NULL,bool bAutoRelease=true);
	~TW_CA2W();
public:
	void SetANSI(LPCSTR ansiStr);
	void SetAutoRelease(bool bAutoRelease=true);
	LPWSTR ToBSTR() const{return m_bstr;}
	LPWSTR ToBSTR(LPCSTR ansiStr);
	operator LPWSTR() const{return m_bstr;}
	operator LPCWSTR() const{return (LPCWSTR)m_bstr;}
private:
	BSTR m_bstr;
	bool m_bAutoRelease;
};

class TW_CW2A
{
public:
	TW_CW2A(LPCWSTR bstrStr=NULL,bool bAutoRelease=true);
	~TW_CW2A();
public:
	void SetBSTR(LPCWSTR bstrStr);
	void SetAutoRelease(bool bAutoRelease=true);
	LPSTR ToANSI() const{return m_str;}
	LPSTR ToANSI(LPCWSTR bstrStr);
	operator LPSTR() const{return m_str;}
	operator LPCSTR() const{return (LPCSTR)m_str;}
private:
	LPSTR m_str;
	bool m_bAutoRelease;
};

#define TWen_USES_CONVERSION TW_CA2W _twca2w;TW_CW2A _twcw2a

#ifdef _UNICODE

#define TWen_CT2A(s) _twcw2a.ToANSI(s)
#define TWen_CT2W(s) (s)
#define TWen_CA2T(s) _twca2w.ToBSTR(s)
#define TWen_CW2T(s) (s)

#else  // !_UNICODE

#define TWen_CT2A (s)
#define TWen_CT2W _twca2w.ToBSTR(s)
#define TWen_CA2T (s)
#define TWen_CW2T _twcw2a.ToANSI(s)

#endif

#endif