#include "stdafx.h"
#include "Base_AnalysisSample.h"
#include "PeakSeak.h"
#include "QuantitativeCalculation.h"
#include "C2Bitmap.h"

 

//获得峰类型的文字描述
CString CPeakStandard::CPeakType::ConvertToTypeName(void)
{
	CString strName(_T(""));
	if (negative==1) strName += _T("负峰/");
	if (single == 1) 
	{
		strName += _T("单峰/");
		goto CPeakStandard_CPeakType_Lable;
	}
	if (overlap_pl == 1) 
	{
		strName += _T("重叠峰/");
		goto CPeakStandard_CPeakType_Lable;
	}
	if (overlap_ls == 1) 
	{
		strName += _T("重叠峰(左肩)/");
		goto CPeakStandard_CPeakType_Lable;
	}
	if (overlap_rs == 1) 
	{
		strName += _T("重叠峰(右肩)/");
		goto CPeakStandard_CPeakType_Lable;
	}
	if (overlap_vs == 1) 
	{
		strName += _T("重叠峰(垂直分峰)/");
		goto CPeakStandard_CPeakType_Lable;
	}

CPeakStandard_CPeakType_Lable:
	if (tail == 1) 
	{
		strName += _T("拖尾");
	}

	if (strName.IsEmpty()) //不可能出现的情况
	{
		CString str(_T(""));
		return str;
	}

	//如果没有拖尾。则出现“单峰/重叠峰(垂直分峰)/”时，把最后一个/删除
	if (strName[strName.GetLength()-1]=='/') 
	{
		strName.Delete(strName.GetLength()-1);
	}

	return strName;
}

void CPeakStandard::CPeakType::GetFromTypeName(CString cStrName)
{
	cStrName += _T("/");
	//解析字符串

	while (!cStrName.IsEmpty())
	{
		int iMark = cStrName.Find(_T("/"));
		CString szToken = cStrName.Left(iMark);
		if (szToken==_T("负峰")) negative=1;
		if (szToken==_T("单峰")) single=1;
		if (szToken==_T("重叠峰")) overlap_pl=1;
		if (szToken==_T("重叠峰(左肩)")) overlap_ls=1;
		if (szToken==_T("重叠峰(右肩)")) overlap_rs=1;
		if (szToken==_T("重叠峰(垂直分峰)")) overlap_vs=1;
		if (szToken==_T("拖尾")) tail=1;
		cStrName = cStrName.Right(cStrName.GetLength() - iMark -1);
	}
}

BOOL CPeakList::SynchronizeToXML(void)
{
	//直接从AnalysisData获得已经有Topic的表格

	CIADataTable cIADataTable;
	if (m_pcDataXML->GetValue_Table(m_cStrPathName, cIADataTable)!=IAERROR_SUCCESS) return FALSE;
	CString szWatch = cIADataTable.GetXMLString();

	//获得Topic的下标，便于赋值

	const long topicNum = cIADataTable.GetColumnNumber();

	const long iID				= cIADataTable.GetColumnIndex(_T("ID"));
	const long iPeakType		= cIADataTable.GetColumnIndex(_T("PeakType")); 
	const long iPeakBeginX	    = cIADataTable.GetColumnIndex(_T("PeakBeginX")); 
	const long iPeakBeginY		= cIADataTable.GetColumnIndex(_T("PeakBeginY")); 
	const long iPeakTOPX		= cIADataTable.GetColumnIndex(_T("PeakTopX")); 
	const long iPeakTOPY	    = cIADataTable.GetColumnIndex(_T("PeakTopY")); 
	const long iPeakEndX		= cIADataTable.GetColumnIndex(_T("PeakEndX")); 
	const long iPeakEndY		= cIADataTable.GetColumnIndex(_T("PeakEndY"));
	const long iReserveTime	    = cIADataTable.GetColumnIndex(_T("ReserveTime"));
	const long iHalfPeakWidth	= cIADataTable.GetColumnIndex(_T("HalfPeakWidth"));
	const long iPeakWidth		= cIADataTable.GetColumnIndex(_T("PeakWidth"));
	const long iPeakHeight	    = cIADataTable.GetColumnIndex(_T("PeakHeight"));
	const long iPeakHighPercent = cIADataTable.GetColumnIndex(_T("PeakHighPercent"));
	const long iPeakArea	    = cIADataTable.GetColumnIndex(_T("PeakArea"));
	const long iPeakAreaPercent = cIADataTable.GetColumnIndex(_T("PeakAreaPercent"));
	const long iDisfactor	    = cIADataTable.GetColumnIndex(_T("Disfactor"));
	const long iSymmetry	    = cIADataTable.GetColumnIndex(_T("Symmetry"));
	const long iResolution		= cIADataTable.GetColumnIndex(_T("Resolution"));
	const long iColumnPlate		= cIADataTable.GetColumnIndex(_T("ColumnPlate"));
	const long iConcentration	= cIADataTable.GetColumnIndex(_T("Concentration"));
	const long iComponentName	= cIADataTable.GetColumnIndex(_T("ComponentName"));
	
	// 检查数组下标越界

	if (  (iID==-1				|| iID>=topicNum)
		||(iPeakType==-1		|| iPeakType>=topicNum)
		||(iPeakBeginX==-1		|| iPeakBeginX>=topicNum)
		||(iPeakBeginY==-1		|| iPeakBeginY>=topicNum)
		||(iPeakTOPX==-1		|| iPeakTOPX>=topicNum)
		||(iPeakTOPY==-1		|| iPeakTOPY>=topicNum)
		||(iPeakEndX==-1		|| iPeakEndX>=topicNum)
		||(iPeakEndY==-1		|| iPeakEndY>=topicNum)
		||(iReserveTime==-1	    || iReserveTime>=topicNum)
		||(iHalfPeakWidth==-1   || iHalfPeakWidth>=topicNum)
		||(iPeakWidth==-1		|| iPeakWidth>=topicNum)
		||(iPeakHeight==-1		|| iPeakHeight>=topicNum)
		||(iPeakHighPercent==-1 || iPeakHighPercent>=topicNum)
		||(iPeakArea==-1		|| iPeakArea>=topicNum)
		||(iPeakAreaPercent==-1 || iPeakAreaPercent>=topicNum)
		||(iDisfactor==-1		|| iDisfactor>=topicNum)
		||(iSymmetry==-1		|| iSymmetry>=topicNum)
		||(iResolution==-1		|| iResolution>=topicNum)
		||(iColumnPlate==-1		|| iColumnPlate>=topicNum)
		||(iConcentration==-1		|| iConcentration>=topicNum)
		||(iComponentName==-1	|| iComponentName>=topicNum)
		) return FALSE;


	
	//清空RESULT_PEAKLIST中的所有行数据，但保留Topic部分
	if (!cIADataTable.DeleteAllRows()) return FALSE;

	//初始化行数据
	CStringArray ArrValues; //表格内一行数据


	for (long i=0; i<topicNum; ++i)
		ArrValues.Add(_T("0"));

	//积分结果赋值

	int ArrayDataSize = m_cArrayData.GetSize();
	for (int i = 0; i<ArrayDataSize; ++i)
	{
		//先判定类型，再确定格式，最后赋值

		CString strType, strFormat;

		//编号
		CString szID;
		szID.Format(_T("%d"), i+1);
		ArrValues[iID] = szID;

		//保留时间
		if(!cIADataTable.GetColumnInfo(iReserveTime, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iReserveTime, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iReserveTime].Format(strFormat, m_cArrayData[i].m_dataTop.dX);
		}
		else
			ArrValues[iReserveTime]= _T("#ERROR#");

		//峰类型

		if(!cIADataTable.GetColumnInfo(iPeakType, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("string"))
			ArrValues[iPeakType] = m_cArrayData[i].m_cPeakType.ConvertToTypeName();
		else
			ArrValues[iPeakType] = _T("#ERROR#");

		//峰面积

		if(!cIADataTable.GetColumnInfo(iPeakArea, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakArea, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakArea].Format(strFormat, m_cArrayData[i].m_dArea);
		}
		else
			ArrValues[iPeakArea]= _T("#ERROR#");

		//峰面积%
		if(!cIADataTable.GetColumnInfo(iPeakAreaPercent, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakAreaPercent, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakAreaPercent].Format(strFormat, m_cArrayData[i].m_dAreaPer);
		}
		else
			ArrValues[iPeakAreaPercent]= _T("#ERROR#");

		//峰高
		if(!cIADataTable.GetColumnInfo(iPeakHeight, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakHeight, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakHeight].Format(strFormat, m_cArrayData[i].m_dHeight);
		}
		else
			ArrValues[iPeakHeight]= _T("#ERROR#");


		//峰宽
		if(!cIADataTable.GetColumnInfo(iPeakWidth, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakWidth, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakWidth].Format(strFormat, m_cArrayData[i].m_dWidth);
		}
		else
			ArrValues[iPeakWidth]= _T("#ERROR#");


		//塔板数

		if(!cIADataTable.GetColumnInfo(iColumnPlate, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iColumnPlate, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iColumnPlate].Format(strFormat, m_cArrayData[i].m_dColumnPlate);
		}
		else
			ArrValues[iColumnPlate]= _T("#ERROR#");

		//对称因子
		if(!cIADataTable.GetColumnInfo(iSymmetry, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iSymmetry, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iSymmetry].Format(strFormat, m_cArrayData[i].m_dSymmetry);
		}
		else
			ArrValues[iSymmetry]= _T("#ERROR#");


		//峰起点X -- 开始时间

		if(!cIADataTable.GetColumnInfo(iPeakBeginX, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakBeginX, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakBeginX].Format(strFormat, m_cArrayData[i].m_dataBegin.dX);
		}
		else
			ArrValues[iPeakBeginX] = _T("#ERROR#");

		//峰落点X -- 结束时间
		if(!cIADataTable.GetColumnInfo(iPeakEndX, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakEndX, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakEndX].Format(strFormat, m_cArrayData[i].m_dataEnd.dX);
		}
		else
			ArrValues[iPeakEndX]= _T("#ERROR#");
			


		//峰起点Y
		if(!cIADataTable.GetColumnInfo(iPeakBeginY, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakBeginY, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakBeginY].Format(strFormat, m_cArrayData[i].m_dataBegin.dY);
		}
		else
			ArrValues[iPeakBeginY]= _T("#ERROR#");
		
		//峰顶点X
		if(!cIADataTable.GetColumnInfo(iPeakTOPX, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakTOPX, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakTOPX].Format(strFormat, m_cArrayData[i].m_dataTop.dX);
		}
		else
			ArrValues[iPeakTOPX]= _T("#ERROR#");

		//峰顶点Y
		if(!cIADataTable.GetColumnInfo(iPeakTOPY, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakTOPY, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakTOPY].Format(strFormat, m_cArrayData[i].m_dataTop.dY);
		}
		else
			ArrValues[iPeakTOPY]= _T("#ERROR#");

		//峰落点Y
		if(!cIADataTable.GetColumnInfo(iPeakEndY, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakEndY, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakEndY].Format(strFormat, m_cArrayData[i].m_dataEnd.dY);
		}
		else
			ArrValues[iPeakEndY]= _T("#ERROR#");

		//半峰宽

		if(!cIADataTable.GetColumnInfo(iHalfPeakWidth, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iHalfPeakWidth, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iHalfPeakWidth].Format(strFormat, m_cArrayData[i].m_dHalfWidth);
		}
		else
			ArrValues[iHalfPeakWidth]= _T("#ERROR#");

		

		

		//峰高%
		if(!cIADataTable.GetColumnInfo(iPeakHighPercent, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iPeakHighPercent, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iPeakHighPercent].Format(strFormat, m_cArrayData[i].m_dHeightPer);
		}
		else
			ArrValues[iPeakHighPercent]= _T("#ERROR#");



		//畸变因子
		if(!cIADataTable.GetColumnInfo(iDisfactor, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iDisfactor, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iDisfactor].Format(strFormat, m_cArrayData[i].m_dDisfactor);
		}
		else
			ArrValues[iDisfactor]= _T("#ERROR#");

		

		//分离度

		if(!cIADataTable.GetColumnInfo(iResolution, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iResolution, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iResolution].Format(strFormat, m_cArrayData[i].m_dResolution);
		}
		else
			ArrValues[iResolution]= _T("#ERROR#");

	

		//分析结果
		if(!cIADataTable.GetColumnInfo(iConcentration, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("float"))
		{
			if(!cIADataTable.GetColumnInfo(iConcentration, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iConcentration].Format(strFormat, m_cArrayData[i].m_dResultConcentration);
		}
		else
			ArrValues[iConcentration]= _T("#ERROR#");

		//组份名称
		if(!cIADataTable.GetColumnInfo(iComponentName, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType == _T("string"))
		{
			if(!cIADataTable.GetColumnInfo(iColumnPlate, XMLPROP_FORMAT, strFormat)) return FALSE;
			ArrValues[iComponentName] = m_cArrayData[i].m_cStrComponentName;
		}
		else
			ArrValues[iComponentName]= _T("#ERROR#");
		
		long iLine = cIADataTable.AppendRow();
		if(!cIADataTable.SetRowValues(iLine, ArrValues)) return FALSE;
	}
	//CString Observer = cIADataTable.GetXMLString();

	//把cIADataTable数据更新到CAnalysisData *m_pcDataXML;
	if(m_pcDataXML->SetValue_Table(m_cStrPathName, cIADataTable)!=IAERROR_SUCCESS) return FALSE;
	szWatch = m_pcDataXML->GetXMLString();
	return TRUE;
}

BOOL CPeakList::SynchronizeFromXML(void)
{
	//把CAnalysisData数据导入pIADataTable
	CIADataTable cIADataTable;
	if(m_pcDataXML->GetValue_Table(m_cStrPathName, cIADataTable)!=IAERROR_SUCCESS) return FALSE;
	m_cArrayData.RemoveAll();

	int res = m_cStrPathName.Find(_T("ResultTable"));
	
	if (res!=-1) 
		return SyncFromXML_ResultTable(&cIADataTable);
	else		 
		return SyncFromXML_PeakListTable(&cIADataTable);
}


BOOL CPeakList::SyncFromXML_PeakListTable(void)
{
	CIADataTable cIADataTable;
	if(m_pcDataXML->GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, cIADataTable)!=IAERROR_SUCCESS) 
		return FALSE;
	m_cArrayData.RemoveAll();
	CString xxx = cIADataTable.GetXMLString();
	return SyncFromXML_PeakListTable(&cIADataTable);
}


BOOL CPeakList::SyncFromXML_ResultTable(void)
{
	CIADataTable cIADataTable;
	if(m_pcDataXML->GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, cIADataTable)!=IAERROR_SUCCESS) 
		return FALSE;
	m_cArrayData.RemoveAll();
	CString xxx = cIADataTable.GetXMLString();
	return SyncFromXML_ResultTable(&cIADataTable);
}


BOOL CPeakList::SyncFromXML_PeakListTable(CIADataTable* pCIADataTable){
	//pIADataTable数据导入成员变量m_cArrayData;
	const long dataNum = pCIADataTable->GetRowsNumber();
	CString xxx = pCIADataTable->GetXMLString();
	for (long i=0; i<dataNum; ++i)
	{
		//获取各个单元格内的数据

		CString strType;

		CString szPeakType; //峰类型CPeakType	
		const long PeakTypeIdex = pCIADataTable->GetColumnIndex(_T("PeakType"));
		if(!pCIADataTable->GetColumnInfo(PeakTypeIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("string")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakTypeIdex, szPeakType)){ASSERT(false); return FALSE;}

		CString szPeakBeginX; //峰起点X
		const long PeakBeginXIdex = pCIADataTable->GetColumnIndex(_T("PeakBeginX"));
		if(!pCIADataTable->GetColumnInfo(PeakBeginXIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakBeginXIdex, szPeakBeginX)) {ASSERT(false); return FALSE;}

		CString szPeakBeginY; //峰起点Y
		const long PeakBeginYIdex = pCIADataTable->GetColumnIndex(_T("PeakBeginY"));
		if(!pCIADataTable->GetColumnInfo(PeakBeginXIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakBeginYIdex, szPeakBeginY)) {ASSERT(false); return FALSE;}

		CString szPeakTOPX; //峰顶点X 
		const long PeakTOPXIdex = pCIADataTable->GetColumnIndex(_T("PeakTopX"));
		if(!pCIADataTable->GetColumnInfo(PeakTOPXIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakTOPXIdex, szPeakTOPX)) {ASSERT(false); return FALSE;}

		CString szPeakTOPY; //峰顶点Y
		const long PeakTOPYIdex = pCIADataTable->GetColumnIndex(_T("PeakTopY"));
		if(!pCIADataTable->GetColumnInfo(PeakTOPYIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakTOPYIdex, szPeakTOPY)) {ASSERT(false); return FALSE;}

		CString szPeakEndX; //峰落点X
		const long PeakEndXIdex = pCIADataTable->GetColumnIndex(_T("PeakEndX"));
		if(!pCIADataTable->GetColumnInfo(PeakEndXIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakEndXIdex, szPeakEndX)) {ASSERT(false); return FALSE;}

		CString szPeakEndY; //峰落点Y
		const long PeakEndYIdex = pCIADataTable->GetColumnIndex(_T("PeakEndY"));
		if(!pCIADataTable->GetColumnInfo(PeakEndYIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakEndYIdex, szPeakEndY)) {ASSERT(false); return FALSE;}

		CString szReserveTime; //保留时间
		const long ReserveTimeIdex = pCIADataTable->GetColumnIndex(_T("ReserveTime"));
		if(!pCIADataTable->GetColumnInfo(ReserveTimeIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, ReserveTimeIdex, szReserveTime)) {ASSERT(false); return FALSE;}

		CString szHalfPeakWidth; //半峰宽

		const long HalfPeakWidthIdex = pCIADataTable->GetColumnIndex(_T("HalfPeakWidth"));
		if(!pCIADataTable->GetColumnInfo(HalfPeakWidthIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, HalfPeakWidthIdex, szHalfPeakWidth)) {ASSERT(false); return FALSE;}

		CString szPeakWidth; //峰宽
		const long PeakWidthIdex = pCIADataTable->GetColumnIndex(_T("PeakWidth"));
		if(!pCIADataTable->GetColumnInfo(PeakWidthIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakWidthIdex, szPeakWidth)) {ASSERT(false); return FALSE;}

		CString szPeakHeight; //峰高
		const long PeakHeightIdex = pCIADataTable->GetColumnIndex(_T("PeakHeight"));
		if(!pCIADataTable->GetColumnInfo(PeakHeightIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakHeightIdex, szPeakHeight)) {ASSERT(false); return FALSE;}

		CString szPeakHighPercent; //峰高%
		const long PeakHighPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakHighPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakHighPercentIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakHighPercentIdex, szPeakHighPercent)) {ASSERT(false); return FALSE;}

		CString szPeakArea; //峰面积

		const long PeakAreaIdex = pCIADataTable->GetColumnIndex(_T("PeakArea"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakAreaIdex, szPeakArea)) {ASSERT(false); return FALSE;}

		CString szPeakAreaPercent; //峰面积%
		const long PeakAreaPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakAreaPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaPercentIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakAreaPercentIdex, szPeakAreaPercent)) {ASSERT(false); return FALSE;}

		CString szDisfactor; //畸变因子
		const long DisfactorIdex = pCIADataTable->GetColumnIndex(_T("Disfactor"));
		if(!pCIADataTable->GetColumnInfo(DisfactorIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, DisfactorIdex, szDisfactor)) {ASSERT(false); return FALSE;}

		CString szSymmetry; //对称因子
		const long SymmetryIdex = pCIADataTable->GetColumnIndex(_T("Symmetry"));
		if(!pCIADataTable->GetColumnInfo(SymmetryIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, SymmetryIdex, szSymmetry)) {ASSERT(false); return FALSE;}

		CString szResolution; //分离度

		const long ResolutionIdex = pCIADataTable->GetColumnIndex(_T("Resolution"));
		if(!pCIADataTable->GetColumnInfo(ResolutionIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;} 
		if(!pCIADataTable->GetCellValue(i, ResolutionIdex, szResolution)) {ASSERT(false); return FALSE;}

		CString szColumnPlate; //塔板数

		const long ColumnPlateIdex = pCIADataTable->GetColumnIndex(_T("ColumnPlate"));
		if(!pCIADataTable->GetColumnInfo(ColumnPlateIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;} 
		if(!pCIADataTable->GetCellValue(i, ColumnPlateIdex, szColumnPlate)) {ASSERT(false); return FALSE;}

		CString szConcentration; //浓度
		const long ConcentrationIdex = pCIADataTable->GetColumnIndex(_T("Concentration"));
		if(!pCIADataTable->GetColumnInfo(ConcentrationIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("float")) {ASSERT(false); return FALSE;} 
		if(!pCIADataTable->GetCellValue(i, ConcentrationIdex, szConcentration)) {ASSERT(false); return FALSE;}

		CString szComponentName; //组份名称
		const long ComponentNameIdex = pCIADataTable->GetColumnIndex(_T("ComponentName"));
		if(!pCIADataTable->GetColumnInfo(ComponentNameIdex, XMLPROP_VALUETYPE, strType)) {ASSERT(false); return FALSE;}
		if (strType != _T("string")) {ASSERT(false); return FALSE;} 
		if(!pCIADataTable->GetCellValue(i, ComponentNameIdex, szComponentName)) {ASSERT(false); return FALSE;}
		//ASSERT(!szComponentName.IsEmpty());

		// 字符串转变成数值型
		const double dPeakBeginX	 =_tstof(szPeakBeginX);
		const double dPeakBeginY	 =_tstof(szPeakBeginY);

		const double dPeakEndX		 =_tstof(szPeakEndX);
		const double dPeakEndY		 =_tstof(szPeakEndY);
		
		const double dPeakTOPX		 =_tstof(szPeakTOPX);
		const double dPeakTOPY	     =_tstof(szPeakTOPY);
		const double dReserveTime	 =_tstof(szReserveTime);
		const double dHalfPeakWidth  =_tstof(szHalfPeakWidth);
		const double dPeakWidth		 =_tstof(szPeakWidth);
		const double dPeakHeight	 =_tstof(szPeakHeight);
		const double dPeakHighPercent=_tstof(szPeakHighPercent);
		const double dPeakArea	     =_tstof(szPeakArea);
		const double dPeakAreaPercent=_tstof(szPeakAreaPercent);
		const double dConcentration	 =_tstof(szConcentration);
		const double dDisfactor	     =_tstof(szDisfactor);
		const double dSymmetry	     =_tstof(szSymmetry);
		const double dResolution	 =_tstof(szResolution);
		const double dColumnPlate	 =_tstof(szColumnPlate);

		//每一行数据创建一个新的CPeakStandard对象数据
		CPeakStandard aPeakStard;
		CPeakStandard::CPeakType aPeakType;
		aPeakType.GetFromTypeName(szPeakType);
		aPeakStard.m_cPeakType = aPeakType;//峰类型

		aPeakStard.m_dReserveTime = dReserveTime;//保留时间

		CCurveDataSingle startCurveData(dPeakBeginX, dPeakBeginY);
		aPeakStard.m_dataBegin = startCurveData;//峰起点

		CCurveDataSingle endCurveData(dPeakEndX, dPeakEndY);
		aPeakStard.m_dataEnd = endCurveData;//峰落点

		CCurveDataSingle topCurveData(dPeakTOPX, dPeakTOPY);
		aPeakStard.m_dataTop = topCurveData;//峰顶（峰，谷）

		aPeakStard.m_dHalfWidth = dHalfPeakWidth;	//半峰宽

		aPeakStard.m_dWidth = dPeakWidth;		//峰宽
		aPeakStard.m_dHeight = dPeakHeight;		//峰高
		aPeakStard.m_dHeightPer = dPeakHighPercent;	//峰高%
		aPeakStard.m_dArea = dPeakArea;		//峰面积

		aPeakStard.m_dAreaPer = dPeakAreaPercent;		//峰面积%
		aPeakStard.m_dResultConcentration = dConcentration;	//分析结果（百分比%）在分析结果表中使用，峰列表中不用显示

		aPeakStard.m_dDisfactor = dDisfactor;   //畸变因子
		aPeakStard.m_dSymmetry = dSymmetry;    //对称因子
		aPeakStard.m_dResolution = dResolution;  //分离度

		aPeakStard.m_dColumnPlate = dColumnPlate; //塔板数

		aPeakStard.m_cStrComponentName = szComponentName;//组份名称

		m_cArrayData.Add(aPeakStard);
	}

	if(m_cArrayData.GetSize() != (int)dataNum) {ASSERT(false); return FALSE;}

	return TRUE;
}

BOOL CPeakList::SyncFromXML_ResultTable(CIADataTable* pCIADataTable){
	//pIADataTable数据导入成员变量m_cArrayData;
	const long dataNum = pCIADataTable->GetRowsNumber();
	for (long i=0; i<dataNum; ++i)
	{
		CString strType;

		CString szPeakType; //峰类型CPeakType	
		const long PeakTypeIdex = pCIADataTable->GetColumnIndex(_T("PeakType"));
		if(!pCIADataTable->GetColumnInfo(PeakTypeIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("string")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakTypeIdex, szPeakType)){ASSERT(false); return FALSE;} 

		CString szReserveTime; //保留时间
		const long ReserveTimeIdex = pCIADataTable->GetColumnIndex(_T("ReserveTime"));
		if(!pCIADataTable->GetColumnInfo(ReserveTimeIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("float")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, ReserveTimeIdex, szReserveTime)){ASSERT(false); return FALSE;}

		CString szComponentName; //组份名
		const long ComponentNameIdex = pCIADataTable->GetColumnIndex(_T("ComponentName"));
		if(!pCIADataTable->GetColumnInfo(ComponentNameIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("string")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, ComponentNameIdex, szComponentName)){ASSERT(false); return FALSE;}

		/*CString szComponentName; //组份名称
		const long ComponentNameIdex = pCIADataTable->GetColumnIndex(_T("ComponentName"));
		if(!pCIADataTable->GetColumnInfo(ComponentNameIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("string")){ASSERT(false); return FALSE;} 
		if(!pCIADataTable->GetCellValue(i, ComponentNameIdex, szComponentName)){ASSERT(false); return FALSE;}*/

		CString szPeakHeight; //峰高
		const long PeakHeightIdex = pCIADataTable->GetColumnIndex(_T("PeakHeight"));
		if(!pCIADataTable->GetColumnInfo(PeakHeightIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("float")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakHeightIdex, szPeakHeight)){ASSERT(false); return FALSE;}

		CString szPeakHighPercent; //峰高%
		const long PeakHighPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakHighPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakHighPercentIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("float")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakHighPercentIdex, szPeakHighPercent)){ASSERT(false); return FALSE;}

		CString szPeakArea; //峰面积

		const long PeakAreaIdex = pCIADataTable->GetColumnIndex(_T("PeakArea"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("float")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakAreaIdex, szPeakArea)){ASSERT(false); return FALSE;}

		CString szPeakAreaPercent; //峰面积%
		const long PeakAreaPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakAreaPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaPercentIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("float")){ASSERT(false); return FALSE;}
		if(!pCIADataTable->GetCellValue(i, PeakAreaPercentIdex, szPeakAreaPercent)){ASSERT(false); return FALSE;}

		CString szConcentration; //浓度
		const long ConcentrationIdex = pCIADataTable->GetColumnIndex(_T("Concentration"));
		if(!pCIADataTable->GetColumnInfo(ConcentrationIdex, XMLPROP_VALUETYPE, strType)){ASSERT(false); return FALSE;}
		if (strType != _T("float")){ASSERT(false); return FALSE;} 
		if(!pCIADataTable->GetCellValue(i, ConcentrationIdex, szConcentration)){ASSERT(false); return FALSE;}


		// 字符串转变成数值型
		const double dReserveTime	 =_tstof(szReserveTime);
		const double dPeakHeight	 =_tstof(szPeakHeight);
		const double dPeakHighPercent=_tstof(szPeakHighPercent);
		const double dPeakArea	     =_tstof(szPeakArea);
		const double dPeakAreaPercent=_tstof(szPeakAreaPercent);
		const double dConcentration	 =_tstof(szConcentration);

		//每一行数据创建一个新的CPeakStandard对象数据
		CPeakStandard aPeakStard;
		CPeakStandard::CPeakType aPeakType;
		aPeakType.GetFromTypeName(szPeakType);
		aPeakStard.m_cPeakType = aPeakType;//峰类型

		aPeakStard.m_dReserveTime = dReserveTime;//保留时间
		//aPeakStard.m_cStrComponentName = szComponentName; //组分名
		aPeakStard.m_cStrComponentName = szComponentName;//组份名称

		aPeakStard.m_dHeight = dPeakHeight;		//峰高
		aPeakStard.m_dHeightPer = dPeakHighPercent;	//峰高%
		aPeakStard.m_dArea = dPeakArea;		//峰面积

		aPeakStard.m_dAreaPer = dPeakAreaPercent;		//峰面积%
		aPeakStard.m_dResultConcentration = dConcentration;	//分析结果（百分比%）在分析结果表中使用，峰列表中不用显示

		m_cArrayData.Add(aPeakStard);
	}

	if(m_cArrayData.GetSize() != (int)dataNum){ASSERT(false); return FALSE;}

	return TRUE;
}


