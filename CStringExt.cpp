
#include "stdafx.h"
#include "CStringExt.h"

/////////////////////////////////////////////////////////////////////
CStringExt::CStringExt():m_bNullNext(false),m_pPositions(NULL),m_lMoveIndex(-1)
{
}
CStringExt::CStringExt(LPCTSTR srcString,TCHAR splitChar)
:m_srcString(srcString),m_splitString(splitChar),m_bNullNext(false),m_pPositions(NULL),m_lMoveIndex(-1)
{
}
CStringExt::CStringExt(LPCTSTR srcString,LPCTSTR splitString)
:m_srcString(srcString),m_splitString(splitString),m_bNullNext(false),m_pPositions(NULL),m_lMoveIndex(-1)
{
}
CStringExt::~CStringExt()
{
	if(m_pPositions){delete []m_pPositions;m_pPositions=NULL;}
}

bool CStringExt::File_Exist(LPCTSTR filePathName)
{
	CString p(filePathName);
	p.Replace(_T('/'),_T('\\'));
	if(p.Right(1)==_T('\\')) p.Delete(p.GetLength()-1);
	return GetFileAttributes(p)!=0xFFFFFFFF;//存在

}
bool CStringExt::GetAppPath(CString &path)//不包括最后的'\\'
{
	CString sPath;
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	sPath.ReleaseBuffer();
	int i=sPath.GetLength()-1;
	while(i>0)
	{
		if(sPath.GetAt(i)=='/' || sPath.GetAt(i)=='\\') break;
		i--;
	}
	if(i<0) return false;
	if(i==0) path="./";
	else path=sPath.Left(i);
	return true;
}

void CStringExt::ToTime(LPCTSTR txt, CTime& time)
//将字符串转化为时间，字符串格式为2009-12-31 23:59:59
{
	CString v1,v2;
	if(!CStringExt::Split(txt,_T(" "),2,&v1,&v2))
	{
		v1=txt;
		if(v1.Find(_T('-'))>-1)//只有日期
		{
			v2=_T("");
		}
		else if(v1.Find(_T(':'))>-1)//只有时间
		{
			v2=v1;
			v1=_T("");
		}
		else return;//错误的格式
	}
	CTime t=CTime::GetCurrentTime();
	int nYear=t.GetYear(), nMonth=t.GetMonth(), nDay=t.GetDay(), nHour=t.GetHour(), nMin=t.GetMinute(), nSec=t.GetSecond();
	CString v3[3];
	if(CStringExt::Split(v1,_T("-"),3,v3))
	{
		nYear=_tstoi(v3[0]);
		nMonth=_tstoi(v3[1]);
		nDay=_tstoi(v3[2]);
	}
	if(CStringExt::Split(v2,_T(":"),3,v3))
	{
		nHour=_tstoi(v3[0]);
		nMin=_tstoi(v3[1]);
		nSec=_tstoi(v3[2]);
	}
	time=CTime(nYear, nMonth, nDay, nHour, nMin, nSec);
}

bool CStringExt::IsLong(LPCTSTR txt)//2147483648
{
	if((txt[0]>'9' || txt[0]<'0') && txt[0]!='-') return false;
	int len=_tcslen(txt);
	if(len<1) return false;
	if(txt[0]=='-'){if(len>11) return false;}
	else if(len>10) return false;
	for(int i=1;i<len;i++) if(txt[i]>'9' || txt[i]<'0') return false;
	if(txt[0]=='-') return _tcscmp(txt,_T("-2147483648"))<=0 || len<11;
	else return _tcscmp(txt,_T("2147483647"))<=0 || len<10;
}

bool CStringExt::IsDWORD(LPCTSTR txt)//4294967296
{
	int len=_tcslen(txt);
	if(len<1 || len>10) return false;
	for(int i=0;i<len;i++) if(txt[i]>'9' || txt[i]<'0') return false;
	return _tcscmp(txt,_T("4294967296"))<=0 || len<19;
}
bool CStringExt::IsLongLong(LPCTSTR txt)//–9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
{
	if((txt[0]>'9' || txt[0]<'0') && txt[0]!='-') return false;
	int len=_tcslen(txt);
	if(len<1) return false;
	if(txt[0]=='-'){if(len>20) return false;}
	else if(len>19) return false;
	for(int i=1;i<len;i++) if(txt[i]>'9' || txt[i]<'0') return false;
	if(txt[0]=='-') return _tcscmp(txt,_T("-9223372036854775808"))<=0 || len<20;
	else return _tcscmp(txt,_T("9223372036854775807"))<=0 || len<19;
}
bool CStringExt::IsDouble(LPCTSTR txt)//没有比较数值范围
{//1.7E +/- 308 (15 digits)
	if((txt[0]>'9' || txt[0]<'0') && txt[0]!='-') return false;
	bool bDot=false;
	int len=_tcslen(txt);
	for(int i=1;i<len;i++)
	{
		if(txt[i]>'9' || txt[i]<'0')
		{
			if(txt[i]!='.') return false;
			if(bDot) return false;
			else bDot=true;
		}
	}
	return len<309;
}

bool CStringExt::Split(LPCTSTR srcString,LPCTSTR splitString,BYTE nNum,...)
{
	CStringExt ext(srcString,splitString);
	va_list argList;
	va_start(argList,nNum);
	ext.BeginEnum();
	bool bRet=true;
	CString *pS=NULL;
	CString value;
	for(BYTE n=0;n<nNum;n++)
	{
		pS=va_arg(argList,CString*);
		//if(!pS || !ext.GetNext(*pS))
		if(!pS) pS=&value;
		if(!ext.GetNext(*pS))
		{
			bRet=false;
			break;
		}
	}
	va_end(argList);
	return bRet;
}
bool CStringExt::Split(LPCTSTR srcString,LPCTSTR splitString,BYTE nNum,CString*values)
{
	ASSERT(nNum && values);
	if(nNum==0 || values==NULL) return false;
	CStringExt ext(srcString,splitString);
	ext.BeginEnum();
	for(BYTE n=0;n<nNum;n++)
	{
		if(!ext.GetNext(values[n])) return false;
	}
	return true;
}
int CStringExt::LastIndexOf(CString in,TCHAR c)
{
	CString sc(c);
	for(int i=in.GetLength()-1;i>-1;i--)
	{
		if(CString(in.GetAt(i))==sc) return i;
	}
	return -1;
}
CString CStringExt::GetFileExt(CString fn,LPCTSTR notFound)
{
	int i=LastIndexOf(fn,_T('.'));
	if(i<0) return notFound;
	CString ret(fn);
	return ret.Right(ret.GetLength()-i-1);
}
void CStringExt::EnumAllSubs(LPCTSTR folder,LPCTSTR types,LPCTSTR typeSep,
							 void (*cb)(LPCTSTR path,LPCTSTR fileName,LPCTSTR fileType,DWORD extraData),bool bSubFilesOnly,DWORD extraData)
{
	CString path(folder);
	CFileFind find;
	BOOL bWorking =find.FindFile(path+_T("\\*.*"));//查找文件
	if(!bWorking){find.Close();return;}
	while (bWorking)
	{
		bWorking = find.FindNextFile();
		if(find.IsDots()) continue;
		if(find.IsDirectory())
		{
			if(bSubFilesOnly) continue;
			else EnumAllSubs(path+_T("\\")+find.GetFileName(),types,typeSep,cb,bSubFilesOnly,extraData);
		}
		else
		{
			CString strTypes(types);
			strTypes=typeSep+strTypes+typeSep;
			strTypes.MakeLower();
			CString fn(find.GetFileName());
			CString ext=GetFileExt(fn,_T(""));
			bool bEnumIt=false;
			if(strTypes.Find(CString(typeSep)+_T("*")+typeSep)>-1 || strTypes.Find(CString(typeSep)+_T("*.*")+typeSep)>-1)
				bEnumIt=true;//*.*
			else if(ext.IsEmpty() &&
				(strTypes.Find(CString(typeSep)+_T("[empty]")+typeSep)>-1 || strTypes.Find(CString(typeSep)+_T("[null]")+typeSep)>-1))
				bEnumIt=true;//无后缀或后缀名为空
			else if(strTypes.Find(typeSep+ext+typeSep)>-1 || strTypes.Find(typeSep+(_T("*.")+ext)+typeSep)>-1)
				bEnumIt=true;//指定后缀
			if(bEnumIt) (*cb)(folder,find.GetFileTitle(),ext,extraData);
		}
	}
	find.Close();
}
void CStringExt::EnumAllSubs_Ex(LPCTSTR folder,LPCTSTR types,LPCTSTR typeSep,
								DWORD (*cbFolder)(LPCTSTR path,LPCTSTR folderName,DWORD extraData),
								void (*cbFolderEnd)(LPCTSTR path,LPCTSTR folderName,DWORD extraData),
								void (*cbFile)(LPCTSTR path,LPCTSTR fileName,DWORD extraData),bool bSubFilesOnly,DWORD extraData)
{
	CString path(folder);
	if(path.Right(1) == _T("\\") || path.Right(1) == _T("/")) path.Delete(path.GetLength()-1);
	CFileFind find;
	BOOL bWorking =find.FindFile(path+_T("\\*.*"));//查找文件
	if(!bWorking){find.Close();return;}
	if(!bSubFilesOnly)
	{
		while (bWorking)
		{
			bWorking = find.FindNextFile();
			if(find.IsDots()) continue;
			if(find.IsDirectory())
			{
				DWORD ed = extraData;
				if(cbFolder) ed = (*cbFolder)(path,find.GetFileName(),extraData);
				EnumAllSubs_Ex(path+_T("\\")+find.GetFileName(),types,typeSep,cbFolder,cbFolderEnd,cbFile,bSubFilesOnly,ed);
				//if(cbFolderEnd)  (*cbFolderEnd)(path,find.GetFileName(),extraData);
				if(cbFolderEnd)  (*cbFolderEnd)(path,find.GetFileName(),ed);
			}
		}
	}

	if(cbFile)//需枚举文件
	{
		bWorking =find.FindFile(path+_T("\\*.*"));//查找文件
		while (bWorking)
		{
			bWorking = find.FindNextFile();
			if(find.IsDots()) continue;
			if(find.IsDirectory()) continue;

			bool bEnumIt=false;
			CString fn(find.GetFileName());
			CStringExt ext(types,typeSep);
			ext.BeginEnum();
			CString type;
			while(ext.GetNext(type))
			{
				type.MakeLower();
				if(type == _T("*") || type == _T("*.*"))
				{
					bEnumIt = true;
					break;
				}
				if(type == _T("[empty]") || type == _T("[null]"))
				{
					if(fn.Find(_T('.')) < 0)//无后缀
					{
						bEnumIt = true;
						break;
					}
				}
				if(type.GetLength() > fn.GetLength()) continue;
				if(type.CompareNoCase(fn.Right(type.GetLength())) == 0)
				{
					bEnumIt = true;
					break;
				}
			}
			if(bEnumIt) (*cbFile)(path,fn,extraData);
		}
	}

	find.Close();
}
CString CStringExt::m_stc_strEnumAllSubs_CB_FileList=_T("");
CString CStringExt::EnumAllSubs_FileList(LPCTSTR folder,LPCTSTR types,LPCTSTR typeSep,DWORD flag)
{
	m_stc_strEnumAllSubs_CB_FileList=_T("");
	EnumAllSubs(folder, types, typeSep, EnumAllSubs_CB_FileList, (flag & EASFL_SubFilesOnly)?true:false, flag);
	if(m_stc_strEnumAllSubs_CB_FileList.GetLength()) m_stc_strEnumAllSubs_CB_FileList.Delete(m_stc_strEnumAllSubs_CB_FileList.GetLength()-1);
	return m_stc_strEnumAllSubs_CB_FileList;
}

void CStringExt::EnumAllSubs_CB_FileList(LPCTSTR path,LPCTSTR fileName,LPCTSTR fileType,DWORD extraData)
{
	CString fn(fileName);
	if(!(extraData & EASFL_NoExt)) fn=fn+_T(".")+fileType;
	CString sep(_T(';'));
	if(extraData & EASFL_SemicolonSep) sep=_T(';');
	else if(extraData & EASFL_CommaSep) sep=_T(',');
	if(extraData & EASFL_ReturnPath) m_stc_strEnumAllSubs_CB_FileList=m_stc_strEnumAllSubs_CB_FileList+path+_T("\\")+fileName+sep;
	else m_stc_strEnumAllSubs_CB_FileList=m_stc_strEnumAllSubs_CB_FileList+fileName+sep;
}

//
void CStringExt::Clone(const CStringExt& ext)
{
	m_srcString=ext.m_srcString;
	m_splitString=ext.m_splitString;
	m_curPos=ext.m_curPos;
	m_count=ext.m_count;
	m_curCount=ext.m_curCount;
	m_bNullNext=ext.m_bNullNext;//当枚举时，若当前值为空，则枚举下一个
	if(m_pPositions){delete []m_pPositions;m_pPositions=NULL;}
	if(ext.m_pPositions)
	{
		m_pPositions=new DWORD[m_count];
		memcpy(m_pPositions,ext.m_pPositions,sizeof(DWORD)*m_count);
	}
	m_lMoveIndex=ext.m_lMoveIndex;
}
//
void CStringExt::SetSRCString(LPCTSTR srcString)
{
	ASSERT(srcString);
	if(srcString) m_srcString=srcString;
}
void CStringExt::SetSplitString(LPCTSTR splitString)
{
	ASSERT(splitString && _tcslen(splitString)>0);
	if(splitString) m_splitString=splitString;
}
void CStringExt::SetSplitString(TCHAR splitChar)
{
	ASSERT(splitChar);
	m_splitString=splitChar;
}
DWORD CStringExt::GetCount()
{
	Count();
	return m_count;
}
////////
void CStringExt::SetNullNext(bool bNullNext)
{
	m_bNullNext=bNullNext;
}
void CStringExt::BeginEnum()//尽量使用循环遍历
{
	m_curPos=0;
	m_curCount=0;
	Count();
}
bool CStringExt::GetNext(CString&value)
{
	if(m_curCount>=m_count) return false;
	m_curCount++;
	int r=m_srcString.GetLength();
	int i=m_srcString.Find(m_splitString,m_curPos);
	if(i>-1) r=i;
	if((DWORD)r==m_curPos) value=_T("");
	else value=m_srcString.Mid(m_curPos,r-m_curPos);
	m_curPos=i+m_splitString.GetLength();
	if(!m_bNullNext || value.GetLength()>0) return true;
	else return GetNext(value);
}
//
void CStringExt::Positions()
{
}
void CStringExt::BeginMove()
{
	Count();
	Positions();
	m_lMoveIndex=0;
}
void CStringExt::MoveNext()
{
	m_lMoveIndex++;
}
void CStringExt::MovePrev()
{
	m_lMoveIndex--;
}
void CStringExt::MoveFirst()
{
	m_lMoveIndex=0;
}
void CStringExt::MoveLast()
{
	m_lMoveIndex=m_count-1;
}
void CStringExt::Move(long lCount)
{
	m_lMoveIndex+=lCount;
}
bool CStringExt::GetCurrent(CString&value)
{
	if(IsBOF() || IsEOF()) return false;
	return GetEnum(m_lMoveIndex,value);
}
bool CStringExt::IsBOF() const
{
	return m_lMoveIndex<0;
}
bool CStringExt::IsEOF() const
{
	return m_lMoveIndex<0 || ((DWORD)m_lMoveIndex)>=m_count;
}
//for single support
bool CStringExt::GetEnum(DWORD dwIndex,CString&value)
{
	DWORD count=0;
	int i;
	int pos=0;
	while(count!=dwIndex)
	{
		i=m_srcString.Find(m_splitString,pos);
		if(i<0) return false;
		pos=i+m_splitString.GetLength();
		count++;
	}
	int r=m_srcString.GetLength();
	i=m_srcString.Find(m_splitString,pos);
	if(i>-1) r=i;
	if(r==pos) value="";
	else value=m_srcString.Mid(pos,r-pos);
	return true;
}
CString CStringExt::operator[](DWORD dwIndex)
{
	CString ret;
	if(GetEnum(dwIndex,ret)) return ret;
	return _T("CStringEx[] Operator Error:Access violation!");
}

void CStringExt::Count() 
{ //修改：加入换行符仅以LF结尾的换行符情况

	int pos = 0;
	m_count = 0;
	int i = -1;
	bool hasTry = false;

	while (true)
	{
		i = m_srcString.Find(m_splitString, pos);
		if( i < 0 ) {
			//if (!hasTry)
			//{
				//m_splitString = _T("\n");
			//	hasTry = !hasTry; 
			//	continue;
			//} else {
				break;
			//}
		}
		pos = i + m_splitString.GetLength();
		m_count++;
	}
	m_count++;
	
}

//////////////////////////////////////////////////////////////////////////////////

CSingleAccess::CSingleAccess(LPCTSTR signalName)
:m_hSignal(NULL)
{
	if(signalName) Setup(signalName);
}

CSingleAccess::~CSingleAccess()
{
	UnInit();
}

bool CSingleAccess::Setup(LPCTSTR signalName)
{
	if(signalName)
	{
#ifdef _DEBUG
		m_signalName=signalName;
#endif
	}
	else
	{
#ifdef _DEBUG
		m_signalName="";
#endif
		return false;
	}
	ASSERT(m_hSignal==NULL);
	if(OpenMutex(MUTEX_ALL_ACCESS,false,signalName))
	{
		TRACE("CSingleAccess无法构造对象,因为名称[%s]已经存在!\n",signalName);
		return false;
	}
	m_hSignal=CreateMutex(NULL,FALSE,signalName);
	if(m_hSignal) ReleaseMutex(m_hSignal);
	return m_hSignal!=NULL;
}
bool CSingleAccess::Open(LPCTSTR signalName)
{
	if(signalName)
	{
#ifdef _DEBUG
		m_signalName=signalName;
#endif
	}
	else
	{
#ifdef _DEBUG
		m_signalName="";
#endif
		return false;
	}
	ASSERT(m_hSignal==NULL);
	m_hSignal=OpenMutex(MUTEX_ALL_ACCESS,false,signalName);
	if(m_hSignal) ReleaseMutex(m_hSignal);
	return m_hSignal!=NULL;
}
void CSingleAccess::UnInit()
{
	if(m_hSignal)
	{
		CloseHandle(m_hSignal);
		m_hSignal=NULL;
	}
}
void CSingleAccess::Access(bool bUI)
//对于Mutex,wait函数会一直等到拥有它的更改权,
//并自动设置它的状态为wait函数所在的线程/进程所拥有
{
#ifdef _DEBUG
	TRACE("CSingleAccess请求访问[%s].\n",m_signalName);
#endif
	if(m_hSignal)
	{
		CWaitingSignal::Wait(m_hSignal,bUI,INFINITE);
	}
#ifdef _DEBUG
	TRACE("CSingleAccess开始访问[%s].\n",m_signalName);
#endif
}
void CSingleAccess::LeaveAccess()
{
#ifdef _DEBUG
	TRACE("CSingleAccess离开访问[%s].\n",m_signalName);
#endif
	if(m_hSignal) ReleaseMutex(m_hSignal);
}
//////////////////////////////////////////////////////////////////////////////////
bool CWaitingSignal::Wait(HANDLE handle,bool bUI,DWORD dwMilliseconds)
{
#ifdef _DEBUG
	TRACE("CWaitingSignal等待开始HANDLE[].\n");
#endif
	bool bRet=false;
	if(handle)
	{
		if(bUI)
		{
			HANDLE hThread[2];
			hThread[0]=handle;
			CTime start=CTime::GetCurrentTime();
			DWORD r;
			while(true)
			{
				MSG msg ; 
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
				{
					if (msg.message == WM_QUIT)
					{
						TRACE("static CWaitingSignal::Wait:收到消息WM_QUIT!\n");
						bRet=false;
						goto End;
					}
					DispatchMessage(&msg); 
				}
				//使用循环是为了保证消息队列中不会有遗漏不处理的消息
				r=MsgWaitForMultipleObjects(1,hThread,FALSE,dwMilliseconds,QS_ALLINPUT|QS_ALLPOSTMESSAGE);
				bRet=r==WAIT_OBJECT_0;
				if(r!=WAIT_OBJECT_0 + 1) break;
				if(dwMilliseconds!=INFINITE)
				{
					if((DWORD)(CTime::GetCurrentTime()-start).GetTotalSeconds()*1000>=dwMilliseconds)
					{
						bRet=false;
						break;
					}
				}
			}
		}
		else bRet=WaitForSingleObject(handle,dwMilliseconds)==WAIT_OBJECT_0;
	}
End:
#ifdef _DEBUG
	TRACE("CWaitingSignal等待结束HANDLE[].\n");
#endif

	return bRet;
}
CWaitingSignal::CWaitingSignal(LPCTSTR signalName,bool bInWaitProccess)
:m_hSignal(NULL)
{
	if(signalName) Setup(signalName,bInWaitProccess);
}

CWaitingSignal::~CWaitingSignal()
{
	UnInit();
}

bool CWaitingSignal::Setup(LPCTSTR signalName,bool bInWaitProccess)
{
	m_bInWaitProccess=bInWaitProccess;
	if(signalName) m_signalName=signalName;
	else return false;
	ASSERT(m_hSignal==NULL);
	m_hSignal=OpenEvent(EVENT_ALL_ACCESS,false,signalName);
	if(m_bInWaitProccess && m_hSignal)
	{
		m_hSignal=NULL;
		TRACE("CWaitingSignal无法构造对象,因为名称[%s]已经存在!\n",signalName);
		return false;
	}
	if(m_bInWaitProccess) m_hSignal=CreateEvent(NULL,FALSE,FALSE,signalName);
	return m_hSignal!=NULL;
}
void CWaitingSignal::UnInit()
{
	if(m_hSignal)
	{
		CloseHandle(m_hSignal);
		m_hSignal=NULL;
	}
}
bool CWaitingSignal::Open(LPCTSTR signalName)
{
	ASSERT(m_hSignal==NULL);
	if(signalName) m_signalName=signalName;
	else return false;
	m_hSignal=OpenEvent(EVENT_ALL_ACCESS,false,signalName);
	return m_hSignal!=NULL;
}
void CWaitingSignal::Reset()
{
	if(m_hSignal) ResetEvent(m_hSignal);
}
bool CWaitingSignal::Wait(bool bUI,DWORD dwMilliseconds)
{
#ifdef _DEBUG
	TRACE("CWaitingSignal等待开始[%s].\n",m_signalName);
#endif
	bool bRet=Wait(m_hSignal,bUI,dwMilliseconds);
#ifdef _DEBUG
	TRACE("CWaitingSignal等待结束[%s].\n",m_signalName);
#endif

	return bRet;
}
void CWaitingSignal::EndWait()
{
#ifdef _DEBUG
	TRACE("CWaitingSignal结束等待[%s].\n",m_signalName);
#endif
	if(m_hSignal) SetEvent(m_hSignal);
}