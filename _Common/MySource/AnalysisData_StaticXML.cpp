#include "stdafx.h"
#include "AnalysisData.h"
#include "CStringExt.h"

CString CAnalysisData::static_NodeName(CAnalysisData::CONST_NODE pNode)
{
	if(pNode == NULL) return _T("");
	return pNode->getNodeName();
}
CAnalysisData::enumNodeType CAnalysisData::static_NodeType(CAnalysisData::CONST_NODE pNode)
{
	return (CAnalysisData::enumNodeType)pNode->getNodeType();
}

DWORD CAnalysisData::static_ChildrenCount(CAnalysisData::CONST_NODE pParent)
{
	const DOMNodeList * pList = NULL;
	if(pParent && (pList = pParent->getChildNodes())) return pList->getLength();
	return 0;
}
CAnalysisData::NODE CAnalysisData::static_ChildNode(CAnalysisData::NODE pParent, DWORD dwIndex)
{
	DOMNodeList* pList = NULL;
	if(pParent && (pList = pParent->getChildNodes()) && (pList->getLength() > dwIndex))
	{
		return pList->item(dwIndex);
	}
	return NULL;
}
void CAnalysisData::static_FisrtChild(CAnalysisData::NODE pParent, CAnalysisData::NODE& pos)
{
	if(pParent) pos = pParent->getFirstChild();
	else pos = NULL;
}
CAnalysisData::NODE CAnalysisData::static_NextChild(CAnalysisData::NODE& pos)
{
	CAnalysisData::NODE ret = pos;
	if(pos)	pos = pos->getNextSibling();
	return ret;
}
void CAnalysisData::static_LastChild(CAnalysisData::NODE pParent, CAnalysisData::NODE& pos)
{
	if(pParent) pos = pParent->getLastChild();
	else pos = NULL;
}
CAnalysisData::NODE CAnalysisData::static_PrevChild(CAnalysisData::NODE& pos)
{
	CAnalysisData::NODE ret = pos;
	if(pos)	pos = pos->getPreviousSibling();
	return ret;
}
void CAnalysisData::static_FisrtChild_Const(CAnalysisData::CONST_NODE pParent, CAnalysisData::CONST_NODE& pos)
{
	if(pParent) pos = pParent->getFirstChild();
	else pos = NULL;
}
CAnalysisData::CONST_NODE CAnalysisData::static_NextChild_Const(CAnalysisData::CONST_NODE& pos)
{
	CAnalysisData::CONST_NODE ret = pos;
	if(pos)	pos = pos->getNextSibling();
	return ret;
}
void CAnalysisData::static_LastChild_Const(CAnalysisData::CONST_NODE pParent, CAnalysisData::CONST_NODE& pos)
{
	if(pParent) pos = pParent->getLastChild();
	else pos = NULL;
}
CAnalysisData::CONST_NODE CAnalysisData::static_PrevChild_Const(CAnalysisData::CONST_NODE& pos)
{
	CAnalysisData::CONST_NODE ret = pos;
	if(pos)	pos = pos->getPreviousSibling();
	return ret;
}






DWORD CAnalysisData::static_PropertiesCount(CAnalysisData::CONST_NODE pParent)
{
	if(pParent)
	{
		const DOMNamedNodeMap* pMap = pParent->getAttributes();
		if(pMap) return pMap->getLength();
	}
	return 0;
}
CAnalysisData::NODE CAnalysisData::static_NodeProperty(CAnalysisData::NODE pParent, DWORD dwIndex)
{
	DOMNamedNodeMap* pMap = NULL;
	if(pParent && (pMap = pParent->getAttributes()) && (pMap->getLength() > dwIndex))
	{
		return pMap->item(dwIndex);
	}
	return NULL;
}



CAnalysisData::NODE CAnalysisData::static_SubNode(CAnalysisData::NODE pParent, const CString& strSubTag, bool bAutoCreate)
{
	if(pParent == NULL) return NULL;
	CAnalysisData::NODE pChild = pParent->getFirstChild();
	while(pChild)
	{
		if(pChild->getNodeName() == strSubTag) return pChild;
		pChild = pChild->getNextSibling();
	}
	if(bAutoCreate)
	{
		if(pParent->getOwnerDocument() == NULL) return NULL;
		pChild = pParent->getOwnerDocument()->createElement(strSubTag);
		return pParent->appendChild(pChild);
	}
	else return NULL;
}
CAnalysisData::NODE CAnalysisData::static_NodeProperty(CAnalysisData::NODE pNode, const CString& strProperty)
{
	if(pNode == NULL) return NULL;
	DOMNamedNodeMap* pMap = pNode->getAttributes();
	if(pMap == NULL) return NULL;
	for(UINT i=0; i<pMap->getLength(); i++)
	{
		CAnalysisData::NODE pProperty = pMap->item(i);
		if(pProperty->getNodeName() == strProperty) return pProperty;
	}
	return NULL;
}
CAnalysisData::NODE CAnalysisData::static_Path(CAnalysisData::NODE pRoot, LPCTSTR strXMLPath, bool bAutoCreate)
{
	if(pRoot == NULL) return NULL;
	if(strXMLPath == NULL) return pRoot;
	CString paths(strXMLPath);
	paths.Replace('/', '\\');
	CStringExt ext(paths, _T("\\"));
	ext.BeginEnum();
	CString path;
	CAnalysisData::NODE pNode = pRoot;
	bool bFirst = true;
	while(ext.GetNext(path))
	{
		if(path.IsEmpty()) continue;
		if(bFirst){bFirst = false;continue;}//去除根目录
		pNode = static_SubNode(pNode, path, bAutoCreate);
		if(pNode == NULL) return NULL;
	}
	return pNode;
}
CAnalysisData::NODE CAnalysisData::static_Path(CAnalysisData::PARSER pParser, LPCTSTR strXMLPath, bool bAutoCreate)
{
	if(pParser == NULL || pParser->getDocument() == NULL || pParser->getDocument()->getDocumentElement() == NULL) return NULL;
	return static_Path(pParser->getDocument()->getDocumentElement(), strXMLPath, bAutoCreate);
}

#include <xercesc\framework\MemBufFormatTarget.hpp>
int CAnalysisData::static_GetOuterXML(CAnalysisData::CONST_NODE pNode, CString& xml)
{
	try
	{
		if(pNode == NULL) return IAERROR_XML_ITEMNOTFOUND;

		//MemBufFormatTarget target; 

		DOMImplementation* pImpl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
		DOMWriter* pWriter = ((DOMImplementationLS*)pImpl)->createDOMWriter();
		pWriter->setEncoding(XMLString::transcode("GB2312"));
		//pWriter->writeNode(&target, *pNode);

		//XMLFormatTarget *mytarget = new LocalFileFormatTarget("d:/example.xml"); // 这一行创建一个XMLTarget,指向你要写入的文件路径
		//pWriter->writeNode(mytarget, *pNode);// doc是你的文档对象
		xml = pWriter->writeToString(*pNode);

		pWriter->release();
		//xml = target.getRawBuffer();
		return IAERROR_SUCCESS;
	}
	catch(...){return IAERROR_XML_SYSERROR;}
}
int CAnalysisData::static_GetInnerXML(CAnalysisData::CONST_NODE pNode, CString& xml)
{
	if(pNode == NULL) return IAERROR_XML_ITEMNOTFOUND;
	xml = _T("");
	pNode = pNode->getFirstChild();
	while(pNode)
	{
		CString sxml;
		static_GetOuterXML(pNode, sxml);
		xml += sxml;
		pNode = pNode->getNextSibling();
	}
	return IAERROR_SUCCESS;
}
int CAnalysisData::static_SetOuterXML(CAnalysisData::NODE pNode, LPCTSTR strXMLString)
{
	if(pNode == NULL || pNode->getParentNode() == NULL || pNode->getOwnerDocument() == NULL) return IAERROR_XML_SYSERROR;
	PARSER parser = static_LoadXML(CString(_T("<Root>")) + strXMLString + _T("</Root>"));
	if(!parser) return IAERROR_XML_SYSERROR;
	CAnalysisData::NODE pNewNode = static_Path(parser, _T("Root"), false);
	if(pNewNode == NULL || pNewNode->getFirstChild() == NULL)
	{
		delete parser;
		return IAERROR_XML_SYSERROR;
	}
	pNewNode = pNewNode->getFirstChild();
	CAnalysisData::NODE pParent = pNode->getParentNode();
	CAnalysisData::NODE pPos = pNode->getNextSibling();
	pParent->replaceChild(pNode->getOwnerDocument()->importNode(pNewNode, true) , pNode);
	pNewNode = pNewNode->getNextSibling();
	while(pNewNode)
	{
		pParent->insertBefore(pNode->getOwnerDocument()->importNode(pNewNode, true), pPos);
		pNewNode = pNewNode->getNextSibling();
	}
	delete parser;
	return IAERROR_SUCCESS;
}
int CAnalysisData::static_SetInnerXML(CAnalysisData::NODE pNode, LPCTSTR strXMLString)
{
	if(!pNode || !pNode->getOwnerDocument()) return IAERROR_XML_SYSERROR;
	PARSER parser = static_LoadXML(CString(_T("<Root>")) + strXMLString + _T("</Root>"));
	if(!parser) return IAERROR_XML_SYSERROR;
	CAnalysisData::NODE pNewNode = static_Path(parser, _T("Root"), false);
	if(pNewNode == NULL)
	{
		delete parser;
		return IAERROR_XML_SYSERROR;
	}
	pNode->setTextContent(NULL);
	pNewNode = pNewNode->getFirstChild();
	while(pNewNode)
	{
		pNode->appendChild(pNode->getOwnerDocument()->importNode(pNewNode, true));
		pNewNode = pNewNode->getNextSibling();
	}
	delete parser;
	return IAERROR_SUCCESS;
}
int CAnalysisData::static_Replace(NODE pOldNode, NODE pNewNode)
{
	try
	{
		if(pNewNode == NULL || pOldNode == NULL || pOldNode->getParentNode() == NULL || pOldNode->getOwnerDocument() == NULL) return IAERROR_XML_SYSERROR;
		pOldNode->getParentNode()->replaceChild(pOldNode->getOwnerDocument()->importNode(pNewNode, true) , pOldNode);
		return IAERROR_SUCCESS;
	}
	catch(...){return IAERROR_XML_SYSERROR;}
}
int CAnalysisData::static_Remove(NODE pNode)
{
	try
	{
		if(pNode == NULL || pNode->getParentNode() == NULL) return IAERROR_XML_SYSERROR;
		pNode->getParentNode()->removeChild(pNode);
		return IAERROR_SUCCESS;
	}
	catch(...){return IAERROR_XML_SYSERROR;}
}

CAnalysisData::PARSER CAnalysisData::static_LoadXML(LPCTSTR xml)
{
	if(xml == NULL || _tcslen(xml) == 0) return NULL;

	PARSER pParser = new _PARSER;
	if(pParser == NULL) return NULL;

	try
	{
		pParser->useScanner(XMLUni::fgWFXMLScanner);
		pParser->setDoNamespaces(false);
	
		MemBufInputSource oInputSource((XMLByte*)xml, _tcslen(xml)*sizeof(TCHAR), _T("XML_PARSE"));
		oInputSource.setEncoding(_T("UTF-16"));
		pParser->parse(oInputSource);
		if(pParser->getErrorCount())
		{
			delete pParser;
			return NULL;
		}
		return pParser;
	}
	catch(...)
	{
		delete pParser;
		return NULL;
	}
}
CAnalysisData::PARSER CAnalysisData::static_LoadXMLFile(LPCTSTR xmlFile)
{
	if(xmlFile == NULL || _tcslen(xmlFile) == 0) return NULL;

	PARSER pParser = new _PARSER;
	if(pParser == NULL) return NULL;

	try
	{
		pParser->useScanner(XMLUni::fgWFXMLScanner);
		pParser->setDoNamespaces(false);
	
		pParser->parse(xmlFile);
		if(pParser->getErrorCount())
		{
			delete pParser;
			return NULL;
		}
		return pParser;
	}
	catch(...)
	{
		delete pParser;
		return NULL;
	}
}
int CAnalysisData::static_Save(CAnalysisData::CONST_PARSER pParser, LPCTSTR xmlFile)
{
	try
	{
		if(xmlFile == NULL || _tcslen(xmlFile) == 0 || pParser == NULL) return IAERROR_XML_SYSERROR;

		LocalFileFormatTarget target(xmlFile); 

		DOMImplementation* pImpl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
		DOMWriter* pWriter = ((DOMImplementationLS*)pImpl)->createDOMWriter();
		if(pWriter->canSetFeature(XMLUni::fgDOMXMLDeclaration,true)) pWriter->setFeature(XMLUni::fgDOMXMLDeclaration, true);
		if(pWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true)) pWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true); 

		pWriter->setEncoding(_T("UTF-8"));

		pWriter->writeNode(&target, *((PARSER)pParser)->getDocument());
		pWriter->release();
		return IAERROR_SUCCESS;
	}
	catch(...){return IAERROR_XML_SYSERROR;}
}
int CAnalysisData::static_Save(CAnalysisData::CONST_NODE pNode, LPCTSTR xmlFile)
{
	try
	{
		if(xmlFile == NULL || _tcslen(xmlFile) == 0 || pNode == NULL) return IAERROR_XML_SYSERROR;

		LocalFileFormatTarget target(xmlFile); 

		DOMImplementation* pImpl = DOMImplementationRegistry::getDOMImplementation(_T("LS"));
		DOMWriter* pWriter = ((DOMImplementationLS*)pImpl)->createDOMWriter();
		if(pWriter->canSetFeature(XMLUni::fgDOMXMLDeclaration,true)) pWriter->setFeature(XMLUni::fgDOMXMLDeclaration, true);
		if(pWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true)) pWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true); 

		pWriter->setEncoding(_T("UTF-8"));

		//pWriter->writeNode(&target, *m_pParser->getDocument()->getDocumentElement());
		pWriter->writeNode(&target, *((NODE)pNode));
		pWriter->release();
		return IAERROR_SUCCESS;
	}
	catch(...){return IAERROR_XML_SYSERROR;}
}

bool CAnalysisData::static_GetValue(CAnalysisData::CONST_NODE pNode, CString& value)
{
	if(pNode == NULL) return false;
	value = pNode->getTextContent();
	return true;
}
bool CAnalysisData::static_SetValue(CAnalysisData::NODE pNode, LPCTSTR value)
{
	if(pNode == NULL) return false;
	pNode->setTextContent(value);
	return true;
}
bool CAnalysisData::static_GetProperty(CAnalysisData::CONST_NODE pNode, const CString& strProperty, CString& value)
{
	if(pNode == NULL) return false;
	pNode = static_NodeProperty((CAnalysisData::NODE)pNode, strProperty);
	if(pNode == NULL) return false;
	value = pNode->getNodeValue();
	return true;
}
bool CAnalysisData::static_SetProperty(CAnalysisData::NODE pNode, const CString& strProperty, LPCTSTR value)
{
	if(pNode == NULL) return false;
	
	CAnalysisData::NODE pProperty = static_NodeProperty(pNode, strProperty);
	if(pProperty) pProperty->setNodeValue(value);
	else
	{
		if(pNode->getOwnerDocument() == NULL) return false;
		DOMNamedNodeMap* pMap = pNode->getAttributes();
		if(pMap == NULL) return false;
		CAnalysisData::NODE pProperty = pNode->getOwnerDocument()->createAttribute(strProperty);
		if(pProperty == NULL) return false;
		pProperty->setNodeValue(value);
		pMap->setNamedItem(pProperty);
	}
	return true;
}

bool CAnalysisData::static_IsEqual(CAnalysisData::CONST_NODE pNode1, CAnalysisData::CONST_NODE pNode2)
{
	if(pNode1 == pNode2) return true;
	if(pNode1 == NULL) return false;
	return pNode1->isEqualNode(pNode2);
}

void CAnalysisData::static_Assign(CAnalysisData::NODE pNode1, CAnalysisData::CONST_NODE pNode2, DWORD dwMode)
{
	if(!pNode1 || !pNode2) return;

	if((static_NodeType(pNode1) != ELEMENT_NODE) || (static_NodeType(pNode2) != ELEMENT_NODE)) return;

	if(dwMode & ASSIGN_SELF)
	{
		//设置属性
		const DOMNamedNodeMap* pMap2 = pNode2->getAttributes();
		if(pMap2)
		{
			for(UINT i=0; i<pMap2->getLength(); i++)
			{
				CAnalysisData::CONST_NODE pProperty2 = pMap2->item(i);
				CAnalysisData::NODE pProperty1 = static_NodeProperty(pNode1, pProperty2->getNodeName());
				if(pProperty1 && (dwMode & ASSIGN_PROPERTY_EXISTING)) pProperty1->setTextContent(pProperty2->getTextContent());
				else if(!pProperty1 && (dwMode & ASSIGN_PROPERTY_NOTEXISTING)) static_SetProperty(pNode1, pProperty2->getNodeName(), pProperty2->getTextContent());
			}
		}
	}

	enumType eType1 = static_Type(pNode1);
	enumType eType2 = static_Type(pNode2);

	if(eType1 != eType2) return;

#if defined(_DEBUG) || defined(DEBUG)
	CString xml1,xml2;
	static_GetOuterXML(pNode1, xml1);
	static_GetOuterXML(pNode2, xml2);
#endif
	//根据数据类型决定子条目处理方式
	switch(eType1)
	{
	case ADT_OBJECT_TABLE:
	case ADT_OBJECT_CURVE2D:
		//拷贝所有子条目
		{
			CString xml;
			if(CAnalysisData::static_GetInnerXML(pNode2, xml) == IAERROR_SUCCESS)
			{
				static_SetInnerXML(pNode1, xml);
			}
		}
		break;
	case ADT_OBJECT:
	case ADT_UNKNOWN:
		//拷贝所有子条目
		if(dwMode & ASSIGN_CHILDREN)
		{
			CAnalysisData::CONST_NODE pChild2 = pNode2->getFirstChild();
			while(pChild2)
			{
				if(static_NodeType(pChild2) == ELEMENT_NODE)
				{
					CAnalysisData::NODE pChild1 = static_SubNode(pNode1, pChild2->getNodeName(), (dwMode & ASSIGN_NODE_NOTEXISTING) != FALSE);
					static_Assign(pChild1, pChild2, dwMode | ASSIGN_SELF | ASSIGN_CHILDREN);
				}
				pChild2 = pChild2->getNextSibling();
			}
		}
		break;
	default:
		if(dwMode & ASSIGN_SELF)
		{
			//设置值
			pNode1->setTextContent(pNode2->getTextContent());
		}
	}
}