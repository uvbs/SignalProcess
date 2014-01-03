
#include "stdafx.h"
#include "ConfigIni.h"



enum {MAX_LENGTH_SECTION_OR_NODE = 64000};


CConfigIni::CConfigIni(const CString& ini){LoadConfig(NULL);}
CConfigIni::~CConfigIni(){SaveConfig(NULL);}


bool CConfigIni::LoadSections(CStringArray& arrSections) const
{
	if(m_szIni.IsEmpty()) return false;
	TCHAR szTmpName[MAX_LENGTH_SECTION_OR_NODE+1] = {0};
	if(!GetPrivateProfileSectionNames(szTmpName, MAX_LENGTH_SECTION_OR_NODE, m_szIni)) return false;
	arrSections.RemoveAll();
	int i = 0;
	for(;;)
	{
		if(szTmpName[i] == 0) break;
		arrSections.Add(szTmpName + i);
		i += _tcslen(szTmpName + i) + 1;
	}
	return true;
}
bool CConfigIni::LoadInstruSections(CStringArray& arrSections) const
{
	CStringArray arr;
	if(!LoadSections(arr)) return false;
	arrSections.RemoveAll();

	for(int i=0; i<arr.GetCount(); i++)
	{
		CString Name = arr.GetAt(i);
		if(Name.IsEmpty() || Name == CConfigIni::l_szIniKey_Sys) continue;
		arrSections.Add(Name);
	}
	return true;
}
bool CConfigIni::LoadString(const CString& strSection, const CString& strItem, CString& value) const
{
	if(m_szIni.IsEmpty()) return false;
	TCHAR szTmpName[MAX_LENGTH_SECTION_OR_NODE+1] = {0};
	if(!GetPrivateProfileString(strSection, strItem, NULL, szTmpName, MAX_LENGTH_SECTION_OR_NODE, m_szIni)) return false;
	value = szTmpName;
	return true;
}
bool CConfigIni::LoadPath(const CString& strSection, const CString& strItem, CString& value) const
{
	CString v;
	if(!LoadString(strSection, strItem, v)) return false;
	value = m_szPath;
	value += v;
	PathCanonicalize(v.GetBuffer(MAX_LENGTH_SECTION_OR_NODE), value.GetBuffer());
	v.ReleaseBuffer();
	value.ReleaseBuffer();
	value = v;
	return true;
}
bool CConfigIni::LoadInt(const CString& strSection, const CString& strItem, int& value) const
{
	CString v;
	if(!LoadString(strSection, strItem, v)) return false;
	value = _ttoi(v);
	return true;
}
bool CConfigIni::SaveString(const CString& strSection, const CString& strItem, const CString& value) const
{
	return WritePrivateProfileString(strSection, strItem, value, m_szIni) != 0;
}
bool CConfigIni::SavePath(const CString& strSection, const CString& strItem, const CString& value) const
{
	ASSERT(false);//路径只能从配置文件读取，不能从程序中配置
	return false;/*
	CString t(value);
	t.Replace(m_szPath, _T(""));
	return SaveString(strSection, strItem, t);//*/
}
bool CConfigIni::SaveInt(const CString& strSection, const CString& strItem, int value) const
{
	CString t;
	t.Format(_T("%d"), value);
	return SaveString(strSection, strItem, t);
}
bool CConfigIni::SaveSystemName(const CString& strSystem) const
{
	return SaveString(l_szIniKey_Sys, l_szIniItem_Sys, strSystem);
}
bool CConfigIni::LoadSystem(const CString& systemName, CConfigIni::structSystem& system)
{
	//路径
	LoadPath(systemName, l_szIniItem_PathData, system.m_szPathData);
	LoadPath(systemName, l_szIniItem_PathTask, system.m_szPathTask);
	LoadPath(systemName, l_szIniItem_PathSample, system.m_szPathSample);
	LoadPath(systemName, l_szIniItem_PathCompound, system.m_szPathCompound);
	LoadPath(systemName, l_szIniItem_PathMethod, system.m_szPathMethod);
	LoadPath(systemName, l_szIniItem_PathLog, system.m_szPathLog);
	//参数
	LoadString(systemName, l_szIniItem_DefaultMethod, system.m_szDefaultMethod);
	LoadString(systemName, l_szIniItem_Inject, system.m_szInject);
	//	LoadString(systemName, _T("SCParam"), system.m_szSCParam);
	//	LoadString(systemName, _T("SysSettings"), system.m_szSysSettings);
	LoadString(systemName, l_szIniItem_Instrument, system.m_szInstrument);
	//硬件
	LoadInt(systemName, l_szIniItem_RGACOM, system.m_iRGACOM);
	LoadInt(systemName, l_szIniItem_ValveCOM, system.m_iValveCOM);
	LoadInt(systemName, l_szIniItem_PumpCOM, system.m_iPumpCOM);

	LoadInt(systemName, l_szIniItem_NextSampleID, system.m_iNextSampleID);

	LoadString(systemName, l_szIniItem_WindowName, system.m_szWindowName);
	LoadString(systemName, l_szIniItem_ProgramName, system.m_szProgramName);
	LoadString(systemName, l_szIniItem_MessageName, system.m_szMessageName);
	LoadString(systemName, l_szIniItem_SignalName, system.m_szSignalName);
	LoadString(systemName, l_szIniItem_XSLFolder, system.m_szXSLFolder);
	if(system.m_szXSLFolder.GetLength())
	{
		TCHAR c = system.m_szXSLFolder.GetAt(system.m_szXSLFolder.GetLength() - 1);
		if(c == '/' || c == '\\') system.m_szXSLFolder.Delete(system.m_szXSLFolder.GetLength() - 1);	
	}

	return true;
}
bool CConfigIni::LoadConfig(LPCTSTR strIni)
{
	if(strIni == NULL)
	{
		TCHAR l_szIniPath[MAX_PATH] = {0};
		GetModuleFileName(AfxGetInstanceHandle(), l_szIniPath, MAX_PATH);
		PathRemoveFileSpec(l_szIniPath);
		PathAddBackslash(l_szIniPath);
		m_szPath = l_szIniPath;
		StrCat(l_szIniPath, l_szIniFilename);
		m_szIni = l_szIniPath;
	}
	else m_szIni = strIni;

	//节点名
	LoadString(l_szIniKey_Sys, l_szIniItem_Sys, m_szName);

	return LoadCurrentSystem();
}
bool CConfigIni::SaveConfig(LPCTSTR strIni) const
{
	SaveSystemName(m_szName);
	return SaveSystem(m_szName, m_System); 
}