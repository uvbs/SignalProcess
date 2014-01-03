#include "stdafx.h"
#include "AnalysisData.h"

#include "CStringConvert.h"
#include "CStringExt.h"
#include "CApplication.h"
#include <regex>

#if defined(UNICODE) || defined(_UNICODE)
#define CRegex std::tr1::wregex
#else
#define CRegex std::tr1::regex
#endif

//////////////////////////////////////////////////////////////////////////
// 公共函数
//////////////////////////////////////////////////////////////////////////
CString TimeToXMLString(const CTime& time)
{
	return time.Format(TIME_TO_STRING_FORMAT);
}
CTime XMLStringToTime(const CString & timeStr)
{
	int   nYear,nMonth,nDate,nHour,nMin,nSec;

	if(timeStr.Find(_T('T'))>1)
		swscanf_s(timeStr, STRING_TO_TIME_FORMAT_ST, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	else 
		swscanf_s(timeStr, STRING_TO_TIME_FORMAT, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	
	return CTime(nYear,nMonth,nDate,nHour,nMin,nSec);
}
bool SaveXmlToFile(LPCTSTR file, LPCTSTR xmlString)
{
	if(file == NULL || xmlString == NULL || _tcslen(file) == 0) return false;
	CoInitialize(NULL);
	IXMLDOMDocumentPtr pDOMDocument;
	CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDOMDocument);

	CString str(xmlString);
	BSTR bstrText = str.AllocSysString(); 

	VARIANT_BOOL ret;
	pDOMDocument->loadXML(bstrText, &ret);
	SysFreeString(bstrText);

	if(ret == 0) return false;

	return pDOMDocument->save(COleVariant(file)) == S_OK;
}

////////////////////////////////////////////////////////////////////////////////////
// CAnalysisData
////////////////////////////////////////////////////////////////////////////////////

LPCTSTR CAnalysisData::static_TypeString(CAnalysisData::enumType eDataType)
{
	switch(eDataType)
	{
	case ADT_CHAR:return _T("Char");
	case ADT_STRING:return XMLVALUE_TYPE_STRING;
	case ADT_INT8:
	case ADT_UINT8:
	case ADT_INT16:
	case ADT_UINT16:
	case ADT_INT32:
	case ADT_UINT32:return XMLVALUE_TYPE_INT;
	case ADT_INT64:return _T("Int64");
	case ADT_UINT64:return _T("UInt64");
	case ADT_FLOAT32:
	case ADT_FLOAT64:return XMLVALUE_TYPE_FLOAT;
	case ADT_TIME:
	case ADT_DATE:
	case ADT_DATETIME:return XMLVALUE_TYPE_TIME;
	case ADT_OBJECT_TABLE:return XMLVALUE_TYPE_TABLE;
	case ADT_OBJECT_CURVE2D:return XMLVALUE_TYPE_CURVE2D;
	case ADT_OBJECT:return _T("Object");
	case ADT_UNKNOWN:
	default:return _T("UnknownDataType");
	}
}
CAnalysisData::enumType CAnalysisData::static_Type(const CString& typeString)
{
	if(typeString.IsEmpty()) return ADT_UNKNOWN;
	if(typeString == _T("Char")) return ADT_CHAR;
	if(typeString == XMLVALUE_TYPE_STRING) return ADT_STRING;
	if(typeString == XMLVALUE_TYPE_INT) return ADT_INT32;
	if(typeString == XMLVALUE_TYPE_FLOAT) return ADT_FLOAT64;
	if(typeString == _T("Int64")) return ADT_INT64;
	if(typeString == _T("UInt64")) return ADT_UINT64;
	if(typeString == XMLVALUE_TYPE_TIME) return ADT_DATETIME;
	if(typeString == XMLVALUE_TYPE_TABLE) return ADT_OBJECT_TABLE;
	if(typeString == XMLVALUE_TYPE_CURVE2D) return ADT_OBJECT_CURVE2D;
	if(typeString == XMLVALUE_TYPE_ICON) return ADT_ICON;
	if(typeString == _T("Object")) return ADT_OBJECT;
	return ADT_UNKNOWN;
}
CAnalysisData::enumType CAnalysisData::static_Type(CONST_NODE pInfoNode)
{
	CString type;
	if(!static_GetProperty(pInfoNode, XMLPROP_VALUETYPE, type)) return ADT_UNKNOWN;
	return static_Type(type);
}
bool CAnalysisData::static_Format(CONST_NODE pInfoNode, CString& strFormat)
{
	return static_GetProperty(pInfoNode, XMLPROP_FORMAT, strFormat);
}
bool CAnalysisData::static_CanExtendTo(CAnalysisData::enumType eDataType1, CAnalysisData::enumType eDataType2)
{
	if(eDataType1 == ADT_UNKNOWN) return false;
	switch(eDataType1)
	{
	case ADT_CHAR:
		if(eDataType2 == ADT_CHAR) return true;//不要break;
	case ADT_STRING:
		return eDataType2 == ADT_STRING;
		break;
	case ADT_UINT8:
		if(eDataType2 == ADT_UINT8) return true;//不要break;
	case ADT_INT8:
		if(eDataType2 == ADT_INT8 ) return true;//不要break;
	case ADT_UINT16:
		if(eDataType2 == ADT_UINT16) return true;//不要break;
	case ADT_INT16:
		if(eDataType2 == ADT_INT16) return true;//不要break;
	case ADT_UINT32:
		if(eDataType2 == ADT_UINT32) return true;//不要break;
	case ADT_INT32:
		if(eDataType2 == ADT_INT32) return true;//不要break;
	case ADT_UINT64:
		if(eDataType2 == ADT_UINT64) return true;//不要break;
	case ADT_INT64:
		return eDataType2 == ADT_INT64 || eDataType2 == ADT_STRING;
		break;
	case ADT_FLOAT32:
		if(eDataType2 == ADT_FLOAT32) return true;//不要break;
	case ADT_FLOAT64:
		return eDataType2 == ADT_FLOAT64 || eDataType2 == ADT_STRING;
		break;
	case ADT_TIME:
		return (eDataType2 == ADT_TIME) || (eDataType2 == ADT_DATETIME);
		break;
	case ADT_DATE:
		if(eDataType2 == ADT_DATE) return true;//不要break;
	case ADT_DATETIME:
		return eDataType2 == ADT_DATETIME || eDataType2 == ADT_STRING;
		break;
	case ADT_OBJECT_TABLE:
		return (eDataType2 == ADT_OBJECT_TABLE) || (eDataType2 == ADT_OBJECT);
		break;
	case ADT_OBJECT_CURVE2D:
		if(eDataType2 == ADT_OBJECT_CURVE2D) return true;//不要break;
	case ADT_OBJECT:
		return eDataType2 == ADT_OBJECT;
		break;
	default:
#ifdef _DEBUG
		if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
		return false;
	}
}

CString CAnalysisData::static_XMLNodeNameFromPath(const CString& xmlPath)
{
	CString t = xmlPath;
	t.Replace(_T('/'), _T('\\'));
	if(t.IsEmpty()) return _T("");
	if(t.GetAt(t.GetLength()-1) == _T('\\')) t.Delete(t.GetLength()-1);
	CStringExt ext(t, _T("\\"));
	DWORD dwCount = ext.GetCount();
	CString ret;
	ext.GetEnum(dwCount-1, ret);
	return ret;
}

bool CAnalysisData::CanGet(CAnalysisData::CONST_NODE pSrcNode, CAnalysisData::enumType eGettingType) const
{
	if(!pSrcNode)
	{
#ifdef _DEBUG
		if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
		return false;
	}
	switch(m_dwDataTypeMask & ADTF_MASK_GETTING)
	{
	case ADTF_VALIDATEONGETTING_SAME:
		{
			CAnalysisData::enumType eSourceType = static_Type(TypeNodeOnGetting(pSrcNode));
			if(eSourceType == ADT_UNKNOWN)
			{
#ifdef _DEBUG
				if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
				return false;
			}
			return eSourceType == eGettingType;
		}
	case ADTF_VALIDATEONGETTING_COMPATIBLE:
		return static_CanExtendTo(static_Type(TypeNodeOnGetting(pSrcNode)), eGettingType);
	case ADTF_VALIDATEONGETTING_DISABLED:return true;
	default:
#ifdef _DEBUG
		if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
		return false;
	}
}
bool CAnalysisData::CanSet(CAnalysisData::NODE pSrcNode, CAnalysisData::enumType eSettingType)
{
	if(!pSrcNode)
	{
#ifdef _DEBUG
		if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
		return false;
	}
	switch(m_dwDataTypeMask & ADTF_MASK_SETTING)
	{
	case ADTF_VALIDATEONSETTING_SAME:
		{
			CAnalysisData::enumType eSourceType = static_Type(TypeNodeOnSetting(pSrcNode));
			if(eSourceType == ADT_UNKNOWN)
			{
				if(m_dwDataTypeMask & ADTF_MODEONSETTING_CREATE)
				{
					return static_SetProperty(TypeNodeOnSetting(pSrcNode), XMLPROP_VALUETYPE, static_TypeString(eSettingType));
				}
				else
				{
#ifdef _DEBUG
					if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
					return false;
				}
			}
			return eSourceType == eSettingType;
		}
	case ADTF_VALIDATEONSETTING_COMPATIBLE:
		{
			CAnalysisData::enumType eSourceType = static_Type(TypeNodeOnSetting(pSrcNode));
			if((eSourceType == ADT_UNKNOWN) && (m_dwDataTypeMask & ADTF_MODEONSETTING_CREATE))
			{
				return static_SetProperty(TypeNodeOnSetting(pSrcNode), XMLPROP_VALUETYPE, static_TypeString(eSettingType));
			}
			return static_CanExtendTo(eSettingType, eSourceType);//注意设置和读取的兼容方向不同
		}
	case ADTF_VALIDATEONSETTING_DISABLED:return true;
	default:
#ifdef _DEBUG
		if(EnableAssertionsForInfoValidating) ASSERT(false);
#endif
		return false;
	}
}

CString CAnalysisData::TypeOnGetting(LPCTSTR strXMLPath) const
{
	CString type;
	static_GetProperty(TypeNodeOnGetting(static_Path(m_pParser, strXMLPath, false)), XMLPROP_VALUETYPE, type);
	return type;
}
CString CAnalysisData::TypeOnSetting(LPCTSTR strXMLPath)
{
	CString type;
	static_GetProperty(TypeNodeOnSetting(static_Path(m_pParser, strXMLPath, false)), XMLPROP_VALUETYPE, type);
	return type;
}
bool CAnalysisData::SetType(LPCTSTR strXMLPath, CAnalysisData::enumType eDataType)
{
	return static_SetProperty(TypeNodeOnSetting(static_Path(m_pParser, strXMLPath, true)), XMLPROP_VALUETYPE, static_TypeString(eDataType));
}
CString CAnalysisData::FormatOnGetting(LPCTSTR strXMLPath) const
{
	CString strFormat;
	static_Format(FormatNodeOnGetting(static_Path(m_pParser, strXMLPath, false)), strFormat);
	return strFormat;
}
CString CAnalysisData::FormatOnSetting(LPCTSTR strXMLPath)
{
	CString strFormat;
	static_Format(FormatNodeOnSetting(static_Path(m_pParser, strXMLPath, false)), strFormat);
	return strFormat;
}
bool CAnalysisData::SetFormat(LPCTSTR strXMLPath, LPCTSTR strFormat)
{
	return static_SetProperty(FormatNodeOnSetting(static_Path(m_pParser, strXMLPath, true)), XMLPROP_FORMAT, strFormat);
}

#ifdef _DEBUG
bool CAnalysisData::EnableAssertionsForInfoValidating = true;
#endif

CAnalysisData::CAnalysisData():m_pParser(NULL),m_dwDataTypeMask(ADTF_VALIDATE_DISABLED)
{
	try
	{    
		XMLPlatformUtils::Initialize();
	}
	catch(xercesc::XMLException & excp) 
	{
		TRACE("错误:XMLPlatformUtils::Initialize().\n");
	}
}
CAnalysisData::~CAnalysisData()
{
	if(m_pParser)
	{
		delete m_pParser;
		m_pParser = NULL;
	}
	XMLPlatformUtils::Terminate();
}

CAnalysisData::DOCUMENT CAnalysisData::GetDocument()
{
	return m_pParser?m_pParser->getDocument():NULL;
}
CAnalysisData::CONST_DOCUMENT CAnalysisData::GetDocument() const
{
	return m_pParser?m_pParser->getDocument():NULL;
}
CAnalysisData::NODE CAnalysisData::GetRoot()
{
	DOCUMENT pDocument = GetDocument();
	return pDocument?pDocument->getDocumentElement():NULL;
}
CAnalysisData::CONST_NODE CAnalysisData::GetRoot() const
{
	CONST_DOCUMENT pDocument = GetDocument();
	return pDocument?pDocument->getDocumentElement():NULL;
}

int CAnalysisData::Create(const CString& xmlPath)
{
	CString s(static_XMLNodeNameFromPath(xmlPath));
	if(s.IsEmpty())
	{
		ASSERT(false);
		return IAERROR_SYSTEMERROR;
	}
	CString initXml;
	initXml.Format(_T("<%s></%s>"), s, s);
	return _LoadFromDocument(initXml, false);
}
int CAnalysisData::_LoadFromDocument(LPCTSTR strXMLString, bool bFireEvent)
{
	if(m_pParser) delete m_pParser;
	m_pParser = static_LoadXML(strXMLString);
	if(!m_pParser) return IAERROR_XML_NOTVALID;
	if(bFireEvent) OnLoadedFromDocument();
	return IAERROR_SUCCESS;
}
int CAnalysisData::LoadFromDocument(LPCTSTR strXMLString)
{
	return _LoadFromDocument(strXMLString, true);
}
int CAnalysisData::LoadFromFile(LPCTSTR strXMLFile)
{
	if(m_pParser) delete m_pParser;
	m_pParser = static_LoadXMLFile(strXMLFile);
	if(!m_pParser) return IAERROR_XML_NOTVALID;
	OnLoadedFromFile(strXMLFile);
	return IAERROR_SUCCESS;
}
int CAnalysisData::SaveToFile(LPCTSTR strXMLFile) const
{
	return static_Save(m_pParser, strXMLFile);
}

CString CAnalysisData::GetXMLString() const
{
	CString strRet;
	GetOuterXML(_T(""), strRet);
	return strRet;
}
int CAnalysisData::SetXMLString(LPCTSTR strXMLString)
{
	return LoadFromDocument(strXMLString);
}

int CAnalysisData::GetOuterXML(LPCTSTR strXMLPath, CString& strXMLString) const
{
	return static_GetOuterXML(static_Path(m_pParser, strXMLPath, false), strXMLString);
}
int CAnalysisData::SetOuterXML(LPCTSTR strXMLPath, LPCTSTR strXMLString)
{
	return static_SetOuterXML(static_Path(m_pParser, strXMLPath, true), strXMLString);
}
int CAnalysisData::GetInnerXML(LPCTSTR strXMLPath, CString& strXMLString) const
{
	return static_GetInnerXML(static_Path(m_pParser, strXMLPath, false), strXMLString);
}
int CAnalysisData::SetInnerXML(LPCTSTR strXMLPath, LPCTSTR strXMLString)
{
	return static_SetInnerXML(static_Path(m_pParser, strXMLPath, true), strXMLString);
}

int CAnalysisData::GetValue(LPCTSTR strXMLPath, CString &strValue) const
{
	return static_GetValue(static_Path(m_pParser, strXMLPath, false), strValue)?IAERROR_SUCCESS:IAERROR_XML_NOTVALID;
}
int CAnalysisData::SetValue(LPCTSTR strXMLPath, LPCTSTR strValue)
{
	return static_SetValue(static_Path(m_pParser, strXMLPath, true), strValue)?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}

int CAnalysisData::GetProperty(LPCTSTR strXMLPath, LPCTSTR strProperty, CString &strValue) const
{
	return static_GetProperty(static_Path(m_pParser, strXMLPath, false), strProperty, strValue)?IAERROR_SUCCESS:IAERROR_XML_NOTVALID;
}
int CAnalysisData::SetProperty(LPCTSTR strXMLPath, LPCTSTR strProperty, LPCTSTR strValue)
{
	return static_SetProperty(static_Path(m_pParser, strXMLPath, true), strProperty, strValue)?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}

int	CAnalysisData::GetValue_XMLString(LPCTSTR strXMLPath, CString &value) const
{
	return GetOuterXML(strXMLPath, value);
}
int	CAnalysisData::SetValue_XMLString(LPCTSTR strXMLPath, LPCTSTR value)
{
	return SetOuterXML(strXMLPath, value);
}

bool CAnalysisData::FromFormat_Double(LPCTSTR strXMLPath, const CString& vString, double& value) const
{
	value = _tstof(vString);
// 	CString strFormat;
// 	if(m_dwDataTypeMask & ADTF_USINGFORMATONGETTING) strFormat = FormatOnGetting(strXMLPath);
// 	if(strFormat.IsEmpty()) strFormat = _T("%f");
// 	float f;
// 	try{swscanf_s(vString, strFormat, &f);}catch(...){f = _tstof(vString);} 
// 	value = f;
	return true;
}
bool CAnalysisData::FromFormat_Int(LPCTSTR strXMLPath, const CString& vString, int& value) const
{
	value = _tstoi(vString);
// 	CString strFormat;
// 	if(m_dwDataTypeMask & ADTF_USINGFORMATONGETTING) strFormat = FormatOnGetting(strXMLPath);
// 	if(strFormat.IsEmpty()) strFormat = _T("%d");	
// 	try{swscanf_s(vString, strFormat, &value);}catch(...){value = _tstoi(vString);} 
	return true;
}
bool CAnalysisData::FromFormat_String(LPCTSTR strXMLPath, const CString& vString, CString& value) const
{
	//需要正则，暂不支持
	value = vString;
	return true;
}
bool CAnalysisData::FromFormat_Time(LPCTSTR strXMLPath, const CString& vString, CTime &value) const
{
	//内定为：%d-%d-%dT%d:%d:%d
// 	CString strFormat;
// 	if(m_dwDataTypeMask & ADTF_USINGFORMATONGETTING) strFormat = FormatOnGetting(strXMLPath);
// 	if(strFormat.IsEmpty()) strFormat = STRING_TO_TIME_FORMAT;
	int   nYear,nMonth,nDate,nHour,nMin,nSec;
//	swscanf_s(vString, strFormat, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	if(vString.Find(_T('T'))>1)
		swscanf_s(vString, STRING_TO_TIME_FORMAT_ST, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	else 
		swscanf_s(vString, STRING_TO_TIME_FORMAT, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	value = CTime(nYear,nMonth,nDate,nHour,nMin,nSec);
	return true;
}
bool CAnalysisData::FromFormat_Time(LPCTSTR strXMLPath, const CString& vString, COleDateTime &value) const
{
	//内定为：%d-%d-%dT%d:%d:%d
	// 	CString strFormat;
	// 	if(m_dwDataTypeMask & ADTF_USINGFORMATONGETTING) strFormat = FormatOnGetting(strXMLPath);
	// 	if(strFormat.IsEmpty()) strFormat = STRING_TO_TIME_FORMAT;
	int   nYear,nMonth,nDate,nHour,nMin,nSec;
	//	swscanf_s(vString, strFormat, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	if(vString.Find(_T('T'))>1)
		swscanf_s(vString, STRING_TO_TIME_FORMAT_ST, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	else 
		swscanf_s(vString, STRING_TO_TIME_FORMAT, &nYear,&nMonth,&nDate,&nHour,&nMin,&nSec);
	value = COleDateTime(nYear,nMonth,nDate,nHour,nMin,nSec);
	return true;
}

int	CAnalysisData::GetValue_Double(LPCTSTR strXMLPath, double &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_DOUBLE))
	{
		TRACE3("Error:CAnalysisData::GetValue_Double(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DOUBLE), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	static_GetValue(pNode, strValue);
	FromFormat_Double(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetValue_Int(LPCTSTR strXMLPath, int &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_INT32))
	{
		TRACE3("Error:CAnalysisData::GetValue_Int(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_INT32), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	static_GetValue(pNode, strValue);
	FromFormat_Int(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetValue_String(LPCTSTR strXMLPath, CString &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_STRING))
	{
		TRACE3("Error:CAnalysisData::GetValue_String(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_STRING), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	static_GetValue(pNode, strValue);
	FromFormat_String(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetValue_Time(LPCTSTR strXMLPath, CTime &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::GetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	static_GetValue(pNode, strValue);
	FromFormat_Time(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetValue_Time(LPCTSTR strXMLPath, COleDateTime &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::GetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	static_GetValue(pNode, strValue);
	FromFormat_Time(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetValue_Table(LPCTSTR strXMLPath, CIADataTable &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_OBJECT_TABLE))
	{
		TRACE3("Error:CAnalysisData::GetValue_Table(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT_TABLE), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	int result = static_GetOuterXML(pNode, strValue);
	if(result != IAERROR_SUCCESS) return result;
	value.SetXMLString(strValue);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetValue_Curve2D(LPCTSTR strXMLPath, CIADataCurve2D &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_OBJECT_CURVE2D))
	{
		TRACE3("Error:CAnalysisData::GetValue_Curve2D(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT_CURVE2D), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	int result = static_GetOuterXML(pNode, strValue);
	if(result != IAERROR_SUCCESS) return result;
	value.SetXMLString(strValue);
	return IAERROR_SUCCESS;
}
int CAnalysisData::GetValue_AnalysisData(LPCTSTR strXMLPath, CAnalysisData &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_OBJECT))
	{
		TRACE3("Error:CAnalysisData::GetValue_AnalysisData(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	int result = static_GetOuterXML(pNode, strValue);
	if(result != IAERROR_SUCCESS) return result;
	value.SetXMLString(strValue);
	return IAERROR_SUCCESS;
}

CString	CAnalysisData::FormatValue_Double(LPCTSTR strXMLPath, double value)
{
	CString strFormat;
	if(m_dwDataTypeMask & ADTF_USINGFORMATONSETTING) strFormat = FormatOnSetting(strXMLPath);
	if(strFormat.IsEmpty()) strFormat = _T("%f");
	CString strValue;
	strValue.Format(strFormat, value);
	return strValue;
}
CString	CAnalysisData::FormatValue_Int(LPCTSTR strXMLPath, int value)
{
	CString strFormat;
	if(m_dwDataTypeMask & ADTF_USINGFORMATONSETTING) strFormat = FormatOnSetting(strXMLPath);
	if(strFormat.IsEmpty()) strFormat = _T("%d");
	CString strValue;
	if(strFormat.Find(_T('d'))>=1)
		strValue.Format(strFormat, value);
	else {
		CString cStrTemp;
		cStrTemp.Format(_T("格式化错误：路径（%s），格式（%s），应该为int"), strXMLPath, strFormat);
		AfxMessageBox(cStrTemp);
		strValue=_T("格式化错误");
	}
	return strValue;
}
CString	CAnalysisData::FormatValue_String(LPCTSTR strXMLPath, LPCTSTR value)
{
	CString strFormat;
	if(m_dwDataTypeMask & ADTF_USINGFORMATONSETTING) strFormat = FormatOnSetting(strXMLPath);
	if(strFormat.IsEmpty()) strFormat = _T("%s");
	if(value == NULL) value = _T(""); 
	CString strValue;

	if(strFormat.Find(_T('s'))>=1)
		strValue.Format(strFormat, value);
	else {
		CString cStrTemp;
		cStrTemp.Format(_T("格式化错误：路径（%s），格式（%s），应该为int"), strXMLPath, strFormat);
		AfxMessageBox(cStrTemp);
		strValue=_T("格式化错误");
	}

	return strValue;
}
CString	CAnalysisData::FormatValue_Time(LPCTSTR strXMLPath, const CTime &value)
{
	CString strFormat;
	if(m_dwDataTypeMask & ADTF_USINGFORMATONSETTING) strFormat = FormatOnSetting(strXMLPath);
	if(strFormat.IsEmpty()) strFormat = TIME_TO_STRING_FORMAT;
	return value.Format(strFormat);
}
CString	CAnalysisData::FormatValue_Time(LPCTSTR strXMLPath, const COleDateTime &value)
{
	CString strFormat;
	if(m_dwDataTypeMask & ADTF_USINGFORMATONSETTING) strFormat = FormatOnSetting(strXMLPath);
	if(strFormat.IsEmpty()) strFormat = TIME_TO_STRING_FORMAT;
	return value.Format(strFormat);
}


int	CAnalysisData::SetValue_Double(LPCTSTR strXMLPath, double value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_DOUBLE))
	{
		TRACE3("Error:CAnalysisData::SetValue_Double(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DOUBLE), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetValue(pNode, FormatValue_Double(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetValue_Int(LPCTSTR strXMLPath, int value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_INT32))
	{
		TRACE3("Error:CAnalysisData::SetValue_Int(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_INT32), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetValue(pNode, FormatValue_Int(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetValue_String(LPCTSTR strXMLPath, LPCTSTR value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_STRING))
	{
		TRACE3("Error:CAnalysisData::SetValue_String(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_STRING), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetValue(pNode, FormatValue_String(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetValue_Time(LPCTSTR strXMLPath, const CTime &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::SetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetValue(pNode, FormatValue_Time(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetValue_Time(LPCTSTR strXMLPath, const COleDateTime &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::SetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetValue(pNode, FormatValue_Time(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetValue_Table(LPCTSTR strXMLPath, const CIADataTable &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_OBJECT_TABLE))
	{
		TRACE3("Error:CAnalysisData::SetValue_Table(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT_TABLE), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return SetValue_XMLString(strXMLPath, value.GetXMLString());
}
int	CAnalysisData::SetValue_Table2(LPCTSTR strXMLPath, const CIADataTable &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_OBJECT_TABLE))
	{
		TRACE3("Error:CAnalysisData::SetValue_Table(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT_TABLE), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}

	static_Assign(pNode, value.m_oTable.GetRoot(), CAnalysisData::ASSIGN_SELF|CAnalysisData::ASSIGN_CHILDREN|CAnalysisData::ASSIGN_NODE_EXISTING|CAnalysisData::ASSIGN_PROPERTY_EXISTING);

	return IAERROR_SUCCESS;
}
int	CAnalysisData::SetValue_Curve2D(LPCTSTR strXMLPath, const CIADataCurve2D &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_OBJECT_CURVE2D))
	{
		TRACE3("Error:CAnalysisData::SetValue_Curve2D(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT_CURVE2D), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return SetValue_XMLString(strXMLPath, value.GetXMLString());
}
int CAnalysisData::SetValue_AnalysisData(LPCTSTR strXMLPath, const CAnalysisData &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_OBJECT))
	{
		TRACE3("Error:CAnalysisData::SetValue_AnalysisData(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_OBJECT), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return SetValue_XMLString(strXMLPath, value.GetXMLString());
}




int	CAnalysisData::GetDefaultValue_Double(LPCTSTR strXMLPath, double &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_DOUBLE))
	{
		TRACE3("Error:CAnalysisData::GetValue_Double(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DOUBLE), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	if(!static_GetProperty(pNode, XMLPROP_DEFAULTVALUE, strValue)) return IAERROR_XML_ITEMNOTFOUND;
	FromFormat_Double(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetDefaultValue_Int(LPCTSTR strXMLPath, int &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_INT32))
	{
		TRACE3("Error:CAnalysisData::GetValue_Int(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_INT32), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	if(!static_GetProperty(pNode, XMLPROP_DEFAULTVALUE, strValue)) return IAERROR_XML_ITEMNOTFOUND;
	FromFormat_Int(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetDefaultValue_String(LPCTSTR strXMLPath, CString &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_STRING))
	{
		TRACE3("Error:CAnalysisData::GetValue_String(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_STRING), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	if(!static_GetProperty(pNode, XMLPROP_DEFAULTVALUE, strValue)) return IAERROR_XML_ITEMNOTFOUND;
	FromFormat_String(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetDefaultValue_Time(LPCTSTR strXMLPath, CTime &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::GetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	if(!static_GetProperty(pNode, XMLPROP_DEFAULTVALUE, strValue)) return IAERROR_XML_ITEMNOTFOUND;
	FromFormat_Time(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}
int	CAnalysisData::GetDefaultValue_Time(LPCTSTR strXMLPath, COleDateTime &value) const
{
	CONST_NODE pNode = static_Path(m_pParser, strXMLPath, false);
	if(!pNode) return IAERROR_XML_ITEMNOTFOUND;
	if(!CanGet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::GetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnGetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	CString strValue;
	if(!static_GetProperty(pNode, XMLPROP_DEFAULTVALUE, strValue)) return IAERROR_XML_ITEMNOTFOUND;
	FromFormat_Time(strXMLPath, strValue, value);
	return IAERROR_SUCCESS;
}



int	CAnalysisData::SetDefaultValue_Double(LPCTSTR strXMLPath, double value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_DOUBLE))
	{
		TRACE3("Error:CAnalysisData::SetValue_Double(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DOUBLE), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetProperty(pNode, XMLPROP_DEFAULTVALUE, FormatValue_Double(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetDefaultValue_Int(LPCTSTR strXMLPath, int value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_INT32))
	{
		TRACE3("Error:CAnalysisData::SetValue_Int(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_INT32), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetProperty(pNode, XMLPROP_DEFAULTVALUE, FormatValue_Int(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetDefaultValue_String(LPCTSTR strXMLPath, LPCTSTR value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_STRING))
	{
		TRACE3("Error:CAnalysisData::SetValue_String(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_STRING), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetProperty(pNode, XMLPROP_DEFAULTVALUE, FormatValue_String(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetDefaultValue_Time(LPCTSTR strXMLPath, const CTime &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::SetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetProperty(pNode, XMLPROP_DEFAULTVALUE, FormatValue_Time(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}
int	CAnalysisData::SetDefaultValue_Time(LPCTSTR strXMLPath, const COleDateTime &value)
{
	NODE pNode = static_Path(m_pParser, strXMLPath, true);
	if(!pNode)
	{
		ASSERT(false);
		return IAERROR_XML_ITEMNOTFOUND;
	}
	if(!CanSet(pNode, ADT_DATETIME))
	{
		TRACE3("Error:CAnalysisData::SetValue_Time(%s, Type:%s[SrcType:%s])\n", strXMLPath, static_TypeString(ADT_DATETIME), TypeOnSetting(strXMLPath));
		return IAERROR_XML_NOTVALID;
	}
	return static_SetProperty(pNode, XMLPROP_DEFAULTVALUE, FormatValue_Time(strXMLPath, value))?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}





CAnalysisData::enumCompareResult CAnalysisData::CompareValue_Double(LPCTSTR strXMLPath, const CAnalysisData& adPassed, LPCTSTR strXMLPathPassed) const
{
	double v1, v2;
	int r1 = GetValue_Double(strXMLPath, v1);
	int r2 = adPassed.GetValue_Double(strXMLPathPassed?strXMLPathPassed:strXMLPath, v2);
	if((r1 != IAERROR_SUCCESS) && (r2 != IAERROR_SUCCESS)) return CR_XMLPATH_NOT_EXIST_THIS_AND_PASSED;
	if(r1 != IAERROR_SUCCESS) return CR_XMLPATH_NOT_EXIST_THIS;
	if(r2 != IAERROR_SUCCESS) return CR_XMLPATH_NOT_EXIST_PASSED;

	if(v1 > v2) return CR_GREATER;
	if(v1 < v2) return CR_LESS;
	return CR_EQUAL;
}
CAnalysisData::enumCompareResult CAnalysisData::CompareValue_Int(LPCTSTR strXMLPath, const CAnalysisData& adPassed, LPCTSTR strXMLPathPassed) const
{
	int v1, v2;
	int r1 = GetValue_Int(strXMLPath, v1);
	int r2 = adPassed.GetValue_Int(strXMLPathPassed?strXMLPathPassed:strXMLPath, v2);
	if((r1 != IAERROR_SUCCESS) && (r2 != IAERROR_SUCCESS)) return CR_XMLPATH_NOT_EXIST_THIS_AND_PASSED;
	if(r1 != IAERROR_SUCCESS) return CR_XMLPATH_NOT_EXIST_THIS;
	if(r2 != IAERROR_SUCCESS) return CR_XMLPATH_NOT_EXIST_PASSED;

	if(v1 > v2) return CR_GREATER;
	if(v1 < v2) return CR_LESS;
	return CR_EQUAL;
}
CAnalysisData::enumCompareResult CAnalysisData::CompareValue_String(LPCTSTR strXMLPath, const CAnalysisData& adPassed, LPCTSTR strXMLPathPassed, bool bCaseSensitive) const
{
	CString v1, v2;
	int r1 = GetValue_String(strXMLPath, v1);
	int r2 = adPassed.GetValue_String(strXMLPathPassed?strXMLPathPassed:strXMLPath, v2);
	if((r1 != IAERROR_SUCCESS) && (r2 != IAERROR_SUCCESS)) return CR_XMLPATH_NOT_EXIST_THIS_AND_PASSED;
	if(r1 != IAERROR_SUCCESS) return CR_XMLPATH_NOT_EXIST_THIS;
	if(r2 != IAERROR_SUCCESS) return CR_XMLPATH_NOT_EXIST_PASSED;

	int v = bCaseSensitive?v1.Compare(v2):v1.CompareNoCase(v2);
	if(v > 0) return CR_GREATER;
	if(v < 0) return CR_LESS;
	return CR_EQUAL;
}

bool CAnalysisData::IsEqual(const CAnalysisData& ad) const
{
	if(m_pParser == NULL || m_pParser->getDocument() == NULL) return false;
	if(ad.m_pParser == NULL || ad.m_pParser->getDocument() == NULL) return false;
	return static_IsEqual(m_pParser->getDocument()->getDocumentElement(),
		ad.m_pParser->getDocument()->getDocumentElement());
}

////////////////////////////////////////////////////////////////////////////////////
// CIADataTable
////////////////////////////////////////////////////////////////////////////////////
int CIADataTable::Default_SortCallback(LPCTSTR strColumn, LPCTSTR strValue1, LPCTSTR strValue2)
{
	int ret = CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, 
		strValue1, _tcslen(strValue1), 
		strValue2, _tcslen(strValue2));
	switch(ret)
	{
	case CSTR_LESS_THAN:return -1;
	case CSTR_EQUAL:return 0;
	case CSTR_GREATER_THAN:return 1;
	default:return 0;
	}
}

CIADataTable::CIADataTable()
{
	m_oTable.Create(_T("IADataTable"));
}
CIADataTable::~CIADataTable()
{
}

int CIADataTable::Create(const CString& xmlPath)
{
	return m_oTable.Create(xmlPath);
}
CString CIADataTable::GetXMLString() const
{
	return m_oTable.GetXMLString();
}
int CIADataTable::SetXMLString(LPCTSTR strXMLString)
{
	return m_oTable.SetXMLString(strXMLString);
}

bool CIADataTable::Sort(CIADATATABLE_SORTCALLBACK cb)
{
	CAnalysisData::NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return false;

	DWORD dwOrders = 0;
	if(!GetTable(NULL, &dwOrders) || dwOrders == 0) return false;
	CString* pOrderBy = new CString[dwOrders];
	bool* pbAsc = new bool[dwOrders];
	for(DWORD i=0; i<dwOrders; i++)
	{
		if(!GetSortInfo(i, pOrderBy[i], pbAsc[i]))
		{
			delete []pOrderBy;
			delete []pbAsc;
			return false;
		}
	}

	if(cb == NULL) cb = Default_SortCallback;
	CArray<DWORD, DWORD> sorted;
	
	pNode = pNode->getFirstChild();
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_ROW)) sorted.Add((DWORD)pNode);
		pNode = pNode->getNextSibling();
	}

	CString strValue1,strValue2;
	for(int i=0; i<=sorted.GetUpperBound(); i++)
	{
		int iBeCurrent = i;
		for(int j=i+1; j<=sorted.GetUpperBound(); j++)
		{
			CAnalysisData::NODE pNodeBeCurrent = (CAnalysisData::NODE)sorted.GetAt(iBeCurrent);
			CAnalysisData::NODE pNodeNext = (CAnalysisData::NODE)sorted.GetAt(j);
			for(DWORD k=0; k<dwOrders; k++)
			{
				if(!CAnalysisData::static_GetProperty(pNodeBeCurrent, pOrderBy[k], strValue1)) strValue1 = _T("");
				if(!CAnalysisData::static_GetProperty(pNodeNext, pOrderBy[k], strValue2)) strValue2 = _T("");
				int result = cb(pOrderBy[k], strValue1, strValue2);
				if(result == 0) continue;
				if(result > 0 && pbAsc[k])
				{
					iBeCurrent = j;
					break;
				}
				else if(result < 0 && !pbAsc[k])
				{
					iBeCurrent = j;
					break;
				}
				else break;
			}
		}
		if(i != iBeCurrent)
		{
			DWORD it = sorted.GetAt(i);
			sorted.SetAt(i, sorted.GetAt(iBeCurrent));
			sorted.SetAt(iBeCurrent, it);
		}
	}
	pNode = m_oTable.GetRoot();
	for(int i=0; i<=sorted.GetUpperBound(); i++) pNode->appendChild((CAnalysisData::NODE)sorted.GetAt(i));
	
	delete []pOrderBy;
	delete []pbAsc;
	return true;
}

bool CIADataTable::GetTable(CString* pstrTable, DWORD* pdwOrders) const
{
	CAnalysisData::CONST_NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return false;
	if(pstrTable) *pstrTable = pNode->getNodeName();
	if(pdwOrders)
	{
		pNode = CAnalysisData::static_NodeProperty((CAnalysisData::NODE)pNode, XMLPROP_TAB_ORDERS);
		if(pNode) *pdwOrders = (DWORD)_tstol(pNode->getNodeValue());
		else *pdwOrders = 0;
	}
	return true;
}
bool CIADataTable::SetTable(DWORD dwOrders)
{
	CString t;
	t.Format(_T("%u"), dwOrders);
	return m_oTable.SetProperty(_T("/"), XMLPROP_TAB_ORDERS, t) == IAERROR_SUCCESS;
}
bool CIADataTable::GetSortInfo(DWORD dwIndex, CString& strOrderBy, bool& bAsc) const
{
	CString t;
	t.Format(_T("%s%u"), XMLPROP_TAB_ORDERBY, dwIndex+1);
	if(m_oTable.GetProperty(_T("/"), t, strOrderBy) != IAERROR_SUCCESS) return false;
	t.Format(_T("%s%u"), XMLPROP_TAB_ASC, dwIndex+1);
	CString v;
	if(m_oTable.GetProperty(_T("/"), t, v) != IAERROR_SUCCESS) return false;
	bAsc = (v.CompareNoCase(_T("true")) == 0 || v.CompareNoCase(_T("yes")) == 0 || v.CompareNoCase(_T("是")) == 0 || _tstol(v));
	return true;
}
bool CIADataTable::SetSortInfo(DWORD dwIndex, LPCTSTR strOrderBy, bool bAsc)
{
	CString t;
	t.Format(_T("%s%u"), XMLPROP_TAB_ORDERBY, dwIndex+1);
	if(m_oTable.SetProperty(_T("/"), t, strOrderBy) != IAERROR_SUCCESS) return false;
	t.Format(_T("%s%u"), XMLPROP_TAB_ASC, dwIndex+1);
	return m_oTable.SetProperty(_T("/"), t, bAsc?_T("true"):_T("false")) == IAERROR_SUCCESS;
}

bool CIADataTable::GetColumns(CStringArray* pArrTopics, CStringArray* pArrTitles) const
{
	CAnalysisData::CONST_NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return false;
	if(pArrTopics) pArrTopics->RemoveAll();
	if(pArrTitles) pArrTitles->RemoveAll();
	pNode = pNode->getFirstChild();
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_COLUMN))
		{
			CString topic, title;
			GetColumn(pNode, &topic, &title);
			if(pArrTopics) pArrTopics->Add(topic);
			if(pArrTitles) pArrTitles->Add(title);
		}
		pNode = pNode->getNextSibling();
	}
	return true;
}
bool CIADataTable::SetColumns(const CStringArray& arrTopics, const CStringArray* pArrTitles)
{
	xercesc_2_8::DOMDocument* pDocument = m_oTable.GetDocument();
	CAnalysisData::NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return false;
	DeleteAllColumn();
	for(int i=0; i<=arrTopics.GetUpperBound(); i++)
	{
		LPCTSTR strTitle = NULL;
		if(pArrTitles && i <= pArrTitles->GetUpperBound()) strTitle = pArrTitles->GetAt(i);
		if(!_AppendColumn(arrTopics.GetAt(i), strTitle)) return false;
	}
	return true;
}
bool CIADataTable::GetColumn(long iTopic, CString* pstrTopic, CString* pstrTitle) const
{
	CIADataTable* pThis = (CIADataTable*)this;
	return GetColumn(pThis->GetColumn(iTopic), pstrTopic, pstrTitle);
}

bool CIADataTable::SetColumn(long iTopic, LPCTSTR strTopic, LPCTSTR strTitle)
{
	return SetColumn(GetColumn(iTopic), strTopic, strTitle);
}
bool CIADataTable::SetColumn(LPCTSTR strOldTopic, LPCTSTR strNewTopic, LPCTSTR strTitle)
{
	return SetColumn(GetColumn(strOldTopic), strNewTopic, strTitle);
}

bool CIADataTable::GetColumnInfo(long iTopic, LPCTSTR propertyName, CString& propertyValue) const
{
	CAnalysisData::CONST_NODE pTopic = GetColumn(iTopic);
	if(!pTopic) return false;
	return CAnalysisData::static_GetProperty(pTopic, propertyName, propertyValue);
}
bool CIADataTable::SetColumnInfo(long iTopic, LPCTSTR propertyName, LPCTSTR propertyValue)
{
	CAnalysisData::NODE pTopic = GetColumn(iTopic);
	if(!pTopic) return false;
	return CAnalysisData::static_SetProperty(pTopic, propertyName, propertyValue);
}
bool CIADataTable::GetColumnInfo(LPCTSTR strTopic, LPCTSTR propertyName, CString& propertyValue) const
{
	CAnalysisData::NODE pTopic = GetColumn(strTopic);
	if(!pTopic) return false;
	return CAnalysisData::static_GetProperty(pTopic, propertyName, propertyValue);
}
bool CIADataTable::SetColumnInfo(LPCTSTR strTopic, LPCTSTR propertyName, LPCTSTR propertyValue)
{
	CAnalysisData::NODE pTopic = GetColumn(strTopic);
	if(!pTopic) return false;
	return CAnalysisData::static_SetProperty(pTopic, propertyName, propertyValue);
}
bool CIADataTable::GetColumnInfo(long iTopic, const CStringArray& propertiesNames, CStringArray& propertiesValues) const
{
	propertiesValues.RemoveAll();
	CAnalysisData::NODE pTopic = GetColumn(iTopic);
	if(!pTopic) return false;
	CString t;
	for(int i=0; i<=propertiesNames.GetUpperBound(); i++)
	{
		if(CAnalysisData::static_GetProperty(pTopic, propertiesNames.GetAt(i), t)) propertiesValues.Add(t);
		else propertiesValues.Add(_T(""));
	}
	return true;
}
bool CIADataTable::SetColumnInfo(long iTopic, const CStringArray& propertiesNames, const CStringArray& propertiesValues)
{
	CAnalysisData::NODE pTopic = GetColumn(iTopic);
	if(!pTopic) return false;
	for(int i=0; i<=propertiesNames.GetUpperBound(); i++)
	{
		CAnalysisData::static_SetProperty(pTopic, propertiesNames.GetAt(i), propertiesValues.GetAt(i));
	}
	return true;
}
bool CIADataTable::GetColumnInfo(LPCTSTR strTopic, const CStringArray& propertiesNames, CStringArray& propertiesValues) const
{
	propertiesValues.RemoveAll();
	CAnalysisData::NODE pTopic = GetColumn(strTopic);
	if(!pTopic) return false;
	CString t;
	for(int i=0; i<=propertiesNames.GetUpperBound(); i++)
	{
		if(CAnalysisData::static_GetProperty(pTopic, propertiesNames.GetAt(i), t)) propertiesValues.Add(t);
		else propertiesValues.Add(_T(""));
	}
	return true;
}
bool CIADataTable::SetColumnInfo(LPCTSTR strTopic, const CStringArray& propertiesNames, const CStringArray& propertiesValues)
{
	CAnalysisData::NODE pTopic = GetColumn(strTopic);
	if(!pTopic) return false;
	for(int i=0; i<=propertiesNames.GetUpperBound(); i++)
	{
		CAnalysisData::static_SetProperty(pTopic, propertiesNames.GetAt(i), propertiesValues.GetAt(i));
	}
	return true;
}

long CIADataTable::AppendColumn(LPCTSTR strTopic, LPCTSTR strTitle)
{
	if(!_AppendColumn(strTopic, strTitle)) return -1;
	return GetColumnNumber() - 1;
}
long CIADataTable::InsertColumn(int iTopic, LPCTSTR strTopic, LPCTSTR strTitle)
{
	CAnalysisData::NODE pPosition = GetColumn(iTopic);
	if(pPosition == NULL) return -1;
	xercesc_2_8::DOMDocument* pDocument = m_oTable.GetDocument();
	CAnalysisData::NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return -1;
	pNode = pNode->insertBefore(pDocument->createElement(XMLPROP_TAB_COLUMN), pPosition);
	if(pNode == NULL) return -1;
	SetColumn(pNode, strTopic, strTitle);
	return iTopic;
}

long CIADataTable::GetColumnNumber() const
{
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pNode = pThis->m_oTable.GetRoot();
	if(pNode == NULL) return -1;
	pNode = pNode->getFirstChild();
	long lCount = 0;
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_COLUMN)) lCount++;
		pNode = pNode->getNextSibling();
	}
	return lCount;
}
bool CIADataTable::DeleteColumn(long iTopic)
{	
	CAnalysisData::NODE pTopic = GetColumn(iTopic);
	if(pTopic == NULL || pTopic->getParentNode() == NULL) return false;
	pTopic->getParentNode()->removeChild(pTopic);
	return true;
}
bool CIADataTable::DeleteColumn(LPCTSTR strTopic)
{	
	CAnalysisData::NODE pTopic = GetColumn(strTopic);
	if(pTopic == NULL || pTopic->getParentNode() == NULL) return false;
	pTopic->getParentNode()->removeChild(pTopic);
	return true;
}
bool CIADataTable::DeleteAllColumn()
{
	for(;;)
	{
		DeleteColumn((long)0);
		if(GetColumnNumber() < 1) return true;
	}
	return false;
}
long CIADataTable::GetColumnIndex(LPCTSTR strTopic) const
{
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pNode = pThis->m_oTable.GetRoot();
	if(pNode == NULL || strTopic == NULL) return -1;
	pNode = pNode->getFirstChild();
	long lIndex = 0;
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_COLUMN))
		{
			CString t;
			if(CAnalysisData::static_GetProperty(pNode, XMLPROP_TAB_COL_ID, t) && t == strTopic) return lIndex;
			lIndex++;
		}
		pNode = pNode->getNextSibling();
	}
	return -1;
}

long CIADataTable::AppendRow()
{
	xercesc_2_8::DOMDocument* pDocument = m_oTable.GetDocument();
	CAnalysisData::NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return -1;
	pNode = pNode->appendChild(pDocument->createElement(XMLPROP_TAB_ROW));
	if(pNode == NULL) return -1;
	return GetRowsNumber() - 1;
}
long CIADataTable::InsertRow(int iLine)
{
	CAnalysisData::NODE pPosition = GetRow(iLine);
	if(pPosition == NULL) return -1;
	xercesc_2_8::DOMDocument* pDocument = m_oTable.GetDocument();
	CAnalysisData::NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return -1;
	pNode = pNode->insertBefore(pDocument->createElement(XMLPROP_TAB_ROW), pPosition);
	if(pNode == NULL) return -1;
	return iLine;
}
long CIADataTable::AppendRow(const CStringArray &arrayString)
{
	int iLine = AppendRow();
	if(iLine < 0) return -1;
	SetRowValues(iLine, arrayString);
	return iLine;
}
long CIADataTable::InsertRow(int iLine, const CStringArray &arrayString)
{
	iLine = InsertRow(iLine);
	if(iLine < 0) return -1;
	SetRowValues(iLine, arrayString);
	return iLine;
}

long CIADataTable::GetRowsNumber() const
{
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pNode = pThis->m_oTable.GetRoot();
	if(pNode == NULL) return -1;
	pNode = pNode->getFirstChild();
	long lCount = 0;
	while(pNode)
	{
		CString ctr = pNode->getNodeName();
		if(pNode->getNodeName() == CString(XMLPROP_TAB_ROW)) lCount++;
		pNode = pNode->getNextSibling();
	}
	return lCount;
}
bool CIADataTable::DeleteRow(long iLine)
{	
	CAnalysisData::NODE pNode = GetRow(iLine);
	if(pNode == NULL || pNode->getParentNode() == NULL) return false;
	pNode->getParentNode()->removeChild(pNode);
	return true;
}
bool CIADataTable::DeleteAllRows()
{
	for(;;)
	{
		DeleteRow(0);
		if(GetRowsNumber() < 1) return true;
	}
	return false;
}

bool CIADataTable::GetRowValues(long iLine, CStringArray& values) const
{	
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pNode = pThis->GetRow(iLine);
	if(pNode == NULL) return false;
	values.RemoveAll();

	CAnalysisData::NODE pTopic = m_oTable.GetRoot()->getFirstChild();
	while(pTopic)
	{
		if(pTopic->getNodeName() == CString(XMLPROP_TAB_COLUMN))
		{
			CString t;
			if(CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, t))
			{
				CString v;
				if(CAnalysisData::static_GetProperty(pNode, t, v)) values.Add(v);
				else values.Add(_T(""));
			}
		}
		pTopic = pTopic->getNextSibling();
	}
	return true;
}
bool CIADataTable::SetRowValues(long iLine, const CStringArray& values)
{	
	CAnalysisData::NODE pNode = GetRow(iLine);
	if(pNode == NULL) return false;

	CAnalysisData::NODE pTopic = m_oTable.GetRoot()->getFirstChild();
	long iIndex = 0;
	while(pTopic)
	{
		if(pTopic->getNodeName() == CString(XMLPROP_TAB_COLUMN))
		{
			if(iIndex > values.GetUpperBound()) break;
			CString t;
			if(CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, t))
			{
				CAnalysisData::static_SetProperty(pNode, t, values.GetAt(iIndex));
			}
			iIndex++;
		}
		pTopic = pTopic->getNextSibling();
	}
	return true;
}

bool CIADataTable::GetColumnValues(long iColumn, CStringArray& values) const
{	
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pTopic = pThis->GetColumn(iColumn);
	if(pTopic == NULL) return false;
	CString strTopic;
	if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, strTopic)) return false;
	return GetColumnValues(strTopic, values);
}
bool CIADataTable::GetColumnValues(LPCTSTR strColumn, CStringArray& values) const
{	
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pTopic = pThis->GetColumn(strColumn);
	if(pTopic == NULL) return false;
	values.RemoveAll();
	CAnalysisData::NODE pNode = m_oTable.GetRoot()->getFirstChild();
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_ROW))
		{
			CString t;
			if(CAnalysisData::static_GetProperty(pNode, strColumn, t)) values.Add(t);
			else values.Add(_T(""));
		}
		pNode = pNode->getNextSibling();
	}
	return true;
}
bool CIADataTable::SetColumnValues(long iColumn, const CStringArray& values)
{	
	CAnalysisData::NODE pTopic = GetColumn(iColumn);
	if(pTopic == NULL) return false;
	CString strTopic;
	if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, strTopic)) return false;
	return SetColumnValues(strTopic, values);
}
bool CIADataTable::SetColumnValues(LPCTSTR strColumn, const CStringArray& values)
{	
	CAnalysisData::NODE pTopic = GetColumn(strColumn);
	if(pTopic == NULL) return false;
	CAnalysisData::NODE pNode = m_oTable.GetRoot()->getFirstChild();
	long iIndex = 0;
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_ROW))
		{
			if(iIndex > values.GetUpperBound()) break;
			CAnalysisData::static_SetProperty(pNode, strColumn, values.GetAt(iIndex));
			iIndex++;
		}
		pNode = pNode->getNextSibling();
	}
	return true;
}

bool CIADataTable::GetCellValue(long iLine, long iColumn, CString& colValue) const
{
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pLine = pThis->GetRow(iLine);
	CAnalysisData::NODE pTopic = pThis->GetColumn(iColumn);
	if(pLine == NULL || pTopic == NULL) return false;
	CString strColumn;
	if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, strColumn)) return false;
	return CAnalysisData::static_GetProperty(pLine, strColumn, colValue);
}
bool CIADataTable::GetCellValue(long iLine, LPCTSTR strColumn, CString& colValue) const
{
	CIADataTable* pThis = (CIADataTable*)this;
	CAnalysisData::NODE pLine = pThis->GetRow(iLine);
	CAnalysisData::NODE pTopic = pThis->GetColumn(strColumn);
	if(pLine == NULL || pTopic == NULL) return false;
	return CAnalysisData::static_GetProperty(pLine, strColumn, colValue);
}
bool CIADataTable::SetCellValue(long iLine, long iColumn, LPCTSTR colValue)
{
	CAnalysisData::NODE pLine = GetRow(iLine);
	CAnalysisData::NODE pTopic = GetColumn(iColumn);
	if(pLine == NULL || pTopic == NULL) return false;
	CString strColumn;
	if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, strColumn)) return false;
	return CAnalysisData::static_SetProperty(pLine, strColumn, colValue);
}
bool CIADataTable::SetCellValue(long iLine, LPCTSTR strColumn, LPCTSTR colValue)
{
	CAnalysisData::NODE pLine = GetRow(iLine);
	CAnalysisData::NODE pTopic = GetColumn(strColumn);
	if(pLine == NULL || pTopic == NULL) return false;
	return CAnalysisData::static_SetProperty(pLine, strColumn, colValue);
}

CAnalysisData::NODE CIADataTable::GetColumn(long iTopic) const
{
	CAnalysisData::NODE pNode = const_cast<CAnalysisData::NODE>(m_oTable.GetRoot());
	if(pNode == NULL || iTopic < 0) return NULL;
	pNode = pNode->getFirstChild();
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_COLUMN))
		{
			if(iTopic == 0) return pNode;
			iTopic--;
		}
		pNode = pNode->getNextSibling();
	}
	return pNode;
}
CAnalysisData::NODE CIADataTable::GetColumn(LPCTSTR strTopic) const
{
	CAnalysisData::NODE pNode = const_cast<CAnalysisData::NODE>(m_oTable.GetRoot());
	if(pNode == NULL || strTopic == NULL) return NULL;
	pNode = pNode->getFirstChild();
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_COLUMN))
		{
			CString t;
			if(CAnalysisData::static_GetProperty(pNode, XMLPROP_TAB_COL_ID, t) && t == strTopic) return pNode;
		}
		pNode = pNode->getNextSibling();
	}
	return NULL;
}
bool CIADataTable::GetColumn(CAnalysisData::CONST_NODE pTopic, CString* pstrTopic, CString* pstrTitle) const
{
	if(pTopic == NULL) return false;
	CString name;
	if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, name)) return false;
	if(pstrTopic) *pstrTopic = name;
	if(pstrTitle)
	{
		if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_TITLE, *pstrTitle)) *pstrTitle = name;
	}
	return true;
}  
bool CIADataTable::SetColumn(CAnalysisData::NODE pTopic, LPCTSTR strTopic, LPCTSTR strTitle)
{
	if(pTopic == NULL) return false;
	CString strOldTopic;
	if(strTopic && _tcslen(strTopic))//需要更改Topic
	{
		if(!CAnalysisData::static_GetProperty(pTopic, XMLPROP_TAB_COL_ID, strOldTopic)) strOldTopic = _T("");
		if(!CAnalysisData::static_SetProperty(pTopic, XMLPROP_TAB_COL_ID, strTopic)) return false;
	}
	if(strTitle) CAnalysisData::static_SetProperty(pTopic, XMLPROP_TAB_COL_TITLE, strTitle);
	if(!strOldTopic.IsEmpty())//更正已有条目
	{
		CAnalysisData::NODE pNode = m_oTable.GetRoot();
		pNode = pNode->getFirstChild();
		while(pNode)
		{
			if(pNode->getNodeName() == CString(XMLPROP_TAB_ROW))
			{
				CString t;
				if(CAnalysisData::static_GetProperty(pNode, strOldTopic, t)) CAnalysisData::static_SetProperty(pNode, strTopic, t);
			}
			pNode = pNode->getNextSibling();
		}
	}
	return true;
}
bool CIADataTable::_AppendColumn(LPCTSTR strTopic, LPCTSTR strTitle)
{
	if(strTopic == NULL || _tcslen(strTopic) == 0) return false;
	xercesc_2_8::DOMDocument* pDocument = m_oTable.GetDocument();
	CAnalysisData::NODE pNode = m_oTable.GetRoot();
	if(pNode == NULL) return false;
	pNode = pNode->appendChild(pDocument->createElement(XMLPROP_TAB_COLUMN));
	if(pNode == NULL) return false;
	SetColumn(pNode, strTopic, strTitle);
	return true;
}
CAnalysisData::NODE CIADataTable::GetRow(long iLine) const
{
	CAnalysisData::NODE pNode = const_cast<CAnalysisData::NODE>(m_oTable.GetRoot());
	if(pNode == NULL || iLine < 0) return NULL;
	pNode = pNode->getFirstChild();
	while(pNode)
	{
		if(pNode->getNodeName() == CString(XMLPROP_TAB_ROW))
		{
			if(iLine == 0) return pNode;
			iLine--;
		}
		pNode = pNode->getNextSibling();
	}
	return pNode;
}

//////////////////////////////////////////////////////////////////////////
// CIATable ---- 分析数据表格 //
//////////////////////////////////////////////////////////////////////////
CIATable::CIATable():m_colorContent(RGB(255,255,255)),m_DislplayFirstColumn(false),m_bAutoAdd_RowButtonAdd(false),
	m_nAdditionRow(0), m_bIsSettingup(false)
{
	m_nFontID_Title = AddFont(_T("黑体"), 14, 4);
	m_nFontID_Content = AddFont(_T("幼圆"), 13, 3);
	for(int i=0; i<BITMAP_COUNT; i++) m_pBitmaps[i] = -1;
}
int CIATable::GetBitmapIndex(enumBitmapIndex eBitmap) const
{
	if(eBitmap >= BITMAP_COUNT) return -1;
	return m_pBitmaps[eBitmap];
}
CString CIATable::static_ValueFromFullTag(LPCTSTR tag)
{
	if(tag == NULL ||_tcslen(tag) == 0) return _T("");
	CString s(tag);
	if(s.Left(begin.GetLength()) != begin || s.Right(end.GetLength()) != end)
	{
		ASSERT(false);
		return _T("");
	}
	s.Delete(0, begin.GetLength());
	s = s.Left(s.GetLength() - end.GetLength());
	CString v2[2];
	if(!CStringExt::Split(s, _T(":"), 2,v2))
	{
		ASSERT(false);
		return _T("");
	}
	return v2[1];
}
CString CIATable::GetBitmapFile(enumBitmapIndex eBitmap) const
{
	if(eBitmap < 0 || eBitmap >= BITMAP_COUNT) return _T("");
	switch(eBitmap)
	{
	case BITMAP_ADD:return static_ValueFromFullTag(TABLE_BITMAP_ADD);
	case BITMAP_DELETE:return static_ValueFromFullTag(TABLE_BITMAP_DELETE);
	case BITMAP_INF:return static_ValueFromFullTag(TABLE_BITMAP_INF);
	case BITMAP_MANUAL_GO:return static_ValueFromFullTag(TABLE_BITMAP_MANUAL_GO);
	case BITMAP_MANUAL_NEXT:return static_ValueFromFullTag(TABLE_BITMAP_MANUAL_NEXT);
	case BITMAP_MANUAL_OK:return static_ValueFromFullTag(TABLE_BITMAP_MANUAL_OK);
	case BITMAP_AUTO_GO:return static_ValueFromFullTag(TABLE_BITMAP_AUTO_GO);
	case BITMAP_AUTO_NEXT:return static_ValueFromFullTag(TABLE_BITMAP_AUTO_NEXT);
	case BITMAP_AUTO_OK:return static_ValueFromFullTag(TABLE_BITMAP_AUTO_OK);
	case BITMAP_RUNNING:return static_ValueFromFullTag(TABLE_BITMAP_RUNNING);
	case BITMAP_PAUSE:return static_ValueFromFullTag(TABLE_BITMAP_PAUSE);		
	case BITMAP_ERROR:return static_ValueFromFullTag(TABLE_BITMAP_ERROR);
	default:return _T("");
	}
}
LPCTSTR CIATable::GetBitmapFlag(enumBitmapIndex eBitmap) const
{
	if(eBitmap < 0 || eBitmap >= BITMAP_COUNT) return _T("");
	switch(eBitmap)
	{
	case BITMAP_ADD:return TABLE_BITMAP_ADD;
	case BITMAP_DELETE:return TABLE_BITMAP_DELETE;
	case BITMAP_INF:return TABLE_BITMAP_INF;
	case BITMAP_MANUAL_GO:return TABLE_BITMAP_MANUAL_GO;
	case BITMAP_MANUAL_NEXT:return TABLE_BITMAP_MANUAL_NEXT;
	case BITMAP_MANUAL_OK:return TABLE_BITMAP_MANUAL_OK;
	case BITMAP_AUTO_GO:return TABLE_BITMAP_AUTO_GO;
	case BITMAP_AUTO_NEXT:return TABLE_BITMAP_AUTO_NEXT;
	case BITMAP_AUTO_OK:return TABLE_BITMAP_AUTO_OK;
	case BITMAP_RUNNING:return TABLE_BITMAP_RUNNING;
	case BITMAP_PAUSE:return TABLE_BITMAP_PAUSE;
	case BITMAP_ERROR:return TABLE_BITMAP_ERROR;
	default:return _T("");
	}
}
const CBitmap* CIATable::GetBitmapObject(enumBitmapIndex eBitmap) const
{
	CIATable* pThis = (CIATable*)this;
	return pThis->GetBitmap(GetBitmapIndex(eBitmap));
}

BOOL CIATable::CreateTable(CWnd *pcParent, CRect rect)
{
	if(!CreateGrid(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rect, pcParent, 1234)) return FALSE;

	if(!m_DislplayFirstColumn) SetColWidth(-1, 0);

	return TRUE;
}
BOOL CIATable::SetTitle(int nCol, LPCTSTR strTitleName, int nColWidth, int nTitleNO)
{
	if(nCol >= GetNumberCols()) return FALSE;

	QuickSetText(nCol, nTitleNO, strTitleName);
	QuickSetFont(nCol, nTitleNO, m_nFontID_Title);
	SetColWidth(nCol, nColWidth);

	return TRUE;
}
void CIATable::AppendRow_ButtonAdd()
{
	int nCol = GetNumberCols();
	if(nCol < 1) return;
	AppendRow();
	int nRow = GetNumberRows();

	JoinCells(1, nRow-1,nCol-1, nRow-1);

	if(nCol > 1)
	{
		QuickSetBackColor(1, nRow-1, RGB(240,240,240));
		QuickSetBorder(1, nRow-1, UG_BDR_RAISED|UG_BDR_LTHIN|UG_BDR_RTHIN|UG_BDR_TTHIN|UG_BDR_BTHIN); 
	}

	QuickSetBitmap(0, nRow-1, m_pBitmaps[BITMAP_ADD]);

	m_nAdditionRow = 1;
}
void CIATable::RomoveRow_ButtonAdd()
{
	int nRow = CUGCtrl::GetNumberRows();
	int nCol = CUGCtrl::GetNumberCols();

	CUGCell cell;
	GetCell(0, nRow-1, &cell);

	//如果有增加行
	if(cell.GetBitmap() == GetBitmap(m_pBitmaps[BITMAP_ADD]))
	{
		for(int i=1; i<nCol-1; ++i)
		{
			UnJoinCells(i, nRow-1);
			QuickSetBackColor(i, nRow-1, m_colorContent);
		}
		m_nAdditionRow = 0;
	}
}

LPCTSTR CIATable::GetXMLBitmapFlag(int iCol, int iRow) const
{
	CIATable* pThis = (CIATable*)this;
	CUGCell cell;
	if(pThis->GetCell(iCol, iRow, &cell) != UG_SUCCESS) return _T("");
	CBitmap* pBitmap = cell.GetBitmap();

	if(pBitmap == GetBitmapObject(BITMAP_DELETE)) return TABLE_BITMAP_DELETE;//删除
	else if(pBitmap == GetBitmapObject(BITMAP_INF)) return TABLE_BITMAP_INF;//信息
	else if(pBitmap == GetBitmapObject(BITMAP_ADD)) return TABLE_BITMAP_ADD;//添加
	else if(pBitmap == GetBitmapObject(BITMAP_ERROR)) return TABLE_BITMAP_ERROR;//错误
	else if(pBitmap == GetBitmapObject(BITMAP_MANUAL_GO)) return TABLE_BITMAP_MANUAL_GO;// 手工任务
	else if(pBitmap == GetBitmapObject(BITMAP_MANUAL_NEXT)) return TABLE_BITMAP_MANUAL_NEXT;
	else if(pBitmap == GetBitmapObject(BITMAP_MANUAL_OK)) return TABLE_BITMAP_MANUAL_OK;
	else if(pBitmap == GetBitmapObject(BITMAP_AUTO_GO)) return TABLE_BITMAP_AUTO_GO;//自动任务
	else if(pBitmap == GetBitmapObject(BITMAP_AUTO_NEXT)) return TABLE_BITMAP_AUTO_NEXT;
	else if(pBitmap == GetBitmapObject(BITMAP_AUTO_OK)) return TABLE_BITMAP_AUTO_OK;
	else if(pBitmap == GetBitmapObject(BITMAP_RUNNING)) return TABLE_BITMAP_RUNNING;//正在运行
	else return _T("");
}
long CIATable::GetBitmapIndexFromXMLFlag(const CString& flag) const
{
	if(flag == TABLE_BITMAP_DELETE) return GetBitmapIndex(BITMAP_DELETE);//删除
	else if(flag == TABLE_BITMAP_INF) return GetBitmapIndex(BITMAP_INF);//信息
	else if(flag == TABLE_BITMAP_ADD) return GetBitmapIndex(BITMAP_ADD);//添加
	else if(flag == TABLE_BITMAP_ERROR) return GetBitmapIndex(BITMAP_ERROR);//错误
	else if(flag == TABLE_BITMAP_MANUAL_GO) return GetBitmapIndex(BITMAP_MANUAL_GO);// 手工任务
	else if(flag == TABLE_BITMAP_MANUAL_NEXT) return GetBitmapIndex(BITMAP_MANUAL_NEXT);
	else if(flag == TABLE_BITMAP_MANUAL_OK) return GetBitmapIndex(BITMAP_MANUAL_OK);
	else if(flag == TABLE_BITMAP_AUTO_GO) return GetBitmapIndex(BITMAP_AUTO_GO);//自动任务
	else if(flag == TABLE_BITMAP_AUTO_NEXT) return GetBitmapIndex(BITMAP_AUTO_NEXT);
	else if(flag == TABLE_BITMAP_AUTO_OK) return GetBitmapIndex(BITMAP_AUTO_OK);
	else if(flag == TABLE_BITMAP_RUNNING) return GetBitmapIndex(BITMAP_RUNNING);//正在运行
	else return -1;
}
void CIATable::UnJoinTitles()
{
	for(int i=GetTH_NumberRows()-1; i>-1; i--)
	{
		for(int j=GetNumberCols()-1; j>-1; j--) UnJoinCells(j, -1 - i);
	}
}
void CIATable::DeleteAllItems()
{
	for(int i=GetNumberRows()-1; i>-1; i--) DeleteRow(0);
}
const CIADataTable&  CIATable::GetNewXMLData(LPCTSTR strXMLPath)
{
	if(strXMLPath) m_dataTable.Create(strXMLPath);

	int nColumnNum = GetNumberCols();

	//储存标题
	CStringArray	arrayTopics;
	CStringArray	arrayTitles;

	m_dataTable.GetColumns(&arrayTopics, NULL);

	int nTitleNum = GetTH_NumberRows();
	for(int i=arrayTopics.GetUpperBound()+1; i<nColumnNum; i++)
	{
		CString id;
		id.Format(_T("Topic_%d"), i);
		arrayTopics.Add(id);//-1标题行是Topic
	}
	for(int i=0; i<nColumnNum; i++)
	{
		CString title;
		for(int j=nTitleNum-1; j>-1; j--)
		{
			title = title + QuickGetText(i, -1 - j) + _T(",");
		}
		if(title.GetLength()) title.Delete(title.GetLength() - 1);
		arrayTitles.Add(title);
	}
	//if(!m_dataTable.SetColumns(arrayTopics, &arrayTitles)) return IAERROR_UNKNOWN;
	m_dataTable.SetColumns(arrayTopics, &arrayTitles);

	return UpdateXMLData();
}

CString CIATable::begin = _T("{[(");
CString CIATable::end = _T(")]}");
CString CIATable::bitmap_predef = _T("BITMAP_PREDEF:");

const CIADataTable&  CIATable::UpdateXMLData()
{
	int nColumnNum = GetNumberCols();

	CString v;
	//储存宽度
	for(int i=0; i<nColumnNum; i++)
	{
		v.Format(_T("%d"), GetColWidth(i));
		m_dataTable.SetColumnInfo(i, XMLPROP_TAB_COL_WIDTH, v);
	}

	//储存各条目
	m_dataTable.DeleteAllRows();
	int nRowNum = GetNumberRows() - m_nAdditionRow;
	CStringArray	arrayItemValues;
	CString tBmpFlag;
	for(int i=0; i<nRowNum; ++i)
	{
		arrayItemValues.RemoveAll();
		for(int j=0; j<nColumnNum; ++j)
		{
			tBmpFlag = GetXMLBitmapFlag(j, i);
			//if(!tBmpFlag.IsEmpty()) tBmpFlag = begin + tBmpFlag + end;
			arrayItemValues.Add(CString(QuickGetText(j, i)) + tBmpFlag);
		}
		if(m_dataTable.AppendRow(arrayItemValues) < 0)
		{
			TRACE(_T("Warning(row:%d):UpdateXMLData\n"), i);
			//return IAERROR_UNKNOWN;
		}
		//if(m_dataTable.XML_AddNewRow(arrayItemValues) < 0) return IAERROR_UNKNOWN;
	}

	return m_dataTable;
}

bool GetRange(int iCol, int iRow, int& iColTo, int& iRowTo, const CDim<CString>& values, CDim<bool>& flags)
{
	if(flags(iCol, iRow)) return false;//属于其他范围
	const CString& v = values(iCol, iRow);
	//获取iColTo
	iColTo = iCol;
	iRowTo = iRow;
	for(DWORD i=iCol+1; i<values.Dim(0); i++)
	{
		const CString& tv = values(i, iRow);
		if(tv.IsEmpty() || tv == v) iColTo = i;
		else break;
	}
	for(DWORD i=iRow+1; i<values.Dim(1); i++)
	{
		const CString& tv = values(iCol, i);
		if(tv.IsEmpty() || tv == v) iRowTo = i;
		else break;
	}

	for(int i=iCol; i<=iColTo; i++)
	{
		for(int j=iRow; j<=iRowTo; j++)
		{
			flags(i, j) = true;
		}
	}
	return true;
}
int  CIATable::SetXMLData(const CIADataTable &xmlData)
{
	CString szWatch = xmlData.GetXMLString();

	m_bIsSettingup = true;
	UnJoinTitles();

	int iCols = 0;

	m_dataTable.SetXMLString(xmlData.GetXMLString());
	//标题
	CStringArray arrayTopics;
	CStringArray arrayTitles;
	if(!m_dataTable.GetColumns(&arrayTopics, &arrayTitles)) return IAERROR_XML_NOTVALID;
	int iTHRows = -1;
	for(int i=0; i<=arrayTitles.GetUpperBound(); i++)
	{
		CStringExt ext(arrayTitles.GetAt(i), _T(","));
		int iCurCount = (int)ext.GetCount();
		if(iTHRows < iCurCount) iTHRows = iCurCount;
	}
	iCols = arrayTitles.GetUpperBound() + 1;
	SetNumberCols(iCols);//列数
	SetTH_NumberRows(iTHRows);//最大标题行数

	//设置所有标题
	CDim<CString> DimTempValues;
	DimTempValues.New(2, iCols, iTHRows);
	CDim<bool> DimTempFlags;
	DimTempFlags.New(2, iCols, iTHRows);
	DimTempFlags.Init(false);
	CString strTemp;
	for(int i=0; i<iCols; i++)
	{
		CStringExt ext(arrayTitles.GetAt(i), _T(","));
		ext.BeginEnum();
		int index = 0;
		while(ext.GetNext(strTemp)) DimTempValues(i, index++) = strTemp;

		//设置标题
		for(int j=0; j<iTHRows; j++)
		{
			QuickSetText(i, -iTHRows + j , DimTempValues(i, j));
			QuickSetFont(i, -iTHRows + j, m_nFontID_Title);
		}
	}
	//合并标题
	int iColTo, iRowTo;
	for(int i=0; i<iCols; i++)
	{
		for(int j=0; j<iTHRows; j++)
		{
			if(!GetRange(i, j, iColTo, iRowTo, DimTempValues, DimTempFlags)) continue;
			JoinCells(i, -iTHRows + j, iColTo, -iTHRows + iRowTo);
		}
	}

	CString v;
	//设置列宽
	for(int i=0; i<iCols; i++)
	{
		if(m_dataTable.GetColumnInfo(i, XMLPROP_TAB_COL_WIDTH, v)) SetColWidth(i, _tstoi(v));
	}

	int ret = UpdateUI_ByLocalXMLData();
	if(IAERROR_SUCCESS != ret) return ret;
	m_bIsSettingup = false;
	OnSettedXMLData();
	RedrawAll();
	return IAERROR_SUCCESS;
}
int CIATable::UpdateUI_ByLocalXMLData()
{
	DeleteAllItems();

	//设置内容,对齐方式
	CString szWatch = m_dataTable.GetXMLString();

	int nRowNum = m_dataTable.GetRowsNumber();
	SetNumberRows(nRowNum);

	CStringArray arrayItemValues;
	CString text, bmpflag;
	for(int i=0; i<nRowNum; ++i) 
	{                                 
		ApplyIADataTable_CellProperties(i);

		if(!m_dataTable.GetRowValues(i, arrayItemValues)) return IAERROR_XML_NOTVALID;
		for(int j=0; j<=arrayItemValues.GetUpperBound(); j++)
			XMLData_ApplyCellContent(j, i, arrayItemValues.GetAt(j));

		OnSettedXMLLine(i);
	}
	if(m_bAutoAdd_RowButtonAdd) AppendRow_ButtonAdd();
	return IAERROR_SUCCESS;
}
void CIATable::XMLData_ApplyCellContent(int col, int row, const CString& cellContent)
{
	CStringArray arrContent;
	CArray<enumResourceType, enumResourceType> arrType;
	XMLData_Parse(cellContent, arrContent, arrType);
	if(QuickSetText(col, row, XMLData_ToText(arrContent, arrType)) != UG_SUCCESS)
		AfxMessageBox(_T("QuickSetText ERROR!"));

	int c = _cpp_min(arrContent.GetCount(), arrType.GetCount());
	for(int i=0; i<c; ++i)
	{
		switch(arrType.GetAt(i))
		{
		case STRING:break;
		case BITMAP_PREDEF:
			QuickSetBitmap(col, row, GetBitmapIndexFromXMLFlag(arrContent.GetAt(i)));
			break;
		default:ASSERT(false);
		}
	}
}

CString CIATable::XMLData_ParseToText(const CString& cellValueFromXml)
{
	CStringArray arrContent;
	CArray<enumResourceType, enumResourceType> arrType;
	XMLData_Parse(cellValueFromXml, arrContent, arrType);
	return XMLData_ToText(arrContent, arrType);
}
void CIATable::XMLData_Parse(const CString& cellValueFromXml, CStringArray& arrContent, CArray<enumResourceType, enumResourceType>& arrType)
{
	arrContent.RemoveAll();
	arrType.RemoveAll();
	
	CStringArray arr;
	int iCur = 0;
	for(;;)
	{
		int iBegin = cellValueFromXml.Find(begin, iCur);
		if(iBegin < 0) break;
		int iEnd = cellValueFromXml.Find(end, iBegin);
		if(iEnd < 0) break;
		arr.Add(cellValueFromXml.Mid(iBegin+begin.GetLength(), iEnd-iBegin-begin.GetLength()));
		iCur = iEnd + end.GetLength();
	}

	iCur = 0;
	for(int i=0; i<arr.GetCount(); ++i)
	{
		const CString& s = arr.GetAt(i);
		CString sa = begin + s + end;
		int iStart = cellValueFromXml.Find(sa, iCur);
		if(iStart != iCur)
		{
			arrContent.Add(cellValueFromXml.Mid(iCur, iStart - iCur));
			arrType.Add(STRING);
		}
		if(s.Left(bitmap_predef.GetLength()) == bitmap_predef)
		{
			arrContent.Add(sa);
			arrType.Add(BITMAP_PREDEF);
		}

		iCur = iStart + sa.GetLength();
	}
	if(iCur < cellValueFromXml.GetLength())
	{
		arrContent.Add(cellValueFromXml.Right(cellValueFromXml.GetLength() - iCur));
		arrType.Add(STRING);
	}
}
CString CIATable::XMLData_ToText(const CStringArray& arrContent, const CArray<enumResourceType, enumResourceType>& arrType)
{
	int c = _cpp_min(arrContent.GetCount(), arrType.GetCount());

	CString ret;
	for(int i=0; i<c; ++i)
	{
		switch(arrType.GetAt(i))
		{
		case STRING:
		//case STRING_RESID:
			ret += arrContent.GetAt(i);
			break;
		}
	}
	return ret;
}

void CIATable::ApplyIADataTable_CellProperties(long row)
{
	int nColumnNum = GetNumberCols();

	CString type,value,readonly,align,ctrlType;
	CUGCell cell;
	for(int i=0; i<nColumnNum; i++)
	{
		QuickSetFont(i, row, m_nFontID_Content);
		GetCell(i, row, &cell);

		if(!m_dataTable.GetColumnInfo(i, XMLPROP_VALUETYPE, type)) type = _T("");
		if(m_dataTable.GetColumnInfo(i, XMLPROP_TAB_COL_ALIGN, align))
		{
			WORD wAlign = 0;
			CStringExt ext(align, _T("|"));
			CString t;
			ext.BeginEnum();
			while(ext.GetNext(t))
			{
				if(t.IsEmpty()) continue;
				if(t.CompareNoCase(_T("left")) == 0) wAlign |= 0;
				else if(t.CompareNoCase(_T("center")) == 0) wAlign |= UG_ALIGNCENTER;
				else if(t.CompareNoCase(_T("right")) == 0) wAlign |= UG_ALIGNRIGHT;
				else if(t.CompareNoCase(_T("top")) == 0) wAlign |= UG_ALIGNTOP;
				else if(t.CompareNoCase(_T("vcenter")) == 0) wAlign |= UG_ALIGNVCENTER;
				else if(t.CompareNoCase(_T("middle")) == 0) wAlign |= UG_ALIGNVCENTER;
				else if(t.CompareNoCase(_T("bottom")) == 0) wAlign |= UG_ALIGNBOTTOM;
			}
			cell.SetAlignment(wAlign);
		}
		else if(type == XMLVALUE_TYPE_INT || type == XMLVALUE_TYPE_FLOAT) cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		else cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);

		if(m_dataTable.GetColumnInfo(i, XMLPROP_VALUE, value))
		{
			if(type == XMLVALUE_TYPE_ICON)
			{
				cell.SetBitmap(GetBitmap(GetBitmapIndexFromXMLFlag(value)));
			}
			else cell.SetText(value);
		}
		if(m_dataTable.GetColumnInfo(i, XMLPROP_TAB_COL_READONLY, readonly))
		{
			readonly.MakeLower();
			if(readonly == _T("false")) cell.SetReadOnly(false);
			else if(readonly == _T("true")) cell.SetReadOnly(true);
		}
		if(m_dataTable.GetColumnInfo(i, XMLPROP_TAB_COL_CTRLTYPE, ctrlType))
		{
			if(ctrlType.CompareNoCase(XMLVALUE_CTRLTYPE_CHECKBOX) == 0)
			{
				cell.SetCellType(UGCT_CHECKBOX);
				CString ctrlData;
				if(m_dataTable.GetColumnInfo(i, XMLPROP_TAB_COL_CTRLDATA, ctrlData)) cell.SetLabelText(ctrlData);
			}
			else if(ctrlType.CompareNoCase(XMLVALUE_CTRLTYPE_DROPLIST) == 0)
			{
				cell.SetCellType(UGCT_DROPLIST);
				CString ctrlData;
				if(m_dataTable.GetColumnInfo(i, XMLPROP_TAB_COL_CTRLDATA, ctrlData))
				{
					ctrlData.Replace(_T("\\n"), _T("\n"));
					cell.SetLabelText(ctrlData);
				}
			}
		}
		
		SetCell(i, row, &cell);
	}
}
int	CIATable::InsertRow(long row)
{
	int ret = CUGCtrl::InsertRow(row);
	if(ret == UG_SUCCESS) ApplyIADataTable_CellProperties(row);
	return ret;
}
int	CIATable::AppendRow()
{
	int ret = CUGCtrl::AppendRow();
	if(ret == UG_SUCCESS) ApplyIADataTable_CellProperties(GetNumberRows() - 1);
	return ret;
}

void CIATable::OnSetup(void)
{
	CFont * pcFont = GetFont(m_nFontID_Content); 

	CUGCell	cell;
	GetGridDefault(&cell); 
	cell.SetTextColor(RGB(0,0,0)); 
	cell.SetFont(pcFont);
	cell.SetReadOnly(true);
	SetGridDefault(&cell); 

	CString AppPath = CApplication::GetApplicationPath();
	AppPath += _T('\\');
	CString ebs = _T("");
	for(int i=0; i<BITMAP_COUNT; i++)
	{
		if(i == BITMAP_NULL) continue;
		CString f = GetBitmapFile((enumBitmapIndex)i);
		if(f && _tcslen(f))
		{
			m_pBitmaps[i] = AddBitmap(AppPath + f);
			if(m_pBitmaps[i] < 0) ebs = ebs + f + _T("\r\n");
		}
	}
	if(!ebs.IsEmpty()) AfxMessageBox(_T("CIATable::OnSetup:以下BMP文件未能打开:\r\n") + ebs);
}

void CIATable::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed)
{
	const long colNum = this->GetNumberCols();
	const long rowNum = this->GetNumberRows();
	ChangeRowBackGroundColor(colNum, rowNum, row);
	if (row < 0) 
		ChangeSelectedBKColor(colNum, rowNum);
		

	if(updn == FALSE)//只有当鼠标放开的时候才执行
	{
		CUGCell cell;
		GetCell(col, row, &cell);
		CBitmap* pBitmap = cell.GetBitmap();
		
		if(pBitmap == GetBitmapObject(BITMAP_DELETE)) Action_DeleteRow(row);//删除
		else if(pBitmap == GetBitmapObject(BITMAP_INF)) Action_DisplayRowInf(row);//信息
		else if(pBitmap == GetBitmapObject(BITMAP_ADD)) Action_AddRow();//添加
		else if(pBitmap == GetBitmapObject(BITMAP_ERROR)) Action_Error(row, col);//错误
		else if(pBitmap == GetBitmapObject(BITMAP_MANUAL_GO)) Action_Manual_Run(row, col);// 手工任务
		else if(pBitmap == GetBitmapObject(BITMAP_MANUAL_NEXT)) Action_Manual_RunNext(row, col);
		else if(pBitmap == GetBitmapObject(BITMAP_MANUAL_OK)) Action_Manual_Finished(row, col);
		else if(pBitmap == GetBitmapObject(BITMAP_AUTO_GO)) Action_Auto_Run(row, col);//自动任务
		else if(pBitmap == GetBitmapObject(BITMAP_AUTO_NEXT)) Action_Auto_RunNext(row, col);
		else if(pBitmap == GetBitmapObject(BITMAP_AUTO_OK)) Action_Auto_Finished(row, col);
		else if(pBitmap == GetBitmapObject(BITMAP_RUNNING)) Action_Running(row, col);//正在运行
		else CUGCtrl::OnLClicked(col,row, updn, rect, point, processed);
	}
}

int CIATable::OnCellTypeNotify(long ID, int col,long row, long msg, long param)
{
	return CUGCtrl::OnCellTypeNotify(ID, col, row, msg, param);
}

void CIATable::ChangeRowBackGroundColor(long colNum, long rowNum, long currRow) {
	ChangeSelectedBKColor(colNum, rowNum);
	ChangeCurrentBKColor(colNum, currRow);
	RedrawAll();
}

void CIATable::ChangeSelectedBKColor(long colNum, long rowNum) {
	
		for (int r=0; r<rowNum; r++) {
			for (int c=0; c<colNum; c++) {
			CUGCell bkcell;
			GetCell(c, r, &bkcell);
			bkcell.SetReadOnly(false);
			COLORREF colRef = bkcell.GetBackColor();
			if (colRef==::GetSysColor(COLOR_WINDOW))
			{
				continue;
			}
			bkcell.SetBackColor(::GetSysColor(COLOR_WINDOW));
			SetCell(c, r, &bkcell);
		}
	}
}
void CIATable::ChangeCurrentBKColor(long colNum, long currRow) {
	if (currRow<0) return;
	
	for (int c=0; c<colNum; c++) {
		CUGCell bkcell;
		GetCell(c, currRow, &bkcell);
		bkcell.SetReadOnly(false);
		bkcell.SetBackColor(RGB(68,163,219)); //天蓝色
		SetCell(c, currRow, &bkcell);
	}
}

int  CIATable::OnCanSizeCol(int col)
{
	CString canSize;
	if(m_dataTable.GetColumnInfo(col, XMLPROP_TAB_COL_CANSIZE, canSize))
	{
		canSize.MakeLower();
		if(canSize == _T("false")) return FALSE;
		else if(canSize == _T("true")) return TRUE;
	}

	return CUGCtrl::OnCanSizeCol(col);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CIADataCurve2D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CIADataCurve2D::SetXMLString(LPCTSTR cXMLString)
{
	if(NULL == cXMLString || _tcslen(cXMLString) == 0) cXMLString = DefaultXML();
	m_bInit = xml.LoadFromString(cXMLString);
	return m_bInit?IAERROR_SUCCESS:IAERROR_XML_SYSERROR;
}

CString CIADataCurve2D::GetXMLString() const
{
	return xml.Siblings[0]->GetOuterXML();//不用xml.GetDoc()
}/*
CString CIADataCurve2D::GetProperty(LPCTSTR prop)
{
	return xml.Siblings[0]->GetProperty(prop);
}
void CIADataCurve2D::SetProperty(LPCTSTR prop, LPCTSTR val)
{
	return xml.Siblings[0]->SetProperty(prop, val);
}//*/
CString CIADataCurve2D::GetCurveCaption(LPCTSTR type) const
{
	CTWenXMLSingle* pSingle = xml.Path(Path_CaptionDefine());
	if(pSingle)
	{
		for (int i=0; i<pSingle->Children.GetCount(); i++)
		{
			CTWenXMLSingle* pChildSingle = pSingle->Children.GetAt(i);
			if(pChildSingle->GetProperty(_T("Type")) == type) return pChildSingle->GetInnerText();
		}
	}
	return _T("");
}
void CIADataCurve2D::SetCurveCaption(LPCTSTR type, LPCTSTR caption)
{
	CTWenXMLSingle* pSingle = xml.Path(Path_CaptionDefine());
	if(pSingle)
	{
		for (int i=0; i<pSingle->Children.GetCount(); i++)
		{
			CTWenXMLSingle* pChildSingle = pSingle->Children.GetAt(i);
			if(pChildSingle->GetProperty(_T("Type")) == type)
			{
				pChildSingle->SetInnerText(caption);
				return;
			}
		}
		//new
		CString s;
		s.Format(_T("<Item Type='%s'>%s</Item>"), type, caption);
		pSingle->AppendChild(s);
	}
}
bool CIADataCurve2D::GetCurve2DSingle(LPCTSTR type, CIADataCurve2DSingle& single) const
{
	CTWenXMLSingle* pSingle = xml.Siblings[0];
	if(pSingle)
	{
		for (int i=0; i<pSingle->Children.GetCount(); i++)
		{
			CTWenXMLSingle* pChildSingle = pSingle->Children.GetAt(i);
			if(pChildSingle->GetProperty(_T("Type")) == type)
			{
				single.m_pSingle = pChildSingle;
				return true;
			}
		}
	}
	return false;
}
bool CIADataCurve2D::AutoCurve2DSingle(LPCTSTR type, CIADataCurve2DSingle& single)
{
	CTWenXMLSingle* pSingle = xml.Siblings[0];
	if(pSingle)
	{
		if(GetCurve2DSingle(type, single)) return true;
		//new
		CString s;
		s.Format(_T("<%s Type='%s'></%s>"), CIADataCurve2DSingle::strType, type, CIADataCurve2DSingle::strType);
		pSingle->AppendChild(s);
		for (int i=pSingle->Children.GetCount()-1; i>-1; i--)
		{
			CTWenXMLSingle* pChildSingle = pSingle->Children.GetAt(i);
			if(pChildSingle->GetProperty(_T("Type")) == type)
			{
				single.m_pSingle = pChildSingle;
				return true;
			}
		}
	}
	return false;
}

void CIADataCurve2D::BeginEnum()
{
	m_iEnum = 0;
}
bool CIADataCurve2D::GetNext(CString& type, CIADataCurve2DSingle& single)
{
	while(m_iEnum < xml.Siblings[0]->Children.GetCount())
	{
		CTWenXMLSingle* pSingle = xml.Siblings[0]->Children.GetAt(m_iEnum++);
		if(pSingle && pSingle->GetNodeName() == CIADataCurve2DSingle::strType)
		{
			type = pSingle->GetProperty(_T("Type"));
			single.m_pSingle = pSingle;
			return true;
		}
	}
	return false;
}

void CIADataCurve2D::BeginEnum(long& pos) const
{
	pos = 0;
}
bool CIADataCurve2D::GetNext(long& pos, CString& type, CIADataCurve2DSingle& single) const
{
	if(pos < 0)
	{
		ASSERT(false);
		return false;
	}
	while(pos < xml.Siblings[0]->Children.GetCount())
	{
		CTWenXMLSingle* pSingle = xml.Siblings[0]->Children.GetAt(pos++);
		if(pSingle && pSingle->GetNodeName() == CIADataCurve2DSingle::strType)
		{
			type = pSingle->GetProperty(_T("Type"));
			single.m_pSingle = pSingle;
			return true;
		}
	}
	return false;
}
bool CIADataCurve2D::QuickGetCurve2DFile(LPCTSTR type, LPCTSTR name, CString& filename) const
{
	CString strType;
	CIADataCurve2DSingle single;
	long pos;
	BeginEnum(pos);
	while(GetNext(pos, strType, single))
	{
		if(strType != type) continue;
		CString strName, strFileName;
		single.BeginEnum();
		while(single.GetNext(strName, strFileName))
		{
			if(strName != name) continue;
			filename = strFileName;
			return true;
		}
	}
	return false;
}
bool CIADataCurve2D::QuickSetCurve2DFile(LPCTSTR type, LPCTSTR name, LPCTSTR filename)
{
	CIADataCurve2DSingle single;
	if(AutoCurve2DSingle(type, single))
	{
		single.SetItemByName(name, filename);
		return true;
	}
	else return false;
}
bool CIADataCurve2D::QuickDeleteCurve2DFile(LPCTSTR type, LPCTSTR name)
{
	CIADataCurve2DSingle single;
	if(GetCurve2DSingle(type, single)) return single.DeleteItemByName(name);
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CIADataCurve2DSingle
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
//CString CIADataCurve2DSingle::strFileExt=_T("csv");
CString CIADataCurve2DSingle::strType=_T("Curve2D");

bool CIADataCurve2DSingle::GetProperty(LPCTSTR prop, CString& val)
{
	val = _T("");
	if(!m_pSingle) return false;
	val = m_pSingle->GetProperty(prop);
	return true;
}
bool CIADataCurve2DSingle::SetProperty(LPCTSTR prop, LPCTSTR val)
{
	if(!m_pSingle) return false;
	m_pSingle->SetProperty(prop, val);
	return true;
}

CTWenXMLSingle* CIADataCurve2DSingle::GetItem(LPCTSTR name) const
{
	if(!m_pSingle) return NULL;
	for(int i=0; i<m_pSingle->Children.GetCount(); i++)
	{
		CTWenXMLSingle* pChildSingle = m_pSingle->Children.GetAt(i);
		if(pChildSingle->GetProperty(_T("name")) == name) return pChildSingle;
	}
	return NULL;
}
bool  CIADataCurve2DSingle::ItemExist(LPCTSTR name)
{
	return GetItem(name) != NULL;
}

bool CIADataCurve2DSingle::GetItemByName(LPCTSTR name, CString& fileName)
{
	fileName = _T("");
	CTWenXMLSingle* pSingle = GetItem(name);
	if(!pSingle) return false;
	fileName = pSingle->GetInnerText();
	return true;
}

void  CIADataCurve2DSingle::SetItemByName(LPCTSTR name, LPCTSTR fileName)
{
	CTWenXMLSingle* pSingle = GetItem(name);
	if(pSingle) pSingle->SetInnerText(fileName);
	else AppendItem(name, fileName);
}

void CIADataCurve2DSingle::AppendItem(LPCTSTR name, LPCTSTR fileName)
{
	if(!m_pSingle) return;
	CString s;
	s.Format(_T("<Item name='%s'>%s</Item>"), name, fileName);
	m_pSingle->AppendChild(s);
}

bool CIADataCurve2DSingle::DeleteItemByName(LPCTSTR name)
{
	CTWenXMLSingle* pSingle = GetItem(name);
	if(!pSingle) return false;
	pSingle->Delete();
	return true;
}

void CIADataCurve2DSingle::BeginEnum()
{
	m_iEnum = 0;
}
bool CIADataCurve2DSingle::GetNext(CString& name, CString& fileName)
{
	name = _T("");
	fileName = _T("");
	if(!m_pSingle || m_iEnum >= GetCount()) return false;
	m_pEnumSingle = m_pSingle->Children.GetAt(m_iEnum++);
	name = m_pEnumSingle->GetProperty(_T("name"));
	fileName = m_pEnumSingle->GetInnerText();
	return true;
}
bool CIADataCurve2DSingle::SetEnum(LPCTSTR fileName)
{
	if(!m_pEnumSingle) return false;
	m_pEnumSingle->SetInnerText(fileName);
	return true;
}
bool CIADataCurve2DSingle::GetEnumProperty(LPCTSTR prop, CString& val) const
{
	val = _T("");
	if(!m_pEnumSingle) return false;
	val = m_pEnumSingle->GetProperty(prop);
	return true;
}
bool CIADataCurve2DSingle::SetEnumProperty(LPCTSTR prop, LPCTSTR val)
{
	val = _T("");
	if(!m_pEnumSingle) return false;
	m_pEnumSingle->SetProperty(prop, val);
	return true;
}
//////////////////////////////////////////////////////////////////////////
// CIAFileCurve2D 类
//////////////////////////////////////////////////////////////////////////

char CIAFileCurve2D::strSep=',';

BOOL CIAFileCurve2D::Open(LPCTSTR file)
{
	if(m_file.m_hFile != CFile::hFileNull) m_file.Close();
	BOOL ret = m_file.Open(file,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
	if(!ret) return FALSE;
	/*if(m_file.GetLength() == 0)//写入标题行
	{
		TWen_USES_CONVERSION;
		CString head;
		head.Format(_T("x%cy%c时间\n"), strSep, strSep);
		LPCSTR s=TWen_CT2A(head);
		m_file.Write(s,strlen(s));
	}//*/
	return true;
}
void CIAFileCurve2D::Close()
{
	if(m_file.m_hFile != CFile::hFileNull) m_file.Close();
	if(m_pcChars){delete []m_pcChars;m_pcChars=NULL;}
}

BOOL CIAFileCurve2D::ApppendPoint(double x, double y)
{
	if(m_file.m_hFile == CFile::hFileNull) return FALSE;
	m_file.SeekToEnd();
	CString line;
	//line.Format(_T("%.5f%c%.5f%c%s\n"), x, strSep, y, strSep, (LPCTSTR)CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")));
	line.Format(_T("%.5f%c%.5f\n"), x, strSep, y);
	TWen_USES_CONVERSION;
	LPCSTR s=TWen_CT2A(line);
	m_file.Write(s,strlen(s));
	return true;
}

void CIAFileCurve2D::BeginEnum()
{
	if(m_file.m_hFile == CFile::hFileNull) return;
	//m_file.SeekToBegin();
	if(m_pcChars){delete []m_pcChars;m_pcChars=NULL;}
	m_pcChars = new char[(UINT)m_file.GetLength()];
	m_file.SeekToBegin();
	m_file.Read(m_pcChars, (UINT)m_file.GetLength());
	{
		TWen_USES_CONVERSION;
		m_Ext.SetSRCString(TWen_CA2T(m_pcChars));
	}
	m_Ext.BeginEnum();
}
bool CIAFileCurve2D::Read(CFile& file,CString& content,char token)
{
	char c;
	content = _T("");
	char maxline[102400];
	int count = 0;
	while(true)
	{
		UINT uc = file.Read(&c, sizeof(char));
		if(uc < sizeof(char)) return false;
		if(token == c) break;
		else maxline[count++]=c;
	}
	if(count == 0) return true;
	maxline[count]=0;
	TWen_USES_CONVERSION;
	content = TWen_CA2T(maxline);
	return true;
}

bool CIAFileCurve2D::GetNext(double& x, double& y, CTime& time)
{/*
	CString s;
	if(!Read(m_file, s, strSep)) return false;
	x = _tstof(s);
	if(!Read(m_file, s, '\n')) return false;
	y = _tstof(s);
/*
	if(!Read(m_file, s, strSep)) return false;
	y = _tstof(s);
	if(!Read(m_file, s, '\n')) return false;
	CStringExt::ToTime(s, time);//*/
	CString line;
	if(!m_Ext.GetNext(line)) return false;
	CString sx, sy, stime;
	if(!CStringExt::Split(line, _T(","), 2, &sx, &sy)) return false;
	x = _tstof(sx);
	y = _tstof(sy);
	//CStringExt::ToTime(stime, time);
	return true;
}
