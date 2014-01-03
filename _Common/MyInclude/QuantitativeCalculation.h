#pragma once
#include "CaliData.h"


/* 
	######################################################################################################################################

				该类作为校正后期对未知样品进行峰匹配和定量计算的业务逻辑用
				也可以考虑把峰匹配和定量计算分割成2个类

	######################################################################################################################################
*/


class CQuantitativeCalculation  
{
public:
	 
	CQuantitativeCalculation(CAnalysisData *pAnalysisData);
	~CQuantitativeCalculation(void);

	//对外接口:整个定量计算过程
	int QuantitativeCalculationProcess(); 

private:
	typedef map<int, CaliItem> MatchType;
	 

	CString m_Method; //校正方法
	CString m_Object; //校正对象

	CCaliData m_cDuplicate;  
	CAnalysisData *pcData;
	MatchType m_Result;

	bool SetCaliData();
	bool SetMethodAndObject();
	bool ReadyCheckPass();  
	CString GetCalibrateObjectAttr();   

	//峰匹配
	void GetRef_IntPeakRow_ByArea_or_Height(MatchType &PeakRows);  
	void Remove_Repeat_PeakRow(MatchType &result);  
	void GetOtherPeakRow_ByResTime(double compResTime, MatchType &PeakRows);  
	int MatchRefReak();  
	int MatchIntReak();   
	int MatchOtherReak();   

	//数据转移
	void ClearTableRowData(CIADataTable *pTable);  //清空表格内行数据，保留属性   
	void PeakListData2ResultData(const CStringArray &PeakList, CStringArray &ResultData, int nID);  
	void GenerateResultData(); //用m_Result生成ResultData 
	void DirectGenerateResultData(); //直接用m_cDuplicate生成ResultData
	
	//定量计算
	void QC_percentage(); //定量：百分比法  
	void QC_external(); //定量：外标法  
	void QC_internal(); //定量：内标法  
	void QC_external_percentage(); //定量：外标百分比法 
	void QC_internal_percentage(); //定量：内标百分比法  
	void QC_normalization(); //定量：外标归一化法  
	CString ConvertScientificNotation(double contents); //转换成符合科学计数法的

	double GetProductFactor();
	double GetDilutionFactor();
	double GetSampleAmount();
	double GetColumnTotal(CIADataTable *cTable, const CString& strColumn);  
	int FindMatchedCompoundGroupRow(const CString& cStrInteriorNo);
	void FindInteriorContentAndAttr(const CString& cStrInteriorNo, CaliItem& IntItem);
	void FindReMatchedPeakListItem(const CString& cStrInteriorNo, CaliItem& peakListItem); //由内标组份反匹配到Peaklist某行数据
	int QuantitativeCalculation();
};

