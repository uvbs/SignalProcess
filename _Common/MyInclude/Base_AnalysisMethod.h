#pragma once


class CBase_AnalysisTemplate;

class CBase_AnalysisMethod : public CObject
{
public:
	CBase_AnalysisMethod(CString cStrName, CBase_AnalysisTemplate *pcTemplate);
	virtual ~CBase_AnalysisMethod(){;}
	//是否方法已经准备好（关键是硬件设置是否到位，所有文件是否都建立）
	BOOL m_bReady;

	//private:
	//方法所包含的数据
	CAnalysisData	m_cCurrentMethodData;
	//分析模板指针
	CBase_AnalysisTemplate  *m_pcTemplate;

public:
	//方法复制
	//除了名称以外，复制其它数据
	void GetDataFrom(CBase_AnalysisMethod &other);

	// 判断该方法文件是否存在
	BOOL IsNameExist(CString cStrName);
	BOOL GetMethodNameList(CStringArray &arrayName);
	static BOOL GetMethodNameList(CBase_AnalysisTemplate *pcTemplate, CStringArray &arrayName);
	CString GetFilePathName() const;
	static CString GetFilePathName(const CString& strMethodName, CBase_AnalysisTemplate* pcTemplate);
	//得到当前方法名称
	CString GetName(void) const;
	//运行当前参数，成功后m_bReady=TRUE;
	BOOL Run(void);
	BOOL IsReady(void);
	//通过名称调入方法
	BOOL Load(CString cStrName);
	//储存方法
	BOOL Save(void);
	BOOL GetInfoString(CString& info);	

	BOOL SaveInstrumentParam(CString cStrXMLPath, CString xmlString);
//	BOOL SetInstrumentParam(CString cStrXMLPath, BOOL bDataToWindow);

	BOOL RunToStruct(LPVOID lpvStruct);
	BOOL SetInstrumentParamToStruct(CString cStrXMLPath, LPVOID lpvStruct);

	//BOOL GetInstrumentParamSize(int& nCols, int& nRows);
	//BOOL GetInstrumentParam(int* pMZ,   CString* pCompoundName, double* pSensitivity, double* pRationTable,  double* pNoise);
	CString GetInstrumentParam(CString cStrXMLPath);
	void PreviewCurruentMethod();
	BOOL SaveAs(const CString& name);//仅名称，不包括路径和后缀
	BOOL Construct();

	CAnalysisData& GetAnalysisData() { return m_cCurrentMethodData; }
private:


};

