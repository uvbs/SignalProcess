
#include "stdafx.h"
#include "CStringConvert.h"

///////////////////////////////
TW_CA2W::TW_CA2W(LPCSTR ansiStr,bool bAutoRelease)
{
	m_bstr=NULL;
	SetAutoRelease(bAutoRelease);
	SetANSI(ansiStr);
}
TW_CA2W::~TW_CA2W()
{
	if(m_bAutoRelease && m_bstr){delete []m_bstr;m_bstr=NULL;}
}
void TW_CA2W::SetAutoRelease(bool bAutoRelease){m_bAutoRelease=bAutoRelease;}
void TW_CA2W::SetANSI(LPCSTR ansiStr)
{
	if(m_bstr){delete []m_bstr;m_bstr=NULL;}
	if(!ansiStr) return;
	int len=lstrlenA(ansiStr)+1;
	m_bstr=new WCHAR[len];
	MultiByteToWideChar(CP_ACP,0,ansiStr,len, m_bstr,len);
}
LPWSTR TW_CA2W::ToBSTR(LPCSTR ansiStr)
{
	SetANSI(ansiStr);
	return ToBSTR();
}
///////////////////////////////////////
TW_CW2A::TW_CW2A(LPCWSTR bstrStr,bool bAutoRelease)
{
	m_str=NULL;
	SetAutoRelease(bAutoRelease);
	SetBSTR(bstrStr);
}
TW_CW2A::~TW_CW2A(){if(m_bAutoRelease && m_str) delete []m_str;}
void TW_CW2A::SetAutoRelease(bool bAutoRelease){m_bAutoRelease=bAutoRelease;}
void TW_CW2A::SetBSTR(LPCWSTR bstrStr)
{
	if(m_str){delete []m_str;m_str=NULL;}
	if(!bstrStr) return;
	int len=lstrlenW(bstrStr)+1;
	m_str=new char[len*2];
	m_str[0]=0;
	WideCharToMultiByte(CP_ACP,0,bstrStr,len,m_str,len*2,NULL,NULL);
}
LPSTR TW_CW2A::ToANSI(LPCWSTR bstrStr)
{
	SetBSTR(bstrStr);
	return ToANSI();
}