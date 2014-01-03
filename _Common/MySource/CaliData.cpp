#include "StdAfx.h"
#include "CaliData.h"
//#include "ChildViewCaliMain.h"
#include <set>


CaliItem::CaliItem(const CStringArray& data)
{
	if (data.GetSize()<18) ASSERT(TRUE);
	
	szNO			= data[1];	//编号
	szReserveTime	= data[2];	//保留时间
	szComponentName = data[3];	//组份名
	szLevel			= data[4];	//级别
	szContents		= data[5];  //含量
	szPeakArea		= data[6];	//峰面积
	szPeakHeight	= data[7];	//峰高
	szResFactor		= data[8];	//响应因子
	szIsReference	= data[9];	//是否参比
	szIsInterior	= data[10];	//是否内标
	szStdContent	= data[11];; //内标绝对含量:有样品信息中的表格内输入
	szInteriorNo	= data[12];	//内标编号
	szSampleFullName= data[13]; 
	szPeakBeginX    = data[14];
	szPeakEndX      = data[15];
	szSlope         = data[16];
	szIntercept     = data[17];
	szCoefficient   = data[18];
}


CaliItem::CaliItem(const CStringArray& data, const CString tableName){
	if (tableName==CALIBRATETABLE) {
		szNO			= data[1];	//编号
		szReserveTime	= data[2];	//保留时间
		szComponentName = data[3];	//组份名
		szLevel			= data[4];	//级别
		szContents		= data[5];  //含量
		szPeakArea		= data[6];	//峰面积
		szPeakHeight	= data[7];	//峰高
		szResFactor		= data[8];	//响应因子
		szIsReference	= data[9];	//是否参比
		szIsInterior	= data[10];	//是否内标
		szStdContent	= data[11]; //内标绝对含量:有样品信息中的表格内输入
		szInteriorNo	= data[12];	//内标编号
		szSampleFullName= data[13]; 
		szPeakBeginX    = data[14];
		szPeakEndX      = data[15];
		szSlope         = data[16];
		szIntercept     = data[17];
		szCoefficient   = data[18];
	}
	else if (tableName==CALIBRATERESULTTABLE)
	{
		szNO			= data[1];	//编号
		szReserveTime	= data[2];	//保留时间
		szComponentName = data[3];	//组份名
		szSlope         = data[4];
		szIntercept     = data[5];
		szCoefficient   = data[6];
		szIsReference	= data[7];	//是否参比
		szIsInterior	= data[8];	//是否内标
		szInteriorNo	= data[9];	//内标编号
		szStdContent	= data[10]; //内标绝对含量:有样品信息中的表格内输入
	}
	else if (tableName==PEAKLISTTABLE){
		szNO			= data[1];	//编号
		szReserveTime	= data[2];	//保留时间
		szComponentName = _T("");	//data[3];	//组份名
		szLevel			= _T("");	//data[4];	//级别
		szContents		= _T("");	//data[5];  //含量
		szPeakArea		= data[4];	//峰面积
		szPeakHeight	= data[6];	//峰高
		szResFactor		= _T("");	//data[8];	//响应因子
		szIsReference	= _T("");	//data[9];	//是否参比
		szIsInterior	= _T("");	//data[10];	//是否内标
		szStdContent	= _T("");	//data[11];; //内标绝对含量:有样品信息中的表格内输入
		szInteriorNo	= _T("");	//data[12];	//内标编号
		szSampleFullName= _T("");	//data[13]; 
		szPeakBeginX    = _T("");	//data[14];
		szPeakEndX      = _T("");	//data[15];
		szSlope         = _T("");	//data[16];
		szIntercept     = _T("");	//data[17];
		szCoefficient   = _T("");	//data[18];
	}
	

}


bool CaliItem::operator== (const CaliItem& rhs){
	return 	((szNO				== rhs.szNO)	//编号
			&&(szReserveTime	== rhs.szReserveTime)	//保留时间
			&&(szComponentName  == rhs.szComponentName)	//组份名
			&&(szLevel			== rhs.szLevel)	//级别
			&&(szContents		== rhs.szContents)  //含量
			&&(szPeakArea		== rhs.szPeakArea)	//峰面积
			&&(szPeakHeight		== rhs.szPeakHeight)	//峰高
			&&(szResFactor		== rhs.szResFactor)	//响应因子
			&&(szIsReference	== rhs.szIsReference)	//是否参比
			&&(szIsInterior		== rhs.szIsInterior)	//是否内标
			&&(szStdContent		== rhs.szStdContent) //内标绝对含量:有样品信息中的表格内输入
			&&(szInteriorNo		== rhs.szInteriorNo)	//内标编号
			&&(szSampleFullName == rhs.szSampleFullName)
			&&(szPeakBeginX     == rhs.szPeakBeginX)
			&&(szPeakEndX       == rhs.szPeakEndX)
			&&(szSlope          == rhs.szSlope)
			&&(szIntercept      == rhs.szIntercept)
			&&(szCoefficient    == rhs.szCoefficient));
}

CCaliData::CCaliData(void)
{
}

CCaliData::~CCaliData(void)
{
}

void CCaliData::InitCaliData(CIADataTable* pCIADataTable) {
	CString szWatch = pCIADataTable->GetXMLString();

	//添加一张表数据,自动生成新的内部数据
	if (!m_CaliItems.empty()) m_CaliItems.clear();

	DWORD dwOrders=0;
	pCIADataTable->GetTable(&m_tableName, &dwOrders);

	const long TableRows = pCIADataTable->GetRowsNumber();

	for (long i=0; i<TableRows; ++i)
	{
		CStringArray arrItem;
		pCIADataTable->GetRowValues(i, arrItem);
		CaliItem caliItem(arrItem, m_tableName);
		m_CaliItems.push_back(caliItem);
	}

	VerifyContent();
}




bool CCaliData::ItemSaveToTable(CIADataTable* pCIADataTable)
{
	CString szWatch = pCIADataTable->GetXMLString();

	pCIADataTable->DeleteAllRows();
	const int TableCols=pCIADataTable->GetColumnNumber();
	if (TableCols!=20)
	{
		::AfxMessageBox(_T("当前要保存的文件内部数据有误"));
		return false;
	}
	const int ItemNum = m_CaliItems.size();
	for (int i=0; i<ItemNum; ++i)
	{
		CStringArray arrItem;
		for (int c=0; c<TableCols; ++c)
		{
			CString str(_T(""));
			arrItem.Add(str);
		}
		arrItem[0]=_T("{[(BITMAP_PREDEF:_Action_Info.bmp)]}");
		arrItem[1]=m_CaliItems[i].szNO;
		arrItem[2]=m_CaliItems[i].szReserveTime;
		arrItem[3]=m_CaliItems[i].szComponentName;
		arrItem[4]=m_CaliItems[i].szLevel;
		arrItem[5]=m_CaliItems[i].szContents;
		arrItem[6]=m_CaliItems[i].szPeakArea;
		arrItem[7]=m_CaliItems[i].szPeakHeight;
		arrItem[8]=m_CaliItems[i].szResFactor;
		arrItem[9]=m_CaliItems[i].szIsReference;
		arrItem[10]=m_CaliItems[i].szIsInterior;
		arrItem[11]=m_CaliItems[i].szStdContent;
		arrItem[12]=m_CaliItems[i].szInteriorNo;
		arrItem[13]=m_CaliItems[i].szSampleFullName;
		arrItem[14]=m_CaliItems[i].szPeakBeginX;
		arrItem[15]=m_CaliItems[i].szPeakEndX;
		arrItem[16]=m_CaliItems[i].szSlope;
		arrItem[17]=m_CaliItems[i].szIntercept;
		arrItem[18]=m_CaliItems[i].szCoefficient;
		arrItem[19]=_T("{[(BITMAP_PREDEF:_Action_Delete.bmp)]}");

		pCIADataTable->AppendRow(arrItem);
	}

	szWatch = pCIADataTable->GetXMLString();
	return true;
}

bool CCaliData::ItemSaveToCalibrateResultTable(CIADataTable* pCIADataTable){
	CString szWatch = pCIADataTable->GetXMLString();

	pCIADataTable->DeleteAllRows();
	const int TableCols=pCIADataTable->GetColumnNumber();
	if (TableCols!=12)
	{
		::AfxMessageBox(_T("当前要保存的文件内部数据有误"));
		return false;
	}
	const int ItemNum = m_CaliItems.size();
	for (int i=0; i<ItemNum; ++i)
	{
		CStringArray arrItem;
		for (int c=0; c<TableCols; ++c)
		{
			CString str(_T(""));
			arrItem.Add(str);
		}
		arrItem[0]=_T("{[(BITMAP_PREDEF:_Action_Info.bmp)]}");
		arrItem[1]=m_CaliItems[i].szNO; 
		arrItem[2]=m_CaliItems[i].szReserveTime;
		arrItem[3]=m_CaliItems[i].szComponentName;
		arrItem[4]=m_CaliItems[i].szSlope;
		arrItem[5]=m_CaliItems[i].szIntercept;
		arrItem[6]=m_CaliItems[i].szCoefficient;
		arrItem[7] =m_CaliItems[i].szIsReference;
		arrItem[8]=m_CaliItems[i].szIsInterior;
		arrItem[9]=m_CaliItems[i].szInteriorNo;
		arrItem[10]=m_CaliItems[i].szStdContent;
		arrItem[11]=_T("{[(BITMAP_PREDEF:_Action_Delete.bmp)]}");

		pCIADataTable->AppendRow(arrItem);
	}

	szWatch = pCIADataTable->GetXMLString();
	return true;
}

bool CCaliData::GenerateTable(CIADataTable &cIADataTable, const CString& caliMethod)
{	
	//确定定制表格类型
	m_tableType = caliMethod;

	//创建以峰高显示类型的表格
	CStringArray ArrTopics, ArrTitles, ArrColWidth, ArrAlign;
	ArrTopics.Add(_T("icon_inf"));      ArrTitles.Add(_T("i"));			ArrColWidth.Add(_T("20"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("NO"));            ArrTitles.Add(_T("编号"));		ArrColWidth.Add(_T("40"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("ReserveTime"));	ArrTitles.Add(_T("保留时间"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("right|vcenter"));
	ArrTopics.Add(_T("ComponentName"));	ArrTitles.Add(_T("组份名"));		ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("Level"));			ArrTitles.Add(_T("样品级别"));	ArrColWidth.Add(_T("60"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("Contents"));		ArrTitles.Add(_T("含量"));		ArrColWidth.Add(_T("100"));	ArrAlign.Add(_T("right|vcenter"));
	
	if (m_tableType == _T("峰面积"))
	{
		ArrTopics.Add(_T("PeakArea"));		ArrTitles.Add(_T("峰面积"));		ArrColWidth.Add(_T("60"));	ArrAlign.Add(_T("right|vcenter"));
	}
	else if (m_tableType == _T("峰高"))
	{
		ArrTopics.Add(_T("PeakHeight"));	ArrTitles.Add(_T("峰高"));		ArrColWidth.Add(_T("60"));	ArrAlign.Add(_T("right|vcenter"));
	}
	else
	{
		::AfxMessageBox(_T("究竟是峰面积还是峰高？"));	
		return false;
	}
	
	ArrTopics.Add(_T("ResFactor"));		ArrTitles.Add(_T("响应因子"));	ArrColWidth.Add(_T("100"));	ArrAlign.Add(_T("right|vcenter"));
	ArrTopics.Add(_T("IsReference"));	ArrTitles.Add(_T("是否参比"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("IsInterior"));	ArrTitles.Add(_T("是否内标"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("InteriorNo"));	ArrTitles.Add(_T("内标编号"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("icon_del"));      ArrTitles.Add(_T("删"));			ArrColWidth.Add(_T("20"));	ArrAlign.Add(_T("center|vcenter"));

	//创建列信息
	if(!cIADataTable.SetColumns(ArrTopics, &ArrTitles)) 
	{
		::AfxMessageBox(_T("创建列信息失败"));
		return FALSE;
	}

	//增加列宽信息
	for (int i=0; i<ArrTitles.GetSize(); ++i)
	{
		if(!cIADataTable.SetColumnInfo(ArrTopics.GetAt(i), XMLPROP_TAB_COL_WIDTH, ArrColWidth.GetAt(i))) 
		{
			::AfxMessageBox(_T("创建列宽失败"));
			return FALSE;
		}
	}

	//增加对齐信息
	for (int i=0; i<ArrAlign.GetSize(); ++i)
	{
		if(!cIADataTable.SetColumnInfo(ArrTopics.GetAt(i), XMLPROP_TAB_COL_ALIGN, ArrAlign.GetAt(i))) 
		{
			::AfxMessageBox(_T("创建对齐方式失败"));
			return FALSE;
		}
	}

	// *********清除无用数据
	ClearUnnecessaryCell();

	//进行数据填充
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		CStringArray arrItem;
		if (!GetRowData(i, arrItem))
		{
			::AfxMessageBox(_T("未取到对应行数据"));
			return false;
		}
		cIADataTable.AppendRow(arrItem);
	}

	//CString szWatch =  cIADataTable.GetXMLString();
	return true;
}

bool CCaliData::GenerateCalibrateResultTable(CIADataTable &cIADataTable) {
	
	CStringArray ArrTopics, ArrTitles, ArrColWidth, ArrAlign;
	ArrTopics.Add(_T("icon_inf"));      ArrTitles.Add(_T("i"));			ArrColWidth.Add(_T("20"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("NO"));            ArrTitles.Add(_T("编号"));		ArrColWidth.Add(_T("40"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("ReserveTime"));	ArrTitles.Add(_T("保留时间"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("right|vcenter"));
	ArrTopics.Add(_T("ComponentName"));	ArrTitles.Add(_T("组份名"));		ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("Slope"));			ArrTitles.Add(_T("斜率"));		ArrColWidth.Add(_T("100"));	ArrAlign.Add(_T("right|vcenter"));
	ArrTopics.Add(_T("Intercept"));		ArrTitles.Add(_T("截距"));		ArrColWidth.Add(_T("100"));	ArrAlign.Add(_T("right|vcenter"));
	ArrTopics.Add(_T("Coefficient"));	ArrTitles.Add(_T("相关系数"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("right|vcenter"));
	ArrTopics.Add(_T("IsReference"));	ArrTitles.Add(_T("是否参比"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("IsInterior"));	ArrTitles.Add(_T("是否内标"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("InteriorNo"));	ArrTitles.Add(_T("内标编号"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));
	ArrTopics.Add(_T("StdContent"));	ArrTitles.Add(_T("内标含量"));	ArrColWidth.Add(_T("80"));	ArrAlign.Add(_T("center|vcenter"));

	//创建列信息
	if(!cIADataTable.SetColumns(ArrTopics, &ArrTitles)) 
	{
		::AfxMessageBox(_T("创建列信息失败"));
		return FALSE;
	}

	//增加列宽信息
	for (int i=0; i<ArrTitles.GetSize(); ++i)
	{
		if(!cIADataTable.SetColumnInfo(ArrTopics.GetAt(i), XMLPROP_TAB_COL_WIDTH, ArrColWidth.GetAt(i))) 
		{
			::AfxMessageBox(_T("创建列宽失败"));
			return FALSE;
		}
	}

	//增加对齐信息
	for (int i=0; i<ArrAlign.GetSize(); ++i)
	{
		if(!cIADataTable.SetColumnInfo(ArrTopics.GetAt(i), XMLPROP_TAB_COL_ALIGN, ArrAlign.GetAt(i))) 
		{
			::AfxMessageBox(_T("创建对齐方式失败"));
			return FALSE;
		}
	}

	//进行数据填充
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		CStringArray arrItem;
		if (!GetCaliResultRowData(i, arrItem))
		{
			::AfxMessageBox(_T("未取到对应行数据"));
			return false;
		}
		cIADataTable.AppendRow(arrItem);
	}

	//CString szWatch =  cIADataTable.GetXMLString();
	return true;
}

bool CCaliData::GetRowData(int nRow, CStringArray& strArry)//获得第i行数据
{
	if (nRow < 0)
	{
		TRACE(_T("row为负数"));
		return false;
	}

	const int itemNum = m_CaliItems.size();
	if (nRow > itemNum-1)
	{
		TRACE(_T("row超过数据个数"));
		return false;
	}

	strArry.Add(_T("{[(BITMAP_PREDEF:_Action_Info.bmp)]}"));
	strArry.Add(m_CaliItems[nRow].szNO);			//编号
	strArry.Add(m_CaliItems[nRow].szReserveTime);	//保留时间
	strArry.Add(m_CaliItems[nRow].szComponentName);	//组份名
	strArry.Add(m_CaliItems[nRow].szLevel);			//级别
	
	double dContents = _tstof(m_CaliItems[nRow].szContents);
	if (dContents == 0.000f)
	{
		m_CaliItems[nRow].szContents = _T("0.000");
	}
	else if (dContents < 0.1f)
	{
		m_CaliItems[nRow].szContents.Format(_T("%.3E"), dContents);
		EraseZeroMinus(m_CaliItems[nRow].szContents);
	}
	else
	{
		ConvertDecimalDigits(m_CaliItems[nRow].szContents, 3);
	}
	


	strArry.Add(m_CaliItems[nRow].szContents);		//含量
	//strArry.Add(_T("")); //含量用户自行输入

	if (m_tableType == _T("峰面积"))
	{
		ConvertDecimalDigits(m_CaliItems[nRow].szPeakArea, 3);
		strArry.Add(m_CaliItems[nRow].szPeakArea);		//峰面积
	}
	else if (m_tableType == _T("峰高"))
	{
		ConvertDecimalDigits(m_CaliItems[nRow].szPeakHeight, 3);
		strArry.Add(m_CaliItems[nRow].szPeakHeight);	//峰高
	}
	else
	{
		::AfxMessageBox(_T("究竟是峰面积还是峰高？？？"));
		return false;
	}

	
	double dResFactor = _tstof(m_CaliItems[nRow].szResFactor);
	if (dResFactor==0.0000f)
	{
		m_CaliItems[nRow].szResFactor = _T("0.0000");
	}
	else
	{
		m_CaliItems[nRow].szResFactor.Format(_T("%.4E"), dResFactor);
		if (dResFactor >= 1.000000f)
			EraseZeroPlus(m_CaliItems[nRow].szResFactor);
		else
			EraseZeroMinus(m_CaliItems[nRow].szResFactor);
	}
	strArry.Add(m_CaliItems[nRow].szResFactor);		//响应因子


	strArry.Add(m_CaliItems[nRow].szIsReference);	//是否参比
	strArry.Add(m_CaliItems[nRow].szIsInterior);	//是否内标
	strArry.Add(m_CaliItems[nRow].szInteriorNo);	//内标编号
	strArry.Add(_T("{[(BITMAP_PREDEF:_Action_Delete.bmp)]}"));	
	return true;
}

bool CCaliData::GetInteriorNoDate(CStringArray& strArry) {
	 
	const int itemNum = m_CaliItems.size();
	if (itemNum <= 0)
	{
		TRACE(_T("无数据"));
		return false;
	}

	for (int row=0; row<itemNum; ++row)
	{
		strArry.Add(m_CaliItems[row].szInteriorNo);
	}
	return true;
}


bool CCaliData::GetCaliResultRowData(int nRow, CStringArray& strArry) {
	if (nRow < 0) return false;

	const int itemNum = m_CaliItems.size();
	if (nRow > itemNum-1) return false;

	strArry.Add(_T("{[(BITMAP_PREDEF:_Action_Info.bmp)]}"));
	strArry.Add(m_CaliItems[nRow].szNO);			//编号
	strArry.Add(m_CaliItems[nRow].szReserveTime);	//保留时间
	strArry.Add(m_CaliItems[nRow].szComponentName);	//组份名
	strArry.Add(m_CaliItems[nRow].szSlope);			//斜率
	strArry.Add(m_CaliItems[nRow].szIntercept);		//截距
	strArry.Add(m_CaliItems[nRow].szCoefficient);	//相关系数
	strArry.Add(m_CaliItems[nRow].szIsReference);	//是否参比
	strArry.Add(m_CaliItems[nRow].szIsInterior);	//是否内标
	strArry.Add(m_CaliItems[nRow].szInteriorNo);	//内标编号
	strArry.Add(m_CaliItems[nRow].szStdContent);	//内标含量
	strArry.Add(_T("{[(BITMAP_PREDEF:_Action_Delete.bmp)]}"));	
	return true;
}

CaliItem& CCaliData::GetRowDate(int row){
	//非防御型，假定传入的row一定存在数据
	return m_CaliItems[row];
}

//**********************************************************************************
//输入，一个待验证的新进样品的保留时间
//输出，instA, instB分别表示同一组分名的行号
//**********************************************************************************
bool CCaliData::VerifyReserveTime(const CString& szReserveTime, int& match_no, int& instA, int& instB)
{
	//bool IsDefiniteIns = false; //返回是否可以插入
	double dInResTime = _tstof(szReserveTime);
	const int itemRow = m_CaliItems.size();
	instB = itemRow-1;
	for (int i=0; i<itemRow; ++i)
	{
		if (m_CaliItems[i].szNO!=_T(""))
		{
			double dResTime = _tstof(m_CaliItems[i].szReserveTime);
			double upper_RT = dResTime * (1.00000000 - 5.0/100); //0.95;
			double lower_RT = dResTime * (1.00000000 + 5.0/100); //1.05;
			if (upper_RT  <= dInResTime && dInResTime <= lower_RT)
			{//区间匹配，找同组分名的结束位置
				instA = i; //确定同组分名的开始位置

				match_no = _wtoi(m_CaliItems[i].szNO);

				for (int j=i+1; j<itemRow; ++j)
				{
					if (m_CaliItems[j].szNO!=_T(""))
					{
						instB = j - 1;
						break;
					}
				}
				if (instA!=instB) instA=instB;
				return true;
			}
		}
		
	}
	return false;
}

void CCaliData::ConvertDecimalDigits(CString& obj, int dts)
{
	double num = _tstof(obj);
	CString szFormat;
	szFormat.Format(_T("%%.%df"), dts);
	CString szConvert;
	szConvert.Format(szFormat, num);
	obj = szConvert;
}

void CCaliData::EraseZeroPlus(CString& str)  
{  
	int strLen = str.GetLength();
	int pos= str.Find(_T("E+")); 
	if (pos+5==strLen) //X.XXXe+000
		str.Delete(pos+2);
} 

void CCaliData::EraseZeroMinus(CString& str)  
{  
	int strLen = str.GetLength();
	int pos = str.Find(_T("E-00"));
	if (pos!=-1)
	{
		str.Delete(pos+2, 2);
		return;
	}
	pos = str.Find(_T("E-0"));
	if (pos!=-1)
	{
		str.Delete(pos+2);
		return;
	}
	//int pos= str.Find(_T("E-")); 
	//if (pos+5==strLen){ // E-000
	//	int next_pos = str.Find(_T("E-0"));
	//	if (next_pos!=-1)
	//		str.Delete(next_pos); 
		
} 

bool CCaliData::IsDigital(CString& str)
{
	const int strLen = str.GetLength();
	for (int i=0; i<strLen; ++i)
	{
		TCHAR tch = str.GetAt(i);
		if ((tch>='0' && tch<='9') || (tch == '.'))
			continue;
		else
			return false;
	}
	return true;
}

bool CCaliData::IsNaturalNumber(CString& str)
{
	const int strLen = str.GetLength();
	for (int i=0; i<strLen; ++i) {
		TCHAR tch = str.GetAt(i);
		if ((tch>='1' && tch<='9'))	continue;
		else	return false;
	}
	return true;
}

bool CCaliData::UpdateRowData(int iRow, const CStringArray& strArry)
{
	const int itemNum = m_CaliItems.size();
	if (iRow<0 || iRow>itemNum-1) {
		return false;
	}

	m_CaliItems[iRow].szPeakArea = strArry[4]; //峰面积
	ConvertDecimalDigits(m_CaliItems[iRow].szPeakArea, 3);

	m_CaliItems[iRow].szPeakHeight = strArry[6]; //峰高
	ConvertDecimalDigits(m_CaliItems[iRow].szPeakHeight, 3);

	m_CaliItems[iRow].szPeakBeginX     = strArry[10];
	m_CaliItems[iRow].szPeakEndX       = strArry[11];

	double dCont = _tstof(m_CaliItems[iRow].szContents);
	double dPeakArea = _tstof(m_CaliItems[iRow].szPeakArea);
	double dPeakHeight = _tstof(m_CaliItems[iRow].szPeakHeight);
	CString szFactor;

	if (m_tableType == _T("峰面积"))
	{
		double dResFactor = 0.0f / dPeakArea; // 计算响应因子
		szFactor.Format(_T("%.4e"), dResFactor);
		if (dResFactor >= 1.000000f)
			EraseZeroPlus(szFactor);
		else
			EraseZeroMinus(szFactor);
		m_CaliItems[iRow].szResFactor		= szFactor;	
	}
	else if (m_tableType == _T("峰高"))
	{
		double dResFactor = 0.0f / dPeakHeight;// 计算响应因子
		szFactor.Format(_T("%.4e"), dResFactor);
		if (dResFactor >= 1.000000f)
			EraseZeroPlus(szFactor);
		else
			EraseZeroMinus(szFactor);
		m_CaliItems[iRow].szResFactor		= szFactor;	
	}
	else
	{
		::AfxMessageBox(_T("究竟是峰面积还是峰高？"));
		return false;
	}

	return true;

}

bool CCaliData::InsertRowData(int iRow, int iStart, const CStringArray& strArry, LPCTSTR lpcszPathName)
{
	CaliItem insertCaliItem(m_CaliItems[iRow]);
	insertCaliItem.szNO				= _T("");	//编号
	insertCaliItem.szReserveTime	= _T("");	//保留时间
	insertCaliItem.szComponentName  = _T("");	//组份名 
	
	//分配新的样品级别
	insertCaliItem.szLevel			= _T("#LEVEL#");	//级别
	insertCaliItem.szContents		= _T("");//strArry[10];  //含量：用户自行输入
	ConvertDecimalDigits(insertCaliItem.szContents, 3);

	insertCaliItem.szPeakArea		= strArry[4];	//峰面积
	ConvertDecimalDigits(insertCaliItem.szPeakArea, 3);

	insertCaliItem.szPeakHeight		= strArry[6];	//峰高
	ConvertDecimalDigits(insertCaliItem.szPeakHeight, 3);

	double dCont = _tstof(insertCaliItem.szContents);
	double dPeakArea = _tstof(insertCaliItem.szPeakArea);
	double dPeakHeight = _tstof(insertCaliItem.szPeakHeight);
	CString szFactor;

	if (m_tableType == _T("峰面积"))
	{
		double dResFactor = 0.0f / dPeakArea; // 计算响应因子
		szFactor.Format(_T("%.4e"), dResFactor);
		if (dResFactor >= 1.000000f)
			EraseZeroPlus(szFactor);
		else
			EraseZeroMinus(szFactor);
		insertCaliItem.szResFactor		= szFactor;	
	}
	else if (m_tableType == _T("峰高"))
	{
		double dResFactor = 0.0f / dPeakHeight;// 计算响应因子
		szFactor.Format(_T("%.4e"), dResFactor);
		if (dResFactor >= 1.000000f)
			EraseZeroPlus(szFactor);
		else
			EraseZeroMinus(szFactor);
		insertCaliItem.szResFactor		= szFactor;	
	}
	else
	{
		::AfxMessageBox(_T("究竟是峰面积还是峰高？2"));
		return false;
	}

	insertCaliItem.szIsReference	= _T("否");	//是否参比
	insertCaliItem.szIsInterior		= _T("否");	//是否内标
	insertCaliItem.szStdContent		= _T("");	//内标含量
	insertCaliItem.szInteriorNo		= _T("");	//内标编号
	insertCaliItem.szSampleFullName = lpcszPathName;
	insertCaliItem.szPeakBeginX     = strArry[10];
	insertCaliItem.szPeakEndX       = strArry[11];

	//把保留时间更改成插入的保留时间
	m_CaliItems[iStart].szReserveTime = strArry[2];


	//开始数据插入
	vector<CaliItem> temp;
	const int itemNum = m_CaliItems.size();
	if (iRow<0 || iRow>itemNum-1)
	{
		::AfxMessageBox(_T("无法在范围外插入数据"));
		return false;
	}
	for (int i=0; i<=iRow; i++)
	{
		temp.push_back(m_CaliItems[i]);
	}
	temp.push_back(insertCaliItem);
	for (int i=iRow+1; i<itemNum; i++) 
	{
		temp.push_back(m_CaliItems[i]);
	}

	m_CaliItems.swap(temp);

	return true;
}

bool CCaliData::InsertRowData(int iRow, const CStringArray& strArry, LPCTSTR lpcszPathName)
{
	CaliItem insertCaliItem;
	CString szRow;
	szRow.Format(_T("%d"), iRow+1);
	insertCaliItem.szNO				= szRow;	//编号
	insertCaliItem.szReserveTime	= strArry[2];	//保留时间
	insertCaliItem.szComponentName  = _T("Compound")+szRow;	//组份名

	//分配新的样品级别
	insertCaliItem.szLevel			= _T("1");//级别
	insertCaliItem.szContents		= _T(""); //strArry[11];  //含量：等待用户自行输入
	insertCaliItem.szPeakArea		= strArry[4];	//峰面积
	insertCaliItem.szPeakHeight		= strArry[6];	//峰高

	double dCont = _tstof(insertCaliItem.szContents);
	double dPeakArea = _tstof(insertCaliItem.szPeakArea);
	double dPeakHeight = _tstof(insertCaliItem.szPeakHeight);
	CString szFactor;

	if (m_tableType == _T("峰面积"))
	{
		double dResFactor = 0.0f / dPeakArea;  //计算响应因子
		szFactor.Format(_T("%0.2f"), dResFactor);
		insertCaliItem.szResFactor		= szFactor;	
	}
	else if (m_tableType == _T("峰高"))
	{
		double dResFactor = 0.0 / dPeakHeight;	//计算响应因子
		szFactor.Format(_T("%0.2f"), dResFactor);
		insertCaliItem.szResFactor		= szFactor;
	}
	else
	{
		::AfxMessageBox(_T("究竟是峰面积还是峰高？2"));
		return false;
	}

	insertCaliItem.szIsReference	= _T("否");	//是否参比
	insertCaliItem.szIsInterior		= _T("否");	//是否内标
	insertCaliItem.szStdContent		= _T("");	//内标含量
	insertCaliItem.szInteriorNo		= _T("");	//内标编号
	insertCaliItem.szSampleFullName = lpcszPathName;
	insertCaliItem.szPeakBeginX     = strArry[10];
	insertCaliItem.szPeakEndX       = strArry[11];

	m_CaliItems.push_back(insertCaliItem);
	return true;
}

bool CCaliData::GetSampleLevel(int& iValue)
{
	const int itemNum = m_CaliItems.size();
	int maxLv = 0;
	for (int i=0; i<itemNum; ++i)
	{
		CString szLevel = m_CaliItems[i].szLevel;
		
		int nLv = _wtoi(szLevel);
		if (nLv> maxLv)
		{
			maxLv = nLv;
		}
	}

	iValue = maxLv+1;
	return true;
}

bool CCaliData::ModifyInsertItemLevel()
{
	int iLevel=0;
	if(!GetSampleLevel(iLevel))
	{
		::AfxMessageBox(_T("无法自动分配样品级别"));
		return false;
	}

	CString szLv;
	szLv.Format(_T("%d"), iLevel);

	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		CString szLevel = m_CaliItems[i].szLevel;
		if (szLevel == _T("#LEVEL#"))
		{
			m_CaliItems[i].szLevel = szLv;
		}
	}
	return true;
}

bool CCaliData::GetSampleFullPath(CStringArray& strArry)
{
	if(!strArry.IsEmpty()) strArry.RemoveAll();

	const int itemNum = m_CaliItems.size();
	std::set<CString> sampleFullPath;
	for (int i=0; i<itemNum; i++)
	{
		sampleFullPath.insert(m_CaliItems[i].szSampleFullName);
	}
	//遍历SampleNames容器后放入deque<SamleFileInfo> m_InFileInfo
	typedef std::set<CString>::iterator SetIter;
	for (SetIter iter=sampleFullPath.begin(); iter!=sampleFullPath.end(); ++iter)
	{
		CString szPath = *iter;
		strArry.Add(szPath);
	}
	return true;
}

bool CCaliData::GetSampleFullPath(int row, CString& strVal)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return false;
	strVal = m_CaliItems[row].szSampleFullName;
	return true;
}

void CCaliData::SubOne(vector<int> &vec, int i)
{
	int itemNum = vec.size();
	for (int j=0; j<itemNum; ++j)
	{
		if (i == vec[j])
		{
			vec[j]--;
		}
	}
}

bool CCaliData::CheakCopyInfo(int iRow)
{
	int itemNum = m_CaliItems.size();
	if (iRow+1 >= itemNum) return false;

	CString strLevel = m_CaliItems[iRow].szLevel;
	int iLevel = _wtoi(strLevel);
	 

	CString strName = m_CaliItems[iRow+1].szComponentName;
	if (strName == _T("")) //说明需要复制
	{
		m_CaliItems[iRow+1].szNO = m_CaliItems[iRow].szNO;
		m_CaliItems[iRow+1].szReserveTime = m_CaliItems[iRow].szReserveTime;		//保留时间
		m_CaliItems[iRow+1].szComponentName = m_CaliItems[iRow].szComponentName;	//组份名
		m_CaliItems[iRow+1].szIsInterior = m_CaliItems[iRow].szIsInterior;	 
		m_CaliItems[iRow+1].szIsReference = m_CaliItems[iRow].szIsReference;
		m_CaliItems[iRow+1].szStdContent = m_CaliItems[iRow].szStdContent; // 内标含量
		m_CaliItems[iRow+1].szInteriorNo = m_CaliItems[iRow].szInteriorNo; // 内标编号
	}
	return true;
}

bool CCaliData::DeleteRowData(CString& szPath)
{
	for (size_t i=0; i<m_CaliItems.size(); ++i)
	{
		if (m_CaliItems[i].szSampleFullName==szPath)
		{
			DeleteRowData(i);
			i = -1; //重新开始循环
		}
	}
	return true;
}

bool CCaliData::DeleteRowData(int iRow)
{
	//是否删除最后一行
	if (m_CaliItems.size()==1)
	{
		if (iRow==0)
		{
			m_CaliItems.clear();
			return true;
		}
	}

	//检查删除的行是否是某组份的首行
	CheakCopyInfo(iRow);

	vector<int> vecCL; //组份级别，待重新排序
	vector<CaliItem> tmpItem;
	int itemNum = m_CaliItems.size();
	int max_level = INT_MIN;
	int min_level = 1;//INT_MAX;
	for (int i = 0; i<itemNum; ++i)
	{
		if (i==iRow) //跳过即将删除数据
		{
			continue;
		}

		CString strLevel = m_CaliItems[i].szLevel;
		//？是否需要增加对字符串is数字的检查？
		int iLevel = _wtoi(strLevel);
		if (iLevel == 0) ASSERT(TRUE); //级别不可能为零
		
		if (iLevel > max_level) max_level = iLevel;
		
		vecCL.push_back(iLevel);
		tmpItem.push_back(m_CaliItems[i]);
	}
	
	//重新排序
	if (max_level == INT_MIN/* || min_level == INT_MAX*/)
	{
		return false;
	}

	int vecCLSize = vecCL.size();
	for (int i=min_level; i<=max_level; ++i)
	{
		for (int j=0; j<vecCLSize; ++j)
		{
			if (i == vecCL[j])
			{
				break;
			}
			if (j== vecCLSize-1)
			{
				int subObj = i+1;
				SubOne(vecCL, subObj);
			}
		}
	}

	//重新赋值
	int tempItemNum = tmpItem.size();
	if (tempItemNum != vecCL.size())
	{
		return false;
	}
	for (int i=0; i<tempItemNum; ++i)
	{
		int iLevel = vecCL[i];
		CString strLevel;
		strLevel.Format(_T("%d"), iLevel);
		tmpItem[i].szLevel = strLevel;
	}

	tmpItem.swap(m_CaliItems);

	return true;
}

bool CCaliData::DeleteNotFirstRow() {
	vector<CaliItem> tmpItem;
	int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if (m_CaliItems[i].szComponentName!=_T(""))
			tmpItem.push_back(m_CaliItems[i]);
	}
	tmpItem.swap(m_CaliItems);
	return true;
}

void CCaliData::VerifyContent(){
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if (!VertifyIsInterior(i)) //非内标，清除用户输入的内标含量
			ClearContent(i);
	}
}

bool CCaliData::ModifyContent(int nRow, CString& szContent)
{
	const int itemNum = m_CaliItems.size();
	if (nRow<0 || nRow>itemNum-1)
		return false;
	
	m_CaliItems[nRow].szStdContent = szContent;
	return true;
}

bool CCaliData::ClearContent(int nRow)
{
	const int itemNum = m_CaliItems.size();
	if (nRow<0 || nRow>itemNum-1)
		return false;

	m_CaliItems[nRow].szStdContent = _T("");
	return true;
}

bool CCaliData::GetPreContent(int nRow, CString& szContent)
{
	const int itemNum = m_CaliItems.size();
	if (nRow<0 || nRow>itemNum-1)
	{
		return false;
	}

	szContent = m_CaliItems[nRow].szStdContent;
	return true;
}

void CCaliData::CaliTable2CaliResultTable(const CCaliData& CaliTable){
	CCaliData temp;
	CaliTable.GenerateDuplicate(temp);
	
	const int tempSize = temp.GetItemSize();
	const int caliItemsSize = GetItemSize();

	int currSize =  (tempSize > caliItemsSize) ? caliItemsSize : tempSize;

	for (int i=0; i<currSize; ++i) {
		temp.m_CaliItems[i].szStdContent = m_CaliItems[i].szStdContent;
	}
	
	temp.m_CaliItems.swap(m_CaliItems);
}

//内标编号相关函数
bool CCaliData::AddInteriorNo(int nRow)
{
	//边界检测
	const int itemNum = m_CaliItems.size();
	if (nRow<0 || nRow>itemNum-1)
		return false;

	//判断当前数据中是否已存在内标编号
	bool bIsExist = IsExistInteriorNo();
	if (!bIsExist)
	{//不存在
		return AllInteriorNoBe1();
	}
	//存在
	////判断之前是否已经标记成内标
	//CString strItemNo = m_CaliItems[nRow].szInteriorNo;
	//if(strItemNo!=_T("1") && strItemNo!=_T(""))
	//	return true; //不做改变
	//else{
	//	if(IsOnlyInteriorNo1(nRow))
	//		return true;
	//}
			

	//找出合理的内标编号
	int no = AutoFindInteriorNo();
	if (no == -1)
	{
		::AfxMessageBox(_T("系统仅支持3个内标"));
		return false;
	}
	CString szNo;
	szNo.Format(_T("%d"), no);
	m_CaliItems[nRow].szInteriorNo = szNo;

	return true;
}

bool CCaliData::IsOnlyInteriorNo1(int row)
{//测试当前行是否是唯一内标编号为1的
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szComponentName!=_T(""))
		{
			if (m_CaliItems[i].szInteriorNo == _T("1"))
			{
				if (i!=row)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool CCaliData::IsExistInteriorNo()
{//检测数据中是否已存在内标编号
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szInteriorNo!=_T(""))
			return true;
	}
	return false;
}

bool CCaliData::AllInteriorNoBe1()
{//每个组份的内标编号为1
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szComponentName!=_T(""))
		{
			m_CaliItems[i].szInteriorNo = _T("1");
		}
	}
	return true;
}

int  CCaliData::AutoFindInteriorNo()
{//找出合理的内标编号
	int no=1;
	set<int> set_No;
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szComponentName!=_T(""))
		{
			int itemNo = _wtoi(m_CaliItems[i].szInteriorNo);
			if (itemNo!=0)
				set_No.insert(itemNo);
		}
	}

	if (set_No.size()>=3)
	{
		return -1; //目前仅支持3个内标
	}

	typedef set<int>::iterator SETITER;
	for (SETITER iter = set_No.begin(); iter!=set_No.end(); ++iter)
	{
		if (*iter==no)
		{
			no++;
		}
	}
	return no;
}

int CCaliData::FindMinInteriorNo() 
{
	int min_no = 4; //因为目前最大内标不会超过3
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szInteriorNo!=_T(""))
		{
			int itemNo = _wtoi(m_CaliItems[i].szInteriorNo);
			if (itemNo <= min_no)
				min_no = itemNo;
		}
	}
	return min_no;
}

bool CCaliData::IsInteriorNoOnlyOne()
{
	set<int> set_No;
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szIsInterior!=_T(""))
		{
			int itemNo = _wtoi(m_CaliItems[i].szInteriorNo);
			set_No.insert(itemNo);
		}
	}
	if (set_No.size()!=1)
	{
		return false;
	}
	return true;
}

bool CCaliData::CheckExistInteriorNo(int row, const CString& strVal)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return false;
	//CString target = m_CaliItems[row].szInteriorNo; //找到源的内标编号
	for (int i=0; i<itemNum; ++i)
	{
		if (m_CaliItems[i].szIsInterior==_T("是"))
		{
			if (i!=row)
			{
				if (m_CaliItems[i].szInteriorNo==strVal)
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool CCaliData::ChangeSameCompnentInteriorNo(int row, const CString& dest)
{//改变同内标编号的所有组份
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
		return false;

	CString sour = m_CaliItems[row].szInteriorNo; //找到源的内标编号
	//遍历修改
	for (int i=0; i<itemNum; ++i)
	{
		if (m_CaliItems[i].szInteriorNo==sour)
			m_CaliItems[i].szInteriorNo = dest;
	}
	return true;

}

bool CCaliData::VertifyInteriorNoInterval(const CString& strNo)
{//测试strNo是否在当前内标编号范围内【非数值范围，是数字匹配】
	set<int> set_No;
	int no = _wtoi(strNo);
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		int itemNo = _wtoi(m_CaliItems[i].szInteriorNo);
		if (itemNo!=0)
			set_No.insert(itemNo);
	}
	typedef set<int>::iterator SETITER;
	for (SETITER iter = set_No.begin(); iter!=set_No.end(); ++iter)
	{
		if (*iter==no)
			return true;
	}
	return false;
}

bool CCaliData::ManualInteriorNo(int col, int row, LPCTSTR szVal)
{
	CString strVal(szVal); 
	//***判断输入数据是否是自然数
	if(!IsNaturalNumber(strVal)) {
		::AfxMessageBox(_T("内标编号必须为大于0的自然数"));
		return false;
	}

	//***判断该单元格对应是否已指定为内标
	if(VertifyIsInterior(row)) {
	//是内标
		if (strVal==_T("")) {
			::AfxMessageBox(_T("将内标组分的编号删除为空"));
			return false;
		}
		//检查当前改变的编号是否跟已存在的编号冲突
		if(!CheckExistInteriorNo(row, strVal)) {
			::AfxMessageBox(_T("修改内标编号与已有的内标编号重复"));
			return false;
		}

		//改变同内标编号的所有组份
		ChangeSameCompnentInteriorNo(row, strVal);
	}
	else {
	//不是内标
		//检查手动修改的内标的范围，不能超出当前已有的范围
		if(VertifyInteriorNoInterval(strVal)) {
		//在范围内可以直接修改
			m_CaliItems[row].szInteriorNo = strVal;
		}
		else //超出范围，不能修改
		{
			::AfxMessageBox(_T("将修改的内标编号超出当前范围"));
			return false;
		}
	}
	return true;
}

void CCaliData::AutoCheckInteriorNoAfterDel()
{
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		bool needDel = true;
		if (m_CaliItems[i].szIsInterior==_T("否")) {
			CString cStrINO = m_CaliItems[i].szInteriorNo;
			for (int j=0; j<itemNum; ++j) {
				if (m_CaliItems[j].szIsInterior==_T("是")) {
					if (cStrINO==m_CaliItems[j].szInteriorNo)
					{
						needDel = false;
					}
				}
			}
			if (needDel)
			{
				m_CaliItems[i].szInteriorNo = _T("");
			}
		}
	}
}

void CCaliData::AutoReorderID() {
	int reorderId = 1;
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if (m_CaliItems[i].szNO!=_T("")) {
			CString cStrReorderID;
			cStrReorderID.Format(_T("%d"), reorderId);
			m_CaliItems[i].szNO = cStrReorderID;
			reorderId++;
		}
	}
}

bool CCaliData::DesignatedReference(int row, bool bIsReference)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
		return false;

	if (bIsReference)
		m_CaliItems[row].szIsReference = _T("是");
	else
		m_CaliItems[row].szIsReference = _T("否");
	return true;
}

bool CCaliData::DesignatedInterior(int row, bool bIsInterior)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
	{
		return false;
	}
	if (bIsInterior)
		m_CaliItems[row].szIsInterior = _T("是");
	else
		m_CaliItems[row].szIsInterior = _T("否");
	
	return true;
}

bool CCaliData::SyncDelInterior(int row)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return false;
	if (m_CaliItems[row].szIsInterior==_T("否")) return false;

	CString cStrNo = m_CaliItems[row].szInteriorNo;
	for (int i=0; i<itemNum; ++i)
	{
		if (m_CaliItems[i].szInteriorNo==cStrNo)
			m_CaliItems[i].szInteriorNo = _T("");
	}
	return true;
}

void CCaliData::ClearAllInteriorNo()
{
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if(m_CaliItems[i].szInteriorNo!=_T(""))
		{
			m_CaliItems[i].szInteriorNo=_T("");
		}
	}
}

bool CCaliData::SynEditedCell(int col, int row, LPCTSTR szVal)
{
	if (szVal == _T("")) { 
		//::AfxMessageBox(_T("组份名不允许为空"));
		return false;
	}

	if (col == 5){
		CString cStr_tmp;
		cStr_tmp.Format(_T("%s"), szVal);
		if (!IsDigital(cStr_tmp))
			return false;
	}

	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
	{
		return false;
	}

	if (col==3)
	{
		if (m_CaliItems[row].szComponentName != _T("")) {
			m_CaliItems[row].szComponentName = szVal;
		}
		else{
			//::AfxMessageBox(_T("不允许在此单元格修改数据"));
			return false;
		}
	}
	else if (col==5)
	{
		CString szTmp;
		szTmp.Format(_T("%s"), szVal);
		//double dContents = _tstof(szTmp);
		//
		//if (dContents < 0.1f)
		//{
		//	szTmp.Format(_T("%e"), dContents);
		//}
		//else
		//{
		//	ConvertDecimalDigits(szTmp, 3); // 强制保留小数位后3位
		//}
		m_CaliItems[row].szContents = szTmp;

	    // 重新计算[响应因子]
		double dCont       = _tstof(m_CaliItems[row].szContents);
		double dPeakArea   = _tstof(m_CaliItems[row].szPeakArea);
		double dPeakHeight = _tstof(m_CaliItems[row].szPeakHeight);
		CString szFactor;
		if (m_tableType == _T("峰面积"))
		{
			double dResFactor = dCont / dPeakArea;
			m_CaliItems[row].szResFactor.Format(_T("%.4e"), dResFactor);

			//if (dResFactor == 0.0000f)
			//{
			//	m_CaliItems[row].szResFactor = _T("0.0000");
			//}
			//else
			//{
			//	szFactor.Format(_T("%.4e"), dResFactor);
			//	if (dResFactor >= 1.000000f)
			//		EraseZeroPlus(szFactor);
			//	else
			//		EraseZeroMinus(szFactor);
			//	m_CaliItems[row].szResFactor		= szFactor;	// 响应因子
			//}

		}
		else if (m_tableType == _T("峰高"))
		{
			double dResFactor = dCont / dPeakHeight;
			m_CaliItems[row].szResFactor.Format(_T("%.4e"), dResFactor);

			//if (dResFactor == 0.0000f)
			//{
			//	m_CaliItems[row].szResFactor = _T("0.0000");
			//}
			//else
			//{
			//	szFactor.Format(_T("%.4e"), dResFactor);
			//	if (dResFactor >= 1.000000f)
			//		EraseZeroPlus(szFactor);
			//	else
			//		EraseZeroMinus(szFactor);
			//	m_CaliItems[row].szResFactor		= szFactor;	// 响应因子
			//}
		}
		else
		{
			//::AfxMessageBox(_T("究竟是峰面积还是峰高？2"));
			return false;
		}
	}
	else
	{
		//::AfxMessageBox(_T("行数出错！")); // 理论上不会运行到这里
		return false;
	}

	return true;
}

bool CCaliData::VertifyFirstLine(int row)
{
	ClearUnnecessaryCell(row);//清除非一组组份首行的其他单元格内数据

	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
		return false;

	if (m_CaliItems[row].szComponentName != _T(""))
		return true;
	
	return false;
}

bool CCaliData::VertifyIsReference(int row)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
		return false;
	if (m_CaliItems[row].szIsReference==_T("是"))
		return true;
	return false;
}

bool CCaliData::VertifyIsInterior(int row)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
		return false;
	if (m_CaliItems[row].szIsInterior==_T("是"))
		return true;
	return false;
}

void CCaliData::ClearUnnecessaryCell()
{//清除非一组组份首行的其他单元格内数据[编号][保留时间][组分名][是否参比][是否内标][内标编号]
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		if (m_CaliItems[i].szComponentName == _T("")) {
			m_CaliItems[i].szNO			 = _T("");
			m_CaliItems[i].szReserveTime = _T("");
			m_CaliItems[i].szIsReference = _T("");
			m_CaliItems[i].szIsInterior	 = _T("");
			m_CaliItems[i].szInteriorNo  = _T(""); 
		}
	}
}

void CCaliData::ClearUnnecessaryCell(int row)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1)
		return ;
	if (m_CaliItems[row].szComponentName == _T(""))
	{
		m_CaliItems[row].szNO			 = _T("");
		m_CaliItems[row].szReserveTime   = _T("");
		m_CaliItems[row].szIsReference   = _T("");
		m_CaliItems[row].szIsInterior	 = _T("");
		m_CaliItems[row].szInteriorNo    = _T("");
	}
}

void CCaliData::UnifyDecimalDigits()
{
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i)
	{
		CString szFactor;
		double dCont       = _tstof(m_CaliItems[i].szContents);
		double dPeakArea   = _tstof(m_CaliItems[i].szPeakArea);
		double dPeakHeight = _tstof(m_CaliItems[i].szPeakHeight);

		ConvertDecimalDigits(m_CaliItems[i].szContents, 3); //含量
		ConvertDecimalDigits(m_CaliItems[i].szPeakArea, 3); //峰面积
		ConvertDecimalDigits(m_CaliItems[i].szPeakHeight, 3); //峰高

		if (m_tableType == _T("峰面积"))
		{
			double dResFactor = dCont / dPeakArea;
			szFactor.Format(_T("%.4e"), dResFactor);
			if (dResFactor >= 1.000000f)
				EraseZeroPlus(szFactor);
			else
				EraseZeroMinus(szFactor);
			m_CaliItems[i].szResFactor		= szFactor;	// 响应因子
		}
		else if (m_tableType == _T("峰高"))
		{
			double dResFactor = dCont / dPeakHeight;
			szFactor.Format(_T("%.4e"), dResFactor);
			if (dResFactor >= 1.000000f)
				EraseZeroPlus(szFactor);
			else
				EraseZeroMinus(szFactor);
			m_CaliItems[i].szResFactor		= szFactor;	// 响应因子
		}
		else
		{
			::AfxMessageBox(_T("究竟是峰面积还是峰高??"));
			return;
		}
	}
}

void CCaliData::GetComponentName(int row, CString& strName)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return;
	while (row!=-1)
	{
		if (m_CaliItems[row].szComponentName!=_T(""))
		{
			strName = m_CaliItems[row].szComponentName;
			break;
		}
		else
			row--;
	}
}

void CCaliData::GetReserveTime(int row, CString& strVal)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return;
	while (row!=-1)
	{
		if (m_CaliItems[row].szReserveTime!=_T(""))
		{
			strVal = m_CaliItems[row].szReserveTime;
			if (!IsDigital(strVal))
				Write_Error("保留时间非数字字符串");
			break;
		}
		else
			row--;
	}
}

void CCaliData::GetOpEdXTime(int row, double &dOp, double& dEd)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return;
	dOp =  _tstof(m_CaliItems[row].szPeakBeginX);
	dEd =  _tstof(m_CaliItems[row].szPeakEndX);
}

bool CCaliData::GetComponentIndex(int row, int& b, int& e)
{
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return false;
	for (int i=row; i>=0; --i)
	{
		if (m_CaliItems[i].szComponentName!=_T(""))
		{
			b = i;
			break;
		}
	}
	if (b+1==itemNum)
	{
		e = b;
		return true;
	}
	for (int i=b+1; i<itemNum; ++i)
	{
		if (m_CaliItems[i].szComponentName!=_T(""))
		{
			e = i-1;
			break;
		}

		if (i==itemNum-1)
		{
			e=itemNum-1;
			break;
		}

		else
		{
			e = b;
		}
	}
	return true;
}

bool CCaliData::GetCaliCurveData_TypeA(int row, deque<CaliCurveData>& data) {
	int b=0,e=-1;
	GetComponentIndex(row, b, e);
	if (b > e) return false;
	for (int row=b; row<=e; ++row)
	{
		if (m_CaliItems[row].szContents==_T("")){
			data.clear();
			return false;
		}
		double dContents = _tstof(m_CaliItems[row].szContents);
		if (dContents==0.0000f) continue;

		if (m_tableType==_T("峰面积")) {
			double dPeakArea = _tstof(m_CaliItems[row].szPeakArea);
			CaliCurveData ccd(dContents, dPeakArea);                 
			data.push_back(ccd);
		}
		else if (m_tableType==_T("峰高")) {
			double dPeakHeight = _tstof(m_CaliItems[row].szPeakHeight);   
			CaliCurveData ccd(dContents, dPeakHeight);                 
			data.push_back(ccd);
		}
		else 
			return false;
	}
	return true;
}

bool CCaliData::GetCaliCurveData_TypeB(int row, deque<CaliCurveData>& data) {
                                                         //need refactoring
	//判断所有内标属性内是否存至少1个内标，否则无显示
	bool hasInterior=false;
	const int itemSize = m_CaliItems.size();
	for (int i=0; i<itemSize; ++i) {
		if (m_CaliItems[i].szIsInterior==_T("是")){
			hasInterior=true;
			break;
		}
	}
	if (!hasInterior) return false;

	
	//判断选中组份是否标记为内标
	int b=0,e=-1;
	GetComponentIndex(row, b, e);
	if (b > e) return false;
	//校正区域显示"无效校正曲线"或直接无显示
	if (m_CaliItems[b].szIsInterior==_T("是")) return false;
	//选中组份无内标编号的，不参与计算校正方程	
	if (m_CaliItems[b].szInteriorNo==_T(""))   return false;


	//那么找到同内标编号且同级别的内标
	CString cStrNo = m_CaliItems[b].szInteriorNo;
	const int itemNum = m_CaliItems.size();
	int bb=0,ee=0;
	for (int i=0; i<itemNum; ++i)
	{
		if (i>=b && i<=e) continue;
		if (m_CaliItems[i].szInteriorNo == cStrNo)
		{
			if (m_CaliItems[i].szIsInterior==_T("是"))
			{
				GetComponentIndex(i, bb, ee);
				break;
			}
		}
	}

	for (int row=b; row<=e; ++row)
	{
		CString cStrLv = m_CaliItems[row].szLevel;
		for (int irow = bb; irow<=ee; irow++)
		{
			if (m_CaliItems[irow].szLevel == cStrLv) //全匹配
			{
				if (m_CaliItems[row].szContents==_T("")||m_CaliItems[irow].szContents==_T("")){
					data.clear();
					return false;
				}
				double dContents_c = _tstof(m_CaliItems[row].szContents);
				double dContents_i = _tstof(m_CaliItems[irow].szContents);
				if (dContents_i==0.0000f) continue;
				double _x = dContents_c/dContents_i;
				double _y;
				if (m_tableType==_T("峰面积")) {
					double dPeakArea_c = _tstof(m_CaliItems[row].szPeakArea); 
					double dPeakArea_i = _tstof(m_CaliItems[irow].szPeakArea); 
					if (dPeakArea_i==0.0000f) continue;
					_y = dPeakArea_c/dPeakArea_i;
				} else if(m_tableType==_T("峰高")) {
					double dPeakHeight_c = _tstof(m_CaliItems[row].szPeakHeight); 
					double dPeakHeight_i = _tstof(m_CaliItems[irow].szPeakHeight); 
					if (dPeakHeight_i==0.0000f) continue;
					_y = dPeakHeight_c/dPeakHeight_i;
				} else {
					return false;
				}
				if (_x==0.0000f) continue;

				if (_x!=1.0000f || _y!=1.0000f) { //这里把原先&& 改成||
					CaliCurveData ccd(_x, _y);
					data.push_back(ccd);
				}
			}
		} 
	}
	return true;
}

bool CCaliData::SetSlope_Intercept_Coefficient(int row, double s, double i, double c) {
	const int itemNum = m_CaliItems.size();
	if (row<0 || row>itemNum-1) return false;

	//if (s==i==c==0.0f)
	//{
	//	m_CaliItems[row].szSlope = _T("");
	//	m_CaliItems[row].szIntercept = _T("");
	//	m_CaliItems[row].szCoefficient = _T("");
	//}

	
	CString cStrSlope, cStrIntercept, cStrCoefficient;
	cStrSlope.Format(SLOPE_DATA_FORMAT, s);
	cStrIntercept.Format(INTERCEPT_DATA_FORMAT, i);
	cStrCoefficient.Format(COEFFICIENT_DATA_FORMAT, c);

	m_CaliItems[row].szSlope = cStrSlope;
	m_CaliItems[row].szIntercept = cStrIntercept;
	m_CaliItems[row].szCoefficient = cStrCoefficient;

	return true;
}

void CCaliData::AddData(const CaliItem& item){
	this->m_CaliItems.push_back(item);
}

void CCaliData::GenerateDuplicate(CCaliData& data)const {
	if (data.GetItemSize()==0)	data.ClearAllData();

	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) 
		data.AddData(m_CaliItems[i]);
}

void CCaliData::GetRefPeakData(CCaliData& data){
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if(m_CaliItems[i].szIsReference == _T("是"))
			data.AddData(m_CaliItems[i]);
	}
}

void CCaliData::CancelRefPeakMark(int no){
	int find_no=0;
	for (int i=0; i<GetItemSize(); i++)
	{
		if (m_CaliItems[i].szIsReference == _T("是"))
		{
			if (find_no==no)
			{
				m_CaliItems[i].szIsReference = _T("否");
			}
		}
	}
}

void CCaliData::CancelIntPeakMark(int no){
	int find_no=0;
	for (int i=0; i<GetItemSize(); i++)
	{
		if (m_CaliItems[i].szIsInterior == _T("是"))
		{
			if (find_no==no)
			{
				m_CaliItems[i].szIsInterior = _T("否");
			}
		}
	}
}

void CCaliData::GetIntPeakData(CCaliData& data){
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if(m_CaliItems[i].szIsInterior == _T("是"))
			data.AddData(m_CaliItems[i]);
	}
}

void CCaliData::GetcOtherPeakData(CCaliData& data){
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if(m_CaliItems[i].szIsReference == _T("否"))
			if(m_CaliItems[i].szIsInterior == _T("否"))
				data.AddData(m_CaliItems[i]);
	}
}

void CCaliData::ModifyResTime(vector<double> &afterResTime) {
	if (afterResTime.empty()) return;

	const int itemNum = m_CaliItems.size();

	CCaliData RefPeak;
	GetRefPeakData(RefPeak);

	if (RefPeak.GetItemSize()!=afterResTime.size())
	{
		::AfxMessageBox(_T("参考峰个数不匹配"));
		ASSERT(TRUE);
		return;
	}

	for (int i=0; i<itemNum; ++i) {
		if(m_CaliItems[i].szIsReference != _T("否")) //该字段应该有是，否，空 3个属性
			continue;
		int iStart=0, iEnd=INT_MAX; //该未知峰对应的参数峰的区间

		GetRefPeakInterval(i, iStart, iEnd);

		CString Ref_i_1_before;
		CString Ref_i_before;
		CString NofRef = m_CaliItems[i].szReserveTime;

		double  dRef_i_1_after;
		double  dRef_i_after;
		double  dRef_i_1_before;
		double  dRef_i_before;
		double  dNofRef_before = _tstof(NofRef);

		if (iStart==0){
			dRef_i_1_before=0.0f;
			dRef_i_1_after=0.0f;
		}
		else {
			RefPeak.GetReserveTime(iStart-1, Ref_i_1_before);
			dRef_i_1_before=_tstof(Ref_i_1_before);
			dRef_i_1_after = afterResTime[iStart-1];
		}
		

		if (iEnd==INT_MAX){
			dRef_i_before=0.0f;
			dRef_i_after =0.0f;
		}
		else{
			RefPeak.GetReserveTime(iEnd-1, Ref_i_before);
			dRef_i_before=_tstof(Ref_i_before);
			dRef_i_after =afterResTime[iEnd-1];
		}
		

		if (dRef_i_before-dRef_i_1_before==0.0f)
		{
			::AfxMessageBox(_T("参考峰前后数据一致，请进行数据检查"));
			continue;
		}
		double  dNofRef_after = (dNofRef_before-dRef_i_1_before)*(dRef_i_after-dRef_i_1_after)/(dRef_i_before-dRef_i_1_before)+dRef_i_1_after;
		CString szModifyResTime;
		szModifyResTime.Format(RESERVETIME_DATA_FORMAT, dNofRef_after);
		m_CaliItems[i].szReserveTime=szModifyResTime;
	}
}

void CCaliData::GetRefPeakInterval(int n, int& before, int& after) {
	const int itemNum = m_CaliItems.size();
	for (int i=n-1; i>=0; --i) {
		if(m_CaliItems[i].szIsReference == _T("是")) {
			before = Row2No(i); 
			break;
		}
	}

	for (int i=n+1; i<itemNum; ++i) {
		if(m_CaliItems[i].szIsReference == _T("是")) {
			after = Row2No(i); 
			break;
		}
	}
}

int CCaliData::Row2No(int row) {
	int result=0;
	const int itemNum = m_CaliItems.size();
	for (int i=0; i<itemNum; ++i) {
		if(m_CaliItems[i].szIsReference == _T("是")) {
			result++;
			if (i==row)
				return result;
		}
	}

	return 0;
}




const CStringArray& CCaliMethod::StringIDs()
{
	static CStringArray sa;
	if(sa.IsEmpty())
	{
		sa.Add(XMLVALUE_CONST_c_percentage);
		sa.Add(XMLVALUE_CONST_c_normalization);
		sa.Add(XMLVALUE_CONST_c_internal);
		sa.Add(XMLVALUE_CONST_c_external);
		sa.Add(XMLVALUE_CONST_c_internal_percentage);
		sa.Add(XMLVALUE_CONST_c_external_percentage);
	}
	return sa;
}
const CStringArray& CCaliMethod::StringNames()
{
	static CStringArray sa;
	if(sa.IsEmpty())
	{
		sa.Add(_T("百分比法"));
		sa.Add(_T("归一化法"));
		sa.Add(_T("内标法"));
		sa.Add(_T("外标法"));
		sa.Add(_T("内标百分比法"));
		sa.Add(_T("外标百分比法"));
	}
	return sa;
}
int CCaliMethod::IndexByStringID(const CString& id)
{
	const CStringArray& sa = StringIDs();
	for(int i=0; i<sa.GetCount(); ++i)
	{
		if(sa.GetAt(i) == id) return i;
	}
	return -1;
}
const CString& CCaliMethod::StringIDByIndex(int index)
{
	static CString e;
	const CStringArray& sa = StringIDs();
	if(index < 0 || index >= sa.GetCount()) return e;
	return sa.GetAt(index);
}
const CString* CCaliMethod::NameByID(const CString& id)
{
	int i = IndexByStringID(id);
	if(i < 0) return NULL;
	return &StringNames().GetAt(i);
}

const CStringArray& CCaliMethodOn::StringIDs()
{
	static CStringArray sa;
	if(sa.IsEmpty())
	{
		sa.Add(XMLVALUE_CONSTENT_c_peak_area);
		sa.Add(XMLVALUE_CONSTENT_c_peak_height);
	}
	return sa;
}
const CStringArray& CCaliMethodOn::StringNames()
{
	static CStringArray sa;
	if(sa.IsEmpty())
	{
		sa.Add(_T("峰面积"));
		sa.Add(_T("峰高"));
	}
	return sa;
}
int CCaliMethodOn::IndexByStringID(const CString& id)
{
	const CStringArray& sa = StringIDs();
	for(int i=0; i<sa.GetCount(); ++i)
	{
		if(sa.GetAt(i) == id) return i;
	}
	return -1;
}
const CString& CCaliMethodOn::StringIDByIndex(int index)
{
	static CString e;
	const CStringArray& sa = StringIDs();
	if(index < 0 || index >= sa.GetCount()) return e;
	return sa.GetAt(index);
}
const CString* CCaliMethodOn::NameByID(const CString& id)
{
	int i = IndexByStringID(id);
	if(i < 0) return NULL;
	return &StringNames().GetAt(i);
}

void CCaliUI::InitComboBox(CComboBox* pCB, const CStringArray* pSA)
{
	if(!pCB) return;
	pCB->ResetContent();
	if(pSA)
	{
		for(int i=0; i<pSA->GetCount(); ++i)
		{
			pCB->AddString(pSA->GetAt(i));
		}
	}
}