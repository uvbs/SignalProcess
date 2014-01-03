#include "StdAfx.h"
#include "PeakSeak.h"
#include "WaveletTransform.h"
#include "RealTime.h"
#include <fstream>



CRealTime::CRealTime(void)
{
	m_InputNums = 0;
	m_prevprocess_peak_nums = 0;
	m_id = 0;
	m_isRealTimeParametersSet = false;

	counter=0;

	m_pDrawCurve = NULL;
	m_pLinePeakSeek = NULL;
	m_pPoint = NULL; 
	m_pPeak  = NULL; 
	m_BaseLineColor=RGB(210, 0, 0);
}

CRealTime::CRealTime(CSuperChartCtrl* p)
{
	m_BaseLineColor=RGB(210, 0, 0);

	if (p!=NULL)
	{
		m_pCtrl=p;

//		m_pLinePeakSeek = m_pCtrl->RealDraw(GROUPID_FIRST);
		m_pPoint = m_pCtrl->CreatePointsSerie(); //创建起点终点标点
		m_pPeak  = m_pCtrl->CreatePointsSerie(); //创建波峰标点
//		m_pPoint->SetGroupID(GROUPID_SECOND);
//		m_pPeak->SetGroupID(GROUPID_SECOND);


	}

}
CRealTime::~CRealTime(void)
{

}


VOID CRealTime::GetPeakResult(std::vector<CPeak> &Peak)
{
	Peak = m_Peak;
}
VOID CRealTime::SetSemaDate(CString filePathName)
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
		delete []pX;
		delete []pY;
		return;
	}

	delete []pX;
	delete []pY;

	file.close();	//关闭CSV文件

}
VOID CRealTime::SetSemaDate(double x[], double y[], int n)
{
	dXMax = x[0];
	dXMin = x[0];
	dYMax = y[0];
	dYMin = y[0];
	for (int i=0; i<n; ++i)
	{
		CSema OneSema(i, x[i], y[i]);
		if (x[i] > dXMax) dXMax = x[i];
		if (x[i] < dXMin) dXMin = x[i];	
		if (y[i] > dYMax) dYMax = y[i];
		if (y[i] < dYMin) dYMin = y[i];
		m_OrigSema.push_back(OneSema);
	}
}
VOID CRealTime::SetChartCtrl(CSuperChartCtrl* p)
{
	if ((p!=NULL) && (m_pCtrl != p))
	{
		m_pCtrl = p;

		if (m_pDrawCurve == NULL) 
			m_pDrawCurve = m_pCtrl->CreateLineSerie(false); //  RealDraw(GROUPID_FIRST);
		if (m_pLinePeakSeek == NULL) 
			m_pLinePeakSeek = m_pCtrl->CreateLineSerie(false); //RealDraw(GROUPID_FIRST);
		if (m_pPoint == NULL) 
		{
			m_pPoint = m_pCtrl->CreatePointsSerie(); //创建起点终点标点
//			m_pPoint->SetGroupID(GROUPID_SECOND);
		}
		if (m_pPeak  == NULL) 
		{
			m_pPeak  = m_pCtrl->CreatePointsSerie(); //创建波峰标点
//			m_pPeak->SetGroupID(GROUPID_SECOND);
		}
	}
}
VOID CRealTime::SetParameter(bool IsDividePeak, bool HasNagetiveSpike, double Threshold, 
	double HalfPeakWidth, double PeakArea, int Series=3, int Avgs=5)
{
	m_IsDividePeak=IsDividePeak; //是否开启垂直分峰
	m_HasNagetiveSpike=HasNagetiveSpike; //是否有负峰情况

	m_Threshold=Threshold; //门限斜率
	m_HalfPeakWidth=HalfPeakWidth; //最小半峰宽
	m_PeakArea=PeakArea; //最小峰面积

	m_Series=Series; //内部连续点判断。默认3
	m_Avgs=Avgs; //内部平均点数.默认5
}

VOID CRealTime::ClearLine()
{
	if (counter==0) 
	{
		counter++;
		return;
	}
	//m_pCtrl->EnableRefresh(false);

	//SetShowHide(true, true);
	//m_pCtrl->EnableRefresh(true);
	//SignalDisplay();
	//m_pCtrl->RefreshCtrl();	
	//::AfxMessageBox( _T("---"));
	m_pCtrl->EnableRefresh(false);


	m_pDrawCurve->ClearSerie();
	m_pLinePeakSeek->ClearSerie();          //清空上次曲线
	m_pPoint->ClearSerie();					//清空上次各起点终点
	m_pPeak->ClearSerie();					//清空上次各波峰波谷点
	//m_pDrawCurve = NULL;
	//m_pLinePeakSeek = NULL;
	//m_pPoint = NULL; 
	//m_pPeak  = NULL; 

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

	m_pCtrl->RemoveAllChartStrings();  //清空上次字符串
	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}

VOID CRealTime::ClearSemaDate()
{
	m_OrigSema.clear();
	m_Peak.clear();
}

VOID CRealTime::DrawBaseLine()
{
	if (m_Peak.size()==0)
	{
		return;
	}
	bool isdraw = true;
	const unsigned PeakSize = m_Peak.size();
	if ((!m_IsDividePeak) && (PeakSize!=0)) //非垂直分峰
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
//				m_phemline[i]->SetGroupID(GROUPID_THREE);		
				m_phemline[i]->m_vPoints.InitPoints(4);
				double dx[] = { m_Peak[i].StartSema.x, m_Peak[i].FinSema.x };
				double dy[] = { m_Peak[i].StartSema.y, m_Peak[i].FinSema.y };
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
			if (m_Peak[bg].Type==1 || m_Peak[bg].Type==-1)
			{
				ed = bg;
				isSignalWave = true;
			}
			else /*(Peak[bg].Type>1)*/ //重叠波峰
			{
				ed = bg;
				int mark = m_Peak[bg].Type;
				while (ed<PeakSize && m_Peak[ed].Type==mark)
				{
					if (ed+1==PeakSize && m_Peak[PeakSize-1].Type==mark)
					{
						break;
					}
					ed++;
					if (m_Peak[ed].Type!=mark)
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
				double dx[] = { m_Peak[nodeIndex].StartSema.x, m_Peak[nodeIndex].FinSema.x };
				double dy[] = { m_Peak[nodeIndex].StartSema.y, m_Peak[nodeIndex].FinSema.y };
				nodeIndex++;
				line->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
//				line->SetGroupID(GROUPID_THREE);
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
					double dx[] = { m_Peak[nodeIndex].StartSema.x, m_Peak[nodeIndex].FinSema.x };
					double dy[] = { m_Peak[nodeIndex].StartSema.y, m_Peak[nodeIndex].FinSema.y };
					nodeIndex++;
					line1->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
//					line1->SetGroupID(GROUPID_THREE);
					line1->m_vPoints.InitPoints(4);
					line1->m_vPoints.AddPoints(dx, dy, 2);
					m_phemline.push_back(line1);

					if (bg+1==ed)
					{
						double dxx[] = { m_Peak[nodeIndex].StartSema.x, m_Peak[nodeIndex].FinSema.x };
						double dyy[] = { m_Peak[nodeIndex].StartSema.y, m_Peak[nodeIndex].FinSema.y };
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
VOID CRealTime::DrawDividePeakBaseLine()
{
	if (m_Peak.size()==0)
	{
		return;
	}
	if (!m_IsDividePeak)
	{
		return;
	}

	m_pCtrl->EnableRefresh(false);

	const unsigned PeakSize = m_Peak.size();
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (abs(m_Peak[i].Type)!=1)
		{
			int PeakType = m_Peak[i].Type;
			while (m_Peak[i].Type == PeakType && m_Peak[i+1].Type == PeakType )
			{
				CChartLineSerie* line;  
				line = m_pCtrl->CreateLineSerie();//创建一根基线的连接线
				line->SetColor(m_BaseLineColor/*RGB(210, 0, 0)*/); //设置基线颜色
//				line->SetGroupID(GROUPID_THREE);
				line->m_vPoints.InitPoints(4);
				double dx[] = { m_Peak[i].FinSema.x, m_Peak[i].FinSema.x };
				double dy[] = { m_OrigSema[m_Peak[i].FinSema.idx].y, m_Peak[i].FinSema.y };
				line->m_vPoints.AddPoints(dx, dy, 2);
				m_pDividePeakBaseLine.push_back(line);
				i++;
			}
		}
	}
	m_pCtrl->EnableRefresh(true);
}
 
VOID CRealTime::DrawResultLine()
{
	m_pCtrl->EnableRefresh(false);
	m_pLinePeakSeek->SetColor(RGB(200, 200, 200)); //设置曲线的颜色(白灰)
	CChartAxis* pAxisSelect;

	m_pCtrl->SetBackColor(RGB(60, 60, 60)); //设置背景颜色
	pAxisSelect = m_pCtrl->GetLeftAxis(); //设置Y坐标
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dYMin*1.0, dYMax*1.1, COORDINATE_SET);
	pAxisSelect = m_pCtrl->GetBottomAxis(); //设置X坐标
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dXMin, dXMax, COORDINATE_SET);
	m_pPoint->SetColor(RGB(255, 70, 70)); //设置标起点落点的颜色(红色)
	m_pPeak->SetPointType(CChartPointsSerie::ptTriangle); //设置波峰点是三角标记
	m_pPeak->SetColor (RGB(0, 138, 138)); //设置标波峰波谷点的颜色(兰色)
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetVisible(true);

	m_pLinePeakSeek->m_vPoints.InitPoints(m_OrigSema.size());
	for (size_t i=0; i<m_OrigSema.size();++i)
	{//画曲线
		m_pLinePeakSeek->m_vPoints.AddPoint(m_OrigSema[i].x, m_OrigSema[i].y);
	}

	if (m_Peak.size()==0)
	{
		m_pCtrl->EnableRefresh(true);
		m_pCtrl->RefreshCtrl();	
		return;
	}

	m_pPoint->m_vPoints.InitPoints(m_Peak.size());
	m_pPeak->m_vPoints.InitPoints(m_Peak.size());
	for (unsigned i=0; i<m_Peak.size(); ++i)
	{//标出起点落点
		double x[]={m_Peak[i].StartSema.x, m_Peak[i].FinSema.x};
		double y[]={m_Peak[i].StartSema.y, m_Peak[i].FinSema.y};
		m_pPoint->m_vPoints.AddPoints(x, y, 2);
		m_pPeak->m_vPoints.AddPoint(m_Peak[i].PeakSema.x, m_Peak[i].PeakSema.y);//波峰波谷
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	

}
VOID CRealTime::SignalProc()
{
	m_Peak.clear();
	if (m_OrigSema.size()==0) return;

	CWaveletTransform wt;
	double avgDiff = wt.Reshape_v2(m_OrigSema);   //信号整形等间距使每个数据之间的时间间隔X轴
	if (avgDiff==0) return;

	std::vector<CSema> AvgData;
	//step1: 求以avg为1点的平均曲线，剩余除不尽的为1点
	if ((m_OrigSema.size()/m_Avgs)<9)
	{
		::AfxMessageBox( _T("内部平均点过多，需要重新调整"));
		return;
	}
	new_Averaging(m_OrigSema, AvgData, m_Avgs); 
	const unsigned AvgDataSize = AvgData.size();

	//step2: 求出平均曲线上每一点的斜率
	std::vector<double> FirstOrder;					      //斜率容器
	double maxSlope = 0.0;								  //曲线中最大斜率
	new_GetAvgDataSlope(AvgData, FirstOrder, maxSlope);

	// step3: 起点→波峰→落点顺序寻峰
	test_GetpeakSearch_Series(AvgData, FirstOrder, m_Peak, m_Threshold, m_HasNagetiveSpike, m_Series);

	// step4: 固定点还原至原曲线
	new_RestorePeakData(m_OrigSema, m_Peak, m_Avgs, m_Threshold);  //还原至原曲线

	//-----------------------------------------------------
	// 测试算法：在分峰前先滤除起峰落点相邻2点的
	test_ErasePeakByNextto(m_Peak);
	new_PeakTypeChange(m_Peak); //波类型发生改变
	//-----------------------------------------------------

	// step5: 垂直分峰，基线校正
	if (m_IsDividePeak)
	{
		new_BaselineAdjust(m_OrigSema, m_Peak, m_HasNagetiveSpike);    //基线校正
		test_DividePeak(m_OrigSema, m_Peak);  //new_DividePeak(m_Peak);//垂直分峰
		new_BaselineAdjust(m_OrigSema, m_Peak, m_HasNagetiveSpike);    //基线校正
	}
	else
	{
		new_BaselineCheak(m_OrigSema, m_Peak, m_HasNagetiveSpike);  //基线校正
	}
	new_PeakTypeChange(m_Peak); //波类型发生改变
	new_GetPeakWidth_v2(m_OrigSema, m_Peak, m_IsDividePeak); 
	new_GetPeakArea(m_OrigSema, m_Peak);

	//step6: 按半峰宽长度减少波峰
	new_ErasePeakByHalfWidth(m_Peak, m_HalfPeakWidth);  //根据半峰宽条件删除峰
	new_ErasePeakByPeakArea(m_Peak, m_PeakArea); //根据峰面积条件删除峰
	new_GetTagPoint(m_Peak, m_TagPoint);
}
VOID CRealTime::SignalDisplay()
{
	DrawBaseLine();
	DrawDividePeakBaseLine();
	DrawResultLine();
}
VOID CRealTime::DrawCurve()
{
	m_pCtrl->EnableRefresh(false);

	m_pDrawCurve->SetColor(RGB(200, 200, 200)); //设置曲线的颜色(白灰)
	CChartAxis* pAxisSelect;
	m_pCtrl->SetBackColor(RGB(60, 60, 60)); //设置背景颜色
	pAxisSelect = m_pCtrl->GetLeftAxis(); //设置Y坐标
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dYMin*1.0, dYMax*1.1, COORDINATE_SET);
	pAxisSelect = m_pCtrl->GetBottomAxis(); //设置X坐标
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dXMin, dXMax, COORDINATE_SET);
	pAxisSelect->SetVisible(true);

	m_pDrawCurve->m_vPoints.InitPoints(m_OrigSema.size());
	for (size_t i=0; i<m_OrigSema.size(); ++i)
	{//画曲线
		m_pDrawCurve->m_vPoints.AddPoint(m_OrigSema[i].x, m_OrigSema[i].y);
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	

}

// !-----------------------------------------------------------------
// !
// ! 2010.11.04 脚手架程序测试修改后接口的功能
// !
// !-----------------------------------------------------------------

VOID CRealTime::ClearBaseLine() //清除所有基线
{
	if (counter==0) 
	{
		counter++;
		return;
	}
	m_pCtrl->EnableRefresh(false);
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
	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}
VOID CRealTime::ClearStartFinishPoints() // 清除起点，落点，波峰点
{
	if (counter==0) 
	{
		counter++;
		return;
	}
	m_pCtrl->EnableRefresh(false);
	m_pPoint->ClearSerie();					//清空上次各起点终点
	m_pPeak->ClearSerie();					//清空上次各波峰波谷点
	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}
VOID CRealTime::ClearOrigCurve() // 清空原曲线
{
	if (counter==0) 
	{
		counter++;
		return;
	}
	m_pCtrl->EnableRefresh(false);
	m_pDrawCurve->ClearSerie();
	m_pLinePeakSeek->ClearSerie();          //清空上次曲线
	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}
VOID CRealTime::SetBaseLineColor(COLORREF cr)
{
	m_pCtrl->EnableRefresh(false);
	if (!m_phemline.empty())
	{
		if (m_phemline[0]->GetColor()!=cr)
		{
			for (unsigned i=0; i<m_phemline.size(); i++)
				m_phemline[i]->SetColor(cr); //设置基线颜色;  
		}
	}

	if (!m_pDividePeakBaseLine.empty())
	{
		typedef std::list<CChartLineSerie*>::iterator LI;
		LI iter=m_pDividePeakBaseLine.begin();
		if ((*iter)->GetColor()!=cr)
		{
			for (; iter!=m_pDividePeakBaseLine.end(); iter++)
				(*iter)->SetColor(cr); //设置垂直分峰基线颜色; 
		}
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}

// !-----------------------2010.12.09 -------------------------------
VOID CRealTime::SetRealTimeData(int begin, int end)
{
	if (!m_InputSema.empty())
		m_InputSema.clear();

	for (; begin!=end; ++begin)
	{
		//CRealTimeSema RealTimeSema(m_OrigSema[begin]);
		m_InputSema.push_back( m_OrigSema[begin] );
	}
}
VOID CRealTime::ClearRealTimeCurve() // 清空实时曲线
{
	if (counter==0) 
	{
		counter++;
		return;
	}
	m_pCtrl->EnableRefresh(false);
	m_pDrawCurve->ClearSerie();
	//m_pLinePeakSeek->ClearSerie();          //清空上次曲线
	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}
VOID CRealTime::DrawRealTimeCurve()
{
	m_pCtrl->EnableRefresh(false);

	m_pDrawCurve->SetColor(RGB(200, 200, 200)); //设置曲线的颜色(白灰)
	CChartAxis* pAxisSelect;
	m_pCtrl->SetBackColor(RGB(60, 60, 60)); //设置背景颜色
	pAxisSelect = m_pCtrl->GetLeftAxis(); //设置Y坐标
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dYMin*1.0, dYMax*1.1, COORDINATE_SET);
	pAxisSelect = m_pCtrl->GetBottomAxis(); //设置X坐标
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dXMin, dXMax, COORDINATE_SET);
	pAxisSelect->SetVisible(true);

	m_pDrawCurve->m_vPoints.InitPoints(m_OrigSema.size());
	for (size_t i=0; i<m_RTTotalSema.size(); ++i)
	{//画曲线
		m_pDrawCurve->m_vPoints.AddPoint(m_RTTotalSema[i].x, m_RTTotalSema[i].y);
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	
}
VOID CRealTime::DrawRealTimeResult()
{
	m_pCtrl->EnableRefresh(false);
	//m_pLinePeakSeek->SetColor(RGB(200, 200, 200)); //设置曲线的颜色(白灰)
	CChartAxis* pAxisSelect;

	m_pCtrl->SetBackColor(RGB(60, 60, 60)); //设置背景颜色
	pAxisSelect = m_pCtrl->GetLeftAxis(); //设置Y坐标
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dYMin*1.0, dYMax*1.1, COORDINATE_SET);
	pAxisSelect = m_pCtrl->GetBottomAxis(); //设置X坐标
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(dXMin, dXMax, COORDINATE_SET);
	//m_pPoint->SetColor(RGB(255, 70, 70)); //设置标起点落点的颜色(红色)
	m_pPeak->SetPointType(CChartPointsSerie::ptTriangle); //设置波峰点是三角标记
	m_pPeak->SetColor (RGB(0, 138, 138)); //设置标波峰波谷点的颜色(兰色)
	if (pAxisSelect==NULL) return;
	pAxisSelect->SetVisible(true);

	//m_pLinePeakSeek->m_vPoints.InitPoints(m_OrigSema.size());
	//for (size_t i=0; i<m_OrigSema.size();++i)
	//{//画曲线
	//	m_pLinePeakSeek->m_vPoints.AddPoint(m_OrigSema[i].x, m_OrigSema[i].y);
	//}
	//
	//if (m_Peak.size()==0)
	//{
	//	m_pCtrl->EnableRefresh(true);
	//	m_pCtrl->RefreshCtrl();	
	//	return;
	//}

	//m_pPoint->m_vPoints.InitPoints(m_Peak.size());
	m_pPeak->m_vPoints.Clear();
	m_pPeak->m_vPoints.InitPoints(m_Peak.size());
	for (unsigned i=0; i<m_RTPeak.size(); ++i)
	{//标出起点落点
		//double x[]={m_RTPeak[i].StartSema.x, m_RTPeak[i].FinSema.x};
		//double y[]={m_RTPeak[i].StartSema.y, m_RTPeak[i].FinSema.y};
		//m_pPoint->m_vPoints.AddPoints(x, y, 2);
		m_pPeak->m_vPoints.AddPoint(m_RTPeak[i].x, m_RTPeak[i].y); //point
	}

	m_pCtrl->EnableRefresh(true);
	m_pCtrl->RefreshCtrl();	

}


// !-----------------------2013.04.19 -------------------------------
// !-----------------------实时曲线积分功能接入工作站--------------------
VOID CRealTime::RealTimeProcess(CAnalysisData* pSample_Parameter_PeakProcess, 
						double xSrc[], 
						double ySrc[], 
						int nSrc, 
						CPeakList &cPeakList,
						CPeakList &cAllPeakList)
{
	if (!m_isRealTimeParametersSet)
	{
		RT_SetProcessParameter(pSample_Parameter_PeakProcess);
	}

	RT_AddRealTimePoints(xSrc, ySrc, nSrc);
	
	cPeakList.RemoveAll(); //保证每次输出都是新的实时积分结果数据

	m_InputNums += nSrc;
	if (m_InputNums >= 100) //每100个数据进行一次实时曲线积分
	{
		m_InputNums = 0;
		RT_Averaging();
		RT_AvgDataSlope();
		RT_PeakSearch();
		RT_PeakReturn();
		RT_RealTimeSema2Peak();
		RT_GetPeakArea();
		RT_GetPeakHeight();
		RT_Peak2PeakList(cPeakList, cAllPeakList);
		//RT_Save2PeakList(cPeakList);
	}
}


VOID CRealTime::RT_SetProcessParameter(CAnalysisData* pSample_Parameter_PeakProcess)
{
	double threshold, minHalfPeakWidth, minPeakArea, minPeakHeight;
	CString szAnalysisData = pSample_Parameter_PeakProcess->GetXMLString();
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_TRD, threshold)!=IAERROR_SUCCESS)  return;
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPW, minHalfPeakWidth)!=IAERROR_SUCCESS) return;
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPA, minPeakArea)!=IAERROR_SUCCESS) return;
	if (pSample_Parameter_PeakProcess->GetValue_Double(XMLPATH_PEAKPROC_MPH, minPeakHeight)!=IAERROR_SUCCESS) return;

	// CP 和 AP 参数部分
	int cp = 3; //连续判点
	int ap = 5; //平均点数
	if (pSample_Parameter_PeakProcess->GetValue_Int(XMLPATH_PEAKPROC_CP, cp)!=IAERROR_SUCCESS)  cp = 3;
	if (pSample_Parameter_PeakProcess->GetValue_Int(XMLPATH_PEAKPROC_AP, ap)!=IAERROR_SUCCESS)  ap = 5;
	if (cp>10) cp=10; 
	else if (cp<0)  cp=3;
	if (ap>10) ap=10;
	else if (ap<0) ap=5; 

	RT_SetProcessParameter(false, false, ap, cp, threshold, minHalfPeakWidth, minPeakArea);

	m_isRealTimeParametersSet = true;
}
VOID CRealTime::RT_SetProcessParameter(bool is_divide_peak,
									   bool has_nagetive_spike,
									   int avgs,
									   int series,
									   double threshold,
									   double half_peak_width,
									   double peak_area)
{
	m_IsDividePeak = is_divide_peak; //是否开启垂直分峰
	m_HasNagetiveSpike = has_nagetive_spike; //是否有负峰情况
	m_Avgs = avgs; //内部平均点数.默认5
	m_Series = series; //内部连续点判断。默认3
	m_Threshold = threshold; //门限斜率
	m_HalfPeakWidth = half_peak_width; //最小半峰宽
	m_PeakArea = peak_area; //最小峰面积

	m_maxSlope = 0.0f;
}

VOID CRealTime::RT_AddRealTimePoints(double xSrc[], double ySrc[], int nSrc)
{
	const int orig_sema_size = m_OrigSema.size();
	if (nSrc==0) return;

	for (int i=0; i<nSrc; i++)
	{
		int sema_no = orig_sema_size + i;
		CSema OneSema(sema_no, xSrc[i], ySrc[i]);
		m_OrigSema.push_back(OneSema);
		m_InputSema.push_back(OneSema);
	}
}

BOOL CRealTime::addRealTimePoints(const CCurveData &curveData)
{
	const int orig_sema_size = m_OrigSema.size();
	const int curveSize = curveData.m_arrayData.GetSize(); 
	if (curveSize==0) return FALSE;
	
	for (int i=0; i<curveSize; i++)
	{
		int sema_no = orig_sema_size + i;
		CSema OneSema(sema_no, curveData.m_arrayData[i].dX, curveData.m_arrayData[i].dY);
		m_OrigSema.push_back(OneSema);
		m_InputSema.push_back(OneSema);
	}

	return TRUE;
}

// 实时曲线处理
//static std::deque<CRealTimeSema> dequetmpSema;// 全局静态队列作为平均化缓存
//static std::deque<CRealTimeSema> dequeBufSema;// 全局静态队列作为寻峰缓存


VOID CRealTime::RT_Averaging()
{//step1: 求以m_Avgs为1点的平均曲线，剩余除不尽的为1点
	if (m_Avgs < 1)
	{
		return; //MessageBox(_T("必须平均化"));
	}

	const unsigned TotalNumber = m_RTTotalSema.size();
	const unsigned InputNum = m_InputSema.size(); //输入信号量数量
	const unsigned TmpNum = dequetmpSema.size();  //存队列中信号量数量
	const unsigned CurrNum = InputNum + TmpNum;
	if (CurrNum > m_Avgs) //从队列中取出平均化
	{
		dequetmpSema.insert(dequetmpSema.end(), m_InputSema.begin(), m_InputSema.end());
		m_InputSema.clear();
		for (unsigned i=0; i<=CurrNum; ++i)
		{
			double m_AvgsTime = 0.0;
			double m_AvgsSema = 0.0;
			if (i % m_Avgs == 0 && i+m_Avgs < CurrNum) 
			{
				for (unsigned j=i; j< i+m_Avgs; j++)
				{
					m_AvgsTime += dequetmpSema[0].x;
					m_AvgsSema += dequetmpSema[0].y;
					m_RTTotalSema.push_back(dequetmpSema[0]);
					dequetmpSema.pop_front(); //弹出
				}
				m_AvgsTime = m_AvgsTime / m_Avgs;
				m_AvgsSema = m_AvgsSema / m_Avgs;

				CRealTimeSema OneRTSema(i+TotalNumber, m_AvgsTime, m_AvgsSema);
				m_InputSema.push_back(OneRTSema);
			}
			if (dequetmpSema.size() < m_Avgs) //剩余
			{
				break;
			}
		}
	}
	else //直接放在队列中
	{
		dequetmpSema.insert(dequetmpSema.end(), m_InputSema.begin(), m_InputSema.end());
	}
}
VOID CRealTime::RT_AvgDataSlope()         
{//step2: 求出平均曲线上每一点的斜率

	//以下需要优化
	if (!m_firstOrder.empty())
	{
		const unsigned totsize =  m_RTTotalAvgSema.size()-1;
		double ydiff = m_InputSema[0].y -  m_RTTotalAvgSema[totsize].y;
		double xdiff = m_InputSema[0].x -  m_RTTotalAvgSema[totsize].x;
		double first_k = ydiff / xdiff;
		m_firstOrder.push_back(first_k);
		if (first_k>=m_maxSlope)
			m_maxSlope =first_k;
	}

	const unsigned avgDataSize = m_InputSema.size()-1;
	double *k = new double[avgDataSize];
	for (unsigned i=0; i<avgDataSize; ++i)
	{	
		double ydiff = m_InputSema[i+1].y - m_InputSema[i].y;
		double xdiff = m_InputSema[i+1].x - m_InputSema[i].x;
		k[i] = (ydiff) / (xdiff);
		if (k[i]>=m_maxSlope)
			m_maxSlope = k[i];
		m_firstOrder.push_back(k[i]);
	}

	delete [] k;
	k = NULL;

	for (unsigned i=0; i<m_InputSema.size(); ++i)
	{
		 m_RTTotalAvgSema.push_back(m_InputSema[i]);
	}
}
VOID CRealTime::RT_PeakSearch()
{
	// step3: 起点→波峰→落点顺序寻峰
	enum enumState {
		STATELESS,			//无状态
		//正峰
		PLUS_UPCOMING_WAVE,	 //上行波
		PLUS_DOWNGOING_WAVE, //下行波
		//负峰
		MINUS_DOWNGOING_WAVE,	//下行波
		MINUS_UPCOMING_WAVE,	//上行波

		HANDLE_POSTPROCESS,   //后处理
		HANDLE_ACCOMPLISH     //处理完毕
	} hState;				//处理过程中的标记

	//处理前的数据初始化
	static enumState lastState = STATELESS; //上一次未处理完毕的状态
	//static unsigned m_id = 0;//静态（m_firstOrder下标）
	static bool OnlyOnce = true;
	const unsigned firstOrderSize = m_firstOrder.size();
	unsigned DataSize = 0;
	if (OnlyOnce)
	{
		DataSize = m_InputSema.size() + dequeBufSema.size()-1; //这次进行处理的数据个数
		OnlyOnce = !OnlyOnce;
	} 
	else
		DataSize = m_InputSema.size() + dequeBufSema.size();
	dequeBufSema.insert(dequeBufSema.end(),m_InputSema.begin(),m_InputSema.end());//输入的信号都放到队列末端
	m_InputSema.clear(); //输入信号清空,这步可以放到下一个函数中运行
	hState = lastState;
	unsigned i=0, j=0, k=0;
	unsigned dequeId=0; //队列编号
	bool isContinuousWave = false; //连续峰处理标记
	bool isFlatPeakProcessed = false; //平头峰处理标记

	//unsigned getStartPoint=0, getpeakPoint=0, getFinishPoint=0;
	//int mark = 1; //波类型标记: 1=波峰; -1=波谷; 2,3,4,...为重叠波峰; -2,-3,-4...为重叠波谷
	//bool isNagetiveSpike = false;

	while (hState != HANDLE_ACCOMPLISH)
	{
		switch (hState)
		{
		case STATELESS:  //无状态
			lastState = STATELESS;
			for (; i<DataSize-m_Series && hState==STATELESS; ++i, ++m_id)
			{
				if (i>=DataSize-m_Series-1 || m_id>=firstOrderSize-m_Series) 
				{
					++m_id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if (m_firstOrder[m_id]<=0)
				{//负峰起点
					if (m_HasNagetiveSpike) //考虑负峰情况
					{
						if (fabs(m_firstOrder[m_id])<m_Threshold)
							continue;
						if (fabs(m_firstOrder[m_id])>=m_Threshold)
						{
							for (int s=1; s<=m_Series; ++s)
							{
								if (fabs(m_firstOrder[m_id+s])<m_Threshold)
								{
									isContinuousWave = false; 
									break;
								}
								if (fabs(m_firstOrder[m_id+s])>m_Threshold)
								{
									if (s==m_Series)
									{
										if ((!m_RTPeak.empty())
											&& (m_RTPeak[m_RTPeak.size()-1].pts == CRealTimeSema::MINUS_CONTINUOUS_WAVE)
											&& isContinuousWave)
										{
											//::AfxMessageBox(_T("dequeBufSema.empty()"));
											m_RTPeak.push_back(m_RTPeak[m_RTPeak.size()-1]); //找到(连续)起点
										}
										//非连续峰情况
										else
										{
											dequeBufSema[i].pts = CRealTimeSema::MINUS_START_POINT; //标记为负峰起点
											m_RTPeak.push_back(dequeBufSema[i]); //找到起点

										}
										for (unsigned x=dequeId; x<i; ++x)
											dequeBufSema[x].pts = CRealTimeSema::HANDLED; //标记已处理
										dequeId = i+1;
										hState = MINUS_DOWNGOING_WAVE; //负峰下行波
										break;
									}
								}
							}
						}
					}
					else
						continue;
				}
				if (m_firstOrder[m_id]>=0) //非负峰考虑情况
				{
					if (m_firstOrder[m_id]>m_Threshold)
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (m_firstOrder[m_id+s]<m_Threshold)
							{
								isContinuousWave = false; 
								break;
							}
							if (m_firstOrder[m_id+s]>=m_Threshold)
							{
								if (s==m_Series)
								{
									if ((!m_RTPeak.empty())
										&& (m_RTPeak[m_RTPeak.size()-1].pts == CRealTimeSema::PLUS_CONTINUOUS_WAVE)
										&& isContinuousWave)
									{
										//::AfxMessageBox(_T("dequeBufSema.empty()"));
										m_RTPeak.push_back(m_RTPeak[m_RTPeak.size()-1]); //找到(连续)起点
									}
									//非连续峰情况
									else
									{
										dequeBufSema[i].pts = CRealTimeSema::PLUS_START_POINT; //标记为起点
										m_RTPeak.push_back(dequeBufSema[i]); //找到起点

									}
									for (unsigned x=dequeId; x<i; ++x)
										dequeBufSema[x].pts = CRealTimeSema::HANDLED; //标记已处理
									dequeId = i+1;
									hState = PLUS_UPCOMING_WAVE; //正峰上行波
									break;
								}
							}
						}
					}
				}
			}
			break;

		case PLUS_UPCOMING_WAVE://正峰上行波
			lastState = PLUS_UPCOMING_WAVE;
			for (j=i; j<DataSize-m_Series && hState == PLUS_UPCOMING_WAVE; ++j, ++m_id)
			{

				if (j>=DataSize-m_Series-1 || m_id>=firstOrderSize-m_Series) 
				{
					++m_id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if (m_firstOrder[m_id]<0)
					continue;
				if (m_firstOrder[m_id]>=0)
				{
					for (int s=1; s<=m_Series; ++s)
					{
						if (m_firstOrder[m_id+s]>0)
							break;
						if (m_firstOrder[m_id+s]<0)
						{
							if (s==m_Series)
							{
								dequeBufSema[j+s].pts = CRealTimeSema::PLUS_PEAK_POINT; //标记波峰
								m_RTPeak.push_back(dequeBufSema[j+s]); //找到波峰

								for (unsigned x=dequeId; x<j+s; ++x)
									dequeBufSema[x].pts = CRealTimeSema::PLUS_RAISE_POINT; //标记上升点
								dequeId = j+s+1;
								hState = PLUS_DOWNGOING_WAVE; //正峰下行波
								break;
							}
						}
					}
				}
			}
			break;

		case PLUS_DOWNGOING_WAVE://正峰下行波
			lastState = PLUS_DOWNGOING_WAVE;
			for (k = j; k<DataSize-m_Series && hState == PLUS_DOWNGOING_WAVE; ++k, ++m_id)
			{
				if (k>=DataSize-m_Series-1 || m_id>=firstOrderSize-m_Series) 
				{
					++m_id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if ( m_firstOrder[m_id]<0 ) 
				{
					if (fabs(m_firstOrder[m_id])>m_Threshold)  //2010.08.24 添加平头峰处理条件
					{
						isFlatPeakProcessed = true;
						continue;
					}

					if (isFlatPeakProcessed && (fabs(m_firstOrder[m_id]) < m_Threshold) && (m_firstOrder[m_id]!=0.000000))
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (fabs(m_firstOrder[m_id+s])>m_Threshold)
								break;
							if (fabs(m_firstOrder[m_id+s])<m_Threshold && (m_firstOrder[m_id+s]!=0.000000))
							{
								if (s==m_Series)
								{
									//getFinishPoint = k+1;
									dequeBufSema[k+1].pts = CRealTimeSema::PLUS_FINISH_POINT; //标记终点
									m_RTPeak.push_back(dequeBufSema[k+1]); //找到终点
									for (unsigned x=dequeId; x<k+1; ++x)
										dequeBufSema[x].pts = CRealTimeSema::PLUS_DROP_POINT; //标记下降点
									dequeId = k+2;
									hState = STATELESS;
									i = k;
									break;
								}
							}
						}

					}
				}
				if ( m_firstOrder[m_id]>0 )
				{
					//getFinishPoint = k;
					dequeBufSema[k+1].pts = CRealTimeSema::PLUS_CONTINUOUS_WAVE; //标记（连续）终点
					m_RTPeak.push_back(dequeBufSema[k+1]); //找到终点
					for (unsigned x=dequeId; x<k+1; ++x)
						dequeBufSema[x].pts = CRealTimeSema::PLUS_DROP_POINT; //标记下降点
					dequeId = k+2; //
					i = k+1;
					isContinuousWave = true; //
					hState = STATELESS; //
					break;
				}
			}
			break;

		case MINUS_DOWNGOING_WAVE://负峰 下行波
			lastState = MINUS_DOWNGOING_WAVE;
			for (j=i; j<DataSize-m_Series && hState==MINUS_DOWNGOING_WAVE; ++j,++m_id)
			{
				if (j>=DataSize-m_Series-1 || m_id >= firstOrderSize-m_Series) 
				{
					++m_id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if (m_firstOrder[m_id]>0)
					continue;
				if (m_firstOrder[m_id]<=0)
				{
					for (int s=1; s<=m_Series; ++s)
					{
						if (m_firstOrder[m_id+s]<0)
							break;
						if (m_firstOrder[m_id+s]>0)
						{
							if (s==m_Series)
							{
								dequeBufSema[j+s].pts = CRealTimeSema::MINUS_TROUGH_POINT; //标记波谷
								m_RTPeak.push_back(dequeBufSema[j+s]); //找到波谷

								for (unsigned x=dequeId; x<j+s; ++x)
									dequeBufSema[x].pts = CRealTimeSema::MINUS_DROP_POINT; //标记下降点
								dequeId = j+s+1;
								hState = MINUS_UPCOMING_WAVE; //正峰下行波
								break;
							}
						}
					}
				}
			}
			break;
		case MINUS_UPCOMING_WAVE://负峰 上行波
			lastState = MINUS_UPCOMING_WAVE;
			for (k=j; k<DataSize-m_Series && hState==MINUS_UPCOMING_WAVE; ++k,++m_id)
			{
				if (k>=DataSize-m_Series-1 || m_id >= firstOrderSize-m_Series)
				{
					++m_id;
					hState = HANDLE_POSTPROCESS;
					break;
				}
				if (m_firstOrder[m_id]>=0)
				{
					if (m_firstOrder[m_id]>m_Threshold)  //2010.08.25 添加平头峰处理条件
					{	
						isFlatPeakProcessed = true;
						continue;
					}
					if (isFlatPeakProcessed && m_firstOrder[m_id]<m_Threshold && (m_firstOrder[m_id]!=0.000000)) 
					{
						for (int s=1; s<=m_Series; ++s)
						{
							if (m_firstOrder[m_id+s]>m_Threshold)
								break;
							if (m_firstOrder[m_id+s]<m_Threshold  && (m_firstOrder[m_id+s]!=0.000000))
							{
								if (s==m_Series)
								{
									//getFinishPoint = k+1;
									dequeBufSema[k+1].pts = CRealTimeSema::MINUS_FINISH_POINT; //标记负峰终点
									m_RTPeak.push_back(dequeBufSema[k+1]); //找到负峰终点
									for (unsigned x=dequeId; x<k+1; ++x)
										dequeBufSema[x].pts = CRealTimeSema::MINUS_RAISE_POINT; //标记负峰上升点
									dequeId = k+2;
									hState = STATELESS;
									i = k;
									break;
								}
							}
						}
					}
				}
				if (m_firstOrder[m_id]<0)
				{
					//getFinishPoint = k;
					dequeBufSema[k+1].pts = CRealTimeSema::MINUS_CONTINUOUS_WAVE; //标记（连续）终点
					m_RTPeak.push_back(dequeBufSema[k+1]); //找到终点
					for (unsigned x=dequeId; x<k+1; ++x)
						dequeBufSema[x].pts = CRealTimeSema::MINUS_RAISE_POINT; //标记负峰上升点
					dequeId = k+2; //
					i = k+1;
					isContinuousWave = true; //
					hState = STATELESS; //
					break;
				}
			}
			break;

		case HANDLE_POSTPROCESS://队列后续处理
			isFlatPeakProcessed = false;
			if (dequeId>=dequeBufSema.size())
				::AfxMessageBox(_T("dequeId>=dequeBufSema.size()"));

			m_id -= dequeBufSema.size()-dequeId-m_Series-1; //调整下标起始点

			for (unsigned x=0; x<dequeId; ++x)
				dequeBufSema.pop_front();

			hState = HANDLE_ACCOMPLISH;
			break;

		default:
			break;
		}
	}
}

VOID CRealTime::RT_PeakReturn()  
{// step4: 将求出的点还原至元曲线
	const unsigned currentTotalSemaSize = m_RTTotalSema.size();
	const unsigned peakSemaSize = m_RTPeak.size();
	static unsigned i=0; //控制起点，波峰，波谷3点下标
	static unsigned p=1; //控制即时还原波峰情况下标 d

	// 即时还原波峰情况
	for ( ; p<peakSemaSize-3 && peakSemaSize>2; p+=3)
	{
		double maxValue = -INT_MAX;
		for (unsigned x = m_RTPeak[p-1].idx; x<m_RTPeak[p].idx+m_Avgs && m_RTPeak[p].idx+m_Avgs < currentTotalSemaSize; ++x)
		{
			if (maxValue <= m_RTTotalSema[x].y)
			{
				maxValue = m_RTTotalSema[x].y;
				m_RTPeak[p] = m_RTTotalSema[x];
			}
		}
	}
	//等到起点，波峰，波谷3点有了之后再还原
	for ( ; i<peakSemaSize-3 && peakSemaSize>2 /*&& peakSemaSize%3==0*/; i+=3)  //起点
	{
		for (unsigned j=m_RTPeak[i].idx; j<m_RTPeak[i].idx+m_Avgs && j<currentTotalSemaSize-1; j++) //先还原起点
		{
			double k=(m_RTTotalSema[j+1].y-m_RTTotalSema[j].y)/(m_RTTotalSema[j+1].x-m_RTTotalSema[j].x);
			if (fabs(k)>m_Threshold)
			{
				if (j==m_RTPeak[i].idx+m_Avgs-1)
				{
					m_RTPeak[i] = m_RTTotalSema[j];
					break;
				}
			}
			else
			{
				m_RTPeak[i] = m_RTTotalSema[j];
				break;
			}
		}
		for (unsigned j=m_RTPeak[i+2].idx; j<m_RTPeak[i+2].idx+m_Avgs && j<currentTotalSemaSize-1; j++) //再还原落点
		{
			double k=(m_RTTotalSema[j+1].y-m_RTTotalSema[j].y)/(m_RTTotalSema[j+1].x-m_RTTotalSema[j].x);
			if (fabs(k)>m_Threshold)
			{
				if (j==m_RTPeak[i+2].idx+m_Avgs-1)
				{
					m_RTPeak[i+2] = m_RTTotalSema[j];
					break;
				}
			}
			else
			{
				m_RTPeak[i+2] = m_RTTotalSema[j];
				break;
			}
		}

		//最后还原波峰或者波谷点
		double maxValue = -INT_MAX;
		double minValue = INT_MAX;
		//if (Peak[i].Type>=1) //还原波峰
		//	{
		for (unsigned j=m_RTPeak[i].idx; j<m_RTPeak[i+2].idx; ++j)
		{
			if (maxValue <= m_RTTotalSema[j].y)
			{
				maxValue = m_RTTotalSema[j].y;
				m_RTPeak[i+1] = m_RTTotalSema[j];
			}
		}
	}
}


VOID CRealTime::RT_RealTimeSema2Peak()
{
	const int rt_peak_size = m_RTPeak.size();
	const int peak_size = int(rt_peak_size / 3);

	for (size_t i=m_prevprocess_peak_nums; i<peak_size; ++i)
	{
		CSema start_sema(m_RTPeak[i*3+0].idx, m_RTPeak[i*3+0].x, m_RTPeak[i*3+0].y);
		CSema peak_sema( m_RTPeak[i*3+1].idx, m_RTPeak[i*3+1].x, m_RTPeak[i*3+1].y);
		CSema fin_sema(  m_RTPeak[i*3+2].idx, m_RTPeak[i*3+2].x, m_RTPeak[i*3+2].y);

		CPeak peak(start_sema, peak_sema, fin_sema);
		m_Peak.push_back(peak);
	}

	m_prevprocess_peak_nums = peak_size;
}
VOID CRealTime::RT_GetPeakArea()
{
	bool isSetUnit = false;
	{//测试脚手架程序时注释这块作用域内的代码 
		bool isGC1120  = ((g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC_PX) || (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC1120))  ? true : false;
		bool isGC2400  = (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC2400)  ? true : false;
		bool isGC1100w = (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC1100W) ? true : false;
		bool isGC1100p = (g_pcConfigIni->CurrentSystemName() == PASYSTEM_NAME_GC1100P) ? true : false;
		isSetUnit = (isGC1120 || isGC2400 || isGC1100w || isGC1100p);
	}

	double magnification = 1.0f;
	if (isSetUnit) 
		magnification = 60000.0f;
	else
		magnification = 60.0f;


	const unsigned PeakSize = m_Peak.size();
	const unsigned SemaSize = m_RTTotalSema.size();
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
			if (m_RTTotalSema[j].x - m_Peak[i].StartSema.x >= 0.000000) //找到起点
			{	
				do 
				{
					double x4 = m_RTTotalSema[j+1].x;
					double y4 = m_RTTotalSema[j+1].y;
					double x3 = m_RTTotalSema[j].x;
					double y3 = m_RTTotalSema[j].y;

					double h = fabs(x4-x3);
					double front = fabs(y3-(x3*k+c));
					double behind = fabs(y4-(x4*k+c));
					area += (front+behind)*h/2;
					j++;
				} while (m_RTTotalSema[j].x - x2 <= 0.0000f); //找到终点
				m_Peak[i].PeakArea = area * magnification;
				break;
			}
		}
	}
}
VOID CRealTime::RT_GetPeakHeight()
{
	const int PeakSize = m_Peak.size();

	for (int i=0; i<PeakSize; ++i)
	{
		double x1 = m_Peak[i].StartSema.x;
		double y1 = m_Peak[i].StartSema.y;
		double x2 = m_Peak[i].FinSema.x;
		double y2 = m_Peak[i].FinSema.y;

		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;

		double py = m_Peak[i].PeakSema.y; 
		double px = m_Peak[i].PeakSema.x;
		double ty = px*tana+c;

		double peakHeight = fabs(ty - py); //出现负值情况的考虑
		m_Peak[i].PeakHeight = peakHeight; //峰高
	}
}
VOID CRealTime::RT_Peak2PeakList(CPeakList &cPeakList, CPeakList &cAllPeakList)
{
	

	
	const int peak_size = m_Peak.size();

	for (size_t i=0; i<peak_size; i++)
	{
		CPeakStandard aPeakStard;
		aPeakStard.m_dReserveTime = m_Peak[i].PeakSema.x; //保留时间
		CCurveDataSingle startCurveData(m_Peak[i].StartSema.x, m_Peak[i].StartSema.y);
		CCurveDataSingle topCurveData(m_Peak[i].PeakSema.x, m_Peak[i].PeakSema.y);
		CCurveDataSingle endCurveData(m_Peak[i].FinSema.x, m_Peak[i].FinSema.y);
		aPeakStard.m_dataBegin = startCurveData;
		aPeakStard.m_dataTop = topCurveData;
		aPeakStard.m_dataEnd = endCurveData;
		//aPeakStard.m_dHalfWidth = m_Peak[i].HalfPeakWidth;	//半峰宽
		//aPeakStard.m_dWidth = m_Peak[i].PeakWidth;		//峰宽
		aPeakStard.m_dHeight = m_Peak[i].PeakHeight;		//峰高
		//aPeakStard.m_dHeightPer = m_Peak[i].PeakHighPercent;	//峰高%
		aPeakStard.m_dArea = m_Peak[i].PeakArea;		//峰面积
		//aPeakStard.m_dAreaPer = m_Peak[i].PeakAreaPercent;		//峰面积%
		//aPeakStard.m_dDisfactor = m_Peak[i].Disfactor;   //畸变因子??
		//aPeakStard.m_dSymmetry = m_Peak[i].Tailing;    //拖尾因子
		//aPeakStard.m_dResolution = m_Peak[i].Resolution;  //分离度
		//aPeakStard.m_dColumnPlate = m_Peak[i].ColumnPlate; //理论塔板数
		//aPeakStard.m_dResultConcentration = 0; //浓度
		cPeakList.m_cArrayData.Add(aPeakStard);
		cAllPeakList.m_cArrayData.Add(aPeakStard);
	}

	m_Peak.clear();
}


VOID CRealTime::RT_Save2PeakList(CPeakList &cPeakList)
{
	cPeakList.RemoveAll(); //保证每次输出都是新的实时积分结果数据

	const int rt_peak_size = m_RTPeak.size();
	const int peak_size = int(rt_peak_size / 3);

	//CPeak类型转变成CPeakList 返回cPeakList
	for (size_t i=m_prevprocess_peak_nums; i<peak_size; ++i)
	{
		CPeakStandard aPeakStard;
		//aPeakStard.m_cPeakType.all = m_Peak[i].PeakType.all;//峰类型
		aPeakStard.m_dReserveTime = m_RTPeak[i*3+1].x; //保留时间
		CCurveDataSingle startCurveData(m_RTPeak[i*3+0].x, m_RTPeak[i*3+0].y);
		CCurveDataSingle topCurveData(m_RTPeak[i*3+1].x, m_RTPeak[i*3+1].y);
		CCurveDataSingle endCurveData(m_RTPeak[i*3+2].x, m_RTPeak[i*3+2].y);
		aPeakStard.m_dataBegin = startCurveData;
		aPeakStard.m_dataTop = topCurveData;
		aPeakStard.m_dataEnd = endCurveData;
		//aPeakStard.m_dHalfWidth = m_Peak[i].HalfPeakWidth;	//半峰宽
		//aPeakStard.m_dWidth = m_Peak[i].PeakWidth;		//峰宽
		//aPeakStard.m_dHeight = m_Peak[i].PeakHeight;		//峰高
		//aPeakStard.m_dHeightPer = m_Peak[i].PeakHighPercent;	//峰高%
		//aPeakStard.m_dArea = m_Peak[i].PeakArea;		//峰面积
		//aPeakStard.m_dAreaPer = m_Peak[i].PeakAreaPercent;		//峰面积%
		//aPeakStard.m_dDisfactor = m_Peak[i].Disfactor;   //畸变因子??
		//aPeakStard.m_dSymmetry = m_Peak[i].Tailing;    //拖尾因子
		//aPeakStard.m_dResolution = m_Peak[i].Resolution;  //分离度
		//aPeakStard.m_dColumnPlate = m_Peak[i].ColumnPlate; //理论塔板数
		//aPeakStard.m_dResultConcentration = 0; //浓度
		cPeakList.m_cArrayData.Add(aPeakStard);
	}

	m_prevprocess_peak_nums = peak_size;
}