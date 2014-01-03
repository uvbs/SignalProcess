#include "stdafx.h"
#include "Base_AnalysisInstrument.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CBase_AnalysisInstrument::CBase_AnalysisInstrument(CBase_AnalysisTemplate* pTemplate) 
//,m_pcSampleConnect(NULL)
{
	m_pTemplate = pTemplate;
	m_bConnect = FALSE;
	m_dataSet.Create(XMLROOT_INSTRUMENT);
	m_dataCurrent.Create(XMLROOT_INSTRUMENT);
}

CBase_AnalysisInstrument::~CBase_AnalysisInstrument()
{

}


int CBase_AnalysisInstrument::Base_SetParam(const CString &cStrFullXMLpath, const CString &cStrXML)
{
	//改变内部数据
	int nRet = m_dataSet.SetValue_XMLString(cStrFullXMLpath, cStrXML);
	m_dataSet.IsEqual(m_dataCurrent);

	//如果不成功
	if(nRet!=IAERROR_SUCCESS) {
		return nRet;
	}

	//如果成功就下发到仪器上
	return Base_DownloadToDevice(NULL);
}


BOOL CBase_AnalysisInstrument::SaveToFile(LPCTSTR lpcszFilename, BOOL bCurrentParam)
{
	CString cStrTemp;
	//如果是当前仪器参数，那先获得仪器参数
	if(bCurrentParam) {
		if(Base_UploadFromDevice(cStrTemp)==IAERROR_SUCCESS) {
			if(m_dataCurrent.SaveToFile(lpcszFilename)==IAERROR_SUCCESS)
				return TRUE;
			else 
				return FALSE;
		}
		else return FALSE;
	}
	//如果不是当前仪器参数，就储存设定参数
	else {
		if(m_dataSet.SaveToFile(lpcszFilename)==IAERROR_SUCCESS)
			return TRUE;
		else 
			return FALSE;
	}
}
BOOL CBase_AnalysisInstrument::LoadFromFile(LPCTSTR lpcszFilename, BOOL bCurrentParam)
{

	//如果是当前仪器参数，那先获得仪器参数
	if(bCurrentParam) {
		if(m_dataCurrent.LoadFromFile(lpcszFilename)==IAERROR_SUCCESS) {
			return TRUE;
		}
		else 
			return FALSE;
	}
	//如果不是当前仪器参数，就储存设定参数
	else {
		if(m_dataSet.LoadFromFile(lpcszFilename)==IAERROR_SUCCESS) {
			return TRUE;
		}
		else 
			return FALSE;
	}
}


void  CBase_AnalysisInstrument::Base_SendRealTimeData(CCurveData &curveData, int nChannel)
{
	m_pTemplate->SendRealTimeData(this, nChannel, curveData);
}
