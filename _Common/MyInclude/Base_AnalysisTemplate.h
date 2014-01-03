#pragma once


//#include "AnalysisMethod.h"
#include "Base_AnalysisSample.h"
#include "Base_AnalysisInstrument.h"
//#include "AnalysisInject.h"


//
//该类代表了一种组合：分析仪器+分析样品+分析方法
//同时被分析任务所驱动
//
//同一个模板下不能有相同的方法、样品名称

enum enumPADATATYPENAME
{
	PA_DATATYPENAME_DATA			=1,		//仪器参数目录
	PA_DATATYPENAME_SAMPLE			=2,		//样品数据目录
	PA_DATATYPENAME_TASK			=3,		//任务文件目录
	PA_DATATYPENAME_COMPOUND		=4,		//化合物信息库目录
	PA_DATATYPENAME_METHOD			=5,		//方法目录
	PA_DATATYPENAME_LOG				=6,		//日志基础目录
	PA_DATATYPENAME_CRYSTALREPORT	=7,

	PA_DATATYPENAME_DEFAULTMETHOD	=6,		//方法文件
	PA_DATATYPENAME_INJECT			=7,		//进样器配置文件
	PA_DATATYPENAME_INSTRUMENT		=8,		//仪器参数文件
};


class CPAWindowTask;
class CPAWindowMain;

struct CInstrument2Sample {
	CBase_AnalysisInstrument *m_pcInstrument;
	int m_nChannel;
	CBase_AnalysisSample *m_pcSample;

	CInstrument2Sample(): m_pcSample(NULL), m_pcInstrument(NULL), m_nChannel(0){}

};

typedef CArray <CInstrument2Sample, CInstrument2Sample&> CArrayI2S;



class CConfigIni 
{
public:
	struct structSystem
	{
		CString m_szWindowName;
		CString m_szProgramName;
		CString m_szMessageName;
		CString m_szSignalName;
		CString m_szXSLFolder;
		CString m_szPathData;
		CString m_szPathTask;
		CString m_szPathSample;
		CString m_szPathCompound;
		CString m_szPathMethod;
		CString m_szPathCrystalReport;
		CString m_szPathLog;
		CString m_szDefaultMethod;
		CString m_szInject;
		CString m_szInstrument;
		CString m_szLastTask;
		int m_iRGACOM;
		int m_iValveCOM;
		int m_iPumpCOM;
		int m_iNextSampleID;
		BOOL m_bShowAutoSampler;

		structSystem():m_iRGACOM(0),m_iValveCOM(0),m_iPumpCOM(0),m_iNextSampleID(0),m_bShowAutoSampler(false){}
	};

public:
	CConfigIni();
	virtual ~CConfigIni();

	bool LoadConfig(LPCTSTR strIni = NULL);
	bool SaveConfig(LPCTSTR strIni = NULL) const;

	inline const CString& CurrentSystemName() const{return m_szName;}
	bool SetCurrentSystemName(const CString& strSystem, bool bRefresh);
	
	inline const structSystem& CurrentSystem() const{return m_System;}
	inline structSystem& CurrentSystem(){return m_System;}

	bool LoadSections(CStringArray& arrSections) const;
	bool LoadInstruSections(CStringArray& arrSections) const;

	bool LoadString(const CString& strSection, const CString& strItem, CString& value) const;
	bool LoadPath(const CString& strSection, const CString& strItem, CString& value) const;
	bool LoadInt(const CString& strSection, const CString& strItem, int& value) const;
	
	bool SaveString(const CString& strSection, const CString& strItem, const CString& value) const;
	bool SavePath(const CString& strSection, const CString& strItem, const CString& value) const;
	bool SaveInt(const CString& strSection, const CString& strItem, int value) const;

	bool SaveSystemName(const CString& strSystem) const;

	const CString& ConfigPath() const{return m_szPath;}

private:
	CString m_szName;
	structSystem m_System;
	CString m_szIni;
	CString m_szPath;

protected:
	bool LoadSystem(const CString& systemName, structSystem& system);
	bool SaveSystem(const CString& systemName, const structSystem& system) const;
	bool LoadCurrentSystem();

	static const TCHAR l_szIniFilename[];
	static const TCHAR l_szIniKey_Sys[];
	static const TCHAR l_szIniItem_Sys[];
	static const TCHAR l_szIniItem_PathData[];
	static const TCHAR l_szIniItem_PathTask[];
	static const TCHAR l_szIniItem_PathSample[];
	static const TCHAR l_szIniItem_PathCompound[];
	static const TCHAR l_szIniItem_PathMethod[];
	static const TCHAR l_szIniItem_PathCrystalReport[];	
	static const TCHAR l_szIniItem_PathLog[];
	static const TCHAR l_szIniItem_DefaultMethod[];
	static const TCHAR l_szIniItem_Inject[];
	static const TCHAR l_szIniItem_Instrument[];
	static const TCHAR l_szIniItem_RGACOM[];
	static const TCHAR l_szIniItem_ValveCOM[];
	static const TCHAR l_szIniItem_PumpCOM[];
	static const TCHAR l_szIniItem_NextSampleID[];	
	static const TCHAR l_szIniItem_WindowName[];
	static const TCHAR l_szIniItem_ProgramName[];
	static const TCHAR l_szIniItem_MessageName[];
	static const TCHAR l_szIniItem_SignalName[];
	static const TCHAR l_szIniItem_XSLFolder[];
	static const TCHAR l_szIniItem_LastTask[];
	static const TCHAR l_szIniItem_ShowAutoSampler[];
};

class CBase_AnalysisTemplate
{
public:
	CBase_AnalysisTemplate(CString cStrSystemName);
	virtual ~CBase_AnalysisTemplate();

	BOOL m_bValid;
	BOOL IsValid(void){return m_bValid;}

protected:

	//
	//----------业务数据参数-------------------------------------
	//
	//系统名称
	CString				m_cStrSystemName;

	//仪器类指针
	CBase_AnalysisInstrument	*m_pcInstrment;
	//当前样品类指针
	CBase_AnalysisSample		*m_pcCurrentSample;
	//当前方法数据指针
	CBase_AnalysisMethod		*m_pcCurrentMethod;
	//进样器配置参数
	CBase_AnalysisInject		*m_pcInject;
	//采集器指针
	//CBase_AnalysisAcquirer		*m_pcAcquirer;

	//仪器和样品的链接
	CArrayI2S	m_cArrayInstrument2Sample;

	// [2013-01-16 chainyu]
	CMapStringToPtr m_mapSample; //对比打开样品文件的对象指针
	//int m_pSampleRefCount; // 样品指针的引用计数
	//virtual BOOL ParseSampleFileFormat(CString& strPath);


public:
	CString	GetSystemName(void) {return m_cStrSystemName;}

	CBase_AnalysisInstrument	*GetInstrument(void)	{return m_pcInstrment;}
	CBase_AnalysisSample		*GetCurrentSample(void)	{return m_pcCurrentSample;}
	CBase_AnalysisMethod		*GetCurrentMethod(void)	{return m_pcCurrentMethod;}
	CBase_AnalysisInject		*GetInject()			{return m_pcInject;}
//	CBase_AnalysisAcquirer		*GetAcquirer()			{return m_pcAcquirer;}

	//
	//----------------------------重要界面类指针------------------------------
	//
	CPAWindowTask			*m_pcWindowTask;//任务窗口
	CPAWindowMain			*m_pcWindowMain;//主界面接口
public:
	void SetMainWnd(CPAWindowMain* pcWnd){ m_pcWindowMain=pcWnd; }
	void SetTaskWnd(CPAWindowTask* pcWnd){ m_pcWindowTask=pcWnd; }

	//
	//运行环境参数
	//
public:
	CString	GetRuntimeForlder(void);       // 得到当前的运行目录比如： d:\run\ ...
	CString GetDataFolder(enumPADATATYPENAME nDataType);
	CString GetDataFilename(enumPADATATYPENAME nDataType);

	// 
	//----------------------------任务有关操作--------------------------------------------
	// 
	BOOL ChangeCurrentTask(CString cStrTaskName);//{m_pcWindowMain->Nodify_CurrentTaskChanged(cStrTaskName);}

	//
	//----------------------------方法有关操作--------------------------------------------
	//
	CBase_AnalysisMethod	*CreateNewMethod(CString cStrName, CString cStrNameFrom=_T(""));//cStrNameFrom==_T("")表示缺省方法//调用者负责释放
	CBase_AnalysisMethod	*LoadMethod(CString cStrMethodName);//调用者负责释放指针
	BOOL GetMethodNameList(CStringArray &arrayMethod){return m_pcCurrentMethod->GetMethodNameList(this, arrayMethod);}
	BOOL ChangeCurrentMethod(CString cStrMethod, BOOL bSaveOldMethod=TRUE);//寻找该名称下的方法，原有方法提示存盘，调用该方法的Run()
	BOOL ChangeCurrentMethod(CBase_AnalysisMethod* pMethod, BOOL bSaveOldMethod=TRUE);

	//
	//------------------------样品有关操作-------------------------------------------
	//

	//样品初始化与显示
	virtual CBase_AnalysisSample *CreateNewSample(CString cStrName, int nInjectNO, CString cStrNameFrom)=0;	//调用者负责释放
	//virtual void DestroySample(CBase_AnalysisSample *pcSample){delete pcSample;}
	virtual CBase_AnalysisSample *LoadSample(CString cStrSampleName, int nInjectNO)=0;						//调入样品，调用者负责释放pcSample
	virtual CBase_AnalysisSample *LoadSampleFile(CString cStrSampleFullPathFileName)=0;						//调入样品，调用者负责释放pcSample
	virtual CMDIChildWnd *Sample_LoadAndShow(CString cStrSampleName,int nInjectNo)=0;		//要求显示样品（根据名称调入）
	virtual CMDIChildWnd *Sample_Show(CBase_AnalysisSample *pcSample, BOOL bFreeTheSamplePointer=FALSE)=0;		//要求显示样品

//	virtual void Sample_ParamChanged(CString cStrParamXMLPath){AfxMessageBox(_T("此函数需要废除，用Notify_SampleDataChanged代替！"));}

	//样品操作
	virtual void Sample_Reset(CBase_AnalysisSample *pcSample);	//要求样品清除所有数据，状态回到null
	virtual int Sample_LinkToInstrument(CBase_AnalysisSample *pcSample, CBase_AnalysisInstrument *pcInstrument, int nChannel);
	virtual int Sample_PrepareAcquire(CBase_AnalysisSample *pcSample);	//样品准备进行采样
	virtual int Sample_StartRequire(CBase_AnalysisSample *pcSample);	//要求样品开始采样
	virtual int Sample_AbortReady(CBase_AnalysisSample *pcSample);	//要求终止样品准备
	virtual int Sample_AbortRun(CBase_AnalysisSample *pcSample)=0;	//要求终止样品采样
	virtual int Sample_StopRun(CBase_AnalysisSample *pcSample);	//要求样品结束采样
	virtual int Sample_ReleaseFromInstrument(CBase_AnalysisSample *pcSample);
	virtual int Sample_DataProcess(CBase_AnalysisSample *pcSample);//要求样品进行数据处理
	virtual int Sample_ReportPrint(CBase_AnalysisSample *pcSample, BOOL bAuto=TRUE)=0;//要求样品进行报表打印输出,bAuto=TRUE表示直接缺省输出，不需人工干预
	virtual int Sample_Close(CString cStrSampleName)=0;				//关闭样品
	virtual int Sample_Close(CBase_AnalysisSample *pcSample)=0;		//关闭样品
	//更新样品显示，pcStrParamPath：哪一部分参数
	virtual void Sample_UpdateView(CBase_AnalysisSample *pcSample, const CBase_AnalysisSample::CParamChanged &itemChanged)=0;				//更新样品显示

	virtual BOOL ChangeCurrentSample(CBase_AnalysisSample *pcNewSample);
	virtual BOOL CurrentSample_Changed(CBase_AnalysisSample *pcNewSample);//  [12/17/2012 洪浩] 该函数应该在OnMDIActivate中调用，表明当前样品已经改变，而不是将要改变

	//样品事件
	//
	//------------------------仪器有关操作--------------------------
	//
	virtual CBase_AnalysisInstrument *CreateNewInstrument(void)=0;//{return new CBase_AnalysisInstrument(this);}//调用者负责释放
	virtual CBase_AnalysisInject* CreateNewInject()=0;// {return new CBase_AnalysisInject(_T("色谱进样器"),this);}

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

	void ZeroFlag(CBase_AnalysisInstrument *pcInstrument, bool bZero);
};

