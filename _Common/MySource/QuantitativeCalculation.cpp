#include "StdAfx.h"
#include "QuantitativeCalculation.h"


CQuantitativeCalculation::CQuantitativeCalculation(CAnalysisData *pAnalysisData)
	:pcData(pAnalysisData) 
{
}

CQuantitativeCalculation::~CQuantitativeCalculation(void)
{
}

int CQuantitativeCalculation::QuantitativeCalculationProcess() {
	if (!ReadyCheckPass()) return IAERROR_XML_NOTVALID;  //定量分析初始数据无法进行之后的业务逻辑
	
	//峰匹配过程

	//提取标记参考峰
	//落在参考峰区间内的峰，为参考峰；若有多个峰落在相同区间，按方法取面积或峰高为参考峰
	//找到所有参考峰后参照公式，修正校正表内其他峰的保留时间
	if(MatchRefReak()!=IAERROR_SUCCESS) return IAERROR_XML_NOTVALID; 

	//提取标记内标峰
	//根据已修正的保留时间窗口，识别内标峰。若存在多个，按方法取面积或峰高选最大峰
	if(MatchIntReak()!=IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;  

	//提取其他峰
	//识别其他校正峰，若存在多个，取保留时间最接近的
	if(MatchOtherReak()!=IAERROR_SUCCESS) return IAERROR_XML_NOTVALID; 

	//根据峰匹配结果序列，生成结果表数据
	if (m_Method == _T("百分比法")) {
		DirectGenerateResultData();
		return IAERROR_SUCCESS;
	}
	else
		GenerateResultData();

	//定量计算过程
	if(QuantitativeCalculation()!=IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;

	CString szWatch = pcData->GetXMLString();
	return IAERROR_SUCCESS;
}

bool CQuantitativeCalculation::SetMethodAndObject() {
	CString szWatch = pcData->GetXMLString();
	if(pcData->GetValue_String(XMLPATH_SAMPLE_PARAM_CALI_METHOD, m_Method)!=IAERROR_SUCCESS) return false;
	if(pcData->GetValue_String(XMLPATH_SAMPLE_PARAM_CALI_OBJECT, m_Object)!=IAERROR_SUCCESS) return false;
	return true;
}

bool CQuantitativeCalculation::SetCaliData() {
	if (pcData==NULL) return false;
	CIADataTable cCalibrateResultTable;
	//if (m_Method == _T("百分比法")) {
	//	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cCalibrateResultTable) != IAERROR_SUCCESS) return false;
	//}
	//else
	//{
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_PARAM_CALI_RESULT_TABLE, cCalibrateResultTable) != IAERROR_SUCCESS) return false;
	//}
	

	//if(!m_cDuplicate.AutoAddItem(&cCalibrateResultTable, XMLPATH_SAMPLE_PARAM_CALI_RESULT_TABLE)) return false;
	m_cDuplicate.InitCaliData(&cCalibrateResultTable);
	return true;
}

bool CQuantitativeCalculation::ReadyCheckPass() {
	if (pcData==NULL) return false;
	if (!SetMethodAndObject()) return false;
	if (m_cDuplicate.GetItemSize()==0) 
		if (!SetCaliData())
			return false;
	return true;
}

/************************************************************************************************************************************
/
/                                                  峰匹配 相关函数
/
/************************************************************************************************************************************/

void CQuantitativeCalculation::GetRef_IntPeakRow_ByArea_or_Height(MatchType &PeakRows) {
	if (PeakRows.empty()) return;
	//按方法取面积或峰高为参考峰
	if (PeakRows.size()>=2) {
		CString szAttr=GetCalibrateObjectAttr();

		CIADataTable cPeakList;
		if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return;

		MatchType::iterator iter=PeakRows.begin(); 

		int maxVal_Row=iter->first;			
		double maxVal=0.0f;

		for ( ; iter!=PeakRows.end(); ++iter) {
			CString szVal;
			int row = iter->first;
			if (!cPeakList.GetCellValue(row, szAttr, szVal)) return;
			double dVal=_tstof(szVal);
			if (dVal > maxVal)	{
				maxVal = dVal;
				maxVal_Row = row;
			}
		}
		m_Result.insert(make_pair(maxVal_Row, PeakRows[maxVal_Row]));
	}
	else
		m_Result.insert(PeakRows.begin(), PeakRows.end());
}


void CQuantitativeCalculation::Remove_Repeat_PeakRow(MatchType &result){
	MatchType tmpRow;
	typedef MatchType::iterator Iter;
	typedef MatchType::const_iterator CIter;

	bool repeat=false;	
	for (Iter restIter=result.begin(); restIter!=result.end(); ++restIter) {
		repeat = false; // 重置标记，不然一次重复会默认全部重复情况
		for (CIter sourIter=m_Result.begin(); sourIter!=m_Result.end(); ++sourIter) {
			if (restIter->first==sourIter->first)
				repeat=true;
		}
		if (!repeat)
			tmpRow.insert(make_pair(restIter->first, restIter->second));
	}
	tmpRow.swap(result);
}


void CQuantitativeCalculation::GetOtherPeakRow_ByResTime(double compResTime, MatchType &PeakRows) {
		
	if (PeakRows.empty()) return;
	//按最接近保留时间为其他峰
		if (PeakRows.size()>=2) {
			CIADataTable cPeakList;
			if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return;
	
			MatchType::iterator iter=PeakRows.begin();
	
			int minDiff_Row=iter->first;			
			double minDiff=static_cast<double>(INT_MAX);
	
			for ( ; iter!=PeakRows.end(); ++iter) {
				CString szResTime;
				int row=iter->first;
				if (!cPeakList.GetCellValue(row, _T("ReserveTime"), szResTime)) return;
				double dResTime=_tstof(szResTime);
				if (fabs(dResTime-compResTime)<minDiff) {
					minDiff = fabs(dResTime-compResTime);
					minDiff_Row = row;
				}
			}
			m_Result.insert(make_pair(minDiff_Row, PeakRows[minDiff_Row]));
		}
		else
			m_Result.insert(PeakRows.begin(), PeakRows.end());
}


int CQuantitativeCalculation::MatchRefReak() {
	
	//*取m_cCaliData中的参考峰
	CCaliData cRefPeak;
	m_cDuplicate.GetRefPeakData(cRefPeak);
	//*提取cPeakList表格，提取保留时间
	CIADataTable cPeakList;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;
	CStringArray caResTime;
	cPeakList.GetColumnValues(_T("ReserveTime"), caResTime);
	double dRefVal; //参考峰窗口值

	CString watch = pcData->GetXMLString();
	if (pcData->GetValue_Double(XMPPATH_SAMPLE_PARAM_CALI_REFERENCEPEAK, dRefVal) != IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;
	
	double dlower=1.0f-dRefVal/100.0f;//窗口开
	double dupper=1.0f+dRefVal/100.0f;//窗口闭
	//*参考峰保留时间
	for (int i=0; i<cRefPeak.GetItemSize(); ++i) { //对应每一个参考峰
		bool IsMatch = false;
		CString szRefPeak_ResTime;
		cRefPeak.GetReserveTime(i,szRefPeak_ResTime);
		double dRefPeak_ResTime=_tstof(szRefPeak_ResTime);
		double dRefPeak_lower=dRefPeak_ResTime*dlower;
		double dRefPeak_upper=dRefPeak_ResTime*dupper;

		MatchType RefPeakRows;
		for (long row=0; row<cPeakList.GetRowsNumber(); ++row) { //对应每一个PeakList内峰
			CString szPeakList_ResTime;
			if (!cPeakList.GetCellValue(row, _T("ReserveTime"), szPeakList_ResTime)) return IAERROR_XML_NOTVALID;
			double dPeakList_ResTime=_tstof(szPeakList_ResTime);
			if (dPeakList_ResTime>dRefPeak_lower && dPeakList_ResTime<dRefPeak_upper){ //PeakList落进参考峰 
				CaliItem cItem = cRefPeak.GetRowDate(i);
				RefPeakRows.insert(make_pair(row,cItem));
				IsMatch = true;
			}
		}
		//当前参考峰未能匹配上，取消参考峰标记
		if (!IsMatch)
			m_cDuplicate.CancelRefPeakMark(i);
			
		//按方法取面积或峰高为参考峰
		GetRef_IntPeakRow_ByArea_or_Height(RefPeakRows);
	}

	////找到所有参考峰后参照公式，修正校正表内其他峰的保留时间
	vector<double> afterResTime;
	typedef map<int, CaliItem>::iterator Iter;
	for (Iter iter=m_Result.begin(); iter!=m_Result.end(); ++iter)
	{
		CString afterRest;
		int row = iter->first;
		if (!cPeakList.GetCellValue(row, _T("ReserveTime"), afterRest)) return IAERROR_XML_NOTVALID;
		double rt = _tstof(afterRest);
		afterResTime.push_back(rt);
	}

	m_cDuplicate.ModifyResTime(afterResTime);

	return IAERROR_SUCCESS;
}


int CQuantitativeCalculation::MatchIntReak() {
	//提取标记内标峰
	CCaliData cIntPeak;
	m_cDuplicate.GetIntPeakData(cIntPeak); //需要从副本中获得

	//根据已修正的保留时间窗口，识别内标峰 若存在多个，按方法取面积或峰高选最大峰

	CIADataTable cPeakList;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;

	double dWndVal; //其他峰窗口值
	if (pcData->GetValue_Double(XMPPATH_SAMPLE_PARAM_CALI_OTHERCALIPEAK, dWndVal) != IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;
	double dlower=1.0f-dWndVal/100.0f;//窗口开
	double dupper=1.0f+dWndVal/100.0f;//窗口闭

	//*内标峰保留时间
	for (int i=0; i<cIntPeak.GetItemSize(); ++i) { //对应每一个内标峰
		bool IsMatch = false;
		
		CString szIntPeak_ResTime;
		cIntPeak.GetReserveTime(i, szIntPeak_ResTime);
		double dIntPeak_ResTime=_tstof(szIntPeak_ResTime);
		double dIntPeak_lower=dIntPeak_ResTime*dlower;
		double dIntPeak_upper=dIntPeak_ResTime*dupper;

		MatchType IntPeakRows;
		for (long row=0; row<cPeakList.GetRowsNumber(); ++row) { //对应每一个PeakList内峰
			CString szPeakList_ResTime;
			if (!cPeakList.GetCellValue(row, _T("ReserveTime"), szPeakList_ResTime)) return IAERROR_XML_NOTVALID;
			double dPeakList_ResTime=_tstof(szPeakList_ResTime);
			if (dPeakList_ResTime>dIntPeak_lower && dPeakList_ResTime<dIntPeak_upper){ //PeakList落进内标峰
				CaliItem cItem = cIntPeak.GetRowDate(i);
				IntPeakRows.insert(make_pair(row, cItem));
				IsMatch = true;
			}
		}
		
		//当前内标峰未能匹配上，取消内标峰标记
		if (!IsMatch)
			m_cDuplicate.CancelIntPeakMark(i);

		//去除重复的峰行号
		Remove_Repeat_PeakRow(IntPeakRows);

		//按方法取面积或峰高为内标峰
		GetRef_IntPeakRow_ByArea_or_Height(IntPeakRows);
	}
	return IAERROR_SUCCESS;
}


int CQuantitativeCalculation::MatchOtherReak(){
	//提取剩余峰
	CCaliData cOtherPeak;
	m_cDuplicate.GetcOtherPeakData(cOtherPeak); 

	//识别其他校正峰，若存在多个，取保留时间最接近的

	CIADataTable cPeakList;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;

	double dWndVal; //其他峰窗口值
	if (pcData->GetValue_Double(XMPPATH_SAMPLE_PARAM_CALI_OTHERCALIPEAK, dWndVal) != IAERROR_SUCCESS) return IAERROR_XML_NOTVALID;
	double dlower=1.0f-dWndVal/100.0f;//窗口开
	double dupper=1.0f+dWndVal/100.0f;//窗口闭

	//*其他峰保留时间
	for (int i=0; i<cOtherPeak.GetItemSize(); ++i) { //对应每一个其他峰
		CString szOtherPeak_ResTime;
		cOtherPeak.GetReserveTime(i, szOtherPeak_ResTime);
		double dOtherPeak_ResTime=_tstof(szOtherPeak_ResTime);
		double dOtherPeak_lower=dOtherPeak_ResTime*dlower;
		double dOtherPeak_upper=dOtherPeak_ResTime*dupper;

		MatchType OtherPeakRows;
		for (long row=0; row<cPeakList.GetRowsNumber(); ++row) { //对应每一个PeakList内峰
			CString szPeakList_ResTime;
			if (!cPeakList.GetCellValue(row, _T("ReserveTime"), szPeakList_ResTime)) return IAERROR_XML_NOTVALID;
			double dPeakList_ResTime=_tstof(szPeakList_ResTime);
			if (dPeakList_ResTime>dOtherPeak_lower && dPeakList_ResTime<dOtherPeak_upper){ //PeakList落进参考峰
				 CaliItem cItem = cOtherPeak.GetRowDate(i);
				 OtherPeakRows.insert(make_pair(row, cItem));
			}
		}

		//去除重复的峰行号
		Remove_Repeat_PeakRow(OtherPeakRows);

		//按最接近保留时间为其他峰
		GetOtherPeakRow_ByResTime(dOtherPeak_ResTime, OtherPeakRows);
	}

	return IAERROR_SUCCESS;
}





/************************************************************************************************************************************
/
/                                                   其他函数
/
/************************************************************************************************************************************/

void CQuantitativeCalculation::ClearTableRowData(CIADataTable *pTable) {
	if (pTable==NULL) ASSERT(TRUE);
	long row = pTable->GetRowsNumber();
	while (row!=0) {
		pTable->DeleteRow(0);
		row = pTable->GetRowsNumber();
	}
}


void CQuantitativeCalculation::PeakListData2ResultData(const CStringArray &PeakList, CStringArray &ResultData, int nID) {
	const int ResultDataRow = 23;
	const int PeakListRow = 23;
	if (PeakList.GetSize()!=PeakListRow) return;
	if (!ResultData.IsEmpty()) ResultData.RemoveAll();

	for (int i=0; i<ResultDataRow; ++i) {
		CString szVal(_T(""));
		ResultData.Add(szVal);
	}

	CString szID;
	szID.Format(_T("%d"), nID);
	ResultData[1]=szID;			//编号

	ResultData[0]=_T("{[(BITMAP_PREDEF:_Action_Info.bmp)]}");
	ResultData[2]=PeakList[2]; //保留时间
	ResultData[3]=PeakList[17]; //组份名
	ResultData[4]=PeakList[3]; //类型
	ResultData[5]=PeakList[4]; //峰面积
	ResultData[6]=PeakList[6]; //峰高
	ResultData[7]=PeakList[5]; //峰面积%
	ResultData[8]=PeakList[19]; //峰高%

	if (m_Method == _T("百分比法"))
	{
		if (m_Object==_T("峰面积"))
			ResultData[9]=PeakList[5]; //含量
		else if (m_Object==_T("峰高"))
			ResultData[9]=PeakList[19]; //含量
	}
	
	ResultData[10]=_T("{[(BITMAP_PREDEF:_Action_Delete.bmp)]}");

	ResultData[11]=PeakList[18]; //半峰宽
	ResultData[12]=PeakList[9]; //对称因子
	ResultData[13]=PeakList[21]; //畸变因子
	ResultData[14]=PeakList[8]; //塔板数
	ResultData[15]=PeakList[22]; //分离度
	ResultData[16]=PeakList[10]; //峰起点X
	ResultData[17]=PeakList[13]; //峰起点Y
	ResultData[18]=PeakList[14]; //峰顶点X
	ResultData[19]=PeakList[15]; //峰顶点Y
	ResultData[20]=PeakList[11]; //峰落点X
	ResultData[21]=PeakList[16]; //峰落点Y
	ResultData[22]=PeakList[7];  //峰宽
}

void CQuantitativeCalculation::GenerateResultData() {
	
	CIADataTable cPeakList;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return;

	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
	ClearTableRowData(&cResultTable);

	typedef MatchType::iterator Iter;
	int row=0;
	for (Iter iter=m_Result.begin(); iter!=m_Result.end(); ++iter) {
		row++;
		CStringArray caPeakListRowData, caResultRowData;
		int peakRows=iter->first;
		//组分名赋值
		CString szCompName=(iter->second).szComponentName;
		if(!cPeakList.SetCellValue(peakRows, _T("ComponentName"), szCompName)) continue;

		if(!cPeakList.GetRowValues(peakRows, caPeakListRowData)) continue;
		PeakListData2ResultData(caPeakListRowData, caResultRowData, row);
		cResultTable.AppendRow(caResultRowData);
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
	//CString szWatch = cResultTable.GetXMLString();
}


void CQuantitativeCalculation::DirectGenerateResultData() {

	CIADataTable cPeakList;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakList) != IAERROR_SUCCESS) return;

	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
	ClearTableRowData(&cResultTable);

	int rowSize = cPeakList.GetRowsNumber(); //m_cDuplicate.GetItemSize();
	vector<CString> vectCompName(rowSize);
	

	typedef MatchType::iterator Iter;
	int row=0;
	for (Iter iter=m_Result.begin(); iter!=m_Result.end(); ++iter) { //这里是否采用m_Result？ 
		row++;
		int peakRows=iter->first;
		CString szCompName=(iter->second).szComponentName;
		if (peakRows<=rowSize)
		{
			vectCompName[peakRows] = szCompName;
		}
	}

	
	for (int row=0; row<rowSize; ++row)
	{
		CStringArray caPeakListRowData, caResultRowData;
		if(!cPeakList.GetRowValues(row, caPeakListRowData)) continue;
		PeakListData2ResultData(caPeakListRowData, caResultRowData, row+1);
		caResultRowData[3] = vectCompName[row]; // 虽然很难看，但需求变化太快
		cResultTable.AppendRow(caResultRowData);
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
	//CString szWatch = cResultTable.GetXMLString();
}



/************************************************************************************************************************************
/
/                                                  定量计算 相关函数
/
/************************************************************************************************************************************/

CString CQuantitativeCalculation::GetCalibrateObjectAttr(){
	CString szAttr;
	if (m_Object==XMLVALUE_CONSTENT_c_peak_area)	 
		szAttr=_T("PeakArea");
	else	
		szAttr=_T("PeakHeight");
	return szAttr;
}


double CQuantitativeCalculation::GetColumnTotal(CIADataTable *pTable, const CString& strColumn) {

	CString szWatch=pTable->GetXMLString();
	double dResult=0.0f;
	CStringArray cItems;
	if (!pTable->GetColumnValues(strColumn, cItems))	return dResult;
	const int itemSize = cItems.GetSize();
	if (itemSize==0) return dResult;

	for (int i=0; i<itemSize; ++i) {
		CString szItem=cItems[i];
		double dItem=_tstof(szItem);
		dResult += dItem;
	}

	return dResult;
}


void CQuantitativeCalculation::QC_percentage(){
	
	//百分比法 : 每个峰的峰面积占所有峰的总面积的百分比
	CString szAttr=GetCalibrateObjectAttr();
	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

	double total=GetColumnTotal(&cResultTable, szAttr);
	const long rowSize=cResultTable.GetRowsNumber();
	for (long row=0; row<rowSize; row++) {
		CString szItem;
		if (!cResultTable.GetCellValue(row, szAttr, szItem)) return;
		double dItem=_tstof(szItem);
		double dContents = dItem/total*100.0f;
		CString szContents = ConvertScientificNotation(dContents);
		//szContents.Format(CONTENTS_DATA_FORMAT, dContents); // %0.4E
		if (!cResultTable.SetCellValue(row, _T("Concentration"), szContents)) return;
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
}


double CQuantitativeCalculation::GetProductFactor() {
	double dProductFactor; //乘积因子
	if (pcData->GetValue_Double(XMPPATH_SAMPLE_PARAM_CALI_PRODUCTFACTOR, dProductFactor) != IAERROR_SUCCESS) return 1.0f;
	return dProductFactor;
}


double CQuantitativeCalculation::GetDilutionFactor(){
	double dDilutionFactor; //稀释因子
	if (pcData->GetValue_Double(XMPPATH_SAMPLE_PARAM_CALI_DILUTIONFACTOR, dDilutionFactor) != IAERROR_SUCCESS) return 1.0f;
	return dDilutionFactor;
}


double CQuantitativeCalculation::GetSampleAmount(){
	double dSampleAmount; //含量值
	if (pcData->GetValue_Double(XMPPATH_SAMPLE_PARAM_CALI_SAMPLEAMOUNT, dSampleAmount) != IAERROR_SUCCESS) return 1.0f;
	return dSampleAmount;
}


void CQuantitativeCalculation::QC_external(){

	//假设组分X的校正方程为y=ax+b；
	//组分X的绝对含量=(峰面积(X)-b)/a*乘积因子*稀释因子
	CString szAttr=GetCalibrateObjectAttr();
	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

	double dProduct = GetProductFactor();  //乘积因子
	double dDilution= GetDilutionFactor(); //稀释因子

	const long rowSize=cResultTable.GetRowsNumber();
	if (rowSize!=m_Result.size()) ASSERT(TRUE);

	CString szItem;
	typedef MatchType::iterator Iter;
	Iter iter = m_Result.begin();
	for (long row=0; row<rowSize; row++) {
		CaliItem caliItem = iter->second;
		iter++;
		double a=_tstof(caliItem.szSlope);
		double b=_tstof(caliItem.szIntercept);

		if (!cResultTable.GetCellValue(row, szAttr, szItem)) return;
		double dItem=_tstof(szItem);

		CString szContents; 
		double dContents=0.0f;

		//如果校正表里a=0，那么相当于这个校正方程是无效的，即这个组分无法进行定量计算。
		//如果样品里有组分能匹配上这个组分，那么在结果表里显示该组分的含量为0
		if (a!=0.0f)
			dContents=(dItem-b)*dProduct*dDilution/a;
		szContents = ConvertScientificNotation(dContents);
		//szContents.Format(CONTENTS_DATA_FORMAT, dContents); // %0.4E
		if (!cResultTable.SetCellValue(row, _T("Concentration"), szContents)) return;
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
}


int CQuantitativeCalculation::FindMatchedCompoundGroupRow(const CString& cStrInteriorNo) {
	
	int findRow=-1;

	CIADataTable cCaliResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_PARAM_CALI_RESULT_TABLE, cCaliResultTable) != IAERROR_SUCCESS) return findRow;
	CStringArray ArrIsInterior, ArrInteriorNo;
	if(!cCaliResultTable.GetColumnValues(_T("IsInterior"), ArrIsInterior)) return findRow;
	if(!cCaliResultTable.GetColumnValues(_T("InteriorNo"), ArrInteriorNo)) return findRow;
	const int ArrIsInteriorSize=ArrIsInterior.GetSize();
	const int ArrInteriorNoSize=ArrInteriorNo.GetSize();
	if(ArrInteriorNoSize!=ArrIsInteriorSize) return findRow;
	
	for (int i=0; i<ArrInteriorNoSize; ++i) {
		if (cStrInteriorNo==ArrInteriorNo[i])
			if (_T("是")==ArrIsInterior[i]){
				findRow=i;
				break;
			}
	}
	return findRow;
}


void CQuantitativeCalculation::FindInteriorContentAndAttr(const CString& cStrInteriorNo, CaliItem& intItem) {

	int findRow = FindMatchedCompoundGroupRow(cStrInteriorNo);
	if (findRow==-1) return;

	CIADataTable cCaliResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_PARAM_CALI_RESULT_TABLE, cCaliResultTable) != IAERROR_SUCCESS) return ;
	CStringArray ArrIntItem;
	if(!cCaliResultTable.GetRowValues(findRow, ArrIntItem)) return;
	CaliItem caliItem(ArrIntItem, CALIBRATERESULTTABLE);
	intItem = caliItem;
}


void CQuantitativeCalculation::FindReMatchedPeakListItem(const CString& cStrInteriorNo, CaliItem& peakListItem){
	
	FindInteriorContentAndAttr(cStrInteriorNo, peakListItem);
	
	int findPeakListRow=-1;
	typedef MatchType::iterator Iter;
	for (Iter iter=m_Result.begin(); iter!=m_Result.end(); ++iter) {
		CaliItem caliItem = iter->second;
		if (peakListItem==caliItem){
			findPeakListRow=iter->first;
			break;
		}
	}

	if (findPeakListRow==-1) return;

	CIADataTable cPeakListTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cPeakListTable) != IAERROR_SUCCESS) return;

	CStringArray ArrPeakListItem;
	if(!cPeakListTable.GetRowValues(findPeakListRow, ArrPeakListItem)) return;
	CaliItem caliItem(ArrPeakListItem, PEAKLISTTABLE);
	peakListItem = caliItem;
}


void CQuantitativeCalculation::QC_internal(){
/*
	组分X的绝对含量=((峰面积(X))/(峰面积(内标))-b)/a×内标绝对含量×乘积因子×稀释因子
	注：
		1）组分X采用的内标为校正表中与X内标编号一致的内标物，内标绝对含量和峰面积(内标)为当前样品中的内标含量和峰面积；
		2）内标绝对含量为用户在样品信息模块中的内标信息框中输入的值；
		3）峰面积(内标)为与校正结果表中内标物匹配的组分的峰面积。
*/

	CString szAttr=GetCalibrateObjectAttr();
	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

	double dProduct=GetProductFactor();  //乘积因子
	double dDilution=GetDilutionFactor(); //稀释因子

	const long rowSize=cResultTable.GetRowsNumber();
	if (rowSize!=m_Result.size()) ASSERT(TRUE);
	
	CString szItem;
	typedef MatchType::iterator Iter;
	Iter iter = m_Result.begin();
	for (long row=0; row<rowSize; row++) {
		if (!cResultTable.GetCellValue(row, szAttr, szItem)) return;
		CaliItem caliItem = iter->second;
		iter++;

		//取出匹配的组份的内标编号，然后从*pcData的CalibrateTable内找出匹配的内标编号（级别1的），提取“内标绝对含量”和对应的“峰面积，峰高”
		CaliItem InterItem;
		CString cStrInteriorNo = caliItem.szInteriorNo;
		FindInteriorContentAndAttr(cStrInteriorNo, InterItem);

		if (caliItem.szIsInterior==_T("是"))
			if (!cResultTable.SetCellValue(row, _T("Concentration"), InterItem.szStdContent)) //直接匹配内标的绝对含量（用户输入量）
				return;


		if (caliItem.szIsInterior!=_T("否")) continue; //非内标才进行定量计算，内标绝对含量已经由用户自行输  


	    //要匹配峰匹配到的校正数据，按内标编号再匹配到的内标组份，再内标组份匹配回PeakList内的记录
		CaliItem PeakListItem;
		FindReMatchedPeakListItem(cStrInteriorNo, PeakListItem);
		CString szValue_in;
		if (szAttr==_T("PeakArea"))
			szValue_in=PeakListItem.szPeakArea;
		else
			szValue_in=PeakListItem.szPeakHeight;

		double dValue=_tstof(szItem);
		double dValue_in=_tstof(szValue_in);
		double a=_tstof(caliItem.szSlope);
		double b=_tstof(caliItem.szIntercept);
		double dStdContent_in=_tstof(InterItem.szStdContent); //内标绝对含量

		if (dValue_in==0.0f)
		{
			//MessageBox(_T(this, "内标峰面积或峰高为0。定量无法继续进行，需要检查数据"), MB_OK|MB_ICONERROR);
			ASSERT(TRUE);
		}
		
		CString szContents;
		double dContents=0.0f;
		if (a!=0.0f && dValue_in!=0.0f)
			dContents=((dValue/dValue_in)-b)*dStdContent_in*dProduct*dDilution/a;

		szContents = ConvertScientificNotation(dContents);
		//szContents.Format(CONTENTS_DATA_FORMAT, dContents); //0.4E
		if (!cResultTable.SetCellValue(row, _T("Concentration"), szContents)) return;
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

}


void CQuantitativeCalculation::QC_external_percentage(){
/*
		组分X的相对含量=组分X的绝对含量/样品量
		注：
		1）组分X的绝对含量由外标法计算得出；
		2）样品量来自“校正设置”中的含量值。
*/
	double dSampleAmount=GetSampleAmount();
	if (dSampleAmount==0.0) {
		//MessageBox(_T("来自“校正设置”中的含量值为0。定量无法继续进行"));
		return;
	}
	
	QC_external();

	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

	const long rowSize=cResultTable.GetRowsNumber();
	for (long row=0; row<rowSize; row++) {
		CString szValue;
		if (!cResultTable.GetCellValue(row, _T("Concentration"), szValue)) return;
		double dValue=_tstof(szValue);
		double dContents = dValue/dSampleAmount*100.0f; //组分X的相对含量=组分X的绝对含量/样品量
		CString szContents = ConvertScientificNotation(dContents);
		//szContents.Format(CONTENTS_DATA_FORMAT, dContents); //0.4E
		if (!cResultTable.SetCellValue(row, _T("Concentration"), szContents)) return;
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
}


void CQuantitativeCalculation::QC_internal_percentage(){
	/*
		组分X的相对含量=组分X的绝对含量/样品量
		注：
		1）组分X的绝对含量由外标法计算得出；
		2）样品量来自“校正设置”中的含量值。
*/
	double dSampleAmount=GetSampleAmount();

	if (dSampleAmount==0.0) {
		//MessageBox(_T("来自“校正设置”中的含量值为0。定量无法继续进行"));
		return;
	}

	QC_internal();

	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

	const long rowSize=cResultTable.GetRowsNumber();
	for (long row=0; row<rowSize; row++) {
		CString szValue;
		if (!cResultTable.GetCellValue(row, _T("Concentration"), szValue)) return;
		double dValue=_tstof(szValue);
		double dContents = dValue/dSampleAmount*100.0f; //组分X的相对含量=组分X的绝对含量/样品量
		CString szContents = ConvertScientificNotation(dContents);
		//szContents.Format(CONTENTS_DATA_FORMAT, dContents); //0.4E
		if (!cResultTable.SetCellValue(row, _T("Concentration"), szContents)) return;
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
}


void CQuantitativeCalculation::QC_normalization(){
/*
		组分X的百分含量=组分X的绝对含量/(∑各组分的绝对含量)
		注：
		1）组分X的绝对含量由外标法计算得出；
		2）所有组分的绝对含量都要由外标法计算得出
*/
	QC_external();

	CIADataTable cResultTable;
	if (pcData->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;

	CString strColumn(_T("Concentration"));
	double dContentTotal = GetColumnTotal(&cResultTable, strColumn); //∑各组分的绝对含量

	const long rowSize=cResultTable.GetRowsNumber();
	for (long row=0; row<rowSize; row++) {
		CString szValue;
		if (!cResultTable.GetCellValue(row, strColumn, szValue)) return;
		double dValue=_tstof(szValue);
		double dContents = dValue/dContentTotal*100.0f;  //组分X的百分含量=组分X的绝对含量/(∑各组分的绝对含量)
		CString szContents = ConvertScientificNotation(dContents);
		//szContents.Format(CONTENTS_DATA_FORMAT, dContents); // %0.4E
		if (!cResultTable.SetCellValue(row, strColumn, szContents)) return;
	}

	if (pcData->SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cResultTable) != IAERROR_SUCCESS) return;
}

CString CQuantitativeCalculation::ConvertScientificNotation(double contents) {
/*
	1. 计算数值＜0.1时，才采用科学计数法表达。
	2. e后面的数值有几位显示几位，比如，表达为e-2，而非e-002。
	3. 零值直接表达为0.000。
*/
	CString retVal(_T(""));
	
	if (contents == 0.0000f)
	{
		retVal = _T("0.000");
	}
	else if (contents < 0.1f)
	{
		retVal.Format(CONTENTS_DATA_FORMAT, contents); // %0.4E

		int strLen = retVal.GetLength();
		int pos = retVal.Find(_T("E-00"));
		if (pos!=-1)
		{
			retVal.Delete(pos+2, 2);
			return retVal;
		}
		pos = retVal.Find(_T("E-0"));
		if (pos!=-1)
		{
			retVal.Delete(pos+2);
			return retVal;
		}

		return retVal;
	}
	else //大于0.1 不采用科学计数
	{
		retVal.Format(_T("%.3f"), contents); // %0.4E
		return retVal;
	}
	return retVal;
}

int CQuantitativeCalculation::QuantitativeCalculation() {
	
	if (m_Method==XMLVALUE_CONST_c_percentage)
		QC_percentage(); //定量：百分比法
	else if (m_Method==XMLVALUE_CONST_c_external)
		QC_external();   //定量：外标法
	else if (m_Method==XMLVALUE_CONST_c_internal)
		QC_internal(); 	 //定量：内标法
	else if (m_Method==XMLVALUE_CONST_c_external_percentage)
		QC_external_percentage(); //定量：外标百分比法
	else if (m_Method==XMLVALUE_CONST_c_internal_percentage)
		QC_internal_percentage(); //定量：内标百分比法
	else if (m_Method==XMLVALUE_CONST_c_normalization)
		QC_normalization();       //定量：外标归一化法
	else 
		return IAERROR_XML_NOTVALID;

	return IAERROR_SUCCESS;
}











