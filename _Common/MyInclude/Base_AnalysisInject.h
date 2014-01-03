#pragma once

struct structInletValve
{
	CString Valve;
	CString Name;
};
typedef CArray<structInletValve, structInletValve&> INLETVALVES;

class CBase_AnalysisInject : public CObject{
protected:
	CBase_AnalysisInject(LPCTSTR name=_T(""), CBase_AnalysisTemplate* pcTemplate=NULL);
	virtual ~CBase_AnalysisInject();
	friend class CBase_AnalysisTemplate;

protected:
	CBase_AnalysisTemplate* m_pTemplate;
	CAnalysisData m_data;
	int m_iInjectDataNum;
	CString m_cStrName;

public:
	//初始化，使用前需调用一次
	virtual BOOL Initialize(void)=0;

	CAnalysisData* GetInject() {return &m_data;}
	virtual BOOL initValve(int iNum){AfxMessageBox(_T("initValve()未实现"));return FALSE;}
	virtual BOOL initValveParam(int idx, LPCTSTR lpcszName, int iValve, CBase_AnalysisMethod* pMethod){AfxMessageBox(_T("initValveParam()未实现"));return FALSE;}
	virtual void startScan(){AfxMessageBox(_T("startScan()未实现"));}
	virtual void endScan(){AfxMessageBox(_T("endScan()未实现"));}

	bool GetInletValves(INLETVALVES& v) const;

public:
	CAnalysisData* GetXMLData() {return &m_data;}


};
