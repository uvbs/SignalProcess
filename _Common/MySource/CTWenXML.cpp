
#include "stdafx.h"
#include "CTWenXML.h"
#include "CStringExt.h"
#include "CStringConvert.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CStringTokenEx
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TCHAR CStringTokenEx::GetNext(CString& value)
{
	return GetNext(value, Tokens);
}
TCHAR CStringTokenEx::GetNext(CString& value, LPCTSTR tokens)
{
	value = _T("");
	if(m_lEnumPos >= SRCString.GetLength()) return NULL;
	for(;;)
	{
		TCHAR c = SRCString[m_lEnumPos++];
		for(size_t i=0; i<_tcslen(tokens); i++)
		{
			if(c == tokens[i]) return c;
		}
		value += c;
		if(m_lEnumPos >= SRCString.GetLength()) break;
	}
	return NULL;
}
TCHAR CStringTokenEx::GetNext(CString& value, TCHAR token)
{
	value = _T("");
	if(m_lEnumPos >= SRCString.GetLength()) return NULL;
	for(;;)
	{
		TCHAR c = SRCString[m_lEnumPos++];
		if(c == token) return c;
		value += c;
		if(m_lEnumPos >= SRCString.GetLength()) break;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTWenXMLElement
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWenXMLElement::stc_Prefix = _T("\t");

bool CTWenXMLElement::static_GetFormatString(CString& out, LPCTSTR xmlString)
{
	CString s;
	s = xmlString;
	s.Replace('\t', ' ');
	s.Replace(_T("\r"), _T(""));
	s.Replace(_T("\n"), _T(""));
	while(s.Find(_T("  "))>-1) s.Replace(_T("  "), _T(" "));
	while(s.Find(_T("< "))>-1) s.Replace(_T("< "), _T("<"));
	while(s.Find(_T(" <"))>-1) s.Replace(_T(" <"), _T("<"));
	while(s.Find(_T(" >"))>-1) s.Replace(_T(" >"), _T(">"));
	while(s.Find(_T("> "))>-1) s.Replace(_T("> "), _T(">"));
	int i = 0;
	out = _T("");
	while(i < s.GetLength())
	{
		int b = s.Find(_T("<!--"), i);
		if(b > -1)
		{
			out += s.Mid(i, b - i);
			int e = s.Find(_T("-->"), b + 4);
			if(e > -1) i = e + 3;//<!--和-->之间的内容全部忽略
			else break;//<!--之后的内容全部忽略
		}
		else
		{
			out += s.Mid(i);
			break;
		}
	}
	return true;
}
CTWenXMLElement* CTWenXMLElement::static_FromXML(LPCTSTR xmlString)
{
	CArray<CTWenXMLElement*, CTWenXMLElement*> Elements;
	if(!static_FromXML(xmlString, Elements)) return NULL;
	return Elements[0];
}
bool CTWenXMLElement::static_FromXML(LPCTSTR xmlString, CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements)
{
	if(!_static_FromXML(xmlString, Elements))
	{
		for(int i=0; i<Elements.GetCount(); i++) delete Elements.GetAt(i);
		Elements.RemoveAll();
		return false;
	}
	return true;
}
bool CTWenXMLElement::_static_FromXML(LPCTSTR xmlString, CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements)
{
	CString formatString;
	if(!static_GetFormatString(formatString, xmlString)) return false;
	CTWenXMLElement* pCurrent = NULL;
	CStringTokenEx ext(formatString);
	ext.BeginEnum();
	CString s;
	TCHAR c;
	while(true)
	{
		c = ext.GetNext(s, _T('<'));
		if(c == NULL)
		{
			if(s.IsEmpty()) return true;
			else return false;
		}
		if (formatString[ext.GetCurrentPosition()] == '/')
		{
			if(!pCurrent) return false;
			pCurrent->InnerText = s;
			c = ext.GetNext(s, _T('/'));
			c = ext.GetNext(s, _T('>'));
			if(c == NULL) return false;
			if(pCurrent->TagName != s) return false;
			pCurrent = pCurrent->GetParent();
			continue;
		}
		c = ext.GetNext(s, _T(" />"));
		if(c == NULL) return false;
		//遇到了一个名称为s的新节点
		CTWenXMLElement* pNode = new CTWenXMLElement;
		pNode->TagName = s;
		if(pCurrent) pCurrent->AppendChild(pNode);
		else Elements.Add(pNode);
		pCurrent = pNode;
		if(c == '/')//<node/>
		{
			if (formatString[ext.GetCurrentPosition()] != '>') return false;
			c = ext.GetNext(s, _T('>'));
			pCurrent = pCurrent->GetParent();
			continue;
		}
		else if(c == ' ')//说明有属性
		{
			CString propName, propValue;
			while(true)
			{
				c = ext.GetNext(propName, _T(" =/>"));
				if(c == NULL) return false;
				if(c == ' ') continue;
				if(c == '/')//<node prop=''/>
				{
					c = ext.GetNext(s, _T('>'));
					pCurrent = pCurrent->GetParent();
					break;
				}
				if(c == '>') break;

				long pos = ext.GetCurrentPosition();
				c = ext.GetNext(s, _T("'\""));
				if(c == NULL || pos != ext.GetCurrentPosition() - 1) return false;
				c = ext.GetNext(propValue, c);
				if(c == NULL) return false;
				//遇到了名称为s的属性
				pCurrent->SetProperty(propName, propValue);
			}
		}
	}
	return true;
}

CTWenXMLElement* CTWenXMLElement::static_FindElement(LPCTSTR Tag, const CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements)
{
	for(int i=0; i<Elements.GetCount(); i++)
	{
		CTWenXMLElement* pElement = Elements.GetAt(i);
		if(pElement->TagName == Tag) return pElement;
	}
	return NULL;
}
CTWenXMLElement* CTWenXMLElement::static_Path(LPCTSTR xmlPath, const CArray<CTWenXMLElement*, CTWenXMLElement*>* pElements)
{
	CString p(xmlPath);
	p.Replace(_T('/'), _T('\\'));
	if(p.Right(1)==_T("\\")) p.Delete(p.GetLength()-1);
	if(p.Left(1) == _T(".")) p.Delete(0);
	if(p.Left(1) == _T("\\")) p.Delete(0);
	CStringExt ext(p, _T("\\"));
	ext.BeginEnum();
	CString cur;
	CTWenXMLElement* pCurrent = NULL;
	while(ext.GetNext(cur))
	{
		if(cur.IsEmpty()) continue;
		pCurrent = static_FindElement(cur, *pElements);
		if(!pCurrent) return NULL;
		pElements = &pCurrent->Children;
	}
	return pCurrent;
}
CTWenXMLElement* CTWenXMLElement::static_AutoPath(LPCTSTR xmlPath, CArray<CTWenXMLElement*, CTWenXMLElement*>* pElements, CTWenXMLElement* pParent)
{
	CString p(xmlPath);
	p.Replace(_T('/'), _T('\\'));
	if(p.Right(1)==_T("\\")) p.Delete(p.GetLength()-1);
	if(p.Left(1) == _T(".")) p.Delete(0);
	if(p.Left(1) == _T("\\")) p.Delete(0);
	CStringExt ext(p, _T("\\"));
	ext.BeginEnum();
	CString cur;
	CTWenXMLElement* pCurrent = NULL;
	while(ext.GetNext(cur))
	{
		if(cur.IsEmpty()) continue;
		pCurrent = static_FindElement(cur, *pElements);
		if(!pCurrent)
		{
			pCurrent = new CTWenXMLElement(pParent);
			pCurrent->TagName = cur;
			pElements->Add(pCurrent);
		}
		pElements = &pCurrent->Children;
		pParent = pCurrent;
	}
	return pCurrent;
}

CString CTWenXMLElement::GetProperty(LPCTSTR propName) const
{
	CString ret;
	if(!Properties.Lookup(propName, ret)) return _T("");
	return ret;
}
void CTWenXMLElement::SetProperty(LPCTSTR propName, LPCTSTR propVal)
{
	Properties.SetAt(propName, propVal);
}
long CTWenXMLElement::GetProperty_Integer(LPCTSTR propName) const
{
	return _tstol(GetProperty(propName));
}
void CTWenXMLElement::SetProperty_Integer(LPCTSTR propName, long propVal)
{
	CString s;
	s.Format(_T("%d"), propVal);
	SetProperty(propName, s);
}
void CTWenXMLElement::SetProperty(LPCTSTR propName, long propVal)
{
	SetProperty_Integer(propName, propVal);
}

CString CTWenXMLElement::GetInnerXML() const
{
	if(Children.GetCount() == 0) return InnerText;
	CString ret;
	for(int i=0; i<Children.GetCount(); i++) ret += Children.GetAt(i)->GetOuterXML();
	return ret;
}
CString CTWenXMLElement::GetOuterXML() const
{
	CString ret(GetPrifix() + _T("<")+TagName);
	POSITION EnumPosition = Properties.GetStartPosition();
	CString propName, propValue;
	CString prop;
	while(EnumPosition)
	{
		Properties.GetNextAssoc(EnumPosition, propName, propValue);
		if(propValue.Find('\"') > -1) prop.Format(_T(" %s='%s'"), propName, propValue);
		else prop.Format(_T(" %s=\"%s\""), propName, propValue);
		ret += prop;
	}
	ret += _T(">");
	if(Children.GetCount()) ret += _T("\r\n");
	ret += GetInnerXML();
	if(Children.GetCount()) return ret + GetPrifix() + _T("</") + TagName + _T(">\r\n");
	else return ret + _T("</") + TagName + _T(">\r\n");
}
CString CTWenXMLElement::GetInnerText() const
{
	if(Children.GetCount() == 0) return InnerText;
	CString ret;
	for(int i=0; i<Children.GetCount(); i++) ret += Children.GetAt(i)->GetInnerText();
	return ret;
}

bool CTWenXMLElement::SetInnerXML(LPCTSTR xml)
{
	DeleteChildren();
	if(!static_FromXML(xml, Children)) return false;
	for(int i=0; i<Children.GetCount(); i++) Children.GetAt(i)->Parent = this;
	return true;
}
void CTWenXMLElement::SetOuterXML(LPCTSTR xml)
{
	if(!Parent) return;
	CArray<CTWenXMLElement*, CTWenXMLElement*> Elements;
	static_FromXML(xml, Elements);
	Parent->CloneChild(this, Elements);
}
void CTWenXMLElement::SetInnerText(LPCTSTR text)
{
	DeleteChildren();
	InnerText = text;
}
void CTWenXMLElement::SetInnerInteger(long integer)
{
	CString text;
	text.Format(_T("%d"), integer);
	SetInnerText(text);
}

void CTWenXMLElement::DeleteChildren()
{
	for(int i = Children.GetCount()-1; i>-1; i--) delete Children.GetAt(i);
	Children.RemoveAll();
}

void CTWenXMLElement::ReplaceChild(CTWenXMLElement* pChild, const CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements)
{
	if(Elements.GetCount() == 0) return;
	for(int i=0; i<Children.GetCount(); i++)
	{
		if(Children.GetAt(i) == pChild)
		{
			CTWenXMLElement* pElement = Elements.GetAt(0);
			ToChild(pElement);
			Children.SetAt(i, pElement);
			break;
		}
	}
	for(int i=1; i<Elements.GetCount(); i++)
	{
		AppendChild(Elements.GetAt(i));
	}
}
void CTWenXMLElement::CloneChild(CTWenXMLElement* pChild, CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements)
{
	if(Elements.GetCount() == 0) return;
	CTWenXMLElement* pElement = Elements.GetAt(0);
	//pChild->Clone(pElement);
	{
		pChild->TagName = pElement->TagName;
		pChild->InnerText = pElement->InnerText;
		pChild->DeleteChildren();
		for(int i=0; i<pElement->Children.GetCount(); i++) pChild->AppendChild(pElement->Children.GetAt(i));

		POSITION EnumPosition = pElement->Properties.GetStartPosition();
		CString propName, propValue;
		while(EnumPosition)
		{
			pElement->Properties.GetNextAssoc(EnumPosition, propName, propValue);
			pChild->Properties.SetAt(propName, propValue);
		}
	}
	pElement->Children.RemoveAll();
	delete pElement;
	Elements.RemoveAt(0);

	for(int i=0; i<Elements.GetCount(); i++)
	{
		AppendChild(Elements.GetAt(i));
	}
}

CTWenXMLElement* CTWenXMLElement::Path(LPCTSTR xmlPath) const
{
	return static_Path(xmlPath, &Children);
}
CTWenXMLElement* CTWenXMLElement::AutoPath(LPCTSTR xmlPath)
{
	return static_AutoPath(xmlPath, &Children, this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTWenXMLDocument
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWenXMLDocument::LoadFromFile(LPCTSTR file)
{
	CFile in;
	if(!in.Open(file,CFile::modeRead)) return false;
	bool ret = false;
	char* pData = new char[(DWORD)in.GetLength()+1];
	in.Read(pData, (DWORD)in.GetLength());
	pData[(DWORD)in.GetLength()] = 0;
	in.Close();
	{
		TWen_USES_CONVERSION;
		ret = LoadFromString(TWen_CA2T(pData));
	}
	delete []pData;
	return ret;
}
bool CTWenXMLDocument::LoadFromString(LPCTSTR xmlString)
{
	DeleteRoots();
	return CTWenXMLElement::static_FromXML(xmlString, Roots);
}
bool CTWenXMLDocument::Save(LPCTSTR file) const
{
	CFile out;
	if(!out.Open(file,CFile::modeWrite|CFile::modeCreate)) return false;
	{
		TWen_USES_CONVERSION;
		CString doc = GetDocument();
		LPCSTR s = TWen_CT2A(doc);
		out.Write(s, strlen(s));
	}
	out.Close();
	return true;
}
CTWenXMLElement* CTWenXMLDocument::Path(LPCTSTR xmlPath) const
{
	return CTWenXMLElement::static_Path(xmlPath, &Roots);
}
CTWenXMLElement* CTWenXMLDocument::AutoPath(LPCTSTR xmlPath)
{
	return CTWenXMLElement::static_AutoPath(xmlPath, &Roots, NULL);
}
CString CTWenXMLDocument::GetDocument() const
{
	CString ret;
	for(int i=0; i<Roots.GetCount(); i++) ret += Roots.GetAt(i)->GetOuterXML();
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTWenXMLConfig
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWenXMLConfig::SaveConfig(LPCTSTR file) const
{
	if(!file || _tcslen(file) == 0) file = (LPCTSTR)XMLFile;
 	if(!file || _tcslen(file) == 0) return false;
	return XMLDocument.Save(file);
}
DWORD CTWenXMLConfig::GetSubSectionID(DWORD dwParentSectionID, LPCTSTR xmlPath) const
{
	if(!dwParentSectionID) return NULL;
	return (DWORD)((CTWenXMLElement*)dwParentSectionID)->Path(xmlPath);
}
DWORD CTWenXMLConfig::AutoSubSection(DWORD dwParentSectionID, LPCTSTR xmlPath, LPCTSTR Note)
{
	if(!dwParentSectionID) return NULL;
	CTWenXMLElement* pElement = ((CTWenXMLElement*)dwParentSectionID)->AutoPath(xmlPath);
	if(pElement && Note) pElement->SetProperty(NotePropertyName, Note);
	return (DWORD)pElement;
}
DWORD CTWenXMLConfig::NewSubSection(DWORD dwParentSectionID, LPCTSTR tagName, LPCTSTR Note)
{
	if(!dwParentSectionID) return NULL;
	CTWenXMLElement* pElement = new CTWenXMLElement;
	pElement->TagName = tagName;
	((CTWenXMLElement*)dwParentSectionID)->AppendChild(pElement);
	if(Note) pElement->SetProperty(NotePropertyName, Note);
	return (DWORD)pElement;
}

bool CTWenXMLConfig::GetBoolean(DWORD dwSectionID, LPCTSTR Item, bool Default) const
{
	CString ret = GetString(dwSectionID, Item);
	if(ret.IsEmpty()) return Default;
	ret.MakeLower();
	if(ret == _T("false") || ret == _T("否") || ret == _T("no") || ret == _T("nil") || ret == _T("null") || ret == _T("0")) return false;
	for(int i=0; i<ret.GetLength(); i++) if(ret[i] != '0') return true;
	return false;//"0000"
}
long CTWenXMLConfig::GetLong(DWORD dwSectionID, LPCTSTR Item, long Default) const
{
	CString ret = GetString(dwSectionID, Item);
	if(ret.IsEmpty()) return Default;
	return _tstol(ret);
}
double CTWenXMLConfig::GetDouble(DWORD dwSectionID, LPCTSTR Item, double Default) const
{
	CString ret = GetString(dwSectionID, Item);
	if(ret.IsEmpty()) return Default;
	return _tstof(ret);
}
CString CTWenXMLConfig::GetString(DWORD dwSectionID, LPCTSTR Item, LPCTSTR Default) const
{
	if(dwSectionID == NULL) return Default;
	CTWenXMLElement* pElement = ((CTWenXMLElement*)dwSectionID)->Path(Item);
	if(!pElement) return Default;
	return pElement->GetInnerText();
}

bool CTWenXMLConfig::SetLong(DWORD dwSectionID, LPCTSTR Item, long Value, LPCTSTR Note)
{
	CString s;
	s.Format(_T("%d"), Value);
	return SetString(dwSectionID, Item, s, Note);
}
bool CTWenXMLConfig::SetDouble(DWORD dwSectionID, LPCTSTR Item, double Value, LPCTSTR Note)
{
	CString s;
	s.Format(_T("%f"), Value);
	return SetString(dwSectionID, Item, s, Note);
}
bool CTWenXMLConfig::SetString(DWORD dwSectionID, LPCTSTR Item, LPCTSTR Value, LPCTSTR Note)
{
	if(dwSectionID == NULL) return false;
	CTWenXMLElement* pElement = ((CTWenXMLElement*)dwSectionID)->AutoPath(Item);
	pElement->SetInnerText(Value);
	if(Note) pElement->SetProperty(NotePropertyName, Note);
	return true;
}