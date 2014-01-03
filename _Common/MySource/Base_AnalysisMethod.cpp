#include "stdafx.h"
#include "Base_AnalysisMethod.h"

//#include "MethodNameDlg.h"		这是一个对话框，移到其他地方使用



CBase_AnalysisMethod::CBase_AnalysisMethod(CString cStrName, CBase_AnalysisTemplate *pcTemplate)
:m_pcTemplate(pcTemplate)
{
	m_cCurrentMethodData.SetFlags(CAnalysisData::ADTF_VALIDATEONGETTING_DISABLED|CAnalysisData::ADTF_VALIDATEONSETTING_COMPATIBLE|CAnalysisData::ADTF_MODEONSETTING_CREATE|CAnalysisData::ADTF_USINGFORMATONSETTING|CAnalysisData::ADTF_USINGFORMATONGETTING);
	m_cCurrentMethodData.Create(XMLROOT_METHOD);
	m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, cStrName);
}

BOOL CBase_AnalysisMethod::IsReady(void)
{
	return m_pcTemplate->GetInstrument()->Base_IsReady();
}

BOOL CBase_AnalysisMethod::Run(void)
{
	AfxMessageBox(_T("未完成"));
	return FALSE;
}
BOOL CBase_AnalysisMethod::RunToStruct(LPVOID lpvStruct)
{
	AfxMessageBox(_T("以下调用有疑问！路径可能不正确"));
	return SetInstrumentParamToStruct(XMLPATH_METHOD_PARAM_INSTRUMENT, lpvStruct);
}

void CBase_AnalysisMethod::GetDataFrom(CBase_AnalysisMethod &other)
{

	CString methodName = GetName();

	if(methodName == _T(""))
	{
		AfxMessageBox(_T("当前方法没有方法名，操作失败"));

		return;
	}

	m_cCurrentMethodData.SetXMLString(other.m_cCurrentMethodData.GetXMLString());

	m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, methodName);

}

BOOL CBase_AnalysisMethod::IsNameExist(CString cStrName)
{
	CString path;

	path = m_pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD);

	path = cStrName + _T(".MTD");

	CFileFind ff;

	return ff.FindFile(path);


}


CString CBase_AnalysisMethod::GetName(void) const
{

	CString methodName;

	m_cCurrentMethodData.GetValue_String(XMLPATH_METHOD_METHODNAME, methodName);

	return methodName;
}

BOOL CBase_AnalysisMethod::GetMethodNameList(CStringArray &arrayName)
{
	return GetMethodNameList(m_pcTemplate, arrayName);
}
BOOL CBase_AnalysisMethod::GetMethodNameList(CBase_AnalysisTemplate *pcTemplate, CStringArray &arrayName)
{

	CString path;

	path = pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD);

	CFileFind ff;

	path += _T("*.mtd");

	BOOL ret = ff.FindFile(path);

	while(ret)
	{
		ret = ff.FindNextFile();

		if(ff.IsDirectory()||ff.IsDots()||ff.IsHidden())
		{
			continue;		
		}
		
		CString fileName = ff.GetFileName();
		if(fileName.Right(4).CompareNoCase(_T(".mtd"))) continue;

		arrayName.Add(fileName.Left(fileName.GetLength()-4));
	}	

	return true;

}



BOOL CBase_AnalysisMethod::Load(CString cStrName)
{

	int ret;

	CString path = m_pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD)+ cStrName + _T(".MTD");

	ret = m_cCurrentMethodData.LoadFromFile(path);

	if(ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CString CBase_AnalysisMethod::GetFilePathName() const
{
	return GetFilePathName(GetName(), m_pcTemplate);
}
CString CBase_AnalysisMethod::GetFilePathName(const CString& strMethodName, CBase_AnalysisTemplate* pcTemplate)
{
	return pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD)+strMethodName+_T(".MTD");
}
BOOL CBase_AnalysisMethod::Save()
{

	int ret;

	CString methodPath = GetFilePathName();

	ret = m_cCurrentMethodData.SaveToFile(methodPath);	


	if(ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}



BOOL CBase_AnalysisMethod::SaveAs(const CString& name)//仅名称，不包括路径和后缀
{

	if(name == _T("")) return false;

	CString currentName;

	m_cCurrentMethodData.GetValue_String(XMLPATH_METHOD_METHODNAME, currentName);


	try
	{

		int ret;

		m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, name);

		CString methodPath = m_pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD)+name+_T(".MTD");		

		ret = m_cCurrentMethodData.SaveToFile(methodPath);

		m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, currentName);

		return (ret == IAERROR_SUCCESS);
	}
	catch (...)
	{
		m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, currentName);
		return false;
	}

	return true;
}

BOOL CBase_AnalysisMethod::GetInfoString(CString& info)
{

	m_cCurrentMethodData.GetValue_XMLString(XMLROOT_METHOD, info);

	return true;
}

/*
BOOL CBase_AnalysisMethod::GetInstrumentParamSize(int& nCols, int& nRows)
{
	nCols = 0;

	nRows = 0;


	CString CalibrateXmlString;

	//g_cAnalysisSystem.GetCurrentMethod()->m_cCurrentMethodData.GetValue_XMLString(XMLPATH_METHOD_INMS_QTABLE, CalibrateXmlString);
	m_cCurrentMethodData.GetValue_XMLString(XMLPATH_METHOD_INMS_QTABLE, CalibrateXmlString);


	if(CalibrateXmlString=="")
	{
		return false;
	}

	IXMLDOMDocumentPtr pDOMDocument;

	IXMLDOMElementPtr pElement;

	IXMLDOMNodeListPtr pNodeList;		

	CoInitialize(NULL);

	CoCreateInstance(CLSID_DOMDocument,   NULL,   CLSCTX_INPROC_SERVER,     
		IID_IXMLDOMDocument,   (void**)&pDOMDocument);

	_bstr_t bstrXML(CalibrateXmlString);

	VARIANT_BOOL ret;

	pDOMDocument->loadXML(bstrXML, &ret);

	if(ret==0)
	{
		return false;
	}

	pDOMDocument->get_documentElement(&pElement);

	pElement->get_childNodes(&pNodeList);

	long rows,cols;

	pNodeList->get_length(&rows);

	nRows = rows - 3;

	for(int i=0;i<rows;i++)
	{
		IXMLDOMNodePtr pNode;

		IXMLDOMNamedNodeMapPtr pAttributeList;

		IXMLDOMNodePtr pAttributeNode;

		pNodeList->get_item(i, &pNode);

		pNode->get_attributes(&pAttributeList);

		pAttributeList->get_length(&cols);

	}

	nCols = cols - 4;

	return true;

}


BOOL CBase_AnalysisMethod::GetInstrumentParam(int* pMZ,   CString* pCompoundName, double* pSensitivity, double* pRationTable,  double* pNoise)
{


	CString CalibrateXmlString;
	CString CompoundXmlString;

	//g_cAnalysisSystem.GetCurrentMethod()->m_cCurrentMethodData.GetValue_XMLString(XMLPATH_METHOD_INMS_QTABLE, CalibrateXmlString);
	m_cCurrentMethodData.GetValue_XMLString(XMLPATH_METHOD_INMS_QTABLE, CalibrateXmlString);

	//g_cAnalysisSystem.GetCurrentMethod()->m_cCurrentMethodData.GetValue_XMLString(XMLPATH_METHOD_CALIBRATE_COMPOUND, CompoundXmlString);
	m_cCurrentMethodData.GetValue_XMLString(XMLPATH_METHOD_CALIBRATE_COMPOUND, CompoundXmlString);




	if(CalibrateXmlString=="")
	{
		return false;
	}

	IXMLDOMDocumentPtr pDOMDocument;

	IXMLDOMElementPtr pElement;

	IXMLDOMNodeListPtr pNodeList;

	CoInitialize(NULL);

	CoCreateInstance(CLSID_DOMDocument,   NULL,   CLSCTX_INPROC_SERVER,     
		IID_IXMLDOMDocument,   (void**)&pDOMDocument);

	_bstr_t bstrXML(CalibrateXmlString);

	VARIANT_BOOL ret;

	pDOMDocument->loadXML(bstrXML, &ret);

	if(ret==0)
	{
		return false;
	}

	pDOMDocument->get_documentElement(&pElement);

	pElement->get_childNodes(&pNodeList);

	long rows,cols;

	pNodeList->get_length(&rows);


	//定量表
	for(int i=1;i<rows-2;i++)
	{
		IXMLDOMNodePtr pNode;

		IXMLDOMNamedNodeMapPtr pAttributeList;

		IXMLDOMNodePtr pAttributeNode;

		pNodeList->get_item(i, &pNode);

		pNode->get_attributes(&pAttributeList);

		pAttributeList->get_length(&cols);


		for(int j=3;j<cols-1;j++)
		{
			IXMLDOMNodePtr pNodeAttri;

			CComBSTR bstr;

			CString str;

			if(i>0)
			{
				pAttributeList->get_item(j, &pNodeAttri);

				pNodeAttri->get_text(&bstr);

				str = bstr;

				pRationTable[(i - 1) * (cols - 4) + j - 3] = str==""?0:_wtof(str);


			}

		}

	}

	for(int i=0;i<rows-1;i++)
	{
		IXMLDOMNodePtr pNode;

		IXMLDOMNamedNodeMapPtr pAttributeList;

		IXMLDOMNodePtr pAttributeNode;

		pNodeList->get_item(i, &pNode);

		pNode->get_attributes(&pAttributeList);

		for(int j=3;j<cols-1;j++)
		{
			IXMLDOMNodePtr pNodeAttri;

			CComBSTR bstr;

			CString str;

			//get mz
			if(i==0)
			{
				pAttributeList->get_item(j, &pNodeAttri);

				pNodeAttri->get_text(&bstr);

				str = bstr;

				pMZ[j-3] = _ttoi(str);

			}

			//get noise
			if(i == rows - 2)
			{
				pAttributeList->get_item(j, &pNodeAttri);

				pNodeAttri->get_text(&bstr);

				str = bstr;

				pNoise[j-3] = _wtof(str);
			}
		}
	}



	for(int i=1;i<rows-2;i++)
	{
		IXMLDOMNodePtr pNode;

		IXMLDOMNodePtr pNodeCompound;

		IXMLDOMNodePtr pNodeSensitivity;

		CComBSTR bstrCompound;

		CComBSTR bstrSensitivity;

		CString temp;




		IXMLDOMNamedNodeMapPtr pAttributeList;

		IXMLDOMNodePtr pAttributeNode;

		pNodeList->get_item(i, &pNode);

		pNode->get_attributes(&pAttributeList);

		pAttributeList->get_item(1, &pNodeCompound);

		pNodeCompound->get_text(&bstrCompound);

		temp = bstrCompound;

		pCompoundName[i-1] = temp;


		pAttributeList->get_item(2, &pNodeSensitivity);

		pNodeSensitivity->get_text(&bstrSensitivity);

		temp = bstrSensitivity;

		pSensitivity[i-1] = _wtof(temp);


	}

	//化合物名字中文换成分子式
	IXMLDOMElementPtr root;

	IXMLDOMNodeListPtr pItems;

	long nItem;

	pDOMDocument->loadXML(_bstr_t(CompoundXmlString), &ret);

	if(ret==0)
	{
		return false;
	}

	pDOMDocument->get_documentElement(&root);

	root->get_childNodes(&pItems);

	pItems->get_length(&nItem);

	for(int i=0;i<nItem;i++)
	{
		IXMLDOMNodePtr pItem;

		IXMLDOMNodePtr pFirstChild;

		CComBSTR bstrNodeValue;

		pItems->get_item(i, &pItem);

		pItem->get_firstChild(&pFirstChild);

		pFirstChild->get_text(&bstrNodeValue);

		CString strNodeValue = bstrNodeValue;

		for(int j=0;j<rows-3;j++)
		{
			if(strNodeValue == pCompoundName[j])
			{
				IXMLDOMNodeListPtr pChildren;				

				IXMLDOMNodePtr pFormulaChild;				

				CComBSTR bstrFormula;

				pItem->get_childNodes(&pChildren);

				pChildren->get_item(4, &pFormulaChild);

				pFormulaChild->get_text(&bstrFormula);

				pCompoundName[j] = bstrFormula;

			}

		}

	}


	return true;



}
*/

BOOL CBase_AnalysisMethod::SetInstrumentParamToStruct(CString cStrXMLPath, LPVOID lpvStruct)
{

	CString xmlParam;

	m_cCurrentMethodData.GetValue_XMLString(cStrXMLPath, xmlParam);

	//m_pcTemplate->GetInstrument()->SetParamToStruct(xmlParam, lpvStruct);

	ASSERT(FALSE);//未完成

	return TRUE;

}

int CBase_AnalysisMethod::SaveInstrumentParam(CString cStrXMLPath, CString xmlString)
{

	CIADataTable cXMLTable;

	cXMLTable.SetXMLString(xmlString);

	m_cCurrentMethodData.SetValue_Table(cStrXMLPath, cXMLTable);	


	return TRUE;

}


CString CBase_AnalysisMethod::GetInstrumentParam(CString cStrXMLPath)
{

	CString xmlParam;

	m_cCurrentMethodData.GetValue_XMLString(cStrXMLPath, xmlParam);

	return xmlParam;

}

void CBase_AnalysisMethod::PreviewCurruentMethod()
{
	CString xmlStr;

	GetInfoString(xmlStr);

	xmlStr = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<?xml-stylesheet type=\"text/xsl\" href=\"MethodPreview.xsl\"?>") + xmlStr;

	IXMLDOMDocumentPtr pDOMDocument;
	CoInitialize(NULL);     
	CoCreateInstance(CLSID_DOMDocument,   NULL,  CLSCTX_INPROC_SERVER,     
		IID_IXMLDOMDocument,   (void**)&pDOMDocument);

	//TCHAR tcPath[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH, tcPath);
	//CString Path = tcPath;

	//CString Path = g_cAnalysisSystem.GetCurrentMethod()->m_pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD);
	CString Path = m_pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD) +  g_pcConfigIni->CurrentSystem().m_szXSLFolder + _T("/");

	Path += _T("MethodPreview.xml");

	BSTR bstrText = xmlStr.AllocSysString(); 
	VARIANT_BOOL ret;
	pDOMDocument->loadXML(bstrText, &ret);


	if(ret == 0)
	{
		SysFreeString(bstrText);

		return ;
	}


	COleVariant path;

	path = Path;

	pDOMDocument->save(path);	

	t_viewXMLFile(NULL, Path, _T("查看方法参数"), NULL);

	SysFreeString(bstrText);

}

BOOL CBase_AnalysisMethod::Construct()
{
	AfxMessageBox(_T("该函数不知啥意思"));return FALSE;
	
/*
	MethodNameDlg methodNameDlg;

	if(methodNameDlg.DoModal()!=IDOK)
	{
		return false;
	}

	CString methodName;

	methodName = methodNameDlg.m_Name;

	if(methodName==_T(""))
	{
		AfxMessageBox(_T("方法名不能为空"));

		return false;
	}

	CString currentName;

	m_cCurrentMethodData.GetValue_String(XMLPATH_METHOD_METHODNAME, currentName);


	try
	{

		int ret;

		m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, methodName);

		CString methodPath = m_pcTemplate->GetDataFolder(PA_DATATYPENAME_METHOD)+methodName+_T(".MTD");		

		ret = m_cCurrentMethodData.SaveToFile(methodPath);

		m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, currentName);

		if(ret == 0)
		{
			return true;
		}
		else
		{

			AfxMessageBox(_T("保存文件失败"));

			return false;
		}



	}
	catch (CException* e)
	{
		m_cCurrentMethodData.SetValue_String(XMLPATH_METHOD_METHODNAME, currentName);

		AfxMessageBox(_T("保存文件失败"));

		return false;
	}

	return true;
*/
}

