#include "StdAfx.h"
#include "Peak.h"
#include <cmath>
#include <algorithm>
#include <deque>
#include "ConfigArgument.h"

const double TAILING = 1.000000; //拖尾因子，判别峰类型是否带/拖尾


VOID CA_GetMinMaxSlope(const std::vector<CSema> &avgData,
	const std::vector<double> &firstOrder,
	double &MinSlope, 
	double &MaxSlope,
	int N)
{
	if (N<1 || static_cast<size_t>(N)>firstOrder.size())  return;

	//初始化斜率最值
	MinSlope = (double)INT_MAX;
	MaxSlope = 0.0;

	//斜率比较缓存
	deque<double> tmpSlope;


	for (size_t i=0; i<firstOrder.size(); i+=N)
	{
		for (int j=0; j<N; ++j)
		{
			if (firstOrder[i]<=0) //小于等于0的斜率不要
				continue;
			tmpSlope.push_back(firstOrder[i]);
		}
		if (tmpSlope.empty()) continue;
		double maxS = tmpSlope[0];
		double minS = tmpSlope[0];
		while (!tmpSlope.empty()) //比较缓存，得到最大值和最小值
		{
			if (tmpSlope[0]>maxS) maxS = tmpSlope[0];
			if (tmpSlope[0]<minS) minS = tmpSlope[0];
			tmpSlope.pop_front();
		}
	
		if(maxS < MinSlope) MinSlope = maxS;
		if(minS > MaxSlope) MaxSlope = minS;
		
	}
}



VOID CA_GetpeakSearch_Series(const std::vector<CSema> &avgData,   //平均化好的信号量
	const std::vector<double> &firstOrder,//平均化好的信号量的斜率
	std::vector<CPeak> &Peak,    //(输出)起点容器
	std::vector<double> &slopeBased, //！门限斜率容器
	std::deque<bool> &hasNagetiveSpike,//!负峰情况容器
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
			for ( ; i<avgDataSize-series && hState==STATELESS; ++i)  
			{
				if (i>=avgDataSize-series-1) 
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
							for (int s=1; s<=series; ++s)
							{
								if (fabs(firstOrder[i+s])<slopeBased[i])
									break;
								if (fabs(firstOrder[i+s])>slopeBased[i])
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
					if (firstOrder[i]>slopeBased[i])
					{
						for (int s=1; s<=series; ++s)
						{
							if (fabs(firstOrder[i+s])<slopeBased[i+s])
								break;
							if (fabs(firstOrder[i+s])>=slopeBased[i+s])
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
						if (firstOrder[j]>slopeBased[j])  //2010.08.25 添加平头峰处理条件
						{	
							processed = true;
							continue;
						}
						if (processed && firstOrder[j]<slopeBased[j] && (firstOrder[j]!=0.000000)) 
						{
							for (int s=1; s<=series; ++s)
							{
								if (firstOrder[j+s]>slopeBased[j+s])
									break;
								if (firstOrder[j+s]<slopeBased[j+s]  && (firstOrder[j+s]!=0.000000))
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
					if (t>0 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
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
				for (k=j; k<avgDataSize-1 && hState==DOWNGOING_WAVE; ++k)
				{
					if (k>=avgDataSize-series-1)
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
							for (int s=1; s<=series; ++s)
							{
								if (fabs(firstOrder[k+s])>slopeBased[k+s])
									break;
								if (fabs(firstOrder[k+s])<slopeBased[k+s] && (firstOrder[k+s]!=0.000000))
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

					if (t>0 && (Peak[t].StartSema.x-Peak[t-1].FinSema.x)==0.000000)
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







//-----------------------------------------------------------------------------
//功能：将获得输入信号量的起点波峰终点还原至元曲线上
//传入参数：
//			1.vector<CSema> &handleSema 元曲线信号量
//			2.avg 平均次数
//			3.std::vector<double> &slopeBased, //！门限斜率容器
//输出参数：
//			1.vector<CPeak> &Peak 还原后信号量的起点波峰终点
//-----------------------------------------------------------------------------
VOID CA_RestorePeakData(const std::vector<CSema> &handleSema, //待还原的信号量
	std::vector<CPeak> &Peak,
	const int avg,
	const std::vector<double> &slopeBased) //！门限斜率容器)  

{// step4: 将求出的点还原至元曲线
	const unsigned handleSemaSize = handleSema.size();
	const unsigned peakSemaSize = Peak.size();

	for (unsigned i=0; i<peakSemaSize; ++i)  //对于每一个峰
	{
		for (unsigned j=Peak[i].StartSema.idx; j<Peak[i].StartSema.idx+avg && j<handleSemaSize-1; j++) //先还原起点
		{
			double k=(handleSema[j+1].y-handleSema[j].y)/(handleSema[j+1].x-handleSema[j].x);
			if (fabs(k)>slopeBased[j])
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
			if (fabs(k)>slopeBased[j])
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
//功能：切线分峰
//传入参数：
//			1.std::vector<CPeak> &Peak 峰信息
//			2.deque<pair<unsigned, unsigned> > &dequeIdx 切线分峰所需的参数
//			
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_TangentDividPeak(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	vector<AnalysisConfigArguments::PeakSepaArg> &PeakSepaArgs)
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
				unsigned sp = Peak[start].StartSema.idx;
				unsigned ep = Peak[fin].FinSema.idx;
				double _k = (handleSema[ep].y-handleSema[sp].y)/(handleSema[ep].x-handleSema[sp].x);
				double _c = handleSema[sp].y- _k*handleSema[sp].x;
				for (unsigned i=Peak[start].PeakSema.idx; i<=Peak[fin].PeakSema.idx; ++i)
				{
					double _y = _k * handleSema[i].x + _c;
					if (Peak[start].Type < 0) //考虑负峰情况
					{
						if ((fabs(_y) < fabs(handleSema[i].y) && _y>0)
						   ||(fabs(_y) > fabs(handleSema[i].y) && _y>=0))
						{
							start++;
							break;
						}
					}
					else
					{
						if ((fabs(_y) > fabs(handleSema[i].y) && _y>0)//连续峰上有任意点落在基线下
							||(fabs(_y) < fabs(handleSema[i].y) && _y<=0))
						{
							start++;
							break;
						}
					}
				}
			}

			Peak[start].FinSema = Peak[fin].FinSema;
			//标记分峰类型//
			for (unsigned i=start; i<=fin; ++i)
				Peak[i].dType = CPeak::PLUSTANGET;
			//////////////

			for ( ; start!=fin+1; ++start)
			{
		
				//校正肩峰的起落点
				unsigned s = Peak[start].StartSema.idx;
				unsigned f = Peak[fin].FinSema.idx;
				//起点校正
				bool isfail = true;
				for (; s<=Peak[start].PeakSema.idx && isfail; ++s)
				{
					double k = (handleSema[f].y-handleSema[s].y)/(handleSema[f].x-handleSema[s].x);
					double c = handleSema[s].y-k*handleSema[s].x;
					for (unsigned m=s; m<=Peak[start].PeakSema.idx; ++m) 
					{
						double _y = k * handleSema[m].x+c;
						if (Peak[start].Type < 0) //考虑负峰情况
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
						Peak[start].StartSema = handleSema[s];
						isfail = false;
					}
				}isfail = true;	//终点校正
				for (; f>=Peak[fin].PeakSema.idx && isfail; --f)
				{
					double k = (handleSema[f].y-handleSema[s].y)/(handleSema[f].x-handleSema[s].x);
					double c = handleSema[f].y-k*handleSema[f].x;
					for (unsigned n=f; n>=Peak[fin].PeakSema.idx; --n)
					{
						double _y = k * handleSema[n].x + c;
						if (Peak[start].Type < 0) //考虑负峰情况
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
						Peak[fin].FinSema = handleSema[f];
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
				unsigned sp = Peak[start].FinSema.idx;
				unsigned ep = Peak[fin].StartSema.idx;
				double _k = (handleSema[ep].y-handleSema[sp].y)/(handleSema[ep].x-handleSema[sp].x);
				double _c = handleSema[sp].y- _k*handleSema[sp].x;
				for (unsigned i=Peak[start].PeakSema.idx; i>=Peak[fin].PeakSema.idx; --i)
				{
					double _y = _k * handleSema[i].x + _c;
					if (Peak[start].Type < 0) //考虑负峰情况
					{
						if ((fabs(_y) < fabs(handleSema[i].y) && _y>0)
							||(fabs(_y) > fabs(handleSema[i].y) && _y>=0))
						{
							//::AfxMessageBox(_T("负峰情况未实现，返回"));
							//return;
							start++;
							break;
						}
					}
					else
					{
						if ((fabs(_y) > fabs(handleSema[i].y) && _y>0)//连续峰上有任意点落在基线下
							||(fabs(_y) < fabs(handleSema[i].y) && _y<=0))
						{
							start--;
							break;
						}
					}
				}
			}

			Peak[start].StartSema = Peak[fin].StartSema;
			//标记分峰类型//
			for (unsigned i=start; i>=fin; --i)
				Peak[i].dType = CPeak::MINUSTANGET;
			//////////////

			for ( ; start!=fin-1; --start)
			{
		
				//校正肩峰的起落点
				unsigned s = Peak[start].FinSema.idx;
				unsigned f = Peak[fin].StartSema.idx;
				//起点校正
				bool isfail = true;
				for (; s>=Peak[start].PeakSema.idx && isfail; --s)
				{
					double k = (handleSema[f].y-handleSema[s].y)/(handleSema[f].x-handleSema[s].x);
					double c = handleSema[s].y-k*handleSema[s].x;
					for (unsigned m=s; m>=Peak[start].PeakSema.idx; --m) 
					{
						double _y = k * handleSema[m].x+c;
						if (Peak[start].Type < 0) //考虑负峰情况
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
						Peak[start].FinSema = handleSema[s];
						isfail = false;
					}
				}isfail = true;	//终点校正
				for (; f<=Peak[fin].PeakSema.idx && isfail; ++f)
				{
					double k = (handleSema[f].y-handleSema[s].y)/(handleSema[f].x-handleSema[s].x);
					double c = handleSema[f].y-k*handleSema[f].x;
					for (unsigned n=f; n<=Peak[fin].PeakSema.idx; ++n)
					{
						double _y = k * handleSema[n].x + c;
						if (Peak[start].Type < 0) //考虑负峰情况
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
						Peak[fin].StartSema = handleSema[f];
						isfail = false;
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
//功能：垂直分峰算法(旧版本)
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 自定义的Peak类型变量
//输出参数：
//			1.vector<CPeak> &Peak 垂直分峰后的波
//-----------------------------------------------------------------------------
VOID CA_DividePeak_legacy(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<AnalysisConfigArguments::PeakSepaArg> &PeakSepaArgs)        
{// 垂直分峰    
	if (PeakSepaArgs.empty())
	{
		return;
	}
	const unsigned PeakSize = Peak.size();
	
	const unsigned ArgSize = PeakSepaArgs.size();
	for (unsigned index = 0; index<ArgSize; ++index)
	{
		if (PeakSepaArgs[index].SepaType == 1) //确定分峰类型是垂直分峰
		{
			unsigned bg = PeakSepaArgs[index].StartPeakNo;
			unsigned ed = PeakSepaArgs[index].FinishPeakNo;
			unsigned bgPeak = PeakSepaArgs[index].StartPeakNo;
			unsigned edPeak = PeakSepaArgs[index].FinishPeakNo;
			bool finish = false;
			bool isPlus = true; //是否正峰
			if (Peak[bg].Type > 0)
			{
				isPlus = true;
			}
			else
			{
				isPlus = false;
			}
			{//测试输入的重叠峰区间内是否全部是正峰或负峰
				unsigned tmp_bg = bg;
				unsigned tmp_ed = ed;
				for (unsigned tmp=tmp_bg; tmp<=tmp_ed; ++tmp)
				{
					if (isPlus)
					{
						if (Peak[tmp].Type < 0)
							ASSERT(TRUE);
					}
					else
					{
						if (Peak[tmp].Type > 0)
							ASSERT(TRUE);
					}
				}
			}

loop:
			while ((ed!=bg) || (!finish))
			{
				if (bg==ed) //这里峰类型有问题，没有考虑负峰
				{
					Peak[bg].Type = abs(Peak[bg].Type)/Peak[bg].Type;
				}

				double k = (Peak[ed].FinSema.y-Peak[bg].StartSema.y)/(Peak[ed].FinSema.x-Peak[bg].StartSema.x);
				double c = Peak[bg].StartSema.y-k*Peak[bg].StartSema.x;
				for (unsigned i=Peak[bg].StartSema.idx; i<Peak[ed].FinSema.idx; ++i)
				{
					double line_y = handleSema[i].x*k+c;
					if (isPlus && (bg!=ed))    //考虑正峰情况
					{
						if ( (line_y>0 && fabs(line_y) > fabs(handleSema[i].y) ) ||
							 (line_y<0 && fabs(line_y) < fabs(handleSema[i].y) ) )
						{
							ed--;
							break; //切线上点大于曲线上的点，抛弃最后一个峰
						}
					}
					if ((!isPlus) && fabs(line_y) > fabs(handleSema[i].y) && (bg!=ed)) //考虑负峰情况
					{
						if ( (line_y<0 && fabs(line_y) > fabs(handleSema[i].y) ) ||
							(line_y>0 && fabs(line_y) < fabs(handleSema[i].y) ) )
						{
							ed--;
							break; //切线上点小于曲线上的点，抛弃最后一个峰
						}
						
					}
					if (i==Peak[ed].FinSema.idx-2 || (bg==ed))
					{
						test_GetDividePeakPoint(Peak, bg, ed);//求当前连续峰的基点,[bg,ed]区间内确认为垂直分峰
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

		}
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
VOID CA_DividePeak(const std::vector<CSema> &OrigPeak, 
	std::vector<CPeak> &Peak,
	vector<AnalysisConfigArguments::PeakSepaArg> &SepaArgs)        
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
		if (Peak[sPNo].Type > 0) isplus = true;
		else isplus = false;
		

	loop: //goto循环
		for ( ; sPNo <= ePNo; --ePNo)
		{
			//起点和终点的连线
			double _k = (Peak[ePNo].FinSema.y-Peak[sPNo].StartSema.y)/(Peak[ePNo].FinSema.x-Peak[sPNo].StartSema.x);
			double _b = Peak[sPNo].StartSema.y - _k*Peak[sPNo].StartSema.x;
			for (unsigned i=Peak[sPNo].StartSema.idx; i<Peak[ePNo].FinSema.idx; ++i)
			{ 
				double Y0 = _k * OrigPeak[i].x + _b; //YO>y在直线上，YO<y在直线下方，=y则在直线上
				if (isplus) //正峰情况
				{
					if ( (Y0>0 && fabs(Y0) > fabs(OrigPeak[i].y) ) || 
						(Y0<0 && fabs(Y0) < fabs(OrigPeak[i].y) ) ) 
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
					if (fabs(Y0) > fabs(OrigPeak[i].y)) 
					{
						if ( (Y0<0 && fabs(Y0) > fabs(OrigPeak[i].y) ) ||
							(Y0>0 && fabs(Y0) < fabs(OrigPeak[i].y) ) )
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
			int typeLab;
			for (unsigned i=s; i<=e; ++i)
			{
				if (abs(Peak[i].Type) != 1)
				{
					typeLab = Peak[i].Type;
					break;
				}
			}
			for (; s<e; ++s)
			{
				Peak[s].dType = CPeak::VERTDIVIDE;
				Peak[s+1].dType = CPeak::VERTDIVIDE;
				Peak[s].Type = typeLab;
				Peak[s+1].Type = typeLab;
				Peak[s].FinSema.y = _k*Peak[s].FinSema.x+_b;
				Peak[s+1].StartSema = Peak[s].FinSema;
			
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


//-----------------------------------------------------------------------------
//功能：禁止积分
//传入参数：
//			1.std::vector<CPeak> &Peak 峰信息
//			2.vector<unsigned> &BanPeakNo 禁止积分峰号
//			
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_BanPeak(std::vector<CPeak> &Peak,
	vector<unsigned> &BanPeakNo)
{
	vector<CPeak> buffPeak;
	const unsigned PeakSize = Peak.size();
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
			buffPeak.push_back(Peak[i]); 
		}
	}

	Peak.swap(buffPeak);
}



//-----------------------------------------------------------------------------
//功能：删峰之前将所有的半峰宽赋值给成员变量m_cArrayHPW
//-----------------------------------------------------------------------------
VOID CA_GetAllHPW(std::vector<CPeak> &Peak, CArray<double> &cArrayHPW)
{
	if (cArrayHPW.GetSize()!=0)
		cArrayHPW.RemoveAll();
	
	const size_t PeakSize = Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		cArrayHPW.Add(Peak[i].HalfPeakWidth);
	}
}


//-----------------------------------------------------------------------------
//功能：分段最小半峰宽删除不要峰
//传入参数：
//			1.std::vector<CPeak> &Peak 峰信息
//			2.vector<double> &MinHalfWidth 每个峰对应分段最小半峰宽
//			
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_EraseByHalfPeakWidth(std::vector<CPeak> &Peak,
	vector<double> &MinHalfWidth)
{
	const unsigned PeakSize = Peak.size();
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

		if (abs(Peak[i].Type)==1) //单峰
		{
			if (Peak[i].HalfPeakWidth - MinHalfWidth[i] >= 0.000000)//大于最小半峰宽
			{
				buffPeak.push_back(Peak[i]);
			}
		}
		else //连续峰
		{
			lastType = Peak[i].Type; //记录连续峰起始类型
			while ( lastType == Peak[i].Type && i<PeakSize) //直到连续峰结束
			{
				if (Peak[i].HalfPeakWidth - MinHalfWidth[i] >= 0.000000) //大于最小半峰宽
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
					buffPeak.push_back(Peak[j]);
				}
				flag = !flag;
			}
			i = FinishMarkPoint;  //重新定位i的下标
		}
	}
	Peak.swap(buffPeak);
}



//-----------------------------------------------------------------------------
//功能：求半峰宽（全部以单峰求半峰宽）
//传入参数：
//			1.std::vector<CSema> &handleSema 元曲线信号量
//			2.std::vector<CPeak> &Peak		 峰信息
//			3.
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_GetHalfPeakWidth(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak)
{
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].StartSema.x;
		double y1 = Peak[i].StartSema.y;
		double x2 = Peak[i].FinSema.x;
		double y2 = Peak[i].FinSema.y;

		double peakWidth = fabs(x2-x1);

		double tana = (y2-y1)/(x2-x1);
		double c = y1 - x1 * tana;

		double py = Peak[i].PeakSema.y; 
		double px = Peak[i].PeakSema.x;
		double ty = px*tana+c;

		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;		

		double diff_y = INT_MAX;
		unsigned idxl, idxr;

			for (unsigned l=Peak[i].StartSema.idx; l<=Peak[i].PeakSema.idx; ++l) //上行，逼近切点
			{
				double ly = handleSema[l].x * tana + _c;
				if (diff_y >= fabs(handleSema[l].y - ly))
				{
					diff_y = fabs(handleSema[l].y - ly);
					idxl = l;
				}
			}
			CSema tmpFront(handleSema[idxl]); //半峰起点
			Peak[i].MidStartSema = tmpFront;
		
		diff_y = INT_MAX;

			for (unsigned r=Peak[i].PeakSema.idx; r<=Peak[i].FinSema.idx; ++r) //下行，逼近切点
			{
				double ry = handleSema[r].x * tana + _c;

				if (diff_y >= fabs(handleSema[r].y - ry))
				{
					diff_y = fabs(handleSema[r].y - ry);
					idxr = r;
				}
			}
			CSema tmpLast(handleSema[idxr]);   //半峰落点
			Peak[i].MidFinSema = tmpLast;
		
		double hk = (Peak[i].MidFinSema.y-Peak[i].MidStartSema.y)/(Peak[i].MidFinSema.x-Peak[i].MidStartSema.x);
		double hc = Peak[i].MidStartSema.y - Peak[i].MidStartSema.x * hk;
		double rey = hx*hk+hc;
		CSema HalfPeak(Peak[i].PeakSema.idx, hx, rey); // 半峰中点
		Peak[i].MidPeakSema = HalfPeak;
	}

	//连接半峰宽直线，与波峰的垂线相交于一点。并求出半峰宽前后长度
	for (unsigned i=0; i<PeakSize; ++i)
	{
		double x1 = Peak[i].MidStartSema.x; 
		double y1 = Peak[i].MidStartSema.y;
		double x2 = Peak[i].MidFinSema.x; 
		double y2 = Peak[i].MidFinSema.y;
		
		double halfPeakWidth = fabs(x2-x1);
		Peak[i].HalfPeakWidth = halfPeakWidth;  //求半峰宽
	}
}


//-----------------------------------------------------------------------------
//功能：手工分峰
//传入参数：
//			1.std::vector<CSema> &handleSema 元曲线信号量
//			2.std::vector<CPeak> &Peak		 峰信息
//			3.const int index,               分峰下标
//			4.const double Spra_t            分峰时间
//输出参数：
//			1.手工分峰是否成功
//			2.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
bool CA_HandPeakSpration(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	const int index,     
	const double Spra_t)
{
	//找到分峰时刻在原曲线上的点
	if (index < 0 || index >= (int)Peak.size())
		return false;
	unsigned SpIndex = INT_MAX; //分峰在原曲线上的点的下标
	double diff = (double)INT_MAX;
	for (unsigned i=Peak[index].StartSema.idx; i<Peak[index].FinSema.idx; ++i)
	{
		if (fabs(handleSema[i].x - Spra_t) <= diff)
		{
			diff = fabs(handleSema[i].x - Spra_t);
			SpIndex = i;
		}
	}

	if (SpIndex==INT_MAX) return false; //未找到下标

	//正峰
	if (Peak[index].Type > 0)
	{
		bool bFindLeft = false;
		bool bFindRight = false;
		CSema leftSema; //左边的峰
		double leftMax = static_cast<double>(-INT_MAX);
		for (unsigned l=SpIndex; l>Peak[index].StartSema.idx; --l)
		{//遍历左边，找到y轴变化趋势。如果全是下降点则寻找失败
			if (handleSema[l].y <= handleSema[l-1].y)
			{
				//leftSema = handleSema[l-1];
				bFindLeft = true;
			}
			if (bFindLeft && fabs(handleSema[l].y) >= leftMax) //有变化趋势后才开始找至高点
			{
				leftMax = fabs(handleSema[l].y);
				leftSema = handleSema[l];
			}
		}

		CSema rightSema; //右边的峰
		double rightMax = static_cast<double>(-INT_MAX);
		for (unsigned r=SpIndex; r<Peak[index].FinSema.idx-1; ++r)
		{
			if (handleSema[r+1].y >= handleSema[r].y)
			{
				//rightSema = handleSema[r+1];
				bFindRight = true;
			}
			if (bFindRight && fabs(handleSema[r].y) >= rightMax)
			{
				rightMax = fabs(handleSema[r].y);
				rightSema = handleSema[r];
			}
		}
		if (bFindLeft && bFindRight) //可以分峰
		{
			//unsigned leftSemaIdx;
			//unsigned rightSemaIdx;
			////判断左峰和右峰哪一个是本来的波峰，则另一个是新的分峰
			//if (leftMax - Peak[index].PeakSema.y = 0.0 ) //左峰是元峰
			//{
			//	leftSemaIdx = 
			//}
			//对手工分峰的时间做校正
			diff = (double)INT_MAX;
			for (unsigned i=leftSema.idx; i<rightSema.idx; ++i)
			{
				if (fabs(handleSema[i].y)<=diff)
				{
					diff = fabs(handleSema[i].y);
					SpIndex = i; //得到正确的分峰时间，原曲线的下标
				}
			}
			//对峰容器的峰序号做校正
			int pType = 1;
			std::vector<CPeak> buffVect;
			for (unsigned i=0; i<(unsigned)index; ++i)
			{	if (Peak[i].Type!=1)
					pType = Peak[i].Type;
				buffVect.push_back(Peak[i]); //前半部分
			}
		
			CPeak addOne(pType+1, handleSema[SpIndex], rightSema, Peak[index].FinSema); //tmp
			Peak[index].FinSema = handleSema[SpIndex];
			Peak[index].PeakSema = leftSema;
			Peak[index].Type = pType+1;
			//把切线分峰直接作为垂直分峰
			double x1 = Peak[index].StartSema.x;
			double y1 = Peak[index].StartSema.y;
			double x2 = addOne.FinSema.x;
			double y2 = addOne.FinSema.y;
			double k = (y2-y1)/(x2-x1);
			double c = y1 - k * x1;
			double old_x = Peak[index].FinSema.x;
			double new_y = old_x * k + c;
			Peak[index].FinSema.y = new_y;
			addOne.StartSema.y = new_y;
			//标记切线分峰为垂直分峰
			Peak[index].dType = CPeak::VERTDIVIDE;
			addOne.dType = CPeak::VERTDIVIDE;

			buffVect.push_back(Peak[index]);
			buffVect.push_back(addOne);

			for (unsigned i=index+1; i<Peak.size(); ++i)
			{
				if (Peak[i].Type!=1)
					Peak[i].Type += 1;
				buffVect.push_back(Peak[i]); //后半部分
			}
			Peak.swap(buffVect);
			return true;
		}
	}
	else //负峰
	{
		bool bFindLeft = false;
		bool bFindRight = false;
		CSema leftSema; //左边的峰
		double leftMax = static_cast<double>(-INT_MAX);
		for (unsigned l=SpIndex; l>Peak[index].StartSema.idx; --l)
		{//遍历左边，找到y轴变化趋势。如果全是下降点则寻找失败
			if (handleSema[l].y >= handleSema[l-1].y)
			{
				//leftSema = handleSema[l-1];
				bFindLeft = true;
			}
			if (bFindLeft && fabs(handleSema[l].y) >= leftMax) //有变化趋势后才开始找至高点
			{
				leftMax = fabs(handleSema[l].y);
				leftSema = handleSema[l];
			}
		}

		CSema rightSema; //右边的峰
		double rightMax = static_cast<double>(-INT_MAX);
		for (unsigned r=SpIndex; r<Peak[index].FinSema.idx-1; ++r)
		{
			if (handleSema[r+1].y <= handleSema[r].y)
			{
				//rightSema = handleSema[r+1];
				bFindRight = true;
			}
		}
		if (bFindLeft && bFindRight) //可以分峰
		{
			//对手工分峰的时间做校正
			diff = static_cast<double>(-INT_MAX);
			for (unsigned i=leftSema.idx; i<rightSema.idx; ++i)
			{
				if (handleSema[i].y>=diff)
				{
					diff = handleSema[i].y;
					SpIndex = i; //得到正确的分峰时间，原曲线的下标
				}
			}
			//对峰容器的峰序号做校正
			int pType = -1;
			std::vector<CPeak> buffVect;
			for (unsigned i=0; i<(unsigned)index; ++i)
			{	
				if (Peak[i].Type!=-1)
					pType = Peak[i].Type;
				buffVect.push_back(Peak[i]); //前半部分
			}

			CPeak addOne(pType-1, handleSema[SpIndex], rightSema, Peak[index].FinSema); //tmp
			Peak[index].FinSema = handleSema[SpIndex];
			Peak[index].PeakSema = leftSema;
			Peak[index].Type = pType-1;
			buffVect.push_back(Peak[index]);
			buffVect.push_back(addOne);

			for (unsigned i=index+1; i<Peak.size(); ++i)
			{
				if (Peak[i].Type!=-1)
					Peak[i].Type -= 1;
				buffVect.push_back(Peak[i]); //后半部分
			}
			Peak.swap(buffVect);
			return true;
		}
	}
	return false;
}







//-----------------------------------------------------------------------------
//功能：手工调峰（起点） 
//传入参数：
//			1.std::vector<CSema> &handleSema 元曲线信号量
//			2.std::vector<CPeak> &Peak		 峰信息
//			3.vector<double> &Arg            手工调峰（起点）参数
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_HandAdjustStart(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<double> &Arg)
{
	const unsigned ArgSize =  Arg.size();
	const unsigned PeakSize = Peak.size();
	const unsigned SemaSize = handleSema.size();
	vector<double> buffVect; //临时容器，最后跟Arg内数据对换
	
	for (unsigned ai=0; ai<ArgSize; ++ai)
	{
		unsigned Ano = UINT_MAX; //待调整的峰号
		double diff = (double)INT_MAX;
		//遍历波峰容器，首先找到最靠近的那个起始点，确定峰号
		for (unsigned i=0; i<PeakSize; ++i)
		{
			double CurrentDiff = fabs(Arg[ai]-Peak[i].StartSema.x);
			if (CurrentDiff - diff == 0.00000000)
			{//几乎不可能
				::AfxMessageBox(_T("调整的点距离前后2个起点长度相同"));
				return;
			}
			if (CurrentDiff < diff)
			{
				diff = CurrentDiff;
				Ano = i;
			}
		}
		//已经找到峰号,对峰号进行判断是否满足调整的条件
		bool clear = false; //是否满足调峰条件
		if (Ano!=UINT_MAX)
		{
			if (Ano == 0)//第一个峰的情况
			{
				if (Peak[Ano].PeakSema.x > Arg[ai])
					clear = true;
			}
			else //第二个峰以后的情况
			{
				if (abs(Peak[Ano].Type)==1) //单峰
				{
					if ((Peak[Ano].PeakSema.x > Arg[ai])
						&&(Peak[Ano-1].FinSema.x <= Arg[ai]))
					{
						clear = true;
					}
				}
				else
				{
					if ((Peak[Ano].PeakSema.x > Arg[ai])
						&&(Peak[Ano-1].PeakSema.x <= Arg[ai]))
					{
						clear = true;
					}
				}
				
			}
		}
		if (clear) //手工调峰可以进行
		{
			unsigned left = 0;
			unsigned right = 0;
			if (Ano == 0)
				right = Peak[Ano].PeakSema.idx;
			else
			{
				left = Peak[Ano-1].PeakSema.idx;
				right = Peak[Ano].PeakSema.idx;
			}
		
			//找到调峰参数对应原曲线上的点
			for (unsigned index = left; index < right; ++index)
			{
				if ((handleSema[index].x <= Arg[ai])
				  &&(handleSema[index+1].x >= Arg[ai]))
				{
					buffVect.push_back(handleSema[index].x); //当前调峰参数正确，且校正
					//手工调峰
					if (abs(Peak[Ano].Type)==1) //单峰
						Peak[Ano].StartSema = handleSema[index];
					else //连续峰
					{
						Peak[Ano].StartSema = handleSema[index]; //起点改变
						Peak[Ano-1].FinSema = Peak[Ano].StartSema; //上一个波峰的终点改变
					}
				}
			}
		}
	}
	Arg.swap(buffVect);
}







//-----------------------------------------------------------------------------
//功能：手工调峰（终点） 
//传入参数：
//			1.std::vector<CSema> &handleSema 元曲线信号量
//			2.std::vector<CPeak> &Peak		 峰信息
//			3.vector<double> &Arg            手工调峰（起点）参数
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_HandAdjustFinish(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<double> &Arg)
{
	const unsigned ArgSize =  Arg.size();
	const unsigned PeakSize = Peak.size();
	const unsigned SemaSize = handleSema.size();
	vector<double> buffVect; //临时容器，最后跟Arg内数据对换

	for (unsigned ai=0; ai<ArgSize; ++ai)
	{
		unsigned Ano = UINT_MAX; //待调整的峰号
		double diff = (double)INT_MAX;
		//遍历波峰容器，首先找到最靠近的那个终止点，并确定峰号
		for (unsigned i=0; i<PeakSize; ++i)
		{
			double CurrentDiff = fabs(Arg[ai]-Peak[i].FinSema.x); 
			if (CurrentDiff - diff == 0.00000000)
			{//几乎不可能
				::AfxMessageBox(_T("调整的点距离前后2个起点长度相同"));
				return;
			}
			if (CurrentDiff < diff)
			{
				diff = CurrentDiff;
				Ano = i;
			}
		}
		//已经找到峰号,对峰号进行判断是否满足调整的条件
		bool clear = false; //是否满足调峰条件
		if (Ano!=UINT_MAX)
		{
			if (Ano == PeakSize-1)//最后一个峰的情况
			{
				if (Peak[Ano].PeakSema.x < Arg[ai])
					clear = true;
			}
			else //第一个峰到最后一个峰之间的情况
			{
				if (abs(Peak[Ano].Type)==1) //单峰
				{
					if ((Peak[Ano].PeakSema.x < Arg[ai])
						&&(Peak[Ano+1].StartSema.x >= Arg[ai]))
					{
						clear = true;
					}
				}
				else
				{
					if ((Peak[Ano].PeakSema.x < Arg[ai])
						&&(Peak[Ano+1].PeakSema.x >= Arg[ai]))
					{
						clear = true;
					}
				}

			}
		}
		if (clear) //手工调峰可以进行
		{
			unsigned left = 0;
			unsigned right = 0;
			if (Ano == PeakSize-1)
			{
				left = Peak[Ano].PeakSema.idx;
				right = Peak[Ano].FinSema.idx;
			}
			else
			{
				left = Peak[Ano].PeakSema.idx;
				right = Peak[Ano+1].PeakSema.idx;
			}

			//找到调峰参数对应原曲线上的点
			for (unsigned index = left; index < right; ++index)
			{
				if ((handleSema[index].x <= Arg[ai])
					&&(handleSema[index+1].x >= Arg[ai]))
				{
					buffVect.push_back(handleSema[index].x); //当前调峰参数正确，且校正
					//手工调峰
					if (abs(Peak[Ano].Type)==1) //单峰
					{
						Peak[Ano].FinSema = handleSema[index];
						break;
					}
					else //连续峰
					{
						Peak[Ano].FinSema = handleSema[index]; //起点改变
						Peak[Ano+1].StartSema = Peak[Ano].FinSema; //上一个波峰的终点改变
						break;
					}
				}
			}
		}
	}
	Arg.swap(buffVect);
}







//-----------------------------------------------------------------------------
//功能：手工调峰（峰谷） 
//传入参数：
//			1.std::vector<CSema> &handleSema 元曲线信号量
//			2.std::vector<CPeak> &Peak		 峰信息
//			3.vector<double> &Arg            手工调峰（峰谷）参数
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_HandAdjustPeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<double> &Arg)
{
	const unsigned ArgSize =  Arg.size();
	const unsigned PeakSize = Peak.size();
	const unsigned SemaSize = handleSema.size();
	//对手工调峰（峰谷）参数进行遍历
	for (unsigned ArgIndex = 0; ArgIndex<ArgSize; ++ArgIndex)
	{
		for (unsigned i=0; i<PeakSize; ++i) //找到需要调整的峰谷位于哪个波内
		{
			if((Arg[ArgIndex] > Peak[i].StartSema.x)
				&&(Arg[ArgIndex] < Peak[i].FinSema.x)) //找到
			{
				//还原至元曲线上
				unsigned left = Peak[i].StartSema.idx;
				unsigned right = Peak[i].FinSema.idx;
				for (unsigned j=left; j<right-1; ++j)
				{
					if ((Arg[ArgIndex] >= handleSema[j].x)
					&&(Arg[ArgIndex] <= handleSema[j+1].x))//还原
					{//是否要精准到具体一个点，对比更相近的那一点
						Peak[i].PeakSema = handleSema[j];
						Arg[ArgIndex] = handleSema[j].x; //校正，手工调峰（峰谷）
					}
				}
			}
		}
	}
}







//-----------------------------------------------------------------------------
//功能：合并峰 
//传入参数：
//			1.std::vector<CSema> &handleSema     元曲线信号量
//			2.std::vector<CPeak> &Peak		     峰信息
//			3.vector<pair<double,double> > &Arg  手工调峰（峰谷）参数
//			5.AnalysisConfigArguments* pCA       配置参数结构指针
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_MergePeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	vector<pair<double,double> > &Arg,
	AnalysisConfigArguments* pCA)
{//以后步骤跟解析合并峰参数有类似，以后需【优化】
	const unsigned ArgsSize = Arg.size();
	const unsigned PeakSize = Peak.size();
	const unsigned SemaSize = handleSema.size();

	for (unsigned ai=0; ai<ArgsSize; ++ai)
	{
		CSema startSema, peakSema, finishSeam;

		double start = Arg[ai].first;   //起点
		double end =   Arg[ai].second;  //终点
		unsigned sNo = INT_MAX; //合并峰起点下标
		unsigned eNo = INT_MAX; //合并峰终点下标
		//对区间起点确定处于第N个峰之前
		for (unsigned pi=0; pi<PeakSize; ++pi)
		{
			if (Peak[pi].PeakSema.x >= start) //判定条件：只要波峰大于起点
			{
				sNo = pi;
				break;
			}
		}
		//对区间终点确定处于第N个峰之后
		for (unsigned pi=PeakSize-1; pi>=0; --pi)
		{
			if (Peak[pi].PeakSema.x <= end)
			{
				eNo = pi;
				break;
			}
		}
		//对合并峰下标判断
		if (sNo!=INT_MAX && eNo!=INT_MAX)
		{
			unsigned diff = eNo - sNo;
			if (diff >= 1)//合并峰参数满足条件，可以合并
			{
				//把起点还原至原曲线
				unsigned left = INT_MAX;  //合并峰起点下标
				unsigned right = INT_MAX; //合并峰终点下标
				unsigned startIndex = 0;
				if (sNo!=0)
					startIndex = Peak[sNo-1].PeakSema.idx;
				for (unsigned si=startIndex; si<SemaSize-1; ++si)
				{
					if ((start>=handleSema[si].x)
					&&(start<=handleSema[si+1].x))
					{
						start = handleSema[si].x; //起点校正
						startSema = handleSema[si];
						left = si;
						break;
					}
				}
				//把终点还原至原曲线
				unsigned endIndex = handleSema[SemaSize-1].idx;
				if (eNo!=PeakSize-1)
					endIndex = Peak[eNo+1].PeakSema.idx;
				for (unsigned si=endIndex; si>0; --si)
				{
					if ((end<=handleSema[si].x)
					&& (end>=handleSema[si-1].x))
					{
						end = handleSema[si].x; //终点校正
						finishSeam = handleSema[si];
						right = si;
						break;
					}
				}
				//找出区间内的最大值作为峰谷
				pair<double,unsigned> maxVal = std::make_pair(static_cast<double>(-INT_MAX),0);
				pair<double,unsigned> minVal = std::make_pair(static_cast<double>( INT_MAX),0);
				//unsigned minusIndex = 0; //记录最小值的下标（负峰）
				double point = 0.0;
				for (unsigned i=left; i<right; ++i)
				{
					if (handleSema[i].y >= maxVal.first)
					{
						maxVal.first = handleSema[i].y; //找到该区间内的最大值
						maxVal.second = i;
						continue;
					}

					if (handleSema[i].y <= minVal.first)
					{
						minVal.first = handleSema[i].y; //找到该区间内的最小值
						minVal.second = i;
						point = handleSema[i].x;
						continue;
					}
				}
				//波峰还是波谷的取舍
				//double realVal = 0.0; //区间内最终确定的峰值
				unsigned realIndex = 0; //区间内最终确定的峰值的下标
				if (maxVal.first > fabs(minVal.first))
				{
					realIndex = maxVal.second;//区间内就是找波峰
				}
				else if (maxVal.first < fabs(minVal.first))
				{
					//判断该最小值是否处于负峰检测区间
					//是，最小值作为波谷
					//否，依然以最大值作为波峰
					if (pCA->GetMinusCheakStatus(point))
						realIndex = minVal.second;
					else
						realIndex = maxVal.second;
				}
				else
				{//几乎不可能
					::AfxMessageBox(_T("合并峰区间内最小值和最大值数值相同"));
					return;
				}



				//对现有的波峰容器进行整理
				vector<CPeak> buffPeak;
				for (unsigned pi=0; pi!=sNo; ++pi)
				{
					buffPeak.push_back(Peak[pi]);
				}
				if (realIndex > SemaSize)
				{
					TRACE(_T("(realIndex > SemaSize)"));
					return;
				}
				
				CPeak peak(1, startSema, handleSema[realIndex], finishSeam);
				buffPeak.push_back(peak);

				for (unsigned pi=eNo+1; pi<PeakSize; ++pi)
				{
					buffPeak.push_back(Peak[pi]);
				}
				Peak.swap(buffPeak);
			}
		}
	}
}


//-----------------------------------------------------------------------------
//功能：增加峰 
//在整个波形曲线内的一段区域，找出最值，形成波峰。3点皆落在曲线上。
//疑问：是否需要对增加峰做基线调整？
//      是否需要标记峰的峰类型，比如手工正峰，手工负峰
//-----------------------------------------------------------------------------
VOID CA_AddPeak(const std::vector<CSema> &handleSema, 
	std::vector<CPeak> &Peak,
	deque<ConfigArgument> &AddArgs)
{
	const unsigned OrigSemaSize = handleSema.size();
	while (!AddArgs.empty())
	{
		//1,找到原曲线上的起始点
		bool bfStart = false;
		CSema StartPoint;
		double starttime = AddArgs[0].BeginTime;
		for (unsigned i=0; i<OrigSemaSize-1; ++i)
		{
			if (handleSema[i].x <= starttime && handleSema[i+1].x >= starttime)
			{
				//starttime = handleSema[i+1].x;
				bfStart = true;
				StartPoint = handleSema[i+1];
				break;
			}
		}

		//2,找到原曲线上的终止点
		bool bfEnd = false;
		CSema EndPoint;
		double endtime = AddArgs[0].EndTime;
		for (unsigned i=0; i<OrigSemaSize-1; ++i)
		{
			if (handleSema[i].x <= endtime && handleSema[i+1].x >= endtime)
			{
				//endtime = handleSema[i];
				bfEnd = true;
				EndPoint = handleSema[i];
				break;
			}
		}

		//3,找元曲线的波峰或波谷
		CSema PeakPoint;
		bool bFindPeak = false;
		double dPeakType = AddArgs[0].InputValue;
		if (bfStart && bfEnd)
		{
			
			if (dPeakType - 1 == 0.000000) //找波峰
			{
				
				for (unsigned i = StartPoint.idx; i<EndPoint.idx; ++i)
				{
					if (handleSema[i].y >= PeakPoint.y)
					{
						PeakPoint = handleSema[i];
						bFindPeak = true;
					}
				}
			}
			else if (dPeakType + 1 == 0.000000)
			//找波谷
			{
				PeakPoint.y = (double)INT_MAX;
				for (unsigned i = StartPoint.idx; i<EndPoint.idx; ++i)
				{
					if (handleSema[i].y <= PeakPoint.y)
					{
						PeakPoint = handleSema[i];
						bFindPeak = true;
					}
				}
			}
		}

		//4,把一组3点完整点加入结果容器
		if (bFindPeak)
		{
			CPeak peak(StartPoint, PeakPoint, EndPoint);
			//总之增加峰总是视为单峰，若时间区间内出血双峰由用户自行负责
			if (dPeakType - 1 == 0.000000)
			{
				peak.Type = 1;
			}
			else if (dPeakType + 1 == 0.000000)
			{
				peak.Type = -1;
			}
			Peak.push_back(peak);

			CA_PeakValleyBaseLineCheck(handleSema, Peak, 
				Peak.size()-1,
				Peak.size()-1); //峰谷基线校正
		}
	

		AddArgs.pop_front();
	}
	//是否需要对结果重新排序？
	
}





VOID CA_NewPeakValleyBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end) 
{
}





VOID CA_PeakValleyBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end) 
{// 峰谷基线校正算法
	if (begin > end)
	{
		ASSERT(_T("(begin > end)"));
		return;
	}

	const unsigned PeakSize = Peak.size();
	unsigned bg = begin;
	unsigned ed = end;
	bool getIndex = false; 

	deque<bool> MinusCheck(PeakSize, false); //分段负峰检测
	for (unsigned i=begin; i<=end; ++i)
	{
		if (Peak[i].Type < 0)
		{
			MinusCheck[i] = true;
		}
	}

	for (; begin<=end; ++begin)
	{  
		bg = begin;
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
				if (Peak[ed].Type!=mark || Peak[ed].dType!=CPeak::PEAKVALLEY)
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
			unsigned j = Peak[i].StartSema.idx;
			do 
			{
				bool isfail = true;
				for (j=Peak[i].StartSema.idx; j<=Peak[i].PeakSema.idx && isfail; ++j)	
				{
					double k = (handleSema[Peak[i].FinSema.idx].y-handleSema[j].y)/(handleSema[Peak[i].FinSema.idx].x-handleSema[j].x);
					double c = handleSema[j].y - k * handleSema[j].x;

					for (unsigned m=j; m<=Peak[i].PeakSema.idx; ++m) 
					{
						double _y = k * handleSema[m].x + c;
						if (MinusCheck[i]) //考虑负峰情况
						{
							if ( (_y <= 0 && (fabs(handleSema[m].y) < fabs(_y))) ||
							   	 (_y > 0  && (fabs(handleSema[m].y) > fabs(_y))) )
							{
								isfail = true;
								break;
							}
						}
						else 
						{
							if ( (_y <= 0 && (fabs(handleSema[m].y) > fabs(_y))) ||
								 (_y > 0  && (fabs(handleSema[m].y) < fabs(_y))) )
							{
								isfail = true;
								break;
							}
						}
						isfail = false;
					}
					if (!isfail)
					{
						Peak[i].StartSema = handleSema[j];
						isfail = false;
					}
				} isfail = true;

				for (unsigned l=Peak[i].FinSema.idx; l>=Peak[i].PeakSema.idx && isfail; --l)
				{
					//double k = (handleSema[Peak[i].FinSema.idx].y-handleSema[j].y)/(handleSema[Peak[i].FinSema.idx].x-handleSema[j].x);
					double k = (handleSema[l].y-handleSema[j].y)/(handleSema[l].x-handleSema[j].x);
					double c = handleSema[l].y-k*handleSema[l].x;
					
					for (unsigned n=l; n>=Peak[i].PeakSema.idx; --n)
					{
						double _y = k * handleSema[n].x + c;
						if (MinusCheck[i]) //考虑负峰情况
						{
							
							if ((handleSema[n].y <= 0 && (fabs(handleSema[n].y) < fabs(_y))) ||
								(handleSema[n].y > 0  && (fabs(handleSema[n].y) > fabs(_y))) )
							{
								isfail = true;
								break;
							}
						}
						else  
						{
							if ( (_y <= 0 && (fabs(handleSema[n].y) > fabs(_y))) ||
								(_y > 0  && (fabs(handleSema[n].y) < fabs(_y))) )
							{
								isfail = true;
								break;
							}
						}
						isfail = false;				
					}
					if (!isfail)
					{
						Peak[i].FinSema = handleSema[l];
						isfail = false;
					}
				}
				i++;	
			} while (i!=o+1);
			getIndex = false;
		}
		begin = ed;
	}
}







//-----------------------------------------------------------------------------
//功能：垂直分峰后基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak		峰容器
//          3.unsigned begin			起始峰编号
//          4.unsigned end				结束峰编号
//输出参数：
//			1.vector<CPeak> &Peak 基线校正后的波
//-----------------------------------------------------------------------------
VOID CA_VertDivideBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end)
{
	//垂直分峰基线校正
	if (begin > end)
	{
		ASSERT(_T("(begin > end)"));
		return;
	}

	const unsigned PeakSize = Peak.size();
	unsigned bg = begin;
	unsigned ed = end;
	bool getIndex = false; 
	deque<bool> MinusCheck(PeakSize, false); //分段负峰检测
	for (unsigned i=begin; i<=end; ++i)
	{
		if (Peak[i].Type < 0)
		{
			MinusCheck[i] = true;
		}
	}
	for (; begin<=end; ++begin)
	{
		bg = begin;

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
		
				if (Peak[ed].Type != mark || Peak[ed].dType != CPeak::VERTDIVIDE)
				{
					ed--;
					break;
				}
				if (ed == PeakSize) //modify if (ed+1 == PeakSize)
				{
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
				if (MinusCheck[bg]) //考虑负峰情况
				{
					//if (fabs(_y) > fabs(handleSema[m].y))
					if ((fabs(_y) > fabs(handleSema[m].y) && _y<=0) ||
						(fabs(_y) < fabs(handleSema[m].y) && _y>0 ))
					{
						isfail = true;
						break;
					}
				}
				else
				{
					if ((fabs(_y) > fabs(handleSema[m].y) && _y>0) ||
						(fabs(_y) < fabs(handleSema[m].y) && _y<=0))
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
				if (MinusCheck[ed]) //考虑负峰情况
				{
					if ((fabs(_y) > fabs(handleSema[n].y) && _y<=0) ||
						(fabs(_y) < fabs(handleSema[n].y) && _y>0 ))
					{
						isfail = true;
						break;
					}
				}
				else
				{
					if ((fabs(_y) > fabs(handleSema[n].y) && _y>0)||
						(fabs(_y) < fabs(handleSema[n].y) && _y<=0))
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
		begin = ed;
	}
}






//-----------------------------------------------------------------------------
//功能：[正]切线分峰后基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak		峰容器
//          3.unsigned begin			起始峰编号
//          4.unsigned end				结束峰编号
//输出参数：
//			1.vector<CPeak> &Peak 基线校正后的波
//-----------------------------------------------------------------------------
VOID CA_PlusTangetBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end)
{
	for (unsigned i=begin; i<=end; ++i)//对给定的区间编号遍历
	{
		for (unsigned j=end; j>=i; --j)
		{
			if (Peak[i].FinSema.x >= Peak[j].StartSema.x ) //找到一组完整的正切分峰
			{
				CA_PeakValleyBaseLineCheck(handleSema, Peak, i+1, j);
				//线段一：
				double x0 = Peak[i].StartSema.x;
				double y0 = Peak[i].StartSema.y;
				double x1 = Peak[i].FinSema.x;
				double y1 = Peak[i].FinSema.y;
				//线段二：
				double x2 = Peak[j].StartSema.x;
				double y2 = Peak[j].StartSema.y;
				double x3 = Peak[j].FinSema.x;
				double y3 = Peak[j].FinSema.y;
				//判断两条线段是否相交（即，主峰跟肩峰是否有重叠）
				if (TwoLineIsIntersect(x0,y0,x1,y1,x2,y2,x3,y3))
				{
					//::AfxMessageBox(_T("线段相交"));
					Peak[i].FinSema = Peak[j].FinSema;
				}
				i = j;
				break;
			}
		}
	}
}



//-----------------------------------------------------------------------------
//功能：[反]切线分峰后基线校正
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak		峰容器
//          3.unsigned begin			起始峰编号
//          4.unsigned end				结束峰编号
//输出参数：
//			1.vector<CPeak> &Peak 基线校正后的波
//-----------------------------------------------------------------------------
VOID CA_MinusTangetBaseLineCheck(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak,
	unsigned begin,
	unsigned end)
{
	for (unsigned i=end; i>begin; --i)//对给定的区间编号遍历
	{
		for (unsigned j=begin; j<=i; ++j)
		{
			if (Peak[j].FinSema.x >= Peak[i].StartSema.x ) //找到一组完整的反切分峰
			{
				CA_PeakValleyBaseLineCheck(handleSema, Peak, j, i-1);
				//线段一：
				double x0 = Peak[i].StartSema.x;
				double y0 = Peak[i].StartSema.y;
				double x1 = Peak[i].FinSema.x;
				double y1 = Peak[i].FinSema.y;

				//线段二：
				double x2 = Peak[j].StartSema.x;
				double y2 = Peak[j].StartSema.y;
				double x3 = Peak[j].FinSema.x;
				double y3 = Peak[j].FinSema.y;
				//判断两条线段是否相交（即，主峰跟肩峰是否有重叠）
				if (TwoLineIsIntersect(x0,y0,x1,y1,x2,y2,x3,y3))
				{
					//::AfxMessageBox(_T("线段相交"));
					Peak[i].StartSema = Peak[j].StartSema;
				}
				i = j;
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
//功能：加入峰谷比参数后自动判断分峰类型，输入分峰处理峰序号容器
//-----------------------------------------------------------------------------

VOID CA_CheckPeakToValley(std::vector<CPeak> &Peak, const double p2vRatio,
	deque<pair<unsigned, unsigned> > &DividePeakId)
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
		double X1 = Peak[bgId].StartSema.x;
		double Y1 = Peak[bgId].StartSema.y;
		double X2 = Peak[edId].FinSema.x;
		double Y2 = Peak[edId].FinSema.y;
		double K = (Y2-Y1)/(X2-X1);
		double C = Y2 - K * X2;
		for (unsigned i=bgId; i<=edId; ++i)
		{
			double XH = Peak[i].PeakSema.x;
			double YH = Peak[i].PeakSema.y;

			double XR = Peak[i].FinSema.x;
			double YR = Peak[i].FinSema.y;

			double H = K * XH + C;
			double R = K * XR + C;

			double PeakH = fabs(fabs(YH)-fabs(H));
			double P2VRatio = fabs(fabs(YR)-fabs(R));

			
			pPEAKh.push_back(PeakH);
			pP2VRatio.push_back(P2VRatio);

			CSema sema(Peak[i].FinSema);
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
				Peak[bgId+i].dType = CPeak::VERTDIVIDE;
				Peak[bgId+i+1].dType = CPeak::VERTDIVIDE;
				Peak[bgId+i].FinSema  = secPoint[i];
				Peak[bgId+i+1].StartSema  = secPoint[i];
			}
			else //峰谷比高于用户指定值：峰谷连线
			{
				Peak[bgId+i].dType = CPeak::PEAKVALLEY;
			}
		}

		DividePeakId.pop_front();
	}

	tmpDeque.swap(DividePeakId);
}



//-----------------------------------------------------------------------------
// 功能：获得基线校正参数
// 传入参数：
//			0.CPeak::DivideType divType             分峰类型
//			1.const std::vector<CPeak> &Peak		峰容器
//          2.pair<unsigned, unsigned> &Arg			垂直分峰基线校正参数
// 输出参数：
//			2.pair<unsigned, unsigned> &Arg	        
//-----------------------------------------------------------------------------
VOID CA_GetBaseLineArgs(CPeak::DivideType divType,
	const std::vector<CPeak> &Peak,
	deque<pair<unsigned, unsigned> > &Arg)
{
	const unsigned ResultSize = Peak.size();
	//先遍历一遍，把符合条件的序号都归入同一容器内
	vector<unsigned> pIndex;
	for (unsigned i=0; i<ResultSize; ++i)
	{
		if (Peak[i].dType == divType)
			pIndex.push_back(i);
	}

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
			pair<unsigned, unsigned> p = std::make_pair(rIndex.front(), rIndex.back());
			Arg.push_back(p);
			rIndex.clear();
			rIndex.push_back(pIndex[i]);
		}
	}
	if (!rIndex.empty())
	{
		pair<unsigned, unsigned> p = std::make_pair(rIndex.front(), rIndex.back());
		Arg.push_back(p);
	}
}


//-----------------------------------------------------------------------------
// 删峰之前将所有的峰面积赋值给成员变量m_cArrayMPA
//-----------------------------------------------------------------------------
VOID CA_GetAllMPA(std::vector<CPeak> &Peak, CArray<double> &cArrayMPA)
{
	if (cArrayMPA.GetSize()!=0)
		cArrayMPA.RemoveAll();

	const size_t PeakSize = Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		cArrayMPA.Add(Peak[i].PeakArea);
	}
}



//-----------------------------------------------------------------------------
// 删峰之前将所有的峰高赋值给成员变量m_cArrayMH
//-----------------------------------------------------------------------------
VOID CA_GetAllMH(std::vector<CPeak> &Peak, CArray<double> &cArrayMH)
{
	cArrayMH.RemoveAll();

	const size_t PeakSize = Peak.size();
	for (size_t i=0; i<PeakSize; ++i)
	{
		cArrayMH.Add(Peak[i].PeakHeight);
	}
}


//-----------------------------------------------------------------------------
//功能：获得峰面积
//传入参数：
//			1.vector<CSema> &handleSema 初始信号量
//			2.vector<CPeak> &Peak 要获得峰面积的波		
//输出参数：    
//			1.vector<CPeak> &Peak 获得峰面积后的波
//-----------------------------------------------------------------------------
VOID CA_GetPeakArea(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak)
{
	const unsigned PeakSize = Peak.size();
	const unsigned SemaSize = handleSema.size();
	//unsigned j = 0;
	for (unsigned i=0; i<PeakSize; ++i)  
	{
		double x2 = Peak[i].FinSema.x;           
		double y2 = Peak[i].FinSema.y;
		double x1 = Peak[i].StartSema.x;
		double y1 = Peak[i].StartSema.y;
		double k = (y2-y1)/(x2-x1);
		double c = y1-k*x1;

		double area = 0.0;
		for (unsigned j = 0; j<SemaSize; ++j)
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
					double front = fabs(fabs(y3)-fabs(x3*k+c));
					double behind = fabs(fabs(y4)-fabs(x4*k+c));
					area += (front+behind)*h/2;
					j++;
				} while (handleSema[j].x-Peak[i].FinSema.x <= 0.000000); //找到终点
				Peak[i].PeakArea = area;
				break;
			}
		}
	}
	//以上针对每个峰求出面积，但对于切线分峰则需要减去重复计算的那一部分
	deque<pair<unsigned, unsigned> > PlusTangetArgs;  //正切分峰
	deque<pair<unsigned, unsigned> > MinusTangetArgs; //反切分峰
	CA_GetBaseLineArgs(CPeak::PLUSTANGET, Peak, PlusTangetArgs);
	while (!PlusTangetArgs.empty())
	{
		deque<pair<unsigned, unsigned> > dequBuff;
		unsigned begin = PlusTangetArgs[0].first;
		unsigned end =  PlusTangetArgs[0].second;
		for (unsigned i = begin; i<=end; ++i)
		{
			if (Peak[i].FinSema.x < Peak[i+1].StartSema.x)
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
				if (Peak[mainPeakNo].PeakArea < Peak[i].PeakArea)
				{
					ASSERT(_T("CPeak::PLUSTANGET (Peak[mainPeakNo].PeakArea < Peak[i].PeakArea)"));
				}
				Peak[mainPeakNo].PeakArea -= Peak[i].PeakArea;
			}
			dequBuff.pop_front();
		}
		PlusTangetArgs.pop_front();
	}

	CA_GetBaseLineArgs(CPeak::MINUSTANGET, Peak, MinusTangetArgs);
	while (!MinusTangetArgs.empty())
	{
		deque<pair<unsigned, unsigned> > dequBuff;
		unsigned begin = MinusTangetArgs[0].second;
		unsigned end =  MinusTangetArgs[0].first;
		for (unsigned i = begin; i>=end; --i)
		{
			if (Peak[i].StartSema.x > Peak[i-1].FinSema.x)
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
			for (unsigned i=subFirstPeakNo; i>=subLastPeakNo; --i)
			{
				if (Peak[mainPeakNo].PeakArea < Peak[i].PeakArea)
				{
					ASSERT(_T("CPeak::MINUSTANGET (Peak[mainPeakNo].PeakArea < Peak[i].PeakArea)"));
				}
				Peak[mainPeakNo].PeakArea -= Peak[i].PeakArea;
			}
			dequBuff.pop_front();
		}
		MinusTangetArgs.pop_front();
	}
}

VOID CA_EraseByPeakArea(std::vector<CPeak> &Peak,
	vector<double> &PeakArea)
{//仅删除峰，未对峰进行类型整理
	vector<CPeak> buffPeak;
	const unsigned PeakSize = Peak.size();
	const unsigned PeakAreaSize = PeakArea.size();
	if (PeakSize!=PeakAreaSize)
	{
		TRACE(_T("(PeakSize!=PeakAreaSize)"));
		ASSERT(TRUE);
	}
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (fabs(Peak[i].PeakArea) > PeakArea[i])
		{
			buffPeak.push_back(Peak[i]);
		}
	}
	Peak.swap(buffPeak);
}

VOID CA_EraseByPeakHeight(std::vector<CPeak> &Peak,
	vector<double> &PeakHeight)
{
	vector<CPeak> buffPeak;
	const unsigned PeakSize = Peak.size();
	const unsigned PeakHeightSize = PeakHeight.size();
	if (PeakSize!=PeakHeightSize) ASSERT(TRUE);
	
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if (fabs(Peak[i].PeakHeight) > PeakHeight[i])
			buffPeak.push_back(Peak[i]);
	}
	Peak.swap(buffPeak);
}







//-----------------------------------------------------------------------------
//功能：求峰的峰面积% 和 峰高%
//传入参数：
//			1.std::vector<CPeak> &Peak		 峰信息
//			
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_GetPeakPercent(std::vector<CPeak> &Peak)
{
	// 求峰的峰面积% 和 峰高%
	const unsigned PeakSize = Peak.size();

	double totPeakArea=0.0, totPeakHeigh=0.0;
	for (unsigned i=0; i<PeakSize; ++i)
	{
		totPeakArea += Peak[i].PeakArea;
		totPeakHeigh += Peak[i].PeakHeight;
	}
	if (totPeakArea==0.0 || totPeakHeigh==0.0)
	{
		ASSERT(totPeakArea==0.0 || totPeakHeigh==0.0);
	}
	for (unsigned i=0; i<PeakSize; ++i)
	{
		Peak[i].PeakAreaPercent = Peak[i].PeakArea / totPeakArea * 100.0f;
		Peak[i].PeakHighPercent = Peak[i].PeakHeight / totPeakHeigh * 100.0f;
	}
}






//-----------------------------------------------------------------------------
//功能：求峰的其他参数（峰高，畸变因子，拖尾因子等）
//传入参数：
//			1.std::vector<CSema> &handleSema 元曲线信号量
//			2.std::vector<CPeak> &Peak		 峰信息
//			3.
//输出参数：
//			1.std::vector<CPeak> &Peak 峰信息被修改
//-----------------------------------------------------------------------------
VOID CA_GetOtherPeakArgs(const std::vector<CSema> &handleSema,
	std::vector<CPeak> &Peak)
{
	const unsigned PeakSize = Peak.size();
	double factor = 1.0; //除数因子
	for (unsigned i=0; i<PeakSize; ++i)
	{
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

		//double PeakWidthLeft =  sqrt((ty/factor-y1/factor)*(ty/factor-y1/factor)+(px-x1)*(px-x1));  
		//double PeakWidthRight = sqrt((y2/factor-ty/factor)*(y2/factor-ty/factor)+(x2-px)*(x2-px));
		double PeakWidthLeft  = fabs(px-x1);
		double PeakWidthRight = fabs(x2-px);
		double lr = PeakWidthLeft/PeakWidthRight;
		Peak[i].Tailing = lr; //拖尾因子

		double hx = px;
		double hy = (py-ty)/2 + ty;
		double _c = hy-hx*tana;		

		double diff_y = INT_MAX;
		unsigned idxl, idxr;

		for (unsigned l=Peak[i].StartSema.idx; l<=Peak[i].PeakSema.idx; ++l) //上行，逼近切点
		{
			double ly = handleSema[l].x * tana + _c;
			if (diff_y >= fabs(handleSema[l].y - ly))
			{
				diff_y = fabs(handleSema[l].y - ly);
				idxl = l;
			}
		}
		CSema tmpFront(handleSema[idxl]); //半峰起点
		Peak[i].MidStartSema = tmpFront;

		diff_y = INT_MAX;

		for (unsigned r=Peak[i].PeakSema.idx; r<=Peak[i].FinSema.idx; ++r) //下行，逼近切点
		{
			double ry = handleSema[r].x * tana + _c;

			if (diff_y >= fabs(handleSema[r].y - ry))
			{
				diff_y = fabs(handleSema[r].y - ry);
				idxr = r;
			}
		}
		CSema tmpLast(handleSema[idxr]);   //半峰落点
		Peak[i].MidFinSema = tmpLast;

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
//功能：各种操作后峰编号影响后的校正
//传入参数：
//			1.vector<CPeak> &Peak 被检查波类型的波
//输出参数：
//			1.vector<CPeak> &Peak 波类型的检查后的波
//-----------------------------------------------------------------------------
VOID CA_PeakTypeChange(std::vector<CPeak> &Peak)
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
			if (Peak[i].FinSema.idx == Peak[i+1].StartSema.idx)
			{
				continue;
			}
			else /*if (Peak[i].FinSema!=Peak[i+1].StartSema)*/
			{
				if (i!=0)
				{
					if (Peak[i].StartSema.idx == Peak[i-1].FinSema.idx)
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







//-----------------------------------------------------------------------------
//功能：判断两条线段是否相交
//传入参数：
//			1.double x0, double y0, double x1, double y1 线段一的两点坐标
//			2.double x2, double y2,	double x3, double y3 线段二的两点坐标
//输出参数：
//			判断结果
//-----------------------------------------------------------------------------
bool TwoLineIsIntersect(double x0, double y0, 
						double x1, double y1, 
						double x2, double y2, 
						double x3, double y3)
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






//-----------------------------------------------------------------------------
//功能：判断峰类型，为了匹配CPeakStandard类中的CPeakType
//-----------------------------------------------------------------------------
VOID CA_SetPeakType(std::vector<CPeak> &Peak)
{
	const size_t PeakSize = Peak.size();
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
		if (Peak[i].Type < 0) Peak[i].PeakType.negative = 1;
		if (abs(Peak[i].Type) == 1) Peak[i].PeakType.single = 1;
		if (Peak[i].dType == CPeak::PEAKVALLEY)   Peak[i].PeakType.overlap_pl = 1; 
		if (Peak[i].dType == CPeak::MINUSTANGET)  Peak[i].PeakType.overlap_ls = 1; 
		if (Peak[i].dType == CPeak::PLUSTANGET)   Peak[i].PeakType.overlap_rs = 1; 
		if (Peak[i].dType == CPeak::VERTDIVIDE)   Peak[i].PeakType.overlap_vs = 1; 
		if (Peak[i].Tailing > TAILING) Peak[i].PeakType.tail = 1;
	}
	//给正切切线分峰的非肩峰打上连续峰标记
	for (size_t i=0; i<PeakSize; ++i)
	{
		if (Peak[i].dType == CPeak::PLUSTANGET)//就是第一个峰为连续峰
		{
			unsigned bg = i;
			double endx = Peak[i].FinSema.x;
			for ( ; i<PeakSize-1; ++i)
			{
				if (endx < Peak[i+1].StartSema.x)
				{
					break;
					i--;
				}
			}
			Peak[bg].PeakType.overlap_pl = 1; 
		} 
	}

	//给//反切情况分峰的非肩峰打上连续峰标记
	for (int i=PeakSize-1; i>=0; --i)
	{
		if (Peak[i].dType == CPeak::MINUSTANGET)
		{
			unsigned bg = i;
			double startx = Peak[i].StartSema.x;
			for ( ; i>0; --i)
			{
				if (startx > Peak[i-1].StartSema.x)
				{
					break;
					i++;
				}
			}
			Peak[bg].PeakType.overlap_pl = 1; 
		} 
	}
}






//-----------------------------------------------------------------------------
//功能：add 获得垂直分峰上的垂线，传入元曲线数据和结果数据，返回垂直分峰连线的结果数组
//-----------------------------------------------------------------------------
VOID CA_GetValleyLine(const CCurveData &curveData, const CPeakList &peakList,
	CArrayPALine &PALineResult)
{
	const int peakSize = peakList.m_cArrayData.GetSize();  //峰结果个数
	const int curveSize = curveData.m_arrayData.GetSize(); //元曲线数据个数

	//判断元曲线数据是否有序
	for (int i=1; i< curveSize; ++i) 
	{
		if (curveData.m_arrayData[i].dX - curveData.m_arrayData[i-1].dX < 0.000000)
		{
			return; //元曲线无序，则返回
		}
	}

	static int j = 0; //遍历元曲线用下标

	//只对峰落点进行判断
	for (int i=0; i<peakSize; ++i)
	{
		double compareX = peakList.m_cArrayData[i].m_dataEnd.dX;
		for ( ; j< curveSize; ++j) //遍历元曲线，对比每一个x数据
		{
			if (curveData.m_arrayData[j].dX - compareX == 0.0000) //找到在原曲线上的点
			{ //开始对比Y轴上数据
				double compareY = peakList.m_cArrayData[i].m_dataEnd.dY;
				if (curveData.m_arrayData[j].dY - compareY != 0.0000) //找到垂直分峰上垂线
				{
					CCurveDataSingle oneSingle(curveData.m_arrayData[j].dX, curveData.m_arrayData[j].dY); 
					CCurveDataSingle twoSingle(compareX, compareY);
					CPALine PALine;
					PALine.m_point[0] = oneSingle;
					PALine.m_point[1] = twoSingle;
					PALineResult.Add(PALine);
				}
			}
		}
	}
}