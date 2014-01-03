#include "stdafx.h"
#include "Base_AnalysisInject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBase_AnalysisInject::CBase_AnalysisInject(LPCTSTR name, CBase_AnalysisTemplate* pTemplate) 
: m_pTemplate(pTemplate), m_cStrName(name)
{
	m_data.Create(XMLROOT_INJECT);
}

CBase_AnalysisInject::~CBase_AnalysisInject()
{

}

bool CBase_AnalysisInject::GetInletValves(INLETVALVES& v) const
{
	//设置通道下拉框数据
	CIADataTable cTableData;
	if(m_data.GetValue_Table(XMLPATH_INJECT_INLETVALVE, cTableData) != IAERROR_SUCCESS) return false;

	int nNum = cTableData.GetRowsNumber();
	CStringArray arrayString;
	structInletValve _v;
	for(int i=0; i<nNum; i++)
	{
		arrayString.RemoveAll();
		if(cTableData.GetRowValues(i, arrayString))
		{
			_v.Valve = CIATable::XMLData_ParseToText(arrayString[0]);
			_v.Name = CIATable::XMLData_ParseToText(arrayString[1]);
			v.Add(_v);
		}
	}

	return true;
}
