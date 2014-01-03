
#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CStringTokenEx
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CStringTokenEx
{
public:
	CStringTokenEx(LPCTSTR srcString = _T(""), LPCTSTR tokens = _T("")):SRCString(srcString),Tokens(tokens),m_lEnumPos(0){}

public:
	void SetSRCString(LPCTSTR srcString){SRCString = srcString;}
	void SetTokens(LPCTSTR tokens){Tokens = tokens;}

public:
	void BeginEnum(){m_lEnumPos = 0;}
	TCHAR GetNext(CString& value);//到达末尾返回NULL，否则返回找到的token
	TCHAR GetNext(CString& value, LPCTSTR tokens);//到达末尾返回NULL，否则返回找到的token
	TCHAR GetNext(CString& value, TCHAR token);
	long GetCurrentPosition() const{return m_lEnumPos;}

public:
	CString SRCString;
	CString Tokens;

private:
	long m_lEnumPos;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTWenXMLElement
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTWenXMLElement
{
public:
	static bool static_GetFormatString(CString& out, LPCTSTR xmlString);
	static CTWenXMLElement* static_FromXML(LPCTSTR xmlString);
	static bool static_FromXML(LPCTSTR xmlString, CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements);
	static CTWenXMLElement* static_FindElement(LPCTSTR Tag, const CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements);
	static CTWenXMLElement* static_Path(LPCTSTR xmlPath, const CArray<CTWenXMLElement*, CTWenXMLElement*>* pElements);
	static CTWenXMLElement* static_AutoPath(LPCTSTR xmlPath, CArray<CTWenXMLElement*, CTWenXMLElement*>* pElements, CTWenXMLElement* pParent);

protected:
	static bool _static_FromXML(LPCTSTR xmlString, CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements);

public:
	CTWenXMLElement(CTWenXMLElement* parent = NULL):Parent(parent){}
	~CTWenXMLElement(){DeleteChildren();}

public:
	CTWenXMLElement* GetParent() const{return Parent;}
	CString GetProperty(LPCTSTR propName) const;
	void SetProperty(LPCTSTR propName, LPCTSTR propVal);
	long GetProperty_Integer(LPCTSTR propName) const;
	void SetProperty_Integer(LPCTSTR propName, long propVal);
	void SetProperty(LPCTSTR propName, long propVal);

	CString GetInnerXML() const;
	CString GetOuterXML() const;
	CString GetInnerText() const;

	bool SetInnerXML(LPCTSTR xml);
	void SetOuterXML(LPCTSTR xml);
	void SetInnerText(LPCTSTR text);//将清空Children
	void SetInnerInteger(long integer);//将清空Children

public:
	void AppendChild(CTWenXMLElement* pChild){ToChild(pChild);Children.Add(pChild);}
	void DeleteChildren();
	void ReplaceChild(CTWenXMLElement* pChild, const CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements);
	void CloneChild(CTWenXMLElement* pChild, CArray<CTWenXMLElement*, CTWenXMLElement*>& Elements);//会自动删除Elements[0]
	CTWenXMLElement* Path(LPCTSTR xmlPath) const;
	CTWenXMLElement* AutoPath(LPCTSTR xmlPath);

protected:
	CString GetPrifix() const{if(Parent) return Parent->GetPrifix() + stc_Prefix;return _T("");}
	void ToChild(CTWenXMLElement* pChild){pChild->Parent = this;}

public:
	CString TagName;

	CString InnerText;
	CArray<CTWenXMLElement*, CTWenXMLElement*> Children;//和InnerText只有一个有效

	CMap<CString, LPCTSTR, CString, LPCTSTR> Properties;

protected:
	CTWenXMLElement* Parent;
	static CString stc_Prefix;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTWenXMLDocument
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTWenXMLDocument
{
public:
	CTWenXMLDocument(){}
	~CTWenXMLDocument(){DeleteRoots();}

public:
	bool LoadFromFile(LPCTSTR file);
	bool LoadFromString(LPCTSTR xmlString);
	bool Save(LPCTSTR file) const;
	void DeleteRoots()
	{
		for(int i = Roots.GetCount()-1; i>-1; i--) delete Roots.GetAt(i);
		Roots.RemoveAll();
	}

	CString GetDocument() const;//此Document不包含头行

	CTWenXMLElement* Path(LPCTSTR xmlPath) const;
	CTWenXMLElement* AutoPath(LPCTSTR xmlPath);

public:
	CArray<CTWenXMLElement*, CTWenXMLElement*> Roots;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTWenXMLConfig
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTWenXMLConfig
{
public:
	CTWenXMLConfig():NotePropertyName(_T("Remark")){}

public:
	bool LoadConfig(LPCTSTR file){XMLFile = file;return XMLDocument.LoadFromFile(file);}
	bool LoadConfigString(LPCTSTR xmlString){return XMLDocument.LoadFromString(xmlString);}
	bool SaveConfig(LPCTSTR file) const;
	CString GetConfigString() const{return XMLDocument.GetDocument();}

	DWORD GetSectionID(LPCTSTR xmlPath) const{return (DWORD)XMLDocument.Path(xmlPath);}
	DWORD AutoSection(LPCTSTR xmlPath, LPCTSTR Note = NULL)
	{
		CTWenXMLElement* pElement = XMLDocument.AutoPath(xmlPath);
		if(pElement && Note) pElement->SetProperty(NotePropertyName, Note);
		return (DWORD)pElement;
	}

	DWORD GetSubSectionID(DWORD dwParentSectionID, LPCTSTR xmlPath) const;
	DWORD AutoSubSection(DWORD dwParentSectionID, LPCTSTR xmlPath, LPCTSTR Note = NULL);
	DWORD NewSubSection(DWORD dwParentSectionID, LPCTSTR tagName, LPCTSTR Note = NULL);//注意，此函数不支持路径

	CTWenXMLElement* SectionElementFromID(DWORD dwSectionID) const{return (CTWenXMLElement*)dwSectionID;}
	DWORD SectionIDFromElement(CTWenXMLElement* pSection) const{return (DWORD)pSection;}

	bool GetBoolean(DWORD dwSectionID, LPCTSTR Item, bool Default = false) const;
	long GetLong(DWORD dwSectionID, LPCTSTR Item, long Default = 0) const;
	double GetDouble(DWORD dwSectionID, LPCTSTR Item, double Default = 0.0) const;
	CString GetString(DWORD dwSectionID, LPCTSTR Item, LPCTSTR Default = _T("")) const;

	bool SetLong(DWORD dwSectionID, LPCTSTR Item, long Value, LPCTSTR Note = NULL);
	bool SetDouble(DWORD dwSectionID, LPCTSTR Item, double Value, LPCTSTR Note = NULL);
	bool SetString(DWORD dwSectionID, LPCTSTR Item, LPCTSTR Value, LPCTSTR Note = NULL);

protected:
	CTWenXMLDocument XMLDocument;
	CString XMLFile;
	CString NotePropertyName;
};