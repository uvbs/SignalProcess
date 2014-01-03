// Copyright (c) 2004, AOpen Inc.
// All rights reserved.
// This file is a part of the Application Class Library.
//
// [Filename]: DB_ADOClass.h
// [Comment ]: 
// [Author  ]: Leo Y.J. Chang
// [Version ]: R0.01
// [Release ]: 2004/02/03
// 
// [Contains]:
// [Related ]:
//
// ============================================================================
// History:
// ============================================================================
// 0.01   02/03/04  Leo     - Initial
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_DB_ADOCLASS_INCLUDE_
#define _INCLUDE_DB_ADOCLASS_INCLUDE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4239)
#pragma warning(disable: 4244)

#include <icrsint.h>

// 导入 ado 库 -----------------------------------------------------------
#pragma warning(disable:4146)
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF")
#pragma warning(default:4146)
using namespace ADODB;  

class CAdoConnection;
class CAdoRecordSet;
class CAdoCommand;
class CAdoParameter;

// 数值类型转换 -----------------------------------
COleDateTime vartodate(const _variant_t& var);
COleCurrency vartocy(const _variant_t& var);
bool vartobool(const _variant_t& var);
BYTE vartoby(const _variant_t& var);
short vartoi(const _variant_t& var);
long vartol(const _variant_t& var);
double vartof(const _variant_t& var);
CString vartostr(const _variant_t& var);

/*########################################################################
------------------------------------------------
CAdoConnection class
------------------------------------------------
########################################################################*/
class CAdoConnection
{
    // 构建/析构 ------------------------------------------
public:
    CAdoConnection();
    virtual ~CAdoConnection();
    
protected:
    void Release();
    
    // 属性 -----------------------------------------------
public:
    // 连接对象 ----------------------------------
    _ConnectionPtr& GetConnection() {return m_pConnection;};
    
    // 异常信息 ----------------------------------
    CString GetLastErrorText();
    ErrorsPtr GetErrors();
    ErrorPtr  GetError(long index);
    
    // 连接字串 ----------------------------------
    CString GetConnectionText() {return m_strConnect;}
    
    // 连接信息 ----------------------------------
    CString GetProviderName();
    CString GetVersion();
    CString GetDefaultDatabase();
    
    // 连接状态 ----------------------------------
    BOOL IsOpen();
    long GetState();
    
    // 连接模式 ----------------------------------
    ConnectModeEnum GetMode();
    BOOL SetMode(ConnectModeEnum mode);
    
    // 连接时间 ----------------------------------
    long GetConnectTimeOut();
    BOOL SetConnectTimeOut(long lTime = 5);
    
    // 数据源信息 -------------------------------
    _RecordsetPtr OpenSchema(SchemaEnum QueryType);
    
    // 操作 -----------------------------------------------
public:
    // 数据库连接 --------------------------------
    BOOL Open(LPCTSTR lpszConnect =_T(""), long lOptions = adConnectUnspecified);
    BOOL ConnectSQLServer(CString dbsrc, CString dbname, CString user, CString pass, long lOptions = adConnectUnspecified);
    BOOL ConnectAccess(CString dbpath, CString pass = _T(""), long lOptions = adConnectUnspecified);
    BOOL OpenUDLFile(LPCTSTR strFileName, long lOptions = adConnectUnspecified);
    void Close();
    
    // 处理 -----------------------------------------------
public:
    // 事务处理 ----------------------------------
    long BeginTrans();
    BOOL RollbackTrans();
    BOOL CommitTrans();
    
    // 执行 SQL 语句 ------------------------------
    _RecordsetPtr Execute(LPCTSTR strSQL, long lOptions = adCmdText);
    BOOL Cancel();
    
    // 数据 -----------------------------------------------
protected:
    CString			m_strConnect;
    _ConnectionPtr	m_pConnection;
};

/*########################################################################
			 ------------------------------------------------
						  CAdoRecordSet class
			 ------------------------------------------------
  ########################################################################*/

class CAdoRecordSet 
{
// 构建/折构 --------------------------------------------
public:
	CAdoRecordSet();
	CAdoRecordSet(CAdoConnection *pConnection);

	virtual ~CAdoRecordSet();
	
protected:
	void Release();

// 属性 ------------------------------------------------	
public:
	// 当前编辑状态 ----------------------------
	EditModeEnum GetEditMode();
	
	// 当前状态 --------------------------------
	BOOL IsEOF();
	BOOL IsBOF();
	BOOL IsOpen();
	long GetState();
	long GetStatus();

	// 充许返回的最大记录数 --------------------
	long GetMaxRecordCount();
	BOOL SetMaxRecordCount(long count);
	
	// 光标位置 --------------------------------
	CursorLocationEnum GetCursorLocation();
	BOOL SetCursorLocation(CursorLocationEnum CursorLocation = adUseClient);
	
	// 光标类型 --------------------------------
	CursorTypeEnum GetCursorType();
	BOOL SetCursorType(CursorTypeEnum CursorType = adOpenStatic);
	
	// 书签 --------------------------------
	_variant_t GetBookmark();
	BOOL SetBookmark(_variant_t varBookMark = _variant_t((long)adBookmarkFirst));
	
	// 当前记录位置 ------------------------
	long GetAbsolutePosition();
	BOOL SetAbsolutePosition(int nPosition);

	long GetAbsolutePage();
	BOOL SetAbsolutePage(int nPage);

	// 每页的记录数 ------------------------
	long GetPageSize();
	BOOL SetCacheSize(const long& lCacheSize);	

	// 页数 --------------------------------
	long GetPageCount();

	// 记录数及字段数 ----------------------
	long GetRecordCount();
	long GetFieldsCount();
	
	// 查询字符串 --------------------------
	CString GetSQLText() {return m_strSQL;}
	void SetSQLText(LPCTSTR strSQL) {m_strSQL = strSQL;}
	
	// 连接对象 ----------------------------
	CAdoConnection* GetConnection() {return m_pConnection;}
	void SetAdoConnection(CAdoConnection *pConnection);

	// 记录集对象 --------------------------
	_RecordsetPtr& GetRecordset();

	CString GetLastError();

// 字段属性 ----------------------------------------------
public:
	// 字段集 -------------------------------
	FieldsPtr GetFields();

	// 字段对象 -----------------------------
	FieldPtr  GetField(long lIndex);
	FieldPtr  GetField(LPCTSTR lpszFieldName);
	
	// 字段名 -------------------------------
	CString GetFieldName(long lIndex);
	
	// 字段数据类型 -------------------------
	DataTypeEnum GetFieldType(long lIndex);
	DataTypeEnum GetFieldType(LPCTSTR lpszFieldName);

	// 字段属性 -----------------------------
	long  GetFieldAttributes(long lIndex);
	long  GetFieldAttributes(LPCTSTR lpszFieldName);

	// 字段定义长度 -------------------------
	long  GetFieldDefineSize(long lIndex);
	long  GetFieldDefineSize(LPCTSTR lpszFieldName);

	// 字段实际长度 -------------------------
	long  GetFieldActualSize(long lIndex);
	long  GetFieldActualSize(LPCTSTR lpszFieldName);

	// 字段是否为NULL -----------------------
	BOOL  IsFieldNull(long index);
	BOOL  IsFieldNull(LPCTSTR lpFieldName);

// 记录更改 --------------------------------------------
public:
	BOOL AddNew();
	BOOL Update();
	BOOL UpdateBatch(AffectEnum AffectRecords = adAffectAll); 
	BOOL CancelUpdate();
	BOOL CancelBatch(AffectEnum AffectRecords = adAffectAll);
	BOOL Delete(AffectEnum AffectRecords = adAffectCurrent);
	
	// 刷新记录集中的数据 ------------------
	BOOL Requery(long Options = adConnectUnspecified);
	BOOL Resync(AffectEnum AffectRecords = adAffectAll, ResyncEnum ResyncValues = adResyncAllValues);   

	BOOL RecordBinding(CADORecordBinding &pAdoRecordBinding);
	BOOL AddNew(CADORecordBinding &pAdoRecordBinding);
	
// 记录集导航操作 --------------------------------------
public:
	BOOL MoveFirst();
	BOOL MovePrevious();
	BOOL MoveNext();
	BOOL MoveLast();
	BOOL Move(long lRecords, _variant_t Start = _variant_t((long)adBookmarkFirst));
	
	// 查找指定的记录 ----------------------
	BOOL Find(LPCTSTR lpszFind, SearchDirectionEnum SearchDirection = adSearchForward);
	BOOL FindNext();

// 查询 ------------------------------------------------
public:
	BOOL Open(LPCTSTR strSQL, long lOption = adCmdText, CursorTypeEnum CursorType = adOpenStatic, LockTypeEnum LockType = adLockOptimistic);
	BOOL Cancel();
	void Close();

	// 保存/载入持久性文件 -----------------
	BOOL Save(LPCTSTR strFileName = _T(""), PersistFormatEnum PersistFormat = adPersistXML);
	BOOL Load(LPCTSTR strFileName);
	
// 字段存取 --------------------------------------------
public:
	BOOL PutCollect(long index, const _variant_t &value);
	BOOL PutCollect(long index, const CString &value);
	BOOL PutCollect(long index, const double &value);
	BOOL PutCollect(long index, const float  &value);
	BOOL PutCollect(long index, const long   &value);
	BOOL PutCollect(long index, const DWORD  &value);
	BOOL PutCollect(long index, const int    &value);
	BOOL PutCollect(long index, const short  &value);
	BOOL PutCollect(long index, const BYTE   &value);
	BOOL PutCollect(long index, const bool   &value);
	BOOL PutCollect(long index, const COleDateTime &value);
	BOOL PutCollect(long index, const COleCurrency &value);

	BOOL PutCollect(LPCTSTR strFieldName, const _variant_t &value);
	BOOL PutCollect(LPCTSTR strFieldName, const CString &value);
	BOOL PutCollect(LPCTSTR strFieldName, const double &value);
	BOOL PutCollect(LPCTSTR strFieldName, const float  &value);
	BOOL PutCollect(LPCTSTR strFieldName, const long   &value);
	BOOL PutCollect(LPCTSTR strFieldName, const DWORD  &value);
	BOOL PutCollect(LPCTSTR strFieldName, const int    &value);
	BOOL PutCollect(LPCTSTR strFieldName, const short  &value);
	BOOL PutCollect(LPCTSTR strFieldName, const BYTE   &value);
	BOOL PutCollect(LPCTSTR strFieldName, const bool   &value);
	BOOL PutCollect(LPCTSTR strFieldName, const COleDateTime &value);
	BOOL PutCollect(LPCTSTR strFieldName, const COleCurrency &value);

	// ---------------------------------------------------------

	BOOL GetCollect(long index, CString &value);
	BOOL GetCollect(long index, double  &value);
	BOOL GetCollect(long index, float   &value);
	BOOL GetCollect(long index, long    &value);
	BOOL GetCollect(long index, DWORD   &value);
	BOOL GetCollect(long index, int     &value);
	BOOL GetCollect(long index, short   &value);
	BOOL GetCollect(long index, BYTE    &value);
	BOOL GetCollect(long index, bool   &value);
	BOOL GetCollect(long index, COleDateTime &value);
	BOOL GetCollect(long index, COleCurrency &value);

	BOOL GetCollect(LPCSTR strFieldName, CString &strValue);
	BOOL GetCollect(LPCSTR strFieldName, double &value);
	BOOL GetCollect(LPCSTR strFieldName, float  &value);
	BOOL GetCollect(LPCSTR strFieldName, long   &value);
	BOOL GetCollect(LPCSTR strFieldName, DWORD  &value);
	BOOL GetCollect(LPCSTR strFieldName, int    &value);
	BOOL GetCollect(LPCSTR strFieldName, short  &value);
	BOOL GetCollect(LPCSTR strFieldName, BYTE   &value);
	BOOL GetCollect(LPCSTR strFieldName, bool   &value);
	BOOL GetCollect(LPCSTR strFieldName, COleDateTime &value);
	BOOL GetCollect(LPCSTR strFieldName, COleCurrency &value);

	// BLOB 数据存取 ------------------------------------------
	BOOL AppendChunk(FieldPtr pField, LPVOID lpData, UINT nBytes);
	BOOL AppendChunk(long index, LPVOID lpData, UINT nBytes);
	BOOL AppendChunk(LPCSTR strFieldName, LPVOID lpData, UINT nBytes);
	BOOL AppendChunk(long index, LPCTSTR lpszFileName);
	BOOL AppendChunk(LPCSTR strFieldName, LPCTSTR lpszFileName);

	BOOL GetChunk(FieldPtr pField, LPVOID lpData);
	BOOL GetChunk(long index, LPVOID lpData);
	BOOL GetChunk(LPCSTR strFieldName, LPVOID lpData);
	BOOL GetChunk(long index, CBitmap &bitmap);
	BOOL GetChunk(LPCSTR strFieldName, CBitmap &bitmap);

// 其他方法 --------------------------------------------
public:
	// 过滤记录 ---------------------------------
	BOOL SetFilter(LPCTSTR lpszFilter);

	// 排序 -------------------------------------
	BOOL SetSort(LPCTSTR lpszCriteria);

	// 测试是否支持某方法 -----------------------
	BOOL Supports(CursorOptionEnum CursorOptions = adAddNew);

	// 克隆 -------------------------------------
	BOOL Clone(CAdoRecordSet &pRecordSet);
	_RecordsetPtr operator = (_RecordsetPtr &pRecordSet);
	
	// 格式化 _variant_t 类型值 -----------------
	
//成员变量 --------------------------------------------
protected:
	CAdoConnection     *m_pConnection;
	_RecordsetPtr		m_pRecordset;
	CString				m_strSQL;
	CString				m_strFind;
	CString				m_strFileName;
	IADORecordBinding	*m_pAdoRecordBinding;
	SearchDirectionEnum m_SearchDirection;
public:
	_variant_t			m_varBookmark;
};

//________________________________________________________________________

/*########################################################################
			------------------------------------------------
						   CAdoParameter class
			------------------------------------------------
  ########################################################################*/
class CAdoParameter
{
// 构建/折构 -------------------------------------------
public:
	CAdoParameter();
	CAdoParameter(DataTypeEnum DataType, long lSize, ParameterDirectionEnum Direction, CString strName);
	virtual ~CAdoParameter();
// 属性 ------------------------------------------------
public:
	_ParameterPtr GetParameter() {return m_pParameter;}

	// 参数精度 ---------------------------
	BOOL SetPrecision(char nPrecision);

	// 参数小数位数 -----------------------
	BOOL SetNumericScale(int nScale);

	// 参数类型 ---------------------------
	ParameterDirectionEnum GetDirection();
	BOOL SetDirection(ParameterDirectionEnum Direction);
	
	// 参数名称 ---------------------------
	CString GetName();
	BOOL SetName(CString strName);

	// 参数长度 ---------------------------
	int GetSize();
	BOOL SetSize(int size);

	// 参数据类型 -------------------------
	DataTypeEnum GetType();
	BOOL SetType(DataTypeEnum DataType);

// 方法 ------------------------------------------------
public:	
	BOOL GetValue(COleDateTime &value);
	BOOL GetValue(CString &value);
	BOOL GetValue(double &value);
	BOOL GetValue(long &value);
	BOOL GetValue(int &value);
	BOOL GetValue(short &value);
	BOOL GetValue(BYTE &value);
	BOOL GetValue(bool &value);

	BOOL SetValue(const float &value);
	BOOL SetValue(const short &value);
	BOOL SetValue(const BYTE &value);
	BOOL SetValue(const COleDateTime &value);
	BOOL SetValue(const CString &value);
	BOOL SetValue(const double &value);
	BOOL SetValue(const long &value);
	BOOL SetValue(const int &value);
	BOOL SetValue(const bool &value);
	BOOL SetValue(const _variant_t &value);

//其他方法 ------------------------------
public:
	_ParameterPtr& operator =(_ParameterPtr& pParameter);

// 数据 ------------------------------------------------
protected:
	_ParameterPtr m_pParameter;
	CString m_strName;
	DataTypeEnum m_nType;
};


/*########################################################################
			------------------------------------------------
						   CAdoCommand class
			------------------------------------------------
  ########################################################################*/

class CAdoCommand
{
// 构建/折构 -------------------------------------------
public:
	CAdoCommand();
	CAdoCommand(CAdoConnection* pAdoConnection, CString strCommandText = _T(""), CommandTypeEnum CommandType = adCmdStoredProc);
	virtual ~CAdoCommand();

// 属性 ------------------------------------------------
public:
	_variant_t GetValue(LPCTSTR lpstrName);
	_variant_t GetValue(long index);

	_ParameterPtr GetParameter(long index);
	_ParameterPtr GetParamter(LPCTSTR lpstrName);

	BOOL Append(_ParameterPtr param);

	ParametersPtr GetParameters();
	
	BOOL SetCommandTimeOut(long lTime);

	long GetState();

	BOOL SetCommandType(CommandTypeEnum CommandType);
	BOOL SetCommandText(LPCTSTR lpstrCommand);

	CAdoParameter operator [](int index);
	CAdoParameter operator [](LPCTSTR lpszParamName);

// 实现 ------------------------------------------------
public:
	_ParameterPtr CreateParameter(LPCTSTR lpstrName, DataTypeEnum Type, 
								  ParameterDirectionEnum Direction, 
								  long Size, _variant_t Value);
	_RecordsetPtr CAdoCommand::Execute(long Options = adCmdStoredProc);
	BOOL Cancel();
	
// 其他方法 --------------------------------------------
public:
	_CommandPtr& GetCommand();
	BOOL SetConnection(CAdoConnection *pConnect);

// 数据 ------------------------------------------------
protected:
	void Release();
	_CommandPtr		m_pCommand;
	CString			m_strSQL;
};


#endif // _INCLUDE_DB_ADOCLASS_INCLUDE_
