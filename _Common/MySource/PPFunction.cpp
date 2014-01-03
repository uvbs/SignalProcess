#include "StdAfx.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include "PPFunction.h"
//#include <Winerror.h>

XERCES_CPP_NAMESPACE_USE

extern "C" void  PASCAL EXPORT GetSimpleChar(const TCHAR * wstr,char * & str){
	DWORD dwNum = WideCharToMultiByte(CP_UTF8,NULL,wstr,-1,NULL,0,NULL,FALSE);

	char *psText;
	psText = new char[dwNum];
	WideCharToMultiByte (CP_UTF8,NULL,wstr,-1,psText,dwNum,NULL,FALSE);
	str=psText;
}

extern "C" void  PASCAL EXPORT GetWideChar(const std::string & resstr,wchar_t * & wstr){
	DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, resstr.c_str(), -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	MultiByteToWideChar (CP_UTF8, 0, resstr.c_str(), -1, pwText, dwNum);
	wstr=pwText;
}


std::string GetSimpleChar(const CString & wstr){
	DWORD dwNum = WideCharToMultiByte(CP_UTF8,NULL,wstr,-1,NULL,0,NULL,FALSE);
	char *psText;
	psText = new char[dwNum];
	WideCharToMultiByte (CP_UTF8,NULL,wstr,-1,psText,dwNum,NULL,FALSE);
	std::string str(psText);
	delete [] psText;
	return str;
}

CString GetWideChar(const std::string & resstr){
	DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, resstr.c_str(), -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	MultiByteToWideChar (CP_UTF8, 0, resstr.c_str(), -1, pwText, dwNum);
	CString wstr;
	wstr=pwText;
	delete [] pwText;
	return wstr;
}


extern "C" BOOL  PASCAL EXPORT IsWellFormXML(const TCHAR * xmlStr){
	try
	{
		_XML_INIT_; 
	}catch(const XMLException&)
	{
		return FALSE;
	}// Initialize Xerces infrastructure

	
		XercesDOMParser * m_Parser=new XercesDOMParser();
		
		m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
		m_Parser->setDoNamespaces( false );
		m_Parser->setDoSchema( false );
		m_Parser->setLoadExternalDTD( false );
		
		
		HandlerBase handler;
		m_Parser->setErrorHandler(&handler);
		
		
		char * _xml_Str=NULL;
		GetSimpleChar(xmlStr,_xml_Str);
		
		InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("CHECK_WELL_FORMAT"));
		if (NULL == pInputSource) {
			if(_xml_Str){delete _xml_Str; _xml_Str=NULL;}
			delete m_Parser;
			_XML_RELEASE_;
			return FALSE; //Memory lack or unknown error
		}
		BOOL retVal=TRUE;
		try{
			m_Parser->parse(*pInputSource);
		}catch (...)
		{
			retVal=FALSE;
		}

		if(_xml_Str){delete _xml_Str; _xml_Str=NULL;}

		delete m_Parser;
		delete pInputSource;
		_XML_RELEASE_;
		return retVal;

}



extern "C" void PASCAL SplitCommaString(CString cStrSource, CStringArray &cArrayStr, TCHAR chSeparator, BOOL bIsXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	//**---- 定义 ----**
	int ipos = 0;    

	//如果是XML，先把壳剥掉
	if(bIsXML) {
		ipos = cStrSource.Find(_T(">")); 
		if(ipos >= 0)
		{   
			++ipos;
			//去掉前面可能的空格＼Tab＼换行符号
			//\n, 换行符(ASCII 10). \r, 回车(ASCII 13). \t, Tab (ASCII 9). \v, 垂直制表符(ASCII 11). 
			for(int i=ipos;i<cStrSource.GetLength();++i) {
				if((int)cStrSource[i]>=9&&(int)cStrSource[i]<=13) {
					++ipos;
					continue;
				}
				else break;
			}
			cStrSource = cStrSource.Right(cStrSource.GetLength()-ipos);
		}

		ipos = cStrSource.Find(_T("<")); 
		if(ipos >= 0)
		{   
			//去掉后面可能的空格＼Tab＼换行符号
			//\n, 换行符(ASCII 10). \r, 回车(ASCII 13). \t, Tab (ASCII 9). \v, 垂直制表符(ASCII 11). 
			for(int i=ipos-1;i>=0;--i) {
				if((int)cStrSource[i]>=9&&(int)cStrSource[i]<=13) {
					--ipos;
					continue;
				}
				else break;
			}

			cStrSource = cStrSource.Left(ipos);
		}
	}



	while(TRUE)
	{	
		ipos = cStrSource.Find(chSeparator);   

		if(ipos >= 0)
		{   
			cArrayStr.Add(cStrSource.Left(ipos));   
			cStrSource = cStrSource.Mid(ipos+1);   
		}
		else if (cStrSource.GetLength() > 0)
		{   
			cArrayStr.Add(cStrSource);   
			break;
		}
		else
		{
			break;
		}
	}

}


extern "C" void PASCAL SplitFile2Part(CString cStrSource, CString &cStrPath, CString &cStrFileTitle, CString &cStrPostfix)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//
	//首先看是不是目录
	//
	CFileFind finder;

	//如果找到了
	if(finder.FindFile(cStrSource)) {
		finder.FindNextFile();
		//如果是目录
		if(finder.IsDirectory()) {
			cStrPath = cStrSource;
			cStrFileTitle = _T("");
			cStrPostfix = _T("");
			return;
		}
	}
	//就算没找到，也要看看是不是目录
	else {
		if(cStrSource.GetAt(cStrSource.GetLength()-1)==_T('\\')||cStrSource.GetAt(cStrSource.GetLength()-1)==_T(':')){
			cStrPath = cStrSource;
			cStrFileTitle = _T("");
			cStrPostfix = _T("");
			return;
		}
	}

	//彻底不是目录了再往下做


	CFile file;
	file.SetFilePath(cStrSource);
	CString cStrFileFullName = file.GetFileName();
	int nPos = -1;
	int nRet;
	//找到最后一个 '.'
	for(;;) {
		nRet = cStrFileFullName.Find(_T('.'), nPos+1);
		if(nRet==-1) break;
		else nPos = nRet;
	}
	if(nPos == -1) {//根本就没有 '.'
		cStrPostfix = _T("");
		cStrFileTitle = cStrFileFullName;
	}
	else { 
		cStrPostfix = cStrFileFullName.Right(cStrFileFullName.GetLength()-nPos-1);
		cStrFileTitle = cStrFileFullName.Left(nPos);
	}
	cStrPath = cStrSource.Left(cStrSource.Find(cStrFileFullName));

}




//
//返回值:
//KMERROR_USER_CANCEL--用户取消
//
//

extern "C" int PASCAL FileAndFolderOperate(CString cStrFrom, CString cStrTo, UINT nType, FILEOP_FLAGS flags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR *szFrom = NULL;
	TCHAR *szTo = NULL;

	if(cStrFrom.GetLength()>0) {
		szFrom= new TCHAR[cStrFrom.GetLength()+2];
		_tcsncpy_s(szFrom, cStrFrom.GetLength()+1, cStrFrom, cStrFrom.GetLength());
		szFrom[cStrFrom.GetLength()+1] = _T('\0');
	}
	if(cStrTo.GetLength()>0) {
		szTo = new TCHAR[cStrTo.GetLength()+2];;
		_tcsncpy_s(szTo, cStrTo.GetLength()+1, cStrTo, cStrTo.GetLength());
		szTo[cStrTo.GetLength()+1] = _T('\0');
	}


	SHFILEOPSTRUCT   FileOp;   

	FileOp.hwnd=NULL;   
	FileOp.wFunc=nType;   
	FileOp.pFrom= szFrom;
	FileOp.pTo = szTo;
	FileOp.fFlags=flags;//FOF_SIMPLEPROGRESS;//|FOF_RENAMEONCOLLISION;//FOF_NOCONFIRMATION|FOF_SILENT|FOF_ALLOWUNDO;   
	FileOp.fAnyOperationsAborted=FALSE;   
	FileOp.hNameMappings   =NULL;
	FileOp.lpszProgressTitle   =_T("");   
	int   rval=SHFileOperation(&FileOp);

	delete [] szTo;
	delete [] szFrom;

	if(rval==0&&FileOp.fAnyOperationsAborted==TRUE){                   
		//AfxMessageBox(_T("入库操作被非法取消！请检查原目录（文件）的完整性！"));
		return KMERROR_USER_CANCEL;
	}
	if(rval!=0){
		if(rval==ERROR_INVALID_NAME)
			return KMERROR_FILE_INVALIDNAME;
		else
			return KMERROR_FILE_OPERATEFAILURE;
	}

	return KMERROR_SUCCESS;
}


//将字节数转化为 "123,567 KB"的显示方式
extern "C" void PASCAL FormatTheSizeOfFile(ULONGLONG ulSize, CString &cStrDisp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(ulSize>1024) {
		cStrDisp.Format(_T("%llu"), ulSize/1024);
		int nOriSize = cStrDisp.GetLength();
		int nCommaPos = cStrDisp.GetLength();
		for(;;) {
			nCommaPos -= 3;
			if(nCommaPos>0)
				cStrDisp.Insert(nCommaPos,_T(','));
			else break;
		}
		cStrDisp += _T(" KB");
	}
	else {
		cStrDisp.Format(_T("%u Byte"), ulSize);
	}
}

//是否在系统目录中
extern "C" BOOL PASCAL IsInSystemFolder(CString cStrTheFileName, CString &cStrFolderNameText, int &nCSIDL, BOOL &bEqual)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	if(cStrTheFileName.GetLength()<2) {
		bEqual = FALSE;
		return FALSE;
	}

	
	CStringArray arraySysFolder;
	CStringArray arraySysFolderName;
	CWordArray	 arrayCSIDL;
	TCHAR szPath[MAX_PATH];
	CString cStrPathWithEnd;

	//System32
	szPath[0]=TCHAR(0);
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_SYSTEM, 0, SHGFP_TYPE_CURRENT,szPath))&&_tcslen(szPath)>2) {
		cStrPathWithEnd = szPath;
		if(szPath[_tcslen(szPath)-1]!=_T('\\'))
			cStrPathWithEnd += _T('\\');
		arraySysFolder.Add(cStrPathWithEnd);
		arraySysFolderName.Add(_T("系统文件夹（System32）"));
		arrayCSIDL.Add(CSIDL_SYSTEM);
	}
	//Windows 目录
	szPath[0]=TCHAR(0);
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_WINDOWS , 0, SHGFP_TYPE_CURRENT,szPath))&&_tcslen(szPath)>2) {
		cStrPathWithEnd = szPath;
		if(szPath[_tcslen(szPath)-1]!=_T('\\'))
			cStrPathWithEnd += _T('\\');
		arraySysFolder.Add(cStrPathWithEnd);
		arraySysFolderName.Add(_T("Windows系统文件夹"));
		arrayCSIDL.Add(CSIDL_WINDOWS);
	}
	//回收站
	szPath[0]=TCHAR(0);
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_BITBUCKET, 0, SHGFP_TYPE_CURRENT,szPath))&&_tcslen(szPath)>2) {
		cStrPathWithEnd = szPath;
		if(szPath[_tcslen(szPath)-1]!=_T('\\'))
			cStrPathWithEnd += _T('\\');
		arraySysFolder.Add(cStrPathWithEnd);
		arraySysFolderName.Add(_T("回收站文件夹"));
		arrayCSIDL.Add(CSIDL_BITBUCKET);
	}
	//Program files
	szPath[0]=TCHAR(0);
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, 0, SHGFP_TYPE_CURRENT,szPath))&&_tcslen(szPath)>2) {
		cStrPathWithEnd = szPath;
		if(szPath[_tcslen(szPath)-1]!=_T('\\'))
			cStrPathWithEnd += _T('\\');
		arraySysFolder.Add(cStrPathWithEnd);
		arraySysFolderName.Add(_T("程序文件夹（Program files）"));
		arrayCSIDL.Add(CSIDL_PROGRAM_FILES);
	}

	cStrPathWithEnd = cStrTheFileName;
	if(cStrTheFileName[cStrTheFileName.GetLength()-1]!=_T('\\'))
		cStrPathWithEnd += _T('\\');

	for(int i=0; i<arraySysFolder.GetCount(); ++i) {
		if(cStrPathWithEnd.MakeUpper().Find(arraySysFolder[i].MakeUpper())==0) {
			cStrFolderNameText=arraySysFolderName[i];
			nCSIDL = arrayCSIDL[i];
			//是目录本身，而不是子目录
			if(arraySysFolder[i].MakeUpper()==cStrPathWithEnd.MakeUpper())
				bEqual = TRUE;
			else
				bEqual = FALSE;
			return TRUE;
		}
	}

	//保证不相等，因为不在里面肯定不相等
	bEqual = FALSE;
	return FALSE;
}

extern "C" int PASCAL GetFolderInformation(
	CString cStrFolderName, 
	BOOL	bIncludeSubFolder,
	BOOL	bIncludeHTMLSubFolder,
	CStringArray &arrayFilesResult, 
	INT64 &nTotalSize, 
	int &nSubFolderNumber, 
	int &nFilesNumber)
{


	//
	//计算文件／目录／容量
	//

	nFilesNumber = 0;
	nSubFolderNumber = 0;
	nTotalSize = 0;


	//
	//	获得完整的文件列表
	//
	CFileFind finder;

	CStringArray arrayFileName;
	//	arrayFileName.Copy(*m_pArrayFileName);
	arrayFileName.Add(cStrFolderName);
	BOOL	bNoFolderAnymore = TRUE;
	arrayFilesResult.RemoveAll();//先把最终的文件列表清空

	//-------将所有文件解开统计文件总的大小,同时获得完全展开后的文件列表-------------
	//对于非展开目录选项的情况下m_cArrayFile2BeDone 是没有意义的
	for(int ii=0;;++ii) {
		bNoFolderAnymore = TRUE;
		nFilesNumber = 0;
		nTotalSize = 0;
		BOOL  bIsHTMLFileFolder;
		for(int i=0; i<arrayFileName.GetCount(); ++i) {
			if(finder.FindFile(arrayFileName[i])==0) {
				continue;
			}
			finder.FindNextFile();

			//HTML储存后产生的目录不算
			if(!bIncludeHTMLSubFolder&&finder.IsDirectory()) {
				bIsHTMLFileFolder = FALSE;
				CString cStrHTML = arrayFileName[i].Right(6);
				if(cStrHTML.CompareNoCase(_T("_files"))==0||cStrHTML.CompareNoCase(_T(".files"))==0) {//表示最后6个字符串是 _files或者.files
					cStrHTML = arrayFileName[i].Left(arrayFileName[i].GetLength()-6) + _T(".htm");
					//找找看同级有没有对应的HTML文件
					for(int jk=0; jk<arrayFileName.GetCount(); ++jk) {
						if(arrayFileName[jk].CompareNoCase(cStrHTML)==0||arrayFileName[jk].CompareNoCase(cStrHTML+_T("l"))==0) {
							bIsHTMLFileFolder = TRUE;
							break;
						}
					}
				}
				if(bIsHTMLFileFolder) continue;
			}
			//如果发现是目录
			if((finder.IsDirectory()&&bIncludeSubFolder)||(finder.IsDirectory()&&ii==0)) {//i==0表示是本身的根目录
				++nSubFolderNumber;
				//查找该目录下的所有文件(包括子目录)
				CString cStrSearch;
				cStrSearch = arrayFileName[i] + _T("\\*");
				WIN32_FIND_DATA findData;
				HANDLE hFind = FindFirstFile(cStrSearch, &findData);
				if (hFind == INVALID_HANDLE_VALUE)//该目录没有文件,就不要了
					continue;
				//加入所有子目录的文件
				do {
					if (_tcscmp(findData.cFileName, _T(".")) == 0 || 
						_tcscmp(findData.cFileName, _T("..")) == 0)
						continue;

					//将所有文件加进去
					{
						arrayFilesResult.Add(arrayFileName[i]+_T("\\")+findData.cFileName);
						//判断入库文件数量是否超标
						//						if(arrayFilesResult.GetCount()>MAX_FILENUMBER_TO_IMPORT_FOLDER) {
						//							AfxMessageBox(_T("入库文件超过单次入库最大数量(10万)!\n\n请减少数量或者将目录作为整体(一个文档)入库!"));
						//							OnCancel();//退出对话框
						//						}
					}
					//统计文件大小
					if(findData.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY) {
						nTotalSize = findData.nFileSizeHigh;
						nTotalSize *= (MAXDWORD+(INT64)1);
						nTotalSize += findData.nFileSizeLow;
						++nFilesNumber;
					}

				} while (FindNextFile(hFind, &findData) != 0);

				FindClose(hFind);
				//发现有目录,所以
				bNoFolderAnymore = FALSE;
			}
			//如果是文件
			else if(!finder.IsDirectory()){
				arrayFilesResult.Add(arrayFileName[i]);
				//统计文件大小
				nTotalSize += finder.GetLength();
				++nFilesNumber;
				//判断入库文件数量是否超标
				//				if(arrayFilesResult.GetCount()>MAX_FILENUMBER_TO_IMPORT_FOLDER) {
				//					AfxMessageBox(_T("入库文件超过单次入库最大数量(10万)!\n\n请减少数量或者将目录作为整体(一个文档)入库!"));
				//					OnCancel();//退出对话框
				//				}
			}
		}//
		//如果这一次还有目录被分解,那就做下去
		if(!bNoFolderAnymore) {
			arrayFileName.RemoveAll();
			arrayFileName.Copy(arrayFilesResult);
			arrayFilesResult.RemoveAll();
		}
		else break;
	}//分解文件循环结束


	return 0;

}

