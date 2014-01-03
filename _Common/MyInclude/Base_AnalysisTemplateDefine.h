
#pragma once


#include "ConfigIni.h"

#include <memory>

#ifndef TSharedPtr
#define TSharedPtr std::tr1::shared_ptr
#endif TSharedPtr


class CBase_AnalysisMethod;
class CBase_AnalysisSample;
class CBase_AnalysisInstrument;
class CBase_AnalysisInject;
class CPAWindowTask;
class CPAWindowMain;


enum enumPADATATYPENAME
{
	PA_DATATYPENAME_DATA			=1,		//仪器参数目录
	PA_DATATYPENAME_SAMPLE			=2,		//样品数据目录
	PA_DATATYPENAME_TASK			=3,		//任务文件目录
	PA_DATATYPENAME_COMPOUND		=4,		//化合物信息库目录
	PA_DATATYPENAME_METHOD			=5,		//方法目录
	PA_DATATYPENAME_LOG				=6,		//日志基础目录

	PA_DATATYPENAME_DEFAULTMETHOD	=6,		//方法文件
	PA_DATATYPENAME_INJECT			=7,		//进样器配置文件
	PA_DATATYPENAME_INSTRUMENT		=8,		//仪器参数文件
};

struct CInstrument2Sample
{
	CBase_AnalysisInstrument *m_pcInstrument;
	int m_nChannel;
	CBase_AnalysisSample *m_pcSample;

	CInstrument2Sample(): m_pcSample(NULL), m_pcInstrument(NULL), m_nChannel(0){}
};
typedef CArray <CInstrument2Sample, CInstrument2Sample&> CArrayI2S;

///////////////////////////////////////////////////////////////////////////
// CAnalysisConfig
///////////////////////////////////////////////////////////////////////////
class CAnalysisConfig : public CConfigIni
{
public:
	struct structSystem
	{
		CString WindowName;
		CString ProgramName;
		CString MessageName;
		CString SignalName;
		CString XSLFolder;
		CString PathData;
		CString PathTask;
		CString PathSample;
		CString PathCompound;
		CString PathMethod;
		CString PathLog;
		CString DefaultMethod;
		CString Inject;
		CString Instrument;
		int NextSampleID;

		structSystem():NextSampleID(0){}
	};

public:
	CAnalysisConfig(const CString& strIni);
	~CAnalysisConfig();
	
	void Reload();//从文件中加载CurrentSystemName和System
	void Save() const;//将CurrentSystemName和System保存到文件

	bool GetInstruSections(CStringArray& arrSections) const;

protected:
	bool LoadSystem(const CString& systemName, structSystem& system);
	bool LoadCurrentSystem();

public:
	CString CurrentSystemName;
	structSystem System;
};


//该类代表了一种组合：分析仪器+分析样品+分析方法
//同时被分析任务所驱动
///////////////////////////////////////////////////////////////////////////
// CBase_AnalysisTemplate
///////////////////////////////////////////////////////////////////////////

class CBase_AnalysisTemplate
{
public:
	CBase_AnalysisTemplate(const CString& cStrSystemName);
	virtual ~CBase_AnalysisTemplate();

public:
	BOOL IsValid() const{return m_bValid;}

	const CString& GetSystemName() const{return m_cStrSystemName;}

	CBase_AnalysisMethod* GetCurrentMethod(){return m_pcCurrentMethod;}
	CBase_AnalysisSample* GetCurrentSample(){return m_pcCurrentSample;}
	CBase_AnalysisInstrument* GetInstrument(){return m_pcInstrment;}
	CBase_AnalysisInject* GetInject(){return m_pcInject;}
	
	void SetMainWnd(CPAWindowMain* pcWnd){ m_pcWindowMain=pcWnd; }
	void SetTaskWnd(CPAWindowTask* pcWnd){ m_pcWindowTask=pcWnd; }

	virtual TSharedPtr<CBase_AnalysisInstrument> NewInstrument() = 0;
	virtual TSharedPtr<CBase_AnalysisInject> NewInject() = 0;

	const CStringArray& GetWorkFolders() const;//或许移动到Config中？
	
	CString GetSampleFolder(const CString& strWorkFolder) const;
	CString GetMethodFolder(const CString& strWorkFolder) const;

	//AnalysisFile相关的函数，仅接受全路径作为参数
	TSharedPtr<CBase_AnalysisSample> LoadSample(const CString& strPathName) = 0;//如果在内存中不存在，则自动加载
	TSharedPtr<CBase_AnalysisSample> NewSample(const CString& strPathName) = 0;

	TSharedPtr<CBase_AnalysisMethod> LoadMethod(const CString& strPathName) = 0;//如果在内存中不存在，则自动加载
	TSharedPtr<CBase_AnalysisMethod> NewMethod(const CString& strPathName) = 0;

	BOOL ChangeCurrentTask(CString cStrTaskName);
	BOOL ChangeCurrentMethod(CBase_AnalysisMethod* pMethod, BOOL bSaveOldMethod = TRUE);//寻找该名称下的方法，原有方法提示存盘，调用该方法的Run()
	virtual BOOL ChangeCurrentSample(CBase_AnalysisSample *pcNewSample);

protected:
	BOOL						m_bValid;
	CString						m_cStrSystemName;
	CBase_AnalysisMethod		*m_pcCurrentMethod;
	CBase_AnalysisSample		*m_pcCurrentSample;
	CBase_AnalysisInject		*m_pcInject;
	CBase_AnalysisInstrument	*m_pcInstrment;
	CPAWindowTask				*m_pcWindowTask;//任务窗口
	CPAWindowMain				*m_pcWindowMain;//主界面接口
	CArrayI2S					m_cArrayInstrument2Sample;//仪器和样品的链接









	
public:
	CString	GetRuntimeForlder(void);       // 得到当前的运行目录比如： d:\run\ ...
	CString GetDefaultDataFolder(enumPADATATYPENAME nDataType);
	CString GetDataFilename(enumPADATATYPENAME nDataType);

	//------------------------样品有关操作-------------------------------------------
	//

	//样品初始化与显示
	virtual CMDIChildWnd *Sample_LoadAndShow(CString cStrSampleName,int nInjectNo)=0;		//要求显示样品（根据名称调入）
	virtual CMDIChildWnd *Sample_Show(CBase_AnalysisSample *pcSample, BOOL bFreeTheSamplePointer=FALSE)=0;		//要求显示样品

	//样品操作
	virtual void Sample_Reset(CBase_AnalysisSample *pcSample);	//要求样品清除所有数据，状态回到null
	virtual int Sample_LinkToInstrument(CBase_AnalysisSample *pcSample, CBase_AnalysisInstrument *pcInstrument, int nChannel);
	virtual int Sample_PrepareAcquire(CBase_AnalysisSample *pcSample);	//样品准备进行采样
	virtual int Sample_StartRequire(CBase_AnalysisSample *pcSample);	//要求样品开始采样
	virtual int Sample_AbortRun(CBase_AnalysisSample *pcSample)=0;	//要求终止样品采样，返回FALSE表示无法终止
	virtual int Sample_StopRun(CBase_AnalysisSample *pcSample);	//要求样品结束采样，返回FALSE表示无法进入采样结束状态
	virtual int Sample_ReleaseFromInstrument(CBase_AnalysisSample *pcSample);
	virtual int Sample_DataProcess(CBase_AnalysisSample *pcSample);//要求样品进行数据处理
	virtual int Sample_ReportPrint(CBase_AnalysisSample *pcSample, BOOL bAuto=TRUE)=0;//要求样品进行报表打印输出,bAuto=TRUE表示直接缺省输出，不需人工干预
	virtual int Sample_Close(CString cStrSampleName)=0;				//关闭样品
	virtual int Sample_Close(CBase_AnalysisSample *pcSample)=0;		//关闭样品
	//更新样品显示，pcStrParamPath：哪一部分参数
	virtual void Sample_UpdateView(CBase_AnalysisSample *pcSample, const CBase_AnalysisSample::CParamChanged &itemChanged)=0;				//更新样品显示


	//样品事件

	//------------------以下区域的函数流程均已确定-------------------
	virtual int Instrument_SetParam(CBase_AnalysisInstrument *pcInstrument, CBase_AnalysisSample *pcSample);
	virtual int Instrument_SetParam(CBase_AnalysisInstrument *pcInstrument, const CString & cStrXML, const CString & cStrFullXMLpath);

	virtual void SendRealTimeData(CBase_AnalysisInstrument *pcInstrument, int nChannel, CCurveData &curveData);

	//通知消息
	void Notify_SampleDead(CBase_AnalysisSample *pcSample);
	void Notify_InstrumentDead(CBase_AnalysisInstrument *pcInstrument);
	void Notify_StartSignal(CBase_AnalysisInstrument *pcInstrument, int nChannel, DWORD dwTime);
	void Notify_SampleStatusChanged(CBase_AnalysisSample *pcSample);
	void Notify_SampleDataChanged(CBase_AnalysisSample *pcSample, const CBase_AnalysisSample::CParamChanged &itemChanged){Sample_UpdateView(pcSample, itemChanged);}
};

