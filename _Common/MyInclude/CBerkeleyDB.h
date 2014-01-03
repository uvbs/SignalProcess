#pragma once

#include "dbxml/DbXml.hpp"
#include "KMDB_Error.h"

using namespace DbXml;

#define MAX_ALIASNAME		50

#ifndef KMDB_ITEMP_ID
	#define KMDB_ITEMP_ID			_T("id")
#endif
#ifndef KMDB_ID_SECURITY_INF
	#define KMDB_ID_SECURITY_INF	_T("_SECURITY_INF_")	//安全信息
#endif


void DbException_DisplayError(DbException  &e,int nLine);
void XmlException_DisplayError(XmlException &e,int nLine);
void BDBErrorCallback(const DbEnv *dbenv, const char *errpfx, const char *msg);



class __declspec(dllexport)  COneLevelXMLData
{
public:
	COneLevelXMLData(void):	m_cStrXML(_T("<Item></Item>")){}
	virtual ~COneLevelXMLData(void){}


//	void InitWithDefaultData(void){m_cStrXML = KMDB_DEFAULT_FILEXMLDATA;}
//	void InitWithCurrencyData(void){m_cStrXML = KMDB_CURRENCY_FILEXMLDATA;}

	//支持压缩功能前用于存放XML数据，将来数据储存用压缩后的数据替代
	CString m_cStrXML;

	//
	//设置替换整个XML数据
	void operator =(const COneLevelXMLData &other){m_cStrXML=other.m_cStrXML;}
	BOOL ReplaceWith(const TCHAR* strXML);	//可能strXML数据格式不合法，返回FALSE
	void Clear(void){COneLevelXMLData empty;*this=empty;};	//清除所有数据
	//
	//-----------以下函数用于添加、修改、删除XML的各数据项---------------------------------------
	//
	//更新部分或全部数据。将传入的数据中存在的项更新自身的数据，没有的项填加到自身的数据中。
	virtual void Data_Update(COneLevelXMLData &other);
	virtual BOOL Data_Update(const TCHAR* strXML);	//可能strXML数据格式不合法，返回FALSE.strXML必须包括<Item><\Item>
	//
	//设置单个项的值，不存在的项进行添加，存在的进行替换。
	virtual int Data_Item_Set(const TCHAR* strName,const TCHAR* strProperty, const TCHAR* strValue);
	//删除某一项，包括一个名字有多项的情况
	BOOL Data_Item_Del(const TCHAR* strName);

	//该函数仅针对没有属性的项设置，支持多个项同时存在。
	int Data_MultiItem_Set(CString cStrName, CString cStrValue);
	int Data_MultiItem_Set(CString cStrName, const CStringArray &arrayProperty, CString cStrValue);


	//------------------------------------------------------------------------------
	//
	//-----------以下函数用于获得XML的各数据项---------------------------------------
	//
	//
	//返回整个XML数据
	BOOL GetXMLString(TCHAR* &strXML);		//调用者负责释放指针
	void GetXMLString(CString &cStrXML){cStrXML=m_cStrXML;}
	CString GetXMLString(void){return m_cStrXML;}

	//得到部分项目的值
	/*
	*   当数量大于１的时候,返回值arrayRes肯定大于１，只是各个arrayRes[i]可能为_T("")，
	*	0—表示成功
	*	3-- 输入XML包ItemXML格式错误
	*	15—未知错误
	*	16—该XML元素不存在
	*	17—该XML属性不存在
	*	18—值为空
	*/
	//返回值存放在arrayRes数组类中
	int GetData(const wchar_t* strName, const wchar_t* strProperty, CStringArray &arrayRes) const;
	CString GetData(const wchar_t* strName, const wchar_t* strProperty) const;


	//返回文件的编号
	CString GetItemID(void) const; 


	/*设置文件编号
	*	0-成功
	*	-1 XML 不合法
	*	-2 XML 解析错误
	*/ 
	int SetItemID(CString id);

	//把ID=..的属性项去掉,主要是为了王八蛋程序AddDocument()
	void EraseItemID(void){	if(SetItemID(_T("weoqrhcniwe8432nsvbs"))==KMERROR_SUCCESS){
		CString cStrTemp = KMDB_ITEMP_ID; cStrTemp+=_T("=\"weoqrhcniwe8432nsvbs\"");
		VERIFY(m_cStrXML.Replace(cStrTemp, _T(""))!=0);
	}
	}
};


typedef CArray<COneLevelXMLData, COneLevelXMLData &> ARRAY_OLXDATA;



class __declspec(dllexport) CBerkeleyDB
{
protected:
	//	static DbEnv * m_DBEnv;
	DbEnv * m_DBEnv;
	char  m_szAliasName[MAX_ALIASNAME];

public:
	CBerkeleyDB(void);
	~CBerkeleyDB(void);

protected:
	CString		m_cStrBDBName;
	XmlContainer container;
	BOOL opened;

public:
	XmlManager* myManager;
	XmlContainer *GetContainer(void){return &container;}

	virtual BOOL Init(const wchar_t * dbpath, const char *pszAlias); 

	BOOL CBerkeleyDB::OpenBDBOtherPlace(const wchar_t *dbpath, const char *pszAlias, BOOL bCreateIfNotExist);

	BOOL close();
	BOOL IsOpen(void){return opened;}

	/*	Add document to container
	*	the document should be well formated and incoding
	*/
	virtual int AddDocument(const wchar_t *str,wchar_t *& pk);

	virtual int AddDocumentByKey(const wchar_t * key,const wchar_t *str,wchar_t *& pk);

	/*
	*	Do update documents
	*	return:
	*		-1	db not opened, use init first
	*		-2	failed 
	*		>=0	the number of document counts have been modified
	*/
	virtual int DoUpdate(const wchar_t * docName,const wchar_t * node_path,const wchar_t * value);

	virtual BOOL DoUpdateDocument(const wchar_t * docName, const wchar_t * value);

	//当数据库变化时调用
	//nAction: -1 删除  0 改变  1 增加
	//继承类使用
	virtual void ContentChanged(const wchar_t *szKey, int nAction){}
	virtual int GetDBItemVersion(const wchar_t *szKey){ASSERT(FALSE);return KMERROR_UNKNOWN;}

	/*
	*	Get specyfied item
	*	0—表示成功
	*	3-- 输入XML包reswchar格式错误
	*	7—该编号不存在
	*	15-未知错误
	*/
	virtual int GetAItem(const wchar_t *szFileID, wchar_t* &resItem);
	virtual int GetAItem(const wchar_t *szFileID, CString &resItem);
	//		int GetAItem(const wchar_t *szFileID, CString &cStrRes);
	virtual int GetAllItems( ARRAY_OLXDATA * arrayResult );

	/*
	*	Delete Speicyfied Document
	*	0-表示成功
	*	其他-错误
	*/
	int DeleteDoc(const wchar_t * key);

	/*	doquery
	*	query should be a xquery string for search
	*	Return format as "<SearchRes><SRItem>result</SRItem></SearchRes>"
	*  to free reswchar, new need user "delete []"
	*  
	*  返回：0--正常；-1--数据库未打开　正数--记录数 <-1--记录数，但是返回失败reswchar为空
	*/
	int DoQuery(const wchar_t * str,wchar_t* &reswchar);
	int DoQuery(const wchar_t *str, ARRAY_OLXDATA *arrayResult);

	/*	Search
	*	返回: 0-正常;	-1--数据库未打开 -2---查询条件不能为空	-3--没有找到匹配条目
	*/
	virtual int SearchDocuments(const CString& tag,const CString& searchStr, ARRAY_OLXDATA& arrayRes,BOOL blur=FALSE, BOOL bCaseInsensitive=TRUE);
	//	virtual int SearchDocumentsBySort(const CString & searchStr,ARRAY_OLXDATA& arrayRes, BOOL bIncludeSub=FALSE){return 255;}
	//	virtual int SearchDocumentsByKeyWord(const CString & searchStr,ARRAY_OLXDATA& arrayRes){return 255;}
	//	virtual int SearchDocumentsByTime(const CTime & from,const CTime & to,ARRAY_OLXDATA& arrayRes,int type=1){return 255;}


	//该函数用在第一次建立DB后，向其写入初始化数据
	virtual int InitNewDB(CString cStrKMDBName);

	//建立标准索引
//	virtual BOOL CreateDefaultIndex(BOOL bCreateIfNoe=TRUE);
	virtual BOOL CreateElementIndex(const std::string &NodeName, const std::string &method);
	int GetIndexNumber(void);
};
