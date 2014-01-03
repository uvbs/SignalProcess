#include "StdAfx.h"
#include "PeakSeak.h"
#include "WaveletTransform.h"
#include <fstream>
   
CPeakSeak::CPeakSeak(void)
{
}
CPeakSeak::~CPeakSeak(void)
{

}
 
VOID CPeakSeak::GetPeakResult(std::vector<CPeak> &Peak)
{	
	Peak = m_Peak;
}
VOID CPeakSeak::SetSemaDate(CString filePathName)
{
	CSheet csvSheet;  
	std::ifstream file(filePathName);  //打开csv文件  
	csvSheet.loadFrom(file);  //加载至CSheet  //MessageBox(_T("获取第1，2列数据"));

	//读取时间数据
	const int dataSize = csvSheet.getRowCount();
	double *pX = new double[dataSize];
	double *pY = new double[dataSize];

	std::vector<double> time, val; //暂存的时间和信号量
	for (int i=0; i<csvSheet.getRowCount(); ++i)  
	{  
		CRow row = csvSheet.getRow(i);  //获取指定行  
		CString s = row.getColumn(0);	//获取指定列  
		double t = wcstod(s, NULL);
		time.push_back(t);
		pX[i] = t; 
	}  

	for (int i=0; i<csvSheet.getRowCount(); ++i)  
	{  
		CRow row = csvSheet.getRow(i);  //获取指定行
		CString s = row.getColumn(1); //获取指定列
		double v = wcstod(s, NULL);
		val.push_back(v);
		pY[i] = v;
	}  


	if (m_OrigSema.size()!=0) //将数据放入容器中
	{
		m_OrigSema.clear();
	}

	if (time.size()==val.size())
	{
		dXMax = time[0];
		dXMin = time[0];
		dYMax = val[0];
		dYMin = val[0];
		for (size_t i=0; i<time.size(); i++)
		{
			CSema OneSema(i, time[i], val[i]);
			if (time[i] > dXMax) dXMax = time[i];
			if (time[i] < dXMin) dXMin = time[i];	
			if (val[i] > dYMax) dYMax = val[i];
			if (val[i] < dYMin) dYMin = val[i];
			m_OrigSema.push_back(OneSema);
		}
	}
	else
	{
		//MessageBox(_T("读取csv数据有误，测定参数数量不一致"));
		delete [] pX;
		delete [] pY;
		return;
	}

	delete [] pX;
	delete [] pY;

	file.close();	//关闭CSV文件

}
VOID CPeakSeak::SetSemaDate(double x[], double y[], int n)
{
	//一次性分配vector的大小
	m_OrigSema.resize(n);

	dXMax = x[0];
	dXMin = x[0];
	dYMax = y[0];
	dYMin = y[0];
	int i=0;
	typedef std::vector<CSema>::iterator ITER;
	for (ITER iter=m_OrigSema.begin(); iter!=m_OrigSema.end(); ++iter)
	{
		CSema OneSema(i, x[i], y[i]);
		if (x[i] > dXMax) dXMax = x[i];
		if (x[i] < dXMin) dXMin = x[i];	
		if (y[i] > dYMax) dYMax = y[i];
		if (y[i] < dYMin) dYMin = y[i];
		*iter = OneSema;
		i++;
	}
	//dXMax = x[0];
	//dXMin = x[0];
	//dYMax = y[0];
	//dYMin = y[0];
	//for (int i=0; i<n; ++i)
	//{
	//	CSema OneSema(i, x[i], y[i]);
	//	if (x[i] > dXMax) dXMax = x[i];
	//	if (x[i] < dXMin) dXMin = x[i];	
	//	if (y[i] > dYMax) dYMax = y[i];
	//	if (y[i] < dYMin) dYMin = y[i];
	//	m_OrigSema.push_back(OneSema);
	//}
}
VOID CPeakSeak::SetSemaDate(const CChartPointsArray& vPoints)
{
	if(vPoints.GetPointsCount() == 0) return;
	dXMax = dXMin = vPoints.m_pPoints[0].x;
	dYMax = dYMin = vPoints.m_pPoints[0].y;
	for (size_t i=0; i<vPoints.GetPointsCount(); ++i)
	{
		CSema OneSema(i, vPoints.m_pPoints[i].x, vPoints.m_pPoints[i].y);
		if (OneSema.x > dXMax) dXMax = OneSema.x;
		if (OneSema.x < dXMin) dXMin = OneSema.x;	
		if (OneSema.y > dYMax) dYMax = OneSema.y;
		if (OneSema.y < dYMin) dYMin = OneSema.y;
		m_OrigSema.push_back(OneSema);
	}
}
VOID CPeakSeak::SetSemaDate(const CChartSerie* pSerie)
{
	if(!pSerie) return;
	SetSemaDate(pSerie->m_vPoints);
}
VOID CPeakSeak::SetParameter(bool IsDividePeak, bool HasNagetiveSpike, 
							double Threshold, double ThresholdFactor,
							double HalfPeakWidth, double PeakArea, double PeakHeight,
							int Series, int Avgs)
{
	m_IsDividePeak=IsDividePeak; //是否开启垂直分峰
	m_HasNagetiveSpike=HasNagetiveSpike; //是否有负峰情况

	m_Threshold=Threshold; //门限斜率
	m_thresholdFactor=ThresholdFactor;

	m_HalfPeakWidth=HalfPeakWidth; //最小半峰宽
	m_PeakArea=PeakArea; //最小峰面积
	m_PeakHeight=PeakHeight; //最小峰高

	m_Series=Series; //内部连续点判断。默认3
	m_Avgs=Avgs; //内部平均点数.默认5
}
VOID CPeakSeak::ClearSemaDate()
{
	m_OrigSema.clear();
	m_Peak.clear();
}

/*
BOOL CPeakSeak::SignalProcess(
	CAnalysisData* pSample_Parameter_PeakProcess, 
	CPeakList &cPeakList, 
	CArray<double> *pcArrayHPW, 
	CArray<double> *pcArrayMPA, 
	CArray<double> *pcArrayMH, 
	int *pMinHalfPeakWidthEarseNum,
	int *pMinPeakAreaEarseNum,
	int *pMinPeakHeightEarseNum,
	CArray<structLine> *pcVLines,
	double *pdMinSlope, 
	double *pdMaxSlope)
{
	double threshold,minHalfPeakWidth,minPeakArea,minPeakHeight;
	CString szAnalysisData = pSample_Parameter_PeakProcess->GetXMLString();
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_TRD, threshold)!=IAERROR_SUCCESS)  return FALSE;
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPW, minHalfPeakWidth)!=IAERROR_SUCCESS) return FALSE;
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPA, minPeakArea)!=IAERROR_SUCCESS) return FALSE;
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPH, minPeakHeight)!=IAERROR_SUCCESS) return FALSE;

	// CP 和 AP 参数部分
	int cp = 3; //连续判点
	int ap = 5; //平均点数
	if (pSample_Parameter_PeakProcess->GetValue_Int(XMLPATH_PEAKPROC_CP, cp)!=IAERROR_SUCCESS)  cp = 3;
	if (pSample_Parameter_PeakProcess->GetValue_Int(XMLPATH_PEAKPROC_AP, ap)!=IAERROR_SUCCESS)  ap = 5;
	if (cp>10) cp=10; 
	else if (cp<0)  cp=3;
	if (ap>10) ap=10;
	else if (ap<0) ap=5; 
	

	// 设置寻峰参数 (默认series, m_iavgN需要修改
	//第1-2个参数：是否开启垂直分峰，是否有负峰情况
	//第3-5个参数：门限斜率，最小半峰宽，最小峰面积，最小峰高
	double threshold_factor = 1.0f;
	SetParameter(true, false, threshold, threshold_factor, minHalfPeakWidth, minPeakArea, minPeakHeight, cp, ap);	


	// 读取设置峰处理参数表
	CIADataTable cPPTimeTable; 
	if (pSample_Parameter_PeakProcess->GetValue_Table(XMLPATH_PEAKPROC_PPT, cPPTimeTable) != IAERROR_SUCCESS) 
		return FALSE;

	// 需要传入的参数表，由pAnalysisData解析后传入
	const long ConfigArgumentNum = cPPTimeTable.GetRowsNumber(); //获得行数，即参数个数
	deque<pair<int, ConfigArgument> > dep_cargs;
	for (long i=0; i<ConfigArgumentNum; ++i)
	{
		CString szState, szNo, szName, szBegainTime, szEndTime, szValue;
		if(!cPPTimeTable.GetCellValue(i, _T("State"), szState)) return FALSE;
		if(!cPPTimeTable.GetCellValue(i, _T("ID"), szNo)) return FALSE; 
		if(!cPPTimeTable.GetCellValue(i, _T("Name"), szName)) return FALSE; 
		if(!cPPTimeTable.GetCellValue(i, _T("BegainTime"), szBegainTime)) return FALSE;
		if(!cPPTimeTable.GetCellValue(i, _T("EndTime"), szEndTime)) return FALSE;
		if(!cPPTimeTable.GetCellValue(i, _T("Value"), szValue)) return FALSE;

		double BegainTime = _tstof(szBegainTime);
		double EndTime = _tstof(szEndTime);
		double Value = _tstof(szValue);

		if (szValue==_T("前切")||szValue==_T("负峰"))
			Value = -1.0f;
		if (szValue==_T("后切")||szValue==_T("正峰"))
			Value = 1.0f;

		if (szState == _T("开"))
		{
			int no = _wtoi(szNo);
			if (no<0) return FALSE;
			ConfigArgument cArg(szName, BegainTime, EndTime,  Value);
			dep_cargs.push_back(std::make_pair(no, cArg));
		}
	}

	std::sort(dep_cargs.begin(), dep_cargs.end(), strict_weak_ordering());
	typedef deque<pair<int, ConfigArgument> >::iterator DEP_ITER;
	deque<ConfigArgument> cArgs; 
	for (DEP_ITER iter=dep_cargs.begin(); iter!=dep_cargs.end(); ++iter)
	{
		cArgs.push_back(iter->second);
	}

	//这里需要做个参数转化，把XML上的字段转化成峰处理参数
	m_AnalysisConfigArg.TransformArgs(cArgs);//多加一层预处理参数适配
	m_AnalysisConfigArg.SetArgs(cArgs);
	m_AnalysisConfigArg.PreprocessConfigArgs(); //配置参数预处理
	


	//峰处理函数
	//第一次扫描：寻峰关键点
	SP_FirstScan();

	//第二次扫描：去除不要的峰
	SP_SecondScan();

	//第三次扫描：重叠峰处理
	SP_ThirdScan();

	//第四次扫描：基线的校正
	SP_FourthScan();
	SP_FourthScan(); // 1 more time

	//第五次扫描：手工调整峰
	SP_FifthScan(); 

	//第六次扫描：积分并整理
	SP_SixthScan();

	//还原到移动平均之前数据
	SP_RestoreOriginalData();


	//CPeak类型转变成CPeakList 返回cPeakList
	for (size_t i=0; i<m_Peak.size(); ++i)
	{
		//int					m_nType;		//峰类型

		CPeakStandard aPeakStard;
		aPeakStard.m_cPeakType.all = m_Peak[i].PeakType.all;//峰类型
		aPeakStard.m_dReserveTime = m_Peak[i].PeakSema.x; //保留时间
		CCurveDataSingle startCurveData(m_Peak[i].StartSema.x, m_Peak[i].StartSema.y);
		CCurveDataSingle topCurveData(m_Peak[i].PeakSema.x, m_Peak[i].PeakSema.y);
		CCurveDataSingle endCurveData(m_Peak[i].FinSema.x, m_Peak[i].FinSema.y);
		aPeakStard.m_dataBegin = startCurveData;
		aPeakStard.m_dataTop = topCurveData;
		aPeakStard.m_dataEnd = endCurveData;
		aPeakStard.m_dHalfWidth = m_Peak[i].HalfPeakWidth;	//半峰宽
		aPeakStard.m_dWidth = m_Peak[i].PeakWidth;		//峰宽
		aPeakStard.m_dHeight = m_Peak[i].PeakHeight;		//峰高
		aPeakStard.m_dHeightPer = m_Peak[i].PeakHighPercent;	//峰高%
		aPeakStard.m_dArea = m_Peak[i].PeakArea;		//峰面积
		aPeakStard.m_dAreaPer = m_Peak[i].PeakAreaPercent;		//峰面积%
		//aPeakStard.m_dResultPer = m_Peak[i];	//分析结果（百分比%）在分析结果表中使用，峰列表中不用显示
		aPeakStard.m_dDisfactor = m_Peak[i].Disfactor;   //畸变因子??
		aPeakStard.m_dSymmetry = m_Peak[i].Tailing;    //拖尾因子
		aPeakStard.m_dResolution = m_Peak[i].Resolution;  //分离度
		aPeakStard.m_dColumnPlate = m_Peak[i].ColumnPlate; //理论塔板数
		aPeakStard.m_dResultConcentration = 0; //浓度
		cPeakList.m_cArrayData.Add(aPeakStard);
	}

	//把中间变量（所有的半峰宽和所有的峰面积）取出
	if (pcArrayHPW != NULL)
	{
		for (int i=0; i<m_cArrayHPW.GetSize(); ++i)
		{
			pcArrayHPW->Add(m_cArrayHPW.GetAt(i)); 
		}
	}

	if (pcArrayMPA != NULL)
	{
		for (int i=0; i<m_cArrayMPA.GetSize(); ++i)
		{
			pcArrayMPA->Add(m_cArrayMPA.GetAt(i)); 
		}
	}

	if(pcArrayMH != NULL)
	{
		for (int i=0; i<m_cArrayMH.GetSize(); ++i)
		{
			pcArrayMH->Add(m_cArrayMH.GetAt(i)); 
		}
	}

	if ( pMinHalfPeakWidthEarseNum != NULL )
		*pMinHalfPeakWidthEarseNum = m_MinHalfPeakWidthEarseNum;

	if ( pMinPeakAreaEarseNum != NULL )
		*pMinPeakAreaEarseNum = m_MinPeakAreaEarseNum;

	if ( pMinPeakHeightEarseNum != NULL )
		*pMinPeakHeightEarseNum = m_MinPeakHeightEarseNum;

	if ( pdMinSlope != NULL)
		*pdMinSlope = m_dMinSlope;

	if ( pdMaxSlope != NULL)
		*pdMaxSlope = m_dMaxSlope;

	if (pcVLines!=NULL)
	{
		const unsigned cArraySize = m_cArrayVLine.GetSize();
		for (unsigned i=0; i<cArraySize; ++i){
			structLine sLine=m_cArrayVLine[i];
			pcVLines->Add(sLine);
		}
	}
	
	return TRUE;
}
*/

VOID CPeakSeak::GetPeakTypeStr(CString &str) 
{
	str.Empty();
	const size_t PeakSize = m_Peak.size();
	for (size_t i=0 ; i<PeakSize; ++i)
	{
		CString szID;
		szID.Format(_T("%d"), i+1);
		str += _T("[") + szID + _T("] : ");
		str += m_Peak[i].PeakType.ConvertToTypeName(); 
		str += '\n';
	}
}


//以下是CHEM2009项目不需要用到的接口
int CPeakDraw::GROUPID_FIRST = 1;  
int CPeakDraw::GROUPID_SECOND = 2;  
int CPeakDraw::GROUPID_THREE = 3;    

CPeakDraw::CPeakDraw()
{
	counter = 0;
	m_pDrawCurve = NULL;
	m_pLinePeakSeek = NULL;
	m_pPoint = NULL; 
	m_pPeak  = NULL;
	m_BaseLineColor = RGB(210, 0, 0);
}

VOID CPeakDraw::SetChartCtrl(CSuperChartCtrl* p)
{
	if ((p!=NULL) && (m_pCtrl != p))
	{
		m_pCtrl = p;

		if (!m_pCtrl->IsGroupExist((size_t)1))
			GROUPID_FIRST = m_pCtrl->CreatNewGroup((size_t)1); //新建控件元素组1，用于画原来的曲线

		if (!m_pCtrl->IsGroupExist((size_t)2))
			GROUPID_SECOND = m_pCtrl->CreatNewGroup((size_t)2);//新建控件元素组2，用于画寻峰后的点

		if (!m_pCtrl->IsGroupExist((size_t)3))
			GROUPID_THREE = m_pCtrl->CreatNewGroup((size_t)3);//新建控件元素组2，用于画寻峰后的基线


		if (m_pDrawCurve == NULL) m_pDrawCurve = m_pCtrl->RealDraw(GROUPID_FIRST);
		if (m_pLinePeakSeek == NULL) m_pLinePeakSeek = m_pCtrl->RealDraw(GROUPID_FIRST);
		if (m_pPoint == NULL) 
		{
			m_pPoint = m_pCtrl->CreatePointsSerie(); //创建起点终点标点
			m_pPoint->SetGroupID(GROUPID_SECOND);
		}
		if (m_pPeak  == NULL) 
		{
			m_pPeak  = m_pCtrl->CreatePointsSerie(); //创建波峰标点
			m_pPeak->SetGroupID(GROUPID_SECOND);
		}
	}
}
VOID CPeakDraw::ClearLine()
{
	if (counter==0) 
	{
		counter++;
		return;
	}

	m_pCtrl->EnableRefresh(false);


	m_pDrawCurve->ClearSerie();
	m_pLinePeakSeek->ClearSerie();          //清空上次曲线
	m_pPoint->ClearSerie();					//清空上次各起点终点
	m_pPeak->ClearSerie();					//清空上次各波峰波谷点


	if (m_phemline.size()!=0) //清除基线
	{
		for (unsigned i=0; i<m_phemline.size(); i++)
			m_phemline[i]->ClearSerie();  
		m_phemline.clear();
	}

	if (m_pDividePeakBaseLine.size()!=0) //清除垂直分峰的连线
	{
		typedef std::list<CChartLineSerie*>::iterator LI;
		for (LI iter=m_pDividePeakBaseLine.begin(); iter!=m_pDividePeakBaseLine.end(); iter++)
			(*iter)->ClearSerie();
		m_pDividePeakBaseLine.clear();
	}

	if (m_pTagline.size()!=0)//清除之前的标记线
	{
		typedef std::list<CChartLineSerie*>::iterator LI;
		for (LI iter=m_pTagline.begin(); iter!=m_pTagline.end(); iter++)
			(*iter)->ClearSerie();
		m_pTagline.clear();
	}

//	m_pCtrl->RemoveAllChartStrings();  //清空上次字符串
	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}
VOID CPeakDraw::DrawCurve()
{
	m_pCtrl->EnableRefresh(false);

	m_pDrawCurve->SetColor(RGB(200, 200, 200)); //设置曲线的颜色(白灰)
	CChartAxis* pAxisSelect;
	m_pCtrl->SetBackColor(RGB(60, 60, 60)); //设置背景颜色
	pAxisSelect = m_pCtrl->GetLeftAxis(); //设置Y坐标
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(m_PeakSeak.dYMin*1.0, m_PeakSeak.dYMax*1.1, COORDINATE_SET);
	pAxisSelect = m_pCtrl->GetBottomAxis(); //设置X坐标
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(m_PeakSeak.dXMin, m_PeakSeak.dXMax, COORDINATE_SET);
	pAxisSelect->SetVisible(true);

	m_pDrawCurve->m_vPoints.InitPoints(m_PeakSeak.m_OrigSema.size());
	for (size_t i=0; i<m_PeakSeak.m_OrigSema.size(); ++i)
	{//画曲线
		m_pDrawCurve->m_vPoints.AddPoint(m_PeakSeak.m_OrigSema[i].x, m_PeakSeak.m_OrigSema[i].y);
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	

}
VOID CPeakDraw::CA_SignalDisplay()
{
	CA_DrawBaseLine();//基线
	DrawResultLine();//起落点
}
VOID CPeakDraw::CA_DrawBaseLine()
{
	if (m_PeakSeak.m_Peak.size()==0)
	{
		return;
	}

	const unsigned PeakSize = m_PeakSeak.m_Peak.size();
	//创建画峰谷连接的直线指针(第一轮，仅创建起点跟终点连接的基线)
	//包括正切，反切，垂直分峰
	for (unsigned i=0; i<PeakSize; ++i)
	{
		CChartLineSerie* baseline;  
		baseline = m_pCtrl->CreateLineSerie(); 
		baseline->SetColor(m_BaseLineColor); //设置基线颜色/*RGB(210, 0, 0)*/
		m_phemline.push_back(baseline);
	}
	//画基线
	for (unsigned i=0; i<PeakSize; ++i)
	{
		//m_phemline[i]->SetGroupID(GROUPID_THREE);	
		m_phemline[i]->m_vPoints.Clear();
		m_phemline[i]->m_vPoints.InitPoints(4);
		double dx[] = { m_PeakSeak.m_Peak[i].StartSema.x, m_PeakSeak.m_Peak[i].FinSema.x };
		double dy[] = { m_PeakSeak.m_Peak[i].StartSema.y, m_PeakSeak.m_Peak[i].FinSema.y };
		m_phemline[i]->m_vPoints.AddPoints(dx, dy, 2);   //画基线
	}
	//画垂直分峰的分峰线
	deque<pair<unsigned, unsigned> > VertDivideArgs;  //垂直分峰
	CA_GetBaseLineArgs(CPeak::VERTDIVIDE, m_PeakSeak.m_Peak, VertDivideArgs);
	while (!VertDivideArgs.empty())
	{
		//分峰线
		unsigned begin = VertDivideArgs[0].first;
		unsigned end = VertDivideArgs[0].second;
		for (unsigned i= begin; i<end; ++i)
		{
			CChartLineSerie* divPeakLine;  
			divPeakLine = m_pCtrl->CreateLineSerie();//创建一根基线的连接线
			divPeakLine->SetColor(m_BaseLineColor); //设置基线颜色/*RGB(210, 0, 0)*/
			//divPeakLine->SetGroupID(GROUPID_THREE);
			divPeakLine->m_vPoints.InitPoints(4);
			double dx[] = { m_PeakSeak.m_Peak[i].FinSema.x, m_PeakSeak.m_Peak[i].FinSema.x };
			double dy[] = { m_PeakSeak.m_OrigSema[m_PeakSeak.m_Peak[i].FinSema.idx].y, m_PeakSeak.m_Peak[i].FinSema.y };
			divPeakLine->m_vPoints.AddPoints(dx, dy, 2);
			m_pDividePeakBaseLine.push_back(divPeakLine);
		}
		VertDivideArgs.pop_front();
	}
	m_pCtrl->EnableRefresh(true);
}
VOID CPeakDraw::SetCPeakSeakData(const CPeakSeak &inPeak)
{
	if (!m_PeakSeak.m_OrigSema.empty()) m_PeakSeak.m_OrigSema.clear();
	if (!m_PeakSeak.m_Peak.empty())     m_PeakSeak.m_Peak.clear();

	m_PeakSeak.m_OrigSema = inPeak.m_OrigSema;
	m_PeakSeak.m_Peak = inPeak.m_Peak;     //峰的容器

	////配置参数结构AnalysisConfigArguments m_AnalysisConfigArg;

	//传入参数
	m_PeakSeak.m_IsDividePeak = inPeak.m_IsDividePeak ; //是否开启垂直分峰
	m_PeakSeak.m_HasNagetiveSpike = inPeak.m_HasNagetiveSpike; //是否有负峰情况
	m_PeakSeak.m_Avgs = inPeak.m_Avgs; //内部平均点数.默认5
	m_PeakSeak.m_Series = inPeak.m_Series; //内部连续点判断。默认3
	m_PeakSeak.m_Threshold = inPeak.m_Threshold; //门限斜率
	m_PeakSeak.m_HalfPeakWidth = inPeak.m_HalfPeakWidth; //最小半峰宽
	m_PeakSeak.m_PeakArea = inPeak.m_PeakArea; //最小峰面积

	//控件坐标轴范围
	m_PeakSeak.dXMin = inPeak.dXMin;
	m_PeakSeak.dXMax = inPeak.dXMax;
	m_PeakSeak.dYMin = inPeak.dYMin;
	m_PeakSeak.dYMax = inPeak.dYMax;

}
VOID CPeakDraw::SignalDisplay()
{
	DrawBaseLine(); //基线
	DrawDividePeakBaseLine();
	DrawResultLine(); //起落点
}
VOID CPeakDraw::DrawBaseLine()
{
	if (m_PeakSeak.m_Peak.empty())
	{
		return;
	}
	bool isdraw = true;
	const unsigned PeakSize = m_PeakSeak.m_Peak.size();
	if ((!m_PeakSeak.m_IsDividePeak) && (PeakSize!=0)) //非垂直分峰
	//if ((m_PeakSeak.m_Peak.dType != CPeak::VERTDIVIDE) && (PeakSize!=0)) //非垂直分峰
	{
		for (unsigned i=0; i<PeakSize; ++i)
		{//创建画直线指针
			CChartLineSerie* line;  
			line = m_pCtrl->CreateLineSerie(); 
			line->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
			m_phemline.push_back(line);
		}
		if (m_phemline.size()==0)
		{
			isdraw = false;
		}
		else
		{
			const unsigned m_phemlineSize = m_phemline.size();
			for (unsigned i=0; i<PeakSize; ++i)
			{//连接直线
				m_phemline[i]->SetGroupID(GROUPID_THREE);		
				m_phemline[i]->m_vPoints.InitPoints(4);
				double dx[] = { m_PeakSeak.m_Peak[i].StartSema.x, m_PeakSeak.m_Peak[i].FinSema.x };
				double dy[] = { m_PeakSeak.m_Peak[i].StartSema.y, m_PeakSeak.m_Peak[i].FinSema.y };
				m_phemline[i]->m_vPoints.AddPoints(dx, dy, 2);   //画基线
			}
		}
	}

	else if (PeakSize!=0 && isdraw)
	{
		unsigned nodeIndex = 0;  //连垂直分峰线的索引
		unsigned bg = 0;
		unsigned ed = bg;
		bool isSignalWave = false;
		for (; bg<PeakSize; ++bg)
		{
			if (m_PeakSeak.m_Peak[bg].Type==1 || m_PeakSeak.m_Peak[bg].Type==-1)
			{
				ed = bg;
				isSignalWave = true;
			}
			else /*(Peak[bg].Type>1)*/ //重叠波峰
			{
				ed = bg;
				int mark = m_PeakSeak.m_Peak[bg].Type;
				while (ed<PeakSize && m_PeakSeak.m_Peak[ed].Type==mark)
				{
					if (ed+1==PeakSize && m_PeakSeak.m_Peak[PeakSize-1].Type==mark)
					{
						break;
					}
					ed++;
					if (m_PeakSeak.m_Peak[ed].Type!=mark)
					{
						ed--;
						break;
					}
				}
			}
			if (isSignalWave && nodeIndex!=PeakSize) //画单峰分峰线
			{
				CChartLineSerie* line;  
				line = m_pCtrl->CreateLineSerie();
				double dx[] = { m_PeakSeak.m_Peak[nodeIndex].StartSema.x, m_PeakSeak.m_Peak[nodeIndex].FinSema.x };
				double dy[] = { m_PeakSeak.m_Peak[nodeIndex].StartSema.y, m_PeakSeak.m_Peak[nodeIndex].FinSema.y };
				nodeIndex++;
				line->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
				line->SetGroupID(GROUPID_THREE);
				line->m_vPoints.InitPoints(4);
				line->m_vPoints.AddPoints(dx, dy, 2);   
				isSignalWave = false;
				m_phemline.push_back(line);
			}
			else if (!isSignalWave && nodeIndex!=PeakSize) //画连续峰分峰线
			{
				for (; bg!=ed; ++bg)
				{
					CChartLineSerie* line1/*, *line2, *line3*/;  
					line1 = m_pCtrl->CreateLineSerie(); 
					double dx[] = { m_PeakSeak.m_Peak[nodeIndex].StartSema.x, m_PeakSeak.m_Peak[nodeIndex].FinSema.x };
					double dy[] = { m_PeakSeak.m_Peak[nodeIndex].StartSema.y, m_PeakSeak.m_Peak[nodeIndex].FinSema.y };
					nodeIndex++;
					line1->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
					line1->SetGroupID(GROUPID_THREE);
					line1->m_vPoints.InitPoints(4);
					line1->m_vPoints.AddPoints(dx, dy, 2);
					m_phemline.push_back(line1);

					if (bg+1==ed)
					{
						double dxx[] = { m_PeakSeak.m_Peak[nodeIndex].StartSema.x, m_PeakSeak.m_Peak[nodeIndex].FinSema.x };
						double dyy[] = { m_PeakSeak.m_Peak[nodeIndex].StartSema.y, m_PeakSeak.m_Peak[nodeIndex].FinSema.y };
						nodeIndex++;
						line1->m_vPoints.InitPoints(4);
						line1->m_vPoints.AddPoints(dxx, dyy, 2);
						m_phemline.push_back(line1);
					}
				}
			}
		}
	}

	m_pCtrl->EnableRefresh(true);
	
}
VOID CPeakDraw::DrawDividePeakBaseLine()
{
	if (m_PeakSeak.m_Peak.size()==0)
	{
		return;
	}
	
	m_pCtrl->EnableRefresh(false);

	const unsigned PeakSize = m_PeakSeak.m_Peak.size();
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (m_PeakSeak.m_Peak[i].dType == CPeak::VERTDIVIDE) //先判断是否是垂直分峰
		{
			CChartLineSerie* line;  
			line = m_pCtrl->CreateLineSerie();//创建一根基线的连接线
			line->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
			line->m_vPoints.InitPoints(4);
			double dx[] = { m_PeakSeak.m_Peak[i].FinSema.x, m_PeakSeak.m_Peak[i].FinSema.x };
			double dy[] = { m_PeakSeak.m_OrigSema[m_PeakSeak.m_Peak[i].FinSema.idx].y, m_PeakSeak.m_Peak[i].FinSema.y };
			line->m_vPoints.AddPoints(dx, dy, 2);
			m_pDividePeakBaseLine.push_back(line);
		}
	}

	m_pCtrl->EnableRefresh(true);
}
VOID CPeakDraw::DrawResultLine()
{
	m_pCtrl->EnableRefresh(false);
	m_pLinePeakSeek->SetColor(RGB(200, 200, 200)); //设置曲线的颜色(白灰)
	CChartAxis* pAxisSelect;

	m_pCtrl->SetBackColor(RGB(60, 60, 60)); //设置背景颜色
	pAxisSelect = m_pCtrl->GetLeftAxis(); //设置Y坐标
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(m_PeakSeak.dYMin*1.0, m_PeakSeak.dYMax*1.1, COORDINATE_SET);
	pAxisSelect = m_pCtrl->GetBottomAxis(); //设置X坐标
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(m_PeakSeak.dXMin, m_PeakSeak.dXMax, COORDINATE_SET);
	m_pPoint->SetColor(RGB(255, 70, 70)); //设置标起点落点的颜色(红色)
	m_pPeak->SetPointType(CChartPointsSerie::ptTriangle); //设置波峰点是三角标记
	m_pPeak->SetColor (RGB(0, 138, 138)); //设置标波峰波谷点的颜色(兰色)
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetVisible(true);

	m_pLinePeakSeek->m_vPoints.InitPoints(m_PeakSeak.m_OrigSema.size());
	for (size_t i=0; i<m_PeakSeak.m_OrigSema.size();++i)
	{//画曲线
		m_pLinePeakSeek->m_vPoints.AddPoint(m_PeakSeak.m_OrigSema[i].x, m_PeakSeak.m_OrigSema[i].y);
	}

	if (m_PeakSeak.m_Peak.size()==0)
	{
		m_pCtrl->EnableRefresh(true);
		m_pCtrl->RefreshCtrl();	
		return;
	}
	m_pPoint->m_vPoints.Clear();
	m_pPeak->m_vPoints.Clear();
	m_pPoint->m_vPoints.InitPoints(m_PeakSeak.m_Peak.size());
	m_pPeak->m_vPoints.InitPoints(m_PeakSeak.m_Peak.size());
	for (unsigned i=0; i<m_PeakSeak.m_Peak.size(); ++i)
	{//标出起点落点
		double x[]={m_PeakSeak.m_Peak[i].StartSema.x, m_PeakSeak.m_Peak[i].FinSema.x};
		double y[]={m_PeakSeak.m_Peak[i].StartSema.y, m_PeakSeak.m_Peak[i].FinSema.y};
		m_pPoint->m_vPoints.AddPoints(x, y, 2);
		m_pPeak->m_vPoints.AddPoint(m_PeakSeak.m_Peak[i].PeakSema.x, m_PeakSeak.m_Peak[i].PeakSema.y);//波峰波谷
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	

}

//--------------------------------------------------------------------------------------------------------------------------------
// 扫描过程
//--------------------------------------------------------------------------------------------------------------------------------
void CPeakSeak::SP_FirstScan(){
	m_Peak.clear();

	if (m_OrigSema.size()==0) return; 
	
	//step0: 对原数据移动平均
	int moving_avg_n = 5;
	SP_MovingAverage(m_OrigSema, moving_avg_n);

	//step1: 求以avg为1点的平均曲线，剩余除不尽的为1点
	std::vector<CSema> AvgData;
	if ((m_OrigSema.size()/m_Avgs)<9) {
		::AfxMessageBox( _T("原数据采样太少，无法对曲线进行积分"));
		return;
	}

	SP_Averaging(AvgData); 
	

	//step2: 求出平均曲线上每一点的斜率
	std::vector<double> FirstOrder;					      //斜率容器
	double maxSlope = 0.0;								  //曲线中最大斜率
	SP_GetAvgDataSlope(AvgData, FirstOrder, maxSlope);
	SP_GetMinMaxSlope(/*AvgData, */FirstOrder, 5);

	// step3: 起点→波峰→落点顺序寻峰
	const unsigned AvgDataSize = AvgData.size();
	m_AnalysisConfigArg.SetPeakSearchArgs(AvgDataSize, m_Threshold);
	m_AnalysisConfigArg.SetMinusCheckArgs(AvgDataSize);
	m_AnalysisConfigArg.Analysis_PeakSearchSlopeBased(AvgData);
	m_AnalysisConfigArg.Analysis_PeakSearchMinusCheck(AvgData);

	SP_GetpeakSearch_Series_plus(AvgData, FirstOrder, 
		m_AnalysisConfigArg.GetPeakSearchArgs(), //分段噪声阀门
		m_AnalysisConfigArg.GetMinusCheckArgs(), //分段负峰检测
		m_thresholdFactor
	);

	// step4: 固定点还原至原曲线
	std::vector<double> SlopeBased(m_OrigSema.size(), m_Threshold);
	SP_RestorePeakData(SlopeBased);  //还原至原曲线
}
void CPeakSeak::SP_SecondScan() {
	// 禁止积分删除峰
	m_AnalysisConfigArg.Analysis_BanPeak(m_Peak); //解析积分参数
	SP_BanPeak(m_AnalysisConfigArg.GetBanPeakArgs()); //禁止积分
	SP_GetHalfPeakWidth(); //获得半峰宽

	// 最小半峰宽删除峰
	const unsigned PeakSize = m_Peak.size();
	m_AnalysisConfigArg.SetMinPeakWidthArgs(PeakSize, m_HalfPeakWidth);
	m_AnalysisConfigArg.Analysis_MinHalfPeakWidth(m_Peak); //解析分段最小半峰宽参数

	SP_GetAllHPW();//所有的半峰宽赋值给成员变量m_cArrayHPW
	SP_EraseByHalfPeakWidth(m_AnalysisConfigArg.GetMinHalfPeakWidthArgs()); //删除不满足半峰宽条件的峰
}
void CPeakSeak::SP_ThirdScan(){

	SP_CheckOverlapPoint(); //追加，重叠峰修正

	// step5: 重叠峰处理(分峰)
	vector<AnalysisConfigArguments::PeakSepaArg> dequeIdx;
	m_AnalysisConfigArg.Analysis_PeakSeparation(m_Peak, dequeIdx);
	SP_TangentDividPeak(dequeIdx); //切线分峰，之后需要基线校正

	deque<pair<unsigned, unsigned> > seriesPeak;  //连续峰序号，默认连续峰全做垂直分峰处理
	SP_GetSeriesPeakDeque(seriesPeak);
	for (size_t i=0; i<seriesPeak.size(); ++i)
		SP_PeakValleyBaseLineCheck(seriesPeak[i].first, seriesPeak[i].second); //峰谷基线校正 
	//SP_PeakTypeChange();

	// 排除含有峰谷连线的连续峰
	typedef deque<pair<unsigned, unsigned> >::iterator deIter;
	for (size_t i=0; i<dequeIdx.size(); ++i) {
		if (dequeIdx[i].SepaType!=1)//排除非垂直分峰
		{
			unsigned startPeakNo = dequeIdx[i].StartPeakNo;
			unsigned finishPeakNo = dequeIdx[i].FinishPeakNo;
			for (deIter iter=seriesPeak.begin(); iter!=seriesPeak.end(); ++iter) {
				if (((*iter).first<=startPeakNo && (*iter).second>=startPeakNo) ||
					((*iter).first<=finishPeakNo && (*iter).second>=finishPeakNo))
				{
					seriesPeak.erase(remove(seriesPeak.begin(), seriesPeak.end(), *iter), seriesPeak.end());
					break;
				}
			}
		}
	}
	     

	// 新加参数 峰谷比  
	bool isPeak2Valley = m_AnalysisConfigArg.IsPeak2Valley(); 
	if (isPeak2Valley) 
	{  
		double dPeak2valley_Ratio = m_AnalysisConfigArg.GetDefaultPeak2Valley(); //获得峰谷比参数，有默认值
		SP_CheckPeakToValley(dPeak2valley_Ratio, seriesPeak);
		SP_PeakTypeChange();
	}  
	else
	{
		for (size_t i=0; i<seriesPeak.size(); ++i) {
			dequeIdx.clear();
			AnalysisConfigArguments::PeakSepaArg idx(seriesPeak[i].first, seriesPeak[i].second, 1);
			dequeIdx.push_back(idx);
		}
		SP_DividePeak(dequeIdx);       //垂直分峰
	}

}
void CPeakSeak::SP_FourthScan() {
	// 基线校正	
	deque<pair<unsigned, unsigned> > VertDivideArgs;  //垂直分峰
	deque<pair<unsigned, unsigned> > PeakVallyArgs;   //峰谷连线
	deque<pair<unsigned, unsigned> > PlusTangetArgs;  //正切分峰
	deque<pair<unsigned, unsigned> > MinusTangetArgs; //反切分峰

	SP_GetBaseLineArgs(CPeak::VERTDIVIDE, VertDivideArgs);
	while (!VertDivideArgs.empty())
	{
		SP_VertDivideBaseLineCheck(
			VertDivideArgs[0].first,
			VertDivideArgs[0].second); //垂直分峰基线校正
		VertDivideArgs.pop_front();
	}

	SP_GetBaseLineArgs(CPeak::PLUSTANGET, PlusTangetArgs);
	while (!PlusTangetArgs.empty())
	{
		SP_PlusTangetBaseLineCheck(
			PlusTangetArgs[0].first,
			PlusTangetArgs[0].second); //正切分峰基线校正
		PlusTangetArgs.pop_front();
	}

	SP_GetBaseLineArgs(CPeak::MINUSTANGET, MinusTangetArgs);
	while (!MinusTangetArgs.empty())
	{
		SP_MinusTangetBaseLineCheck(
			MinusTangetArgs[0].first,
			MinusTangetArgs[0].second); //反切分峰基线校正
		MinusTangetArgs.pop_front();
	}

	SP_GetBaseLineArgs(CPeak::PEAKVALLEY, PeakVallyArgs);
	while (!PeakVallyArgs.empty()) 
	{
		SP_PeakValleyBaseLineCheck(
			PeakVallyArgs[0].first,
			PeakVallyArgs[0].second); //峰谷基线校正
		PeakVallyArgs.pop_front();
	}
	
	SP_PeakTypeChange();
	
}
void CPeakSeak::SP_FifthScan(){

	SP_CheckOverlapPoint(); //追加，重叠峰修正
	
	// 2012-08-07 需求变更
	// 积分处理时需要将手工调整参数（分裂峰、手工基线和手工峰顶）的处理放到最小峰面积和最小峰高之后。
	SP_GetOtherPeakArgs(); //获得峰高
	// 分段最小峰面积 
	SP_GetPeakArea(); //峰面积
	// 扩大峰面积这个功能应该从积分算法中移出，放到积分事件之后
	bool isSetUnit = false;

	{//测试脚手架程序时注释这块作用域内的代码
		//bool isGC1120  = ((g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC_PX) || (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC1120))  ? true : false;
		//bool isGC2400  = (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC2400)  ? true : false;
		//bool isGC1100w = (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC1100W) ? true : false;
		//bool isGC1100p = (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC1100P) ? true : false;
		//isSetUnit = (isGC1120 || isGC2400 || isGC1100w || isGC1100p);
	}
	 
	m_Magnification = 1.0f;
	if (isSetUnit) 
		m_Magnification = 60000.0f;
	else
		m_Magnification = 60.0f;
	SP_SetPeakAreaUnit(m_Magnification);
	
	unsigned PeakSize = m_Peak.size();
	m_AnalysisConfigArg.SetMinPeakAreaArgs(PeakSize, m_PeakArea);
	m_AnalysisConfigArg.Analysis_MinPeakArea(m_Peak);
	SP_GetAllMPA(); //所有的峰面积赋值给成员变量 m_cArrayMPA
	SP_EraseByPeakArea(m_AnalysisConfigArg.GetMinPeakAreaArgs());
	

	// 分段最小峰高
	PeakSize = m_Peak.size();
	m_AnalysisConfigArg.SetMinPeakHeightArgs(PeakSize, m_PeakHeight);
	m_AnalysisConfigArg.Analysis_MinPeakHeight(m_Peak);
	SP_GetAllMH();
	SP_EraseByPeakHeight(m_AnalysisConfigArg.GetMinPeakHeightArgs());
	

	// 合并峰 和 手工调整 处理顺序按用户添加事件顺序
	deque<ConfigArgument> allConfigArgument = m_AnalysisConfigArg.GetAllArguments();
	const int configArgSize = allConfigArgument.size();
	for (int i=0; i<configArgSize; i++)
	{
		if (allConfigArgument[i].ArgName == _T("合并峰"))
		{
			//合并峰
			m_AnalysisConfigArg.Analysis_MergePeak(m_Peak);
			SP_MergePeak(m_AnalysisConfigArg.GetMergePeakNo());
			SP_PeakTypeChange(); //合并峰后的峰类型调整
		}

		else if (allConfigArgument[i].ArgName == _T("手工分峰"))
		{
			//手工分峰 => 分裂峰
			deque<double> handPeakSera;
			m_AnalysisConfigArg.Analysis_HandPeakSeparation(m_Peak, handPeakSera);
			
			while (!handPeakSera.empty()) //得到的handPeakSera未排序
			{
				int nfindIndex = m_AnalysisConfigArg.Analysis_OneHandPeakSepar(m_Peak, handPeakSera[0]);
				if (nfindIndex != -1) //找到可以做手工分峰的峰下标
				{
					SP_SplitPeak(nfindIndex, handPeakSera[0]);
					SP_PeakTypeChange(); //手工分峰后对峰编号的调整
				}
				handPeakSera.pop_front();
			}
		}

		else if (allConfigArgument[i].ArgName == _T("手工基线"))
		{
			//手工封顶
			m_AnalysisConfigArg.Analysis_HandAdjustPeak(m_Peak);
			SP_HandAdjustPeak(m_AnalysisConfigArg.GetHandAdjustPeakArgs());  //调整峰谷

			//手工基线
			m_AnalysisConfigArg.Analysis_HandworkBaseLine(m_Peak);
			SP_HandAdjustStart(m_AnalysisConfigArg.GetHandAdjustStartArgs());//调整起点
			SP_HandAdjustFinish(m_AnalysisConfigArg.GetHandAdjustFinishArgs());//调整终点
		}
	}

}
void CPeakSeak::SP_SixthScan(){
	// step7:最终整理
	// 峰的其他参数（畸变因子，拖尾因子，峰高，峰宽）
	SP_GetOtherPeakArgs();

	// 再计算一遍 分段最小峰面积 
	SP_GetPeakArea(); //峰面积
	// 扩大峰面积这个功能应该从积分算法中移出，放到积分事件之后
	SP_SetPeakAreaUnit(m_Magnification);


	// 增加峰 和 删除峰 需要按用户添加积分事件的顺序进行调用
	typedef void (CPeakSeak::*pFuncPointer)(ConfigArgument &);
	pFuncPointer fp_addpeak = &CPeakSeak::SP_AddPeak_One; 
	pFuncPointer fp_delpeak = &CPeakSeak::SP_DelPeak;

	m_AnalysisConfigArg.Analysis_AddelPeak(m_Peak);
	deque<ConfigArgument> AddDelPeakArgs = m_AnalysisConfigArg.GetAddDelPeakArgs();
	while (!AddDelPeakArgs.empty())
	{	
		ConfigArgument arg = AddDelPeakArgs[0];  
		if (arg.ArgName==_T("增加峰"))	(this->*fp_addpeak)(arg);
		if (arg.ArgName==_T("删除峰"))	(this->*fp_delpeak)(arg);
		AddDelPeakArgs.pop_front();
	}

	// 求峰的百分比（峰高，峰面积）
	SP_GetPeakPercent();

	// 判断峰的具体类型
	SP_CheckPeakType(); //先检查纠正
	SP_SetPeakType();

	// 计算分离度
	SP_CalcResolution();

	// 保存垂直分峰数据
//	SP_GetDividePeakLine();
}

//--------------------------------------------------------------------------------------------------------------------------------
// 把全局CA_ 函数移到CPeakSeak类中来                                                                                               
//--------------------------------------------------------------------------------------------------------------------------------
//第1次扫描
void CPeakSeak::SP_MovingAverage(vector<CSema> &OrigData, int N=5){
	m_OrigSema_backup = OrigData; //??? TODO

	int OrigDataSize = OrigData.size();
	if (OrigDataSize < N) return;
	
	int i=0;
	typedef vector<CSema>::iterator ITER;
	ITER flag_iter;
	for (ITER iter=OrigData.begin(); iter!=OrigData.end(); iter++)
	{
		if (i == OrigDataSize - N)
			flag_iter = iter;

		if (i >= OrigDataSize - N)
		{
			(*iter).x = (*flag_iter).x;
			(*iter).y = (*flag_iter).y;
			continue;
		}

		for (int j=1; j<N; j++)
		{
			ITER next_iter = iter;
			int next = 0;
			while (next!=j)
			{
				next_iter++;
				next++;
			}
			(*iter).y = (*iter).y + (*next_iter).y;
		}
		(*iter).y = (*iter).y / N;
		i++;
	}



	//for (int i=0; i<OrigDataSize - N; i++)
	//{
	//	for (int j=1; j<N; j++)
	//	{
	//		OrigData[i].y = OrigData[i].y + OrigData[i+j].y;
	//	}
	//	OrigData[i].y = OrigData[i].y / N;
	//}
	//for (int i=OrigDataSize-N; i<OrigDataSize; i++)
	//{
	//	OrigData[i].x = OrigData[OrigDataSize-N].x;
	//	OrigData[i].y = OrigData[OrigDataSize-N].y;
	//}
}
void CPeakSeak::SP_Averaging(std::vector<CSema> &avgData) { 
	if (m_Avgs==0) 
	{
		avgData = m_OrigSema;
		return;
	}

	const unsigned handleSemaSize = m_OrigSema.size();
	for (unsigned i=0; i<handleSemaSize; ++i)
	{
		double avgTime = 0.0;
		double avgSema = 0.0;
		if (i % m_Avgs == 0 && i+m_Avgs < handleSemaSize) 
		{
			for (unsigned j=i; j< i+m_Avgs; j++)
			{
				avgTime += m_OrigSema[j].x;
				avgSema += m_OrigSema[j].y;
			}
			avgTime = avgTime / m_Avgs;
			avgSema = avgSema / m_Avgs;
			CSema OneSema(i, avgTime, avgSema);
			avgData.push_back(OneSema);
		}
		if ((handleSemaSize-m_Avgs*avgData.size())/m_Avgs<1) //剩余
		{
			for (unsigned j=m_Avgs*avgData.size(); j<handleSemaSize; ++j)
			{
				avgTime += m_OrigSema[j].x;    
				avgSema += m_OrigSema[j].y;  
			}
			avgTime = avgTime/(handleSemaSize-m_Avgs*avgData.size()+1);
			avgSema = avgSema/(handleSemaSize-m_Avgs*avgData.size()+1);
			CSema OneSema(m_Avgs*avgData.size(), avgTime, avgSema);
			avgData.push_back(OneSema);
			break;
		}
	}
}
void CPeakSeak::SP_GetAvgDataSlope(const vector<CSema> &avgData, vector<double> &firstOrder, double &maxSlope)					//最大斜率            
{//step2: 求出平均曲线上每一点的斜率
	//如果输入N个数据，传出的则是N-1个结果
	//最后一个数据没有门限斜率

	const unsigned avgDataSize = avgData.size()-1;
	double *k = new double[avgDataSize];
	firstOrder.resize(avgDataSize);

	typedef vector<double>::iterator ITER_D;
	typedef vector<CSema>::const_iterator ITER_C;
	ITER_C av_iter = avgData.begin();

	int i=0;
	for (ITER_D iter=firstOrder.begin(); iter!=firstOrder.end(); iter++)
	{
		ITER_C next_av_iter = av_iter;
		next_av_iter++;

		double ydiff = (*next_av_iter).y - (*av_iter).y;
		double xdiff = (*next_av_iter).x - (*av_iter).x;
		k[i] = (ydiff) / (xdiff);
		if ( k[i] >= maxSlope ) maxSlope = k[i];
		(*iter) = k[i];

		av_iter = next_av_iter;
		i++;
	}

	delete [] k;
	k = NULL;

	//const unsigned avgDataSize = avgData.size()-1;
	//double *k = new double[avgDataSize];
	//for (unsigned i=0; i<avgDataSize; ++i)
	//{	
	//	double ydiff = avgData[i+1].y - avgData[i].y;
	//	double xdiff = avgData[i+1].x - avgData[i].x;
	//	k[i] = (ydiff) / (xdiff);
	//	if (k[i]>=maxSlope)
	//		maxSlope = k[i];
	//	firstOrder.push_back(k[i]);
	//}
	//delete [] k;
	//k = NULL;
}
void CPeakSeak::SP_GetMinMaxSlope(/*const vector<CSema> &avgData,*/ const vector<double> &firstOrder,int N)
{
	if (N<1 || static_cast<size_t>(N)>firstOrder.size())  return;

	//初始化斜率最值
	m_dMinSlope = (double)INT_MAX;
	m_dMaxSlope = 0.0;

	//斜率比较缓存
	deque<double> tmpSlope;

	typedef vector<double>::const_iterator ITER;
	for (ITER iter=firstOrder.begin(); iter!=firstOrder.end(); iter++)
	{
		for (int j=0; j<N; j++)
		{
			if (*iter <= 0.0f) 
			{
				continue;
			}
			tmpSlope.push_back(*iter);
		}

		if (tmpSlope.empty())
		{
			continue;
		}

		double maxS = tmpSlope[0];
		double minS = tmpSlope[0];
		while (!tmpSlope.empty()) 
		{
			if (tmpSlope[0]>maxS) maxS = tmpSlope[0];
			if (tmpSlope[0]<minS) minS = tmpSlope[0];
			tmpSlope.pop_front();
		}
		if(maxS < m_dMinSlope) m_dMinSlope = maxS;
		if(minS > m_dMaxSlope) m_dMaxSlope = minS;
	}


	//for (size_t i=0; i<firstOrder.size(); i+=N)
	//{
	//	for (int j=0; j<N; ++j)
	//	{
	//		if (firstOrder[i]<=0) //小于等于0的斜率不要
	//			continue;
	//		tmpSlope.push_back(firstOrder[i]);
	//	}
	//	if (tmpSlope.empty()) continue;
	//	double maxS = tmpSlope[0];
	//	double minS = tmpSlope[0];
	//	while (!tmpSlope.empty()) //比较缓存，得到最大值和最小值
	//	{
	//		if (tmpSlope[0]>maxS) maxS = tmpSlope[0];
	//		if (tmpSlope[0]<minS) minS = tmpSlope[0];
	//		tmpSlope.pop_front();
	//	}

	//	if(maxS < m_dMinSlope) m_dMinSlope = maxS;
	//	if(minS > m_dMaxSlope) m_dMaxSlope = minS;
	//}
}

void CPeakSeak::SP_GetpeakSearch_Series(const vector<CSema> &avgData,   const vector<double> &firstOrder,vector<double> &slopeBased, deque<bool> &hasNagetiveSpike)   
{// step3: 起点→波峰→落点顺序寻峰
	
	enum enumState {
		STATELESS,			//无状态
		UPCOMING_WAVE,		//上行波
		DOWNGOING_WAVE,		//下行波
		HANDLE_ACCOMPLISH   //处理完毕
	} hState;				//处理过程中的标记

	const unsigned avgDataSize = avgData.size();
	unsigned i=0, j=0, k=0;
	unsigned getStartPoint=0, getpeakPoint=0, getFinishPoint=0;
	int mark = 1; //波类型标记: 1=波峰; -1=波谷; 2,3,4,...为重叠波峰; -2,-3,-4...为重叠波谷
	bool isNagetiveSpike = false;
	bool processed = false;
	hState = STATELESS;
	while (hState != HANDLE_ACCOMPLISH)
	{
		switch (hState)
		{
		case STATELESS:  //无状态
			for ( ; i<avgDataSize-m_Series && hState==STATELESS; ++i)  
			{
				if (i>=avgDataSize-m_Series-1) 
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}

				if (firstOrder[i]<=0)
				{//负峰起点
					if (hasNagetiveSpike[i]) //负峰考虑情况 
					{
						if (fabs(firstOrder[i])<slopeBased[i])
							continue;
						if (fabs(firstOrder[i])>=slopeBased[i])
						{
							for (int s=1; s<=m_Series; ++s)
							{
								if (fabs(firstOrder[i+s])<slopeBased[i])
									break;
								if (fabs(firstOrder[i+s])>slopeBased[i])
								{
									if (s==m_Series)
									{
										isNagetiveSpike = true;
										getStartPoint = i;
										hState = DOWNGOING_WAVE;
										break;
									}
								}
							}
						}
					}
					else
						continue;
				}	

				if (firstOrder[i]>=0) //非负峰考虑情况
				{
					if (firstOrder[i]>slopeBased[i])
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (fabs(firstOrder[i+s])<slopeBased[i+s])
								break;
							if (fabs(firstOrder[i+s])>=slopeBased[i+s])
							{
								if (s==m_Series)
								{
									getStartPoint = i;
									hState = UPCOMING_WAVE;
									break;
								}
							}
						}
					}
				}
			}
			break;

		case UPCOMING_WAVE: //上行波
			if (!isNagetiveSpike)
			{
				for (j=i/*-1*/; j<avgDataSize-1 && hState==UPCOMING_WAVE; ++j)
				{
					if (j>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}

					if (firstOrder[j]<0)
						continue;
					if (firstOrder[j]>=0)
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (firstOrder[j+s]>0)//
								break;
							if (firstOrder[j+s]<0)
							{
								if (s==m_Series)
								{
									getpeakPoint = j+s;
									hState = DOWNGOING_WAVE;
									break;
								}
							}
						}
					}
				}
			}
			else
			{//负峰落点
				for (j=k+1; j<avgDataSize-1 && hState==UPCOMING_WAVE; ++j)
				{
					if (j>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j]>slopeBased[j])  //2010.08.25 添加平头峰处理条件
						{	
							processed = true;
							continue;
						}
						if (processed && firstOrder[j]<slopeBased[j] && (firstOrder[j]!=0.000000)) 
						{
							for (int s=1; s<=m_Series; ++s)
							{
								if (firstOrder[j+s]>slopeBased[j+s])
									break;
								if (firstOrder[j+s]<slopeBased[j+s]  && (firstOrder[j+s]!=0.000000))
								{
									if (s==m_Series)
									{
										getFinishPoint = j+1;
										hState = STATELESS;
										i = j;
										break;
									}
								}
							}
						}
					}
					if (firstOrder[j]<0)
					{
						getFinishPoint = j;
						hState = STATELESS;
						i = j;  // modify i=j-1;
						break;
					}

				}
				if (hState==STATELESS)
				{
					processed = false;

					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					m_Peak.push_back(OnePeak); //负峰

					unsigned t = m_Peak.size()-1;
					if (t>0 && (m_Peak[t].StartSema.x-m_Peak[t-1].FinSema.x)==0.000000)
					{
						m_Peak[t-1].Type = -mark;// 之前的标志改为连续波谷
						m_Peak[t].Type = -mark;  //连续波谷
					}
					else
					{
						m_Peak[t].Type = -1; //单个波谷
						mark++;
					}
					isNagetiveSpike = false;
				}
			}
			break;

		case DOWNGOING_WAVE: //下行波
			if (!isNagetiveSpike)
			{
				for (k=j; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}

					if ( firstOrder[k]<0 ) 
					{
						if (fabs(firstOrder[k])>slopeBased[k])  //2010.08.24 添加平头峰处理条件
						{
							processed = true;
							continue;
						}

						if (processed && (fabs(firstOrder[k]) < slopeBased[k]) && (firstOrder[k]!=0.000000))
						{
							for (int s=1; s<=m_Series; ++s)
							{
								if (fabs(firstOrder[k+s])>slopeBased[k+s])
									break;
								if (fabs(firstOrder[k+s])<slopeBased[k+s] && (firstOrder[k+s]!=0.000000))
								{
									if (s==m_Series)
									{
										getFinishPoint = k+1;
										hState = STATELESS;
										i = k;
										break;
									}
								}
							}

						}
					}
					if ( firstOrder[k]>0 )
					{
						getFinishPoint = k;
						hState = STATELESS;
						i = k;
						break;
					}
				}
				if (hState==STATELESS)
				{
					processed = false;
					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					m_Peak.push_back(OnePeak);  //正峰

					unsigned t = m_Peak.size()-1;

					if (t>0 && (m_Peak[t].StartSema.x-m_Peak[t-1].FinSema.x)==0.000000)
					{
						m_Peak[t-1].Type = mark;	// 之前的标志改为连续波峰
						m_Peak[t].Type = mark;	//连续波峰
					}
					else
					{
						m_Peak[t].Type = 1; //单个波峰
						mark++;
					}
				}
			}
			else
			{//负峰波谷
				for (k=i+1; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[k]>0)
						continue;

					if (firstOrder[k]<=0)
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (firstOrder[k+s]<0)
								break;
							if (firstOrder[k+s]>0)
							{
								if (s==m_Series)
								{
									getpeakPoint = k+1;
									hState = UPCOMING_WAVE;
									break;
								}
							}
						}
					}
				}
			}
			break;

		default:
			break;
		}
	}
}

// 增强版：对平头峰的处理
void CPeakSeak::SP_GetpeakSearch_Series_plus(const vector<CSema> &avgData, const vector<double> &firstOrder,vector<double> &slopeBased, deque<bool> &hasNagetiveSpike, double thresholdFactor)   
{// step3: 起点→波峰→落点顺序寻峰

	enum enumState {
		STATELESS,			//无状态
		UPCOMING_WAVE,		//上行波
		DOWNGOING_WAVE,		//下行波
		HANDLE_ACCOMPLISH   //处理完毕
	} hState;				//处理过程中的标记

	const double threshold_factor = thresholdFactor; //1.0f; //下行波门限斜率乘积因子
	const unsigned series_factor = 0;

	const unsigned avgDataSize = avgData.size();
	unsigned i=0, j=0, k=0;
	unsigned getStartPoint=0, getpeakPoint=0, getFinishPoint=0;
	int mark = 1; //波类型标记: 1=波峰; -1=波谷; 2,3,4,...为重叠波峰; -2,-3,-4...为重叠波谷
	bool isNagetiveSpike = false;
	bool processed = false;
	bool isTwiceCheck = false;
	hState = STATELESS;
	while (hState != HANDLE_ACCOMPLISH)
	{
		switch (hState)
		{
		case STATELESS:  //无状态
			for ( ; i<avgDataSize-m_Series && hState==STATELESS; ++i)  
			{
				if (i>=avgDataSize-m_Series-1) 
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}

				if (firstOrder[i]<=0)
				{//负峰起点
					if (hasNagetiveSpike[i]) //负峰考虑情况 
					{
						if (fabs(firstOrder[i])<slopeBased[i])
							continue;
						if (fabs(firstOrder[i])>=slopeBased[i])
						{
							for (int s=1; s<=m_Series; ++s)
							{
								if (fabs(firstOrder[i+s])<slopeBased[i])
									break;
								if (fabs(firstOrder[i+s])>slopeBased[i])
								{
									if (s==m_Series)
									{
										isNagetiveSpike = true;
										getStartPoint = i;
										hState = DOWNGOING_WAVE;
										break;
									}
								}
							}
						}
					}
					else
						continue;
				}	

				if (firstOrder[i]>=0) //非负峰考虑情况
				{
					if (firstOrder[i]>slopeBased[i])
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (fabs(firstOrder[i+s])<slopeBased[i+s])
								break;
							if (fabs(firstOrder[i+s])>=slopeBased[i+s])
							{
								if (s==m_Series)
								{
									getStartPoint = i;
									hState = UPCOMING_WAVE;
									break;
								}
							}
						}
					}
				}
			}
			break;

		case UPCOMING_WAVE: //上行波
			if (!isNagetiveSpike)
			{
				for (j=i/*-1*/; j<avgDataSize-1 && hState==UPCOMING_WAVE; ++j)
				{
					if (j>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}

					if (firstOrder[j]<0)
						continue;
					if (firstOrder[j]>=0)
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (firstOrder[j+s]>0)
								break;
							if (firstOrder[j+s]<0)
							{
								if (s==m_Series)
								{
									getpeakPoint = j+s;
									isTwiceCheck = true; // 打开平头峰持续判断开关
									hState = DOWNGOING_WAVE;
									break;
								}
							}
						}
					}
				}
			}
			else
			{//负峰落点
				for (j=k+1; j<avgDataSize-1 && hState==UPCOMING_WAVE; ++j)
				{
					if (j>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j]>slopeBased[j])  //2010.08.25 添加平头峰处理条件
						{	
							processed = true;
							continue;
						}
						if (processed && firstOrder[j]<slopeBased[j] && (firstOrder[j]!=0.000000)) 
						{
							for (int s=1; s<=m_Series; ++s)
							{
								if (firstOrder[j+s]>slopeBased[j+s])
									break;
								if (firstOrder[j+s]<slopeBased[j+s]  && (firstOrder[j+s]!=0.000000))
								{
									if (s==m_Series)
									{
										getFinishPoint = j+1;
										hState = STATELESS;
										i = j;
										break;
									}
								}
							}
						}
					}
					if (firstOrder[j]<0)
					{
						getFinishPoint = j;
						hState = STATELESS;
						i = j;  // modify i=j-1;
						break;
					}

				}
				if (hState==STATELESS)
				{
					processed = false;

					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					m_Peak.push_back(OnePeak); //负峰

					unsigned t = m_Peak.size()-1;
					if (t>0 && (m_Peak[t].StartSema.x-m_Peak[t-1].FinSema.x)==0.000000)
					{
						m_Peak[t-1].Type = -mark;// 之前的标志改为连续波谷
						m_Peak[t].Type = -mark;  //连续波谷
					}
					else
					{
						m_Peak[t].Type = -1; //单个波谷
						mark++;
					}
					isNagetiveSpike = false;
				}
			}
			break;

		case DOWNGOING_WAVE: //下行波
			if (!isNagetiveSpike)
			{
				for (k=j; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}

					if ( firstOrder[k]<0 ) 
					{
						if (!processed) // 进入平头峰判断开关
						{
							if (fabs(firstOrder[k])>slopeBased[k])  //2010.08.24 添加平头峰处理条件
							{
								for (unsigned s=k; s<=k+m_Series && s<avgDataSize; ++s)
								{
									if (fabs(firstOrder[k]) < slopeBased[k])
									{
										processed = false;
										break;
									}
									else
									{
										processed = true;
									}
								}
								//continue;
							}
						}

						if (processed && (fabs(firstOrder[k]) > slopeBased[k]) && (firstOrder[k]!=0.000000)) // 之前<
						{
							int new_series = m_Series+series_factor;
							for (int s=1; s<=new_series; ++s)
							{
								double new_slopeBased = slopeBased[k+s] * threshold_factor;
								if (fabs(firstOrder[k+s])>new_slopeBased)
									break;
								if (fabs(firstOrder[k+s])<new_slopeBased && (firstOrder[k+s]!=0.000000))
								{
									if (s==m_Series)
									{
										getFinishPoint = k+1;
										hState = STATELESS;
										i = k;
										break;
									}
								}
							}

						}
					}
					if (processed && firstOrder[k]>0 )
					{
						getFinishPoint = k;
						hState = STATELESS;
						i = k;
						break;
					}
				}
				if (hState==STATELESS)
				{
					processed = false;
					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					m_Peak.push_back(OnePeak);  //正峰

					processed = false;

					unsigned t = m_Peak.size()-1;

					if (t>0 && (m_Peak[t].StartSema.x-m_Peak[t-1].FinSema.x)==0.000000)
					{
						m_Peak[t-1].Type = mark;	// 之前的标志改为连续波峰
						m_Peak[t].Type = mark;	//连续波峰
					}
					else
					{
						m_Peak[t].Type = 1; //单个波峰
						mark++;
					}
				}
			}
			else
			{//负峰波谷
				for (k=i+1; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-m_Series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[k]>0)
						continue;

					if (firstOrder[k]<=0)
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (firstOrder[k+s]<0)
								break;
							if (firstOrder[k+s]>0)
							{
								if (s==m_Series)
								{
									getpeakPoint = k+1;
									hState = UPCOMING_WAVE;
									break;
								}
							}
						}
					}
				}
			}
			break;

		default:
			break;
		}
	}
}

void CPeakSeak::SP_RestorePeakData(const vector<double> &slopeBased) {
	// step4: 将求出的点还原至元曲线
	const unsigned handleSemaSize = m_OrigSema.size();
	const unsigned peakSemaSize = m_Peak.size();

	for (unsigned i=0; i<peakSemaSize; ++i)  //对于每一个峰
	{
		for (unsigned j=m_Peak[i].StartSema.idx; j<m_Peak[i].StartSema.idx+m_Avgs && j<handleSemaSize-1; j++) //先还原起点
		{
			double k=(m_OrigSema[j+1].y-m_OrigSema[j].y)/(m_OrigSema[j+1].x-m_OrigSema[j].x);
			if (fabs(k)>slopeBased[j])
			{
				if (j==m_Peak[i].StartSema.idx+m_Avgs-1)
				{
					m_Peak[i].StartSema = m_OrigSema[j];
					break;
				}
			}
			else
			{
				m_Peak[i].StartSema = m_OrigSema[j];
				break;
			}
		}

		for (unsigned j=m_Peak[i].FinSema.idx; j<m_Peak[i].FinSema.idx+m_Avgs && j<handleSemaSize-1; j++) //再还原落点
		{
			double k=(m_OrigSema[j+1].y-m_OrigSema[j].y)/(m_OrigSema[j+1].x-m_OrigSema[j].x);
			if (fabs(k)>slopeBased[j])
			{
				if (j==m_Peak[i].FinSema.idx+m_Avgs-1)
				{
					m_Peak[i].FinSema = m_OrigSema[j];
					break;
				}
			}
			else
			{
				m_Peak[i].FinSema = m_OrigSema[j];
				break;
			}
		}

		//最后还原波峰或者波谷点
		double maxValue = -INT_MAX;
		double minValue = INT_MAX;
		if (m_Peak[i].Type>=1) //还原波峰
		{
			for (unsigned j=m_Peak[i].StartSema.idx; j<m_Peak[i].FinSema.idx; ++j)
			{
				if (maxValue <= m_OrigSema[j].y)
				{
					maxValue = m_OrigSema[j].y;
					m_Peak[i].PeakSema = m_OrigSema[j];
				}
			}
		}
		else //还原波谷
		{
			for (unsigned j=m_Peak[i].StartSema.idx; j<m_Peak[i].FinSema.idx; ++j)
			{
				if (minValue >= m_OrigSema[j].y)
				{
					minValue = m_OrigSema[j].y;
					m_Peak[i].PeakSema = m_OrigSema[j];	
				}
			}
		}
	}
}
// 第2次扫描
void CPeakSeak::SP_BanPeak(vector<unsigned> BanPeakNo)
{
	vector<CPeak> buffPeak;
	const unsigned PeakSize = m_Peak.size();
	const unsigned BanPeakNoSize = BanPeakNo.size();
	for (unsigned i=0; i<PeakSize; ++i)
	{
		bool flag = true; //标记点
		for (unsigned j=0; j<BanPeakNoSize; ++j)
		{
			if (i == BanPeakNo[j])
			{
				flag = false; //如果删除峰编号存在，则不放入容器，相当于删除操作
			}
		}
		if (flag)
		{
			buffPeak.push_back(m_Peak[i]); 
		}
	}
	const unsigned buffPeakSize = buffPeak.size();
	if (buffPeakSize!=PeakSize) //发生了删除峰
	{
		m_Peak.swap(buffPeak);
		SP_PeakTypeChange(); //删峰后的峰类型调整
	}
}
void CPeakSeak::SP_PeakTypeChange() {
	const unsigned PeakSize = m_Peak.size();
	if (PeakSize==0) return;
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (  (m_Peak[i].dType == CPeak::PLUSTANGET)
			||(m_Peak[i].dType == CPeak::MINUSTANGET))
		{
			continue;
		}

		if (m_Peak[i].dType == CPeak::PEAKVALLEY)
		{
			m_Peak[i].Type = m_Peak[i].Type/abs(m_Peak[i].Type);
		}

		if (abs(m_Peak[i].Type)!=1) 
		{
			if (m_Peak[i].FinSema==m_Peak[i+1].StartSema)
			{
				continue;
			}
			else /*if (m_Peak[i].FinSema!=m_Peak[i+1].StartSema)*/
			{
				if (i!=0) 
				{  // 不是第一个峰
					if (m_Peak[i].StartSema==m_Peak[i-1].FinSema)
						continue;
			
					if (m_Peak[i].Type==0)
						continue;

					m_Peak[i].Type  = m_Peak[i].Type/abs(m_Peak[i].Type);
					m_Peak[i].dType = CPeak::PEAKVALLEY;
				}
				else
				{
					if (abs(m_Peak[i].Type)!=1)
					{
						m_Peak[i].Type = m_Peak[i].Type/abs(m_Peak[i].Type);
						m_Peak[i].dType = CPeak::PEAKVALLEY;
					}
				}
			}
		}
		else //单峰，把垂直分峰改成峰谷连线
		{
			m_Peak[i].dType = CPeak::PEAKVALLEY;
		}
	}
	// 最后一个峰的情况
	if (PeakSize>=2)
	{
		if (  (m_Peak[PeakSize-1].dType == CPeak::PLUSTANGET)
			||(m_Peak[PeakSize-1].dType == CPeak::MINUSTANGET))
		{
			return;
		}

		if (m_Peak[PeakSize-1].StartSema != m_Peak[PeakSize-2].FinSema)
		{
			m_Peak[PeakSize-1].Type = m_Peak[PeakSize-1].Type/abs(m_Peak[PeakSize-1].Type);
			m_Peak[PeakSize-1].dType = CPeak::PEAKVALLEY;
		}
	}

	// 凡是单峰，起点终点必须在原曲线上 (不然无法修正垂线分峰后的基线校正问题
	for (unsigned i=0; i<PeakSize; ++i){
		if (abs(m_Peak[i].Type)==1){
			unsigned start_idx = m_Peak[i].StartSema.idx;
			m_Peak[i].StartSema = m_OrigSema[start_idx];

			unsigned end_idx = m_Peak[i].FinSema.idx;
			m_Peak[i].FinSema = m_OrigSema[end_idx];
		}

	}

	// 凡是单个垂线分峰，校正为峰谷基线
	for (unsigned i=0; i<PeakSize; ++i){
		if (m_Peak[i].dType == CPeak::VERTDIVIDE){
			int peakType = m_Peak[i].Type;
			int peakType_sum = 0;
			for (unsigned j=0; j<PeakSize; ++j){
				if (m_Peak[j].Type == peakType)
					peakType_sum += peakType;
			}
			int peak_amount = peakType_sum / peakType;
			if (peak_amount == 1)
			{
				m_Peak[i].dType = CPeak::PEAKVALLEY;
				m_Peak[i].Type = abs(m_Peak[i].Type) / m_Peak[i].Type;

				unsigned start_idx = m_Peak[i].StartSema.idx;
				m_Peak[i].StartSema = m_OrigSema[start_idx];

				unsigned end_idx = m_Peak[i].FinSema.idx;
				m_Peak[i].FinSema = m_OrigSema[end_idx];

			}
		}
	}
}
void CPeakSeak::SP_GetHalfPeakWidth() {

	const unsigned PeakSize = m_Peak.size();
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = m_Peak[i].StartSema.x;
		double y1 = m_Peak[i].StartSema.y;
		double x2 = m_Peak[i].FinSema.x;
		double y2 = m_Peak[i].FinSema.y;

		double peakWidth = fabs(x2-x1);

		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;

		double py = m_Peak[i].PeakSema.y; 
		double px = m_Peak[i].PeakSema.x;
		double ty = px*tana+c;

		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;		

		double diff_y = INT_MAX;
		unsigned idxl=0, idxr=0;

		for (unsigned l=m_Peak[i].StartSema.idx; l<=m_Peak[i].PeakSema.idx; ++l) //上行，逼近切点
		{
			double ly = m_OrigSema[l].x * tana + _c;
			if (diff_y >= fabs(m_OrigSema[l].y - ly))
			{
				diff_y = fabs(m_OrigSema[l].y - ly);
				idxl = l;
			}
		}
		CSema tmpFront(m_OrigSema[idxl]); //半峰起点
		m_Peak[i].MidStartSema = tmpFront;

		diff_y = INT_MAX;

		for (unsigned r=m_Peak[i].PeakSema.idx; r<=m_Peak[i].FinSema.idx; ++r) //下行，逼近切点
		{
			double ry = m_OrigSema[r].x * tana + _c;

			if (diff_y >= fabs(m_OrigSema[r].y - ry))
			{
				diff_y = fabs(m_OrigSema[r].y - ry);
				idxr = r;
			}
		}
		CSema tmpLast(m_OrigSema[idxr]);   //半峰落点
		m_Peak[i].MidFinSema = tmpLast;

		double hk = (m_Peak[i].MidFinSema.y-m_Peak[i].MidStartSema.y)/(m_Peak[i].MidFinSema.x-m_Peak[i].MidStartSema.x);
		double hc = m_Peak[i].MidStartSema.y - m_Peak[i].MidStartSema.x * hk;
		double rey = hx*hk+hc;
		CSema HalfPeak(m_Peak[i].PeakSema.idx, hx, rey); // 半峰中点
		m_Peak[i].MidPeakSema = HalfPeak;
	}

	//连接半峰宽直线，与波峰的垂线相交于一点。并求出半峰宽前后长度
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = m_Peak[i].MidStartSema.x; 
		double y1 = m_Peak[i].MidStartSema.y;
		double x2 = m_Peak[i].MidFinSema.x; 
		double y2 = m_Peak[i].MidFinSema.y;

		double halfPeakWidth = fabs(x2-x1);
		m_Peak[i].HalfPeakWidth = halfPeakWidth;  //求半峰宽
	}
}
void CPeakSeak::SP_GetAllHPW()
{//删峰之前将所有的半峰宽赋值给成员变量m_cArrayHPW

	if (m_cArrayHPW.GetSize()!=0)
		m_cArrayHPW.RemoveAll();

	const size_t PeakSize = m_Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		m_cArrayHPW.Add(m_Peak[i].HalfPeakWidth);
	}
}
void CPeakSeak::SP_EraseByHalfPeakWidth(vector<double> &MinHalfWidth)
{
	const unsigned PeakSize = m_Peak.size();
	const unsigned MinHalfWidthSize = MinHalfWidth.size();
	if (PeakSize!=MinHalfWidthSize)	ASSERT(TRUE);

	std::vector<CPeak> buffPeak; //临时波峰容器

	//寻找一个连续峰
	int lastType = 0; //上一次波峰类型

	//遍历整个波峰数据
	for (unsigned i=0; i<PeakSize; ++i)
	{
		bool flag = false; //如果是重叠峰，只有有一个峰大于最小半峰宽所有的都不去除
		unsigned StartMarkPoint = i; //标记连续峰的开始下标
		unsigned FinishMarkPoint = i; //标记连续峰的结束下标

		if (abs(m_Peak[i].Type)==1) //单峰
		{
			if (m_Peak[i].HalfPeakWidth - MinHalfWidth[i] >= 0.000000)//大于最小半峰宽
			{
				buffPeak.push_back(m_Peak[i]);
			}
		}
		else //连续峰
		{
			lastType = m_Peak[i].Type; //记录连续峰起始类型
			while ( lastType == m_Peak[i].Type && i<PeakSize) //直到连续峰结束
			{
				if (m_Peak[i].HalfPeakWidth - MinHalfWidth[i] >= 0.000000) //大于最小半峰宽
				{
					flag = true;
				}
				FinishMarkPoint = i; //改变连续峰的结束下标
				i++;
				if (i==PeakSize) //防止越界
				{
					FinishMarkPoint = i-1;
					break;
				}
			}
			if (flag) //把满足条件的峰装入临时容器
			{
				for (unsigned j = StartMarkPoint; j<=FinishMarkPoint; ++j)
				{
					buffPeak.push_back(m_Peak[j]);
				}
				flag = !flag;
			}
			i = FinishMarkPoint;  //重新定位i的下标
		}
	}

	const unsigned buffPeakSize = buffPeak.size();
	m_MinHalfPeakWidthEarseNum = PeakSize - buffPeakSize;
	if (m_MinHalfPeakWidthEarseNum!=0) //发生了删除峰
	{
		m_Peak.swap(buffPeak);
		SP_PeakTypeChange(); //删峰后的峰类型调整
	}
}
//第3次扫描 
void CPeakSeak::SP_TangentDividPeak(vector<AnalysisConfigArguments::PeakSepaArg> &PeakSepaArgs)
{
	if (PeakSepaArgs.empty())
	{
		return;
	}
	const unsigned ArgSize = PeakSepaArgs.size();
	for (unsigned index = 0; index<ArgSize; ++index)
		//while (!PeakSepaArgs.empty())
	{   
		// 正切
		if (PeakSepaArgs[index].SepaType == 2)
		{
			unsigned start = PeakSepaArgs[index].StartPeakNo; //连续波起点
			unsigned fin = PeakSepaArgs[index].FinishPeakNo;  //连续波终点

			for ( ; PeakSepaArgs[index].StartPeakNo!=PeakSepaArgs[index].FinishPeakNo; ++(PeakSepaArgs[index].StartPeakNo))
			{
				unsigned sp = m_Peak[start].StartSema.idx;
				unsigned ep = m_Peak[fin].FinSema.idx;
				double _k = (m_OrigSema[ep].y-m_OrigSema[sp].y)/(m_OrigSema[ep].x-m_OrigSema[sp].x);
				double _c = m_OrigSema[sp].y- _k*m_OrigSema[sp].x;
				for (unsigned i=m_Peak[start].PeakSema.idx; i<=m_Peak[fin].PeakSema.idx; ++i)
				{
					double _y = _k * m_OrigSema[i].x + _c;
					if (m_Peak[start].Type < 0) //考虑负峰情况
					{
						if ((fabs(_y) < fabs(m_OrigSema[i].y) && _y>0)
							||(fabs(_y) > fabs(m_OrigSema[i].y) && _y>=0))
						{
							start++;
							break;
						}
					}
					else
					{
						if ((fabs(_y) > fabs(m_OrigSema[i].y) && _y>0)//连续峰上有任意点落在基线下
							||(fabs(_y) < fabs(m_OrigSema[i].y) && _y<=0))
						{
							start++;
							break;
						}
					}
				}
			}

			m_Peak[start].FinSema = m_Peak[fin].FinSema;
			//标记分峰类型//
			for (unsigned i=start; i<=fin; ++i){
				if(start==fin) return; //单峰返回
	
				m_Peak[i].dType = CPeak::PLUSTANGET;
				m_Peak[i].Type =  abs(m_Peak[i].Type) / m_Peak[i].Type;
			}
			

			for ( ; start!=fin+1; ++start)
			{

				//校正肩峰的起落点
				unsigned s = m_Peak[start].StartSema.idx;
				unsigned f = m_Peak[fin].FinSema.idx;
				//起点校正
				bool isfail = true;
				for (; s<=m_Peak[start].PeakSema.idx && isfail; ++s)
				{
					double k = (m_OrigSema[f].y-m_OrigSema[s].y)/(m_OrigSema[f].x-m_OrigSema[s].x);
					double c = m_OrigSema[s].y-k*m_OrigSema[s].x;
					for (unsigned m=s; m<=m_Peak[start].PeakSema.idx; ++m) 
					{
						double _y = k * m_OrigSema[m].x+c;
						if (m_Peak[start].Type < 0) //考虑负峰情况
						{
							if (fabs(_y) > fabs(m_OrigSema[m].y))
							{
								isfail = true;
								break;
							}
						}
						else
						{
							if (fabs(_y) > fabs(m_OrigSema[m].y) && _y>0)
							{
								isfail = true;
								break;
							}
							else if(fabs(_y) < fabs(m_OrigSema[m].y) && _y<=0)
							{
								isfail = true;
								break;
							}
						}
						isfail = false;
					}
					if (!isfail)
					{
						//放入起点
						m_Peak[start].StartSema = m_OrigSema[s];
						isfail = false;
					}
				}isfail = true;	//终点校正
				for (; f>=m_Peak[fin].PeakSema.idx && isfail; --f)
				{
					double k = (m_OrigSema[f].y-m_OrigSema[s].y)/(m_OrigSema[f].x-m_OrigSema[s].x);
					double c = m_OrigSema[f].y-k*m_OrigSema[f].x;
					for (unsigned n=f; n>=m_Peak[fin].PeakSema.idx; --n)
					{
						double _y = k * m_OrigSema[n].x + c;
						if (m_Peak[start].Type < 0) //考虑负峰情况
						{
							if (fabs(_y) > fabs(m_OrigSema[n].y))
							{
								isfail = true;
								break;
							}
						}
						else
						{
							if (fabs(_y) > fabs(m_OrigSema[n].y) && _y>0)
							{
								isfail = true;
								break;
							}
							else if(fabs(_y) < fabs(m_OrigSema[n].y) && _y<=0)
							{
								isfail = true;
								break;
							}
						}
						isfail = false;				
					}
					if (!isfail)
					{
						//放入落点
						m_Peak[fin].FinSema = m_OrigSema[f];
						isfail = false;
					}
				}
			}
		}	
		else if (PeakSepaArgs[index].SepaType == 3)
		{
			//负切
			//正切的逆序
			unsigned start = PeakSepaArgs[index].FinishPeakNo; //连续波起点
			unsigned fin = PeakSepaArgs[index].StartPeakNo;  //连续波终点

			for ( ; PeakSepaArgs[index].StartPeakNo!=PeakSepaArgs[index].FinishPeakNo; --(PeakSepaArgs[index].FinishPeakNo))
			{
				unsigned sp = m_Peak[start].FinSema.idx;
				unsigned ep = m_Peak[fin].StartSema.idx;
				double _k = (m_OrigSema[ep].y-m_OrigSema[sp].y)/(m_OrigSema[ep].x-m_OrigSema[sp].x);
				double _c = m_OrigSema[sp].y- _k*m_OrigSema[sp].x;
				for (unsigned i=m_Peak[start].PeakSema.idx; i>=m_Peak[fin].PeakSema.idx; --i)
				{
					double _y = _k * m_OrigSema[i].x + _c;
					if (m_Peak[start].Type < 0) //考虑负峰情况
					{
						if ((fabs(_y) < fabs(m_OrigSema[i].y) && _y>0)
							||(fabs(_y) > fabs(m_OrigSema[i].y) && _y>=0))
						{
							start++;
							break;
						}
					}
					else
					{
						if ((fabs(_y) > fabs(m_OrigSema[i].y) && _y>0)//连续峰上有任意点落在基线下
							||(fabs(_y) < fabs(m_OrigSema[i].y) && _y<=0))
						{
							start--;
							break;
						}
					}
				}
			}

			m_Peak[start].StartSema = m_Peak[fin].StartSema;
			//标记分峰类型//
			for (int i=start; i>=fin; i--){
				if(start==fin) return; //单峰返回
				if (i<0) break;
				m_Peak[i].dType = CPeak::MINUSTANGET;
				m_Peak[i].Type =  abs(m_Peak[i].Type) / m_Peak[i].Type;
			}
			

			for ( ; start!=fin-1; --start)
			{
				if (start<0) continue;
				
				//校正肩峰的起落点
				unsigned s = m_Peak[start].FinSema.idx;
				unsigned f = m_Peak[fin].StartSema.idx;
				//起点校正
				bool isfail = true;
				for (; s>=m_Peak[start].PeakSema.idx && isfail; --s)
				{
					double k = (m_OrigSema[f].y-m_OrigSema[s].y)/(m_OrigSema[f].x-m_OrigSema[s].x);
					double c = m_OrigSema[s].y-k*m_OrigSema[s].x;
					for (unsigned m=s; m>=m_Peak[start].PeakSema.idx; --m) 
					{
						double _y = k * m_OrigSema[m].x+c;
						if (m_Peak[start].Type < 0) //考虑负峰情况
						{
							if (fabs(_y) > fabs(m_OrigSema[m].y))
							{
								isfail = true;
								break;
							}
						}
						else
						{
							if (fabs(_y) > fabs(m_OrigSema[m].y) && _y>0)
							{
								isfail = true;
								break;
							}
							else if(fabs(_y) < fabs(m_OrigSema[m].y) && _y<=0)
							{
								isfail = true;
								break;
							}
						}
						isfail = false;
					}
					if (!isfail)
					{
						//放入起点
						m_Peak[start].FinSema = m_OrigSema[s];
						isfail = false;
					}
				}isfail = true;	//终点校正
				for (; f<=m_Peak[fin].PeakSema.idx && isfail; ++f)
				{
					double k = (m_OrigSema[f].y-m_OrigSema[s].y)/(m_OrigSema[f].x-m_OrigSema[s].x);
					double c = m_OrigSema[f].y-k*m_OrigSema[f].x;
					for (unsigned n=f; n<=m_Peak[fin].PeakSema.idx; ++n)
					{
						double _y = k * m_OrigSema[n].x + c;
						if (m_Peak[start].Type < 0) //考虑负峰情况
						{
							if (fabs(_y) > fabs(m_OrigSema[n].y))
							{
								isfail = true;
								break;
							}
						}
						else
						{
							if (fabs(_y) > fabs(m_OrigSema[n].y) && _y>0)
							{
								isfail = true;
								break;
							}
							else if(fabs(_y) < fabs(m_OrigSema[n].y) && _y<=0)
							{
								isfail = true;
								break;
							}
						}
						isfail = false;				
					}
					if (!isfail)
					{
						//放入落点
						m_Peak[fin].StartSema = m_OrigSema[f];
						isfail = false;
					}
				}
			}
		}
	}
}
void CPeakSeak::SP_GetSeriesPeakDeque(deque<pair<unsigned, unsigned> > &seriesPeak){
	if (!seriesPeak.empty()) seriesPeak.clear();
	const unsigned peakSize = m_Peak.size();
	if (peakSize == 0) return;

	vector<int> peakType;
	for (unsigned i=0; i<peakSize; ++i) 
		peakType.push_back(m_Peak[i].Type);

	for (unsigned i=0; i<peakSize-1; ++i) {
		if (abs(peakType[i])!=1) //find begin 
		{
			for (unsigned j=i+1; j<peakSize; ++j)
			{
				if (peakType[j]!=peakType[i])
				{
					seriesPeak.push_back(std::make_pair(i,j-1));
					i=j-1;
					break;
				}
			}
		}
	}

	//最后一组峰
	unsigned lastPeak = peakSize-1;
	if (abs(peakType[lastPeak])!=1)
	{
		int end_peak_no = lastPeak;
		while (lastPeak - 1 >=0)
		{
			lastPeak -= 1;
			if (lastPeak==0)
			{
				if (peakType[lastPeak]!=peakType[end_peak_no])
				{
					seriesPeak.push_back(std::make_pair(lastPeak+1,end_peak_no));
					return;
				}
				seriesPeak.push_back(std::make_pair(0,end_peak_no));
				return; 
			}
			if (peakType[lastPeak]!=peakType[end_peak_no]){
				seriesPeak.push_back(std::make_pair(lastPeak+1,end_peak_no));
				return; //只取一组
			}
		}
	}
}
void CPeakSeak::SP_CheckPeakToValley(const double p2vRatio, deque<pair<unsigned, unsigned> > &DividePeakId)
{
	if (DividePeakId.empty()) return;

	deque<pair<unsigned, unsigned> > tmpDeque; //定义一个临时容器，最终与输入容器对换

	while (!DividePeakId.empty())
	{
		unsigned bgId = DividePeakId[0].first;
		unsigned edId = DividePeakId[0].second;

		//如果为单峰
		if (bgId >= edId) 
		{
			DividePeakId.pop_front();
			continue;
		}

		// 连续峰
		unsigned PEAKh_Size = edId - bgId + 1;
		unsigned P2VRatio_Size = edId - bgId;
		if (PEAKh_Size > 100000) return; //不考虑大于100000个连续峰的情况  

		vector<double> pPEAKh; //临时峰高
		vector<double> pP2VRatio; //临时峰谷比结果
		vector<CSema> secPoint; //连续峰基线上的点，个数比连续峰波峰数少1

		//连续峰遍历求峰高
		double X1 = m_Peak[bgId].StartSema.x;
		double Y1 = m_Peak[bgId].StartSema.y;
		double X2 = m_Peak[edId].FinSema.x;
		double Y2 = m_Peak[edId].FinSema.y;
		double K = (Y2-Y1)/(X2-X1);
		double C = Y2 - K * X2;
		for (unsigned i=bgId; i<=edId; ++i)
		{
			double XH = m_Peak[i].PeakSema.x;
			double YH = m_Peak[i].PeakSema.y;

			double XR = m_Peak[i].FinSema.x;
			double YR = m_Peak[i].FinSema.y;

			double H = K * XH + C;
			double R = K * XR + C;

			double PeakH = fabs(fabs(YH)-fabs(H));
			double P2VRatio = fabs(fabs(YR)-fabs(R));

			pPEAKh.push_back(PeakH);
			pP2VRatio.push_back(P2VRatio);

			CSema sema(m_Peak[i].FinSema);
			sema.y = R;
			secPoint.push_back(sema);
		}

		//bool bSucceed = false; //是否该组连续峰采用垂直分峰
		for (unsigned i=0; i<pPEAKh.size()-1; i++)
		{
			double H1 = pPEAKh[i];
			double H2 = pPEAKh[i+1];
			double HV = (H1 >= H2) ? H2 : H1;

			pP2VRatio[i] = HV / pP2VRatio[i];

			if (pP2VRatio[i] <= p2vRatio) //峰谷比低于用户指定值：垂直分峰
			{
				//bSucceed = true;
				//break;
				m_Peak[bgId+i].dType = CPeak::VERTDIVIDE;
				m_Peak[bgId+i+1].dType = CPeak::VERTDIVIDE;
				m_Peak[bgId+i].FinSema  = secPoint[i];
				m_Peak[bgId+i+1].StartSema  = secPoint[i];
			}
			else //峰谷比高于用户指定值：峰谷连线
			{
				m_Peak[bgId+i].dType = CPeak::PEAKVALLEY;
			}
		}

		//if (bSucceed)
		//{
		//	std::pair<unsigned, unsigned> p = std::make_pair(bgId, edId);
		//	tmpDeque.push_back(p);
		//}

		DividePeakId.pop_front();
	}

	tmpDeque.swap(DividePeakId);
}
void CPeakSeak::SP_SingleVertDivideBaseLineCheck(unsigned no) {
	unsigned startIdx = m_Peak[no].StartSema.idx;
	unsigned peakIdx  = m_Peak[no].PeakSema.idx;
	unsigned finIdx   = m_Peak[no].FinSema.idx;

	bool isSuss = false;
	bool isChange = false;
	double x1=m_Peak[no].StartSema.x;
	double y1=m_Peak[no].StartSema.y;
	double x2=m_Peak[no].FinSema.x;
	double y2=m_Peak[no].FinSema.y;
	double k=(y2-y1)/(x2-x1);
	double c=y1-k*x1;

	for (unsigned i=finIdx-1;  i>=startIdx+1; --i) {
		double line_y  = k*m_OrigSema[i].x+c;  //基线上
		double curve_y = m_OrigSema[i].y;      //原曲线上

		//判断是否会造成阴影。找到一点则破弃该垂线分峰
		//（2013-05-14 逻辑上的破绽。如果该峰后面连续着垂直分峰，那么应该把起点移动到非阴影处。
		//				如果全部处于基线下方才标记峰谷连线）
		bool isAboveBaseLine = false;
		if (curve_y > 0)
		{
			isAboveBaseLine = (curve_y - line_y >= 0.0001f) ? true: false;
		}
		else
		{
			isAboveBaseLine = (fabs(curve_y) - fabs(line_y) <= 0.0001f) ? true: false;
		}

		if (!isAboveBaseLine) // 曲线在基线下方
		{
			if (i==startIdx+1)
			{
				isChange = true;
			}
			else
			{
				if (i>=peakIdx)
					isChange = true;
				else
					m_Peak[no].StartSema = m_OrigSema[i];
			}

			break;
		}

	}

	if (isChange) //有重叠阴影部分
	{
		if (m_Peak[no].dType == CPeak::VERTDIVIDE){
			m_Peak[no].dType = CPeak::PEAKVALLEY;
			m_Peak[no].Type = abs(m_Peak[no].Type)/m_Peak[no].Type;
		}

	}


	//for (unsigned i=startIdx+1; i<finIdx; ++i) {
	//	double line_y  = k*m_OrigSema[i].x+c;  //基线上
	//	double curve_y = m_OrigSema[i].y;      //原曲线上

	//	//判断是否会造成阴影。找到一点则破弃该垂线分峰
	//	bool isAboveBaseLine = false;
	//	if (curve_y > 0)
	//	{
	//		isAboveBaseLine = (curve_y - line_y >= 0.0001f) ? true: false;
	//	}
	//	else
	//	{
	//		isAboveBaseLine = (fabs(curve_y) - fabs(line_y) <= 0.0001f) ? true: false;
	//	}
	//	if (isAboveBaseLine)
	//	{
	//		if (i==finIdx-1) 
	//			isSuss = true;
	//		continue;
	//	}

	//	break;
	//}
	//
	//if (!isSuss) //有重叠阴影部分
	//{
	//	if (m_Peak[no].dType == CPeak::VERTDIVIDE){
	//		m_Peak[no].dType = CPeak::PEAKVALLEY;
	//		m_Peak[no].Type = abs(m_Peak[no].Type)/m_Peak[no].Type;
	//	}
	//	
	//}

}
void CPeakSeak::SP_SinglePeakValleyBaseLineCheck(unsigned no) {
	unsigned startIdx = m_Peak[no].StartSema.idx;
	unsigned peakIdx  = m_Peak[no].PeakSema.idx;
	unsigned finIdx   = m_Peak[no].FinSema.idx;

	//单峰前段
	for (unsigned i=startIdx; i<peakIdx; ++i) {
		bool isSuss = false;
		double x1=m_OrigSema[i].x;
		double y1=m_OrigSema[i].y;
		double x2=m_Peak[no].FinSema.x;
		double y2=m_Peak[no].FinSema.y;
		double k=(y2-y1)/(x2-x1);
		double c=y1-k*x1;

		for (unsigned ii=i+1; ii<=peakIdx; ++ii) 
		{
			double line_y= k*m_OrigSema[ii].x+c;
			double curve_y = m_OrigSema[ii].y;

			//这里判断是否会造成阴影。找到一段完全无阴影的波段的那个起点
			
			bool isAboveBaseLine = false;

			if (curve_y > 0)
			{
				isAboveBaseLine = (curve_y - line_y >= 0.0000f) ? true: false;
			}
			else
			{
				isAboveBaseLine = (fabs(curve_y) - fabs(line_y) <= 0.0000f) ? true: false;
			}
			
			if (isAboveBaseLine)
			{
				if (ii==peakIdx) 
					isSuss = true;
				continue;
			}

			break;
		}

		if (isSuss)
		{
			if (m_Peak[no].dType == CPeak::VERTDIVIDE)
			{
			
				int srt_idx = m_Peak[no].StartSema.idx;
				int orig_idx = m_OrigSema[i].idx;
				if (orig_idx != srt_idx)
					m_Peak[no].StartSema = m_OrigSema[i];
			}
			else 
				m_Peak[no].StartSema = m_OrigSema[i];

			break;
		}
		else //有重叠阴影部分
		{
			if (m_Peak[no].dType == CPeak::VERTDIVIDE)
				m_Peak[no].dType = CPeak::PEAKVALLEY;
		}
	}

	//单峰后段
	for (unsigned i=finIdx; i>peakIdx; --i) {
		bool isSuss = false;
		double x1=m_OrigSema[i].x;
		double y1=m_OrigSema[i].y;
		double x2=m_Peak[no].StartSema.x;
		double y2=m_Peak[no].StartSema.y;
		double k=(y2-y1)/(x2-x1);
		double c=y1-k*x1;

		for (unsigned ii=i-1; ii>=peakIdx; --ii) {
			double line_y= k*m_OrigSema[ii].x+c;
			double curve_y = m_OrigSema[ii].y;

			bool isAboveBaseLine = false;

			if (curve_y > 0)
			{
				isAboveBaseLine = (curve_y - line_y >= 0.0000f) ? true: false;
			}
			else
			{
				isAboveBaseLine = (fabs(curve_y) - fabs(line_y) <= 0.0000f) ? true: false;
			}

			if (isAboveBaseLine)
			{
				if (ii==peakIdx) 
					isSuss = true;
				continue;
			}

			break;
		}
		if (isSuss)
		{
			if (m_Peak[no].dType == CPeak::VERTDIVIDE)
			{
				int fin_idx = m_Peak[no].FinSema.idx;
				int orig_idx = m_OrigSema[i].idx;
				if (orig_idx != fin_idx)
					m_Peak[no].FinSema = m_OrigSema[i];
			}
			else
				m_Peak[no].FinSema = m_OrigSema[i];

			break;
		}
		else //有重叠阴影部分
		{
			if (m_Peak[no].dType == CPeak::VERTDIVIDE)
				m_Peak[no].dType = CPeak::PEAKVALLEY;
		}
	}
}
void CPeakSeak::SP_PeakValleyBaseLineCheck(unsigned begin, unsigned end) 
{// 峰谷基线校正算法
	if (begin > end)
	{
		ASSERT(_T("(begin > end)"));
		return;
	}

	const unsigned PeakSize = m_Peak.size();
	unsigned bg = begin;
	unsigned ed = end;
	bool getIndex = false; 

	deque<bool> MinusCheck(PeakSize, false); //分段负峰检测
	for (unsigned i=begin; i<=end; ++i)
	{
		if (m_Peak[i].Type < 0)
		{
			MinusCheck[i] = true;
		}
	}

	for (; begin<=end; ++begin)
	{  
		bg = begin;
		if (m_Peak[bg].Type==1 || m_Peak[bg].Type==-1) //单峰情况
		{
			ed = bg;
			getIndex = true;
		}

		else //重叠波峰
		{
			ed = bg+1;
			int mark = m_Peak[bg].Type;
			for (; ed<PeakSize; ++ed)
			{
				if (m_Peak[ed].Type!=mark || m_Peak[ed].dType!=CPeak::PEAKVALLEY)
				{
					ed--;
					getIndex = true;
					break;
				}
				if (ed+1==PeakSize)
				{
					getIndex = true;
					break;
				}
			}
		}
		if (getIndex) 
		{
			unsigned i = bg;
			unsigned o = ed;
			unsigned j = m_Peak[i].StartSema.idx;
			do 
			{
				// 起点和终点肯定跟原曲线重合
				int srt_idx = m_Peak[i].StartSema.idx;
				double orig_srt_p = m_OrigSema[srt_idx].y;
				m_Peak[i].StartSema.y = orig_srt_p;

				int fin_idx = m_Peak[o].FinSema.idx;
				double orig_fin_p = m_OrigSema[fin_idx].y;
				m_Peak[o].FinSema.y = orig_fin_p;

				SP_SinglePeakValleyBaseLineCheck(i);
				
				i++;	

			} while (i!=o+1);
			getIndex = false;
		}
		begin = ed;
	}
}
void CPeakSeak::SP_DividePeak(vector<AnalysisConfigArguments::PeakSepaArg> &SepaArgs)        
{// 垂直分峰    

	if (SepaArgs.empty()) return;
	//先测试第一组数据
	const unsigned SepaArgsSize = SepaArgs.size();
	for (unsigned ss=0; ss<SepaArgsSize; ++ss)
	{
		if (SepaArgs[ss].SepaType != 1) continue;

		unsigned sPNo = SepaArgs[ss].StartPeakNo;
		unsigned ePNo = SepaArgs[ss].FinishPeakNo;

		bool isplus = true;
		if (m_Peak[sPNo].Type > 0) isplus = true;
		else isplus = false;


loop: //goto循环
		for ( ; sPNo <= ePNo; --ePNo)
		{
			//起点和终点的连线
			double _k = (m_Peak[ePNo].FinSema.y-m_Peak[sPNo].StartSema.y)/(m_Peak[ePNo].FinSema.x-m_Peak[sPNo].StartSema.x);
			double _b = m_Peak[sPNo].StartSema.y - _k*m_Peak[sPNo].StartSema.x;
			for (unsigned i=m_Peak[sPNo].StartSema.idx; i<m_Peak[ePNo].FinSema.idx; ++i)
			{ 
				double Y0 = _k * m_OrigSema[i].x + _b; //YO>y在直线上，YO<y在直线下方，=y则在直线上
				if (isplus) //正峰情况
				{
					if ( (Y0>0 && fabs(Y0) > fabs(m_OrigSema[i].y) ) || 
						(Y0<0 && fabs(Y0) < fabs(m_OrigSema[i].y) ) ) 
					{
						if (sPNo==ePNo)
						{
							sPNo++;
							ePNo=SepaArgs[0].FinishPeakNo;
							goto loop;
						}
						ePNo--;
						goto loop;
					}
				}
				else
				{	//负峰情况
					if (fabs(Y0) > fabs(m_OrigSema[i].y)) 
					{
						if ( (Y0<0 && fabs(Y0) > fabs(m_OrigSema[i].y) ) ||
							(Y0>0 && fabs(Y0) < fabs(m_OrigSema[i].y) ) )
						{
							if (sPNo==ePNo)
							{
								sPNo++;
								ePNo=SepaArgs[0].FinishPeakNo;
								goto loop;
							}
							ePNo--;
							goto loop;
						}
					}
				}
			}

			//点全部在直线上，说明这段峰可以做垂直连线
			unsigned s=sPNo;
			unsigned e=ePNo;
			//获得[s,e]内的峰类型标记
			int typeLab = 0;
			for (unsigned i=s; i<=e; ++i)
			{
				if (abs(m_Peak[i].Type) != 1)
				{
					typeLab = m_Peak[i].Type;
					break;
				}
			}
			for (; s<e; ++s)
			{
				m_Peak[s].dType = CPeak::VERTDIVIDE;
				m_Peak[s+1].dType = CPeak::VERTDIVIDE;
				m_Peak[s].Type = typeLab;
				m_Peak[s+1].Type = typeLab;
				m_Peak[s].FinSema.y = _k*m_Peak[s].FinSema.x+_b;
				m_Peak[s+1].StartSema = m_Peak[s].FinSema;

			}
			if (sPNo==ePNo)
			{
				sPNo++;
				ePNo=SepaArgs[ss].FinishPeakNo;
			}
			else
			{
				sPNo=ePNo;
				ePNo=SepaArgs[ss].FinishPeakNo;
			}
		}
	}
}
void CPeakSeak::SP_CheckOverlapPoint(){
	const double overlap_range = 0.006f;
	
	const int PeakSize = m_Peak.size(); 
	if (PeakSize<=1) return;

	//先找到可用的峰类型起始编号
	int peakno_flag = 0;
	for (int i=0; i<PeakSize; i++){
		int current_peakno_flag = abs(m_Peak[i].Type);
		if (current_peakno_flag >= peakno_flag)
		{
			peakno_flag = current_peakno_flag;
		}
	}
	peakno_flag++;

	for (int i=0; i<PeakSize-1; i++)
	{
		if (m_Peak[i].dType!=CPeak::PEAKVALLEY)
			continue;

		//if (abs(m_Peak[i].Type)!=1)
		//	continue;

		double next_peak_start_x = m_Peak[i+1].StartSema.x;
		double curr_peak_end_x = m_Peak[i].FinSema.x;

		if (next_peak_start_x - curr_peak_end_x <= overlap_range) // 视为重叠
		{
			unsigned orig_start_idx = m_Peak[i].FinSema.idx;
			unsigned orig_end_idx = m_Peak[i+1].StartSema.idx;

			unsigned overlap_idx = 0;
			double overlap_point = static_cast<double>(INT_MAX*1.0f);
			for (unsigned idx=orig_start_idx; idx<=orig_end_idx; idx++)
			{
				double origin_point = m_OrigSema[idx].y;
				if (origin_point <= overlap_point)
				{
					overlap_point = origin_point;
					overlap_idx = idx;
				}
			}

			// 重新标记重叠峰
			m_Peak[i].FinSema = m_OrigSema[overlap_idx];
			m_Peak[i+1].StartSema = m_OrigSema[overlap_idx];

			if (abs(m_Peak[i].Type)!=1){
				m_Peak[i+1].Type = m_Peak[i].Type;
			}
			else{
				m_Peak[i].Type = peakno_flag;
				m_Peak[i+1].Type = peakno_flag;
				peakno_flag++;
			}

			
		}
	}
}
//第4次扫描
void CPeakSeak::SP_SplitPeak(const int index,const double Splitime) {
	//找到分裂时间在元曲线上的点
	unsigned s=m_Peak[index].StartSema.idx;
	unsigned p=m_Peak[index].PeakSema.idx;
	unsigned f=m_Peak[index].FinSema.idx;

	double sx=m_Peak[index].StartSema.x;
	double px=m_Peak[index].PeakSema.x;
	double fx=m_Peak[index].FinSema.x;

	unsigned idx=0; //分裂下标
	double diff = (double)INT_MAX;

	if (Splitime>sx && Splitime<px) //在左端
	{
		for (unsigned i=s; i!=p; ++i){
			if (fabs(m_OrigSema[i].x-Splitime)<diff){
				diff=fabs(m_OrigSema[i].x-Splitime);
				idx=i;
			}
		}
		SP_SplitPeak_Left(index, idx);
	}

	else if (Splitime>px && Splitime<fx) //在右端
	{
		for (unsigned i=p; i!=f; ++i){
			if (fabs(m_OrigSema[i].x-Splitime)<diff){
				diff=fabs(m_OrigSema[i].x-Splitime);
				idx=i;
			}
		}
		SP_SplitPeak_Right(index, idx);
	}

	else
		return;
}
void CPeakSeak::SP_SplitPeak_Left(const int index,const unsigned left) {
	//找到分裂时间在元曲线上的点
	unsigned l=left;
	unsigned s=m_Peak[index].StartSema.idx;

	//寻找分裂峰的波峰
	double height=0.0f;
	unsigned p=0; //分裂峰波峰下标
	for (unsigned i=s; i!=l; ++i){
		if (fabs(m_OrigSema[i].y)>fabs(height)){
			height=m_OrigSema[i].y;
			p=i;
		}
	}
	if (p==0.0f) return;

	//对峰容器的峰序号做校正(先直接插在后面，然后顺序对换
	int findType = 1;
	std::vector<CPeak> buffVect;
	for (unsigned i=0; i<(unsigned)index; ++i)
	{	
		if (abs(m_Peak[i].Type)!=1)
			findType = m_Peak[i].Type;
		buffVect.push_back(m_Peak[i]); //前半部分
	}

	if (findType>0) findType++;
	else findType--;

	CPeak newPeak(findType, m_Peak[index].StartSema, m_OrigSema[p], m_OrigSema[l]); 
	m_Peak[index].StartSema = m_OrigSema[l];
	m_Peak[index].Type = findType;


	//把切线分峰直接作为垂直分峰
	double x1 = newPeak.StartSema.x;	//m_Peak[index].StartSema.x;
	double y1 = newPeak.StartSema.y;	//m_Peak[index].StartSema.y;
	double x2 = m_Peak[index].FinSema.x;//newPeak.FinSema.x;
	double y2 = m_Peak[index].FinSema.y;//newPeak.FinSema.y;
	double k = (y2-y1)/(x2-x1);
	double c = y1 - k * x1;
	double old_x = m_Peak[index].StartSema.x;
	double new_y = old_x * k + c;
	m_Peak[index].StartSema.y = new_y;
	newPeak.FinSema.y = new_y;
	//标记切线分峰为垂直分峰
	m_Peak[index].dType = CPeak::VERTDIVIDE;
	newPeak.dType = CPeak::VERTDIVIDE;

	buffVect.push_back(newPeak);
	buffVect.push_back(m_Peak[index]);
	
	for (unsigned i=index+1; i<m_Peak.size(); ++i)
	{
		if (abs(m_Peak[i].Type)!=1){
			if (m_Peak[i].Type>0) m_Peak[i].Type++;
			else m_Peak[i].Type--;
		}
		buffVect.push_back(m_Peak[i]); //后半部分
	}
	m_Peak.swap(buffVect);
}
void CPeakSeak::SP_SplitPeak_Right(const int index,const unsigned right) {
	//找到分裂时间在元曲线上的点
	unsigned r=right;
	unsigned f=m_Peak[index].FinSema.idx;

	//寻找分裂峰的波峰
	double height=0.0f;
	unsigned p=0; //分裂峰波峰下标
	for (unsigned i=r; i!=f; ++i){
		if (fabs(m_OrigSema[i].y)>fabs(height)){
			height=m_OrigSema[i].y;
			p=i;
		}
	}
	if (p==0.0f) return;

	//对峰容器的峰序号做校正
	int findType = 1;
	std::vector<CPeak> buffVect;
	for (unsigned i=0; i<(unsigned)index; ++i)
	{	if (abs(m_Peak[i].Type)!=1)
	findType = m_Peak[i].Type;
	buffVect.push_back(m_Peak[i]); //前半部分
	}

	if (findType>0) findType++;
	else findType--;

	CPeak newPeak(findType, m_OrigSema[r], m_OrigSema[p], m_Peak[index].FinSema); 
	m_Peak[index].FinSema = m_OrigSema[r];
	//m_Peak[index].PeakSema = leftSema;
	m_Peak[index].Type = findType;


	//把切线分峰直接作为垂直分峰
	double x1 = m_Peak[index].StartSema.x;
	double y1 = m_Peak[index].StartSema.y;
	double x2 = newPeak.FinSema.x;
	double y2 = newPeak.FinSema.y;
	double k = (y2-y1)/(x2-x1);
	double c = y1 - k * x1;
	double old_x = m_Peak[index].FinSema.x;
	double new_y = old_x * k + c;
	m_Peak[index].FinSema.y = new_y;
	newPeak.StartSema.y = new_y;
	//标记切线分峰为垂直分峰
	m_Peak[index].dType = CPeak::VERTDIVIDE;
	newPeak.dType = CPeak::VERTDIVIDE;

	buffVect.push_back(m_Peak[index]);
	buffVect.push_back(newPeak);

	for (unsigned i=index+1; i<m_Peak.size(); ++i)
	{
		if (abs(m_Peak[i].Type)!=1){
			if (m_Peak[i].Type>0) m_Peak[i].Type++;
			else m_Peak[i].Type--;
		}
		buffVect.push_back(m_Peak[i]); //后半部分
	}
	m_Peak.swap(buffVect);
}
bool CPeakSeak::SP_HandPeakSpration(const int index,const double Spra_t)
{
	//找到分峰时刻在原曲线上的点
	if (index < 0 || index >= (int)m_Peak.size())
		return false;
	unsigned SpIndex = INT_MAX; //分峰在原曲线上的点的下标
	double diff = (double)INT_MAX;
	for (unsigned i=m_Peak[index].StartSema.idx; i<m_Peak[index].FinSema.idx; ++i)
	{
		if (fabs(m_OrigSema[i].x - Spra_t) <= diff)
		{
			diff = fabs(m_OrigSema[i].x - Spra_t);
			SpIndex = i;
		}
	}

	if (SpIndex==INT_MAX) return false; //未找到下标

	//正峰
	if (m_Peak[index].Type > 0)
	{
		bool bFindLeft = false;
		bool bFindRight = false;
		CSema leftSema; //左边的峰
		double leftMax = static_cast<double>(-INT_MAX);
		for (unsigned l=SpIndex; l>m_Peak[index].StartSema.idx; --l)
		{//遍历左边，找到y轴变化趋势。如果全是下降点则寻找失败
			if (m_OrigSema[l].y <= m_OrigSema[l-1].y)
			{
				//leftSema = m_OrigSema[l-1];
				bFindLeft = true;
			}
			if (bFindLeft && fabs(m_OrigSema[l].y) >= leftMax) //有变化趋势后才开始找至高点
			{
				leftMax = fabs(m_OrigSema[l].y);
				leftSema = m_OrigSema[l];
			}
		}

		CSema rightSema; //右边的峰
		double rightMax = static_cast<double>(-INT_MAX);
		for (unsigned r=SpIndex; r<m_Peak[index].FinSema.idx-1; ++r)
		{
			if (m_OrigSema[r+1].y >= m_OrigSema[r].y)
			{
				//rightSema = m_OrigSema[r+1];
				bFindRight = true;
			}
			if (bFindRight && fabs(m_OrigSema[r].y) >= rightMax)
			{
				rightMax = fabs(m_OrigSema[r].y);
				rightSema = m_OrigSema[r];
			}
		}
		if (bFindLeft && bFindRight) //可以分峰
		{
			//对手工分峰的时间做校正
			diff = (double)INT_MAX;
			for (unsigned i=leftSema.idx; i<rightSema.idx; ++i)
			{
				if (fabs(m_OrigSema[i].y)<=diff)
				{
					diff = fabs(m_OrigSema[i].y);
					SpIndex = i; //得到正确的分峰时间，原曲线的下标
				}
			}
			//对峰容器的峰序号做校正
			int pType = 1;
			std::vector<CPeak> buffVect;
			for (unsigned i=0; i<(unsigned)index; ++i)
			{	if (m_Peak[i].Type!=1)
			pType = m_Peak[i].Type;
			buffVect.push_back(m_Peak[i]); //前半部分
			}

			CPeak addOne(pType+1, m_OrigSema[SpIndex], rightSema, m_Peak[index].FinSema); //tmp
			m_Peak[index].FinSema = m_OrigSema[SpIndex];
			m_Peak[index].PeakSema = leftSema;
			m_Peak[index].Type = pType+1;
			//把切线分峰直接作为垂直分峰
			double x1 = m_Peak[index].StartSema.x;
			double y1 = m_Peak[index].StartSema.y;
			double x2 = addOne.FinSema.x;
			double y2 = addOne.FinSema.y;
			double k = (y2-y1)/(x2-x1);
			double c = y1 - k * x1;
			double old_x = m_Peak[index].FinSema.x;
			double new_y = old_x * k + c;
			m_Peak[index].FinSema.y = new_y;
			addOne.StartSema.y = new_y;
			//标记切线分峰为垂直分峰
			m_Peak[index].dType = CPeak::VERTDIVIDE;
			addOne.dType = CPeak::VERTDIVIDE;

			buffVect.push_back(m_Peak[index]);
			buffVect.push_back(addOne);

			for (unsigned i=index+1; i<m_Peak.size(); ++i)
			{
				if (m_Peak[i].Type!=1)
					m_Peak[i].Type += 1;
				buffVect.push_back(m_Peak[i]); //后半部分
			}
			m_Peak.swap(buffVect);
			return true;
		}
	}
	else //负峰
	{
		bool bFindLeft = false;
		bool bFindRight = false;
		CSema leftSema; //左边的峰
		double leftMax = static_cast<double>(-INT_MAX);
		for (unsigned l=SpIndex; l>m_Peak[index].StartSema.idx; --l)
		{//遍历左边，找到y轴变化趋势。如果全是下降点则寻找失败
			if (m_OrigSema[l].y >= m_OrigSema[l-1].y)
			{
				//leftSema = m_OrigSema[l-1];
				bFindLeft = true;
			}
			if (bFindLeft && fabs(m_OrigSema[l].y) >= leftMax) //有变化趋势后才开始找至高点
			{
				leftMax = fabs(m_OrigSema[l].y);
				leftSema = m_OrigSema[l];
			}
		}

		CSema rightSema; //右边的峰
		double rightMax = static_cast<double>(-INT_MAX);
		for (unsigned r=SpIndex; r<m_Peak[index].FinSema.idx-1; ++r)
		{
			if (m_OrigSema[r+1].y <= m_OrigSema[r].y)
			{
				bFindRight = true;
			}
		}
		if (bFindLeft && bFindRight) //可以分峰
		{
			//对手工分峰的时间做校正
			diff = static_cast<double>(-INT_MAX);
			for (unsigned i=leftSema.idx; i<rightSema.idx; ++i)
			{
				if (m_OrigSema[i].y>=diff)
				{
					diff = m_OrigSema[i].y;
					SpIndex = i; //得到正确的分峰时间，原曲线的下标
				}
			}
			//对峰容器的峰序号做校正
			int pType = -1;
			std::vector<CPeak> buffVect;
			for (unsigned i=0; i<(unsigned)index; ++i)
			{	
				if (m_Peak[i].Type!=-1)
					pType = m_Peak[i].Type;
				buffVect.push_back(m_Peak[i]); //前半部分
			}

			CPeak addOne(pType-1, m_OrigSema[SpIndex], rightSema, m_Peak[index].FinSema); //tmp
			m_Peak[index].FinSema = m_OrigSema[SpIndex];
			m_Peak[index].PeakSema = leftSema;
			m_Peak[index].Type = pType-1;
			buffVect.push_back(m_Peak[index]);
			buffVect.push_back(addOne);

			for (unsigned i=index+1; i<m_Peak.size(); ++i)
			{
				if (m_Peak[i].Type!=-1)
					m_Peak[i].Type -= 1;
				buffVect.push_back(m_Peak[i]); //后半部分
			}
			m_Peak.swap(buffVect);
			return true;
		}
	}
	return false;
}
void CPeakSeak::SP_HandAdjustStart(vector<pair<unsigned, double> > &Arg)
{
	const unsigned ArgSize =  Arg.size();
	const unsigned PeakSize = m_Peak.size();
	const unsigned SemaSize = m_OrigSema.size();
	vector<pair<unsigned, double> > buffVect; //临时容器，最后跟Arg内数据对换

	for (unsigned i=0; i<ArgSize; ++i)
	{
		unsigned p = Arg[i].first;         //待调整的峰号
		double dModify_time = Arg[i].second; //调整时间

		//已经找到峰号,对峰号进行判断是否满足调整的条件
		bool clear = false; //是否满足调峰条件

		if (p == 0)//第一个峰的情况
		{
			if (m_Peak[p].PeakSema.x > dModify_time)
				clear = true;
		}
		else //第二个峰以后的情况
		{
			if (abs(m_Peak[p].Type)==1) //单峰
			{
				if ((m_Peak[p].PeakSema.x > dModify_time) && (m_Peak[p-1].FinSema.x <= dModify_time))
					clear = true;
			}
			else
			{
				if ((m_Peak[p].PeakSema.x > dModify_time) && (m_Peak[p-1].PeakSema.x <= dModify_time))
					clear = true;
			}
		}

		if (clear) //手工调峰可以进行
		{
			unsigned left = 0;
			unsigned right = 0;
			if (p == 0)
				right = m_Peak[p].PeakSema.idx;
			else
			{
				left = m_Peak[p-1].PeakSema.idx;
				right = m_Peak[p].PeakSema.idx;
			}

			//找到调峰参数对应原曲线上的点
			for (unsigned index = left; index < right; ++index)
			{
				if ((m_OrigSema[index].x <= dModify_time) && (m_OrigSema[index+1].x >= dModify_time))
				{
					buffVect.push_back(std::make_pair(p, m_OrigSema[index].x)); //当前调峰参数正确，且校正
					//手工调峰
					//if (abs(m_Peak[Ano].Type)==1) //单峰
					m_Peak[p].StartSema = m_OrigSema[index];
					break;
					//else //连续峰
					//{
					//	m_Peak[Ano].StartSema = m_OrigSema[index]; //起点改变
					//	m_Peak[Ano-1].FinSema = m_Peak[Ano].StartSema; //上一个波峰的终点改变
					//}
				}
			}
		}
	}
	Arg.swap(buffVect);
}
void CPeakSeak::SP_HandAdjustFinish(vector<pair<unsigned, double> > &Arg)
{
	const unsigned ArgSize =  Arg.size();
	const unsigned PeakSize = m_Peak.size();
	const unsigned SemaSize = m_OrigSema.size();
	vector<pair<unsigned, double> > buffVect; //临时容器，最后跟Arg内数据对换
	
	if(PeakSize<1) return;//  否则以下就会数组越界[12/28/2012 洪浩]

	for (unsigned i=0; i<ArgSize; ++i)
	{
		unsigned p = Arg[i].first;         //待调整的峰号
		double dModify_time = Arg[i].second; //调整时间

		//已经找到峰号,对峰号进行判断是否满足调整的条件
		bool clear = false; //是否满足调峰条件
		
		if (p == PeakSize-1)//最后一个峰的情况
		{
			if (m_Peak[p].PeakSema.x < dModify_time)
				clear = true;
		}
		else //第一个峰到最后一个峰之间的情况
		{
			if (abs(m_Peak[p].Type)==1) //单峰
			{
				if ((m_Peak[p].PeakSema.x < dModify_time) && (m_Peak[p+1].StartSema.x >= dModify_time))
					clear = true;
			}
			else
			{
				if ((m_Peak[p].PeakSema.x < dModify_time) && (m_Peak[p+1].PeakSema.x >= dModify_time))
					clear = true;
			}
		}
	
		if (clear) //手工调峰可以进行
		{
			unsigned left = 0;
			unsigned right = 0;
			if (p == PeakSize-1)
			{
				left = m_Peak[p].PeakSema.idx;
				right = m_OrigSema.size();
			}
			else
			{
				left = m_Peak[p].PeakSema.idx;
				right = m_Peak[p+1].StartSema.idx;
			}

			//找到调峰参数对应原曲线上的点
			for (unsigned index = left; index < right; ++index)
			{
				if ((m_OrigSema[index].x <= dModify_time) && (m_OrigSema[index+1].x >= dModify_time))
				{
					buffVect.push_back(std::make_pair(p, m_OrigSema[index].x)); //当前调峰参数正确，且校正
					//手工调峰
					//if (abs(m_Peak[Ano].Type)==1) //单峰
					//{
						m_Peak[p].FinSema = m_OrigSema[index];
						break;
					//}
					//else //连续峰
					//{
					//	m_Peak[Ano].FinSema = m_OrigSema[index]; //起点改变
					//	m_Peak[Ano+1].StartSema = m_Peak[Ano].FinSema; //上一个波峰的终点改变
					//	break;
					//}
				}
			}
		}
	}
	Arg.swap(buffVect);
}
void CPeakSeak::SP_HandAdjustPeak(vector<double> &Arg)
{
	const unsigned ArgSize =  Arg.size();
	const unsigned PeakSize = m_Peak.size();
	const unsigned SemaSize = m_OrigSema.size();
	//对手工调峰（峰谷）参数进行遍历
	for (unsigned ArgIndex = 0; ArgIndex<ArgSize; ++ArgIndex)
	{
		for (unsigned i=0; i<PeakSize; ++i) //找到需要调整的峰谷位于哪个波内
		{
			if((Arg[ArgIndex] > m_Peak[i].StartSema.x)
				&&(Arg[ArgIndex] < m_Peak[i].FinSema.x)) //找到
			{
				//还原至元曲线上
				unsigned left = m_Peak[i].StartSema.idx;
				unsigned right = m_Peak[i].FinSema.idx;
				for (unsigned j=left; j<right-1; ++j)
				{
					if ((Arg[ArgIndex] >= m_OrigSema[j].x)
						&&(Arg[ArgIndex] <= m_OrigSema[j+1].x))//还原
					{//是否要精准到具体一个点，对比更相近的那一点
						m_Peak[i].PeakSema = m_OrigSema[j];
						Arg[ArgIndex] = m_OrigSema[j].x; //校正，手工调峰（峰谷）
					}
				}
			}
		}
	}
}
//第5次扫描
bool CPeakSeak::SP_TwoLineIsIntersect(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3)
{ //两条线段是否相交X0X1 AND X1X2
	double x, y;
	double Minx01 = min(x0, x1);
	double Miny01 = min(y0, y1);
	double Minx23 = min(x2, x3);
	double Miny23 = min(y2, y3);
	double Maxx01 = max(x0, x1);
	double Maxy01 = max(y0, y1);
	double Maxx23 = max(x2, x3);
	double Maxy23 = max(y2, y3);

	if(x1!=x0 && x2!=x3)
	{
		double k1 = (y1-y0)/(x1-x0);
		double k2 = (y3-y2)/(x3-x2);
		double Den = (y1-y0)*(x3-x2) - (y3-y2)*(x1-x0);
		if(k1==k2)
		{ //平行不相交
			double d1 = abs(y0*(x1-x0)-x0*(y1-y0)-y2*(x3-x2)+x2*(y3-y2)); //距离公式d = abs(c1-c2) / sqrt(a*a+b*b)
			if(d1==0)
			{//直线重合
				if((x2>Minx01 && x2<Maxy01 && y2>Miny01 && y2<Maxy01) || (x3>Minx01 && x3<Maxy01 && y3>Miny01 && y3<Maxy01)
					|| (x0>Minx23 && x0<Maxy23 && y0>Miny23 && y0<Maxy23) || (x1>Minx23 && x1<Maxy23 && y1>Miny23 && y1<Maxy23))
				{  //实际碰撞问题线段重合认为相交了
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}   
		}
		x = ((y2-y0)*(x1-x0)*(x3-x2)+(y1-y0)*(x3-x2)*x0-(y3-y2)*(x1-x0)*x2)/Den;
		y = ((y1-y0)*(x-x0))/(x1-x0) + y0;

		if(Minx01<=x && x<=Maxx01 && Miny01<=y && y<=Maxy01 && Minx23<=x && x<=Maxx23 && Miny23<=y && y<=Maxy23)
		{
			//InterX = x;
			//InterY = y;
			return true;
		}
	}
	else if(x1==x0 && x2!=x3)
	{
		x = x0;
		y = ((y3-y2)*(x0-x2))/(x3-x2) + y2;
		if(Minx01<=x && x<=Maxx01 && Miny01<=y && y<=Maxy01 && Minx23<=x && x<=Maxx23 && Miny23<=y && y<=Maxy23)
		{
			//InterX = x;
			//InterY = y;
			return true;
		}
	}
	else if(x1!=x0 && x2==x3)
	{
		x = x2;
		y = ((y1-y0)*(x2-x0))/(x1-x0) + y0;
		if(Minx01<=x && x<=Maxx01 && Miny01<=y && y<=Maxy01 && Minx23<=x && x<=Maxx23 && Miny23<=y && y<=Maxy23)
		{
			//InterX = x;
			//InterY = y;
			return true;
		}       
	}
	return false;
}
void CPeakSeak::SP_GetBaseLineArgs(CPeak::DivideType divType, deque<pair<unsigned, unsigned> > &Arg)
{
	if (m_Peak.empty()) return;

	const unsigned ResultSize = m_Peak.size();
	if (ResultSize==1)
	{
		if (m_Peak[0].dType == divType) {
			pair<unsigned, unsigned> p = std::make_pair(0,0); //直接放入第一个峰
			Arg.push_back(p);
		}
		return;
	}
	
	//先把符合分峰条件的序号都归入同一容器内
	vector<unsigned> pIndex;
	for (unsigned i=0; i<ResultSize-1; ++i)
	{
		if (m_Peak[i].dType == divType){

			pIndex.push_back(i);

			if (i+1 == ResultSize-1) //防止如(2,2)的情况
				if (m_Peak[i+1].dType == divType)
					pIndex.push_back(i+1);

			if (m_Peak[i].Type != m_Peak[i+1].Type)
				pIndex.push_back(UINT_MAX);
		}
	}
	pIndex.push_back(UINT_MAX);
	const unsigned IdextSize = pIndex.size();
	if (IdextSize==0) return;

	//设置双端队列deque<unsigned> rIndex
	deque<unsigned> rIndex;

	rIndex.push_front(pIndex[0]);
	for (unsigned i=1; i<IdextSize; ++i)
	{
		if (pIndex[i] - rIndex.back() == 1)
		{
			rIndex.push_back(pIndex[i]);
		}
		else
		{
			if (rIndex.front() == UINT_MAX && rIndex.back()== UINT_MAX){
				rIndex.clear();
				rIndex.push_back(pIndex[i]);
				continue;
			}

			pair<unsigned, unsigned> p = std::make_pair(rIndex.front(), rIndex.back());
			Arg.push_back(p);
			rIndex.clear();
			rIndex.push_back(pIndex[i]);
		}
	}
	if (Arg.empty()) // if (!rIndex.empty())
	{
		if ((rIndex.front()==UINT_MAX) || (rIndex.back()==UINT_MAX))
			return;

		pair<unsigned, unsigned> p = std::make_pair(rIndex.front(), rIndex.back());
		Arg.push_back(p);

	}
}
void CPeakSeak::SP_VertDivideBaseLineCheck(unsigned begin, unsigned end)
{
	const unsigned start_no=begin;
	const unsigned end_no=end;
	//垂直分峰基线校正
	if (begin > end)
	{
		ASSERT(_T("(begin > end)"));
		return;
	}

	// 起点和终点肯定跟原曲线重合
	int srt_idx = m_Peak[begin].StartSema.idx;
	double orig_srt_p = m_OrigSema[srt_idx].y;
	m_Peak[begin].StartSema.y = orig_srt_p;

	int fin_idx = m_Peak[end].FinSema.idx;
	double orig_fin_p = m_OrigSema[fin_idx].y;
	m_Peak[end].FinSema.y = orig_fin_p;

	const unsigned PeakSize = m_Peak.size();
	unsigned bg = begin;
	unsigned ed = end;
	bool getIndex = false; 
	deque<bool> MinusCheck(PeakSize, false); //分段负峰检测
	for (unsigned i=begin; i<=end; ++i)
	{
		if (m_Peak[i].Type < 0)
		{
			MinusCheck[i] = true;
		}
	}
	for (; begin<=end; ++begin)
	{
		bg = start_no;

		if (abs(m_Peak[bg].Type)==1) //单峰情况
		{
			ed = bg;
		}
		else				  //重峰情况
		{
			ed = bg+1;
			int mark = m_Peak[bg].Type;
			for (; ed<PeakSize; ++ed)
			{

				if (m_Peak[ed].Type != mark || m_Peak[ed].dType != CPeak::VERTDIVIDE)
				{
					ed--;
					break;
				}
				if (ed+1 == PeakSize) //modify if (ed == PeakSize)
				{
					break;
				}

			}
		}
		begin = ed;
		for (unsigned i=bg; i<=ed; i++)
			SP_SingleVertDivideBaseLineCheck(i);
		 
	}

	
}
void CPeakSeak::SP_PlusTangetBaseLineCheck(unsigned begin, unsigned end)
{
	for (unsigned i=begin; i<=end; ++i)//对给定的区间编号遍历
	{
		for (unsigned j=end; j>=i; --j)
		{
			if (m_Peak[i].FinSema.x >= m_Peak[j].StartSema.x ) //找到一组完整的正切分峰
			{
				SP_PeakValleyBaseLineCheck(i+1, j);
				//线段一：
				double x0 = m_Peak[i].StartSema.x;
				double y0 = m_Peak[i].StartSema.y;
				double x1 = m_Peak[i].FinSema.x;
				double y1 = m_Peak[i].FinSema.y;
				//线段二：
				double x2 = m_Peak[j].StartSema.x;
				double y2 = m_Peak[j].StartSema.y;
				double x3 = m_Peak[j].FinSema.x;
				double y3 = m_Peak[j].FinSema.y;
				//判断两条线段是否相交（即，主峰跟肩峰是否有重叠）
				if (SP_TwoLineIsIntersect(x0,y0,x1,y1,x2,y2,x3,y3))
				{
					//::AfxMessageBox( _T("线段相交"));
					m_Peak[i].FinSema = m_Peak[j].FinSema;
				}
				i = j;
				break;
			}
		}
	}
}
void CPeakSeak::SP_MinusTangetBaseLineCheck(unsigned begin,unsigned end)
{
	for (int i=end; i!=begin-1; --i)//对给定的区间编号遍历
	{
		if (i<0) break;

		for (unsigned j=begin; j<=i; ++j)
		{
			if (m_Peak[j].FinSema.x >= m_Peak[i].StartSema.x ) //找到一组完整的反切分峰
			{
				CA_PeakValleyBaseLineCheck(m_OrigSema, m_Peak, j, i-1);
				//线段一：
				double x0 = m_Peak[i].StartSema.x;
				double y0 = m_Peak[i].StartSema.y;
				double x1 = m_Peak[i].FinSema.x;
				double y1 = m_Peak[i].FinSema.y;

				//线段二：
				double x2 = m_Peak[j].StartSema.x;
				double y2 = m_Peak[j].StartSema.y;
				double x3 = m_Peak[j].FinSema.x;
				double y3 = m_Peak[j].FinSema.y;
				//判断两条线段是否相交（即，主峰跟肩峰是否有重叠）
				if (TwoLineIsIntersect(x0,y0,x1,y1,x2,y2,x3,y3))
				{
					//::AfxMessageBox( _T("线段相交"));
					m_Peak[i].StartSema = m_Peak[j].StartSema;
				}
				i = j;
				break;
			}
		}
	}
}
void CPeakSeak::SP_MergePeak(vector<pair<unsigned,unsigned> > &Arg)
{//以后步骤跟解析合并峰参数有类似，以后需【优化】
	const unsigned ArgsSize = Arg.size();
	const unsigned SemaSize = m_OrigSema.size();
	unsigned PeakSize = m_Peak.size();

	for (unsigned ai=0; ai<ArgsSize; ++ai)
	{
		unsigned beginNo = Arg[ai].first;
		unsigned endNo = Arg[ai].second;

		CSema startSema(m_Peak[beginNo].StartSema);
		CSema finishSeam(m_Peak[endNo].FinSema);
		CSema peakSema;
		bool isPlus = abs(m_Peak[beginNo].Type)==m_Peak[beginNo].Type? true: false;
		double dPeakMax=static_cast<double>(-1*INT_MAX), dPeakMin=static_cast<double>(INT_MAX);
		unsigned maxNo=-1, minNo=-1;
		for (unsigned i=beginNo; i<=endNo; ++i)
		{
			if (m_Peak[i].PeakSema.y > dPeakMax)
			{
				dPeakMax = m_Peak[i].PeakSema.y;
				maxNo = i;
			}
			if (m_Peak[i].PeakSema.y < dPeakMin)
			{
				dPeakMin = m_Peak[i].PeakSema.y;
				minNo = i;
			}
		}

		if (maxNo==-1 && minNo==-1) return;

		if (isPlus)
			peakSema = m_Peak[maxNo].PeakSema;
		else
			peakSema = m_Peak[minNo].PeakSema;

		//对现有的波峰容器进行整理
		vector<CPeak> buffPeak;
		for (unsigned pi=0; pi!=beginNo; ++pi)
		{
			buffPeak.push_back(m_Peak[pi]);
		}

		CPeak cpeak(1, startSema, peakSema, finishSeam);
		buffPeak.push_back(cpeak);

		PeakSize = m_Peak.size();
		for (unsigned pi=endNo+1; pi<PeakSize; ++pi)
		{
			buffPeak.push_back(m_Peak[pi]);
		}
		m_Peak.swap(buffPeak);

		//峰谷基线校正
		//SP_PeakValleyBaseLineCheck(beginNo,beginNo);

	}
}
void CPeakSeak::SP_AddPeak(deque<ConfigArgument> &AddArgs)
{
	const unsigned OrigSemaSize = m_OrigSema.size();
	while (!AddArgs.empty())
	{
		//1, 找到原曲线上的起始点
		bool bfStart = false;
		CSema StartPoint;
		double starttime = AddArgs[0].BeginTime;
		for (unsigned i=0; i<OrigSemaSize-1; ++i)
		{
			if (m_OrigSema[i].x <= starttime && m_OrigSema[i+1].x >= starttime)
			{
				//starttime = m_OrigSema[i+1].x;
				bfStart = true;
				StartPoint = m_OrigSema[i+1];
				break;
			}
		}

		//2, 找到原曲线上的终止点
		bool bfEnd = false;
		CSema EndPoint;
		double endtime = AddArgs[0].EndTime;
		for (unsigned i=0; i<OrigSemaSize-1; ++i)
		{
			if (m_OrigSema[i].x <= endtime && m_OrigSema[i+1].x >= endtime)
			{
				//endtime = m_OrigSema[i];
				bfEnd = true;
				EndPoint = m_OrigSema[i];
				break;
			}
		}

		//3,找元曲线的波峰或波谷
		CSema m_PeakPoint;
		bool bFindPeak = false;
		double dPeakType = AddArgs[0].InputValue;
		if (bfStart && bfEnd) 
		{

			if (dPeakType > 0.0f) //找波峰
			{

				for (unsigned i = StartPoint.idx; i<EndPoint.idx; ++i)
				{
					if (m_OrigSema[i].y >= m_PeakPoint.y)
					{
						m_PeakPoint = m_OrigSema[i];
						bFindPeak = true;
					}
				}
			}
			else if (dPeakType < 0.0f)
				//找波谷
			{
				m_PeakPoint.y = (double)INT_MAX;
				for (unsigned i = StartPoint.idx; i<EndPoint.idx; ++i)
				{
					if (m_OrigSema[i].y <= m_PeakPoint.y)
					{
						m_PeakPoint = m_OrigSema[i];
						bFindPeak = true;
					}
				}
			}
		}

		//4,把一组3点完整点加入结果容器
		if (bFindPeak)
		{
			CPeak peak(StartPoint, m_PeakPoint, EndPoint);
			//总之增加峰总是视为单峰，若时间区间内出血双峰由用户自行负责
			if (dPeakType - 1 == 0.000000)
			{
				peak.Type = 1;
			}
			else if (dPeakType + 1 == 0.000000)
			{
				peak.Type = -1;
			}
			m_Peak.push_back(peak);

			CA_PeakValleyBaseLineCheck(m_OrigSema, m_Peak, 
				m_Peak.size()-1,
				m_Peak.size()-1); //峰谷基线校正
		}


		AddArgs.pop_front();
	}
	//是否需要对结果重新排序？



	// 重新一轮求各参数
	SP_GetOtherPeakArgs();// 峰的其他参数（畸变因子，拖尾因子）
	SP_GetPeakArea(); //峰面积
	SP_SetPeakAreaUnit(m_Magnification);
	//SP_GetAllMPA();//删峰之前将所有的峰面积赋值给成员变量m_cArrayMPA
	//SP_GetAllMH();
}
void CPeakSeak::SP_AddPeak_One(ConfigArgument &AddArg){
	//先判断当前添加峰参数是否会与之前的峰产生重叠
	const unsigned PeakSize = m_Peak.size();
 
	double dA = AddArg.BeginTime;
	double dB = AddArg.EndTime;
	for (unsigned i=0; i<PeakSize; ++i)  // 跟当前峰判断是否会有重叠
	{
		double dC = m_Peak[i].StartSema.x;
		double dD = m_Peak[i].FinSema.x;

		if (m_AnalysisConfigArg.IsOverlapping(dA, dB, dC, dD))
		{
			return;
		}
	}

	const unsigned OrigSemaSize = m_OrigSema.size();

	//1,找到原曲线上的起始点
	bool bfStart = false;
	CSema StartPoint;
	double starttime = AddArg.BeginTime;
	for (unsigned i=0; i<OrigSemaSize-1; ++i)
	{
		if (m_OrigSema[i].x <= starttime && m_OrigSema[i+1].x >= starttime)
		{
			//starttime = m_OrigSema[i+1].x;
			bfStart = true;
			StartPoint = m_OrigSema[i+1];
			break;
		}
	}

	//2,找到原曲线上的终止点
	bool bfEnd = false;
	CSema EndPoint;
	double endtime = AddArg.EndTime;
	for (unsigned i=0; i<OrigSemaSize-1; ++i)
	{
		if (m_OrigSema[i].x <= endtime && m_OrigSema[i+1].x >= endtime)
		{
			//endtime = m_OrigSema[i];
			bfEnd = true;
			EndPoint = m_OrigSema[i];
			break;
		}
	}

	//3,找元曲线的波峰或波谷
	CSema m_PeakPoint;
	bool bFindPeak = false;
	double dPeakType = AddArg.InputValue;
	if (bfStart && bfEnd)
	{

		if (dPeakType > 0.0f) //找波峰
		{
			m_PeakPoint.y = -(double)INT_MAX;
			for (unsigned i = StartPoint.idx; i<EndPoint.idx; ++i)
			{
				if (m_OrigSema[i].y >= m_PeakPoint.y)
				{
					m_PeakPoint = m_OrigSema[i];
					bFindPeak = true;
				}
			}
		}
		else if (dPeakType < 0.0f)
		//找波谷 
		{
			m_PeakPoint.y = (double)INT_MAX;
			for (unsigned i = StartPoint.idx; i<EndPoint.idx; ++i)
			{
				if (m_OrigSema[i].y <= m_PeakPoint.y)
				{
					m_PeakPoint = m_OrigSema[i];
					bFindPeak = true;
				}
			}
		}
	}

	//4,把一组3点完整点加入结果容器
	if (bFindPeak)
	{
		CPeak peak(StartPoint, m_PeakPoint, EndPoint);
		//总之增加峰总是视为单峰，若时间区间内出血双峰由用户自行负责
		if (dPeakType - 1 == 0.000000)
		{
			peak.Type = 1;
		}
		else if (dPeakType + 1 == 0.000000)
		{
			peak.Type = -1;
		}
		m_Peak.push_back(peak);


		SP_PeakValleyBaseLineCheck(m_Peak.size()-1,m_Peak.size()-1); //峰谷基线校正

	}
	 

	//对结果重新排序
	std::sort(m_Peak.begin(), m_Peak.end());

	// 重新一轮求各参数
	SP_GetOtherPeakArgs();// 峰的其他参数（畸变因子，拖尾因子）
	SP_GetPeakArea(); //峰面积
	SP_SetPeakAreaUnit(m_Magnification);
}
//第6次扫描
void CPeakSeak::SP_GetOtherPeakArgs()
{
	const unsigned PeakSize = m_Peak.size();
	double factor = 1.0f; //除数因子
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = m_Peak[i].StartSema.x;
		double y1 = m_Peak[i].StartSema.y;
		double x2 = m_Peak[i].FinSema.x;
		double y2 = m_Peak[i].FinSema.y;

		double peakWidth = fabs(x2-x1);
		m_Peak[i].PeakWidth = peakWidth;  //峰宽

		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;

		double py = m_Peak[i].PeakSema.y; 
		double px = m_Peak[i].PeakSema.x;
		double ty = px*tana+c;

		double peakHeight = fabs(ty - py); //出现负值情况的考虑
		m_Peak[i].PeakHeight = peakHeight; //峰高

		//double PeakWidthLeft =  sqrt((ty/factor-y1/factor)*(ty/factor-y1/factor)+(px-x1)*(px-x1));  
		//double PeakWidthRight = sqrt((y2/factor-ty/factor)*(y2/factor-ty/factor)+(x2-px)*(x2-px));
		double PeakWidthLeft  = fabs(px-x1);
		double PeakWidthRight = fabs(x2-px);
		//double lr = PeakWidthLeft/PeakWidthRight;
		//m_Peak[i].Tailing = lr; //拖尾因子

		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;		

		double diff_y = INT_MAX;
		//unsigned idxl, idxr;
		unsigned idxl=0, idxr=0;

		for (unsigned l=m_Peak[i].StartSema.idx; l<=m_Peak[i].PeakSema.idx; ++l) //上行，逼近切点
		{
			double ly = m_OrigSema[l].x * tana + _c;
			if (diff_y >= fabs(m_OrigSema[l].y - ly))
			{
				diff_y = fabs(m_OrigSema[l].y - ly);
				idxl = l;
			}
		}
		CSema tmpFront(m_OrigSema[idxl]); //半峰起点
		m_Peak[i].MidStartSema = tmpFront;

		diff_y = INT_MAX;

		for (unsigned r=m_Peak[i].PeakSema.idx; r<=m_Peak[i].FinSema.idx; ++r) //下行，逼近切点
		{
			double ry = m_OrigSema[r].x * tana + _c;

			if (diff_y >= fabs(m_OrigSema[r].y - ry))
			{
				diff_y = fabs(m_OrigSema[r].y - ry);
				idxr = r;
			}
		}
		CSema tmpLast(m_OrigSema[idxr]);   //半峰落点
		m_Peak[i].MidFinSema = tmpLast;

		double hk = (m_Peak[i].MidFinSema.y-m_Peak[i].MidStartSema.y)/(m_Peak[i].MidFinSema.x-m_Peak[i].MidStartSema.x);
		double hc = m_Peak[i].MidStartSema.y - m_Peak[i].MidStartSema.x * hk;
		double rey = hx*hk+hc;
		CSema HalfPeak(m_Peak[i].PeakSema.idx, hx, rey); // 半峰中点
		m_Peak[i].MidPeakSema = HalfPeak;

	}

	//连接半峰宽直线，与波峰的垂线相交于一点。并求出半峰宽前后长度
	//const unsigned HalfSize = Half.size();
	//ASSERT(HalfSize==PeakSize);
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = m_Peak[i].MidStartSema.x; 
		double y1 = m_Peak[i].MidStartSema.y;
		double x2 = m_Peak[i].MidFinSema.x; 
		double y2 = m_Peak[i].MidFinSema.y;
		//double halfPeakWidth = sqrt((y2/factor-y1/factor)*(y2/factor-y1/factor)+(x2-x1)*(x2-x1));
		double halfPeakWidth = fabs(x2-x1);
		m_Peak[i].HalfPeakWidth = halfPeakWidth;  //求半峰宽

		double x0 = m_Peak[i].MidPeakSema.x;      //求半峰宽前后长度
		double y0 = m_Peak[i].MidPeakSema.y;
		//double halfPeakWidthLeft =  sqrt((y0/factor-y1/factor)*(y0/factor-y1/factor)+(x0-x1)*(x0-x1));
		//double halfPeakWidthRight = sqrt((y2/factor-y0/factor)*(y2/factor-y0/factor)+(x2-x0)*(x2-x0));
		double halfPeakWidthLeft =  fabs(x0-x1);
		double halfPeakWidthRight = fabs(x2-x0);
		double disfactor = halfPeakWidthLeft/halfPeakWidthRight;
		m_Peak[i].Disfactor = disfactor;  //求畸变因子


		double resTime = m_Peak[i].PeakSema.x;
		double w50 = m_Peak[i].HalfPeakWidth;
		double rw_tmp = 0.0000f;
		if (w50 != 0.0000f) 
			rw_tmp = resTime / w50;
		m_Peak[i].ColumnPlate = 5.54 * rw_tmp * rw_tmp; //塔板数
	}

	SP_GetTailFactor();
}
void CPeakSeak::SP_GetTailFactor(){

	const unsigned PeakSize = m_Peak.size();
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = m_Peak[i].StartSema.x;
		double y1 = m_Peak[i].StartSema.y;
		double x2 = m_Peak[i].FinSema.x;
		double y2 = m_Peak[i].FinSema.y;
		double peak_y = m_Peak[i].PeakSema.y; 
		double peak_x = m_Peak[i].PeakSema.x;

		// 基线连线
		double k = (y2-y1)/(x2-x1);
		double c = y1 - x1 * k;
		// 保留时间在基线上的点
		double baseline_y = peak_x * k + c;

		double height_5percent = (peak_y-baseline_y)*0.05 + baseline_y; //5%峰高
		double height_5percent_c = height_5percent - peak_x * k;

		//左交点
		double diff_y = INT_MAX;
		unsigned idxl, idxr; //左右点索引
		for (unsigned l=m_Peak[i].StartSema.idx; l<=m_Peak[i].PeakSema.idx; ++l) //上行，逼近切点
		{
			double ly = m_OrigSema[l].x * k + height_5percent_c;
			if (diff_y >= fabs(m_OrigSema[l].y - ly))
			{
				diff_y = fabs(m_OrigSema[l].y - ly);
				idxl = l;
			}
		}
		CSema left_at_height_5percent(m_OrigSema[idxl]); //5%峰高起点

		//右交点
		diff_y = INT_MAX;
		for (unsigned r=m_Peak[i].PeakSema.idx; r<=m_Peak[i].FinSema.idx; ++r) //下行，逼近切点
		{
			double ry = m_OrigSema[r].x * k + height_5percent_c;
			if (diff_y >= fabs(m_OrigSema[r].y - ry))
			{
				diff_y = fabs(m_OrigSema[r].y - ry);
				idxr = r;
			}
		}
		CSema right_at_height_5percent(m_OrigSema[idxr]); //5%峰高终点


		//W5.0为5%峰高处的峰宽（min），tw为5%峰高处前峰与峰保留时间的时间间隔（min）。
		double w5p0 = right_at_height_5percent.x - left_at_height_5percent.x;
		double tw = peak_x - left_at_height_5percent.x;
		if (tw!=0.0000f)
			m_Peak[i].Tailing =  w5p0 / (tw * 2); //拖尾因子
	}
}

void CPeakSeak::SP_CalcResolution()
{//必须放在半峰宽计算之后

	const unsigned PeakSize = m_Peak.size();
	if (PeakSize == 1) 
	{
		m_Peak[0].Resolution = -1.000f;
		return;
	}

	for (unsigned i=1; i<PeakSize; ++i) 
	{
		double res_time_diff = m_Peak[i].PeakSema.x - m_Peak[i-1].PeakSema.x;
		double half_peak_sum = m_Peak[i].HalfPeakWidth + m_Peak[i-1].HalfPeakWidth;

		if (half_peak_sum == 0.0000f)
		{
			m_Peak[i].Resolution = 0.000f;
		}

		double R = (2.35/2.0) * res_time_diff / half_peak_sum;
		m_Peak[i].Resolution = R;
	}
}

void CPeakSeak::SP_GetPeakArea()
{
	const unsigned PeakSize = m_Peak.size();
	const unsigned SemaSize = m_OrigSema.size();
	//unsigned j = 0;
	for (unsigned i=0; i<PeakSize; ++i)  
	{
		double x2 = m_Peak[i].FinSema.x;           
		double y2 = m_Peak[i].FinSema.y;
		double x1 = m_Peak[i].StartSema.x;
		double y1 = m_Peak[i].StartSema.y;
		double k = (y2-y1)/(x2-x1);
		double c = y1-k*x1;

		double area = 0.0;
		for (unsigned j = 0; j<SemaSize; ++j)
		{
			if (m_OrigSema[j].x-m_Peak[i].StartSema.x >= 0.000000) //找到起点
			{	
				do 
				{
					double x4 = m_OrigSema[j+1].x;
					double y4 = m_OrigSema[j+1].y;
					double x3 = m_OrigSema[j].x;
					double y3 = m_OrigSema[j].y;

					double h = fabs(x4-x3);
					double front = fabs(y3-(x3*k+c));
					double behind = fabs(y4-(x4*k+c));
					area += (front+behind)*h/2;
					j++;
				} while (m_OrigSema[j].x - x2 <= 0.0000f); //找到终点
				m_Peak[i].PeakArea = area;
				break;
			}
		}
	}
	//以上针对每个峰求出面积，但对于切线分峰则需要减去重复计算的那一部分
	deque<pair<unsigned, unsigned> > PlusTangetArgs;  //正切分峰
	deque<pair<unsigned, unsigned> > MinusTangetArgs; //反切分峰
	CA_GetBaseLineArgs(CPeak::PLUSTANGET, m_Peak, PlusTangetArgs);
	while (!PlusTangetArgs.empty())
	{
		deque<pair<unsigned, unsigned> > dequBuff;
		unsigned begin = PlusTangetArgs[0].first;
		unsigned end =  PlusTangetArgs[0].second;
		for (unsigned i = begin; i<=end; ++i)
		{
			if (m_Peak[i].FinSema.x < m_Peak[i+1].StartSema.x)
			{
				std::pair<unsigned, unsigned> p = std::make_pair(begin, i);
				dequBuff.push_back(p);
				begin = i+1;
			}
			if (i==end-1)
			{
				std::pair<unsigned, unsigned> p = std::make_pair(begin, end);
				dequBuff.push_back(p);
				break;
			}
		}
		while (!dequBuff.empty())
		{
			unsigned mainPeakNo = dequBuff[0].first; //主峰编号
			unsigned subFirstPeakNo = dequBuff[0].first+1;   //肩峰的第一个编号
			unsigned subLastPeakNo = dequBuff[0].second; //肩峰的最后一个编号
			for (unsigned i=subFirstPeakNo; i<=subLastPeakNo; ++i)
			{
				if (m_Peak[mainPeakNo].PeakArea < m_Peak[i].PeakArea)
				{
					ASSERT(_T("CPeak::PLUSTANGET (m_Peak[mainPeakNo].PeakArea < m_Peak[i].PeakArea)"));
				}
				m_Peak[mainPeakNo].PeakArea -= m_Peak[i].PeakArea;
			}
			dequBuff.pop_front();
		}
		PlusTangetArgs.pop_front();
	}

	CA_GetBaseLineArgs(CPeak::MINUSTANGET, m_Peak, MinusTangetArgs);
	while (!MinusTangetArgs.empty())
	{
		deque<pair<unsigned, unsigned> > dequBuff;
		unsigned begin = MinusTangetArgs[0].second;
		unsigned end =  MinusTangetArgs[0].first;
		for (int i = begin; i!=end-1; --i)
		{
			if (i<0) continue;
			
			if (m_Peak[i].StartSema.x > m_Peak[i-1].FinSema.x)
			{
				std::pair<unsigned, unsigned> p = std::make_pair(i, begin);
				dequBuff.push_back(p);
				begin = i-1;
			}
			if (i==end+1)
			{
				std::pair<unsigned, unsigned> p = std::make_pair(end, begin);
				dequBuff.push_back(p);
				break;
			}
		}

		while (!dequBuff.empty())
		{
			unsigned mainPeakNo = dequBuff[0].second; //主峰编号
			unsigned subFirstPeakNo = dequBuff[0].second-1;   //肩峰的第一个编号
			unsigned subLastPeakNo = dequBuff[0].first;		//肩峰的最后一个编号
			for (int i=subFirstPeakNo; i!=subLastPeakNo-1; --i)
			{
				if (m_Peak[mainPeakNo].PeakArea < m_Peak[i].PeakArea)
				{
					ASSERT(_T("CPeak::MINUSTANGET (m_Peak[mainPeakNo].PeakArea < m_Peak[i].PeakArea)"));
				}
				m_Peak[mainPeakNo].PeakArea -= m_Peak[i].PeakArea;
			}
			dequBuff.pop_front();
		}
		MinusTangetArgs.pop_front();
	}
}
void CPeakSeak::SP_SetPeakAreaUnit(double factor){
	const unsigned PeakSize = m_Peak.size();
	for (unsigned i=0; i<PeakSize; ++i){
		m_Peak[i].PeakArea =m_Peak[i].PeakArea * factor;
	}
}
void CPeakSeak::SP_GetAllMPA()
{
	if (m_cArrayMPA.GetSize()!=0)
		m_cArrayMPA.RemoveAll();

	const size_t PeakSize = m_Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		m_cArrayMPA.Add(m_Peak[i].PeakArea);
	}
}
void CPeakSeak::SP_GetAllMH()
{
	if (m_cArrayMH.GetSize()!=0)
		m_cArrayMH.RemoveAll();

	const size_t PeakSize = m_Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		m_cArrayMH.Add(m_Peak[i].PeakHeight);
	}
}
void CPeakSeak::SP_EraseByPeakArea(vector<double> &PeakArea)
{//仅删除峰，未对峰进行类型整理
	vector<CPeak> buffPeak;
	const unsigned PeakSize = m_Peak.size();
	const unsigned PeakAreaSize = PeakArea.size();
	if (PeakSize!=PeakAreaSize)
	{
		TRACE(_T("(PeakSize!=PeakAreaSize)"));
		ASSERT(TRUE);
	}
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (fabs(m_Peak[i].PeakArea) > PeakArea[i])
		{
			buffPeak.push_back(m_Peak[i]);
		}
	}
	const unsigned buffPeakSize = buffPeak.size();
	m_MinPeakAreaEarseNum = PeakSize - buffPeakSize;
	if (m_MinPeakAreaEarseNum!=0)
	{
		m_Peak.swap(buffPeak);
		SP_PeakTypeChange();
	}
}
void CPeakSeak::SP_EraseByPeakHeight(vector<double> &PeakHeight)
{
	vector<CPeak> buffPeak;
	const unsigned PeakSize = m_Peak.size();
	const unsigned PeakHeightSize = PeakHeight.size();
	if (PeakSize!=PeakHeightSize) ASSERT(TRUE);

	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (fabs(m_Peak[i].PeakHeight) > PeakHeight[i])
			buffPeak.push_back(m_Peak[i]);
	}
	const unsigned buffPeakSize = buffPeak.size();
	m_MinPeakHeightEarseNum = PeakSize - buffPeakSize;
	if (m_MinPeakHeightEarseNum!=0)
	{
		m_Peak.swap(buffPeak);
		SP_PeakTypeChange();
	}
	
}
void CPeakSeak::SP_DelPeak(deque<ConfigArgument> &delArgs){

	vector<unsigned> DelArgs; //删除峰序号

	//只要删除峰区间内有某个峰的峰顶，则删除这个峰
	unsigned ArgsSize = delArgs.size();
	const unsigned PeakSize = m_Peak.size();

	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (delArgs[i].ArgName == _T("删除峰")) 
		{
			double startTime = delArgs[i].BeginTime;
			double endTime   = delArgs[i].EndTime;
			for (unsigned j=0; j<PeakSize; ++j)
			{
				double peakTime = m_Peak[j].PeakSema.x;

				// 考虑分裂峰起点或终点与波峰重合情况
				double spStartTime = m_Peak[j].StartSema.x;
				double spFinTime = m_Peak[j].FinSema.x;

				// 保留2位小数处理
				int ispPeak  = static_cast<int>((peakTime + 0.005) * 100);
				int ispStart = static_cast<int>((spStartTime+0.005) * 100);
				int ispFin   = static_cast<int>((spFinTime + 0.005) * 100);
				peakTime = (double)ispPeak / 100;
				spStartTime = (double)ispStart / 100;
				spFinTime = (double)ispFin / 100;

				if (( fabs(spStartTime - peakTime)<=0.001f ) || ( fabs(peakTime - spFinTime)<=0.001f ))
				{
					//if ((fabs(spStartTime - startTime)<=0.01f && fabs(spStartTime - endTime)<=0.01f) ||
					//	(fabs(spFinTime   - startTime)<=0.01f && fabs(spFinTime   - endTime)<=0.01f))
					if (((spStartTime >= startTime) && (spStartTime <= endTime)) ||
						((spFinTime   >= startTime) && (spFinTime   <= endTime)))
					{
						DelArgs.push_back(j);
					}
				}
				else //非分裂峰
				{
					if (peakTime >= startTime && peakTime <= endTime)
					{
						DelArgs.push_back(j);
					}
				}
			}
		}
	}

	vector<CPeak> buffPeak;
	//const unsigned PeakSize = m_Peak.size();
	ArgsSize = DelArgs.size();

	for (unsigned i=0; i<PeakSize; ++i)
	{
		bool isRepeat = false;
		for (unsigned j=0; j<ArgsSize; ++j)
			if (i==DelArgs[j])	isRepeat = true;
		if (!isRepeat)	buffPeak.push_back(m_Peak[i]);
		isRepeat = false;
	}

	buffPeak.swap(m_Peak);

	// 如果删除了连续峰中的峰，则重新给峰标记
	SP_PeakTypeChange();

	// 修改峰的起点终点
	//SP_AdjustStartandEndPoint();

}
void CPeakSeak::SP_DelPeak(ConfigArgument &delArg){

	int DelArg = -1; //删除峰序号
	vector<unsigned> DelArgs; //删除峰序号

	//只要删除峰区间内有某个峰的峰顶，则删除这个峰
	const unsigned PeakSize = m_Peak.size();


	if (delArg.ArgName == _T("删除峰"))
	{
		double startTime = delArg.BeginTime;
		double endTime   = delArg.EndTime;
		for (unsigned j=0; j<PeakSize; ++j)
		{
			double peakTime = m_Peak[j].PeakSema.x;

			// 考虑分裂峰起点或终点与波峰重合情况
			double spStartTime = m_Peak[j].StartSema.x;
			double spFinTime = m_Peak[j].FinSema.x;

			// 保留2位小数处理
			int ispPeak  = static_cast<int>((peakTime + 0.005) * 100);
			int ispStart = static_cast<int>((spStartTime+0.005) * 100);
			int ispFin   = static_cast<int>((spFinTime + 0.005) * 100);
			peakTime = (double)ispPeak / 100;
			spStartTime = (double)ispStart / 100;
			spFinTime = (double)ispFin / 100;

			if (( fabs(spStartTime - peakTime)<=0.001f ) || ( fabs(peakTime - spFinTime)<=0.001f ))
			{
				if (((spStartTime >= startTime) && (spStartTime <= endTime)) ||
					((spFinTime   >= startTime) && (spFinTime   <= endTime)))
				{
					DelArgs.push_back(j);
				}
			}
			else //非分裂峰
			{
				if (peakTime >= startTime && peakTime <= endTime)
				{
					DelArgs.push_back(j);
				}
			}
		}
	}

	vector<CPeak> buffPeak;
	//const unsigned PeakSize = m_Peak.size();
	const unsigned ArgsSize = DelArgs.size();

	for (unsigned i=0; i<PeakSize; ++i)
	{
		bool isRepeat = false;
		for (unsigned j=0; j<ArgsSize; ++j)
			if (i==DelArgs[j])	isRepeat = true;
		if (!isRepeat)	buffPeak.push_back(m_Peak[i]);
		isRepeat = false;
	}

	buffPeak.swap(m_Peak);

	// 如果删除了连续峰中的峰，则重新给峰标记
	SP_PeakTypeChange();

	// 修改峰的起点终点
	//SP_AdjustStartandEndPoint();

}
void CPeakSeak::SP_DelPeak_One(ConfigArgument &delArg){

	int DelArg = -1; //删除峰序号

	//只要删除峰区间内有某个峰的峰顶，则删除这个峰
	const unsigned PeakSize = m_Peak.size();


	if (delArg.ArgName == _T("删除峰"))
	{
		double startTime = delArg.BeginTime;
		double endTime   = delArg.EndTime;
		for (unsigned j=0; j<PeakSize; ++j)
		{
			double peakTime = m_Peak[j].PeakSema.x;

			// 考虑分裂峰起点或终点与波峰重合情况
			double spStartTime = m_Peak[j].StartSema.x;
			double spFinTime = m_Peak[j].FinSema.x;

			// 保留2位小数处理
			int ispPeak  = static_cast<int>((peakTime + 0.005) * 100);
			int ispStart = static_cast<int>((spStartTime+0.005) * 100);
			int ispFin   = static_cast<int>((spFinTime + 0.005) * 100);
			peakTime = (double)ispPeak / 100;
			spStartTime = (double)ispStart / 100;
			spFinTime = (double)ispFin / 100;

			if (( fabs(spStartTime - peakTime)<=0.001f ) || ( fabs(peakTime - spFinTime)<=0.001f ))
			{
				if (((spStartTime >= startTime) && (spStartTime <= endTime)) ||
					((spFinTime   >= startTime) && (spFinTime   <= endTime)))
				{
					DelArg = j; 
				}
			}
			else //非分裂峰
			{
				if (peakTime >= startTime && peakTime <= endTime)
				{
					DelArg = j;
				}
			}
		}
	}

	if (DelArg==-1) return;
	
	vector<CPeak> buffPeak;
	
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (i != DelArg)	
			buffPeak.push_back(m_Peak[i]);
	}

	buffPeak.swap(m_Peak);

	// 如果删除了连续峰中的峰，则重新给峰标记
	SP_PeakTypeChange();

	// 修改峰的起点终点
	//SP_AdjustStartandEndPoint();
}
void CPeakSeak::SP_AdjustStartandEndPoint() {
	const unsigned PeakSize = m_Peak.size();
	for (unsigned i=0; i<PeakSize; ++i) {
		//只调整单峰的起点和落点（即，从垂直分峰线上拉回元曲线上）
		if (abs(m_Peak[i].Type)==1) {
			unsigned curve_start_idx = m_Peak[i].StartSema.idx;
			unsigned curve_end_idx = m_Peak[i].FinSema.idx;
			m_Peak[i].StartSema.y = m_OrigSema[curve_start_idx].y;
			m_Peak[i].FinSema.y = m_OrigSema[curve_end_idx].y;
		}
	}
}
void CPeakSeak::SP_GetPeakPercent()
{
	// 求峰的峰面积% 和 峰高%
	const unsigned PeakSize = m_Peak.size();

	double totPeakArea=0.0, totPeakHeigh=0.0;
	for (unsigned i=0; i<PeakSize; ++i)
	{
		totPeakArea += m_Peak[i].PeakArea;
		totPeakHeigh += m_Peak[i].PeakHeight;
	}
	if (totPeakArea==0.0 || totPeakHeigh==0.0)
	{
		ASSERT(totPeakArea==0.0 || totPeakHeigh==0.0);
	}
	for (unsigned i=0; i<PeakSize; ++i)
	{
		m_Peak[i].PeakAreaPercent = m_Peak[i].PeakArea / totPeakArea * 100.0f;
		m_Peak[i].PeakHighPercent = m_Peak[i].PeakHeight / totPeakHeigh * 100.0f;
	}
}
void CPeakSeak::SP_CheckPeakType()
{
	const size_t PeakSize = m_Peak.size();
	for (size_t i=0; i<PeakSize; ++i) {
		if (m_Peak[i].dType == CPeak::VERTDIVIDE){
			//验证采用的垂直分峰标记最后结果是否垂直分峰
			//只要看起点和终点是否落在原曲线上
			int fin_idx = m_Peak[i].FinSema.idx;
			double fin_p = m_Peak[i].FinSema.y;
			double orig_fin_p = m_OrigSema[fin_idx].y;
			bool is_fin_overlap = (fin_p == orig_fin_p) ? true : false;
			if (!is_fin_overlap) // 没有重合，确实是垂直分峰，不必继续判断了
				continue;

			// 继续判断起点
			int srt_idx = m_Peak[i].StartSema.idx;
			double srt_p = m_Peak[i].StartSema.y;
			double orig_srt_p = m_OrigSema[srt_idx].y;
			bool is_srt_overlap = (srt_p == orig_srt_p) ? true : false;
			if (is_srt_overlap) 
				m_Peak[i].dType = CPeak::PEAKVALLEY; //起点，终点都重合，必定是峰谷连线
		}
	}
}
void CPeakSeak::SP_SetPeakType()
{
	const size_t PeakSize = m_Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		//unsigned short negative:				1;	//负峰
		//unsigned short single :				1;	//单峰
		//unsigned short overlap_pl:			1;	//重叠峰
		//unsigned short overlap_ls:			1;	//重叠峰(左肩)
		//unsigned short overlap_rs:			1;	//重叠峰(右肩)
		//unsigned short overlap_vs:			1;	//重叠峰(垂直分峰)
		//unsigned short tail:					1;	//拖尾

		//  PEAKVALLEY, //峰谷基线（初始化）
		//	VERTDIVIDE, //垂直分峰
		//	PLUSTANGET, //正切分峰
		//	MINUSTANGET //反切分峰

		//int    Type;          //峰类型,1=波峰; -1=波谷; >1重叠波峰; <-1重叠波谷
		if (m_Peak[i].Type < 0) m_Peak[i].PeakType.negative = 1;
		if (abs(m_Peak[i].Type) == 1) m_Peak[i].PeakType.single = 1;
		if (m_Peak[i].dType == CPeak::PEAKVALLEY)   m_Peak[i].PeakType.overlap_pl = 1; 
		if (m_Peak[i].dType == CPeak::MINUSTANGET)  m_Peak[i].PeakType.overlap_ls = 1; 
		if (m_Peak[i].dType == CPeak::PLUSTANGET)   m_Peak[i].PeakType.overlap_rs = 1; 
		if (m_Peak[i].dType == CPeak::VERTDIVIDE)   m_Peak[i].PeakType.overlap_vs = 1; 
		if (m_Peak[i].Tailing > TAILING) m_Peak[i].PeakType.tail = 1;
	}
	//给正切切线分峰的非肩峰打上连续峰标记
	for (size_t i=0; i<PeakSize; ++i)
	{
		if (m_Peak[i].dType == CPeak::PLUSTANGET)//就是第一个峰为连续峰
		{
			unsigned bg = i;
			double endx = m_Peak[i].FinSema.x;
			for ( ; i<PeakSize-1; ++i)
			{
				if (endx < m_Peak[i+1].StartSema.x)
				{
					break;
					i--;
				}
			}
			m_Peak[bg].PeakType.overlap_pl = 1; 
		} 
	}

	//给//反切情况分峰的非肩峰打上连续峰标记
	for (int i=PeakSize-1; i>=0; --i)
	{
		if (m_Peak[i].dType == CPeak::MINUSTANGET)
		{
			unsigned bg = i;
			double startx = m_Peak[i].StartSema.x;
			for ( ; i>0; --i)
			{
				if (startx > m_Peak[i-1].StartSema.x)
				{
					break;
					i++;
				}
			}
			m_Peak[bg].PeakType.overlap_pl = 1; 
		} 
	}
}
//void CPeakSeak::SP_GetDividePeakLine() {
//	if (m_cArrayVLine.GetSize()!=0)
//		m_cArrayVLine.RemoveAll();
//
//	const unsigned PeakSize = m_Peak.size();
//	if (PeakSize==0) return;
//	
//	for (unsigned i=0; i<PeakSize; ++i)
//	{
//		if (m_Peak[i].dType == CPeak::VERTDIVIDE) //先判断是否是垂直分峰
//		{
//			structLine vLine_left,vLine_right;
//
//			//左端垂线
//			vLine_left.begin.x=m_Peak[i].StartSema.x;
//			vLine_left.begin.y=m_OrigSema[m_Peak[i].StartSema.idx].y;
//			vLine_left.end.x=m_Peak[i].StartSema.x;
//			vLine_left.end.y=m_Peak[i].StartSema.y;
//
//			//右边垂线
//			vLine_right.begin.x=m_Peak[i].FinSema.x;
//			vLine_right.begin.y=m_OrigSema[m_Peak[i].FinSema.idx].y;
//			vLine_right.end.x=m_Peak[i].FinSema.x;
//			vLine_right.end.y=m_Peak[i].FinSema.y;
//
//			m_cArrayVLine.Add(vLine_left); 
//			m_cArrayVLine.Add(vLine_right); 
//		}
//	}
//}
//最终回扫描
void CPeakSeak::SP_RestoreOriginalData() {
	//把积分结果数据还原至数据移动平均化之前
	const int c_av = 5;
	const size_t peakSize = m_Peak.size();
	const unsigned origPeakSize = m_OrigSema_backup.size();

	for (size_t i=0; i<peakSize; ++i)
	{
		if (m_Peak[i].dType==CPeak::VERTDIVIDE)
		{
			// 波峰还原
			unsigned peak_index = m_Peak[i].PeakSema.idx;
			
			double peak_y = (double)INT_MAX * -1.0;
			for (int j = peak_index-c_av; j<peak_index+c_av; ++j)
			{
				if (j<0 || j>=origPeakSize) continue;
				double orig_peak_y = m_OrigSema_backup[j].y;
				if (peak_y <= orig_peak_y)
				{
					peak_y = orig_peak_y;
					m_Peak[i].PeakSema = m_OrigSema_backup[j];
				}
			}
			

			// 判断是否在曲线上，在就还原

			// 还原起点
			unsigned start_index = m_Peak[i].StartSema.idx;
			double start_y = m_Peak[i].StartSema.y;
			double origin_y = m_OrigSema[start_index].y;
			if (fabs(start_y - origin_y) <= 0.0000001f)
			{
				m_Peak[i].StartSema = m_OrigSema_backup[start_index];

				start_y = (double)INT_MAX * 1.0;
				for (int j = start_index; j<start_index+c_av; ++j){
					if (j<0 || j>=origPeakSize) continue;
					double orig_start_y = m_OrigSema_backup[j].y;
					if (start_y >= orig_start_y){
						start_y = orig_start_y;
						m_Peak[i].StartSema = m_OrigSema_backup[j];
					}
				}
			}
			// 还原终点
			unsigned end_index = m_Peak[i].FinSema.idx;
			double end_y = m_Peak[i].FinSema.y;
			origin_y =  m_OrigSema[end_index].y;
			if (fabs(end_y - origin_y) <= 0.0000001f)
			{
				m_Peak[i].FinSema = m_OrigSema_backup[end_index];

				end_y = (double)INT_MAX * 1.0;
				for (int j = end_index; j<end_index+c_av; ++j){
					if (j<0 || j>=origPeakSize) continue;
					double orig_end_y = m_OrigSema_backup[j].y;
					if (end_y >= orig_end_y){
						end_y = orig_end_y;
						m_Peak[i].FinSema = m_OrigSema_backup[j];
					}
			    }

			}

		}
		else //非垂线分峰，直接还原
		{

			// 波峰还原
			unsigned peak_index = m_Peak[i].PeakSema.idx;

			double peak_y = (double)INT_MAX * -1.0;
			for (int j = peak_index-c_av; j<peak_index+c_av; ++j)
			{
				if (j<0 || j>=origPeakSize) continue;
				double orig_peak_y = m_OrigSema_backup[j].y;
				if (peak_y <= orig_peak_y)
				{
					peak_y = orig_peak_y;
					m_Peak[i].PeakSema = m_OrigSema_backup[j];
				}
			}

			// 还原起点
			unsigned start_index = m_Peak[i].StartSema.idx;
			double start_y = m_Peak[i].StartSema.y;
			double origin_y = m_OrigSema[start_index].y;
			if (fabs(start_y - origin_y) <= 0.0000001f)
			{
				m_Peak[i].StartSema = m_OrigSema_backup[start_index];

				start_y = (double)INT_MAX * 1.0;
				for (int j = start_index; j<start_index+c_av; ++j){
					if (j<0 || j>=origPeakSize) continue;
					double orig_start_y = m_OrigSema_backup[j].y;
					if (start_y >= orig_start_y){
						start_y = orig_start_y;
						m_Peak[i].StartSema = m_OrigSema_backup[j];
					}
				}
			}
			// 还原终点
			unsigned end_index = m_Peak[i].FinSema.idx;
			double end_y = m_Peak[i].FinSema.y;
			origin_y =  m_OrigSema[end_index].y;
			if (fabs(end_y - origin_y) <= 0.0000001f)
			{
				m_Peak[i].FinSema = m_OrigSema_backup[end_index];

				end_y = (double)INT_MAX * 1.0;
				for (int j = end_index; j<end_index+c_av; ++j){
					if (j<0 || j>=origPeakSize) continue;
					double orig_end_y = m_OrigSema_backup[j].y;
					if (end_y >= orig_end_y){
						end_y = orig_end_y;
						m_Peak[i].FinSema = m_OrigSema_backup[j];
					}
				}
			}



		}
	}
}