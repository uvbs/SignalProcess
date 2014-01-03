#pragma once
#include "AnalysisData.h"
#include "Base_SampleChart.h"


class CBase_AnalysisTemplate;
class CTaskItemOption;
class CBase_AnalysisMethod;
class CBase_AnalysisInstrument;

#define ANALYSIS_SAMPLE_FILE_EXT _T(".cg.smp")
#define STRING_MAINCURVE_GROUP		_T("ResultPoints")
#define STRING_MAINCURVE_NAME		_T("AcquireData")
//"样品名称#进样号__Curve.csv"
#define STRING_MAINCURVE_FILENAME	_T("%s#%d_Curve.csv")


#define XMLPROP_SAMPLECHART_RANGEMIN_BOTTOM _T("rangeBottomMin")
#define XMLPROP_SAMPLECHART_RANGEMAX_BOTTOM _T("rangeBottomMax")
#define XMLPROP_SAMPLECHART_RANGEMIN_LEFT _T("rangeLeftMin")
#define XMLPROP_SAMPLECHART_RANGEMAX_LEFT _T("rangeLeftMax")



class CPeakStandard
{
public:
	CPeakStandard():m_dReserveTime(0.0),m_dResultConcentration(0.0){}

	union CPeakType 
	{
		CPeakType():all(0){}
		struct {
			unsigned short negative:			1;	//负峰
			unsigned short single :				1;	//单峰
			unsigned short overlap_pl:			1;	//重叠峰
			unsigned short overlap_ls:			1;	//重叠峰(左肩)
			unsigned short overlap_rs:			1;	//重叠峰(右肩)
			unsigned short overlap_vs:			1;	//重叠峰(垂直分峰)
			unsigned short tail:				1;	//拖尾
			//------
			unsigned short reserve2future:		9;	//以后用				
	};
	unsigned short all;

		//--------
		void CleanMark(void){all=0;}
		//获得峰类型的文字描述
		CString ConvertToTypeName(void); 
		void	GetFromTypeName(CString cStrName);
	} ;


public:
	CPeakType			m_cPeakType;	//峰类型
	//int					m_nType;		//峰类型
	double				m_dReserveTime;	//保留时间
	CCurveDataSingle	m_dataBegin;    //峰起点
	CCurveDataSingle	m_dataTop;		//峰顶（峰，谷）
	CCurveDataSingle	m_dataEnd;      //峰落点
	double				m_dHalfWidth;	//半峰宽
	double				m_dWidth;		//峰宽
	double				m_dHeight;		//峰高
	double				m_dHeightPer;	//峰高%
	double				m_dArea;		//峰面积
	double				m_dAreaPer;		//峰面积%
	double				m_dDisfactor;   //畸变因子??
	double				m_dSymmetry;    //拖尾因子
	double				m_dResolution;  //分离度
	double				m_dColumnPlate; //理论塔板数
	//以下在ResultTable里使用
	double				m_dResultConcentration;	//浓度：分析结果（百分比%）在分析结果表中使用，峰列表中不用显示
	CString				m_cStrComponentName;//组份名称
};

class CPeakList
{
//private:
//	CPeakList(){}
public:
	CString			m_cStrPathName;
	CAnalysisData *m_pcDataXML;
public:
	CPeakList(const CString &cStrPathName, CAnalysisData *pcDataXML){m_cStrPathName=cStrPathName;m_pcDataXML=pcDataXML;}
	void RemoveAll(void){m_cArrayData.RemoveAll();}
	void Add(CPeakStandard &item){m_cArrayData.Add(item);}

	//将m_cArrayData数据同步到m_pcDataXML中，按照m_cStrPathName路径
	BOOL SynchronizeToXML(void); //{AfxMessageBox(_T("小于：将m_cArrayData数据同步到m_pcDataXML中，按照m_cStrPathName路径"));}
	//从m_pcDataXML中，按照m_cStrPathName路径获得数据到m_cArrayData
	BOOL SynchronizeFromXML(void); //{AfxMessageBox(_T("从m_pcDataXML中，按照m_cStrPathName路径获得数据到m_cArrayData"));}
	BOOL SyncFromXML_PeakListTable(CIADataTable* pCIADataTable);
	BOOL SyncFromXML_ResultTable(CIADataTable* pCIADataTable);


	//峰处理结果数据
	CArray<CPeakStandard,CPeakStandard&> m_cArrayData;
};



//通用样品类
class CBase_AnalysisSample : public CObject
{
	friend class CBase_AnalysisTemplate;
	friend class CBase_SampleChart;

public:
	enum enumSampleType
	{
		SAMPLE_TEST,
		SAMPLE_STANDARD,
	};

	enum STATUS
	{
		null = 0		,	//空闲（空样品）					
		preparing		,	//样品准备中（）						(已经连接仪器)
		ready			,	//样品自己准备好了（和仪器没关系）	(已经连接仪器)
		running			,	//采样								(已经连接仪器)	
		idle			,	//运行状态结束，可中断，但未完成数据处理
		finish			,	//完成						
		abort			,	//异常终止，或出错，数据应该都废除
		busy				//样品忙，不可中断状态
	};

	enum enumDrawType
	{
		DT_NONE = 0,
		DT_LINE = 1,
		DT_POINT = 2,
	};

	//用于标记样品哪一部分参数已经更新。
	union CParamChanged{
		CParamChanged():allparameters(0){}
		struct {
			unsigned short curve_main :			1;	//主曲线
			unsigned short curve_temperature:	1;	//温度曲线

			unsigned short param_integration:	1;	//积分参数
			unsigned short param_information:	1;	//样品信息
			unsigned short param_instrument:	1;	//仪器参数
			unsigned short param_calibration:	1;	//校正参数（表）

			unsigned short result_integration:	1;	//积分结果
			unsigned short result_analysis:		1;	//分析结果

			unsigned short filepath:			1;	//存盘路径
			//------
			unsigned short reserve2future:		7;	//以后用				
		};
		unsigned short allparameters;
		
		//--------
		void CleanMark(void){allparameters=0;}
		void FullMark(void) {allparameters=0xFFFF;}
		void Params(){param_integration=1;param_information=1;param_instrument=1;param_calibration=1;/*param_systemAdvice=1;*/}
	};
public:
	CBase_AnalysisSample(LPCTSTR cStrName, int nInjectNO, CBase_AnalysisTemplate* );
	virtual ~CBase_AnalysisSample();

	//样品所有参数
	CBase_AnalysisTemplate*	m_pTemplate;		//分析模版
	DWORD					m_dStartAcquireTime_mins;//开始采样时间，相对时间不是真正的时间
	CString					m_cStrSavePath;		//样品文件所在路径（图形文件的路径与此相同）
	CString					m_cStrMainFileName;	//样品文件的名称


	//
	//---------------------------------------------------------------------
	//-----------------------------样品参数--------------------------------
	//---------------------------------------------------------------------
	// 
	public:
		CAnalysisData			m_data;				//样品所有参数
		//以下为临时参数，必须与m_data数据保持同步。为了提高效率
		//积分结果
		CPeakList			m_cPeakList;
		CPeakList			m_cResultList;

		//临时数据，每次积分后赋值
		CArray<double>		m_arrayInterRes1;	//最小半峰宽条件之前的积分结果（半峰宽）
		CArray<double>		m_arrayInterRes2;	//最小峰面积条件之前的积分结果（半峰宽）
		CArray<double>		m_arrayInterRes3;	//最小峰高条件之前的积分结果（半峰宽）
		struct structLine
		{
			struct structPoint
			{
				double x,y;
			}begin,end;
		};
		CArray<structLine>	m_arrayVLines;		//垂直分峰线
		double				m_dMaxThreShold;	//最大的噪声斜率
		double				m_dMinThreShold;	//最小的噪声斜率
	protected:
		//曲线相关数据
		CCurveData		m_dataMainCurve;

	//
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	// 


private:
	//主曲线存盘文件名
//	CString			m_cStrCurveFileName;
	bool m_bDeleted;




public:

	//
	//--------------样品参数的读写-------------------
	//
	//样品类型
	enumSampleType GetType() const;
	bool SetType(enumSampleType eType);
	LPCTSTR TypeDescription() const;
	static LPCTSTR ToTypeDescription(enumSampleType eType);
	static LPCTSTR ToTypeString(enumSampleType eType);
	static enumSampleType FromTypeString(const CString& typeString, enumSampleType eDefault =  SAMPLE_TEST);
	static CString ToInstrumentChannelString(int iSignalChannel){return CString((TCHAR)('A' + iSignalChannel));}

	//样品名称
	CString GetSampleName() const;//不带进样号！！！！！！！不能用作文件名
	bool ChangeSampleName(LPCTSTR lpcszName, int nInjectNO);
	//带后缀的文件名，但不包括路径！！！！！！
	//此函数只在第一次新样品的时候使用
	
	static CString CreateSampleFileName(CString cStrSampleName, int nInjectNO) {CString cStrTemp;cStrTemp.Format(_T("%s_#%d%s"), cStrSampleName, nInjectNO, ANALYSIS_SAMPLE_FILE_EXT);return cStrTemp;}
	CString CreateSampleFileName() {return CreateSampleFileName(GetSampleName(), GetInjectNO());}
	CString GetSampleFileName() const{return m_cStrMainFileName;}//带后缀的文件名，但不包括路径！！！！！！

	//进样号
	int GetInjectNO() const;
	bool SetInjectNO(int iNO);

	//仪器信号通道
	int GetInstrumentChannel(void) const;

private:
	friend class CAnalysisTaskItem;
	bool SetInstrumentChannel(int iChannel){return m_data.SetValue_Int(XMLPATH_SAMPLE_PARAM_INSTRU_SC, iChannel) == IAERROR_SUCCESS;}
	bool SetChannel(int iChannel){return m_data.SetValue_Int(XMLPATH_SAMPLE_PARAM_INSTRU_IC, iChannel) == IAERROR_SUCCESS;}
	bool SetAcquisitionTime(DWORD dwMinutes){return m_data.SetValue_Int(XMLPATH_SAMPLE_PARAM_INSTRU_AT, dwMinutes) == IAERROR_SUCCESS;}

public:
	//采用的方法
	CString GetSourceMethod() const;
	bool SetSourceMethod(LPCTSTR lpcszMethodName);
	CString GetCurMethod() const;

	//分析次数
	//int GetCount() const;
	//bool SetCount(int iCount);

	//绘画类型
	enumDrawType GetDrawType() const;
	bool SetDrawType(enumDrawType eDrawType);

	//综合设置
	//bool SetAttribute(LPCTSTR lpcszName, LPCTSTR lpcszMethodName, int iCount);

	int GetIADataCurve2D(CIADataCurve2D& xmlC2D) const;
	//样品曲线数据主组和主组中的主曲线
	bool GetIADataCurve2D_MainInfo(CString* pMainGroup, CString* pMainSerie) const
	{
		//目前规定死；如果是正式版，需从m_xmlC2D中获取；若不存在，则自动创建
		if(pMainGroup) *pMainGroup = _T("ResultPoints");
		if(pMainSerie) *pMainSerie = _T("AcquireData");
		return true;
	}

	//曲线相关函数
	CString CreateMainCurveFileName(void){CString cStrTemp; cStrTemp.Format(STRING_MAINCURVE_FILENAME, GetSampleName(), GetInjectNO()); return cStrTemp;}
	CString GetMainCurveFileName(void){return GetCurveFileName(STRING_MAINCURVE_GROUP, STRING_MAINCURVE_NAME);}
	CString CreateCurveFileName(const CString& cStrMainGroup, const CString& cStrMainSerie){CString cStrTemp; cStrTemp.Format(_T("%s#%d_%s@%s"), GetSampleName(), GetInjectNO(),cStrMainSerie, cStrMainGroup); return cStrTemp;}
	CString GetCurveFileName(const CString& cStrMainGroup, const CString& cStrMainSerie);





public:
	//
	//-----------------基础功能-----------------------------
	//
	//样品当前状态
	CBase_AnalysisSample::STATUS GetStatus(void) const{return m_statusCurrent;}
	LPCTSTR GetStatusString(void) const
	{
		switch(m_statusCurrent)
		{
			case null:return _T("空样品");
			case preparing:return _T("样品准备");
			case ready:return _T("样品准备好");		
			case running:return _T("采样中...");		
			case idle:return _T("采样结束");		
			case finish:return _T("完成");		
			case abort:return _T("异常终止");		
			case busy:return _T("样品忙");
			default:return _T("未知状态");
		}
	}
	void SetStatus(CBase_AnalysisSample::STATUS status);
	private:
		//该变量不能直接赋值(包括在本类和派生类中)，必须调用ChangeStatus(),此函数会做一些通知
		CBase_AnalysisSample::STATUS	m_statusCurrent;


public:
	//static BOOL IsNameExist(LPCTSTR, CBase_AnalysisTemplate*);
	static BOOL IsNameExist(LPCTSTR lpcszName, int nInjectNO,CBase_AnalysisTemplate* pTemplate);

	static BOOL Delete(LPCTSTR, CBase_AnalysisTemplate*);
	static BOOL GetSampleNameList(CStringArray &arrayName, CBase_AnalysisTemplate*);

	//与XML数据相关的函数
protected:
	virtual	void XML_InitMainCurveData(void);
	void UpdateXMLCurve2DNames();

public:
		bool Xml_PutParameter_Method(CAnalysisData& adMethod);
		bool Xml_PutParameter_Method(LPCTSTR xmlString);//传入的是整个方法文件的xml文本（包含Method标记）
		bool Xml_PutParameter_Instrument(LPCTSTR xmlString);//传入的是整个Method/Instrument的全部内容（包含Instrument标记）
		bool Xml_PutParameter_Calibrate(LPCTSTR xmlString);//传入的是整个Method/Calibrate的全部内容（包含Calibrate标记）
		bool Xml_PutParameter_PeakProcess(LPCTSTR xmlString);
		bool Xml_PutParameter_Report(LPCTSTR xmlString);
		bool Xml_GetParameter_Method(CString& xmlString) const;
		bool Xml_GetParameter_Instrument(CString& xmlString) const;
		bool Xml_GetParameter_Calibrate(CString& xmlString) const;
		bool Xml_GetParameter_PeakProcess(CString& xmlString) const;
		bool Xml_GetParameter_Report(CString& xmlString) const;

		CAnalysisData& GetAnalysisData(){return m_data;}



public:
	//
	//-----------------基本功能-----------------------------
	//
	virtual void GetDataFrom(const CBase_AnalysisSample &src){m_data.SetXMLString(src.m_data.GetXMLString());}
	virtual void DuplicateFrom(const CBase_AnalysisSample &src){GetDataFrom(src);m_pTemplate = src.m_pTemplate;}
	virtual void GetDataFrom(const CBase_AnalysisMethod &src);
	 
	void CreateID();
	//通过样品名称从标准目录中调入
	BOOL Load(LPCTSTR lpszSampleName,int nInjectNO);
	BOOL LoadSampleFile(CString cStrFullPathName);
	BOOL Save();
	BOOL Delete();

public:
	//
	//-------------基础的样品操作函数------------------
	//
	virtual BOOL Base_IsReady(void){if(GetStatus()==ready) return TRUE; else return FALSE;}
	virtual BOOL Base_GetReady(void);//该函数留一个机会给样品做自己的判断或操作，以做样品本身采样前的准备
	virtual void Base_ResetData(void);
	virtual int	 Base_StartRun(double dTimeMins);
	virtual int  Base_StopRun(void);
	virtual int  Base_AbortRun(void);
	virtual int	 Base_DataProcess(void);
	virtual int	 Base_DataCalibrate(void);
	virtual int  Base_ReportPrint(BOOL bAutoPrint=TRUE)=0;

public:
	CBase_AnalysisMethod*	m_pMethod;			//采用的方法
	CBase_SampleChart*		m_pChartCtrl;		//对应的曲线窗口
	virtual void SetChartCtrl(CBase_SampleChart* pChartCtrl){m_pChartCtrl=pChartCtrl;}
	virtual void RefreshDraw(enumDrawType eDrawType);
	COleDateTime m_dtStartRun,m_dtStopRun;//开始和结束采样的时间
	bool m_bNeedStop;
	DWORD m_dwAutoStopMins;


	//-----------以下区域的函数流程均已确定------------------------------------
	//-----------------------------------------------------------------------
public:
	virtual int Base_RealtimeData(const CCurveData &curveData);

public:
	bool SaveChart2File(int iWidth, int iHeight, const CString& file);
};
