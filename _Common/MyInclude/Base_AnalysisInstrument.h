#pragma once

//#include "Base_AnalysisTemplate.h"

class CBase_AnalysisTemplate;

class CBase_AnalysisInstrument : public CObject {
public:
	friend class CBase_AnalysisTemplate;

protected:
	CBase_AnalysisInstrument(CBase_AnalysisTemplate*  pcTemplate=NULL);
	virtual ~CBase_AnalysisInstrument();


	//系统模版
	CBase_AnalysisTemplate* m_pTemplate;

	//连接到本仪器上的样品（接受实时数据）
	//CBase_AnalysisSample*	m_pcSampleConnect;

	//仪器参数
	CAnalysisData m_dataSet;		//仪器设定参数
	CAnalysisData m_dataCurrent;	//仪器实时参数

	//仪器是否连接（控制面板是否正常，或者）
	BOOL m_bConnect;


	//是否仪器准备好


	//-----------以下区域的函数流程均已确定------------------------------------
	//-----------------------------------------------------------------------
public:
	virtual BOOL Base_IsReady()=0;
	virtual BOOL Instrument_IsReady() = 0;
	BOOL SaveToFile(LPCTSTR lpcszFilename, BOOL bCurrentParam=TRUE);
	BOOL LoadFromFile(LPCTSTR lpcszFilename, BOOL bCurrent=TRUE);
	virtual int	 Base_SetParam(const CString &cStrFullXMLpath, const CString &cStrXML);
	//将仪器参数下发到仪器，并执行。如果pXMLString为空就下发 m_dataSet, 否则存储pXMLString到m_dataSet，然后下发
	virtual int	 Base_DownloadToDevice(const CString &XMLString)=0;
	virtual int  Base_UploadFromDevice(CString& xmlString)=0;//从设备上载，保存到成员数据
	virtual int  Base_StartRequire(int nChannel)=0;
	virtual int  Base_StopNotify(int nChannel)=0;
	virtual BOOL Base_IsChannelAvailable(int nChannel)=0;
	virtual int  Base_GetChannelNum(void)=0;


protected:
	//设置仪器参数
	virtual BOOL Initialize(void)=0;
	//virtual int  Base_AttachSample(CBase_AnalysisSample * pcSample){if(!m_pcSampleConnect){ m_pcSampleConnect = pcSample; return IAERROR_SUCCESS;} else return IAERROR_CG_INSTRUMENT_BUSY;}
	//virtual void Base_ReleaseSample(void){m_pcSampleConnect = NULL;}

	//仅供内部使用，接收到实时数据后调用
	virtual void  Base_SendRealTimeData(CCurveData &curveData, int nChannel);

};
