#include "StdAfx.h"
#include "Peak.h"
#include <cmath>
#include <algorithm>
#include <deque>

 
CSema::CSema():idx(0),x(0.0),y(0.0)
{  
}   
CSema::CSema(int index, double time, double val):idx(index),x(time),y(val)
{
}
bool CSema::operator<(const CSema &s)
{
	return (this->x)<(s.x);
}  
bool CSema::operator==(const CSema &rhs)
{
	return ((this->idx)==rhs.idx);
		//&& ((this->x)-rhs.x==0.00000000) 
		//&& ((this->y)-rhs.y==0.00000000);
}
bool CSema::operator!=(const CSema &rhs)
{
	return !(*this==rhs);
		/*((this->idx)!=rhs.idx)
		|| ((this->x)-rhs.x!=0.00000000) 
		|| ((this->y)-rhs.y!=0.00000000);*/
}
CSema::~CSema()
{
}
CRealTimeSema::CRealTimeSema(int index, double time, double val)
	:CSema(index, time, val)
{ pts = UNKNOWN; }

CRealTimeSema::CRealTimeSema(int index, double time, double val, RealTimeStates rts)
	:CSema(index, time, val)
{ pts = rts; }

CRealTimeSema::CRealTimeSema(const CSema& sema)
	:CSema(sema)
{            }

CPeak::CPeak(void):Type(0),dType(PEAKVALLEY),StartSema(),PeakSema(),FinSema(),HalfPeakWidth(0.0),
	PeakWidth(0.0),PeakArea(0.0),Disfactor(0.0),Tailing(0.0),PeakAreaPercent(0.0),PeakHighPercent(0.0),
	ColumnPlate(0.0),Resolution(0.0)
{
} 
CPeak::CPeak(CSema startSema, CSema peakSema, CSema finSema)
	:StartSema(startSema),PeakSema(peakSema),FinSema(finSema)
{
	Type = 0;
	dType = PEAKVALLEY;
	HalfPeakWidth=0.0;
	PeakWidth=0.0;
	PeakHeight=0.0;
	PeakArea=0.0;
	Disfactor=0.0;
	Tailing=0.0;
	// add
	PeakAreaPercent = 0.0;  //峰面积%
	PeakHighPercent = 0.0;  //峰高%
	ColumnPlate = 0.0;      //理论塔板数
	Resolution = 0.0;       //分离度
}

CPeak::CPeak(int sgn, CSema startSema, CSema peakSema, CSema finSema)
	:Type(sgn),dType(PEAKVALLEY),StartSema(startSema),PeakSema(peakSema),FinSema(finSema)
{
	HalfPeakWidth=0.0;
	PeakWidth=0.0;
	PeakHeight=0.0;
	PeakArea=0.0;
	Disfactor=0.0;
	Tailing=0.0;
	// add
	PeakAreaPercent = 0.0;  //峰面积%
	PeakHighPercent = 0.0;  //峰高%
	ColumnPlate = 0.0;      //理论塔板数
	Resolution = 0.0;       //分离度
}

CPeak::CPeak(int sgn, CSema startSema, CSema peakSema, CSema finSema, double pw, double ph,
			 CSema MstartSema, CSema MpeakSema, CSema MfinSema,  double hpw,  
			 double pa, double df, double tg):
Type(sgn),dType(PEAKVALLEY),StartSema(startSema),PeakSema(peakSema),FinSema(finSema),PeakWidth(pw),PeakHeight(ph),
MidStartSema(MstartSema),MidPeakSema(MpeakSema),MidFinSema(MfinSema),HalfPeakWidth(hpw),
PeakArea(pa),Disfactor(df),Tailing(tg)
{ 
	// add
	PeakAreaPercent = 0.0;  //峰面积%
	PeakHighPercent = 0.0;  //峰高%
	ColumnPlate = 0.0;      //理论塔板数
	Resolution = 0.0;       //分离度
}

bool CPeak::operator==(const CPeak &rhs)
{
	return (this->Type==rhs.Type)  
		&& (this->dType==rhs.dType)  
		&& (this->StartSema==rhs.StartSema)  
		&& (this->PeakSema==rhs. PeakSema)    
		&& (this->FinSema==rhs.FinSema)       
		&& (this->PeakWidth-rhs.PeakWidth==0.00000000)  
		&& (this->PeakHeight-rhs.PeakHeight==0.00000000)      
		&& (this->MidStartSema==rhs.MidStartSema)  
		&& (this->MidPeakSema==rhs.MidPeakSema)  
		&& (this->MidFinSema==rhs.MidFinSema)    
		&& (this->HalfPeakWidth-rhs.HalfPeakWidth==0.00000000)   
		&& (this->PeakArea-rhs.PeakArea==0.00000000)      
		&& (this->Disfactor-rhs.Disfactor==0.00000000)       
		&& (this->Tailing-rhs.Tailing==0.00000000)// add
		&& (this->PeakAreaPercent-rhs.PeakAreaPercent==0.00000000)
		&& (this->PeakHighPercent-rhs.PeakHighPercent==0.00000000)
		&& (this->ColumnPlate-rhs.ColumnPlate==0.00000000)
		&& (this->Resolution-rhs.Resolution==0.00000000);      
}
bool CPeak::operator==(int value)
{
	return (this->Type==value); 
}
bool CPeak::operator!=(const CPeak &rhs)
{
	return !(*this==rhs);
}
bool CPeak::operator< (const CPeak &rhs) const
{
	return this->PeakSema.x < rhs.PeakSema.x;
}
CPeak&  CPeak::operator= (const CPeak& rhs)
{ 
	if (*this!=rhs)
	{
		Type = rhs.Type;          //峰类型,1=波峰; -1=波谷; >1重叠波峰; <-1重叠波谷
		dType = rhs.dType;
		StartSema= rhs.StartSema;     //峰起点
		PeakSema= rhs.PeakSema;      //峰，谷
		FinSema= rhs.FinSema;       //峰落点
		PeakWidth= rhs.PeakWidth;     //峰宽
		PeakHeight= rhs.PeakHeight;    //峰高
		MidStartSema= rhs.MidStartSema;  //半峰起点
		MidPeakSema= rhs.MidPeakSema;   //半峰，谷
		MidFinSema= rhs.MidFinSema;    //半峰落点
		HalfPeakWidth= rhs.HalfPeakWidth; //半峰宽
		PeakArea= rhs.PeakArea;      //峰面积
		Disfactor= rhs.Disfactor;     //畸变因子
		Tailing= rhs.Tailing;       //拖尾因子
		// add
		PeakAreaPercent= rhs.PeakAreaPercent;  //峰面积%
		PeakHighPercent= rhs.PeakHighPercent;  //峰高%
		ColumnPlate= rhs.ColumnPlate;      //理论塔板数
		Resolution= rhs.Resolution;       //分离度
	}
	return *this;
}
CPeak::~CPeak(void)
{
}

//获得峰类型的文字描述
CString CPeak::CPeakType::ConvertToTypeName(void)
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

void CPeak::CPeakType::GetFromTypeName(CString cStrName)
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

//全局函数
//-----------------------------------------------------------------------------
//功能：对输入信号量进行平均化
//传入参数：
//			1.vector<CSema> &handleSema 待处理的信号量
//			2.avg 平均次数
//输出参数：vector<CSema> &avgData 平均化好的信号量
//-----------------------------------------------------------------------------			
void new_Averaging(const std::vector<CSema> &handleSema, //待处理的信号量
				   std::vector<CSema> &avgData,		     //平均化好的信号量
				   unsigned avg )					//平均次数
{//step1: 求以avg为1点的平均曲线，剩余除不尽的为1点
	if (avg==0)
	{
		avgData = handleSema;
		return;
	}
	const unsigned handleSemaSize = handleSema.size();
	for (unsigned i=0; i<handleSemaSize; ++i)
	{
		double avgTime = 0.0;
		double avgSema = 0.0;
		if (i % avg == 0 && i+avg < handleSemaSize) 
		{
			for (unsigned j=i; j< i+avg; j++)
			{
				avgTime += handleSema[j].x;
				avgSema += handleSema[j].y;
			}
			avgTime = avgTime / avg;
			avgSema = avgSema / avg;
			CSema OneSema(i, avgTime, avgSema);
			avgData.push_back(OneSema);
		}
		if ((handleSemaSize-avg*avgData.size())/avg<1) //剩余
		{
			for (unsigned j=avg*avgData.size(); j<handleSemaSize; ++j)
			{ 
				avgTime += handleSema[j].x;    
				avgSema += handleSema[j].y;  
			}
			avgTime = avgTime/(handleSemaSize-avg*avgData.size()+1);
			avgSema = avgSema/(handleSemaSize-avg*avgData.size()+1);
			CSema OneSema(avg*avgData.size(), avgTime, avgSema);
			avgData.push_back(OneSema);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//功能：获得输入信号量的两点之间斜率
//传入参数：
//			1.vector<CSema> &avgData平均化好的信号量
//输出参数：
//			1.vector<double> &firstOrder 斜率存放容器
//			2.maxSlope 平均化好的信号量中最大斜率
//-----------------------------------------------------------------------------
void new_GetAvgDataSlope(const std::vector<CSema> &avgData, //平均化好的信号量
						 std::vector<double> &firstOrder,	//斜率存放容器
						 double &maxSlope)					//最大斜率            
{//step2: 求出平均曲线上每一点的斜率
	//如果输入N个数据，传出的则是N-1个结果
	//最后一个数据没有门限斜率
	const unsigned avgDataSize = avgData.size()-1;
	double *k = new double[avgDataSize];
	for (unsigned i=0; i<avgDataSize; ++i)
	{	
		double ydiff = avgData[i+1].y - avgData[i].y;
		double xdiff = avgData[i+1].x - avgData[i].x;
		k[i] = (ydiff) / (xdiff);
		if (k[i]>=maxSlope)
			maxSlope = k[i];
		firstOrder.push_back(k[i]);
	}
	delete [] k;
	k = NULL;
}
//-----------------------------------------------------------------------------
//功能：获得输入信号量的起点波峰终点
//传入参数：
//			1.vector<CSema> &avgData平均化好的信号量
//			2.vector<double> &firstOrder 平均化好的信号量的斜率
//			3.slopeBased 门限斜率
//			4.hasNagetiveSpike 负峰情况
//			5.isTwiceJudge 二次判断
//输出参数：
//			1.vector<CPeak> &Peak 信号量的起点波峰终点
//-----------------------------------------------------------------------------
void new_GetpeakSearch(const std::vector<CSema> &avgData,   //平均化好的信号量
					   const std::vector<double> &firstOrder,//平均化好的信号量的斜率
					   std::vector<CPeak> &Peak,    //(输出)起点容器
					   double slopeBased, //门限斜率
					   bool hasNagetiveSpike = true,//输入变量，考虑是否有负峰情况
					   bool isTwiceJudge = true)   
					    
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
	hState = STATELESS;
	while (hState != HANDLE_ACCOMPLISH)
	{
		switch (hState)
		{
		case STATELESS:  //无状态
			for ( ; i<avgDataSize-1; ++i)
			{
				if (i==avgDataSize-2)
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}
				 
				if (firstOrder[i]<0) //负峰考虑情况
				{
					if (hasNagetiveSpike)
					{
						if (fabs(firstOrder[i])<slopeBased)
							continue;
						if (fabs(firstOrder[i])>=slopeBased)
						{
							if (!isTwiceJudge) //不进行连续2点判断
							{
								isNagetiveSpike = true;
								getStartPoint = i;
								hState = DOWNGOING_WAVE;
								break;
							}
							if (fabs(firstOrder[i+1])<slopeBased)
								continue;
							if (fabs(firstOrder[i+1])>=slopeBased)
							{
								isNagetiveSpike = true;
								getStartPoint = i;
								hState = DOWNGOING_WAVE;
								break;
							}
						}
					}
					else
						continue;
				}	

				if (firstOrder[i]>=0) //非负峰考虑情况
				{
					if (firstOrder[i]<slopeBased)
						continue;
					if (firstOrder[i]>slopeBased)
					{
						if (!isTwiceJudge) //不进行连续2点判断
						{
							getStartPoint = i;
							hState = UPCOMING_WAVE;
							break;
						}
						if (firstOrder[i+1]<slopeBased)
							continue;
						if (firstOrder[i+1]>slopeBased)
						{
							getStartPoint = i;
							hState = UPCOMING_WAVE;
							break;
						}
					}
				}
			}
			break;

		case UPCOMING_WAVE: //上行波
			if (!isNagetiveSpike)
			{
				for (j=i+1; j<avgDataSize-1; ++j)
				{
					if (j==avgDataSize-2)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]<0)
						continue;
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j+1]>0)
							continue;
						if (firstOrder[j+1]<0)
						{
							getpeakPoint = j+1;
							hState = DOWNGOING_WAVE;
							break;
						}
					}
				}
			}
			else
			{
				for (j=k+1; j<avgDataSize-1; ++j)
				{
					if (j==avgDataSize-2)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j]>slopeBased)
							continue;
						if (firstOrder[j]<slopeBased ) 
						{
							if (!isTwiceJudge) //不进行连续2点判断
							{
								getFinishPoint = j+1;
								hState = STATELESS;
								i = j;
								break;
							}
							if (firstOrder[j+1]<slopeBased)
							{
								getFinishPoint = j+1;
								hState = STATELESS;
								i = j;
								break;
							}
						}
					}
					if (firstOrder[j]<0)
					{
						getFinishPoint = j;
						hState = STATELESS;
						i = j-1;
						break;
					}

				}
				if (hState==STATELESS)
				{
					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					Peak.push_back(OnePeak);

					unsigned t = Peak.size()-1;
					if (t>1 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
					{
						Peak[t-1].Type = -mark;// 之前的标志改为连续波谷
						Peak[t].Type = -mark;  //连续波谷
					}
					else
					{
						Peak[t].Type = -1; //单个波谷
						mark++;
					}


					isNagetiveSpike = false;

				}
			}
			break;

		case DOWNGOING_WAVE: //下行波
			if (!isNagetiveSpike)
			{
				for (k = j+1; k<avgDataSize-1; ++k)
				{
					if (k==avgDataSize-2)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if ( firstOrder[k]<0 ) 
					{
						if (fabs(firstOrder[k])>slopeBased)
							continue;

						if ( fabs(firstOrder[k])<slopeBased )
						{
							if (!isTwiceJudge) //不进行连续2点判断
							{
								getFinishPoint = k+1;
								hState = STATELESS;
								i = k;
								break;
							}
							if ( fabs(firstOrder[k+1])<slopeBased/* && firstOrder[k]<0*/)
							{
								getFinishPoint = k+1;
								hState = STATELESS;
								i = k;
								break;
							}
							if (fabs(firstOrder[k+1])>slopeBased)
								continue;

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
					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					Peak.push_back(OnePeak);

					unsigned t = Peak.size()-1;

					if (t>1 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
					{
						Peak[t-1].Type = mark;	// 之前的标志改为连续波峰
						Peak[t].Type = mark;	//连续波峰
					}
					else
					{
						Peak[t].Type = 1; //单个波峰
						mark++;
					}


				}
			}
			else
			{
				for (k = i+1; k<avgDataSize-1; ++k)
				{
					if (k==avgDataSize-2)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[k]>0)
						continue;
					if (firstOrder[k]<=0)
					{
						if (firstOrder[k+1]<0)
							continue;
						if (firstOrder[k+1]>0)
						{
							getpeakPoint = k+1;
							hState = UPCOMING_WAVE;
							break;
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


//-----------------------------------------------------------------------------
//功能：将获得输入信号量的起点波峰终点还原至元曲线上
//传入参数：
//			1.vector<CSema> &handleSema 元曲线信号量
//			2.avg 平均次数
//			3.slopeBased 门限斜率
//输出参数：
//			1.vector<CPeak> &Peak 还原后信号量的起点波峰终点
//-----------------------------------------------------------------------------
void new_RestorePeakData(const std::vector<CSema> &handleSema, //待还原的信号量
						 std::vector<CPeak> &Peak,
						 const int avg,
						 const double slopeBased)  

{// step4: 将求出的点还原至元曲线
	const unsigned handleSemaSize = handleSema.size();
	const unsigned peakSemaSize = Peak.size();

	for (unsigned i=0; i<peakSemaSize; ++i)  //对于每一个峰
	{
		for (unsigned j=Peak[i].StartSema.idx; j<Peak[i].StartSema.idx+avg && j<handleSemaSize-1; j++) //先还原起点
		{
			double k=(handleSema[j+1].y-handleSema[j].y)/(handleSema[j+1].x-handleSema[j].x);
			if (fabs(k)>slopeBased)
			{
				if (j==Peak[i].StartSema.idx+avg-1)
				{
					Peak[i].StartSema = handleSema[j];
					break;
				}
			}
			else
			{
				Peak[i].StartSema = handleSema[j];
				break;
			}
		}

		for (unsigned j=Peak[i].FinSema.idx; j<Peak[i].FinSema.idx+avg && j<handleSemaSize-1; j++) //再还原落点
		{
			double k=(handleSema[j+1].y-handleSema[j].y)/(handleSema[j+1].x-handleSema[j].x);
			if (fabs(k)>slopeBased)
			{
				if (j==Peak[i].FinSema.idx+avg-1)
				{
					Peak[i].FinSema = handleSema[j];
					break;
				}
			}
			else
			{
				Peak[i].FinSema = handleSema[j];
				break;
			}
		}

		//最后还原波峰或者波谷点
		double maxValue = -INT_MAX;
		double minValue = INT_MAX;
		if (Peak[i].Type>=1) //还原波峰
		{
			for (unsigned j=Peak[i].StartSema.idx; j<Peak[i].FinSema.idx; ++j)
			{
				if (maxValue <= handleSema[j].y)
				{
					maxValue = handleSema[j].y;
					Peak[i].PeakSema = handleSema[j];
				}
			}
		}
		else //还原波谷
		{
			for (unsigned j=Peak[i].StartSema.idx; j<Peak[i].FinSema.idx; ++j)
			{
				if (minValue >= handleSema[j].y)
				{
					minValue = handleSema[j].y;
					Peak[i].PeakSema = handleSema[j];	
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
//功能：垂直分峰算法
//传入参数：
//			1.vector<CPeak> &Peak 自定义的Peak类型变量
//输出参数：
//			1.vector<CPeak> &Peak 垂直分峰后的波
//-----------------------------------------------------------------------------
void new_DividePeak(std::vector<CPeak> &Peak)          
{// 垂直分峰
	//std::vector<std::pair<double, double> > TempStart;    //起点容器
	//std::vector<std::pair<double, double> > TempFinish;   //落点容器     

	const unsigned PeakSize = Peak.size();
	unsigned bg=0;
	unsigned ed=bg;
	bool getIndex = false; //获得计算下标
	for (; bg<PeakSize; ++bg)
	{
		if (Peak[bg].Type==1 || Peak[bg].Type==-1) //单峰情况, 不做出任何改变
		{
			continue;
		}
		else //获得连续峰下标
		{
			ed = bg+1;
			int mark = Peak[bg].Type;
			for (; ed<PeakSize; ++ed)
			{
				if (ed+1==PeakSize && Peak[PeakSize-1].Type==mark)
				{
					getIndex = true;
					break;
				}
				if (Peak[ed].Type!=mark)
				{
					ed--;
					getIndex = true;
					break;
				}
			}
		}

		if (getIndex) //连续峰情况
		{
			double k = (Peak[ed].FinSema.y-Peak[bg].StartSema.y)/(Peak[ed].FinSema.x-Peak[bg].StartSema.x);
			double c = Peak[bg].StartSema.y-k*Peak[bg].StartSema.x;
			double tmp=Peak[bg].StartSema.y;
			for (;bg!=ed; ++bg)
			{
				double _y = Peak[bg].FinSema.x*k+c;
				if ((Peak[bg].Type>1 && _y > Peak[bg].FinSema.y) || (Peak[bg].Type<-1 && _y < Peak[bg].FinSema.y))
				{
					//Peak[bg].StartSema.y = tmp;//
					//Peak[bg+1].Type = Peak[bg+1].Type/abs(Peak[bg+1].Type);

					if (bg+1!=ed)
						++bg;
				}
				else
				{
					Peak[bg].FinSema.y = _y;
					tmp = Peak[bg+1].StartSema.y;
					Peak[bg+1].StartSema.y = _y;
				}
			}
			getIndex = false;
		}	
	}
}
//-----------------------------------------------------------------------------
//功能：垂直分峰后基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.hasNagetiveSpike 负峰情况
//输出参数：
//			1.vector<CPeak> &Peak 基线校正后的波
//-----------------------------------------------------------------------------
void new_BaselineAdjust(const std::vector<CSema> &handleSema,
					   std::vector<CPeak> &Peak,
					   bool hasNagetiveSpike = true) //输入变量，考虑是否有负峰情况
{
	//垂直分峰基线校正
	const unsigned PeakSize = Peak.size();
	unsigned bg=0;
	unsigned ed=bg;
	for (; bg<PeakSize; ++bg)
	{
		if (abs(Peak[bg].Type)==1) //单峰情况
		{
			ed = bg;
		}
		else				  //重峰情况
		{
			ed = bg+1;
			int mark = Peak[bg].Type;
			for (; ed<PeakSize; ++ed)
			{
				if (ed+1==PeakSize)
				{
					break;
				}
				if (Peak[ed].Type!=mark)
				{
					ed--;
					break;
				}
			}
		}
		unsigned s = Peak[bg].StartSema.idx;
		unsigned f = Peak[ed].FinSema.idx;
		//起点校正
		bool isfail = true;
		for (; s<=Peak[bg].PeakSema.idx && isfail; ++s)
		{
			double k = (handleSema[f].y-handleSema[s].y)/(handleSema[f].x-handleSema[s].x);
			double c = handleSema[s].y-k*handleSema[s].x;
			for (unsigned m=s; m<=Peak[bg].PeakSema.idx; ++m) 
			{
				double _y = k * handleSema[m].x+c;
				if (hasNagetiveSpike) //考虑负峰情况
				{
					if (fabs(_y) > fabs(handleSema[m].y))
					{
						isfail = true;
						break;
					}
				}
				else
				{
					if (fabs(_y) > fabs(handleSema[m].y) && _y>0)
					{
						isfail = true;
						break;
					}
					else if(fabs(_y) < fabs(handleSema[m].y) && _y<=0)
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
				Peak[bg].StartSema = handleSema[s];
				isfail = false;
			}
		}isfail = true;	//落点校正
		for (; f>=Peak[ed].PeakSema.idx && isfail; --f)
		{
			double k = (handleSema[f].y-handleSema[s].y)/(handleSema[f].x-handleSema[s].x);
			double c = handleSema[f].y-k*handleSema[f].x;
			for (unsigned n=f; n>=Peak[ed].PeakSema.idx; --n)
			{
				double _y = k * handleSema[n].x + c;
				if (hasNagetiveSpike) //考虑负峰情况
				{
					if (fabs(_y) > fabs(handleSema[n].y))
					{
						isfail = true;
						break;
					}
				}
				else
				{
					if (fabs(_y) > fabs(handleSema[n].y) && _y>0)
					{
						isfail = true;
						break;
					}
					else if(fabs(_y) < fabs(handleSema[n].y) && _y<=0)
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
				Peak[ed].FinSema = handleSema[f];
				isfail = false;
			}
		}
	bg = ed;
	}
}
//-----------------------------------------------------------------------------
//功能：非垂直分峰后基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.hasNagetiveSpike 负峰情况
//输出参数：
//			1.vector<CPeak> &Peak 基线校正后的波
//-----------------------------------------------------------------------------
void new_BaselineCheak(const std::vector<CSema> &handleSema,
					   std::vector<CPeak> &Peak,
					   bool hasNagetiveSpike = true) //输入变量，考虑是否有负峰情况
{
	// 非垂直分峰
	const unsigned PeakSize = Peak.size();
	unsigned bg=0;
	unsigned ed=bg;
	bool getIndex = false; //获得计算下标

	for (; bg<PeakSize; ++bg)
	{
		if (Peak[bg].Type==1 || Peak[bg].Type==-1) //单峰情况
		{
			ed = bg;
			getIndex = true;
		}

		else //重叠波峰
		{
			ed = bg+1;
			int mark = Peak[bg].Type;
			for (; ed<PeakSize; ++ed)
			{
				if (ed+1==PeakSize)
				{
					getIndex = true;
					break;
				}
				if (Peak[ed].Type!=mark)
				{
					ed--;
					getIndex = true;
					break;
				}
			}
		}
		if (getIndex) 
		{
			unsigned i = bg;
			unsigned o = ed;
			unsigned j = Peak[i].StartSema.idx;
			do 
			{
				bool isfail = true;
				for (j=Peak[i].StartSema.idx; j<=Peak[i].PeakSema.idx && isfail; ++j)	
				{
					//---------------------------------------------------------
					//if (j==Peak[i].PeakSema.idx)
					//{
					//	//startSema[i] = (handleSema[sIndex[i]]); //相邻一点
					//	Peak[i].StartSema = handleSema[Peak[i].StartSema.idx];
					//	isfail = false;
					//	break;
					//}
					//---------------------------------------------------------
					double k = (handleSema[Peak[i].FinSema.idx].y-handleSema[j].y)/(handleSema[Peak[i].FinSema.idx].x-handleSema[j].x);
					//double k = (handleSema[fIndex[i]].second - handleSema[j].second)/(handleSema[fIndex[i]].first - handleSema[j].first);
					double c = handleSema[j].y-k*handleSema[j].x;
					//double c = handleSema[j].second - k * handleSema[j].first;
					for (unsigned m=j; m<=Peak[i].PeakSema.idx; ++m) 
					{
						double _y = k*handleSema[m].x+c;
						if (hasNagetiveSpike) //考虑负峰情况
						{
							if (getIndex && fabs(_y) > fabs(handleSema[m].y))
							{
								isfail = true;
								break;
							}
						}
						else
						{
							if (getIndex && fabs(_y) > fabs(handleSema[m].y) && _y>0)
							{
								isfail = true;
								break;
							}
							else if(getIndex && fabs(_y) < fabs(handleSema[m].y) && _y<=0)
							{
								isfail = true;
								break;
							}
						}
						isfail = false;
					}
					if (!isfail)
					{
						//startSema[i] = handleSema[j]; //放入起点
						//sIndex[i] = j;
						Peak[i].StartSema = handleSema[j];
						isfail = false;
					}
				} isfail = true;
				for (unsigned l=Peak[i].FinSema.idx; l>=Peak[i].PeakSema.idx && isfail; --l)
				{
					//---------------------------------------------------------
					//if (l==Peak[i].PeakSema.idx)
					//{
					//	//finishSema[i]=handleSema[fIndex[i]]; //相邻一点
					//	Peak[i].FinSema = handleSema[Peak[i].FinSema.idx];
					//	break;
					//}
					//---------------------------------------------------------
					double k = (handleSema[l].y-handleSema[j].y)/(handleSema[l].x-handleSema[j].x);
					//double k = (handleSema[l].second-handleSema[j].second)/(handleSema[l].first-handleSema[j].first);
					double c = handleSema[l].y-k*handleSema[l].x;
					//double b = handleSema[l].second - k * handleSema[l].first;
					for (unsigned n=l; n>=Peak[i].PeakSema.idx; --n)
					{
						double _y = k * handleSema[n].x + c;
						if (hasNagetiveSpike) //考虑负峰情况
						{
							if (getIndex && fabs(_y) > fabs(handleSema[n].y))//
							{
								isfail = true;
								break;
							}
						}
						else
						{
							if (getIndex && fabs(_y) > fabs(handleSema[n].y) && _y>0)//
							{
								isfail = true;
								break;
							}
							else if(getIndex && fabs(_y) < fabs(handleSema[n].y) && _y<=0)//
							{
								isfail = true;
								break;
							}
						}
						isfail = false;				
					}
					if (!isfail)
					{
						//finishSema[i]=handleSema[l]; //放入落点
						//fIndex[i]=l;
						Peak[i].FinSema = handleSema[l];
						isfail = false;
					}
				}
				i++;	
			} while (i!=o+1);
			getIndex = false;
		}
		bg = ed;
	}
	//if (startSema.size()!=peakSema.size()||finishSema.size()!=peakSema.size())
	//{	
	//	::AfxMessageBox(_T("计算错误，存在负峰情况。请勾选考虑负峰情况"));
	//	ASSERT(0);
	//}
}
//-----------------------------------------------------------------------------
//功能：基线校正后对波类型的检查
//传入参数：
//			1.vector<CPeak> &Peak 被检查波类型的波
//输出参数：
//			1.vector<CPeak> &Peak 波类型的检查后的波
//-----------------------------------------------------------------------------
void new_PeakTypeChange(std::vector<CPeak> &Peak)
{
	const unsigned PeakSize = Peak.size();
	if (PeakSize==0) return;
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (  (Peak[i].dType == CPeak::PLUSTANGET)
			||(Peak[i].dType == CPeak::MINUSTANGET))
		{
			continue;
		}

		if (abs(Peak[i].Type)!=1) 
		{
			if (Peak[i].FinSema==Peak[i+1].StartSema)
			{
				continue;
			}
			else /*if (Peak[i].FinSema!=Peak[i+1].StartSema)*/
			{
				if (i!=0)
				{
					if (Peak[i].StartSema==Peak[i-1].FinSema)
					{
						continue;
					}
					if (Peak[i].Type==0)
						continue;

					Peak[i].Type = Peak[i].Type/abs(Peak[i].Type);
				}
			}
		}
	}
}
void new_GetPeakWidth(const std::vector<CSema> &handleSema,
					   std::vector<CPeak> &Peak,
					   bool isDividePeak)
{
	const unsigned PeakSize = Peak.size();
	const int factor = 1; //Y轴统一因子
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].StartSema.x;
		double y1 = Peak[i].StartSema.y;
		double x2 = Peak[i].FinSema.x;
		double y2 = Peak[i].FinSema.y;

		double peakWidth = sqrt((y2/factor-y1/factor)*(y2/factor-y1/factor)+(x2-x1)*(x2-x1));
		Peak[i].PeakWidth = peakWidth;  //峰宽

		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;

		double py = Peak[i].PeakSema.y; 
		double px = Peak[i].PeakSema.x;
		double ty = px*tana+c;

		double peakHeight = fabs(fabs(ty)-fabs(py));
		Peak[i].PeakHeight = peakHeight; //峰高

		double PeakWidthLeft =  sqrt((ty/factor-y1/factor)*(ty/factor-y1/factor)+(px-x1)*(px-x1));  
		double PeakWidthRight = sqrt((y2/factor-ty/factor)*(y2/factor-ty/factor)+(x2-px)*(x2-px));
		double lr = PeakWidthLeft/PeakWidthRight;
		Peak[i].Tailing = lr; //拖尾因子

		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;


		bool getHalfStart = false;
		bool getHalfFin = false;
		if (isDividePeak)
		{
			unsigned StartIdx = Peak[i].StartSema.idx;
			unsigned StartIdxNext = Peak[i].StartSema.idx+1;
			double gapPre_start = fabs(handleSema[StartIdx].y);
			double gapNext_start = fabs(handleSema[StartIdxNext].y);
			double gap_start = fabs(handleSema[StartIdx].x * tana + _c);
			if (gap_start>=gapPre_start && gap_start<=gapNext_start) //半峰宽起点在分峰线上
			{
				CSema temp(StartIdx,Peak[i].StartSema.x, gap_start); //半峰起点	
				Peak[i].MidStartSema = temp;
				getHalfStart = true;
			}

			unsigned FinIdx = Peak[i].FinSema.idx-1;
			unsigned FinIdxNext = Peak[i].StartSema.idx;
			double gapPre_fin = fabs(handleSema[FinIdx].y);
			double gapNext_fin = fabs(handleSema[FinIdxNext].y);
			double gap_fin = fabs(handleSema[FinIdx].x * tana + _c);
			if (gap_fin>=gapPre_fin && gap_fin<=gapNext_fin) //半峰宽落点在分峰线上
			{
				CSema temp(FinIdxNext, Peak[i].FinSema.x, gap_fin); //半峰落点	
				Peak[i].MidFinSema = temp;
				getHalfStart = true;
			}
		}

		double diff_y = INT_MAX;
		unsigned idxl, idxr;
		if (!getHalfStart)
		{
			for (unsigned l=Peak[i].StartSema.idx; l<=Peak[i].PeakSema.idx; ++l) //上行，逼近切点
			{
				double ly = handleSema[l].x * tana + _c;
				if (diff_y >= fabs(handleSema[l].y - ly))
				{
					diff_y = fabs(handleSema[l].y - ly);
					idxl = l;
				}
			}
			CSema tmp(handleSema[idxl]); //半峰起点
			Peak[i].MidStartSema = tmp;
		}
		diff_y = INT_MAX;
		if (!getHalfFin)
		{
			for (unsigned r=Peak[i].PeakSema.idx; r<=Peak[i].FinSema.idx; ++r) //下行，逼近切点
			{
				double ry = handleSema[r].x * tana + _c;
			
				if (diff_y >= fabs(handleSema[r].y - ry))
				{
					diff_y = fabs(handleSema[r].y - ry);
					idxr = r;
				}
			}
			CSema tmp(handleSema[idxr]);   //半峰落点
			Peak[i].MidFinSema = tmp;
		}
		double hk = (Peak[i].MidFinSema.y-Peak[i].MidStartSema.y)/(Peak[i].MidFinSema.x-Peak[i].MidStartSema.x);
		double hc = Peak[i].MidStartSema.y - Peak[i].MidStartSema.x * hk;
		double rey = hx*hk+hc;
		CSema HalfPeak(Peak[i].PeakSema.idx, hx, rey); // 半峰中点
		Peak[i].MidPeakSema = HalfPeak;
		//CPeak OneHalf(HalfStart, HalfPeak, HalfFin);
		//Half.push_back(OneHalf);  //半峰容器
	}

	//连接半峰宽直线，与波峰的垂线相交于一点。并求出半峰宽前后长度
	//const unsigned HalfSize = Half.size();
	//ASSERT(HalfSize==PeakSize);
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].MidStartSema.x; 
		double y1 = Peak[i].MidStartSema.y;
		double x2 = Peak[i].MidFinSema.x; 
		double y2 = Peak[i].MidFinSema.y;
		double halfPeakWidth = sqrt((y2/factor-y1/factor)*(y2/factor-y1/factor)+(x2-x1)*(x2-x1));
		Peak[i].HalfPeakWidth = halfPeakWidth;  //求半峰宽

		double x0 = Peak[i].MidPeakSema.x;      //求半峰宽前后长度
		double y0 = Peak[i].MidPeakSema.y;
		double halfPeakWidthLeft =  sqrt((y0/factor-y1/factor)*(y0/factor-y1/factor)+(x0-x1)*(x0-x1));
		double halfPeakWidthRight = sqrt((y2/factor-y0/factor)*(y2/factor-y0/factor)+(x2-x0)*(x2-x0));
		double disfactor = halfPeakWidthLeft/halfPeakWidthRight;
		Peak[i].Disfactor = disfactor;  //求畸变因子
	}
}

//-----------------------------------------------------------------------------
//功能：获得波的峰宽。半峰宽。峰高。畸变因子。拖尾因子等各个参数
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 要获得各个参数的波
//			3.isDividePeak 是否垂直分峰
//输出参数：
//			1.vector<CPeak> &Peak 获得各个参数后的波
//-----------------------------------------------------------------------------
void new_GetPeakWidth_v2(const std::vector<CSema> &handleSema,
						 std::vector<CPeak> &Peak,
						 bool isDividePeak)
{
	const unsigned PeakSize = Peak.size();
	//const int factor = 1; //Y轴统一因子
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].StartSema.x;
		double y1 = Peak[i].StartSema.y;
		double x2 = Peak[i].FinSema.x;
		double y2 = Peak[i].FinSema.y;

		//double peakWidth = sqrt((y2/factor-y1/factor)*(y2/factor-y1/factor)+(x2-x1)*(x2-x1));
		double peakWidth = fabs(x2-x1);
		Peak[i].PeakWidth = peakWidth;  //峰宽

		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;

		double py = Peak[i].PeakSema.y; 
		double px = Peak[i].PeakSema.x;
		double ty = px*tana+c;

		double peakHeight = fabs(fabs(ty)-fabs(py));
		Peak[i].PeakHeight = peakHeight; //峰高

		//double PeakWidthLeft =  sqrt((ty/factor-y1/factor)*(ty/factor-y1/factor)+(px-x1)*(px-x1));  
		//double PeakWidthRight = sqrt((y2/factor-ty/factor)*(y2/factor-ty/factor)+(x2-px)*(x2-px));
		double PeakWidthLeft  = fabs(px-x1);
		double PeakWidthRight = fabs(x2-px);
		double lr = PeakWidthLeft/PeakWidthRight;
		Peak[i].Tailing = lr; //拖尾因子

		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;


		bool getHalfStart = false;
		bool getHalfFin = false;
		if (isDividePeak)
		{
			unsigned StartIdx = Peak[i].StartSema.idx;
			unsigned StartIdxNext = Peak[i].StartSema.idx+1;
			double gapPre_start = fabs(handleSema[StartIdx].y);
			double gapNext_start = fabs(handleSema[StartIdxNext].y);
			double gap_start = fabs(handleSema[StartIdx].x * tana + _c);
			if (gap_start>=gapPre_start && gap_start<=gapNext_start) //半峰宽起点在分峰线上
			{
				CSema temp(StartIdx,Peak[i].StartSema.x, gap_start); //半峰起点	
				Peak[i].MidStartSema = temp;
				getHalfStart = true;
			}

			unsigned FinIdx = Peak[i].FinSema.idx-1;
			unsigned FinIdxNext = Peak[i].StartSema.idx;
			double gapPre_fin = fabs(handleSema[FinIdx].y);
			double gapNext_fin = fabs(handleSema[FinIdxNext].y);
			double gap_fin = fabs(handleSema[FinIdx].x * tana + _c);
			if (gap_fin>=gapPre_fin && gap_fin<=gapNext_fin) //半峰宽落点在分峰线上
			{
				CSema temp(FinIdxNext, Peak[i].FinSema.x, gap_fin); //半峰落点	
				Peak[i].MidFinSema = temp;
				getHalfStart = true;
			}
		}

		double diff_y = INT_MAX;
		unsigned idxl, idxr;
		if (!getHalfStart)
		{
			for (unsigned l=Peak[i].StartSema.idx; l<=Peak[i].PeakSema.idx; ++l) //上行，逼近切点
			{
				double ly = handleSema[l].x * tana + _c;
				if (diff_y >= fabs(handleSema[l].y - ly))
				{
					diff_y = fabs(handleSema[l].y - ly);
					idxl = l;
				}
			}
			CSema tmp(handleSema[idxl]); //半峰起点
			Peak[i].MidStartSema = tmp;
		}
		diff_y = INT_MAX;
		if (!getHalfFin)
		{
			for (unsigned r=Peak[i].PeakSema.idx; r<=Peak[i].FinSema.idx; ++r) //下行，逼近切点
			{
				double ry = handleSema[r].x * tana + _c;

				if (diff_y >= fabs(handleSema[r].y - ry))
				{
					diff_y = fabs(handleSema[r].y - ry);
					idxr = r;
				}
			}
			CSema tmp(handleSema[idxr]);   //半峰落点
			Peak[i].MidFinSema = tmp;
		}
		double hk = (Peak[i].MidFinSema.y-Peak[i].MidStartSema.y)/(Peak[i].MidFinSema.x-Peak[i].MidStartSema.x);
		double hc = Peak[i].MidStartSema.y - Peak[i].MidStartSema.x * hk;
		double rey = hx*hk+hc;
		CSema HalfPeak(Peak[i].PeakSema.idx, hx, rey); // 半峰中点
		Peak[i].MidPeakSema = HalfPeak;
		//CPeak OneHalf(HalfStart, HalfPeak, HalfFin);
		//Half.push_back(OneHalf);  //半峰容器
	}

	//连接半峰宽直线，与波峰的垂线相交于一点。并求出半峰宽前后长度
	//const unsigned HalfSize = Half.size();
	//ASSERT(HalfSize==PeakSize);
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].MidStartSema.x; 
		double y1 = Peak[i].MidStartSema.y;
		double x2 = Peak[i].MidFinSema.x; 
		double y2 = Peak[i].MidFinSema.y;
		//double halfPeakWidth = sqrt((y2/factor-y1/factor)*(y2/factor-y1/factor)+(x2-x1)*(x2-x1));
		double halfPeakWidth = fabs(x2-x1);
		Peak[i].HalfPeakWidth = halfPeakWidth;  //求半峰宽

		double x0 = Peak[i].MidPeakSema.x;      //求半峰宽前后长度
		double y0 = Peak[i].MidPeakSema.y;
		//double halfPeakWidthLeft =  sqrt((y0/factor-y1/factor)*(y0/factor-y1/factor)+(x0-x1)*(x0-x1));
		//double halfPeakWidthRight = sqrt((y2/factor-y0/factor)*(y2/factor-y0/factor)+(x2-x0)*(x2-x0));
		double halfPeakWidthLeft =  fabs(x0-x1);
		double halfPeakWidthRight = fabs(x2-x0);
		double disfactor = halfPeakWidthLeft/halfPeakWidthRight;
		Peak[i].Disfactor = disfactor;  //求畸变因子
	}
}
//-----------------------------------------------------------------------------
//功能：获得波的峰面积
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 要获得峰面积的波		
//输出参数：
//			1.vector<CPeak> &Peak 获得峰面积后的波
//-----------------------------------------------------------------------------
void new_GetPeakArea(const std::vector<CSema> &handleSema,
					 std::vector<CPeak> &Peak)
{
	const unsigned PeakSize = Peak.size();
	const unsigned handleSemaSize = handleSema.size();
	unsigned j = 0;
	for (unsigned i=0; i<PeakSize; ++i)  
	{
		double x2 = Peak[i].FinSema.x;           
		double y2 = Peak[i].FinSema.y;
		double x1 = Peak[i].StartSema.x;
		double y1 = Peak[i].StartSema.y;
		double k = (y2-y1)/(x2-x1);
		double c = y1-k*x1;

		double area = 0.0;
		for (; j<handleSemaSize; ++j)
		{
			if (handleSema[j].x-Peak[i].StartSema.x >= 0.000000) //找到起点
			{	
				do 
				{
					double x4 = handleSema[j+1].x;
					double y4 = handleSema[j+1].y;
					double x3 = handleSema[j].x;
					double y3 = handleSema[j].y;

					double h = x4 - x3;
					double up = fabs(fabs(y3)-fabs(x3*k+c));
					double down = fabs(fabs(y4)-fabs(x4*k+c));
					area += (up+down)*h/2;
					j++;
				} while (handleSema[j].x-Peak[i].FinSema.x <= 0.000000); //找到终点
				Peak[i].PeakArea = area;
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
//功能：删除比给定半峰宽参数小的波峰
//传入参数：		
//			1.vector<CPeak> &Peak 待处理的波
//			2.halfPeakWidth 给定半峰宽参数
//输出参数：
//			1.vector<CPeak> &Peak 删除比给定半峰宽参数小的波峰后的波
//-----------------------------------------------------------------------------
void new_ErasePeakByHalfWidth(std::vector<CPeak> &Peak,
							  const double halfPeakWidth)
{
	const unsigned PeakSize = Peak.size();
	std::vector<CPeak> Temp; //临时容器
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (Peak[i].HalfPeakWidth - halfPeakWidth >= 0.000000)
		{
			Temp.push_back(Peak[i]);
		}
	}
	Peak.swap(Temp);
	//删峰后改变峰类型
	const unsigned SwapPeakSize = Peak.size();
	typedef std::vector<CPeak>::iterator CI;
	for (CI it=Peak.begin(); it!=Peak.end(); ++it)
	{
		if (abs(it->Type)!=1)
		{
			int times = count(Peak.begin(), Peak.end(), *it);
			if (times==1)
			{
				it->Type = (it->Type)/abs(it->Type); //波类型变为单峰
			}
		}
	}
	
}
//-----------------------------------------------------------------------------
//功能：删除比给定峰面积参数小的波峰
//传入参数：		
//			1.vector<CPeak> &Peak 待处理的波
//			2.halfPeakWidth 给定峰面积参数
//输出参数：
//			1.vector<CPeak> &Peak 删除比给定峰面积参数小的波峰后的波
//-----------------------------------------------------------------------------
void new_ErasePeakByPeakArea(std::vector<CPeak> &Peak,
							 const double peakArea)
{
	const unsigned PeakSize = Peak.size();
	std::vector<CPeak> Temp; //临时容器
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (Peak[i].PeakArea >= peakArea)
		{
			Temp.push_back(Peak[i]);
		}
	}
	Peak.swap(Temp);
	//删峰后改变峰类型
	const unsigned SwapPeakSize = Peak.size();
	typedef std::vector<CPeak>::iterator CI;
	for (CI it=Peak.begin(); it!=Peak.end(); ++it)
	{
		if (abs(it->Type)!=1)
		{
			//int PeakType = it->Type;
			//int times = count(Peak.begin(), Peak.end(), it->Type);
			int times=0;
			for (unsigned i=0; i<SwapPeakSize; ++i)
			{
				if (Peak[i].Type==(it->Type))
					times++;
			}
			if (times==1)
			{
				it->Type = (it->Type)/abs(it->Type); //波类型变为单峰
			}
		}
	}
}
//-----------------------------------------------------------------------------
//功能：获得基线的标记点
//传入参数：		
//			1.vector<CPeak> &Peak 待画基线的波
//输出参数：
//			1.vector<CSema> &TagPoint 获得基线的标记点的容器
//-----------------------------------------------------------------------------
void new_GetTagPoint(const std::vector<CPeak> &Peak,
					 std::vector<CSema> &TagPoint)
{
	if (Peak.empty())
	{
		return;
	}
	const unsigned PeakSize = Peak.size();
	double diff_h=fabs(Peak[0].PeakHeight); //标记点高
	for (unsigned i=0; i<PeakSize; ++i) //求最小峰高
	{
		if (diff_h>=fabs(Peak[i].PeakHeight))
		{
			diff_h=fabs(Peak[i].PeakHeight);
		}
	}
	diff_h = diff_h*0.15; //标记线为峰高30%
	for (unsigned i=0; i<PeakSize; ++i)
	{
		//diff_h = Peak[i].PeakHeight*0.05;  //标记线为峰高10%
		CSema maeue(Peak[i].StartSema.idx, Peak[i].StartSema.x, Peak[i].StartSema.y+diff_h);
		TagPoint.push_back(maeue);
		CSema maeshita(Peak[i].StartSema.idx, Peak[i].StartSema.x, Peak[i].StartSema.y-diff_h);
		TagPoint.push_back(maeshita);
		CSema ushiroue(Peak[i].FinSema.idx, Peak[i].FinSema.x, Peak[i].FinSema.y+diff_h);
		TagPoint.push_back(ushiroue);
		CSema ushiroshita(Peak[i].FinSema.idx, Peak[i].FinSema.x, Peak[i].FinSema.y-diff_h);
		TagPoint.push_back(ushiroshita);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//  测试用的算法
//----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
VOID test_GetDividePeakPoint(std::vector<CPeak> &Peak, unsigned bg, unsigned ed)
{
	if (bg==ed)
	{
		return;
	}

	//标记垂直分峰
	for (unsigned i=bg; i<=ed; ++i)
		Peak[i].dType = CPeak::VERTDIVIDE;

	double k = (Peak[ed].FinSema.y-Peak[bg].StartSema.y)/(Peak[ed].FinSema.x-Peak[bg].StartSema.x);
	double c = Peak[bg].StartSema.y-k*Peak[bg].StartSema.x;
	double tmp=Peak[bg].StartSema.y;
	for (;bg!=ed; ++bg)
	{
		

		double _y = Peak[bg].FinSema.x*k+c;
		//if ((Peak[bg].Type>1 && _y > Peak[bg].FinSema.y) || (Peak[bg].Type<-1 && _y < Peak[bg].FinSema.y))
		//{
		//	//Peak[bg].StartSema.y = tmp;//
		//	//Peak[bg+1].Type = Peak[bg+1].Type/abs(Peak[bg+1].Type);

		//	if (bg+1!=ed)
		//		++bg;
		//}
		//else
		//{
			Peak[bg].FinSema.y = _y;
			tmp = Peak[bg+1].StartSema.y;
			Peak[bg+1].StartSema.y = _y;
		//}
	}
}
//-----------------------------------------------------------------------------
//功能：垂直分峰算法
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//输出参数：
//			1.vector<CPeak> &Peak 垂直分峰后的波
//-----------------------------------------------------------------------------
VOID test_DividePeak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak)  //重写垂直分峰算法        
{// 垂直分峰    

	const unsigned PeakSize = Peak.size();
	unsigned bg=0;
	unsigned ed=bg;
	bool getIndex = false; //获得计算下标
	for (; bg<PeakSize; ++bg)
	{
		if (Peak[bg].Type==1 || Peak[bg].Type==-1) //单峰情况, 不做出任何改变
		{
			continue; //之后算法进行基线校正
		}
		else //获得连续峰下标
		{
			ed = bg+1;
			int mark = Peak[bg].Type;
			for (; ed<PeakSize; ++ed)
			{
				if (ed+1==PeakSize && Peak[PeakSize-1].Type==mark)
				{
					getIndex = true;
					break;
				}
				if (Peak[ed].Type!=mark)
				{
					ed--;
					getIndex = true;
					break;
				}
			}
		}

		if (getIndex) //连续峰情况
		{
			unsigned bgPeak = bg;
			unsigned edPeak = ed;
			bool finish = false;
loop:
			while ((ed!=bg) || (!finish))
			{
				if (bg==ed)
				{
					Peak[bg].Type = 1;
				}
				
				double k = (Peak[ed].FinSema.y-Peak[bg].StartSema.y)/(Peak[ed].FinSema.x-Peak[bg].StartSema.x);
				double c = Peak[bg].StartSema.y-k*Peak[bg].StartSema.x;
				for (unsigned i=Peak[bg].StartSema.idx; i<Peak[ed].FinSema.idx; ++i)
				{
					double line_y = handleSema[i].x*k+c;
					if (line_y> handleSema[i].y && (bg!=ed)) //当前之考虑正峰情况
					{
						ed--;
						break; //切线上点大于曲线上的点，抛弃最后一个峰
					}
					if (i==Peak[ed].FinSema.idx-2 /*|| (bg+1==ed)*/ || (bg==ed))
					{
						test_GetDividePeakPoint(Peak, bg, ed);//求当前连续峰的基点
						if (edPeak==ed)
						{
							//处理完毕！
							finish = true;
							bg = ed;
							break;
						}
						else
						{
							bg = ed+1;
							ed = edPeak;
							goto loop;
						}
					}
				}
			}
			getIndex = false;
		}	
	}
}
//-----------------------------------------------------------------------------
//功能：删除相邻2点距离间隔的成峰
//传入参数：
//			1.vector<CPeak> &Peak 自定义的Peak类型变量
//输出参数：
//			1.vector<CPeak> &Peak 删除相邻2点距离间隔的成峰后的波
//-----------------------------------------------------------------------------
VOID test_ErasePeakByNextto(std::vector<CPeak> &Peak)
{
	
	std::vector<CPeak> Tmp;
	const unsigned PeakSize = Peak.size();
	if (PeakSize==0) return;
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (  (Peak[i].StartSema.idx==Peak[i].PeakSema.idx)
			||(Peak[i].StartSema.idx+1==Peak[i].PeakSema.idx)
			||(Peak[i].StartSema.idx+2==Peak[i].PeakSema.idx)
			||(Peak[i].PeakSema.idx==Peak[i].FinSema.idx)
			||(Peak[i].PeakSema.idx+1==Peak[i].FinSema.idx)
			||(Peak[i].PeakSema.idx+2==Peak[i].FinSema.idx))
		{
			continue;
		}
		else
			Tmp.push_back(Peak[i]);
	}
	Peak.swap(Tmp);
	//删峰后改变峰类型
	const unsigned SwapPeakSize = Peak.size();
	typedef std::vector<CPeak>::iterator CI;
	for (CI it=Peak.begin(); it!=Peak.end(); ++it)
	{
		if (abs(it->Type)!=1)
		{
			//int PeakType = it->Type;
			int times = count(Peak.begin(), Peak.end(), it->Type);
			//int times=0;
			//for (unsigned i=0; i<SwapPeakSize; ++i)
			//{
			//	if (Peak[i].Type==(it->Type))
			//		times++;
			//}
			if (times==1)
			{
				it->Type = (it->Type)/abs(it->Type); //波类型变为单峰
			}
		}
	}
}


VOID test_GetpeakSearch(const std::vector<CSema> &avgData,   //平均化好的信号量
					   const std::vector<double> &firstOrder,//平均化好的信号量的斜率
					   std::vector<CPeak> &Peak,    //(输出)起点容器
					   double slopeBased, //门限斜率
					   bool hasNagetiveSpike = true,//输入变量，考虑是否有负峰情况
					   bool isTwiceJudge = true)   

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
	const int series = 2;
	while (hState != HANDLE_ACCOMPLISH)
	{
		switch (hState)
		{
		case STATELESS:  //无状态
			for ( ; i<avgDataSize-series; ++i)  //
			{
				if (i==avgDataSize-series-1) //
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}

				if (firstOrder[i]<0) //负峰考虑情况
				{
					if (hasNagetiveSpike)
					{
						if (fabs(firstOrder[i])<slopeBased)
							continue;
						if (fabs(firstOrder[i])>=slopeBased)
						{
							if (!isTwiceJudge) //不进行连续2点判断
							{
								isNagetiveSpike = true;
								getStartPoint = i;
								hState = DOWNGOING_WAVE;
								break;
							}
							if (fabs(firstOrder[i+1])<slopeBased)
								continue;
							if (fabs(firstOrder[i+1])>=slopeBased)
							{
								isNagetiveSpike = true;
								getStartPoint = i;
								hState = DOWNGOING_WAVE;
								break;
							}
						}
					}
					else
						continue;
				}	

				if (firstOrder[i]>=0) //非负峰考虑情况
				{
					if (firstOrder[i]<slopeBased)
						continue;
					if (firstOrder[i]>slopeBased)
					{
						if (!isTwiceJudge) //不进行连续2点判断
						{
							getStartPoint = i;
							hState = UPCOMING_WAVE;
							break;
						}
						if (firstOrder[i+1]<slopeBased)
							continue;
						if (firstOrder[i+1]>slopeBased)
						{
							getStartPoint = i;
							hState = UPCOMING_WAVE;
							break;
						}
					}
				}
			}
			break;

		case UPCOMING_WAVE: //上行波
			if (!isNagetiveSpike)
			{
				for (j=i+1; j<avgDataSize-1; ++j)
				{
					if (j==avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]<0)
						continue;
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j+1]>0)
							continue;
						if (firstOrder[j+1]<0 && firstOrder[j+2]<0) //2010.08.25 添加对波峰噪点的处理
						{
							getpeakPoint = j+1;
							hState = DOWNGOING_WAVE;
							break;
						}
					}
				}
			}
			else
			{
				for (j=k+1; j<avgDataSize-1; ++j)
				{
					if (j==avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j]>slopeBased)  //2010.08.25 添加平头峰处理条件
						{	
							processed = true;
							continue;
						}
						if (processed && firstOrder[j]<slopeBased ) 
						{
							if (!isTwiceJudge) //不进行连续2点判断
							{
								getFinishPoint = j+1;
								hState = STATELESS;
								i = j;
								break;
							}
							if (firstOrder[j+1]<slopeBased)
							{
								getFinishPoint = j+1;
								hState = STATELESS;
								i = j;
								break;
							}
						}
					}
					if (firstOrder[j]<0)
					{
						getFinishPoint = j;
						hState = STATELESS;
						i = j-1;
						break;
					}

				}
				if (hState==STATELESS)
				{
					processed = false;

					CPeak OnePeak(avgData[getStartPoint],avgData[getpeakPoint],avgData[getFinishPoint]);
					Peak.push_back(OnePeak);

					unsigned t = Peak.size()-1;
					if (t>1 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
					{
						Peak[t-1].Type = -mark;// 之前的标志改为连续波谷
						Peak[t].Type = -mark;  //连续波谷
					}
					else
					{
						Peak[t].Type = -1; //单个波谷
						mark++;
					}


					isNagetiveSpike = false;

				}
			}
			break;

		case DOWNGOING_WAVE: //下行波
			if (!isNagetiveSpike)
			{
				for (k = j+1; k<avgDataSize-1; ++k)
				{
					if (k==avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if ( firstOrder[k]<0 ) 
					{
						if (fabs(firstOrder[k])>slopeBased)  //2010.08.24 添加平头峰处理条件
						{
							processed = true;
							continue;
						}

						if ( processed && (fabs(firstOrder[k]) < slopeBased) && (firstOrder[k]!=0.000000))
						{
							if (!isTwiceJudge) //不进行连续2点判断
							{
								getFinishPoint = k+1;
								hState = STATELESS;
								i = k;
								break;
							}
							if ( fabs(firstOrder[k+1])<slopeBased && (firstOrder[k+1]!=0.000000)/* && firstOrder[k]<0*/)
							{
								getFinishPoint = k+1;
								hState = STATELESS;
								i = k;
								break;
							}
							if (fabs(firstOrder[k+1])>slopeBased)
								continue;

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
					Peak.push_back(OnePeak);

					unsigned t = Peak.size()-1;

					if (t>1 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
					{
						Peak[t-1].Type = mark;	// 之前的标志改为连续波峰
						Peak[t].Type = mark;	//连续波峰
					}
					else
					{
						Peak[t].Type = 1; //单个波峰
						mark++;
					}
				}
			}
			else
			{
				for (k = i+1; k<avgDataSize-1; ++k)
				{
					if (k==avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[k]>0)
						continue;
					if (firstOrder[k]<=0)
					{
						if (firstOrder[k+1]<0)
							continue;
						if (firstOrder[k+1]>0)
						{
							getpeakPoint = k+1;
							hState = UPCOMING_WAVE;
							break;
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



//-----------------------------------------------------------------------------
//功能：获得输入信号量的起点波峰终点带内部连续点判断
//传入参数：
//			1.vector<CSema> &avgData平均化好的信号量
//			2.vector<double> &firstOrder 平均化好的信号量的斜率
//			3.slopeBased 门限斜率
//			4.hasNagetiveSpike 负峰情况
//			5.series 内部连续N点判断
//输出参数：
//			1.vector<CPeak> &Peak 信号量的起点波峰终点
//-----------------------------------------------------------------------------
VOID test_GetpeakSearch_Series(const std::vector<CSema> &avgData,   //平均化好的信号量
								const std::vector<double> &firstOrder,//平均化好的信号量的斜率
								std::vector<CPeak> &Peak,    //(输出)起点容器
								double slopeBased, //门限斜率
								bool hasNagetiveSpike,//输入变量，考虑是否有负峰情况
								const int series = 2 )   

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
			for ( ; i<avgDataSize-series && hState==STATELESS; ++i)  //
			{
				if (i>=avgDataSize-series-1) //
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}

				if (firstOrder[i]<=0)
				{//负峰起点
					if (hasNagetiveSpike) //负峰考虑情况
					{
						if (fabs(firstOrder[i])<slopeBased)
							continue;
						if (fabs(firstOrder[i])>=slopeBased)
						{
							for (int s=1; s<=series; ++s)
							{
								if (fabs(firstOrder[i+s])<slopeBased)
									break;
								if (fabs(firstOrder[i+s])>slopeBased)
								{
									if (s==series)
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
					if (firstOrder[i]>slopeBased)
					{
						for (int s=1; s<=series; ++s)
						{
							if (fabs(firstOrder[i+s])<slopeBased)
								break;
							if (fabs(firstOrder[i+s])>=slopeBased)
							{
								if (s==series)
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
					if (j>=avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}

					if (firstOrder[j]<0)
						continue;
					if (firstOrder[j]>=0)
					{
						for (int s=1; s<=series; ++s)
						{
							if (firstOrder[j+s]>0)//
								break;
							if (firstOrder[j+s]<0)
							{
								if (s==series)
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
					if (j>=avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[j]>=0)
					{
						if (firstOrder[j]>slopeBased)  //2010.08.25 添加平头峰处理条件
						{	
							processed = true;
							continue;
						}
						if (processed && firstOrder[j]<slopeBased && (firstOrder[j]!=0.000000)) 
						{
							for (int s=1; s<=series; ++s)
							{
								if (firstOrder[j+s]>slopeBased)
									break;
								if (firstOrder[j+s]<slopeBased  && (firstOrder[j+s]!=0.000000))
								{
									if (s==series)
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
					Peak.push_back(OnePeak); //负峰

					unsigned t = Peak.size()-1;
					if (t>1 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
					{
						Peak[t-1].Type = -mark;// 之前的标志改为连续波谷
						Peak[t].Type = -mark;  //连续波谷
					}
					else
					{
						Peak[t].Type = -1; //单个波谷
						mark++;
					}
					isNagetiveSpike = false;
				}
			}
			break;

		case DOWNGOING_WAVE: //下行波
			if (!isNagetiveSpike)
			{
				for (k=j/*+1*/; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					
					if ( firstOrder[k]<0 ) 
					{
						if (fabs(firstOrder[k])>slopeBased)  //2010.08.24 添加平头峰处理条件
						{
							processed = true;
							continue;
						}

						if (processed && (fabs(firstOrder[k]) < slopeBased) && (firstOrder[k]!=0.000000))
						{
							for (int s=1; s<=series; ++s)
							{
								if (fabs(firstOrder[k+s])>slopeBased)
									break;
								if (fabs(firstOrder[k+s])<slopeBased && (firstOrder[k+s]!=0.000000))
								{
									if (s==series)
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
					Peak.push_back(OnePeak);  //正峰

					unsigned t = Peak.size()-1;

					if (t>1 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
					{
						Peak[t-1].Type = mark;	// 之前的标志改为连续波峰
						Peak[t].Type = mark;	//连续波峰
					}
					else
					{
						Peak[t].Type = 1; //单个波峰
						mark++;
					}
				}
			}
			else
			{//负峰波谷
				for (k=i+1; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-series-1)
					{
						hState = HANDLE_ACCOMPLISH;
						break;
					}
					if (firstOrder[k]>0)
						continue;

					if (firstOrder[k]<=0)
					{
						for (int s=1; s<=series; ++s)
						{
							if (firstOrder[k+s]<0)
								break;
							if (firstOrder[k+s]>0)
							{
								if (s==series)
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
//----------------------------------------------------------------------------------------------------------------------
// 追加3项新功能
//----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//功能：对单个峰进行基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//			3.PeakNo 波编号
//			4.hasNagetiveSpike 负峰情况
//输出参数：
//			1.vector<CPeak> &Peak 对单个峰进行基线校正后的波
//-----------------------------------------------------------------------------
VOID one_BaselineCheak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, const int PeakNo, bool hasNagetiveSpike = true)
{
	bool isfail = true;
	unsigned i = PeakNo;
	Peak[i].Type = 1;
	unsigned j = 0;
	for (unsigned j=Peak[i].StartSema.idx; j<=Peak[i].PeakSema.idx && isfail; ++j)	
	{
		double k = (handleSema[Peak[i].FinSema.idx].y-handleSema[j].y)/(handleSema[Peak[i].FinSema.idx].x-handleSema[j].x);
		double c = handleSema[j].y-k*handleSema[j].x;
		for (unsigned m=j; m<=Peak[i].PeakSema.idx; ++m) 
		{
			double _y = k*handleSema[m].x+c;
			if (hasNagetiveSpike) //考虑负峰情况
			{
				if (/*getIndex && */fabs(_y) > fabs(handleSema[m].y))
				{
					isfail = true;
					break;
				}
			}
			else
			{
				if (/*getIndex && */fabs(_y) > fabs(handleSema[m].y) && _y>0)
				{
					isfail = true;
					break;
				}
				else if(/*getIndex && */fabs(_y) < fabs(handleSema[m].y) && _y<=0)
				{
					isfail = true;
					break;
				}
			}
			isfail = false;
		}
		if (!isfail)
		{
			//调整起点
			Peak[i].StartSema = handleSema[j];
			isfail = false;
		}
	} isfail = true;
	for (unsigned l=Peak[i].FinSema.idx; l>=Peak[i].PeakSema.idx && isfail; --l)
	{
		double k = (handleSema[l].y-handleSema[j].y)/(handleSema[l].x-handleSema[j].x);
		double c = handleSema[l].y-k*handleSema[l].x;
		for (unsigned n=l; n>=Peak[i].PeakSema.idx; --n)
		{
			double _y = k * handleSema[n].x + c;
			if (hasNagetiveSpike) //考虑负峰情况
			{
				if (/*getIndex && */fabs(_y) > fabs(handleSema[n].y))//
				{
					isfail = true;
					break;
				}
			}
			else
			{
				if (/*getIndex && */fabs(_y) > fabs(handleSema[n].y) && _y>0)//
				{
					isfail = true;
					break;
				}
				else if(/*getIndex && */fabs(_y) < fabs(handleSema[n].y) && _y<=0)//
				{
					isfail = true;
					break;
				}
			}
			isfail = false;				
		}
		if (!isfail)
		{
			//调整落点
			Peak[i].FinSema = handleSema[l];
		}
	}
};
//-----------------------------------------------------------------------------
//功能：对单个峰获得峰宽
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//			3.PeakNo 波编号
//			4.isDividePeak 垂直分峰
//输出参数：
//			1.vector<CPeak> &Peak 对单个峰获得峰宽后的波
//-----------------------------------------------------------------------------
int one_GetPeakWidth( const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, const int PeakNo, bool isDividePeak=false)
{
	//const unsigned PeakSize = Peak.size();
	////const int factor = 1; //Y轴统一因子
	//for (unsigned i=0; i<PeakSize; ++i)
	//{
		unsigned i = PeakNo;

		double x1 = Peak[i].StartSema.x;
		double y1 = Peak[i].StartSema.y;
		double x2 = Peak[i].FinSema.x;
		double y2 = Peak[i].FinSema.y;
		double peakWidth = fabs(x2-x1);
		Peak[i].PeakWidth = peakWidth;  //峰宽
		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;
		double py = Peak[i].PeakSema.y; 
		double px = Peak[i].PeakSema.x;
		double ty = px*tana+c;
		double peakHeight = fabs(fabs(ty)-fabs(py));
		Peak[i].PeakHeight = peakHeight; //峰高
		double PeakWidthLeft  = fabs(px-x1);
		double PeakWidthRight = fabs(x2-px);
		double lr = PeakWidthLeft/PeakWidthRight;
		Peak[i].Tailing = lr; //拖尾因子
		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;
		bool getHalfStart = false;
		bool getHalfFin = false;
		if (isDividePeak)
		{
			unsigned StartIdx = Peak[i].StartSema.idx;
			unsigned StartIdxNext = Peak[i].StartSema.idx+1;
			double gapPre_start = fabs(handleSema[StartIdx].y);
			double gapNext_start = fabs(handleSema[StartIdxNext].y);
			double gap_start = fabs(handleSema[StartIdx].x * tana + _c);
			if (gap_start>=gapPre_start && gap_start<=gapNext_start) //半峰宽起点在分峰线上
			{
				CSema temp(StartIdx,Peak[i].StartSema.x, gap_start); //半峰起点	
				Peak[i].MidStartSema = temp;
				getHalfStart = true;
			}

			unsigned FinIdx = Peak[i].FinSema.idx-1;
			unsigned FinIdxNext = Peak[i].StartSema.idx;
			double gapPre_fin = fabs(handleSema[FinIdx].y);
			double gapNext_fin = fabs(handleSema[FinIdxNext].y);
			double gap_fin = fabs(handleSema[FinIdx].x * tana + _c);
			if (gap_fin>=gapPre_fin && gap_fin<=gapNext_fin) //半峰宽落点在分峰线上
			{
				CSema temp(FinIdxNext, Peak[i].FinSema.x, gap_fin); //半峰落点	
				Peak[i].MidFinSema = temp;
				getHalfStart = true;
			}
		}

		double diff_y = INT_MAX;
		unsigned idxl, idxr;
		bool getidxl = false;
		bool getidxr = false;
		if (!getHalfStart)
		{
			for (unsigned l=Peak[i].StartSema.idx; l<=Peak[i].PeakSema.idx; ++l) //上行，逼近切点
			{
				double ly = handleSema[l].x * tana + _c;
				if (diff_y >= fabs(handleSema[l].y - ly))
				{
					diff_y = fabs(handleSema[l].y - ly);
					idxl = l;
					getidxl = true;
				}
			}
			if (getidxl)
			{
				CSema tmp(handleSema[idxl]); //半峰起点
				Peak[i].MidStartSema = tmp;
			}
		}
		diff_y = INT_MAX;
		if (!getHalfFin)
		{
			
			for (unsigned r=Peak[i].PeakSema.idx; r<=Peak[i].FinSema.idx; ++r) //下行，逼近切点
			{
				double ry = handleSema[r].x * tana + _c;

				if (diff_y >= fabs(handleSema[r].y - ry))
				{
					diff_y = fabs(handleSema[r].y - ry);
					idxr = r;
					getidxr = true;
				}
			}
			if (getidxr)
			{
				CSema tmp(handleSema[idxr]);   //半峰落点
				Peak[i].MidFinSema = tmp;
			}
		}
		if (!getidxl && !getidxr)
		{
			::AfxMessageBox(_T("起点落点设置有误，请重新输入"));
			return -1;
		}


		double hk = (Peak[i].MidFinSema.y-Peak[i].MidStartSema.y)/(Peak[i].MidFinSema.x-Peak[i].MidStartSema.x);
		double hc = Peak[i].MidStartSema.y - Peak[i].MidStartSema.x * hk;
		double rey = hx*hk+hc;
		CSema HalfPeak(Peak[i].PeakSema.idx, hx, rey); // 半峰中点
		Peak[i].MidPeakSema = HalfPeak;
		//CPeak OneHalf(HalfStart, HalfPeak, HalfFin);
		//Half.push_back(OneHalf);  //半峰容器
	

	//连接半峰宽直线，与波峰的垂线相交于一点。并求出半峰宽前后长度
	//const unsigned HalfSize = Half.size();
	//ASSERT(HalfSize==PeakSize);
	//for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].MidStartSema.x; 
		double y1 = Peak[i].MidStartSema.y;
		double x2 = Peak[i].MidFinSema.x; 
		double y2 = Peak[i].MidFinSema.y;
		//double halfPeakWidth = sqrt((y2/factor-y1/factor)*(y2/factor-y1/factor)+(x2-x1)*(x2-x1));
		double halfPeakWidth = fabs(x2-x1);
		Peak[i].HalfPeakWidth = halfPeakWidth;  //求半峰宽

		double x0 = Peak[i].MidPeakSema.x;      //求半峰宽前后长度
		double y0 = Peak[i].MidPeakSema.y;
		//double halfPeakWidthLeft =  sqrt((y0/factor-y1/factor)*(y0/factor-y1/factor)+(x0-x1)*(x0-x1));
		//double halfPeakWidthRight = sqrt((y2/factor-y0/factor)*(y2/factor-y0/factor)+(x2-x0)*(x2-x0));
		double halfPeakWidthLeft =  fabs(x0-x1);
		double halfPeakWidthRight = fabs(x2-x0);
		double disfactor = halfPeakWidthLeft/halfPeakWidthRight;
		Peak[i].Disfactor = disfactor;  //求畸变因子
	}
	return 0;
}
//-----------------------------------------------------------------------------
//功能：对单个峰获得峰面积
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//			3.PeakNo 波编号
//输出参数：
//			1.vector<CPeak> &Peak 对单个峰获得峰面积后的波
//-----------------------------------------------------------------------------
VOID one_GetPeakArea(const std::vector<CSema> &handleSema,std::vector<CPeak> &Peak, const int PeakNo)
{
	const unsigned PeakSize = Peak.size();
	const unsigned handleSemaSize = handleSema.size();
	unsigned j = 0;
	/*for (unsigned i=0; i<PeakSize; ++i)  
	{*/
	unsigned i = PeakNo;
	double x2 = Peak[i].FinSema.x;           
	double y2 = Peak[i].FinSema.y;
	double x1 = Peak[i].StartSema.x;
	double y1 = Peak[i].StartSema.y;
	double k = (y2-y1)/(x2-x1);
	double c = y1-k*x1;

	double area = 0.0;
	for (; j<handleSemaSize; ++j)
	{
		if (handleSema[j].x-Peak[i].StartSema.x >= 0.000000) //找到起点
		{	
			do 
			{
				double x4 = handleSema[j+1].x;
				double y4 = handleSema[j+1].y;
				double x3 = handleSema[j].x;
				double y3 = handleSema[j].y;

				double h = x4 - x3;
				double up = fabs(fabs(y3)-fabs(x3*k+c));
				double down = fabs(fabs(y4)-fabs(x4*k+c));
				area += (up+down)*h/2;
				j++;
			} while (handleSema[j].x-Peak[i].FinSema.x <= 0.000000); //找到终点
			Peak[i].PeakArea = area;
			break;
		}
	}
	//}
}
//-----------------------------------------------------------------------------
//功能：手动对单个峰进行基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//			3.Para& p 单峰参数
//输出参数：
//			1.vector<CPeak> &Peak 手动对单个峰进行基线校正后的波
//-----------------------------------------------------------------------------
VOID neo_BaseLineAdj(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, Para& p) // 1. 基线调整
{
	std::vector<CPeak> PeakCopy(Peak); 
	const unsigned SemaSize = handleSema.size();
	const int PeakSize = Peak.size();
	const int PeakNo = p.pn-1;
	if ((PeakNo>PeakSize-1)||(PeakNo<0)||PeakSize==0 ) //调整的波不存在
		return;
	if (p.st > p.ft)
	{
		double tmp = p.ft;
		p.ft = p.st;
		p.st = tmp;
	}
	//先调整起点
	unsigned ml = Peak[PeakNo].StartSema.idx; //左移下标
	unsigned mr = Peak[PeakNo].StartSema.idx; //右移下标
	if (Peak[PeakNo].StartSema.x>=p.st)
	{
		while (ml>1)
		{
			if (p.st<=handleSema[ml].x && p.st>=handleSema[ml-1].x)
			{
				Peak[PeakNo].StartSema = handleSema[ml];
				break;
			}
			ml--;
		}
	}
	else
	{
		while (mr<SemaSize-1)
		{
			if (handleSema[mr].x<=p.st && handleSema[mr+1].x>=p.st)
			{
				Peak[PeakNo].StartSema = handleSema[mr];
				break;
			}
			mr++;
		}
	}
	//调整落点
	ml = Peak[PeakNo].FinSema.idx; //左移下标
	mr = Peak[PeakNo].FinSema.idx; //右移下标
	if (Peak[PeakNo].FinSema.x>=p.ft)
	{
		while (ml>1)
		{
			if (p.ft<=handleSema[ml].x && p.ft>=handleSema[ml-1].x)
			{
				Peak[PeakNo].FinSema = handleSema[ml];
				break;
			}
			ml--;
		}
	}
	else
	{
		while (mr<SemaSize-1)
		{
			if (handleSema[mr].x<=p.ft && handleSema[mr+1].x>=p.ft)
			{
				Peak[PeakNo].FinSema = handleSema[mr];
				break;
			}
			mr++;
		}
	}
	one_BaselineCheak(handleSema, Peak, PeakNo, false); //调整基线
	//new_PeakTypeChange(m_Peak); //波类型发生改变
	int ret = one_GetPeakWidth(handleSema, Peak, PeakNo, false); //峰宽，半峰宽调整
	if (ret == -1)
	{
		Peak.swap(PeakCopy);
		return;
	}
	one_GetPeakArea(handleSema, Peak, PeakNo); //峰面积
	new_PeakTypeChange(Peak);
}

int BinarySearch(const std::vector<CSema> &handleSema, double x) 
{ 
	int n = handleSema.size();
	int left=0;
	int right=n-1;
	while(left<=right)
	{ 
		int middle=(left+right)/2;
		if (x<=handleSema[middle].x && x>=handleSema[middle-1].x) return middle;
		if (x>handleSema[middle].x) left=middle+1;
		else right=middle-1;
	} 
	return -1;
}

//-----------------------------------------------------------------------------
//功能：手动增加一个峰
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//			3.Para& p 单峰参数
//输出参数：
//			1.vector<CPeak> &Peak 手动增加一个峰后的波
//-----------------------------------------------------------------------------
VOID neo_AddNewPeak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, Para& p)	// 3. 给定参数增加峰
{
	const unsigned SemaSize = handleSema.size();
	const unsigned PeakSize = Peak.size();
	if (SemaSize==0) //调整的波不存在
		return;
	if (p.st > p.ft)
	{
		double tmp = p.ft;
		p.ft = p.st;
		p.st = tmp;
	}
	int ml = BinarySearch(handleSema, p.st); //二分查找起点索引
	int mr = BinarySearch(handleSema, p.ft); //二分查找终点索引

	double peak = 0.0;
	int mm = 0; 
	for (int i=ml; i<=mr; ++i) //顺序查找波(峰)
	{
		if (handleSema[i].y>=peak)
		{
			peak = handleSema[i].y;
			mm = i; 
		}
	}
	//这里需要对插入的人工峰排序
	CPeak newPeak(handleSema[ml], handleSema[mm], handleSema[mr]);
	//std::vector<CPeak> PeakCopy;
	int PeakNo = 0;
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (handleSema[mm].x < Peak[i].PeakSema.x)
		{
			Peak.insert(Peak.begin()+i, newPeak);
			PeakNo = i;
			break;
		}
	}
	if (handleSema[mm].x > Peak[PeakSize-1].PeakSema.x)
	{
		Peak.push_back(newPeak);
		PeakNo = PeakSize;
	}


	//Peak.push_back(newPeak);
	one_BaselineCheak(handleSema, Peak, PeakNo, false); //调整基线
	int ret = one_GetPeakWidth(handleSema, Peak, PeakNo, false);
	if (ret == -1)
	{
		//Peak.swap(PeakCopy);
		return;
	}
	one_GetPeakArea(handleSema, Peak, PeakNo);

}
//-----------------------------------------------------------------------------
//功能：手动删除一个峰
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//			3.Para& p 单峰参数
//输出参数：
//			1.vector<CPeak> &Peak 手动删除一个峰后的波
//-----------------------------------------------------------------------------
VOID neo_DelOnePeak(const std::vector<CSema> &handleSema, std::vector<CPeak> &Peak, Para& p)
{
	std::vector<CPeak> NewPeak;

	if (p.st > p.ft)
	{
		double tmp = p.ft;
		p.ft = p.st;
		p.st = tmp;
	}
	//int ml = BinarySearch(handleSema, p.st); //二分查找起点索引
	//int mr = BinarySearch(handleSema, p.ft); //二分查找终点索引
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (Peak[i].PeakSema.x<p.st || Peak[i].PeakSema.x>p.ft)
		{
			CPeak OnePeak(Peak[i]);
			NewPeak.push_back(OnePeak);
		}
	}
	Peak.swap(NewPeak);
	new_PeakTypeChange(Peak);
}



//----------------------------------------------------------------------------------------------------------------------
// 2010.12.09 - 2010.12.13 
// 更改寻峰接口，功能为对实时新增点的状态判定

static std::deque<CRealTimeSema> dequetmpSema;// 全局静态队列作为平均化缓存
static std::deque<CRealTimeSema> dequeBufSema;// 全局静态队列作为寻峰缓存

void RealTime_Averaging(const std::vector<CRealTimeSema> &InputSema, //传入:的信号量
						std::vector<CRealTimeSema> &OutputSema,		 //传出:平均化好的信号量
						std::vector<CRealTimeSema> &TotalAvgSema,    //总的平均化好的信号量
						std::vector<CRealTimeSema> &TotalSema,       //总的信号量
						unsigned avg )	//平均次数
{//step1: 求以avg为1点的平均曲线，剩余除不尽的为1点
	if (avg==0)
	{
		//MessageBox(_T("必须平均化"));
		return;
	}
	const unsigned TotalNumber = TotalSema.size();
	const unsigned InputNum = InputSema.size(); //输入信号量数量
	const unsigned TmpNum = dequetmpSema.size();  //存队列中信号量数量
	const unsigned CurrNum = InputNum + TmpNum;
	if (CurrNum > avg) //从队列中取出平均化
	{
		dequetmpSema.insert(dequetmpSema.end(), InputSema.begin(), InputSema.end());
		for (unsigned i=0; i<=CurrNum; ++i)
		{
			double avgTime = 0.0;
			double avgSema = 0.0;
			if (i % avg == 0 && i+avg < CurrNum) 
			{
				for (unsigned j=i; j< i+avg; j++)
				{
					avgTime += dequetmpSema[0].x;
					avgSema += dequetmpSema[0].y;
					TotalSema.push_back(dequetmpSema[0]);
					dequetmpSema.pop_front(); //弹出
				}
				avgTime = avgTime / avg;
				avgSema = avgSema / avg;

				CRealTimeSema OneRTSema(i+TotalNumber, avgTime, avgSema);
				OutputSema.push_back(OneRTSema);
				//TotalAvgSema.push_back(OneRTSema);放到RealTime_GetAvgDataSlope函数内添加
			}
			if (dequetmpSema.size()<avg) //剩余
			{
				break;
			}
		}
	}
	else //直接放在队列中
	{
		dequetmpSema.insert(dequetmpSema.end(), InputSema.begin(), InputSema.end());
	}
}


void RealTime_GetAvgDataSlope(std::vector<CRealTimeSema> &InputSema, //平均化好的信号量
							  std::vector<CRealTimeSema> &TotalAvgSema,
							  std::vector<double> &firstOrder,	//斜率存放容器
							  double &maxSlope)					//最大斜率            
{//step2: 求出平均曲线上每一点的斜率

	//以下需要优化
	if (!firstOrder.empty())
	{
		const unsigned totsize = TotalAvgSema.size()-1;
		double ydiff = InputSema[0].y - TotalAvgSema[totsize].y;
		double xdiff = InputSema[0].x - TotalAvgSema[totsize].x;
		double first_k = ydiff / xdiff;
		firstOrder.push_back(first_k);
		if (first_k>=maxSlope)
			maxSlope =first_k;
	}
	
	const unsigned avgDataSize = InputSema.size()-1;
	double *k = new double[avgDataSize];
	for (unsigned i=0; i<avgDataSize; ++i)
	{	
		double ydiff = InputSema[i+1].y - InputSema[i].y;
		double xdiff = InputSema[i+1].x - InputSema[i].x;
		k[i] = (ydiff) / (xdiff);
		if (k[i]>=maxSlope)
			maxSlope = k[i];
		firstOrder.push_back(k[i]);
	}
	delete [] k;
	k = NULL;

	for (unsigned i=0; i<InputSema.size(); ++i)
	{
		
		TotalAvgSema.push_back(InputSema[i]);
	}

	//InputSema.clear(); //可以之后删除
	
	//dequetmpSema.clear();//可以之后删除
	

}

void RealTime_GetpeakSearch(std::vector<CRealTimeSema> &InputSema, //平均化好的信号量
							const std::vector<double>  &firstOrder,		//平均化好的信号量的斜率
							std::vector<CRealTimeSema> &TotAvgSema,
							std::vector<CRealTimeSema> &RealTimeSema,   //(输出)起点容器
							const double slopeBased, bool hasNagetiveSpike, bool isTwiceJudge)
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
		
		HANDLE_ACCOMPLISH   //处理完毕
	} hState;				//处理过程中的标记


		////未知状态
		//UNKNOWN,		  //初始化
		//HANDLED,          //已处理，但无法判断处于何种状态
		////正峰情况
		//PLUS_START_POINT, //起点
		//PLUS_RAISE_POINT, //上升
		//PLUS_PEAK_POINT,  //波峰点
		//PLUS_DROP_POINT,  //下降
		//PLUS_FINISH_POINT, //终点
		////负峰情况
		//MINUS_START_POINT, //起点
		//MINUS_DROP_POINT,  //下降
		//MINUS_TROUGH_POINT,//波谷点
		//MINUS_RAISE_POINT, //上升
		//MINUS_FINISH_POINT //终点

	//处理前的数据初始化
	static enumState lastState = STATELESS; //上一次未处理完毕的状态
	static unsigned id = 0;//静态（firstOrder下标）
	id -= dequeBufSema.size(); //调整计数下标
	const unsigned DataSize = InputSema.size()+dequeBufSema.size(); //这次进行处理的数据个数
	dequeBufSema.insert(dequeBufSema.end(),InputSema.begin(),InputSema.end()); //输入的信号都放到队列末端
	InputSema.clear(); //输入信号清空,这步可以放到下一个函数中运行
	hState = lastState;
	unsigned i=0, j=0, k=0;

	//unsigned getStartPoint=0, getpeakPoint=0, getFinishPoint=0;
	//int mark = 1; //波类型标记: 1=波峰; -1=波谷; 2,3,4,...为重叠波峰; -2,-3,-4...为重叠波谷
	//bool isNagetiveSpike = false;
	
	while (hState != HANDLE_ACCOMPLISH)
	{
		switch (hState)
		{
		case STATELESS:  //无状态
			lastState = STATELESS;
			for ( ; i<=DataSize-1; ++i, ++id)
			{
				if (i>=DataSize-1) 
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}
				if (firstOrder[id]>=0) //非负峰考虑情况
				{
					if (firstOrder[id]<slopeBased)
					{
						//InputSema[i].pts = CRealTimeSema::HANDLED; //标记已处理
						continue;
					}
					if (firstOrder[id]>slopeBased)
					{
						if (firstOrder[id+1]<slopeBased)
						{
							//InputSema[i+1].pts = CRealTimeSema::HANDLED; //标记已处理
							continue;
						}
						if (firstOrder[id+1]>=slopeBased)
						{
							//isNagetiveSpike = true;
							//getStartPoint = i;
							RealTimeSema.push_back(TotAvgSema[id]); //找到起点
							hState = PLUS_UPCOMING_WAVE; //正峰上行波
							break;
						}
					}
				}
			}
			break;


		case PLUS_UPCOMING_WAVE://正峰上行波
			lastState = PLUS_UPCOMING_WAVE;
			for (j=i/*+1*/; j<=DataSize-1; ++j, ++id)
			{
				if (j>=DataSize-1) 
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}
				if (firstOrder[id]<0)
				{
					//InputSema[i].pts = CRealTimeSema::PLUS_RAISE_POINT; //标记上升
					continue;
				}
				if (firstOrder[id]>=0)
				{
					if (firstOrder[id+1]>0)
					{
						//InputSema[i].pts = CRealTimeSema::PLUS_RAISE_POINT; //标记上升
						continue;
					}
					if (firstOrder[id+1]<0/* && firstOrder[j+2]<0*/) //2010.08.25 添加对波峰噪点的处理
					{
						//getpeakPoint = j+1;
						RealTimeSema.push_back(TotAvgSema[id+1]);//找到波峰
						hState = PLUS_DOWNGOING_WAVE; //正峰下行波
						break;
					}
				}
			}
			break;



		case PLUS_DOWNGOING_WAVE://正峰下行波
			lastState = PLUS_DOWNGOING_WAVE;
			for (k = j/*+1*/; k<=DataSize-1; ++k, ++id)
			{
				if (k>=DataSize-1) 
				{
					hState = HANDLE_ACCOMPLISH;
					break;
				}

				if ( firstOrder[id]<0 ) 
				{
	
					if ((fabs(firstOrder[id]) < slopeBased) && (firstOrder[id]!=0.000000))
					{
					
						if ( fabs(firstOrder[id+1])<slopeBased && (firstOrder[id+1]!=0.000000))
						{
							//getFinishPoint = k+1;
							RealTimeSema.push_back(TotAvgSema[id+1]); //找到终点
							hState = STATELESS;
							i = k;
							break;
						}
						if (fabs(firstOrder[id+1])>slopeBased)
							continue;

					}
				}
				if ( firstOrder[id+1]>0 )
				{
					//getFinishPoint = k;
					RealTimeSema.push_back(TotAvgSema[id]);
					hState = STATELESS;
					i = k;
					break;
				}
			}
			break;


		case MINUS_DOWNGOING_WAVE://负峰 上行波
			break;
		case MINUS_UPCOMING_WAVE://负峰 下行波
			break;
		case HANDLE_ACCOMPLISH://处理完毕
			break;
		default:
			break;
}}}

void RealTime_GetpeakSearch_v2(std::vector<CRealTimeSema> &InputSema, //平均化好的信号量
								const std::vector<double>  &firstOrder,		//平均化好的信号量的斜率
								std::vector<CRealTimeSema> &TotAvgSema,
								std::vector<CRealTimeSema> &RealTimeSema,   //(输出)起点容器
								const double slopeBased, bool hasMinusPeak, bool isTwiceJudge,
								const int series = 2)
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

	
	//未知状态
	//UNKNOWN,		  //初始化
	//HANDLED,          //已处理，但无法判断处于何种状态
	//正峰情况
	//PLUS_START_POINT, //起点
	//PLUS_RAISE_POINT, //上升
	//PLUS_PEAK_POINT,  //波峰点
	//PLUS_DROP_POINT,  //下降
	//PLUS_FINISH_POINT, //终点
	//PLUS_CONTINUOUS_WAVE, //连续峰
	//负峰情况
	//MINUS_START_POINT, //起点
	//MINUS_DROP_POINT,  //下降
	//MINUS_TROUGH_POINT,//波谷点
	//MINUS_RAISE_POINT, //上升
	//MINUS_FINISH_POINT //终点
	
	//处理前的数据初始化
	static enumState lastState = STATELESS; //上一次未处理完毕的状态
	static unsigned id = 0;//静态（firstOrder下标）
	static bool OnlyOnce = true;
	const unsigned firstOrderSize = firstOrder.size();
	unsigned DataSize = 0;
	if (OnlyOnce)
	{
		DataSize = InputSema.size() + dequeBufSema.size()-1; //这次进行处理的数据个数
		OnlyOnce = !OnlyOnce;
	} 
	else
		DataSize = InputSema.size() + dequeBufSema.size();
	dequeBufSema.insert(dequeBufSema.end(),InputSema.begin(),InputSema.end());//输入的信号都放到队列末端
	InputSema.clear(); //输入信号清空,这步可以放到下一个函数中运行
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
			for (; i<DataSize-series && hState==STATELESS; ++i, ++id)
			{
				if (i>=DataSize-series-1 || id>=firstOrderSize-series) 
				{
					++id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if (firstOrder[id]<=0)
				{//负峰起点
					if (hasMinusPeak) //考虑负峰情况
					{
						if (fabs(firstOrder[id])<slopeBased)
							continue;
						if (fabs(firstOrder[id])>=slopeBased)
						{
							for (int s=1; s<=series; ++s)
							{
								if (fabs(firstOrder[id+s])<slopeBased)
								{
									isContinuousWave = false; 
									break;
								}
								if (fabs(firstOrder[id+s])>slopeBased)
								{
									if (s==series)
									{
										if ((!RealTimeSema.empty())
											&& (RealTimeSema[RealTimeSema.size()-1].pts == CRealTimeSema::MINUS_CONTINUOUS_WAVE)
											&& isContinuousWave)
										{
											//::AfxMessageBox(_T("dequeBufSema.empty()"));
											RealTimeSema.push_back(RealTimeSema[RealTimeSema.size()-1]); //找到(连续)起点
										}
										//非连续峰情况
										else
										{
											dequeBufSema[i].pts = CRealTimeSema::MINUS_START_POINT; //标记为负峰起点
											RealTimeSema.push_back(dequeBufSema[i]); //找到起点

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
				if (firstOrder[id]>=0) //非负峰考虑情况
				{
					if (firstOrder[id]>slopeBased)
					{
						for (int s=1; s<=series; ++s)
						{
							if (firstOrder[id+s]<slopeBased)
							{
								isContinuousWave = false; 
								break;
							}
							if (firstOrder[id+s]>=slopeBased)
							{
								if (s==series)
								{
									if ((!RealTimeSema.empty())
										&& (RealTimeSema[RealTimeSema.size()-1].pts == CRealTimeSema::PLUS_CONTINUOUS_WAVE)
										&& isContinuousWave)
									{
										//::AfxMessageBox(_T("dequeBufSema.empty()"));
										RealTimeSema.push_back(RealTimeSema[RealTimeSema.size()-1]); //找到(连续)起点
									}
									//非连续峰情况
									else
									{
										dequeBufSema[i].pts = CRealTimeSema::PLUS_START_POINT; //标记为起点
										RealTimeSema.push_back(dequeBufSema[i]); //找到起点

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
			for (j=i; j<DataSize-series && hState == PLUS_UPCOMING_WAVE; ++j, ++id)
			{

				if (j>=DataSize-series-1 || id>=firstOrderSize-series) 
				{
					++id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if (firstOrder[id]<0)
					continue;
				if (firstOrder[id]>=0)
				{
					for (int s=1; s<=series; ++s)
					{
						if (firstOrder[id+s]>0)
							break;
						if (firstOrder[id+s]<0)
						{
							if (s==series)
							{
								dequeBufSema[j+s].pts = CRealTimeSema::PLUS_PEAK_POINT; //标记波峰
								RealTimeSema.push_back(dequeBufSema[j+s]); //找到波峰

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
			for (k = j; k<DataSize-series && hState == PLUS_DOWNGOING_WAVE; ++k, ++id)
			{
				if (k>=DataSize-series-1 || id>=firstOrderSize-series) 
				{
					++id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if ( firstOrder[id]<0 ) 
				{
					if (fabs(firstOrder[id])>slopeBased)  //2010.08.24 添加平头峰处理条件
					{
						isFlatPeakProcessed = true;
						continue;
					}

					if (isFlatPeakProcessed && (fabs(firstOrder[id]) < slopeBased) && (firstOrder[id]!=0.000000))
					{
						for (int s=1; s<=series; ++s)
						{
							if (fabs(firstOrder[id+s])>slopeBased)
								break;
							if (fabs(firstOrder[id+s])<slopeBased && (firstOrder[id+s]!=0.000000))
							{
								if (s==series)
								{
									//getFinishPoint = k+1;
									dequeBufSema[k+1].pts = CRealTimeSema::PLUS_FINISH_POINT; //标记终点
									RealTimeSema.push_back(dequeBufSema[k+1]); //找到终点
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
				if ( firstOrder[id]>0 )
				{
					//getFinishPoint = k;
					dequeBufSema[k+1].pts = CRealTimeSema::PLUS_CONTINUOUS_WAVE; //标记（连续）终点
					RealTimeSema.push_back(dequeBufSema[k+1]); //找到终点
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
			for (j=i; j<DataSize-series && hState==MINUS_DOWNGOING_WAVE; ++j,++id)
			{
				if (j>=DataSize-series-1 || id >= firstOrderSize-series) 
				{
					++id;
					hState = HANDLE_POSTPROCESS;
					break;
				}

				if (firstOrder[id]>0)
					continue;
				if (firstOrder[id]<=0)
				{
					for (int s=1; s<=series; ++s)
					{
						if (firstOrder[id+s]<0)
							break;
						if (firstOrder[id+s]>0)
						{
							if (s==series)
							{
								dequeBufSema[j+s].pts = CRealTimeSema::MINUS_TROUGH_POINT; //标记波谷
								RealTimeSema.push_back(dequeBufSema[j+s]); //找到波谷

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
			for (k=j; k<DataSize-series && hState==MINUS_UPCOMING_WAVE; ++k,++id)
			{
				if (k>=DataSize-series-1 || id >= firstOrderSize-series)
				{
					++id;
					hState = HANDLE_POSTPROCESS;
					break;
				}
				if (firstOrder[id]>=0)
				{
					if (firstOrder[id]>slopeBased)  //2010.08.25 添加平头峰处理条件
					{	
						isFlatPeakProcessed = true;
						continue;
					}
					if (isFlatPeakProcessed && firstOrder[id]<slopeBased && (firstOrder[id]!=0.000000)) 
					{
						for (int s=1; s<=series; ++s)
						{
							if (firstOrder[id+s]>slopeBased)
								break;
							if (firstOrder[id+s]<slopeBased  && (firstOrder[id+s]!=0.000000))
							{
								if (s==series)
								{
									//getFinishPoint = k+1;
									dequeBufSema[k+1].pts = CRealTimeSema::MINUS_FINISH_POINT; //标记负峰终点
									RealTimeSema.push_back(dequeBufSema[k+1]); //找到负峰终点
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
				if (firstOrder[id]<0)
				{
					//getFinishPoint = k;
					dequeBufSema[k+1].pts = CRealTimeSema::MINUS_CONTINUOUS_WAVE; //标记（连续）终点
					RealTimeSema.push_back(dequeBufSema[k+1]); //找到终点
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

			id -= dequeBufSema.size()-dequeId-series-1; //调整下标起始点

			for (unsigned x=0; x<dequeId; ++x)
				dequeBufSema.pop_front();

			hState = HANDLE_ACCOMPLISH;
			break;

		default:
			break;
		}
	}
}

  
void RealTime_RestorePeakData(std::vector<CRealTimeSema> &InputSema, //最初传入的信号量，传出结果
								std::vector<CRealTimeSema> &TotalSema,      //总的信号量
								std::vector<CRealTimeSema> &RealTimeSema,   //起点波峰波谷终点容器
								const int avg,
								const double slopeBased)  
{// step4: 将求出的点还原至元曲线
	const unsigned currentTotalSemaSize = TotalSema.size();
	const unsigned peakSemaSize = RealTimeSema.size();
	static unsigned i=0; //控制起点，波峰，波谷3点下标
	static unsigned p=1; //控制即时还原波峰情况下标 d
	// 即时还原波峰情况
	for ( ; p<peakSemaSize-3 && peakSemaSize>2; p+=3)
	{
		double maxValue = -INT_MAX;
		for (unsigned x = RealTimeSema[p-1].idx; x<RealTimeSema[p].idx+avg && RealTimeSema[p].idx+avg < currentTotalSemaSize; ++x)
		{
			if (maxValue <= TotalSema[x].y)
			{
				maxValue = TotalSema[x].y;
				RealTimeSema[p] = TotalSema[x];
			}
		}
	}
	//等到起点，波峰，波谷3点有了之后再还原
	for ( ; i<peakSemaSize-3 && peakSemaSize>2 /*&& peakSemaSize%3==0*/; i+=3)  //起点
	{
		for (unsigned j=RealTimeSema[i].idx; j<RealTimeSema[i].idx+avg && j<currentTotalSemaSize-1; j++) //先还原起点
		{
			double k=(TotalSema[j+1].y-TotalSema[j].y)/(TotalSema[j+1].x-TotalSema[j].x);
			if (fabs(k)>slopeBased)
			{
				if (j==RealTimeSema[i].idx+avg-1)
				{
					RealTimeSema[i] = TotalSema[j];
					break;
				}
			}
			else
			{
				RealTimeSema[i] = TotalSema[j];
				break;
			}
		}
		for (unsigned j=RealTimeSema[i+2].idx; j<RealTimeSema[i+2].idx+avg && j<currentTotalSemaSize-1; j++) //再还原落点
		{
			double k=(TotalSema[j+1].y-TotalSema[j].y)/(TotalSema[j+1].x-TotalSema[j].x);
			if (fabs(k)>slopeBased)
			{
				if (j==RealTimeSema[i+2].idx+avg-1)
				{
					RealTimeSema[i+2] = TotalSema[j];
					break;
				}
			}
			else
			{
				RealTimeSema[i+2] = TotalSema[j];
				break;
			}
		}

		//最后还原波峰或者波谷点
		double maxValue = -INT_MAX;
		double minValue = INT_MAX;
		//if (Peak[i].Type>=1) //还原波峰
	//	{
			for (unsigned j=RealTimeSema[i].idx; j<RealTimeSema[i+2].idx; ++j)
			{
				if (maxValue <= TotalSema[j].y)
				{
					maxValue = TotalSema[j].y;
					RealTimeSema[i+1] = TotalSema[j];
				}
			}
			//Sleep(2);
		//}
		//else //还原波谷
		//{
		//	for (unsigned j=Peak[i].StartSema.idx; j<Peak[i].FinSema.idx; ++j)
		//	{
		//		if (minValue >= handleSema[j].y)
		//		{
		//			minValue = handleSema[j].y;
		//			Peak[i].PeakSema = handleSema[j];	
		//		}
		//	} 
		//}
	}
}



