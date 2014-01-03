#pragma once
/*
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xqilla/xqilla-dom3.hpp>
#include <xqilla/xqilla-simple.hpp>


XERCES_CPP_NAMESPACE_USE
*/

//#include "ugctrl.h"

#include "MarkupMSXML.h"
#include "CStringExt.h"
#include "ErrorCode.h"
#include "AnalysisXMLDefine.h"

//////////////////////////////////////////////////////////////////////////
// 公共函数
//////////////////////////////////////////////////////////////////////////

//-------------------------定义时间转换的宏---------------------------------------
#define TIME_TO_STRING_FORMAT		_T("%Y-%m-%d %H:%M:%S")	//2005-01-10T12:30:00
#define STRING_TO_TIME_FORMAT		_T("%d-%d-%d %d:%d:%d")
#define STRING_TO_TIME_FORMAT_ST	_T("%d-%d-%dT%d:%d:%d")

extern CString TimeToXMLString(const CTime& time);
extern CTime XMLStringToTime(const CString & timeStr);
extern bool SaveXmlToFile(LPCTSTR file, LPCTSTR xmlString);


//--------分析数据的数据结构--------------------------------
class CIADataTable;
class CIADataCurve2D;




//------------------------------------------------------------------------+
//-------------------------------- XML属性定义 ----------------------------|
//------------------------------------------------------------------------+

//--------通用属性----
// #define XMLPROP_NAME		_T("_E_Name_")		//名称
// #define XMLPROP_VALUETYPE	_T("_E_Type_")		//数据类型
// #define XMLPROP_FORMAT		_T("_E_Format_")	//数据显示格式
// #define XMLPROP_UNIT		_T("_E_Unit_")		//数据单位
// #define XMLPROP_VALUE		_T("_E_Value_")		//数据(缺省)值

//--------表格属性----
// #define XMLPROP_TAB_ORDERS			_T("Orders")
// #define XMLPROP_TAB_ORDERBY			_T("OrderBy")
// #define XMLPROP_TAB_ASC				_T("Asc")

//每列属性(注：每列同时具有上面5项通用属性
// #define XMLPROP_TAB_COLUMN			_T("Column")		//列标记
// 
// #define XMLPROP_TAB_COL_TITLE		_T("Title")			//列ID
// #define XMLPROP_TAB_COL_ID			_T("ID")			//列ID
// #define XMLPROP_TAB_COL_WIDTH		_T("ColumnWidth")	//列宽度
// #define XMLPROP_TAB_COL_READONLY	_T("ReadOnly")		//true或者false,是否能编辑
// 
// #define XMLPROP_TAB_ROW				_T("Row")			//行标记


//------------------------------------------------------------------------+
//--------------------------------- XML值定义 -----------------------------|
//------------------------------------------------------------------------+
// #define XMLVALUE_TYPE_STRING	_T("string")
// #define XMLVALUE_TYPE_INT		_T("int")
// #define XMLVALUE_TYPE_FLOAT		_T("float")
// #define XMLVALUE_TYPE_TIME		_T("time")
// #define XMLVALUE_TYPE_TABLE		_T("table")
// #define XMLVALUE_TYPE_CURVE2D	_T("curve2d")

////////////////////////////////////////////////////////////////////////////////////
// CAnalysisData
////////////////////////////////////////////////////////////////////////////////////
//所有int型的返回值，如无特殊说明，都是IAERROR_XXX
//路径_T("/")表示根目录

/*
//遍历子节点
CAnalysisData ad;
ad.LoadFromFile(file);
CAnalysisData::NODE node = NULL;
CAnalysisData::static_FisrtChild(ad.GetRoot(), node);
while(node)
{
	//处理node，该node是ad.GetRoot()的子节点，如果要处理ad.GetRoot()的孙节点或超代节点，需要递归
	CAnalysisData::static_NextChild(node);
}

//遍历一个指定路径
假设路径为/A/B/C
CStringArray sa;
sa.Add(_T("A"));
sa.Add(_T("B"));
sa.Add(_T("C"));
CAnalysisData::NODE node = ad.GetRoot();
for(int i=0; i<sa.GetCount(); ++i)
{
	node = CAnalysisData::static_SubNode(node, sa.GetAt(i), false);
	if(node)//找到了，0：A；1：B；2：C
	{
	}
	else break;
}

//遍历属性
	CAnalysisData ad;
	ad.LoadFromFile(_T("D:\\PWB\\chem2009\\Debug\\Data\\Sample\\11111_#4.cg.smp"));
	CAnalysisData::NODE node = CAnalysisData::static_Path(ad.GetRoot(), _T("Sample/MethodID"), false);
	DWORD dwCount = CAnalysisData::static_PropertiesCount(node);
	for(DWORD i=0; i<dwCount; ++i)
	{
		CAnalysisData::NODE pnode = CAnalysisData::static_NodeProperty(node, i);
		CString name = CAnalysisData::static_NodeName(pnode);
		CString v;
		CAnalysisData::static_GetValue(pnode, v);
	}
//*/

class CAnalysisData
{
#ifdef _DEBUG
public:
	static bool EnableAssertionsForInfoValidating;
#endif

public:
	typedef XercesDOMParser _PARSER;
	typedef XercesDOMParser* PARSER;
	typedef const XercesDOMParser* CONST_PARSER;

	typedef xercesc_2_8::DOMDocument* DOCUMENT;
	typedef const xercesc_2_8::DOMDocument* CONST_DOCUMENT;

	typedef DOMNode* NODE;
	typedef const DOMNode* CONST_NODE;
	
    enum enumNodeType
	{
		ELEMENT_NODE                = 1,
		ATTRIBUTE_NODE              = 2,
		TEXT_NODE                   = 3,
		CDATA_SECTION_NODE          = 4,
		ENTITY_REFERENCE_NODE       = 5,
		ENTITY_NODE                 = 6,
		PROCESSING_INSTRUCTION_NODE = 7,
		COMMENT_NODE                = 8,
		DOCUMENT_NODE               = 9,
		DOCUMENT_TYPE_NODE          = 10,
		DOCUMENT_FRAGMENT_NODE      = 11,
		NOTATION_NODE               = 12
    };

	static CString static_NodeName(CONST_NODE pNode);
	static enumNodeType static_NodeType(CONST_NODE pNode);
	
	static DWORD static_ChildrenCount(CONST_NODE pParent);
	static NODE static_ChildNode(NODE pParent, DWORD dwIndex);
	static void static_FisrtChild(NODE pParent, NODE& pos);
	static NODE static_NextChild(NODE& pos);
	static void static_LastChild(NODE pParent, NODE& pos);
	static NODE static_PrevChild(NODE& pos);
	static void static_FisrtChild_Const(CONST_NODE pParent, CONST_NODE& pos);
	static CONST_NODE static_NextChild_Const(CONST_NODE& pos);
	static void static_LastChild_Const(CONST_NODE pParent, CONST_NODE& pos);
	static CONST_NODE static_PrevChild_Const(CONST_NODE& pos);
	
	static DWORD static_PropertiesCount(CONST_NODE pParent);
	static NODE static_NodeProperty(NODE pParent, DWORD dwIndex);

	static NODE static_SubNode(NODE pParent, const CString& strSubTag, bool bAutoCreate);
	static NODE static_NodeProperty(NODE pNode, const CString& strProperty);
	static NODE static_Path(NODE pRoot, LPCTSTR strXMLPath, bool bAutoCreate);
	static NODE static_Path(PARSER pParser, LPCTSTR strXMLPath, bool bAutoCreate);

	static int static_GetOuterXML(CONST_NODE pNode, CString& xml);
	static int static_GetInnerXML(CONST_NODE pNode, CString& xml);
	static int static_SetOuterXML(NODE pNode, LPCTSTR xml);
	static int static_SetInnerXML(NODE pNode, LPCTSTR xml);
	static int static_Replace(NODE pOldNode, NODE pNewNode);
	static int static_Remove(NODE pNode);//删除非根节点
	
	static PARSER static_LoadXML(LPCTSTR xml);
	static PARSER static_LoadXMLFile(LPCTSTR xmlFile);
	static int static_Save(CONST_PARSER pParser, LPCTSTR xmlFile);
	static int static_Save(CONST_NODE pNode, LPCTSTR xmlFile);
	
	static bool static_GetValue(CONST_NODE pNode, CString& value);
	static bool static_SetValue(NODE pNode, LPCTSTR value);
	static bool static_GetProperty(CONST_NODE pNode, const CString& strProperty, CString& value);
	static bool static_SetProperty(NODE pNode, const CString& strProperty, LPCTSTR value);

	static bool static_IsEqual(CONST_NODE pNode1, CONST_NODE pNode2);

	enum enumAssignMode
	{
		ASSIGN_SELF = 0X10000000,
		ASSIGN_CHILDREN = 0X20000000,

		ASSIGN_NODE_EXISTING = 0X00000001,
		ASSIGN_NODE_NOTEXISTING = 0X00000002,
		ASSIGN_NODE_ALL = 0X00000003,

		ASSIGN_PROPERTY_EXISTING = 0X00010000,
		ASSIGN_PROPERTY_NOTEXISTING = 0X00020000,
		ASSIGN_PROPERTY_ALL = 0X00030000,
	};
	static void static_Assign(NODE pNode1, CONST_NODE pNode2, DWORD dwMode = ASSIGN_CHILDREN | ASSIGN_NODE_ALL | ASSIGN_PROPERTY_ALL);

public:
	enum enumType
	{
		ADT_CHAR,
		ADT_STRING,
		ADT_INT8,
		ADT_UINT8,
		ADT_INT16,
		ADT_UINT16,
		ADT_INT32,
		ADT_UINT32,
		ADT_INT64,
		ADT_UINT64,
		ADT_FLOAT32,
		ADT_FLOAT = ADT_FLOAT32,
		ADT_FLOAT64,
		ADT_DOUBLE = ADT_FLOAT64,
		ADT_TIME,
		ADT_DATE,
		ADT_DATETIME,
		ADT_OBJECT_TABLE,	//CIADataTable
		ADT_OBJECT_CURVE2D,	//CIADataCurve2D
		ADT_ICON,
		ADT_OBJECT,			//CAnalysisData
		ADT_UNKNOWN,
	};
	enum enumFlag
	{
		//GETTING、SETTING、 MODE和FORMAT可以组合
		ADTF_VALIDATEONGETTING_SAME			= 0x00000001,//获取时验证类型，并且要求完全类型匹配，如，用long获取int或short或unsigned long就会失败
		ADTF_VALIDATEONGETTING_COMPATIBLE	= 0x00000002,//获取时验证类型，使用兼容类型匹配，如，用long获取int或short或unsigned long依然会成功
		ADTF_VALIDATEONGETTING_DISABLED		= 0x00000003,//获取时不验证类型

		ADTF_VALIDATEONSETTING_SAME			= 0x00010000,
		ADTF_VALIDATEONSETTING_COMPATIBLE	= 0x00020000,
		ADTF_VALIDATEONSETTING_DISABLED		= 0x00030000,

		ADTF_MODEONSETTING_CREATE			= 0x01000000,//当Set时，若对应的类型不存在，则创建（若未指定此掩码，则失败）

		ADTF_VALIDATE_DISABLED				= ADTF_VALIDATEONGETTING_DISABLED | ADTF_VALIDATEONSETTING_DISABLED,

		ADTF_MASK_GETTING		= 0x0000FFFF,
		ADTF_MASK_SETTING		= 0x00FF0000,

		
		ADTF_USINGFORMATONGETTING		= 0x02000000,
		ADTF_USINGFORMATONSETTING		= 0x04000000,
	};
	static LPCTSTR static_TypeString(CAnalysisData::enumType eDataType);
	static CAnalysisData::enumType static_Type(const CString& typeString);
	static CAnalysisData::enumType static_Type(CONST_NODE pInfoNode);
	static bool static_Format(CONST_NODE pInfoNode, CString& strFormat);
	static bool static_CanExtendTo(CAnalysisData::enumType eDataType1, CAnalysisData::enumType eDataType2);//eDataType1是否是eDataType2的兼容类型（eDataType1的范围是否小于eDataType2），如，ADT_INT8是ADT_INT16的兼容类型，但反过来不是
	//注意，整数并不兼容于浮点数
	
public:
	static CString static_XMLNodeNameFromPath(const CString& xmlPath);

public:
	CAnalysisData();
	virtual ~CAnalysisData();

private://禁止直接拷贝；使用GetXMLString和SetXMLString等代替
	CAnalysisData& operator = (const CAnalysisData &cad);
	CAnalysisData(const CAnalysisData &cad);

public:
	DOCUMENT GetDocument();
	CONST_DOCUMENT GetDocument() const;
	NODE GetRoot();
	CONST_NODE GetRoot() const;

public:
	int Create(const CString& xmlPath);//如果tag为空，则使用初始路径中的名称;
	int LoadFromDocument(LPCTSTR strXMLString);
	int LoadFromFile(LPCTSTR strXMLFile);
	int SaveToFile(LPCTSTR strXMLFile) const;
	
public:
	CString GetXMLString() const;
	int SetXMLString(LPCTSTR strXMLString);

	int GetOuterXML(LPCTSTR strXMLPath, CString& strXMLString) const;
	int SetOuterXML(LPCTSTR strXMLPath, LPCTSTR strXMLString);
	int GetInnerXML(LPCTSTR strXMLPath, CString& strXMLString) const;
	int SetInnerXML(LPCTSTR strXMLPath, LPCTSTR strXMLString);

	int GetValue(LPCTSTR strXMLPath, CString &strValue) const;
	int SetValue(LPCTSTR strXMLPath, LPCTSTR strValue);

	int GetProperty(LPCTSTR strXMLPath, LPCTSTR strProperty, CString &strValue) const;
	int SetProperty(LPCTSTR strXMLPath, LPCTSTR strProperty, LPCTSTR strValue);

public:
	int	GetValue_XMLString(LPCTSTR strXMLPath, CString &value) const;//得到该路径下的XML原始字串,相当于GetOuterXML
	int	SetValue_XMLString(LPCTSTR strXMLPath, LPCTSTR value);//相当于SetOuterXML

	// 以下这些函数将根据配置检查获取自动创建类型
	// 如果数据类型不匹配，返回IAERROR_XML_NOTVALID
	int	GetValue_Double(LPCTSTR strXMLPath, double &value) const;
	int	GetValue_Int(LPCTSTR strXMLPath, int &value) const;
	int	GetValue_String(LPCTSTR strXMLPath, CString &value) const;
	int	GetValue_Time(LPCTSTR strXMLPath, CTime &value) const;
	int	GetValue_Time(LPCTSTR strXMLPath, COleDateTime &value) const;
	int	GetValue_Table(LPCTSTR strXMLPath, CIADataTable &value) const;
	int	GetValue_Curve2D(LPCTSTR strXMLPath, CIADataCurve2D &value) const;
	int GetValue_AnalysisData(LPCTSTR strXMLPath, CAnalysisData &value) const;

	int	SetValue_Double(LPCTSTR strXMLPath, double value);
	int	SetValue_Int(LPCTSTR strXMLPath, int value);
	int	SetValue_String(LPCTSTR strXMLPath, LPCTSTR value);
	int	SetValue_Time(LPCTSTR strXMLPath, const CTime &value);
	int	SetValue_Time(LPCTSTR strXMLPath, const COleDateTime &value);
	int	SetValue_Table(LPCTSTR strXMLPath, const CIADataTable &value);//相当于SetOuterXML(..., value.GetXMLString())
	int	SetValue_Table2(LPCTSTR strXMLPath, const CIADataTable &value);
	int	SetValue_Curve2D(LPCTSTR strXMLPath, const CIADataCurve2D &value);//相当于SetOuterXML(..., value.GetXMLString())
	int SetValue_AnalysisData(LPCTSTR strXMLPath, const CAnalysisData &value);//相当于SetOuterXML(..., value.GetXMLString())
	

	int	GetDefaultValue_Double(LPCTSTR strXMLPath, double &value) const;
	int	GetDefaultValue_Int(LPCTSTR strXMLPath, int &value) const;
	int	GetDefaultValue_String(LPCTSTR strXMLPath, CString &value) const;
	int	GetDefaultValue_Time(LPCTSTR strXMLPath, CTime &value) const;
	int	GetDefaultValue_Time(LPCTSTR strXMLPath, COleDateTime &value) const;

	int	SetDefaultValue_Double(LPCTSTR strXMLPath, double value);
	int	SetDefaultValue_Int(LPCTSTR strXMLPath, int value);
	int	SetDefaultValue_String(LPCTSTR strXMLPath, LPCTSTR value);
	int	SetDefaultValue_Time(LPCTSTR strXMLPath, const CTime &value);
	int	SetDefaultValue_Time(LPCTSTR strXMLPath, const COleDateTime &value);

public:
	enum enumCompareResult
	{
		CR_XMLPATH_NOT_EXIST_THIS,
		CR_XMLPATH_NOT_EXIST_PASSED,
		CR_XMLPATH_NOT_EXIST_THIS_AND_PASSED,
		CR_EQUAL,
		CR_GREATER,
		CR_LESS,
	};
	//如果strXMLPathPassed为空，则使用strXMLPath
	enumCompareResult CompareValue_Double(LPCTSTR strXMLPath, const CAnalysisData& adPassed, LPCTSTR strXMLPathPassed = NULL) const;
	enumCompareResult CompareValue_Int(LPCTSTR strXMLPath, const CAnalysisData& adPassed, LPCTSTR strXMLPathPassed = NULL) const;
	enumCompareResult CompareValue_String(LPCTSTR strXMLPath, const CAnalysisData& adPassed, LPCTSTR strXMLPathPassed = NULL, bool bCaseSensitive = true) const;

public:
	void SetFlags(DWORD dwFlags){m_dwDataTypeMask = dwFlags;}
	DWORD GetFlags() const{return m_dwDataTypeMask;}
	bool SetFormat(LPCTSTR strXMLPath, LPCTSTR strFormat);

	//将xml中的指定格式的字符串转换为具体的值
	bool FromFormat_Double(LPCTSTR strXMLPath, const CString& vString, double& value) const;
	bool FromFormat_Int(LPCTSTR strXMLPath, const CString& vString, int& value) const;
	bool FromFormat_String(LPCTSTR strXMLPath, const CString& vString, CString& value) const;
	bool FromFormat_Time(LPCTSTR strXMLPath, const CString& vString, CTime &value) const;
	bool FromFormat_Time(LPCTSTR strXMLPath, const CString& vString, COleDateTime &value) const;

	//将值转换为指定xml中的指定格式的字符串
	CString	FormatValue_Double(LPCTSTR strXMLPath, double value);
	CString	FormatValue_Int(LPCTSTR strXMLPath, int value);
	CString	FormatValue_String(LPCTSTR strXMLPath, LPCTSTR value);
	CString	FormatValue_Time(LPCTSTR strXMLPath, const CTime &value);
	CString	FormatValue_Time(LPCTSTR strXMLPath, const COleDateTime &value);

public:
	bool IsEqual(const CAnalysisData& ad) const;

protected:
	virtual void OnLoadedFromDocument(){}
	virtual void OnLoadedFromFile(LPCTSTR strXMLFile){}
	virtual bool CanGet(CAnalysisData::CONST_NODE pSrcNode, CAnalysisData::enumType eGettingType) const;//eRequiredType是用户想要获取或者设置的类型，如果不允许获取或者设置，返回false
	virtual bool CanSet(CAnalysisData::NODE pSrcNode, CAnalysisData::enumType eSettingType);
	virtual CAnalysisData::CONST_NODE TypeNodeOnGetting(CAnalysisData::CONST_NODE pNode) const{return pNode;}//指定pNode从何处获取类型信息
	virtual CAnalysisData::CONST_NODE FormatNodeOnGetting(CAnalysisData::CONST_NODE pNode) const{return pNode;}//指定pNode从何处获取格式信息
	virtual CAnalysisData::NODE TypeNodeOnSetting(CAnalysisData::NODE pNode){return pNode;}//指定pNode从何处设置类型信息
	virtual CAnalysisData::NODE FormatNodeOnSetting(CAnalysisData::NODE pNode){return pNode;}//指定pNode从何处设置格式信息

private:
	int _LoadFromDocument(LPCTSTR strXMLString, bool bFireEvent);

	CString TypeOnGetting(LPCTSTR strXMLPath) const;
	CString TypeOnSetting(LPCTSTR strXMLPath);
	bool SetType(LPCTSTR strXMLPath, CAnalysisData::enumType eDataType);
	CString FormatOnGetting(LPCTSTR strXMLPath) const;
	CString FormatOnSetting(LPCTSTR strXMLPath);

protected:
	PARSER m_pParser;
	DWORD m_dwDataTypeMask;//为和以前保持兼容，默认为不起用
};

////////////////////////////////////////////////////////////////////////////////////
// CIADataTable
////////////////////////////////////////////////////////////////////////////////////
//Title:用于IATable从xml导入数据时生成标题栏，支持多行标题，具体效果参考IATable
 

//strValue1 > strValue2:return 1;
//strValue1 < strValue2:return -1;
//strValue1 = strValue2:return 0;
typedef int (*CIADATATABLE_SORTCALLBACK)(LPCTSTR strColumn, LPCTSTR strValue1, LPCTSTR strValue2);

class CIADataTable
{
protected:
	static int Default_SortCallback(LPCTSTR strColumn, LPCTSTR strValue1, LPCTSTR strValue2);

public:
	CIADataTable();
	~CIADataTable();

public:
	int Create(const CString& xmlPath);
	CString GetXMLString() const;
	int SetXMLString(LPCTSTR strXMLString);

public:
	//根据当前的排序信息排序；注意，仅当调用本函数时才会排序
	//如果cb为空，则使用默认的排序方式(string)
	bool Sort(CIADATATABLE_SORTCALLBACK cb = NULL);

public:
	//strTable:元素名称
	bool GetTable(CString* pstrTable, DWORD* pdwOrders) const;
	bool SetTable(DWORD dwOrders);
	bool GetSortInfo(DWORD dwIndex, CString& strOrderBy, bool& bAsc) const;
	bool SetSortInfo(DWORD dwIndex, LPCTSTR strOrderBy, bool bAsc);

	//Topic的必须可以作为xml属性名称
	bool GetColumns(CStringArray* pArrTopics, CStringArray* pArrTitles = NULL) const;
	bool SetColumns(const CStringArray& arrTopics, const CStringArray* pArrTitles = NULL);
	bool GetColumn(long iTopic, CString* pstrTopic, CString* pstrTitle = NULL) const;
	bool SetColumn(long iTopic, LPCTSTR strTopic, LPCTSTR strTitle = NULL);//更改已经存在的标题
	bool SetColumn(LPCTSTR strOldTopic, LPCTSTR strNewTopic, LPCTSTR strTitle = NULL);//更改已经存在的标题
	bool GetColumnInfo(long iTopic, LPCTSTR propertyName, CString& propertyValue) const;
	bool SetColumnInfo(long iTopic, LPCTSTR propertyName, LPCTSTR propertyValue);
	bool GetColumnInfo(LPCTSTR strTopic, LPCTSTR propertyName, CString& propertyValue) const;
	bool SetColumnInfo(LPCTSTR strTopic, LPCTSTR propertyName, LPCTSTR propertyValue);
	bool GetColumnInfo(long iTopic, const CStringArray& propertiesNames, CStringArray& propertiesValues) const;
	bool SetColumnInfo(long iTopic, const CStringArray& propertiesNames, const CStringArray& propertiesValues);
	bool GetColumnInfo(LPCTSTR strTopic, const CStringArray& propertiesNames, CStringArray& propertiesValues) const;
	bool SetColumnInfo(LPCTSTR strTopic, const CStringArray& propertiesNames, const CStringArray& propertiesValues);

	long AppendColumn(LPCTSTR strTopic, LPCTSTR strTitle = NULL);
	long InsertColumn(int iTopic, LPCTSTR strTopic, LPCTSTR strTitle = NULL);

	long GetColumnNumber() const;
	bool DeleteColumn(long iTopic);
	bool DeleteColumn(LPCTSTR strTopic);
	bool DeleteAllColumn();
	long GetColumnIndex(LPCTSTR strTopic) const;

	long AppendRow();
	long InsertRow(int iLine);
	long AppendRow(const CStringArray &arrayString);
	long InsertRow(int iLine, const CStringArray &arrayString);

	long GetRowsNumber() const;
	bool DeleteRow(long iLine);
	bool DeleteAllRows();

	bool GetRowValues(long iLine, CStringArray& values) const;	
	bool SetRowValues(long iLine, const CStringArray& values);

	bool GetColumnValues(long iColumn, CStringArray& values) const;
	bool GetColumnValues(LPCTSTR strColumn, CStringArray& values) const;
	bool SetColumnValues(long iColumn, const CStringArray& values);
	bool SetColumnValues(LPCTSTR strColumn, const CStringArray& values);

	bool GetCellValue(long iLine, long iColumn, CString& colValue) const;
	bool GetCellValue(long iLine, LPCTSTR strColumn, CString& colValue) const;
	bool SetCellValue(long iLine, long iColumn, LPCTSTR colValue);
	bool SetCellValue(long iLine, LPCTSTR strColumn, LPCTSTR colValue);
	
protected:
	CAnalysisData::NODE GetColumn(long iTopic) const;
	CAnalysisData::NODE GetColumn(LPCTSTR strTopic) const;
	bool GetColumn(CAnalysisData::CONST_NODE pTopic, CString* pstrTopic, CString* pstrTitle) const;
	bool SetColumn(CAnalysisData::NODE pTopic, LPCTSTR strTopic, LPCTSTR strTitle);
	bool _AppendColumn(LPCTSTR strTopic, LPCTSTR strTitle);
	CAnalysisData::NODE GetRow(long iLine) const;

protected:
	CAnalysisData m_oTable;
	friend class CAnalysisData;
};

////////////////////////////////////////////////////////////////////////////////////
// CIATable
////////////////////////////////////////////////////////////////////////////////////
// 标题生成规则：首先获取最大标题行
//				合并临近相同的标题或紧接的空标题
//假设现有Titles：<1,2,3,4,5><c1><r1,c2><r1,r2,c3><r1,r2,c4>
/*
---------------------------------------------------------------------------
| 1 |    |      r1      |
|---|    |--------------|--------------------------------------------------
| 2 |    | c2 |   r2    |
|---|    |    |---------|--------------------------------------------------
| 3 | c1 |    | c3 | c4 |
|---|    |    |    |    |--------------------------------------------------
| 4 |    |    |    |    |
|---|    |    |    |    |--------------------------------------------------
| 5 |    |    |    |    |
---------------------------------------------------------------------------
*/

#define TABLE_BITMAP_DELETE			_T("{[(BITMAP_PREDEF:_Action_Delete.bmp)]}")
#define TABLE_BITMAP_INF			_T("{[(BITMAP_PREDEF:_Action_Info.bmp)]}")
#define TABLE_BITMAP_ADD			_T("{[(BITMAP_PREDEF:_Action_Add.bmp)]}")
#define TABLE_BITMAP_ERROR			_T("{[(BITMAP_PREDEF:_Action_Error.bmp)]}")
#define TABLE_BITMAP_MANUAL_GO		_T("{[(BITMAP_PREDEF:_Action_ManualGO.bmp)]}")
#define TABLE_BITMAP_MANUAL_NEXT	_T("{[(BITMAP_PREDEF:_Action_ManualGONext.bmp)]}")
#define TABLE_BITMAP_MANUAL_OK		_T("{[(BITMAP_PREDEF:_Action_ManualOk.bmp)]}")
#define TABLE_BITMAP_AUTO_GO		_T("{[(BITMAP_PREDEF:_Action_AutoGO.bmp)]}")
#define TABLE_BITMAP_AUTO_NEXT		_T("{[(BITMAP_PREDEF:_Action_AutoGONext.bmp)]}")
#define TABLE_BITMAP_AUTO_OK		_T("{[(BITMAP_PREDEF:_Action_AutoOk.bmp)]}")
#define TABLE_BITMAP_RUNNING		_T("{[(BITMAP_PREDEF:_Action_Running.bmp)]}")
#define TABLE_BITMAP_PAUSE			_T("{[(BITMAP_PREDEF:_Action_Pause.bmp)]}")
#define TABLE_BITMAP_STOP			_T("{[(BITMAP_PREDEF:_Action_Stop.bmp)]}")
//资源ID								_T("{[(STRING_RESID:1001)]}")
//资源ID								_T("{[(BITMAP_RESID:1001)]}")
//资源ID								_T("{[(ICON_RESID:1001)]}")
//资源ID								_T("{[(CURSOR_RESID:1001)]}")
//文件								_T("{[(BITMAP_FILE: 路径)]}")//路径可以是绝对路径，也可以使用占位符
//路径占位符：[AppFolder],[...待定]
//文件								_T("{[(ICON_FILE: 路径)]}")
//文件								_T("{[(CURSOR_FILE: 路径)]}")
//data								_T("{[(BITMAP_DATA: 数据)]}")
//data								_T("{[(ICON_DATA: 数据)]}")
//data								_T("{[(CURSOR_DATA: 数据)]}")

class CIATable: public CUGCtrl
{
	static CString begin;
	static CString end;
	static CString bitmap_predef;

	static CString static_ValueFromFullTag(LPCTSTR tag);

public:
	enum enumBitmapIndex
	{
		BITMAP_NULL = 0,	//空
		BITMAP_ADD,			//添加
		BITMAP_DELETE,		//删除
		BITMAP_INF,			//信息
		BITMAP_MANUAL_GO,	//手工
		BITMAP_MANUAL_NEXT,	//准备执行
		BITMAP_MANUAL_OK,	//完成
		BITMAP_AUTO_GO,		//自动
		BITMAP_AUTO_NEXT,	//自动
		BITMAP_AUTO_OK,		//自动
		BITMAP_RUNNING,		//正在执行
		BITMAP_ERROR,		//错误
		BITMAP_PAUSE,
		BITMAP_STOP,

		BITMAP_COUNT,
	};

public:
	CIATable();
	virtual ~CIATable(void){}

public:
	int GetBitmapIndex(enumBitmapIndex eBitmap) const;
	CString GetBitmapFile(enumBitmapIndex eBitmap) const;
	LPCTSTR GetBitmapFlag(enumBitmapIndex eBitmap) const;
	const CBitmap* GetBitmapObject(enumBitmapIndex eBitmap) const;

	void SetAutoAdd_AddRow(bool bAuto){m_bAutoAdd_RowButtonAdd = bAuto;}//加一行以便添加 "增加"按钮

	BOOL CreateTable(CWnd *pcParent, CRect rect);
	BOOL SetTitle(int nCol, LPCTSTR strTitleName, int nColWidth, int nTitleNO = -1);
	void AppendRow_ButtonAdd();
	void RomoveRow_ButtonAdd();
	int GetTH_NumberRows() const{return m_GI?m_GI->m_numberTopHdgRows:-1;}
	
	LPCTSTR GetXMLBitmapFlag(int iCol, int iRow) const;
	long GetBitmapIndexFromXMLFlag(const CString& flag) const;
	void UnJoinTitles();
	void DeleteAllItems();

	int		InsertRow(long row);
	int		AppendRow();
	virtual const CIADataTable& GetNewXMLData(LPCTSTR strXMLPath);//将表格数据转换为XML,注意，除了ID、Title和列宽之外的任何Topic属性（如值类型，值格式，值名称，值单位等）将丢失
	virtual const CIADataTable& UpdateXMLData();//将表格数据转换为XML,数据必须和当前的标题定义相匹配
	virtual int SetXMLData(const CIADataTable &xmlData);//将XML数据写入表格

	enum enumResourceType
	{
		STRING,
		BITMAP_PREDEF,

		//以下内容暂不支持
// 		STRING_RESID,
// 		BITMAP_RESID,
// 		ICON_RESID,
// 		CURSOR_RESID,
// 
// 		BITMAP_FILE,
// 		ICON_FILE,
// 		CURSOR_FILE,
	};
	static CString XMLData_ParseToText(const CString& cellValueFromXml);
	static void XMLData_Parse(const CString& cellValueFromXml, CStringArray& arrContent, CArray<enumResourceType, enumResourceType>& arrType);
	static CString XMLData_ToText(const CStringArray& arrContent, const CArray<enumResourceType, enumResourceType>& arrType);

protected:
	virtual void OnSetup();
	virtual void OnSettedXMLLine(int iRow){}//SetXMLData触发
	virtual void OnSettedXMLData(){}//SetXMLData触发
	int UpdateUI_ByLocalXMLData();
	void XMLData_ApplyCellContent(int col, int row, const CString& cellContent);
	
protected:
	virtual void SetAppDataToTable() = 0;
	virtual void SetTableToAppData() = 0;

	//点击表格单元需调用的动作
	virtual void Action_AddRow()=0;
	virtual void Action_DeleteRow(int nRow) = 0;
	virtual void Action_DisplayRowInf(int nRow) = 0;
	//	virtual void Action_RunRow(int nRow) = 0;

	virtual void Action_Manual_Finished(int nRow, int nColumn){/*AfxMessageBox(_T("手工任务已完成，显示完成后的任务结论。用户可选择重新执行操作！")); */}
	virtual void Action_Manual_RunNext(int nRow, int nColumn){AfxMessageBox(_T("用户点击了下一步执行的按钮，请实现该段代码！"));}
	virtual void Action_Manual_Run(int nRow, int nColumn){/*AfxMessageBox(_T("用户点击了执行的按钮，但没有按顺序执行，请实现该段代码！\n\n原来的Action_RunRow函数"));*/}

	virtual void Action_Auto_Finished(int nRow, int nColumn){/*AfxMessageBox(_T("自动任务已完成，显示完成后的任务结论。"));*/}
	virtual void Action_Auto_Run(int nRow, int nColumn){AfxMessageBox(_T("该任务自动执行，不能人工操作！"));}
	virtual void Action_Auto_RunNext(int nRow, int nColumn){AfxMessageBox(_T("该任务自动执行，不能人工操作！"));}

	virtual void Action_Running(int nRow, int nColumn){AfxMessageBox(_T("用户点击的该项正在执行操作！"));}
	virtual void Action_Error(int nRow, int nColumn){AfxMessageBox(_T("用户点击的该出错，显示错误信息！"));}

	//鼠标左击的通用操作定义
	virtual void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);
	virtual  int OnCellTypeNotify(long ID, int col,long row, long msg, long param); 

	//改变选中行背景颜色
	virtual void ChangeRowBackGroundColor(long colNum, long rowNum, long currRow);
	virtual void ChangeSelectedBKColor(long colNum, long rowNum);
	virtual void ChangeCurrentBKColor(long colNum, long currRow);

	virtual int  OnCanSizeCol(int col);

public:
	CFont* IA_GetTitleFont(){return GetFont(m_nFontID_Title);}
	CFont* IA_GetContentFont(){return GetFont(m_nFontID_Content);}

protected:
	int		m_nFontID_Title;
	int		m_nFontID_Content;			//表格中的正文字体
	COLORREF m_colorContent;
	COLORREF m_colorBackGround;
	bool	m_DislplayFirstColumn;
	bool	m_bAutoAdd_RowButtonAdd;	//在每次RedrawAll()的时候自动加入　增加行
	int		m_nAdditionRow;				//增加的功能性行数(在最后),计算总行数的时候要扣掉
	int		m_pBitmaps[BITMAP_COUNT];
	CString ErrMsg;
	CIADataTable	m_dataTable;
	bool	m_bIsSettingup;

private:
	void ApplyIADataTable_CellProperties(long row);
};
 
/*
class CIADataCurve3D
{

};*/

//数据保存格式
/*
<Sample>
	<Result>
		<CurveDatas>
			<CaptionDefine>
				<ResultPoints>结果曲线数据</ResultPoints>
			</CaptionDefine>

			<CurveData Type='ResultPoints' count="10" m_dwSecondsPrev="0" nDrawType="1">
				<Item index="6" name="H2O">n002_H2O.csv</Item>
				<Item index="7" name="N2">n002_N2.csv</Item>
				<Item index="8" name="O2">n002_O2.csv</Item>
				<Item index="9" name="CO2">n002_CO2.csv</Item>
			</CurveData>


			<CurveData Caption="离子量曲线数据" Type='M'  count="10" m_dwSecondsPrev="0" nDrawType="1">
				<Item index="2" name="M18" type="M">n002_M18.csv</Item>
				<Item index="3" name="M28" type="M">n002_M28.csv</Item>
				<Item index="4" name="M32" type="M">n002_M32.csv</Item>
				<Item index="5" name="M44" type="M">n002_M44.csv</Item>
			</CurveData>


			<CurveData Caption="电流强度曲线数据" Type='FL'  count="10" m_dwSecondsPrev="0" nDrawType="1">
				<Item index="0" name="FL" type="FL">n002_FL.csv</Item>
			</CurveData>


			<CurveData Caption="总离子强度曲线数据" Type='I_Total'  count="10" m_dwSecondsPrev="0" nDrawType="1">
				<Item index="1" name="I_Total" type="I_Total">n002_I_Total.csv</Item>
			</CurveData>
		</CurveDatas>
	</Result>
</Sample>

rangeMin,rangeMax:样品曲线的显示范围
//*/
class CIADataCurve2DSingle;
class CIADataCurve2D : CObject// : public CIADataXML
	//仅提供Curve2D的xml操作
{
public:
	static LPCTSTR DefaultXML(){return _T("<CurveDatas><CaptionDefine></CaptionDefine></CurveDatas>");}
	static LPCTSTR Path_CaptionDefine(){return _T("CurveDatas/CaptionDefine");}

public:
	CIADataCurve2D(LPCTSTR cStrXMLPath = _T("")):m_cStrXMLPath(cStrXMLPath),m_bInit(false){SetXMLString(DefaultXML());}
	virtual ~CIADataCurve2D(){}

public:
	CString m_cStrXMLPath;	//XML路径
	int SetXMLPath(const CString &cStrXMLPath){m_cStrXMLPath=cStrXMLPath; return IAERROR_SUCCESS;}

public:
	int SetXMLString(LPCTSTR cXMLString);//如果要新建，设置cXMLString为NULL或者"<CurveDatas></CurveDatas>"
	CString GetXMLString() const;

	//CString GetProperty(LPCTSTR prop);
	//bool SetProperty(LPCTSTR prop, LPCTSTR val);

	CString GetCurveCaption(LPCTSTR type) const;
	void SetCurveCaption(LPCTSTR type, LPCTSTR caption);//必须确保Path_CaptionDefine()的路径已经存在
	
	bool GetCurve2DSingle(LPCTSTR type, CIADataCurve2DSingle& single) const;
	bool AutoCurve2DSingle(LPCTSTR type, CIADataCurve2DSingle& single);

	void BeginEnum();//尽量使用const版本
	bool GetNext(CString& type, CIADataCurve2DSingle& single);//尽量使用const版本
	
	void BeginEnum(long& pos) const;
	bool GetNext(long& pos, CString& type, CIADataCurve2DSingle& single) const;

	bool QuickGetCurve2DFile(LPCTSTR type, LPCTSTR name, CString& filename) const;
	bool QuickSetCurve2DFile(LPCTSTR type, LPCTSTR name, LPCTSTR filename);
	bool QuickDeleteCurve2DFile(LPCTSTR type, LPCTSTR name);

public:
	bool m_bInit;

protected:
	CTWenXMLBase xml;
	int m_iEnum;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CIADataCurve2DSingle
// 功能：单个曲线数据信息处理，格式：
/*
<CurveData Type='ResultPoints' count="10" m_dwSecondsPrev="0" nDrawType="1">
	<Item index="6" name="H2O">n002_H2O.csv</Item>
	<Item index="7" name="N2">n002_N2.csv</Item>
	<Item index="8" name="O2">n002_O2.csv</Item>
	<Item index="9" name="CO2">n002_CO2.csv</Item>
</CurveData>
//*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CIADataCurve2DSingle : CObject// : public CIADataXML
	//仅提供Curve2D的xml操作
{
public:
	CIADataCurve2DSingle():m_pSingle(NULL){}

public:
	bool ItemExist(LPCTSTR name);

	bool GetItemByName(LPCTSTR name, CString& fileName);

	void SetItemByName(LPCTSTR name, LPCTSTR fileName);

	void AppendItem(LPCTSTR name, LPCTSTR fileName);

	bool DeleteItemByName(LPCTSTR name);

	bool GetProperty(LPCTSTR prop, CString& val);
	bool SetProperty(LPCTSTR prop, LPCTSTR val);

	void BeginEnum();
	bool GetNext(CString& name, CString& fileName);
	bool SetEnum(LPCTSTR fileName);
	bool GetEnumProperty(LPCTSTR prop, CString& val) const;
	bool SetEnumProperty(LPCTSTR prop, LPCTSTR val);

	int GetCount() const{return m_pSingle?m_pSingle->Children.GetCount():-1;}

protected:
	static CString strType;
	CTWenXMLSingle* m_pSingle;
	friend class CIADataCurve2D;
	CTWenXMLSingle* GetItem(LPCTSTR name) const;
	int m_iEnum;
	CTWenXMLSingle* m_pEnumSingle;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CIAFileCurve2D
// 功能：点数据处理
// 1.保存数据：Open();AppendPoint()
// 2.枚举所有点：Open();BeginEnum();while(GetNext()) ;
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CIAFileCurve2D
	//仅提供Curve2D的数据文件操作
{
public:
	CIAFileCurve2D():m_pcChars(NULL){m_file.m_hFile = CFile::hFileNull;m_Ext.SetSplitString(_T('\n'));}
	~CIAFileCurve2D(){Close();}
public:
	BOOL Open(LPCTSTR file);//数据文件全路径，是一个以csv为后缀名的csv格式的文件

	BOOL ApppendPoint(double x, double y);//x,y保存于单独的行

	void BeginEnum();
	bool GetNext(double& x, double& y, CTime& time);//time目前无效

	void Close();


	CStringExt m_Ext;

protected:
	CFile m_file;
	static char strSep;//csv分割字符

	static bool Read(CFile& file,CString& content,char token);
	char* m_pcChars;
};