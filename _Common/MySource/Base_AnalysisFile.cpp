#include "stdafx.h"
#include "Base_AnalysisSample.h"
#include "PeakSeak.h"
#include "QuantitativeCalculation.h"
#include "C2Bitmap.h"


//#include "others.h"
//#include "MyDDE.h"
//#include "MyOPC.h"
//#include "ResultInfo.h"
//#include "ResultListView.h"

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

	
	/*
	//pIADataTable数据导入成员变量m_cArrayData;
	const long dataNum = cIADataTable.GetRowsNumber();
	for (long i=0; i<dataNum; ++i)
	{
		//获取各个单元格内的数据
		//CString szID; //序号
		//const long IDIdex = cIADataTable.GetColumnIndex(_T("ID"));
		//if(!cIADataTable.GetCellValue(i, IDIdex, szID)) return FALSE; 
		CString strType;

		CString szPeakType; //峰类型CPeakType	
		const long PeakTypeIdex = cIADataTable.GetColumnIndex(_T("PeakType"));
		if(!cIADataTable.GetColumnInfo(PeakTypeIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("string")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakTypeIdex, szPeakType)) return FALSE; 

		CString szPeakBeginX; //峰起点X
		const long PeakBeginXIdex = cIADataTable.GetColumnIndex(_T("PeakBeginX"));
		if(!cIADataTable.GetColumnInfo(PeakBeginXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakBeginXIdex, szPeakBeginX)) return FALSE;

		CString szPeakBeginY; //峰起点Y
		const long PeakBeginYIdex = cIADataTable.GetColumnIndex(_T("PeakBeginY"));
		if(!cIADataTable.GetColumnInfo(PeakBeginXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakBeginYIdex, szPeakBeginY)) return FALSE;

		CString szPeakTOPX; //峰顶点X
		const long PeakTOPXIdex = cIADataTable.GetColumnIndex(_T("PeakTopX"));
		if(!cIADataTable.GetColumnInfo(PeakTOPXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakTOPXIdex, szPeakTOPX)) return FALSE;

		CString szPeakTOPY; //峰顶点Y
		const long PeakTOPYIdex = cIADataTable.GetColumnIndex(_T("PeakTopY"));
		if(!cIADataTable.GetColumnInfo(PeakTOPYIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakTOPYIdex, szPeakTOPY)) return FALSE;

		CString szPeakEndX; //峰落点X
		const long PeakEndXIdex = cIADataTable.GetColumnIndex(_T("PeakEndX"));
		if(!cIADataTable.GetColumnInfo(PeakEndXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakEndXIdex, szPeakEndX)) return FALSE;

		CString szPeakEndY; //峰落点Y
		const long PeakEndYIdex = cIADataTable.GetColumnIndex(_T("PeakEndY"));
		if(!cIADataTable.GetColumnInfo(PeakEndYIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakEndYIdex, szPeakEndY)) return FALSE;

		CString szReserveTime; //保留时间
		const long ReserveTimeIdex = cIADataTable.GetColumnIndex(_T("ReserveTime"));
		if(!cIADataTable.GetColumnInfo(ReserveTimeIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, ReserveTimeIdex, szReserveTime)) return FALSE;

		CString szHalfPeakWidth; //半峰宽
		const long HalfPeakWidthIdex = cIADataTable.GetColumnIndex(_T("HalfPeakWidth"));
		if(!cIADataTable.GetColumnInfo(HalfPeakWidthIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, HalfPeakWidthIdex, szHalfPeakWidth)) return FALSE;

		CString szPeakWidth; //峰宽
		const long PeakWidthIdex = cIADataTable.GetColumnIndex(_T("PeakWidth"));
		if(!cIADataTable.GetColumnInfo(PeakWidthIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakWidthIdex, szPeakWidth)) return FALSE;

		CString szPeakHeight; //峰高
		const long PeakHeightIdex = cIADataTable.GetColumnIndex(_T("PeakHeight"));
		if(!cIADataTable.GetColumnInfo(PeakHeightIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakHeightIdex, szPeakHeight)) return FALSE;

		CString szPeakHighPercent; //峰高%
		const long PeakHighPercentIdex = cIADataTable.GetColumnIndex(_T("PeakHighPercent"));
		if(!cIADataTable.GetColumnInfo(PeakHighPercentIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakHighPercentIdex, szPeakHighPercent)) return FALSE;

		CString szPeakArea; //峰面积
		const long PeakAreaIdex = cIADataTable.GetColumnIndex(_T("PeakArea"));
		if(!cIADataTable.GetColumnInfo(PeakAreaIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakAreaIdex, szPeakArea)) return FALSE;

		CString szPeakAreaPercent; //峰面积%
		const long PeakAreaPercentIdex = cIADataTable.GetColumnIndex(_T("PeakAreaPercent"));
		if(!cIADataTable.GetColumnInfo(PeakAreaPercentIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, PeakAreaPercentIdex, szPeakAreaPercent)) return FALSE;

		CString szDisfactor; //畸变因子
		const long DisfactorIdex = cIADataTable.GetColumnIndex(_T("Disfactor"));
		if(!cIADataTable.GetColumnInfo(DisfactorIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, DisfactorIdex, szDisfactor)) return FALSE;
		
		CString szSymmetry; //拖尾因子
		const long SymmetryIdex = cIADataTable.GetColumnIndex(_T("Symmetry"));
		if(!cIADataTable.GetColumnInfo(SymmetryIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!cIADataTable.GetCellValue(i, SymmetryIdex, szSymmetry)) return FALSE;

		CString szResolution; //分离度
		const long ResolutionIdex = cIADataTable.GetColumnIndex(_T("Resolution"));
		if(!cIADataTable.GetColumnInfo(ResolutionIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!cIADataTable.GetCellValue(i, ResolutionIdex, szResolution)) return FALSE;

		CString szColumnPlate; //理论塔板数
		const long ColumnPlateIdex = cIADataTable.GetColumnIndex(_T("ColumnPlate"));
		if(!cIADataTable.GetColumnInfo(ColumnPlateIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!cIADataTable.GetCellValue(i, ColumnPlateIdex, szColumnPlate)) return FALSE;

		CString szConcentration; //分析结果
		const long ConcentrationIdex = cIADataTable.GetColumnIndex(_T("Concentration"));
		if(!cIADataTable.GetColumnInfo(ConcentrationIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!cIADataTable.GetCellValue(i, ConcentrationIdex, szConcentration)) return FALSE;

		CString szComponentName; //组份名称
		const long ComponentNameIdex = cIADataTable.GetColumnIndex(_T("ComponentName"));
		if(!cIADataTable.GetColumnInfo(ComponentNameIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("string")) return FALSE; 
		if(!cIADataTable.GetCellValue(i, ComponentNameIdex, szComponentName)) return FALSE;

		// 字符串转变成数值型
		const double dPeakBeginX	 =_tstof(szPeakBeginX);
		const double dPeakBeginY	 =_tstof(szPeakBeginY);
		const double dPeakTOPX		 =_tstof(szPeakTOPX);
		const double dPeakTOPY	     =_tstof(szPeakTOPY);
		const double dPeakEndX		 =_tstof(szPeakEndX);
		const double dPeakEndY		 =_tstof(szPeakEndY);
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
		//aPeakStard.m_nType = iType;//峰类型
		aPeakStard.m_dReserveTime = dReserveTime;//保留时间
		CCurveDataSingle startCurveData(dPeakBeginX, dPeakBeginY);
		CCurveDataSingle topCurveData(dPeakTOPX, dPeakTOPY);
		CCurveDataSingle endCurveData(dPeakEndX, dPeakEndY);
		aPeakStard.m_dataBegin = startCurveData;//峰起点
		aPeakStard.m_dataTop = topCurveData;//峰顶（峰，谷）
		aPeakStard.m_dataEnd = endCurveData;//峰落点
		aPeakStard.m_dHalfWidth = dHalfPeakWidth;	//半峰宽
		aPeakStard.m_dWidth = dPeakWidth;		//峰宽
		aPeakStard.m_dHeight = dPeakHeight;		//峰高
		aPeakStard.m_dHeightPer = dPeakHighPercent;	//峰高%
		aPeakStard.m_dArea = dPeakArea;		//峰面积
		aPeakStard.m_dAreaPer = dPeakAreaPercent;		//峰面积%
		aPeakStard.m_dResultConcentration = dConcentration;	//分析结果（百分比%）在分析结果表中使用，峰列表中不用显示
		aPeakStard.m_dDisfactor = dDisfactor;   //畸变因子??
		aPeakStard.m_dSymmetry = dSymmetry;    //拖尾因子
		aPeakStard.m_dResolution = dResolution;  //分离度
		aPeakStard.m_dColumnPlate = dColumnPlate; //理论塔板数
		aPeakStard.m_cStrComponentName = szComponentName;//组份名称
		
		m_cArrayData.Add(aPeakStard);
	}
	
	if(m_cArrayData.GetSize() != (int)dataNum) return FALSE;

	return TRUE;
	*/
}
BOOL CPeakList::SyncFromXML_PeakListTable(CIADataTable* pCIADataTable){
	//pIADataTable数据导入成员变量m_cArrayData;
	const long dataNum = pCIADataTable->GetRowsNumber();
	for (long i=0; i<dataNum; ++i)
	{
		//获取各个单元格内的数据
		//CString szID; //序号
		//const long IDIdex = pCIADataTable->GetColumnIndex(_T("ID"));
		//if(!pCIADataTable->GetCellValue(i, IDIdex, szID)) return FALSE; 
		CString strType;

		CString szPeakType; //峰类型CPeakType	
		const long PeakTypeIdex = pCIADataTable->GetColumnIndex(_T("PeakType"));
		if(!pCIADataTable->GetColumnInfo(PeakTypeIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("string")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakTypeIdex, szPeakType)) return FALSE; 

		CString szPeakBeginX; //峰起点X
		const long PeakBeginXIdex = pCIADataTable->GetColumnIndex(_T("PeakBeginX"));
		if(!pCIADataTable->GetColumnInfo(PeakBeginXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakBeginXIdex, szPeakBeginX)) return FALSE;

		CString szPeakBeginY; //峰起点Y
		const long PeakBeginYIdex = pCIADataTable->GetColumnIndex(_T("PeakBeginY"));
		if(!pCIADataTable->GetColumnInfo(PeakBeginXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakBeginYIdex, szPeakBeginY)) return FALSE;

		CString szPeakTOPX; //峰顶点X
		const long PeakTOPXIdex = pCIADataTable->GetColumnIndex(_T("PeakTopX"));
		if(!pCIADataTable->GetColumnInfo(PeakTOPXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakTOPXIdex, szPeakTOPX)) return FALSE;

		CString szPeakTOPY; //峰顶点Y
		const long PeakTOPYIdex = pCIADataTable->GetColumnIndex(_T("PeakTopY"));
		if(!pCIADataTable->GetColumnInfo(PeakTOPYIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakTOPYIdex, szPeakTOPY)) return FALSE;

		CString szPeakEndX; //峰落点X
		const long PeakEndXIdex = pCIADataTable->GetColumnIndex(_T("PeakEndX"));
		if(!pCIADataTable->GetColumnInfo(PeakEndXIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakEndXIdex, szPeakEndX)) return FALSE;

		CString szPeakEndY; //峰落点Y
		const long PeakEndYIdex = pCIADataTable->GetColumnIndex(_T("PeakEndY"));
		if(!pCIADataTable->GetColumnInfo(PeakEndYIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakEndYIdex, szPeakEndY)) return FALSE;

		CString szReserveTime; //保留时间
		const long ReserveTimeIdex = pCIADataTable->GetColumnIndex(_T("ReserveTime"));
		if(!pCIADataTable->GetColumnInfo(ReserveTimeIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, ReserveTimeIdex, szReserveTime)) return FALSE;

		CString szHalfPeakWidth; //半峰宽
		const long HalfPeakWidthIdex = pCIADataTable->GetColumnIndex(_T("HalfPeakWidth"));
		if(!pCIADataTable->GetColumnInfo(HalfPeakWidthIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, HalfPeakWidthIdex, szHalfPeakWidth)) return FALSE;

		CString szPeakWidth; //峰宽
		const long PeakWidthIdex = pCIADataTable->GetColumnIndex(_T("PeakWidth"));
		if(!pCIADataTable->GetColumnInfo(PeakWidthIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakWidthIdex, szPeakWidth)) return FALSE;

		CString szPeakHeight; //峰高
		const long PeakHeightIdex = pCIADataTable->GetColumnIndex(_T("PeakHeight"));
		if(!pCIADataTable->GetColumnInfo(PeakHeightIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakHeightIdex, szPeakHeight)) return FALSE;

		CString szPeakHighPercent; //峰高%
		const long PeakHighPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakHighPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakHighPercentIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakHighPercentIdex, szPeakHighPercent)) return FALSE;

		CString szPeakArea; //峰面积
		const long PeakAreaIdex = pCIADataTable->GetColumnIndex(_T("PeakArea"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakAreaIdex, szPeakArea)) return FALSE;

		CString szPeakAreaPercent; //峰面积%
		const long PeakAreaPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakAreaPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaPercentIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakAreaPercentIdex, szPeakAreaPercent)) return FALSE;

		CString szDisfactor; //畸变因子
		const long DisfactorIdex = pCIADataTable->GetColumnIndex(_T("Disfactor"));
		if(!pCIADataTable->GetColumnInfo(DisfactorIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, DisfactorIdex, szDisfactor)) return FALSE;

		CString szSymmetry; //对称因子
		const long SymmetryIdex = pCIADataTable->GetColumnIndex(_T("Symmetry"));
		if(!pCIADataTable->GetColumnInfo(SymmetryIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, SymmetryIdex, szSymmetry)) return FALSE;

		CString szResolution; //分离度
		const long ResolutionIdex = pCIADataTable->GetColumnIndex(_T("Resolution"));
		if(!pCIADataTable->GetColumnInfo(ResolutionIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!pCIADataTable->GetCellValue(i, ResolutionIdex, szResolution)) return FALSE;

		CString szColumnPlate; //塔板数
		const long ColumnPlateIdex = pCIADataTable->GetColumnIndex(_T("ColumnPlate"));
		if(!pCIADataTable->GetColumnInfo(ColumnPlateIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!pCIADataTable->GetCellValue(i, ColumnPlateIdex, szColumnPlate)) return FALSE;

		CString szConcentration; //浓度
		const long ConcentrationIdex = pCIADataTable->GetColumnIndex(_T("Concentration"));
		if(!pCIADataTable->GetColumnInfo(ConcentrationIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!pCIADataTable->GetCellValue(i, ConcentrationIdex, szConcentration)) return FALSE;

		CString szComponentName; //组份名称
		const long ComponentNameIdex = pCIADataTable->GetColumnIndex(_T("ComponentName"));
		if(!pCIADataTable->GetColumnInfo(ComponentNameIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("string")) return FALSE; 
		if(!pCIADataTable->GetCellValue(i, ComponentNameIdex, szComponentName)) return FALSE;

		// 字符串转变成数值型
		const double dPeakBeginX	 =_tstof(szPeakBeginX);
		const double dPeakBeginY	 =_tstof(szPeakBeginY);
		const double dPeakTOPX		 =_tstof(szPeakTOPX);
		const double dPeakTOPY	     =_tstof(szPeakTOPY);
		const double dPeakEndX		 =_tstof(szPeakEndX);
		const double dPeakEndY		 =_tstof(szPeakEndY);
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
		CCurveDataSingle topCurveData(dPeakTOPX, dPeakTOPY);
		CCurveDataSingle endCurveData(dPeakEndX, dPeakEndY);
		aPeakStard.m_dataBegin = startCurveData;//峰起点
		aPeakStard.m_dataTop = topCurveData;//峰顶（峰，谷）
		aPeakStard.m_dataEnd = endCurveData;//峰落点
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

	if(m_cArrayData.GetSize() != (int)dataNum) return FALSE;

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
		if(!pCIADataTable->GetColumnInfo(PeakTypeIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("string")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakTypeIdex, szPeakType)) return FALSE; 

		CString szReserveTime; //保留时间
		const long ReserveTimeIdex = pCIADataTable->GetColumnIndex(_T("ReserveTime"));
		if(!pCIADataTable->GetColumnInfo(ReserveTimeIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, ReserveTimeIdex, szReserveTime)) return FALSE;

		CString szPeakHeight; //峰高
		const long PeakHeightIdex = pCIADataTable->GetColumnIndex(_T("PeakHeight"));
		if(!pCIADataTable->GetColumnInfo(PeakHeightIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakHeightIdex, szPeakHeight)) return FALSE;

		CString szPeakHighPercent; //峰高%
		const long PeakHighPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakHighPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakHighPercentIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakHighPercentIdex, szPeakHighPercent)) return FALSE;

		CString szPeakArea; //峰面积
		const long PeakAreaIdex = pCIADataTable->GetColumnIndex(_T("PeakArea"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakAreaIdex, szPeakArea)) return FALSE;

		CString szPeakAreaPercent; //峰面积%
		const long PeakAreaPercentIdex = pCIADataTable->GetColumnIndex(_T("PeakAreaPercent"));
		if(!pCIADataTable->GetColumnInfo(PeakAreaPercentIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE;
		if(!pCIADataTable->GetCellValue(i, PeakAreaPercentIdex, szPeakAreaPercent)) return FALSE;

		CString szConcentration; //浓度
		const long ConcentrationIdex = pCIADataTable->GetColumnIndex(_T("Concentration"));
		if(!pCIADataTable->GetColumnInfo(ConcentrationIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("float")) return FALSE; 
		if(!pCIADataTable->GetCellValue(i, ConcentrationIdex, szConcentration)) return FALSE;

		CString szComponentName; //组份名称
		const long ComponentNameIdex = pCIADataTable->GetColumnIndex(_T("ComponentName"));
		if(!pCIADataTable->GetColumnInfo(ComponentNameIdex, XMLPROP_VALUETYPE, strType)) return FALSE;
		if (strType != _T("string")) return FALSE; 
		if(!pCIADataTable->GetCellValue(i, ComponentNameIdex, szComponentName)) return FALSE;

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
		aPeakStard.m_dHeight = dPeakHeight;		//峰高
		aPeakStard.m_dHeightPer = dPeakHighPercent;	//峰高%
		aPeakStard.m_dArea = dPeakArea;		//峰面积
		aPeakStard.m_dAreaPer = dPeakAreaPercent;		//峰面积%
		aPeakStard.m_dResultConcentration = dConcentration;	//分析结果（百分比%）在分析结果表中使用，峰列表中不用显示
		aPeakStard.m_cStrComponentName = szComponentName;//组份名称

		m_cArrayData.Add(aPeakStard);
	}

	if(m_cArrayData.GetSize() != (int)dataNum) return FALSE;

	return TRUE;
}




CBase_AnalysisSample::CBase_AnalysisSample(LPCTSTR lpcszName, int nInjectNO,CBase_AnalysisTemplate *pTemplate) : 
m_pChartCtrl(NULL),
m_pMethod(NULL),
m_dStartAcquireTime_mins(0),
m_cResultList(XMLPATH_SAMPLE_RESULT_RESULT, &m_data),
m_cPeakList(XMLPATH_SAMPLE_RESULT_PEAKLIST, &m_data),
m_dMaxThreShold(10000.0),
m_dMinThreShold(0.0),
m_bDeleted(false),m_bNeedStop(false),m_dwAutoStopMins(0)
{
#ifdef _DEBUG
	//m_data.EnableAssertionsForInfoValidating = false;
#endif

	m_data.SetFlags(CAnalysisData::ADTF_VALIDATEONGETTING_DISABLED|CAnalysisData::ADTF_VALIDATEONSETTING_COMPATIBLE|CAnalysisData::ADTF_MODEONSETTING_CREATE|CAnalysisData::ADTF_USINGFORMATONSETTING|CAnalysisData::ADTF_USINGFORMATONGETTING);
	m_data.Create(XMLROOT_SAMPLE);
	
	m_pTemplate = pTemplate;
	//设定存盘目录
	m_cStrSavePath = m_pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE);
	//设定样品的存盘文件名称
	m_cStrMainFileName = CreateSampleFileName(lpcszName, nInjectNO);
	//设定样品初始状态
	m_statusCurrent=null;
	//设定样品名称
	ChangeSampleName(lpcszName,nInjectNO);
	//初始化曲线
	XML_InitMainCurveData();
	//优化主曲线数据
	//m_arrayCurveData.SetSize(0, 20*60*5);//以每秒20个数据来计算，初始化10分钟数据，增长5分钟数据

#ifdef _DEBUG
	//m_data.EnableAssertionsForInfoValidating = true;
#endif
}

CBase_AnalysisSample::~CBase_AnalysisSample()
{
	if(!m_bDeleted) Save();
	//通知Template我死了
	m_pTemplate->Notify_SampleDead(this);

}

CBase_AnalysisSample::enumSampleType CBase_AnalysisSample::GetType() const
{
	CString value;
	m_data.GetValue_String(XMLPATH_SAMPLE_SAMPLEPURPOSE, value);
	return FromTypeString(value);
}
bool CBase_AnalysisSample::SetType(CBase_AnalysisSample::enumSampleType eType)
{
	return m_data.SetValue_String(XMLPATH_SAMPLE_SAMPLEPURPOSE, ToTypeString(eType)) == IAERROR_SUCCESS;
}
LPCTSTR CBase_AnalysisSample::TypeDescription() const
{
	return ToTypeDescription(GetType());
}
LPCTSTR CBase_AnalysisSample::ToTypeDescription(CBase_AnalysisSample::enumSampleType eType)
{
	switch(eType)
	{
	case SAMPLE_TEST:return _T("试样");
	case SAMPLE_STANDARD:return _T("标样");
	default:return _T("");
	}
}
LPCTSTR CBase_AnalysisSample::ToTypeString(CBase_AnalysisSample::enumSampleType eType)
{
	switch(eType)
	{
	case SAMPLE_TEST:return _T("analysis");
	case SAMPLE_STANDARD:return _T("standard");
	default:return _T("");
	}
}
CBase_AnalysisSample::enumSampleType CBase_AnalysisSample::FromTypeString(const CString& typeString, enumSampleType eDefault)
{
	if(typeString == _T("analysis")) return SAMPLE_TEST;
	if(typeString == _T("standard")) return SAMPLE_STANDARD;
	return eDefault;
}

//样品名称
CString CBase_AnalysisSample::GetSampleName() const
{
	CString value;
	if(m_data.GetValue_String(XMLPATH_SAMPLE_SAMPLENAME, value) == IAERROR_SUCCESS) return value;
	return _T("");
}


bool CBase_AnalysisSample::ChangeSampleName(LPCTSTR lpcszName, int nInjectNO)
{
	if(lpcszName == NULL || _tcslen(lpcszName) == 0) return false;//名称不能为空
	CString oldName;
	if(m_data.GetValue_String(XMLPATH_SAMPLE_SAMPLENAME, oldName) == IAERROR_SUCCESS && oldName == lpcszName) return true;//名称相同

	//  [5/3/2011 Harley]-------------------------------------
	//  [5/3/2011 Harley]-------------------------------------
	//  [5/3/2011 Harley]-------------------------------------
	//  [5/3/2011 Harley]-------------------------------------
	//这句有一点问题：当前样品没有存在标准目录下的时候
	if(IsNameExist(lpcszName, nInjectNO, this->m_pTemplate)) return false;

	//设定新的样品名称
	m_data.SetValue_String(XMLPATH_SAMPLE_SAMPLENAME, lpcszName);
	m_data.SetValue_Int(XMLPATH_SAMPLE_SAMPLEINJECTNO, nInjectNO);
	//生成新的样品文件名
	//CString szNewname = m_cStrSavePath + lpcszName + ANALYSIS_SAMPLE_FILE_EXT;
	CString szNewname = CreateSampleFileName(lpcszName, nInjectNO);
	//oldName = m_cStrSavePath + oldName + ANALYSIS_SAMPLE_FILE_EXT;
	oldName = m_cStrSavePath + m_cStrMainFileName;
	//如果成功移动，将文件名变量更新
	if(MoveFile(oldName, m_cStrSavePath + szNewname))
		m_cStrMainFileName = szNewname;

	//曲线文件可能会改变
	UpdateXMLCurve2DNames();
	return true;
}

//进样号
int CBase_AnalysisSample::GetInjectNO() const
{
	int value;
	if(m_data.GetValue_Int(XMLPATH_SAMPLE_SAMPLEINJECTNO, value) == IAERROR_SUCCESS) return value;
	return 0;
}

int CBase_AnalysisSample::GetInstrumentChannel(void) const
{
	int value;
	if(m_data.GetValue_Int(XMLPATH_SAMPLE_PARAM_INSTRU_SC, value) == IAERROR_SUCCESS) return value;
	return 0;

}

bool CBase_AnalysisSample::SetInjectNO(int iNO)
{
	return m_data.SetValue_Int(XMLPATH_SAMPLE_SAMPLEINJECTNO, iNO) == IAERROR_SUCCESS;
}

//采用的方法
CString CBase_AnalysisSample::GetSourceMethod() const
{
	CString value;
	if(m_data.GetValue_String(XMLPATH_SAMPLE_METHODNAME, value) == IAERROR_SUCCESS) return value;
	return _T("");
}
bool CBase_AnalysisSample::SetSourceMethod(LPCTSTR lpcszMethodName)
{
	return m_data.SetValue_String(XMLPATH_SAMPLE_METHODNAME, lpcszMethodName) == IAERROR_SUCCESS;
	//需要重新加载m_pMethod,并替换掉内部的方法段
	//m_pTemplate->FreeMethod(m_pMethod);
	//m_pMethod = m_pTemplate->LoadMethod(lpcszMethodName);
	//ReplaceMethod();
}
CString CBase_AnalysisSample::GetCurMethod() const
{
	if(m_pMethod == NULL) return GetSourceMethod();
	return m_pMethod->GetName();
}
/*
//分析次数
int CBase_AnalysisSample::GetCount() const
{
	int value;
	if(m_data.GetValue_Int(XMLPATH_SAMPLE_COUNT, value) == IAERROR_SUCCESS) return value;
	return 5;
}
bool CBase_AnalysisSample::SetCount(int iCount)
{
	return m_data.SetValue_Int(XMLPATH_SAMPLE_COUNT, iCount) == IAERROR_SUCCESS;
}
*/
//绘画类型
CBase_AnalysisSample::enumDrawType CBase_AnalysisSample::GetDrawType() const
{
	int value;
	//if(m_data.GetValue_Int(XMLPATH_SAMPLE_DRAWTYPE, value) == IAERROR_SUCCESS) return (enumDrawType)value;
	//以后应该成曲线属性里读出来
	return DT_LINE;
}
bool CBase_AnalysisSample::SetDrawType(CBase_AnalysisSample::enumDrawType eDrawType)
{
	//以后应该设置到曲线属性里，该函数还需要增加参数:哪一条曲线

	//return m_data.SetValue_Int(XMLPATH_SAMPLE_DRAWTYPE, eDrawType) == IAERROR_SUCCESS;
	return TRUE;
}
/*
//综合设置
bool CBase_AnalysisSample::SetAttribute(LPCTSTR lpcszName, LPCTSTR lpcszMethodName, int iCount)
{
	ChangeSampleName(lpcszName);
	SetSourceMethod(lpcszMethodName);
	//SetCount(iCount);
	return true;
}
*/
void CBase_AnalysisSample::CreateID()
{
	m_data.SetValue_Int(XMLPATH_SAMPLE_SAMPLEID, ++g_cConfigIni.CurrentSystem().m_iNextSampleID);
}
void CBase_AnalysisSample::GetDataFrom(const CBase_AnalysisMethod &src)
{
	CString cStrMethod = src.m_cCurrentMethodData.GetXMLString();

	//这样做不太好，临时凑合
	VERIFY(cStrMethod.Replace(_T("<Method>"), _T("<Sample>"))==1);
	VERIFY(cStrMethod.Replace(_T("</Method>"), _T("</Sample>"))==1);
	m_data.SetXMLString(cStrMethod);

}

int CBase_AnalysisSample::GetIADataCurve2D(CIADataCurve2D& xmlC2D) const
{
	return m_data.GetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D);
}

BOOL CBase_AnalysisSample::LoadSampleFile(CString cStrFullPathName)
{
	CFileFind find;

	if(!find.FindFile(cStrFullPathName))
		return FALSE;

	find.FindNextFile();
	m_cStrSavePath = find.GetFilePath();
	m_cStrSavePath = m_cStrSavePath.Left(m_cStrSavePath.GetLength()-find.GetFileName().GetLength());
	TCHAR pp= m_cStrSavePath.GetAt(m_cStrSavePath.GetLength()-1);
	if(m_cStrSavePath.GetAt(m_cStrSavePath.GetLength()-1)!=_T('\\'))
		m_cStrSavePath +=_T("\\");

	m_cStrMainFileName = find.GetFileName();



	if (m_data.LoadFromFile(cStrFullPathName) == IAERROR_SUCCESS) {
		//调入样品后确定该样品的状态
		CIADataCurve2D xmlC2D;
		CString fn, mg, ms;
		if(!PASUCCESS(GetIADataCurve2D(xmlC2D)) || !GetIADataCurve2D_MainInfo(&mg, &ms) || !xmlC2D.QuickGetCurve2DFile(mg, ms, fn)) 
			m_statusCurrent = null;//SetStatus(null);
		else{
			CIAFileCurve2D file;
			if(file.Open(m_cStrSavePath+GetMainCurveFileName()))
			{
				file.BeginEnum();
				double x=0, y=0;
				CTime t;
				BOOL bHaveData = FALSE;
				while(file.GetNext(x, y, t))
				{
					bHaveData = TRUE;
					m_dataMainCurve.AppendData(x, y);
				}
				file.Close();
				if(bHaveData) {
					m_statusCurrent = idle;//SetStatus(null);
					//再查看有没有结果数据
					CIADataTable dt;
					if(m_data.GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, dt) == IAERROR_SUCCESS && dt.GetRowsNumber() > 0) 
						m_statusCurrent = finish;//SetStatus(finish);
				}
				else
					m_statusCurrent = null;//SetStatus(null);

			}
			else
			{
				m_statusCurrent = null;//SetStatus(null);
				//TRACE(_T("无法打开主曲线数据文件：")+GetMainCurveFileName());
			}

		}
		//如果样品主曲线不存在存在，调用SetStatus(null)
		//else 如果样品主曲线存在，并且Sample\Result\ResultTable有内容，调用SetStatus(finish)
		//else 如果样品主曲线存在，并且Sample\Result\ResultTable为空，调用SetStatus(idle)
		// 
		//改变样品文件名
		// 
		//m_data.GetValue_String(XMLPATH_SAMPLE_SAMPLENAME, m_cStrMainFileName);
		//m_cStrMainFileName += ANALYSIS_SAMPLE_FILE_EXT;
		


		//
		//讲积分结果、峰列表同步到临时变量
		//
		//m_cResultList.SynchronizeFromXML();
		//m_cPeakList.SynchronizeFromXML();
		//打开样品文件即重新积分一次（为了获得垂直分峰线结果）
		int res = Base_DataProcess();
		Base_DataCalibrate();
		//if (res!=IAERROR_SUCCESS) return FALSE;//并不是所有的样品都有数据，也有部分样品尚未开始采样


		// 
		//通知样品数据变化
		//
		// 
		CParamChanged item;
		item.FullMark();
		m_pTemplate->Notify_SampleDataChanged(this, item);


		return TRUE;
	}
	return FALSE;
}

BOOL CBase_AnalysisSample::Load(LPCTSTR lpcszName, int nInjectNO)
{
	CString szFullname = m_cStrSavePath+CreateSampleFileName(lpcszName, nInjectNO);
// 	szFullname = m_cStrSavePath;//m_pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE);
// 	szFullname += lpcszName;
// 	szFullname += ANALYSIS_SAMPLE_FILE_EXT;
	return LoadSampleFile(szFullname);
}
BOOL CBase_AnalysisSample::Save(void)
{
	//保存图表信息
	if(m_pChartCtrl && IsWindow(m_pChartCtrl->GetSafeHwnd()))
	{
		CIADataCurve2D xmlC2D;
		//if(!PASUCCESS(GetIADataCurve2D(xmlC2D)))
		GetIADataCurve2D(xmlC2D);

		CIADataCurve2DSingle single;
		xmlC2D.AutoCurve2DSingle(_T("ResultPoints"), single);

		CString tmp;

		CChartAxis* pXAxis = m_pChartCtrl->GetBottomAxis();
		double dblMin, dblMax;
		pXAxis->GetMinMax(dblMin, dblMax);
		tmp.Format(_T("%.5f"), dblMin);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMIN_BOTTOM, tmp);
		tmp.Format(_T("%.5f"), dblMax);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMAX_BOTTOM, tmp);

		CChartAxis* pYAxis = m_pChartCtrl->GetLeftAxis();
		pYAxis->GetMinMax(dblMin, dblMax);
		tmp.Format(_T("%.5f"), dblMin);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMIN_LEFT, tmp);
		tmp.Format(_T("%.5f"), dblMax);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMAX_LEFT, tmp);

		tmp.Format(_T("%d"), GetDrawType());
		single.SetProperty(_T("nDrawType"), tmp);

		m_data.SetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D);
	}

	if (m_data.SaveToFile(m_cStrSavePath+m_cStrMainFileName) == IAERROR_SUCCESS)
		return TRUE;
	return FALSE;
}
BOOL CBase_AnalysisSample::Delete()
{
	//删除图表信息
	CIADataCurve2D xmlC2D;
	if(PASUCCESS(GetIADataCurve2D(xmlC2D)))
	{
		CIADataCurve2DSingle single;
		long lPos = 0;
		xmlC2D.BeginEnum(lPos);
		CString type, name, file;
		while(xmlC2D.GetNext(lPos, type, single))
		{
			single.BeginEnum();
			while(single.GetNext(name, file))
			{
				DeleteFile(m_pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE) + file);
			}
		}
	}
	m_bDeleted = true;
	return DeleteFile(m_cStrSavePath+m_cStrMainFileName);
}

BOOL CBase_AnalysisSample::IsNameExist(LPCTSTR lpcszName, int nInjectNO,CBase_AnalysisTemplate* pTemplate)
{
	CString szFullname = CreateSampleFileName(lpcszName, nInjectNO);
// 	szFullname = pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE);
// 	szFullname += lpcszName;
// 	szFullname += ANALYSIS_SAMPLE_FILE_EXT;
	DWORD dwAttr = GetFileAttributes(szFullname);
	if (dwAttr == INVALID_FILE_ATTRIBUTES || (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
		return FALSE;
	}
	return TRUE;
}
BOOL CBase_AnalysisSample::Delete(LPCTSTR lpcszName, CBase_AnalysisTemplate* pTemplate)
{
	CString szFullname;
	szFullname = pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE);
	szFullname += lpcszName;
	szFullname += ANALYSIS_SAMPLE_FILE_EXT;
	return DeleteFile(szFullname);
}

BOOL CBase_AnalysisSample::GetSampleNameList(CStringArray &arrayName, CBase_AnalysisTemplate* pTemplate)
{
	CString path = pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE) + _T("*") + ANALYSIS_SAMPLE_FILE_EXT;

	CFileFind ff;
	BOOL ret = ff.FindFile(path);
	while(ret)
	{
		ret = ff.FindNextFile();
		if(ff.IsDirectory()||ff.IsDots()||ff.IsHidden()) continue;		

		CString fileName = ff.GetFileName();
		fileName = fileName.Left(fileName.GetLength() - _tcslen(ANALYSIS_SAMPLE_FILE_EXT));
		arrayName.Add(fileName);
	}	

	return true;
}

/////////////////////////////////////////////////////////////////
void CBase_AnalysisSample::UpdateXMLCurve2DNames()
{
	CIADataCurve2D xmlC2D;
	if(PASUCCESS(GetIADataCurve2D(xmlC2D)))
	{
		int index;
		CString name,fileName,newFileName,type;
		CIADataCurve2DSingle single;
		xmlC2D.BeginEnum();
		while(xmlC2D.GetNext(type, single))
		{
			single.BeginEnum();
			while(single.GetNext(name, fileName))
			{
				if(type==STRING_MAINCURVE_GROUP&&name==STRING_MAINCURVE_NAME) 
					newFileName = CreateMainCurveFileName();
				else 
					newFileName = CreateCurveFileName(type, name);
				//newFileName.Format(_T("%s_%s.csv"), strName, name);//样品名_成分名.c2d
				if(newFileName != fileName)//需要更改文件名
				{
					CString path = g_pcAnalysisSystem->GetDataFolder(PA_DATATYPENAME_SAMPLE);
					if(!CStringExt::File_Exist(path+fileName) || MoveFile(path+fileName, path+newFileName)) single.SetEnum(newFileName);
				}
			}
		}

		m_data.SetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D);
	}
}
void CBase_AnalysisSample::RefreshDraw(enumDrawType eDrawType)
{
	//ASSERT(false);
	SetDrawType(eDrawType);
	//刷新绘图
	if(!m_pChartCtrl) return;
	CWnd* parent=m_pChartCtrl->GetParent();
	if(parent) parent->SendMessage(WM_USER+847, eDrawType, NULL);
}

int CBase_AnalysisSample::Base_RealtimeData(const CCurveData &curveData)
{
	if(GetStatus() == ready)
	{
		if(m_dStartAcquireTime_mins == -1 && curveData.m_arrayData.GetCount() > 0)
			m_dStartAcquireTime_mins = (DWORD)(curveData.m_arrayData.GetAt(0).dX*60000);
	}
	double xOffset = m_dStartAcquireTime_mins/-60000.0;

	//如果样品正在运行才保存数据
	if(GetStatus()==running) {
		for(int i=0; i<curveData.m_arrayData.GetCount(); ++i) {
			/*if(curveData.m_arrayData[i].dX<m_dStartAcquireTime_mins) {
				ASSERT(FALSE);
			}//*/
			if(curveData.m_arrayData[i].dX+xOffset < 0) continue;
			m_dataMainCurve.AppendData(curveData.m_arrayData[i].dX+xOffset, curveData.m_arrayData[i].dY);
		}
	}

	switch (GetStatus()){
		case idle:
		case finish:
			//AfxMessageBox(_T("Internal Error: 样品状态不正常！"));
			return IAERROR_CG_SAMPLE_STATUSERROR;
			break;
		default:
			//画实时曲线
			//单位从毫秒变成分钟；微伏变成毫伏
			if(m_pChartCtrl) m_pChartCtrl->AddRealTimeCurveDataMain(curveData, 1.0, 1.0, xOffset, 0.0);
			//else AfxMessageBox(_T("m_pChartCtrl为空！"));
	}

	if(GetStatus()==running && m_bNeedStop)
	{
		int count = curveData.m_arrayData.GetCount();
		if(curveData.m_arrayData[count-1].dX+xOffset >= m_dwAutoStopMins)
		{
			//TRACE(("Base_RealtimeData(%.3f)：%s\r\n"), curveData.m_arrayData[count-1].dX+xOffset, (LPCTSTR)(COleDateTime::GetCurrentTime()).Format(_T("%Y-%m-%dT%H:%M:%S")));
			m_pTemplate->Sample_StopRun(this);
		}
	}

	return IAERROR_SUCCESS;
}

void CBase_AnalysisSample::SetStatus(CBase_AnalysisSample::STATUS status)
{
	BOOL bChanged;
	if(status!=m_statusCurrent) bChanged=TRUE;
	else bChanged=FALSE;

	m_statusCurrent = status;

	if(bChanged)
		m_pTemplate->Notify_SampleStatusChanged(this);
}

BOOL CBase_AnalysisSample::Base_GetReady()
{
	m_dStartAcquireTime_mins = -1;
	SetStatus(ready);
	return TRUE;
}
//
//----------开始样品运行--------------------------------------
//
//返回值：
//	IAERROR_CG_SAMPLE_RUNNING：		错误，样品已经在运行
//	IAERROR_CG_SAMPLE_DATAEXIST：	错误，样品已经有数据
//	IAERROR_CG_INSTRUMENT_ERROR：	错误，仪器无法设置参数
//	IAERROR_CG_SAMPLE_BUSY：			样品正忙
//	IAERROR_CG_SAMPLE_NOTREADY		样品尚未准备好
//	IAERROR_SUCCESS：				样品已经准备好，可以进行下一步操作
//
//

int CBase_AnalysisSample::Base_StartRun(double dTimeMins)
{

	//
	//确定当前样品状态判断是否能进入运行状态
	//
	switch (GetStatus()){
	case ready		://样品准备好了	
		break;
	case running:	//采样							
		return IAERROR_CG_SAMPLE_RUNNING;
		break;
	case idle		://运行状态结束，可中断，但未完成数据处理
		return IAERROR_CG_SAMPLE_DATAEXIST;
		break;
	case finish		://完成						
		return IAERROR_CG_SAMPLE_DATAEXIST;
		break;
	case busy		://样品忙，不可中断状态
		return IAERROR_CG_SAMPLE_BUSY;
		break;
	case preparing	://样品准备(等待仪器参数到位)		
		return IAERROR_CG_SAMPLE_BUSY;
		break;
	case abort		://异常终止，或出错，数据应该都废除
	case null		://空闲（空样品）	
		return IAERROR_CG_SAMPLE_NOTREADY;
		break;
	default:
		ASSERT(FALSE);
		break;
	}


	//
	// 记录开始采样的时间，以便以后计算真正的保留时间
	// 
	m_dStartAcquireTime_mins = (DWORD)dTimeMins;
	m_dtStartRun = COleDateTime::GetCurrentTime();
	//TRACE("Base_StartRun（%.3f）：%s\r\n", dTimeMins, (LPCTSTR)m_dtStartRun.Format(_T("%Y-%m-%dT%H:%M:%S")));
	m_bNeedStop = false;
	//
	//仪器相关操作
	//
	//todo("可能需要启动程序升温或者其他仪器相关操作");
	//
	//开始记录数据
	//
	m_dataMainCurve.RemoveAll();

	//todo("与实时曲线数据记录的相关操作");
	//
	//界面操作
	//
	//在图形上加入开始标记
	//m_pChartCtrl->AddMarkLine(1);
	if(m_pChartCtrl)
	{
		m_pChartCtrl->EnableRefresh(false);
		m_pChartCtrl->m_pcCurveMain->ClearSerie();
		m_pChartCtrl->EnableRefresh(true);
		m_pChartCtrl->ResetAxis();
		m_pChartCtrl->RefreshCtrl();
	}

	//改变状态
	SetStatus(running);

	return IAERROR_SUCCESS;

}

void CBase_AnalysisSample::Base_ResetData(void)
{
	//清除曲线数据,清除m_xmlC2D中的数据部分"));
	{
		::DeleteFile(m_cStrSavePath+GetMainCurveFileName());
		m_data.SetValue_XMLString(XMLPATH_SAMPLE_DATA_CURVE, _T(""));
		XML_InitMainCurveData();
	}

	//清除样品参数（XML数据）中的相关数据和结果
	//清除Sample\\Result 所包含的数据部分
	{
		CIADataTable table;
		if(m_data.GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, table) == IAERROR_SUCCESS)
		{
			table.DeleteAllRows();
			m_data.SetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, table);
		}
		if(m_data.GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, table) == IAERROR_SUCCESS)
		{
			table.DeleteAllRows();
			m_data.SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, table);
		}
	}

	//清除曲线数据
	m_dataMainCurve.RemoveAll();

	//设置状态
	SetStatus(null);
	
	if(m_pChartCtrl)
	{
		m_pChartCtrl->EnableRefresh(false);
		m_pChartCtrl->m_pcCurveMain->ClearSerie();
		m_pChartCtrl->EnableRefresh(true);
		m_pChartCtrl->RefreshCtrl();
	}
}


//
//----------停止样品运行--------------------------------------
//
//返回值：
//	IAERROR_CG_SAMPLE_STATUSERROR：	错误，样品不在采样状态
//	IAERROR_SUCCESS：				操作成功
//

int  CBase_AnalysisSample::Base_StopRun(void)
{

	//
	//确定当前样品状态判断是否能进入停止状态
	//
	if(GetStatus()!=running)
		return IAERROR_CG_SAMPLE_STATUSERROR;
	
	m_data.SetValue_Time  (XMLPATH_SAMPLE_PARAM_INFO_ANALYSISTIME, CTime::GetCurrentTime());

	{//储存曲线数据
		CIAFileCurve2D file;

		SetStatus(idle);

		if(!file.Open(m_cStrSavePath+GetMainCurveFileName())) return IAERROR_FILE_NOTFOUND;

		for(int i=0; i<m_dataMainCurve.m_arrayData.GetCount(); ++i) {
			file.ApppendPoint(m_dataMainCurve.m_arrayData[i].dX, m_dataMainCurve.m_arrayData[i].dY);
		}
		file.Close();
	}

	m_dtStopRun = COleDateTime::GetCurrentTime();
	//TRACE("Base_StopRun：%s\r\n", (LPCTSTR)m_dtStopRun.Format(_T("%Y-%m-%dT%H:%M:%S")));

	//储存其他参数
	Save();


	//
	//界面操作
	//
	//清除当前图形窗口的显示内容
	
	//显示储存的样品曲线
	if(m_pChartCtrl)
	{
		m_pChartCtrl->EnableRefresh(false);
		m_pChartCtrl->ClearPeak(this, CBase_SampleChart::FLAGMASK_ALL);
		m_pChartCtrl->ClearCompSeries();
		m_pChartCtrl->ClearCurve2DSeries();
		m_pChartCtrl->ClearMarkLines();
		m_pChartCtrl->RedrawPeak(this, CBase_SampleChart::FLAG_DATA_SOURCE);
		m_pChartCtrl->ShowPeak(this, CBase_SampleChart::FLAG_DATA_SOURCE);
		m_pChartCtrl->EnableRefresh(true);
		m_pChartCtrl->RefreshCtrl();
	}

	//设定状态
	SetStatus(idle);
	
	CBase_AnalysisSample::CParamChanged pc;
	pc.param_information = true;
	if(m_pTemplate) m_pTemplate->Notify_SampleDataChanged(this, pc);

	return IAERROR_SUCCESS;

}

//
//----------(异常)终止样品运行--------------------------------------
//
//返回值：
//	IAERROR_CG_SAMPLE_STATUSERROR：	错误，样品不在采样状态
//	IAERROR_SUCCESS：				操作成功
//

int  CBase_AnalysisSample::Base_AbortRun(void)
{

	//
	//确定当前样品状态判断是否能进入停止状态
	//
	if(GetStatus()!=running)
		return IAERROR_CG_SAMPLE_STATUSERROR;

	//停止记录数据，并且清除已采集数据
	//
	//todo("停止记录数据但不储存");
	Base_ResetData();//清除数据
	m_dtStopRun = COleDateTime::GetCurrentTime();
	//
	//界面操作
	//
	//todo("改变相关的界面显示");

	//改变状态
	SetStatus(abort);

	return IAERROR_SUCCESS;

}


void CBase_AnalysisSample::XML_InitMainCurveData(void)
/*
<Sample>
	<Result>
		<CurveDatas>
			<CaptionDefine>
				<Item Type="Data">谱图数据</Item>
			</CaptionDefine>
			<Curve2D Type="Data">
				<Item name="谱图">未命名_谱图数据.csv</Item>
			</Curve2D>
		</CurveDatas>
	</Result>
</Sample>
*/
{
	CIADataCurve2D curve;
	//获得Curve2D,就算没有得到也没关系
	GetIADataCurve2D(curve);

/*
	curve.SetCurveCaption(SAMPLE_CURVE_TYPE_DATA, SAMPLE_CURVE_NAME_DATA);

	CIADataCurve2DSingle single;
	curve.AutoCurve2DSingle(SAMPLE_CURVE_TYPE_DATA, single);

	CString cStrTemp;
	cStrTemp.Format(_T("%s_谱图数据.csv"), GetName());
	single.AppendItem(_T("谱图"), cStrTemp);
	//记录存盘文件名
	m_cStrCurveFileName = cStrTemp;

	//curve.SetXMLPath(XMLPATH_SAMPLE_DATA_CURVE);
*/
	//加入一根主曲线
	curve.QuickSetCurve2DFile(STRING_MAINCURVE_GROUP, STRING_MAINCURVE_NAME, CreateMainCurveFileName());
	m_data.SetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE,curve);
}



bool CBase_AnalysisSample::Xml_PutParameter_Method(CAnalysisData& adMethod)
{
	CString xmlString;/*
	adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM_INSTRUMENT, xmlString);
	if(!Xml_PutParameter_Instrument(xmlString)) return false;
	adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM_CALIBRATE, xmlString);
	if(!Xml_PutParameter_Calibrate(xmlString)) return false;
	adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM_PEAKPROCESS, xmlString);
	if(!Xml_PutParameter_PeakProcess(xmlString)) return false;
	adMethod.GetValue_XMLString(XMLPATH_METHOD_REPORT, xmlString);
	if(!Xml_PutParameter_Report(xmlString)) return false;//*/
	if((adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM, xmlString) == IAERROR_SUCCESS) &&
		(m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM, xmlString) == IAERROR_SUCCESS))
	{
		CParamChanged itemChanged;
		itemChanged.Params();
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}
	return true;
}
bool CBase_AnalysisSample::Xml_PutParameter_Method(LPCTSTR xmlString)
{
	CAnalysisData adMethod;
	adMethod.SetXMLString(xmlString);

	return Xml_PutParameter_Method(adMethod);
}
bool CBase_AnalysisSample::Xml_PutParameter_Instrument(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_INSTRUMENT, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_instrument = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_Calibrate(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_CALIBRATE, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_calibration = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_PeakProcess(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_integration = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_Report(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_REPORT, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_information = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_GetParameter_Method(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_Instrument(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_INSTRUMENT, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_Calibrate(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_CALIBRATE, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_PeakProcess(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_Report(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_REPORT, xmlString) == IAERROR_SUCCESS;
}

CString CBase_AnalysisSample::GetCurveFileName(const CString& cStrMainGroup, const CString& cStrMainSerie)
{
	CString cStrName;
	CIADataCurve2D xmlC2D;
	VERIFY(PASUCCESS(GetIADataCurve2D(xmlC2D)));

	VERIFY(xmlC2D.QuickGetCurve2DFile(cStrMainGroup, cStrMainSerie, cStrName));
	return cStrName;
}

int	 CBase_AnalysisSample::Base_DataProcess(void)
{
	CPeakSeak	cPreaSeek;
	CAnalysisData cAnalysisData;
//	CIADataTable  cIAResultTable(NULL, _T("<PeakListTable></PeakListTable>"));
	CString cStrPeakProcess;

	//获得积分参数
	CString cStrTemp = m_data.GetXMLString();
	double *x, *y;
	int nNum = m_dataMainCurve.ConvertData(x, y, 1.0, 1.0, 0.0, 0.0);
	if(nNum<1)
		return IAERROR_SAMPLE_NODATA;

	int n;
		
	RETURNifERROR(m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, cStrPeakProcess));
	RETURNifERROR(cAnalysisData.LoadFromDocument(cStrPeakProcess));

	cStrTemp = cAnalysisData.GetXMLString();

	cPreaSeek.SetSemaDate(x, y, nNum);
	delete []x;
	delete []y;
	//cPreaSeek.SignalProcess(&cAnalysisData, &cIAResultTable);
	m_cPeakList.RemoveAll();
	m_arrayInterRes1.RemoveAll();
	m_arrayInterRes2.RemoveAll();
	m_arrayInterRes3.RemoveAll();
	m_arrayVLines.RemoveAll();
	//先不用该函数确定噪声最大值最小值
//	cPreaSeek.SignalProcess(&cAnalysisData, m_cPeakList, &m_arrayInterRes1, &m_arrayInterRes2, &m_dMinThreShold, &m_dMaxThreShold);
	double dTemp;
	cPreaSeek.SignalProcess(&cAnalysisData, m_cPeakList, &m_arrayInterRes1, &m_arrayInterRes2, &m_arrayInterRes3, &m_arrayVLines, &dTemp, &dTemp);
 	VERIFY(m_cPeakList.SynchronizeToXML());

	/*const unsigned PeakSize = cPreaSeek.m_Peak.size();
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (cPreaSeek.m_Peak[i].dType == CPeak::VERTDIVIDE)
		{
			structLine line;
			line.begin.x = line.end.x = cPreaSeek.m_Peak[i].FinSema.x;
			line.begin.y = cPreaSeek.m_OrigSema[cPreaSeek.m_Peak[i].FinSema.idx].y;
			line.end.y = cPreaSeek.m_Peak[i].FinSema.y;
			m_arrayVLines.Add(line);
		}
	}*/


	
	cStrTemp = m_data.GetXMLString();

	CParamChanged item;
	item.result_integration = 1;
	m_pTemplate->Notify_SampleDataChanged(this, item);

	return IAERROR_SUCCESS;
}


int	 CBase_AnalysisSample::Base_DataCalibrate(void)
{//和 定量计算 接口
	CString cStrTemp = m_data.GetXMLString();

	CAnalysisData cAnalysisData;
	CString cStrCalibrate;

	RETURNifERROR(m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_CALIBRATE, cStrCalibrate));
	RETURNifERROR(cAnalysisData.LoadFromDocument(cStrCalibrate));

	cStrTemp = cAnalysisData.GetXMLString();//调试

	CString cStrCalibrateMethod, cStrCalibrateObject;

	m_data.GetValue_String(XMLPATH_SAMPLE_PARAM_CALI_METHOD, cStrCalibrateMethod);
	m_data.GetValue_String(XMLPATH_SAMPLE_PARAM_CALI_OBJECT, cStrCalibrateObject);

	if(cStrCalibrateObject!=XMLVALUE_CONSTENT_c_peak_area&&cStrCalibrateObject!=XMLVALUE_CONSTENT_c_peak_height) {
		//ASSERT(FALSE);
		return IAERROR_XML_NOTVALID;
	}

	//峰匹配，定量计算
	CQuantitativeCalculation *pQC = new CQuantitativeCalculation(&m_data);
	int IRESULT = pQC->QuantitativeCalculationProcess();
	delete pQC;
	if (IRESULT!=IAERROR_SUCCESS) return IRESULT;
	
	//更新
	m_cPeakList.SynchronizeFromXML();
	m_cResultList.SynchronizeFromXML();
	
	if(cStrCalibrateMethod==XMLVALUE_CONST_c_normalization) {
		int nCount = m_cPeakList.m_cArrayData.GetCount();
		m_cResultList.RemoveAll();
		m_cResultList.m_cArrayData.Copy(m_cPeakList.m_cArrayData);
		for(int i=0; i<nCount; ++i) {
			if(cStrCalibrateObject==XMLVALUE_CONSTENT_c_peak_area)
				m_cResultList.m_cArrayData[i].m_dResultConcentration = m_cResultList.m_cArrayData[i].m_dAreaPer; 
			else 
				m_cResultList.m_cArrayData[i].m_dResultConcentration = m_cResultList.m_cArrayData[i].m_dHeightPer; 
		}

	}
	//else if(cStrCalibrateMethod==XMLVALUE_CONST_c_cnormalization) {
	//	AfxMessageBox(_T("校正归一化法 尚未实现"));
	//	return IAERROR_XML_NOTVALID;
	//}
	//else if(cStrCalibrateMethod==XMLVALUE_CONST_c_external) {
	//	AfxMessageBox(_T("外标法 尚未实现"));
	//	return IAERROR_XML_NOTVALID;
	//}
	//else if(cStrCalibrateMethod==XMLVALUE_CONST_c_internal) {
	//	AfxMessageBox(_T("内标法 尚未实现"));
	//	return IAERROR_XML_NOTVALID;
	//}
	//else {
	//	CString cStrTemp;
	//	cStrTemp.Format(_T("定量方法名称错误：%s"), cStrCalibrateMethod);
	//	AfxMessageBox(cStrTemp);
	//	//ASSERT(FALSE);
	//	return IAERROR_XML_NOTVALID;
	//}


	//VERIFY(m_cResultList.SynchronizeToXML());

	cStrTemp = m_data.GetXMLString();

	CParamChanged item;
	item.result_analysis = 1;
	m_pTemplate->Notify_SampleDataChanged(this, item);

	return IAERROR_SUCCESS;


}


bool CBase_AnalysisSample::SaveChart2File(int iWidth, int iHeight, const CString& file)
{
	if(!m_pChartCtrl) return false;
	if(!::IsWindow(m_pChartCtrl->GetSafeHwnd())) return false;

	m_pChartCtrl->EnableRefresh(false);

	CWnd* pParent = m_pChartCtrl->GetParent();
	CRect oldRCParent, oldRCChart;
	m_pChartCtrl->GetWindowRect(oldRCChart);
	pParent->ScreenToClient(oldRCChart);
	pParent->GetWindowRect(oldRCParent);
	if(pParent->GetParent()) pParent->GetParent()->ScreenToClient(oldRCParent);
	
	int offsetW = iWidth - oldRCChart.Width();
	int offsetH = iHeight - oldRCChart.Height();
	CRect rc(oldRCParent);
	rc.right += offsetW;
	rc.bottom += offsetH;
	pParent->MoveWindow(rc);

	rc = oldRCChart;
	rc.right = rc.left + iWidth;
	rc.bottom = rc.top + iHeight;
	m_pChartCtrl->MoveWindow(rc);
	
	COLORREF oldBkColor = m_pChartCtrl->GetBackColor();
	m_pChartCtrl->SetBackColor(RGB(255,255,255));

	bool oldLeftGrid = m_pChartCtrl->GetLeftAxis()->GetGrid()->IsVisible();
	bool oldBottomGrid = m_pChartCtrl->GetBottomAxis()->GetGrid()->IsVisible();
	m_pChartCtrl->GetLeftAxis()->GetGrid()->SetVisible(false);
	m_pChartCtrl->GetBottomAxis()->GetGrid()->SetVisible(false);

	m_pChartCtrl->EnableRefresh(true);
	m_pChartCtrl->RefreshCtrl();

	C2Bitmap::SaveToFile(m_pChartCtrl->GetDC(), CRect(0,0,iWidth,iHeight), file, iWidth, iHeight);//保存pDC的rc区域的内容为指定w和h的bmp，0表示无缩放

	//恢复
	m_pChartCtrl->EnableRefresh(false);
	m_pChartCtrl->MoveWindow(oldRCChart);
	pParent->MoveWindow(oldRCParent);
	m_pChartCtrl->SetBackColor(oldBkColor);
	m_pChartCtrl->GetLeftAxis()->GetGrid()->SetVisible(oldLeftGrid);
	m_pChartCtrl->GetBottomAxis()->GetGrid()->SetVisible(oldBottomGrid);
	m_pChartCtrl->EnableRefresh(true);
	m_pChartCtrl->RefreshCtrl();

	return true;
}