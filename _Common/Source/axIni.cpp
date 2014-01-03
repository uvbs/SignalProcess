// Ini.cpp: Implementierung der Klasse CIni.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "axIni.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// If the IniFilename contains no path,
// the module-directory will be add to the FileName,
// to avoid storing in the windows-directory
/*static*/ void CIni::AddModulPath(CString& strFileName,BOOL bModulPath /*= TRUE*/)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( strFileName, drive, dir, fname, ext );
	if ( ! drive[0]  )
	{
		//PathCanonicalize(..) doesn't work with for all Plattforms !
		CString strModule;
		if ( bModulPath )
			GetModuleFileName(NULL,strModule.GetBuffer(MAX_INI_BUFFER),MAX_INI_BUFFER);
		else
			GetCurrentDirectory(MAX_INI_BUFFER,strModule.GetBuffer(MAX_INI_BUFFER));
		strModule.ReleaseBuffer();
		_splitpath( strModule, drive, dir, fname, ext );
		strModule = drive;
		strModule+= dir;
		strModule+= strFileName;
		strFileName = strModule;
	}
}
/*static*/ CString CIni::GetDefaultSection()
{
	return AfxGetAppName();
}
/*static*/ CString CIni::GetDefaultAppName(void)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	CString strTemp;
	CString strApplName;
	GetModuleFileName(NULL,strTemp.GetBuffer(MAX_INI_BUFFER),MAX_INI_BUFFER);
	strTemp.ReleaseBuffer();
	_splitpath( strTemp, drive, dir, fname, ext );
	strApplName = fname;

	return strApplName;
}


/*static*/ CString CIni::GetDefaultIniFile(BOOL bModulPath /*= TRUE*/)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	CString strTemp;
	CString strApplName;
	GetModuleFileName(NULL,strTemp.GetBuffer(MAX_INI_BUFFER),MAX_INI_BUFFER);
	strTemp.ReleaseBuffer();
	_splitpath( strTemp, drive, dir, fname, ext );
	strTemp = fname; //"ApplName"
	strTemp += ".ini";  //"ApplName.ini"
	if ( bModulPath )
	{
		strApplName  = drive;
		strApplName += dir;
		strApplName += strTemp;
	}
	else
	{
		GetCurrentDirectory(MAX_INI_BUFFER,strApplName.GetBuffer(MAX_INI_BUFFER));
		strApplName.ReleaseBuffer();
		strApplName.TrimRight('\\');
		strApplName.TrimRight('/');
		strApplName += "\\";
		strApplName += strTemp;
	}
	return strApplName;
}



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
// Creates/Use file : "Drive:\ApplPath\ApplName.ini"
CIni::CIni(BOOL bModulPath /*= TRUE*/):
	m_bModulPath(bModulPath),
		m_bFromRegistry(FALSE)
{
	m_strFileName = GetDefaultIniFile(m_bModulPath);
//	m_strSection  = GetDefaultSection();
	m_strSection  = GetDefaultAppName();
	m_strAppName  = GetDefaultAppName();
}
CIni::CIni(CIni const& Ini, BOOL bModulPath /*= TRUE*/):
	m_strAppName(Ini.m_strAppName),
	m_strFileName(Ini.m_strFileName),
	m_strSection(Ini.m_strSection),
	m_bModulPath(Ini.m_bModulPath),
	m_bFromRegistry(FALSE)
{
	if (m_strFileName.IsEmpty())
		m_strFileName = GetDefaultIniFile(m_bModulPath);
	AddModulPath(m_strFileName,m_bModulPath);
	if (m_strAppName.IsEmpty())
		m_strAppName = GetDefaultAppName();
	if (m_strSection.IsEmpty())
		m_strSection = GetDefaultSection();
}
CIni::CIni(CString const& strFileName, BOOL bModulPath /*= TRUE*/):
	m_strFileName(strFileName),
	m_bModulPath(bModulPath),
		m_bFromRegistry(FALSE)
{
	if (m_strFileName.IsEmpty())
		m_strFileName = GetDefaultIniFile(m_bModulPath);
	AddModulPath(m_strFileName,bModulPath);
	if (m_strAppName.IsEmpty())
		m_strAppName = GetDefaultAppName();
	m_strSection = GetDefaultSection();
}
CIni::CIni(CString const& strFileName, CString const& strSection, BOOL bModulPath /*= TRUE*/):
	m_strSection(strSection),
	m_bModulPath(bModulPath),
	m_bFromRegistry(FALSE)
{
	if (m_strFileName.IsEmpty())
		m_strFileName = GetDefaultIniFile(m_bModulPath);
	AddModulPath(m_strFileName,bModulPath);
	if (m_strAppName.IsEmpty())
		m_strAppName = GetDefaultAppName();
	if (m_strSection.IsEmpty())
		m_strSection = GetDefaultSection();
}

CIni::~CIni()
{
}
//////////////////////////////////////////////////////////////////////
// Zugriff auf Quelle/Ziel von IO-Operationen
//////////////////////////////////////////////////////////////////////
void CIni::SetFileName(CString const& strFileName)
{
	m_strFileName = strFileName;
	AddModulPath(m_strFileName);
}
void CIni::SetAppName(CString const& strAppName)
{
	m_strAppName = strAppName;
}
void CIni::SetSection(CString const& strSection)
{
	m_strSection = strSection;
}

CString const& CIni::GetFileName() const
{
	return m_strFileName;
}
CString const& CIni::GetAppName() const
{
	return m_strAppName;
}

CString const& CIni::GetSection() const
{
	return m_strSection;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

void CIni::Init( LPCSTR strFileName, LPCSTR strSection/* = NULL*/)
{
	if (strSection != NULL)
		m_strSection = strSection;
	if (strFileName != NULL)
		m_strFileName = strFileName;
}
CString CIni::GetString(CString strEntry,LPCSTR strDefault/*=NULL*/,LPCSTR strSection/* = NULL*/)
{
	if (strDefault == NULL)
		return CString(GetLPCSTR(strEntry,strSection,""));
	else
		return CString(GetLPCSTR(strEntry,strSection,strDefault));
}
double CIni::GetDouble(CString strEntry,double fDefault/* = 0.0*/,LPCSTR strSection/* = NULL*/)
{
	CString strDefault;
	strDefault.Format("%g",fDefault);
	GetLPCSTR(strEntry,strSection,strDefault);
	return atof(m_chBuffer);
}
float CIni::GetFloat(CString strEntry,float fDefault/* = 0.0*/, LPCSTR strSection/* = NULL*/)
{
	CString strDefault;
	strDefault.Format("%g",fDefault);
	GetLPCSTR(strEntry,strSection,strDefault);
	return (float)atof(m_chBuffer);
}
int CIni::GetInt(CString strEntry,int nDefault/* = 0*/,LPCSTR strSection/* = NULL*/)
{
	CString strDefault;
	strDefault.Format("%d",nDefault);
	GetLPCSTR(strEntry,strSection,strDefault);
	return atoi(m_chBuffer);
}
DWORD CIni::GetDWORD(CString strEntry,DWORD nDefault/* = 0*/,LPCSTR strSection/* = NULL*/)
{
	CString strDefault;
	strDefault.Format("%d",nDefault);
	GetLPCSTR(strEntry,strSection,strDefault);
	return (DWORD)atoi(m_chBuffer);
}
BOOL CIni::GetBool(CString strEntry,BOOL bDefault/* = FALSE*/,LPCSTR strSection/* = NULL*/)
{
	CString strDefault;
	strDefault.Format("%d",bDefault);
	GetLPCSTR(strEntry,strSection,strDefault);
	return ( atoi(m_chBuffer) != 0 );
}
CPoint CIni::GetPoint(CString strEntry, CPoint ptDefault, LPCSTR strSection)
{
	CPoint ptReturn=ptDefault;

	CString strDefault;
	strDefault.Format("(%d,%d)",ptDefault.x, ptDefault.y);

	CString strPoint = GetString(strEntry,strDefault);
	sscanf(strPoint,"(%d,%d)", &ptReturn.x, &ptReturn.y);

	return ptReturn;
}
CRect CIni::GetRect(CString strEntry, CRect rectDefault, LPCSTR strSection)
{
	CRect rectReturn=rectDefault;

	CString strDefault;
	strDefault.Format("(%d,%d,%d,%d)",rectDefault.top,rectDefault.left,rectDefault.bottom,rectDefault.right);

	CString strRect = GetString(strEntry,strDefault);
	sscanf(strRect,"(%d,%d,%d,%d)", &rectReturn.top,&rectReturn.left,&rectReturn.bottom,&rectReturn.right);

	return rectReturn;
}
COLORREF CIni::GetColRef(CString strEntry, COLORREF crDefault, LPCSTR strSection)
{
	int temp[3]=
	{	GetRValue(crDefault),
		GetGValue(crDefault),
		GetBValue(crDefault)
	};
	CString strDefault;
	strDefault.Format("RGB(%hd,%hd,%hd)",temp[0],temp[1],temp[2]);

	CString strColRef = GetString(strEntry,strDefault);
	sscanf(strColRef,"RGB(%d,%d,%d)", temp, temp+1, temp+2);

	return RGB(temp[0],temp[1],temp[2]);
}

void CIni::WriteString(CString strEntry,CString strBuffer, LPCSTR strSection/* = NULL*/)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	//WritePrivateProfileString(m_strSection,strEntry,str,m_strFileName);

	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);


}
void CIni::WriteDouble(CString strEntry,double f, LPCSTR strSection/*= NULL*/)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("%g",f);
//	WritePrivateProfileString(m_strSection,strEntry,strBuffer,m_strFileName);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);


}
void CIni::WriteFloat(CString strEntry,float f, LPCSTR strSection/* = NULL*/)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("%g",f);
//	WritePrivateProfileString(m_strSection,strEntry,strBuffer,m_strFileName);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);



}
void CIni::WriteInt(CString strEntry,int n, LPCSTR strSection/* = NULL*/)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("%d",n);
//	WritePrivateProfileString(m_strSection,strEntry,strBuffer,m_strFileName);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);




}
void CIni::WriteDWORD(CString strEntry,DWORD n, LPCSTR strSection/* = NULL*/)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("%d",n);
//	WritePrivateProfileString(m_strSection,strEntry,strBuffer,m_strFileName);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);



}
void CIni::WriteBool(CString strEntry,BOOL b, LPCSTR strSection/* = NULL*/)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("%d",b);
//	WritePrivateProfileString(m_strSection, strEntry, strBuffer, m_strFileName);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);



}
void CIni::WritePoint(CString strEntry,CPoint pt, LPCSTR strSection)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("(%d,%d)",pt.x,pt.y);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);
}
void CIni::WriteRect(CString strEntry,CRect rect, LPCSTR strSection)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("(%d,%d,%d,%d)",rect.top,rect.left,rect.bottom,rect.right);
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);
}
void CIni::WriteColRef(CString strEntry,COLORREF cr, LPCSTR strSection)
{
	if (strSection != NULL) 
		m_strSection = strSection;
	CString strBuffer;
	strBuffer.Format("RGB(%d,%d,%d)",GetRValue(cr), GetGValue(cr), GetBValue(cr));
	if(m_bFromRegistry)
		Write_R(m_strAppName,m_strSection,strEntry,strBuffer);
	else 
		Write(m_strFileName,m_strSection,strEntry,strBuffer);
}
char* CIni::GetLPCSTR(CString strEntry, LPCSTR strSection, LPCSTR strDefault)
{
	// evtl Section neu setzen
	if (strSection != NULL)
		m_strSection = strSection;

	CString temp;
	if(!m_bFromRegistry) {
		if (strDefault == NULL)
			temp = Read(m_strFileName,m_strSection,strEntry,CString());
		else
			temp = Read(m_strFileName,m_strSection,strEntry,strDefault);
	}
	else {
		if (strDefault == NULL)
			temp = Read_R(m_strAppName,m_strSection,strEntry,CString());
		else
			temp = Read_R(m_strAppName,m_strSection,strEntry,strDefault);
	}

	return (char*)memcpy(m_chBuffer,(LPCTSTR)temp,temp.GetLength()+1);// '+1' damit die Null am Ende mit kopiert wird
}
void CIni::SerGetString( BOOL bGet,CString & str,CString strEntry,LPCSTR strSection,LPCSTR strDefault)
{
	if (bGet)
		str = GetString(strEntry,strDefault/*=NULL*/,strSection/* = NULL*/);
	else
		WriteString(strEntry,str, strSection/* = NULL*/);
}
void CIni::SerGetDouble( BOOL bGet,double& f, CString strEntry,LPCSTR strSection/* = NULL*/,double fDefault/* = 0.0*/)
{
	if (bGet)
		f = GetDouble(strEntry,fDefault/*=NULL*/,strSection/* = NULL*/);
	else
		WriteDouble(strEntry,f, strSection/* = NULL*/);
}
void CIni::SerGetFloat( BOOL bGet,float & f, CString strEntry, LPCSTR strSection/* = NULL*/,float fDefault/* = 0.0*/)
{
	if (bGet)
		f = GetFloat(strEntry,fDefault/*=NULL*/,strSection/* = NULL*/);
	else
		WriteFloat(strEntry,f, strSection/* = NULL*/);
}
void CIni::SerGetInt( BOOL bGet,int & n, CString strEntry,LPCSTR strSection/* = NULL*/,int nDefault/* = 0*/)
{
	if (bGet)
		n = GetInt(strEntry,nDefault/*=NULL*/,strSection/* = NULL*/);
	else
		WriteInt(strEntry,n, strSection/* = NULL*/);
}
void CIni::SerGetDWORD( BOOL bGet,DWORD & n, CString strEntry,LPCSTR strSection/* = NULL*/,DWORD nDefault/* = 0*/)
{
	if (bGet)
		n = (DWORD)GetInt(strEntry,nDefault/*=NULL*/,strSection/* = NULL*/);
	else
		WriteInt(strEntry,n, strSection/* = NULL*/);
}
void CIni::SerGetBool( BOOL bGet,BOOL & b, CString strEntry,LPCSTR strSection/* = NULL*/,BOOL bDefault/* = FALSE*/)
{
	if (bGet)
		b = GetBool(strEntry,bDefault/*=NULL*/,strSection/* = NULL*/);
	else
		WriteBool(strEntry,b, strSection/* = NULL*/);
}

void CIni::SerGetPoint( BOOL bGet,CPoint & pt, CString strEntry, LPCSTR strSection, CPoint ptDefault)
{
	if (bGet)
		pt = GetPoint(strEntry,ptDefault,strSection);
	else
		WritePoint(strEntry,pt, strSection);
}
void CIni::SerGetRect( BOOL bGet,CRect & rect, CString strEntry, LPCSTR strSection, CRect rectDefault)
{
	if (bGet)
		rect = GetRect(strEntry,rectDefault,strSection);
	else
		WriteRect(strEntry,rect, strSection);
}
void CIni::SerGetColRef( BOOL bGet,COLORREF & cr, CString strEntry, LPCSTR strSection, COLORREF crDefault)
{
	if (bGet)
		cr = GetColRef(strEntry,crDefault,strSection);
	else
		WriteColRef(strEntry,cr, strSection);
}
// Überladene Methoden //////////////////////////////////////////////////////////////////////////////////////////////////77
// Einfache Typen /////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIni::SerGet( BOOL bGet,CString & str, CString strEntry, LPCSTR strSection/*= NULL*/, LPCSTR strDefault/*= NULL*/)
{
	SerGetString(bGet,str,strEntry,strSection,strDefault);
}
void CIni::SerGet( BOOL bGet,double & f, CString strEntry, LPCSTR strSection/*= NULL*/, double fDefault/* = 0.0*/)
{
	SerGetDouble(bGet,f,strEntry,strSection,fDefault);
}
void CIni::SerGet( BOOL bGet,float & f, CString strEntry, LPCSTR strSection/*= NULL*/, float fDefault/* = 0.0*/)
{
	SerGetFloat(bGet,f,strEntry,strSection,fDefault);
}
void CIni::SerGet( BOOL bGet,int & n, CString strEntry, LPCSTR strSection/*= NULL*/, int nDefault/* = 0*/)
{
	SerGetInt(bGet,n,strEntry,strSection,nDefault);
}
void CIni::SerGet( BOOL bGet,short & n, CString strEntry, LPCSTR strSection/*= NULL*/, int nDefault/* = 0*/)
{
	int nTemp = n;
	SerGetInt(bGet,nTemp,strEntry,strSection,nDefault);
	n = (short)nTemp;
}
void CIni::SerGet( BOOL bGet,DWORD & n, CString strEntry, LPCSTR strSection/*= NULL*/, DWORD nDefault/* = 0*/)
{
	SerGetDWORD(bGet,n,strEntry,strSection,nDefault);
}
void CIni::SerGet( BOOL bGet,WORD & n, CString strEntry, LPCSTR strSection/*= NULL*/, DWORD nDefault/* = 0*/)
{
	DWORD dwTemp = n;
	SerGetDWORD(bGet,dwTemp,strEntry,strSection,nDefault);
	n = (WORD)dwTemp;
}
//	void		SerGet(	BOOL bGet,BOOL		& b,	CString strEntry,	LPCSTR strSection = NULL,	BOOL bDefault = FALSE);
void CIni::SerGet( BOOL bGet,CPoint & pt, CString strEntry, LPCSTR strSection/*= NULL*/, CPoint ptDefault/* = CPoint(0,0)*/)
{
	SerGetPoint(bGet,pt,strEntry,strSection,ptDefault);
}
void CIni::SerGet( BOOL bGet,CRect & rect, CString strEntry, LPCSTR strSection/*= NULL*/, CRect rectDefault/* = CRect(0,0,0,0)*/)
{
	SerGetRect(bGet,rect,strEntry,strSection,rectDefault);
}
//	void		SerGet(	BOOL bGet,COLORREF	& cr,	CString strEntry,	LPCSTR strSection = NULL,	COLORREF crDefault = RGB(128,128,128));

// Überladene Methoden ////////////////////////////////////////////////////////////////////////////////////////////
// ARRAYS /////////////////////////////////////////////////////////////////////////////////////////////////////////
// Entries werden durch Unterstrich + Index ergenzt////////////////////////////////////////////////////////////////
void CIni::SerGet( BOOL bGet,CString * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, LPCSTR Default/*=NULL*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,double * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, double Default/* = 0.0*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,float * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, float Default/* = 0.0*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,int * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, int Default/* = 0*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,short * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, int Default/* = 0*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,DWORD * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, DWORD Default/* = 0*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,WORD * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, DWORD Default/* = 0*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,CPoint * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, CPoint Default/* = CPoint(0,0)*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}
void CIni::SerGet( BOOL bGet,CRect * ar, int nCount, CString strEntry, LPCSTR strSection/*=NULL*/, CRect Default/* = CRect(0,0,0,0)*/)
{
	CString strBuffer;
	for ( int i=0 ; i<nCount ; i++)
	{
		strBuffer.Format("_%i",i);
		strBuffer = strEntry + strBuffer;
		SerGet(bGet,ar[i],strBuffer,strSection,Default);
	}
}

//////////////////////////////////////////////////////////////////////
// statische Methoden
//////////////////////////////////////////////////////////////////////
CString CIni::Read(CString const& strFileName, CString const& strSection, CString const& strEntry, CString const& strDefault)
{
	CString strReturn;
	GetPrivateProfileString(strSection,
		strEntry,
		strDefault,
		strReturn.GetBufferSetLength(MAX_INI_BUFFER),
		MAX_INI_BUFFER,
		strFileName);
	strReturn.ReleaseBuffer();
	return strReturn;
}

CString CIni::Read_R(CString const& strAppName, CString const& strSection, CString const& strEntry, CString const& strDefault)
{
	HKEY   hKey1; 
	CString cStrKey;
	DWORD  dwDisposition;
	DWORD Reserved;
	cStrKey = _T("Software\\") + strAppName + _T("\\")+ strSection;
	LONG lRetCode = RegCreateKeyEx ( HKEY_CURRENT_USER, 
								cStrKey, 
								0, 
								NULL, 
								REG_OPTION_NON_VOLATILE,
								KEY_READ, 
								NULL, 
								&hKey1, 
								&dwDisposition); 
	CString strReturn;
	DWORD dwSize = MAX_INI_BUFFER;
	DWORD dwType;
	CHAR   szData[MAX_INI_BUFFER];
	lRetCode = RegQueryValueEx(
					hKey1,
					strEntry,
					NULL,
					&dwType,
					(BYTE *)szData,
					&dwSize);

	if(lRetCode!=ERROR_SUCCESS) strReturn = "";
	else strReturn = szData;

	//strReturn.ReleaseBuffer();
	lRetCode = RegCloseKey( hKey1 );
	return strReturn;

}


void CIni::Write(CString const& strFileName, CString const& strSection, CString const& strEntry, CString const& strValue)
{
		WritePrivateProfileString(strSection,
			strEntry,
			strValue,
			strFileName);
}


void CIni::Write_R(CString const& strAppName, CString const& strSection, CString const& strEntry, CString & strValue)
{
		HKEY   hKey1; 
		CString cStrKey;
		DWORD  dwDisposition;
		DWORD Reserved;
		cStrKey = _T("Software\\") + strAppName + _T("\\")+ strSection;
		LONG lRetCode = RegCreateKeyEx ( HKEY_CURRENT_USER, 
									cStrKey, 
									0, 
									NULL, 
									REG_OPTION_NON_VOLATILE,
									KEY_WRITE, 
									NULL, 
									&hKey1, 
									&dwDisposition); 

		LPTSTR pStr = strValue.GetBuffer();
		RegSetValueEx(
						hKey1,
						strEntry,
						0,
						REG_SZ,
						(BYTE *)pStr,
						strValue.GetLength());

	lRetCode = RegCloseKey( hKey1 );

}

