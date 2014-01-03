
#include "StdAfx.h"
#include "CBerkeleyDB.h"
#include "Windows.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

void DbException_DisplayError(DbException  &e, int nLine)
{
	CString cStrTemp;
	wchar_t *wstr, *wstr1;
	GetWideChar(e.what(),wstr);
	GetWideChar(__FILE__,wstr1);
	cStrTemp.Format(_T("请将以下信息反馈至 Puhonghao@126.com，非常感谢！\n\n File:%s Line:%d \n DbException--错误信息:%s, 错误代码:%d\n\n系统未能处理此异常，您需要退出系统再重新进入！"), wstr1, nLine, wstr, e.get_errno());
	AfxMessageBox(cStrTemp);
	delete []wstr;
	delete []wstr1;
}
void XmlException_DisplayError(XmlException &e,int nLine)
{
	CString cStrTemp;
	wchar_t *wstr, *wstr1;
	GetWideChar(e.what(),wstr);
	GetWideChar(__FILE__,wstr1);
	cStrTemp.Format(_T("请将以下信息反馈至 Puhonghao@126.com，非常感谢！\n\n  File:%s Line:%d \n XmlException--错误信息:%s, 错误代码:%d, DB错误代码:%d\n\n系统未能处理此异常，您需要退出系统再重新进入！"), wstr1, nLine, wstr, e.getExceptionCode(), e.getDbErrno());
	AfxMessageBox(cStrTemp);
	delete []wstr;
	delete []wstr1;
}

void BDBErrorCallback(const DbEnv *dbenv, const char *errpfx, const char *msg)
{
	CString cStrTemp;
	wchar_t *wszErrpgx=NULL;
	wchar_t *wszMsg = NULL;
	if(errpfx)
		GetWideChar(errpfx, wszErrpgx);
	if(msg)
		GetWideChar(msg, wszMsg);

	cStrTemp.Format(_T("文档库错误调试信息：%s --- %s"), wszErrpgx, wszMsg);

	if(wszErrpgx) delete []wszErrpgx;
	if(wszMsg) delete []wszMsg;
	//写入日志文件
	g_pcInformation->reportSpecialError(cStrTemp, _T("DB内部错误"), pael_WriteLogFile);
	//AfxMessageBox(cStrTemp);
}


//
//
//---------------------------------------- CBerkerleyDB ----------------------------------------
//
//

CBerkeleyDB::CBerkeleyDB():opened(false),container(NULL),myManager(NULL),m_DBEnv(NULL)
{

}

CBerkeleyDB::~CBerkeleyDB()
{
	close();
}



BOOL CBerkeleyDB::Init(const wchar_t *dbpath, const char *pszAlias){

	CString cStrPath, cStrFileTitle, cStrPostfix;

	::SplitFile2Part(dbpath, cStrPath, cStrFileTitle, cStrPostfix);

	char * szPath=NULL;
	GetSimpleChar(cStrPath, szPath);

	std::string envHome(szPath);
	delete [] szPath;


	XmlTransaction containerTxn=NULL;
	char * DB_PATH=NULL;
	try{
		if(!m_DBEnv) {
			m_DBEnv=new DbEnv(0);
			m_DBEnv->set_errcall(BDBErrorCallback);
			m_DBEnv->set_cachesize(0, 64 * 1024 * 1024, 1);
			m_DBEnv->set_flags(DB_LOG_AUTOREMOVE, 1);
			u_int32_t nMutex;

			int nRet = m_DBEnv->mutex_get_max(&nMutex);
			//m_DBEnv->get_lg_regionmax(&nMutex);
			VERIFY(m_DBEnv->set_lg_regionmax(650000)==0);
			VERIFY(m_DBEnv->set_lk_max_locks(1000*300)==0);
			VERIFY(m_DBEnv->set_lk_max_lockers(1000*300)==0);
			VERIFY(m_DBEnv->set_lk_max_objects(1000*300)==0);

			nRet=m_DBEnv->open(envHome.c_str(),DB_RECOVER|DB_INIT_MPOOL|DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_TXN|DB_REGISTER,0);
			if(nRet!=0) {
				switch(nRet) {
					case DB_RUNRECOVERY:
						AfxMessageBox(_T("DB_RUNRECOVERY"));
						break;
					case DB_VERSION_MISMATCH:
						AfxMessageBox(_T("当前DB版本和文档库创建的DB版本不同"));
						break;
					case EAGAIN:
						AfxMessageBox(_T("The shared memory region was locked and (repeatedly) unavailable"));
						break;
					case EINVAL:
						AfxMessageBox(_T("EINVAL"));
						break;
					case ENOSPC:
						AfxMessageBox(_T("ENOSPC"));
						break;
					case ENOENT:
						AfxMessageBox(_T("文件或目录不存在"));
						break;
				}
				m_DBEnv->close(0);delete m_DBEnv;m_DBEnv=NULL;
				return FALSE;
			}
			//			m_DBEnv->open(envHome.c_str(),DB_INIT_MPOOL|DB_INIT_LOCK|DB_INIT_LOG,0);
		}
		myManager =new XmlManager(m_DBEnv,DBXML_ADOPT_DBENV);
		containerTxn = myManager->createTransaction();
		GetSimpleChar(dbpath,DB_PATH);
		container = myManager->openContainer(containerTxn,DB_PATH,DB_CREATE|DBXML_INDEX_NODES|DBXML_STATISTICS);//蒲 洪浩的文档库
		containerTxn.commit();
		BOOL bRet = container.getIndexNodes();
		//		container.addAlias(XMCONTAINER_ALIAS);
		strncpy_s(m_szAliasName, MAX_ALIASNAME-1, pszAlias, strlen(pszAlias));
		container.addAlias(m_szAliasName);
		delete [] DB_PATH;
		//移到前面去了
//		containerTxn.commit();
		opened=TRUE;

		g_pcInformation->reportSpecialError(_T("库打开成功"), dbpath, pael_WriteLogFile);

		return TRUE;
	}catch(DbException &e){
		if(e.get_errno()==2){;}//2表示路径不存在
		else{
			CString  cStrTemp;
			cStrTemp.Format(_T("错误：Init(path=%s)"), dbpath);
			AfxMessageBox(cStrTemp);//
			DbException_DisplayError(e,__LINE__);//显示错误信息
			//ASSERT(FALSE);
		}
		if(DB_PATH!=NULL){
			delete[] DB_PATH;
			DB_PATH=NULL;
		}
		m_DBEnv->close(0);delete m_DBEnv;m_DBEnv=NULL;

		g_pcInformation->reportSpecialError(_T("库打开失败"), dbpath, pael_WriteLogFile);
		return FALSE;
	}

	catch (XmlException &e) {
		XmlException_DisplayError(e,__LINE__);
		if(DB_PATH!=NULL){
			delete[] DB_PATH;
			DB_PATH=NULL;
		}
		containerTxn.abort();
		m_DBEnv->close(0);delete m_DBEnv;m_DBEnv=NULL;
		g_pcInformation->reportSpecialError(_T("库打开失败"), dbpath, pael_WriteLogFile);
		return false;
	}

}




BOOL CBerkeleyDB::close(){

	wchar_t *wszName=NULL;
	CString cStrName;

	try {
		//		if(container!=NULL && opened==TRUE) {		//  [12/8/2008 Harley]
		if(!container.isNull()&& opened==TRUE) {
			std::string str = container.getName();
			GetWideChar(str, wszName);
			cStrName = wszName;
			delete [] wszName;

			container.close();
			container=NULL;
			opened=FALSE;
			g_pcInformation->reportSpecialError(_T("container Close OK"), cStrName,pael_WriteLogFile);
		}
		if (myManager != NULL) {
			delete myManager;
			myManager=NULL;
			m_DBEnv=NULL;//因为删除Manager时同时关闭和删除了Env（DBXML_ADOPT_DBENV参数）
			g_pcInformation->reportSpecialError(_T("myManager close OK"), cStrName, pael_WriteLogFile);
		}
		return TRUE;
	} catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return FALSE;
	} catch(XmlException &e) {
		XmlException_DisplayError(e,__LINE__);
		return FALSE;
	}
}

int CBerkeleyDB::InitNewDB(CString cStrKMDBName)
{
	ASSERT(cStrKMDBName.GetLength()>0);
	//
	//写入安全信息
	//
	CString cStrSecuInf;
	cStrSecuInf.Format(_T("<Item><Title>%s</Title></Item>"), cStrKMDBName);

	TCHAR *pk=NULL;
	//如果已经存在
	int nRet = AddDocumentByKey(KMDB_ID_SECURITY_INF, cStrSecuInf, pk);
	if(nRet==KMERROR_XMLITEM_ALREADYEXIST) {
		if(!DoUpdateDocument(KMDB_ID_SECURITY_INF, cStrSecuInf))
			return KMERROR_DB_CANNOTUPDATE;
	}
	else if(nRet!=KMERROR_SUCCESS) {
		return KMERROR_DB_CANNOTUPDATE;
	}

	//	if(pk==NULL||_tcscmp(pk, KMDB_ID_SECURITY_INF)!=0) {
	//		ASSERT(FALSE);
	//		return KMERROR_UNKNOWN;
	//	}
	if(pk) {delete []pk; pk=NULL;}



	return KMERROR_SUCCESS;

}





//Add documents to container
int CBerkeleyDB::AddDocument(const wchar_t *str,wchar_t * &pk){

	if(!opened) {
		//		AfxMessageBox(_T("文档库没有打开,无法添加新纪录!"));
		return KMERROR_DB_NOTOPEN;
	}

	char *psText=NULL;


	//
	//先把id=...去掉
	//
	//下次把这个王八蛋程序改掉!
	//
	COneLevelXMLData fxData;
	fxData.ReplaceWith(str);
	fxData.EraseItemID();

	GetSimpleChar(fxData.GetXMLString(),psText);
	//GetSimpleChar(str,psText);

	//XmlTransaction txn=NULL;
	std::string pkstr;
	try{
		XmlUpdateContext updateContext = myManager->createUpdateContext();
		std::string name="KMDB";
		pkstr=container.putDocument(name,psText,updateContext,DBXML_GEN_NAME);
		delete [] psText; psText=NULL;
		//CString s=str;
		CString s=fxData.GetXMLString();
		int fstLeft=s.Find(_T('<')); //Search First "<"
		int fstRiht=s.Find(_T('>')); //Search First ">"
		CString newStr=s.Mid(fstLeft+1,fstRiht-fstLeft-1).Trim();
		newStr=newStr.Left(4);
		if(newStr.Compare(_T("Item"))==0){
			XmlQueryContext context = myManager->createQueryContext();
			XmlModify mod=myManager->createModify();
			XmlQueryExpression predicates=myManager->prepare("/Item",context);
			mod.addAppendStep(predicates, XmlModify::Attribute, "id", pkstr);
			XmlDocument doc=container.getDocument(pkstr);
			XmlValue docValue(doc);
			int counts=mod.execute(docValue,context,updateContext);
		}

	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return KMERROR_EXCEPTION;
	} 
	catch(XmlException &e){
		XmlException_DisplayError(e,__LINE__);

		if(psText) {delete []psText;psText=NULL;}
		return KMERROR_EXCEPTION;
	}
	if(!pkstr.empty())
		GetWideChar(pkstr,pk);

	//数据库改变提醒
	ContentChanged(pk, 1);

	return KMERROR_SUCCESS;
}


//Add documents to container
int CBerkeleyDB::AddDocumentByKey(const wchar_t * key,const wchar_t *str,wchar_t *& pk){

	if(!opened)
		return KMERROR_DB_NOTOPEN;

	char *psText=NULL;

	GetSimpleChar(str,psText);

	char * itemKey=NULL;

	GetSimpleChar(key,itemKey);

	//XmlTransaction txn=NULL;
	std::string pkstr;
	try{
		// Get an XmlUpdateContext. Useful from a performance perspective.
		XmlUpdateContext updateContext = myManager->createUpdateContext();
		pkstr=container.putDocument(itemKey,psText,updateContext);
		delete [] itemKey; itemKey=NULL;
		delete []psText; psText=NULL;
		CString s=str;
		int fstLeft=s.Find(_T('<')); //Search First "<"
		int fstRiht=s.Find(_T('>')); //Search First ">"
		CString newStr=s.Mid(fstLeft+1,fstRiht-fstLeft-1).Trim();
		newStr=newStr.Left(4);
		if(newStr.Compare(_T("Item"))==0){
			XmlQueryContext context = myManager->createQueryContext();
			XmlModify mod=myManager->createModify();
			XmlQueryExpression predicates=myManager->prepare("/Item",context);
			mod.addAppendStep(predicates, XmlModify::Attribute, "id", pkstr);
			XmlDocument doc=container.getDocument(pkstr);
			XmlValue docValue(doc);
			int counts=mod.execute(docValue,context,updateContext);
		}

	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return KMERROR_UNKNOWN;
	} 
	catch(XmlException &e){
		if(e.getExceptionCode()==XmlException::UNIQUE_ERROR)
			return KMERROR_XMLITEM_ALREADYEXIST;

		XmlException_DisplayError(e,__LINE__);

		if(itemKey) {delete [] itemKey;itemKey=NULL;}
		if(psText) {delete []psText;psText=NULL;}
		return KMERROR_UNKNOWN;
	}
	if(!pkstr.empty())
		GetWideChar(pkstr,pk);
	else
		return KMERROR_UNKNOWN;

	//数据库改变提醒
	ContentChanged(pk, 1);

	return KMERROR_SUCCESS;
}

int CBerkeleyDB::GetAllItems( ARRAY_OLXDATA * arrayResult )
{
	if(!opened)
		return KMERROR_DB_NOTOPEN;

	arrayResult->RemoveAll();
	XmlResults results=container.getAllDocuments(0);
	while (results.hasNext())
	{
		XmlDocument doc;
		if(results.next(doc)==true)
		{
			COneLevelXMLData xmlData;
			XmlValue xmlValue(doc);
			wchar_t * tempWstr=NULL;
			GetWideChar(xmlValue.asString(),tempWstr);
			if(!xmlData.ReplaceWith(tempWstr)){
				delete tempWstr;
				tempWstr=NULL;
				arrayResult->RemoveAll();
				return KMERROR_UNKNOWN;
			}
			delete[] tempWstr;
			tempWstr=NULL;
			GetWideChar(doc.getName(),tempWstr);
			xmlData.SetItemID(tempWstr);
			delete[] tempWstr;
			tempWstr=NULL;
			arrayResult->Add(xmlData);
		}
	}

	return KMERROR_SUCCESS;

}


int CBerkeleyDB::GetAItem(const wchar_t *szFileID, CString &cStrItem)
{

	TCHAR *resItem=NULL;

	int nRet = GetAItem(szFileID, resItem);
	if(nRet==KMERROR_SUCCESS) {
		cStrItem = resItem;
	}
	if(resItem) 
		delete []resItem;

	return nRet;



}

int CBerkeleyDB::GetAItem(const wchar_t *szFileID, wchar_t* &resItem)
{

	if(!opened)
		return KMERROR_DB_NOTOPEN;

	char *psText=NULL;
	GetSimpleChar(szFileID,psText);
	try{
		XmlDocument doc=container.getDocument(psText);
		delete [] psText;
		psText = NULL;
		XmlValue xmlValue(doc);
		GetWideChar(xmlValue.asString(),resItem);
	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return KMERROR_UNKNOWN;
	} 
	catch(XmlException &e){
		if(NULL!=psText)
			delete [] psText;
		if(e.getExceptionCode()==XmlException::DOCUMENT_NOT_FOUND)
			return KMERROR_FILEID_NOTEXIST;
		else {
			CString cStrTemp;
			cStrTemp.Format(_T("错误：GetAItem(%s, %s)"), szFileID, resItem);
			AfxMessageBox(cStrTemp);//
			XmlException_DisplayError(e,__LINE__);
		}
	}

	return KMERROR_SUCCESS;
}

int CBerkeleyDB::DeleteDoc(const wchar_t * key){
	if(!opened)
		return 15;
	char * psText=NULL;
	GetSimpleChar(key,psText);

	try{
		XmlUpdateContext updateContext = myManager->createUpdateContext();
		container.deleteDocument(psText,updateContext);
		delete [] psText;

		//数据库改变提醒
		ContentChanged(key, -1);

		return KMERROR_SUCCESS;
	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return false;
	} 
	catch(XmlException &e){
		delete [] psText;
		if(e.getExceptionCode()==XmlException::DOCUMENT_NOT_FOUND)
			return KMERROR_DB_FILEIDNOTFOUND;
		else {
			XmlException_DisplayError(e,__LINE__);
			return KMERROR_UNKNOWN;
		}
	}

}




int CBerkeleyDB::DoUpdate(const wchar_t *docName, const wchar_t *node, const wchar_t * value){

	if(!opened)
		return -1;

	char * psText=NULL;
	GetSimpleChar(docName,psText);
	char * s_node=NULL;
	GetSimpleChar(node,s_node);
	char * s_value=NULL;
	GetSimpleChar(value,s_value);

	int counts=0;

	try{
		//Excutes modification
		XmlQueryContext context = myManager->createQueryContext();
		XmlModify mod=myManager->createModify();
		XmlQueryExpression predicates=myManager->prepare(s_node,context);
		mod.addUpdateStep(predicates,s_value);
		XmlUpdateContext uc = myManager->createUpdateContext();
		XmlDocument doc=container.getDocument(psText);
		XmlValue docValue(doc);

		counts=mod.execute(docValue,context,uc);

		delete [] s_node;
		delete [] s_value;
		delete []psText;
	}catch(std::exception &){
		delete [] s_node;
		delete [] s_value;
		delete []psText;
		return -2;
	}

	//数据库改变提醒
	ContentChanged(docName, 0);

	return counts;
}


BOOL CBerkeleyDB::DoUpdateDocument(const wchar_t * docName,const wchar_t * value)
{

	if(!opened) {
		//		AfxMessageBox(_T("数据库没有打开!"));
		return FALSE;
	}
	CString cStrValue2Update = value;


	//如果是文件Item，加上ID
	COneLevelXMLData xmlData;
	if(xmlData.ReplaceWith(value)) {
		xmlData.SetItemID(docName);
		cStrValue2Update = xmlData.GetXMLString();
	}

	char * pszKey=NULL;
	GetSimpleChar(docName, pszKey);

	XmlUpdateContext updateContext = myManager->createUpdateContext();
	XmlDocument xmlDoc;
	try{
		xmlDoc = container.getDocument(pszKey);
	}
	catch(DbException &e) {
		if(pszKey) {delete []pszKey;pszKey=NULL;}
		e;
		//		DbException_DisplayError(e,__LINE__);//显示错误信息
		return false;
	} 
	catch(XmlException &e){
		if(pszKey) {delete []pszKey;pszKey=NULL;}
		e;
		//		XmlException_DisplayError(e,__LINE__);
		return false;
	}

	char * psText=NULL;
	GetSimpleChar(cStrValue2Update, psText);

	xmlDoc.setContent(psText);

	if(psText){delete []psText; psText=NULL;}
	if(pszKey){delete []pszKey; pszKey=NULL;}

	try{
		container.updateDocument(xmlDoc,updateContext);
	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return false;
	} 
	catch(XmlException &e){
		XmlException_DisplayError(e,__LINE__);
		return false;
	}

	//数据库改变提醒
	ContentChanged(docName, 0);

	return TRUE;


}


//Query Container using xquery or simply use xpath
int CBerkeleyDB::DoQuery(const wchar_t *str,wchar_t* &reswchar){

	if(!opened)
		return -1;

	char *psText=NULL;

	GetSimpleChar(str,psText);

	//Get Result Set
	XmlResults res;
	try{
		XmlQueryContext context = myManager->createQueryContext();
		//std::string querystr="collection('" + container.getName() + "')" + psText;
		std::string querystr="collection('";
		//		querystr+=XMCONTAINER_ALIAS;
		querystr+=m_szAliasName;
		querystr+="')" ;
		querystr+= psText;
		res = myManager->query(querystr,context);
		if(psText) {delete []psText;psText=NULL;}
	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		if(psText) {delete []psText;psText=NULL;}
		return false;
	} 
	catch(XmlException &e) {
		if(psText) {delete []psText;psText=NULL;}
		XmlException_DisplayError(e,__LINE__);

		return e.getExceptionCode()>0? e.getExceptionCode()*(-1):e.getExceptionCode();
	}

	if(res.isNull())
		return -1;

	//Constructs output string
	XmlValue value;

	std::string resstr="";

	int index=0; //Item counts

	while(res.next(value)){
		std::string name=value.asDocument().getName();
		DbXml::XmlDocument xmlDoc=container.getDocument(name);
		XmlValue xmlValue(xmlDoc);
		try{
			_XML_INIT_;
		}catch(xercesc::XMLException&){
			return -2; //Not Well Formated XML String
		}

		XercesDOMParser * m_Parser= new XercesDOMParser;
		m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
		m_Parser->setDoNamespaces( false );
		m_Parser->setDoSchema( false );
		m_Parser->setLoadExternalDTD( false );
		const char * _xml_Str=xmlValue.asString().c_str();
		InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

		if (NULL == pInputSource) {
			delete m_Parser;
			_XML_RELEASE_;
			return -2;
		}

		try{
			m_Parser->parse(*pInputSource);
		}catch(DOMException &){
			delete pInputSource;
			delete m_Parser;
			_XML_RELEASE_;
			return -2;
		}catch(XMLException &){
			delete pInputSource;
			delete m_Parser;
			_XML_RELEASE_;
			return -2;
		}

		delete pInputSource;
		xercesc_2_8::DOMDocument* p_xmlDoc = m_Parser->getDocument();

		DOMElement* root = p_xmlDoc->getDocumentElement();
		wchar_t * newXMLName=NULL;
		GetWideChar(name,newXMLName);
		root->setAttribute(_T("id"),newXMLName);
		delete [] newXMLName;
		//==================================
		//Contruct New XML String
		DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
		DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
		/*const */XMLCh * newValue = writer->writeToString(*root);
		writer->release();
		char * s_newValue=NULL;
		GetSimpleChar(newValue,s_newValue);
		//delete newValue;
		XMLString::release(&newValue);

		delete m_Parser;
		_XML_RELEASE_;

		resstr+=s_newValue;
		delete [] s_newValue;
		index++;
	}

	CString cStrTemp;
	cStrTemp.Format(_T("<SearchRes count=\"%d\"> %s </SearchRes>"), index,resstr);

	reswchar = new wchar_t[cStrTemp.GetLength()+1];
	if(reswchar==NULL){
		return (-1*index);
	}

	wcscpy_s(reswchar, cStrTemp.GetLength()+1, cStrTemp);
	return index;

}


//Query Container using xquery or simply use xpath
int CBerkeleyDB::DoQuery(const wchar_t *str, ARRAY_OLXDATA *arrayResult)
{

	COneLevelXMLData pcFXData;

	if(!opened)
		return -1;

	char *psText=NULL;

	GetSimpleChar(str,psText);

	//Get Result Set
	XmlResults res;
	try{
		XmlQueryContext context = myManager->createQueryContext();
		std::string querystr="collection('";
		//		querystr+=XMCONTAINER_ALIAS;
		querystr+=m_szAliasName;
		querystr+="')" ;
		querystr +=psText;
		res = myManager->query(querystr,context);
		delete []psText;
	}
	catch(DbException &e) {
		if(psText) {delete []psText;psText=NULL;}
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return false;
	} 
	catch(XmlException &e) {
		XmlException_DisplayError(e,__LINE__);

		if(psText) {delete []psText;psText=NULL;}
		return e.getExceptionCode()>0? e.getExceptionCode()*(-1):e.getExceptionCode();
	}

	if(res.isNull())
		return -1;

	//Constructs output string
	XmlValue value;
	int index=0; //Item counts

	while(res.next(value)){
		std::string name=value.asDocument().getName();
		DbXml::XmlDocument xmlDoc=container.getDocument(name);
		XmlValue xmlValue(xmlDoc);
		wchar_t * domstr=NULL;
		GetWideChar(xmlValue.asString(),domstr);
		pcFXData.ReplaceWith(domstr);
		wchar_t * wName=NULL;
		GetWideChar(name,wName);
		pcFXData.SetItemID(wName);
		delete [] wName;
		arrayResult->Add(pcFXData);
		delete [] domstr;

		index++;
	}

	return index;
}


int CBerkeleyDB::SearchDocuments(const CString& tag, const CString& str, ARRAY_OLXDATA& arrayRes, BOOL blur, BOOL bCaseSensitive)
{
	CString cStrSearch = str;

	//文件路径都要大写
	//	if(tag==KMDB_ITEM_ORIGINAL||tag==KMDB_ITEM_FILEPATH)
	//		cStrSearch.MakeUpper();

	//搜索字串中不能有 &
	cStrSearch.Replace(_T("&"), _T("&amp;"));


	if(!opened)
		return -1;

	char *psText=NULL;

	if (cStrSearch.IsEmpty())
	{
		return -2;
	}

	if(tag.IsEmpty()){
		psText=new char[2];
		psText[0]='*';
		psText[1]=0;
	}
	else
		GetSimpleChar(tag,psText);

	CString resTokenize;
	int curpos=0;

	std::string condationStr="for $root in collection('";

	//	condationStr+=XMCONTAINER_ALIAS;
	condationStr+=m_szAliasName;
	condationStr+="')/Item where ";
	resTokenize=cStrSearch.Tokenize(_T(","),curpos);
	BOOL gtOne=false;
	while (!resTokenize.IsEmpty())
	{
		if (gtOne)
		{
			condationStr+=" or ";
		}

		char * tokenize=NULL;
		GetSimpleChar(resTokenize,tokenize);
		//模糊查询并且区分大小写
		if (blur==TRUE&&bCaseSensitive==TRUE)
		{
			condationStr+="contains($root/";
			condationStr+=psText;
			condationStr+=",\"";
			condationStr+=tokenize;
			condationStr+="\")";
		}
		//不模糊查询，区分大小写(缺省参数！)(就是完全相等）
		else if (blur==FALSE&&bCaseSensitive==TRUE) {
			condationStr+="$root[";
			condationStr+=psText;
			condationStr+="=\"";
			condationStr+=tokenize;
			condationStr+="\"]";
		}
		//不区分大小写（先模糊查出来，后面再判断）
		else//(blur==TRUE&&bCaseSensitive==FALSE)
		{
			condationStr+="dbxml:contains($root/";
			condationStr+=psText;
			condationStr+=",\"";
			condationStr+=tokenize;
			condationStr+="\")";
		}
		//既不模糊查询，又要不区分大小写。这种情况处理不了！
		//else {
		//	ASSERT(FALSE);
		//	if(psText) delete[]psText;
		//	if(tokenize) delete[]tokenize;
		//	return KMERROR_PARAM_ERROR;
		//}

		gtOne=TRUE;

		delete [] tokenize;

		resTokenize=cStrSearch.Tokenize(_T(","),curpos);
	}

	condationStr+=" return $root";
	delete []psText;
	//Get Result Set
	XmlResults res;
	try{
		XmlQueryContext context = myManager->createQueryContext();
		res = myManager->query(condationStr,context);

	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return false;
	} 
	catch(XmlException &e) {
		XmlException_DisplayError(e,__LINE__);

		return e.getExceptionCode()>0? e.getExceptionCode()*(-1):e.getExceptionCode();
	}

	if(res.isNull())
		return -3;
	XmlValue itemValue;
	while (res.next(itemValue))
	{
		COneLevelXMLData fileXml;
		wchar_t * itemValue_u=NULL;
		GetWideChar(itemValue.asString(),itemValue_u);
		if(fileXml.ReplaceWith(itemValue_u)) {
			std::string name=itemValue.asDocument().getName();
			wchar_t * wName=NULL;
			GetWideChar(name,wName);
			fileXml.SetItemID(wName);
			delete [] wName;
			arrayRes.Add(fileXml);
		}
		if(itemValue_u) {delete [] itemValue_u;itemValue_u=NULL;}
	}

	//这里需要判断一下不区分大小写,并且不是模糊查询的情况
	//把不一样的字串去掉
	CString cStrContent;
	if(bCaseSensitive==FALSE&&blur==FALSE) {
		for (int i=0; i<arrayRes.GetCount(); ++i){
			cStrContent = arrayRes[i].GetData(tag, NULL);
			if(cStrContent.CompareNoCase(str)!=0) {
				arrayRes.RemoveAt(i);
				--i;
			}
		}
	}



	return KMERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//									COneLevelXMLData
//
////////////////////////////////////////////////////////////////////////////////////////////////////

XERCES_CPP_NAMESPACE_USE


BOOL COneLevelXMLData::ReplaceWith(const TCHAR* strXML)
{
	if(IsWellFormXML(strXML)){ 
		m_cStrXML=strXML;
		if(m_cStrXML.Find(_T("<Item"))==-1) {
			m_cStrXML.Empty();
			return FALSE;
		}
		return TRUE;
	}
	else{
		return FALSE;
	}
}

int COneLevelXMLData::Data_Item_Set(const TCHAR* strName,const TCHAR* strProperty, const TCHAR* strValue)
{
	CString cStrValue = strValue;

	if(m_cStrXML.GetLength()<4) return KMERROR_NO_VALUE;

	bool changed=false; //Changed status

	XercesDOMParser * m_Parser=NULL;

	try{
		_XML_INIT_;  // Initialize Xerces infrastructure
		m_Parser = new XercesDOMParser();
		m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
		m_Parser->setDoNamespaces( false );
		m_Parser->setDoSchema( false );
		m_Parser->setLoadExternalDTD( false );
		char * _xml_Str=NULL;
		GetSimpleChar(m_cStrXML,_xml_Str);

		InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

		if (NULL == pInputSource) {
			delete [] _xml_Str;
			delete m_Parser;
			_XML_RELEASE_;
			return 15; //Memory lack or unknown error
		}
		m_Parser->parse(*pInputSource);
		delete pInputSource;
		delete [] _xml_Str;
		xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();
		//const XMLCh * encoding=xmlDoc->getEncoding();

		DOMElement* root = xmlDoc->getDocumentElement();
		if (NULL == root) {
			delete m_Parser;
			_XML_RELEASE_;
			return KMERROR_UNKNOWN; //Memory lack or unknown error
		}
		//const XMLCh * nodeName=root->getNodeName();
		DOMNodeList * list= root->getElementsByTagName(strName);
		const XMLSize_t count=list->getLength();
		DOMNode* curNode=NULL;
		DOMElement * addElement=NULL;
		DOMText*  dataVal=NULL;
		switch(count){
			case 0:
				addElement=xmlDoc->createElement(strName);
				root->appendChild(addElement);
				if(strProperty!=NULL){
					addElement->setAttribute(strProperty, cStrValue);
				}else{
					dataVal = xmlDoc->createTextNode(cStrValue);
					addElement->appendChild(dataVal);
				}
				changed=true;
				break;
			case 1:
				curNode=list->item(0); //get first item
				if(curNode->getNodeType() && curNode->getNodeType()==DOMNode::ELEMENT_NODE){
					DOMElement* curElement = (xercesc::DOMElement* ) curNode;
					if(strProperty!=NULL){
						curElement->setAttribute(strProperty,cStrValue);
						changed=true;
					}else{
						if(curNode->hasChildNodes()){
							DOMNodeList * child=curNode->getChildNodes();
							if(child->getLength()!=1){
								delete m_Parser;
								_XML_RELEASE_;
								return 18;
							}
							if(child->item(0)->getNodeType() && child->item(0)->getNodeType()==DOMNode::TEXT_NODE){
								DOMText * textNode=dynamic_cast< DOMText* >(child->item(0));
								textNode->setNodeValue(cStrValue); //set Text Value
								changed=true;
							}
						}else{
							DOMText * textNode=xmlDoc->createTextNode(cStrValue);
							curNode->appendChild(textNode);
							changed=true;
						}
					}
				}
				break;
			default:
				delete m_Parser;
				_XML_RELEASE_;
				return 16; //Not well formated xml
		}

		if(changed){
			DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
			DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
			XMLCh * value = writer->writeToString(*root);
			writer->release();
			delete m_Parser;
			_XML_RELEASE_;
			TCHAR * resStr=new wchar_t[wcslen(value)+1];
			wcscpy_s(resStr,wcslen(value)+1,value);
			m_cStrXML=resStr;
			//delete value;
			XMLString::release(&value);
			delete [] resStr;
			return 0; //Success
		}
		delete m_Parser;
		_XML_RELEASE_;
		return 15; 

	}catch(xercesc::XMLException&){
		if(m_Parser!=NULL)
			delete m_Parser;
		_XML_RELEASE_;
		return 3; //Not Well Formated XML String
	}

}


BOOL COneLevelXMLData::Data_Item_Del(const TCHAR* strName)
{
	if(m_cStrXML.GetLength()<4) return FALSE;


	XercesDOMParser * m_Parser=NULL;
	try{
		_XML_INIT_;  // Initialize Xerces infrastructure
		m_Parser= new XercesDOMParser;
		m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
		m_Parser->setDoNamespaces( false );
		m_Parser->setDoSchema( false );
		m_Parser->setLoadExternalDTD( false );
		char * _xml_Str=NULL;
		GetSimpleChar(m_cStrXML,_xml_Str);

		InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

		if (NULL == pInputSource) {
			delete [] _xml_Str;
			delete m_Parser;
			_XML_RELEASE_;
			return FALSE; //Memory lack or unknown error
		}
		m_Parser->parse(*pInputSource);
		delete pInputSource;
		delete [] _xml_Str;
		xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();

		DOMElement* root = xmlDoc->getDocumentElement();

		BOOL changed=FALSE;
		int nNotElement = 0;
		for(;;) {

			DOMNodeList * list= root->getElementsByTagName(strName);
			const XMLSize_t count=list->getLength();

			if((int)count<=nNotElement) break;
			for(int i=0; i<(int)count; ++i) {
				DOMNode * curNode=list->item(i);
				if(curNode->getNodeType() && curNode->getNodeType()==DOMNode::ELEMENT_NODE){
					DOMNode * delEl=root->removeChild(curNode);
					delEl->release();
					changed=TRUE;
					break;
				}
				else ++nNotElement;
			}
		}

		if(changed){
			DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
			DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
			XMLCh * value = writer->writeToString(*root);
			writer->release();
			TCHAR * resStr=new wchar_t[wcslen(value)+1];
			wcscpy_s(resStr,wcslen(value)+1,value);
			m_cStrXML=resStr;
			//delete value;
			XMLString::release(&value);
			delete [] resStr;
		}

		delete m_Parser;
		_XML_RELEASE_;
		return TRUE;
	}catch(xercesc::XMLException&){
		if(m_Parser!=NULL)
			delete m_Parser;
		_XML_RELEASE_;
		return FALSE; //Not Well Formated XML String
	}


}



BOOL COneLevelXMLData::GetXMLString(TCHAR* &strXML)
{
	if(m_cStrXML.GetLength()<4) return FALSE;

	strXML=new wchar_t[wcslen(m_cStrXML)+1];
	if(strXML==NULL)
		return FALSE;
	wcscpy_s(strXML,wcslen(m_cStrXML)+1,m_cStrXML);
	return TRUE;

}

CString COneLevelXMLData::GetData(const wchar_t* strName, const wchar_t* strProperty) const
{
	CStringArray res;

	GetData(strName, strProperty, res);
	if(res.GetCount()!=1) return _T("");
	return res[0];
}

int COneLevelXMLData::GetData(const TCHAR* strName, const TCHAR* strProperty, CStringArray &arrayRes) const
{
	arrayRes.RemoveAll();

	if(this->m_cStrXML.GetLength()<1)
		return 3;

	try{
		_XML_INIT_;
	}catch(xercesc::XMLException&){
		return 15; //Not Well Formated XML String
	}

	int returnValue=0;
	XercesDOMParser * m_Parser= new XercesDOMParser;
	m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
	m_Parser->setDoNamespaces( false );
	m_Parser->setDoSchema( false );
	m_Parser->setLoadExternalDTD( false );
	char * _xml_Str=NULL;
	GetSimpleChar(m_cStrXML,_xml_Str);
	InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		returnValue=15; //Memory lack or unknown error
		goto RELEASE_AND_RETURN;
	}
	try{
		m_Parser->parse(*pInputSource);
	}catch(DOMException &){
		delete pInputSource;
		delete [] _xml_Str;
		returnValue=3;
		goto RELEASE_AND_RETURN;
	}catch(XMLException &){
		delete pInputSource;
		delete [] _xml_Str;
		returnValue=3;
		goto RELEASE_AND_RETURN;
	}

	delete pInputSource;
	delete [] _xml_Str;
	xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();

	DOMElement* root = xmlDoc->getDocumentElement();
	DOMNodeList * list= root->getElementsByTagName(strName);
	const XMLSize_t count=list->getLength();
	if(count<=0){ //always only one
		returnValue=16; //Can't Element Matches Name "strName"
		goto RELEASE_AND_RETURN;
	}

	//arrayRes.SetSize(count);
	for(unsigned int i=0;i<count;i++){
		DOMNode* curNode=list->item(i); //get first item
		if(curNode->getNodeType() && curNode->getNodeType()==DOMNode::ELEMENT_NODE){
			DOMElement* curElement = (xercesc::DOMElement* ) curNode;
			if(strProperty!=NULL){
				const XMLCh * resValue=curElement->getAttribute(strProperty);
				if(resValue==NULL){
					//如果只有一个，那么找不到就返回错
					if(count==1) {
						returnValue=17;
						arrayRes.RemoveAll();
						goto RELEASE_AND_RETURN; //Break To Return Tag
					}
					//否则就返回一个空，因为有可能下一个会有
					arrayRes.Add(_T(""));
				}
				else if(wcslen(resValue)<=0){
					//					returnValue=18;
					//					arrayRes.RemoveAll();
					//					goto RELEASE_AND_RETURN; //Break To Return Tag
					arrayRes.Add(_T(""));
				}
				else 
					arrayRes.Add(resValue);
				continue;
			}
			else if(curNode->hasChildNodes()){
				DOMNodeList * child=curNode->getChildNodes();
				if(child->getLength()!=1){
					returnValue=18;
					arrayRes.RemoveAll();
					goto RELEASE_AND_RETURN; //Break To Return Tag
				}
				else if(child->item(0)->getNodeType() && child->item(0)->getNodeType()==DOMNode::TEXT_NODE){
					DOMText * textNode=dynamic_cast< DOMText* >(child->item(0));
					const XMLCh * resValue=textNode->getNodeValue();
					//if(resValue==NULL || wcslen(resValue)<=0){
					if(resValue==NULL&&count==1){
						returnValue=18;
						arrayRes.RemoveAll();
						goto RELEASE_AND_RETURN; //Break To Return Tag

					}
					else if(resValue==NULL&&count>1){
						arrayRes.Add(_T(""));
					}
					arrayRes.Add(resValue);
				}
			}
		}
	}


RELEASE_AND_RETURN:	//Tag For resource release and return
	delete m_Parser;
	_XML_RELEASE_;


	return returnValue;

}

CString COneLevelXMLData::GetItemID(void) const
{
	if(m_cStrXML.GetLength()<4) return _T("");

	CString returnValue;
	try{
		_XML_INIT_;
	}catch(xercesc::XMLException&){
		return returnValue; //Not Well Formated XML String
	}

	XercesDOMParser * m_Parser= new XercesDOMParser;
	m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
	m_Parser->setDoNamespaces( false );
	m_Parser->setDoSchema( false );
	m_Parser->setLoadExternalDTD( false );
	char * _xml_Str=NULL;
	GetSimpleChar(m_cStrXML,_xml_Str);
	InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return returnValue;
	}

	try{
		m_Parser->parse(*pInputSource);
	}catch(DOMException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return returnValue;
	}catch(XMLException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return returnValue;
	}

	delete pInputSource;
	delete [] _xml_Str;
	xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();

	DOMElement* root = xmlDoc->getDocumentElement();
	const XMLCh *id= root->getAttribute(_T("id"));
	if(id!=NULL){
		returnValue=id;
	}

	delete m_Parser;
	_XML_RELEASE_;
	return returnValue;

}


int COneLevelXMLData::SetItemID(CString id){
	if(m_cStrXML.GetLength()<4) return -1;

	try{
		_XML_INIT_;
	}catch(xercesc::XMLException&){
		return -2; //Not Well Formated XML String
	}

	XercesDOMParser * m_Parser= new XercesDOMParser;
	m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
	m_Parser->setDoNamespaces( false );
	m_Parser->setDoSchema( false );
	m_Parser->setLoadExternalDTD( false );
	char * _xml_Str=NULL;
	GetSimpleChar(m_cStrXML,_xml_Str);
	InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return -2;
	}

	try{
		m_Parser->parse(*pInputSource);
	}catch(DOMException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return -2;
	}catch(XMLException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return -2;
	}

	delete pInputSource;
	delete [] _xml_Str;
	xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();

	DOMElement* root = xmlDoc->getDocumentElement();
	root->setAttribute(_T("id"),id);

	//==================================
	//Contruct New XML String
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
	DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
	XMLCh * newValue = writer->writeToString(*root);
	writer->release();
	m_cStrXML=newValue;
	//delete newValue;//  [3/17/2009 Harley]
	XMLString::release(&newValue);

	delete m_Parser;
	_XML_RELEASE_;
	return 0;
}



BOOL COneLevelXMLData::Data_Update(const TCHAR* strXML)
{
	if(!IsWellFormXML(strXML))
	{
		return FALSE;
	}

	if (m_cStrXML.IsEmpty())
	{
		m_cStrXML=strXML;
		return TRUE;
	}

	try{
		_XML_INIT_;
	}catch(xercesc::XMLException&){
		return FALSE; //Not Well Formated XML String
	}

	int returnValue=0;
	XercesDOMParser * m_Parser= new XercesDOMParser;
	m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
	m_Parser->setDoNamespaces( false );
	m_Parser->setDoSchema( false );
	m_Parser->setLoadExternalDTD( false );
	char * _xml_Str=NULL;

	////////////////////////////////////////////////////////////////
	GetSimpleChar(strXML,_xml_Str);
	InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}
	try{
		m_Parser->parse(*pInputSource);
	}catch(DOMException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}catch(XMLException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}

	delete pInputSource;
	delete [] _xml_Str;


	xercesc_2_8::DOMDocument* strXMLDoc = m_Parser->getDocument();
	DOMElement* strXMLRoot = strXMLDoc->getDocumentElement();

	const XMLCh * rootName=strXMLRoot->getTagName();

	if(wcscmp(rootName,_T("Item"))!=0)
	{
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}

	/////////////////////////////////////////////////////////////

	_xml_Str=NULL;
	pInputSource=NULL;

	GetSimpleChar(m_cStrXML,_xml_Str);
	pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("J-BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}
	try{
		m_Parser->parse(*pInputSource);
	}catch(DOMException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}catch(XMLException &){
		delete pInputSource;
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}

	delete pInputSource;
	delete [] _xml_Str;
	xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();
	DOMElement* root = xmlDoc->getDocumentElement();
	//====================================================
	//Set Root Attribute

	DOMNamedNodeMap * tempAttrs= strXMLRoot->getAttributes();
	XMLSize_t attrCounts=tempAttrs->getLength();

	for (XMLSize_t i=0L;i<attrCounts;i++)
	{
		DOMNode * copyNode=xmlDoc->importNode(tempAttrs->item(i),true);
		root->setAttributeNode((DOMAttr *)copyNode);
	}


	//===================================
	//Process Child nodes

	DOMNodeList * strNodeList = strXMLRoot->getChildNodes();

	XMLSize_t strChildCounts = strNodeList->getLength();

	if (strChildCounts<=0L)
	{
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE;
	}

	for (XMLSize_t i=0L;i<strChildCounts;i++)
	{
		DOMElement * strElement=(DOMElement *)strNodeList->item(i);
		const XMLCh * elementName=strElement->getTagName();
/*
		//While is a "Link" or "Sort" Element, just add
		if(wcscmp(elementName,KMDB_ITEM_LINK)==0 || wcscmp(elementName,KMDB_ITEM_SORT)==0)
		{
			DOMNode * copyNode=xmlDoc->importNode(strElement,true);
			root->appendChild(copyNode);
			continue;
		}

		//While is a "Keyword" Element, replace it
		if (wcscmp(elementName,KMDB_ITEM_KEYWORD)==0)
		{
			DOMNodeList * temp_list = root->getElementsByTagName(KMDB_ITEM_KEYWORD);
			if (temp_list->getLength()>1L)
			{
				delete m_Parser;
				_XML_RELEASE_;
				return FALSE;
			}

			if (temp_list->getLength() < 1L)
			{
				DOMNode * copyNode=xmlDoc->importNode(strElement,true);
				root->appendChild(copyNode);
			}else
			{
				DOMElement * tempRootEl=(DOMElement *)temp_list->item(0);
				DOMNamedNodeMap * tempAttrs= strElement->getAttributes();
				XMLSize_t attrCounts=tempAttrs->getLength();
				for (XMLSize_t i=0L;i<attrCounts;i++)
				{
					DOMNode * copyNode=xmlDoc->importNode(tempAttrs->item(i),true);
					tempRootEl->setAttributeNode((DOMAttr *)copyNode);
				}
				tempRootEl->setTextContent(strElement->getTextContent());
			}
			continue;
		}
*/
		//chech attribute and fields
		DOMNodeList * temp_list = root->getElementsByTagName(elementName);
		if (temp_list->getLength()>1L)
		{
			delete m_Parser;
			_XML_RELEASE_;
			return FALSE;
		}

		if (temp_list->getLength() < 1L)
		{
			DOMNode * copyNode=xmlDoc->importNode(strElement,true);
			root->appendChild(copyNode);
			continue;
		}

		DOMElement * tempRootEl=(DOMElement *)temp_list->item(0);
		DOMNamedNodeMap * tempAttrs= strElement->getAttributes();
		XMLSize_t attrCounts=tempAttrs->getLength();
		for (XMLSize_t i=0L;i<attrCounts;i++)
		{
			DOMNode * copyNode = xmlDoc->importNode(tempAttrs->item(i),true);
			tempRootEl->setAttributeNode((DOMAttr *)copyNode);
		}
		//if(strElement->getTextContent()!=NULL)
		tempRootEl->setTextContent(strElement->getTextContent());
	}
	//==================================
	//Contruct New XML String
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
	DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
	XMLCh * value = writer->writeToString(*root);
	writer->release();
	m_cStrXML=value;
	//delete value;
	XMLString::release(&value);

	//===============================

	delete m_Parser;
	_XML_RELEASE_;
	return TRUE;

}


void COneLevelXMLData::Data_Update(COneLevelXMLData &other)
{
	Data_Update(other.m_cStrXML);
}

//BOOL CFileXMLData::Data_Link_Add(const TCHAR* strTitle, const TCHAR* strFilePath, const TCHAR* strType, const TCHAR* strGrade,const TCHAR * strFileID)

int COneLevelXMLData::Data_MultiItem_Set(CString cStrName, const CStringArray &arrayProperty, CString cStrValue)
{
	if(m_cStrXML.GetLength()<4) return KMERROR_XML_FORMATERROR;
	//没有文件ID的不能加
	if(cStrValue.GetLength()<1) return KMERROR_PARAM_ERROR;

	//如果以前有这个文件ID,也不能加
	CStringArray res;
	GetData(cStrName, NULL, res);
	for(int i=0; i<res.GetCount(); ++i) {
		if(res[i]==cStrValue)
			return KMERROR_XMLITEM_ALREADYEXIST;
	}

	try{
		_XML_INIT_;
	}catch(xercesc::XMLException&){
		return FALSE; //Not Well Formated XML String
	}
	XercesDOMParser * m_Parser= new XercesDOMParser;
	m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
	m_Parser->setDoNamespaces( false );
	m_Parser->setDoSchema( false );
	m_Parser->setLoadExternalDTD( false );
	char * _xml_Str=NULL;
	GetSimpleChar(m_cStrXML,_xml_Str);
	InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE; //Memory lack or unknown error
	}
	m_Parser->parse(*pInputSource);
	delete pInputSource;
	delete [] _xml_Str;
	xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();

	DOMElement* root = xmlDoc->getDocumentElement();
	DOMElement* link=xmlDoc->createElement(cStrName);
	CString cStrTemp;
	for(int i=0; i<arrayProperty.GetCount(); ++i) {
		cStrTemp.Format(_T("C%d"), i);
		link->setAttribute(cStrTemp, arrayProperty[i]);
	}
	DOMText * fileID= xmlDoc->createTextNode(cStrValue);
	link->appendChild(fileID);
	root->appendChild(link);

	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
	DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
	XMLCh * value = writer->writeToString(*root);
	writer->release();
	TCHAR * resStr=new wchar_t[wcslen(value)+1];
	wcscpy_s(resStr,wcslen(value)+1,value);
	m_cStrXML=resStr;
	//delete value;
	XMLString::release(&value);
	delete [] resStr;

	delete m_Parser;
	_XML_RELEASE_;
	return TRUE;

}


int COneLevelXMLData::Data_MultiItem_Set(CString cStrName, CString cStrValue)
{
	if(m_cStrXML.GetLength()<4) return FALSE;

	try{
		_XML_INIT_;
	}catch(xercesc::XMLException&){
		return FALSE; //Not Well Formated XML String
	}
	XercesDOMParser * m_Parser= new XercesDOMParser;
	m_Parser->setValidationScheme( XercesDOMParser::Val_Never );
	m_Parser->setDoNamespaces( false );
	m_Parser->setDoSchema( false );
	m_Parser->setLoadExternalDTD( false );
	char * _xml_Str=NULL;
	GetSimpleChar(m_cStrXML,_xml_Str);
	InputSource* pInputSource = new MemBufInputSource((XMLByte *)_xml_Str, strlen(_xml_Str), _T("BDB_GUID"));

	if (NULL == pInputSource) {
		delete [] _xml_Str;
		delete m_Parser;
		_XML_RELEASE_;
		return FALSE; //Memory lack or unknown error
	}
	m_Parser->parse(*pInputSource);
	delete pInputSource;
	delete [] _xml_Str;
	xercesc_2_8::DOMDocument* xmlDoc = m_Parser->getDocument();

	DOMElement* root = xmlDoc->getDocumentElement();
	DOMElement* sort=xmlDoc->createElement(cStrName);
	//	sort->setAttribute(TAG_SORT_ID, strID);
	DOMText * fileID= xmlDoc->createTextNode(cStrValue);
	sort->appendChild(fileID);
	root->appendChild(sort);

	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
	DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();
	XMLCh * value = writer->writeToString(*root);
	writer->release();
	TCHAR * resStr=new wchar_t[wcslen(value)+1];
	wcscpy_s(resStr,wcslen(value)+1,value);
	m_cStrXML=resStr;
	//delete value;
	XMLString::release(&value);
	delete [] resStr;

	delete m_Parser;
	_XML_RELEASE_;
	return TRUE;
}

/* 此函数好像有问题
BOOL CBerkeleyDB::CreateDefaultIndex(BOOL bCreateIfNone)
{
	try{
		XmlIndexSpecification is = container.getIndexSpecification();

		if(bCreateIfNone) {
			std::string uri, name, index;
// 			int count=0;
// 			while(is.next(uri,name,index)) {
// 				count++;
//			}
 			if(is.next(uri,name,index)) {
 				return TRUE;
 			}
		}


		// Declare the syntax type:
		XmlValue::Type syntaxType = XmlValue::STRING;

		// Add the default index.
		is.addDefaultIndex("node-element-equality-string");

		// Save the index specification back to the container.
		XmlUpdateContext uc = myManager->createUpdateContext();
		container.setIndexSpecification(is, uc); 
	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return FALSE;
	} 
	catch(XmlException &e) {
		XmlException_DisplayError(e,__LINE__);
		return e.getExceptionCode()>0? e.getExceptionCode()*(-1):e.getExceptionCode();
	}

	return TRUE;
}
*/


BOOL CBerkeleyDB::CreateElementIndex(const std::string &NodeName, const std::string &method)
{
	try{
		XmlIndexSpecification is = container.getIndexSpecification();

		is.addIndex("", NodeName,		method);
		// Save the index specification back to the container.
		XmlUpdateContext uc = myManager->createUpdateContext();
		container.setIndexSpecification(is, uc); 

	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return FALSE;
	} 
	catch(XmlException &e) {
		XmlException_DisplayError(e,__LINE__);
		return e.getExceptionCode()>0? e.getExceptionCode()*(-1):e.getExceptionCode();
	}

	return TRUE;
}

int CBerkeleyDB::GetIndexNumber(void)
{
	std::string uri, name, index;
	int count=0;
	
	try{
		XmlIndexSpecification is = container.getIndexSpecification();

 			while(is.next(uri,name,index)) {
 				count++;
			}
	}
	catch(DbException &e) {
		DbException_DisplayError(e,__LINE__);//显示错误信息
		return 0;
	} 
	catch(XmlException &e) {
		XmlException_DisplayError(e,__LINE__);
		return e.getExceptionCode()>0? e.getExceptionCode()*(-1):e.getExceptionCode();
		return 0;
	}

	return count;
}