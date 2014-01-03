#include "StdAfx.h"
#include "ConfigArgument.h"
#include "Peak.h"
#include <math.h>

#define DEFAULTPEAK2VALLEY 500.0f

ConfigArgument::ConfigArgument()
{  }

ConfigArgument::ConfigArgument(CString argname, double begint)
	:ArgName(argname),
	BeginTime(begint),
	EndTime(-1.1f),
	InputValue(0.0f)
{  }

ConfigArgument::ConfigArgument(CString argname, double begint, double endt)
	:ArgName(argname),
	BeginTime(begint),
	EndTime(endt),
	InputValue(0.0f)
{  }

ConfigArgument::ConfigArgument(CString argname, double begint, double endt, double val)
	:ArgName(argname),
	 BeginTime(begint),
	 EndTime(endt),
	 InputValue(val)
{  }

ConfigArgument::ConfigArgument(double begint, double val)
	:ArgName(_T("手工调峰")),
	BeginTime(begint),
	InputValue(val)
{  }

bool ConfigArgument::operator== (const ConfigArgument &rhs)
{
	return (this->ArgName==rhs.ArgName)  
		&& (this->BeginTime - rhs.BeginTime == 0.0000f)
		&& (this->EndTime - rhs.EndTime == 0.0000f) 
		&& (this->InputValue - rhs.InputValue == 0.0000f);
}

AnalysisConfigArguments::PeakSepaArg::PeakSepaArg(unsigned b, unsigned e, int t)
	:StartPeakNo(b),
	FinishPeakNo(e),
	SepaType(t)
{  }

AnalysisConfigArguments::AnalysisConfigArguments()
	:_defaultSlope(0.0000f),
	_defaultHalfWidth(0.0000f),
	_defaultPeakArea(0.0000f),
	_defaultPeakHeight(0.0000f),
	_defaultPeak2Valley(DEFAULTPEAK2VALLEY) //内部定制默认峰谷比，不需要用户输入
{  }


AnalysisConfigArguments::AnalysisConfigArguments(double defaultSlope, double defaultHalfWidth, double defaultPeakArea, double defaultPeakHeight)
	:_defaultSlope(defaultSlope),
	 _defaultHalfWidth(defaultHalfWidth),
	 _defaultPeakArea(defaultPeakArea),
	 _defaultPeakHeight(defaultPeakHeight),
	 _defaultPeak2Valley(DEFAULTPEAK2VALLEY) //内部定制默认峰谷比，不需要用户输入
{

}

void AnalysisConfigArguments::TransformArgs(deque<ConfigArgument>& args) {
	/* 
	最小半峰宽(分段) --> 分段最小半峰宽;
	斜率(分段)		--> 分段噪声阀门; 
	最小峰面积(分段) --> 分段最小峰面积; 
	最小峰高(分段)	-->  
	禁止积分         --> 禁止积分
	负峰检测			--> 分段负峰检测;
	峰谷基线			--> 重叠峰处理; 峰谷比;
	切线分峰value>=1.0为前切，否则为后切 --> 重叠峰处理;
	合并峰			--> 合并峰;
	分裂峰			--> 手工分峰;
	手工基线			--> 手工调峰;
	手工峰顶			--> 手工调峰;
	添加峰			--> 增加峰;
	*/
	deque<ConfigArgument> tmp;
	size_t argsSize = args.size();
	for (size_t i=0; i<argsSize; ++i) {
		if (args[i].ArgName == _T("最小半峰宽(分段)"))
			args[i].ArgName = _T("分段最小半峰宽");

		else if (args[i].ArgName == _T("斜率(分段)"))
			args[i].ArgName = _T("分段噪声阀门");

		else if (args[i].ArgName == _T("最小峰面积(分段)"))
			args[i].ArgName = _T("分段最小峰面积");

		else if (args[i].ArgName == _T("最小峰高(分段)"))
			args[i].ArgName = _T("分段最小峰高");

		else if (args[i].ArgName == _T("负峰检测"))
			args[i].ArgName = _T("分段负峰检测");

		else if (args[i].ArgName == _T("峰谷基线")) {
			args[i].ArgName = _T("重叠峰处理");
			args[i].InputValue = 0.0;
		}
		else if (args[i].ArgName == _T("切线分峰")) {
			args[i].ArgName = _T("重叠峰处理");
			if(args[i].InputValue>=1.0) args[i].InputValue = 2.0;
			else args[i].InputValue = 3.0;
		}
		else if (args[i].ArgName == _T("分裂峰"))
			args[i].ArgName = _T("手工分峰");

		else if (args[i].ArgName == _T("手工峰顶")){
			args[i].ArgName = _T("手工调峰");
			args[i].InputValue = 22;
		}
		else if (args[i].ArgName == _T("添加峰"))
			args[i].ArgName = _T("增加峰");
		else{// do nothing
		}
	}
}

void AnalysisConfigArguments::SetArgs(const deque<ConfigArgument>& args)
{
	if (!_ConfigArgument.empty())
		_ConfigArgument.clear();
	_ConfigArgument.insert(_ConfigArgument.end(),
							args.begin(),
							args.end());
}

//---------------------------------------------------------------------------------
// 设置默认参数
//---------------------------------------------------------------------------------
void AnalysisConfigArguments::SetPeakSearchArgs(int size, double defaultSlope)
{
	std::vector<double> vectBuff(size, defaultSlope);
	_SlopeBased.swap(vectBuff);
}

void AnalysisConfigArguments::SetMinusCheckArgs(int size)
{
	std::deque<bool> dequeBuff(size, false);
	_MinusCheck.swap(dequeBuff);
}

void AnalysisConfigArguments::SetMinPeakWidthArgs(int size, double defaultVal)
{
	std::vector<double> vectBuff(size, defaultVal);
	_MinHalfWidth.swap(vectBuff);
}

void AnalysisConfigArguments::SetMinPeakAreaArgs(int size, double defaultVal)
{
	std::vector<double> vectBuff(size, defaultVal);
	_MinPeakArea.swap(vectBuff);
}

void AnalysisConfigArguments::SetMinPeakHeightArgs(int size, double defaultVal){
	std::vector<double> vectBuff(size, defaultVal);
	_MinPeakHeight.swap(vectBuff);
}

void AnalysisConfigArguments::addArgs(const ConfigArgument& configArg) 
{
	_ConfigArgument.push_back(configArg);
}

//---------------------------------------------------------------------------------
// 配置参数预处理
//---------------------------------------------------------------------------------
void AnalysisConfigArguments::Pre_PeakSearchSlopeBased()
{
	//噪声阀门整理成区间值
	//------------------------------------------------------------------
	//---噪声阀门处理原则: 
	//---假设有A和B段区间，A区阀门为5000，B区阀门为8000且两区间重叠了。
	//---则按：完整A区间5000，剩下部分为B区间8000处理。即重叠区间全部算A区间的5000。
	//------------------------------------------------------------------
	const unsigned ArgsSize = _ConfigArgument.size();
	double lastTime = static_cast<double>(-INT_MAX);
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段噪声阀门"))
		{
			if (_ConfigArgument[i].BeginTime > lastTime) //区间无重叠
			{
				continue;
			}
			else //区间重叠
			{
				_ConfigArgument[i].BeginTime = lastTime;
			}
			lastTime = _ConfigArgument[i].EndTime;
		}
	}
	//Sleep(2);
}

void AnalysisConfigArguments::Pre_PeakSearchMinusCheck()
{
	//分段负峰检测整理成区间值
	//------------------------------------------------------------------
	//---分段负峰检测处理原则: 
	//---假设有A和B段区间，A区阀门为5000，B区阀门为8000且两区间重叠了。
	//---则按：完整A区间5000，剩下部分为B区间8000处理。即重叠区间全部算A区间的5000。
	//------------------------------------------------------------------
	const unsigned ArgsSize = _ConfigArgument.size();
	double lastTime = static_cast<double>(-INT_MAX);
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段负峰检测"))
		{
			if (_ConfigArgument[i].BeginTime > lastTime) //区间无重叠
			{
				continue;
			}
			else //区间重叠
			{
				_ConfigArgument[i].BeginTime = lastTime;
			}
			lastTime = _ConfigArgument[i].EndTime;
		}
	}
	//Sleep(2);
}

void AnalysisConfigArguments::Pre_HalfPeakWidth()
{
	//分段最小半峰宽 整理成区间值
	//------------------------------------------------------------------
	//---分段最小半峰宽 处理原则: 
	//---假设有A和B段区间，A区最小半峰宽5000，B区最小半峰宽为8000且两区间重叠了。
	//---则按：完整A区间5000，剩下部分为B区间8000处理。即重叠区间全部算A区间的5000。
	//------------------------------------------------------------------
	const unsigned ArgsSize = _ConfigArgument.size();
	double lastTime = static_cast<double>(-INT_MAX);
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段最小半峰宽"))
		{
			if (_ConfigArgument[i].BeginTime > lastTime) //区间无重叠
			{
				//continue;
			}
			else //区间重叠
			{
				_ConfigArgument[i].BeginTime = lastTime;
			}
			//增加正确的输出配置参数
			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);
			lastTime = _ConfigArgument[i].EndTime;
		}
	}
	//Sleep(2);
}

void AnalysisConfigArguments::Pre_PeakSeparation()
{
	//重叠峰处理 整理成区间值
	//------------------------------------------------------------------
	//---重叠峰处理 处理原则: 
	//---A,B区域，重叠区域都算A的 #B从A结束处开始
	//------------------------------------------------------------------

	const unsigned ArgsSize = _ConfigArgument.size();

	deque<ConfigArgument> deque_buff;

	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName != _T("重叠峰处理")){
			deque_buff.push_back(_ConfigArgument[i]);
			continue;
		}
		

		bool isOverlap = false;
		double start_time = _ConfigArgument[i].BeginTime;
		double end_time = _ConfigArgument[i].EndTime;
		// 121204 后面的峰区间不能重叠之前的区间
		for (unsigned j=0; j<i; ++j)
		{
			if (_ConfigArgument[j].ArgName == _T("重叠峰处理")){

				double prev_start_time = _ConfigArgument[j].BeginTime;
				double prev_end_time = _ConfigArgument[j].BeginTime;

				if ((start_time > prev_start_time && start_time < prev_end_time) 
					||(end_time > prev_start_time && end_time < prev_end_time) )
				{ 
					isOverlap = true;
					break;
				}
			}
		}
		if (!isOverlap)
		{
			//增加正确的输出配置参数
			deque_buff.push_back(_ConfigArgument[i]);

			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);
		}
	}

	deque_buff.swap(_ConfigArgument);

}

void AnalysisConfigArguments::Pre_Peak2ValleyRatio()
{
	if (_ConfigArgument.empty()) return;
	const unsigned ArgsSize = _ConfigArgument.size();
	unsigned ResultInx = 0;
	bool bSucceed = false;
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("峰谷比"))
		{
			_defaultPeak2Valley = _ConfigArgument[i].InputValue;
			ResultInx = i;
			bSucceed = true;
		}
	}
	//增加正确的输出配置参数
	if (bSucceed)
	{
		ConfigArgument configArg(_ConfigArgument[ResultInx].ArgName, _ConfigArgument[ResultInx].BeginTime, 
			_ConfigArgument[ResultInx].EndTime,_ConfigArgument[ResultInx].InputValue);
		_ResultArgument.push_back(configArg);
	}
}

void AnalysisConfigArguments::PreprocessConfigArgs()
{
	
	Pre_PeakSearchSlopeBased(); //噪声阀门 整理成区间值
	Pre_PeakSearchMinusCheck(); //分段负峰检测 整理成区间值
	Pre_HalfPeakWidth();        //分段最小半峰宽 整理成区间值
	Pre_PeakSeparation();       //重叠峰处理 整理成区间值

	Pre_Peak2ValleyRatio(); // 峰谷比
}


//---------------------------------------------------------------------------------
// 解析所需要的参数
//---------------------------------------------------------------------------------
VOID AnalysisConfigArguments::Analysis_PeakSearchSlopeBased
	(const vector<CSema> &Data)
{
	deque<ConfigArgument> dequeBuffArgs;
	deque<double> dequeBuff;
	//传入的参数已经排序
	//遍历现有的参数，找到“分段噪声阀门”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned DataSize = Data.size();

	double lastTime = -INT_MAX;
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段噪声阀门"))
		{//重叠区域的处理
			if (_ConfigArgument[i].BeginTime - lastTime >= 0.000000)
			{
				dequeBuff.push_back(_ConfigArgument[i].BeginTime);
				dequeBuff.push_back(_ConfigArgument[i].InputValue);
				dequeBuff.push_back(_ConfigArgument[i].EndTime);
				//增加正确的输出配置参数
				ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
					_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
				_ResultArgument.push_back(configArg);
			}
			lastTime = _ConfigArgument[i].EndTime;
		}
	}


	if (dequeBuff.empty())
		TRACE(_T("dequeBuff.empty()"));	

	//给分段噪声阀门赋值
	for (unsigned i=0; i<DataSize && !dequeBuff.empty(); ++i)
	{
		if (Data[i].x - dequeBuff[0] >= 0.000000)
		{
			_SlopeBased[i]=dequeBuff[1];
		}
		if (Data[i].x - dequeBuff[2] > 0.000000)
		{
			dequeBuff.pop_front();
			dequeBuff.pop_front();
			dequeBuff.pop_front();
		}
	}
}

//解析寻峰需要的负峰检测
VOID AnalysisConfigArguments::Analysis_PeakSearchMinusCheck
	(const vector<CSema> &Data)
{
	deque<ConfigArgument> dequeBuffArgs;
	deque<double> dequeBuff;
	//传入的参数已经排序
	//遍历现有的参数，找到“分段负峰检测”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned DataSize = Data.size();

	double lastTime = -INT_MAX;
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段负峰检测"))
		{//重叠区域的处理
			if (_ConfigArgument[i].BeginTime - lastTime >= 0.000000 )
			{
				dequeBuff.push_back(_ConfigArgument[i].BeginTime);
				dequeBuff.push_back(_ConfigArgument[i].EndTime);
				//增加正确的输出配置参数
				ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
					_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
				_ResultArgument.push_back(configArg);
			}
			lastTime = _ConfigArgument[i].EndTime;
		}
	}


	if (dequeBuff.empty())
		TRACE(_T("dequeBuff.empty()"));	

	//给分段负峰检测赋值
	for (unsigned i=0; i<DataSize && !dequeBuff.empty(); ++i)
	{
		if (Data[i].x - dequeBuff[0] >= 0.000000)
		{
			_MinusCheck[i] = true; 
		}
		if (Data[i].x - dequeBuff[1] > 0.000000)
		{
			dequeBuff.pop_front();
			dequeBuff.pop_front();
		}
	}
	//Sleep(2);
}

void AnalysisConfigArguments::Analysis_TangentDividPeak
	(const vector<CPeak> &Peak, deque<PeakSepaArg> &dequeIdx)
{	//假设传入的参数已经排序（即，按信号处理顺序且同名称的放一起）
	//遍历现有的参数，找到“切线分峰”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();

	deque<PeakSepaArg> dequeBuffIdx;
	//时间上的筛选
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		unsigned BgIdx = 0;
		unsigned EdIdx = 0;
		bool findBgIdx = false;
		bool findEdIdx = false;
		if (_ConfigArgument[i].ArgName == _T("切线分峰"))
		{
			for (unsigned spoint=0; spoint<PeakSize; ++spoint)//找起点
			{
				if (Peak[spoint].PeakSema.x - _ConfigArgument[i].BeginTime >= 0.000000)
				{
					BgIdx = spoint;
					if (dequeBuffIdx.empty())
					{
						findBgIdx = true;
						break;
					}
					else if (!dequeBuffIdx.empty() && BgIdx > dequeBuffIdx[dequeBuffIdx.size()-1].FinishPeakNo)
					{
						findBgIdx = true;
						break;
					}
				}
			}
			for (int epoint=PeakSize-1; epoint>=0; --epoint)//找终点
			{
				if (Peak[epoint].PeakSema.x - _ConfigArgument[i].EndTime <= 0.000000)
				{
					EdIdx = epoint;
					if (EdIdx!=BgIdx) //不对单一波切线分峰
						findEdIdx = true;
					break;
				}
			}
	
		}
		if (findBgIdx && findEdIdx)
		{
			//std::pair<unsigned, unsigned> p = std::make_pair(BgIdx, EdIdx);
			AnalysisConfigArguments::PeakSepaArg tangConfig(BgIdx, EdIdx, (int)_ConfigArgument[i].InputValue);
			dequeBuffIdx.push_back(tangConfig);
		}
	}

	
	//峰类型的筛选
	for (unsigned nowsize=0; nowsize<dequeBuffIdx.size(); ++nowsize)
	{
		int val = dequeBuffIdx[nowsize].SepaType;
		bool findBg = false;
		bool findEd = false;
		unsigned bg = 0;
		unsigned ed = 0;
		int pType = 0; //峰类型
		
		for (; dequeBuffIdx[nowsize].StartPeakNo!=dequeBuffIdx[nowsize].FinishPeakNo+1;
			dequeBuffIdx[nowsize].StartPeakNo++)
		{
			if (abs(Peak[dequeBuffIdx[nowsize].StartPeakNo].Type)==1)//单峰
			{
				if (findBg)
				{
					ed = dequeBuffIdx[nowsize].StartPeakNo-1;
					//std::pair<unsigned, unsigned> p = std::make_pair(bg, ed);
					AnalysisConfigArguments::PeakSepaArg tangConfig(bg, ed, val);
					dequeIdx.push_back(tangConfig);
					findBg = false;
				}
			}
			else //连续峰
			{
				if (findBg && (Peak[dequeBuffIdx[nowsize].StartPeakNo].Type != pType))
				{
					ed = dequeBuffIdx[nowsize].StartPeakNo-1;
					//std::pair<unsigned, unsigned> p = std::make_pair(bg, ed);
					AnalysisConfigArguments::PeakSepaArg tangConfig(bg, ed, val);
					dequeIdx.push_back(tangConfig);
					pType = Peak[dequeBuffIdx[nowsize].StartPeakNo].Type;
					findBg = false;	
				}
				if (!findBg)
				{
					bg = dequeBuffIdx[nowsize].StartPeakNo;
					pType = Peak[bg].Type;
					findBg = true;

				}
				if ((dequeBuffIdx[nowsize].StartPeakNo==dequeBuffIdx[nowsize].FinishPeakNo)
					&& (bg!=dequeBuffIdx[nowsize].FinishPeakNo)) //收尾但排除单峰
				{
					AnalysisConfigArguments::PeakSepaArg tangConfig(bg, dequeBuffIdx[nowsize].FinishPeakNo, val);
					dequeIdx.push_back(tangConfig);
				}
			}
		}
	}
	//Sleep(2);
}

VOID AnalysisConfigArguments::Analysis_PeakSeparation
	(const vector<CPeak> &Peak, vector<PeakSepaArg> &psArgs)
{//配置参数已经做过预处理，全部区间化。不存在有重叠区域
	if (!psArgs.empty()) psArgs.clear();

	//默认全部峰是垂直分峰
	//ConfigArgument argument(_T("重叠峰处理"),0.0, (double)INT_MAX,1);
	//_ConfigArgument.push_back(argument);

	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();

	deque<PeakSepaArg> dequeBuffIdx;
	//时间上的筛选
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		unsigned BgIdx = 0;
		unsigned EdIdx = 0;
		bool findBgIdx = false;
		bool findEdIdx = false;
		if (_ConfigArgument[i].ArgName == _T("重叠峰处理"))
		{
			for (unsigned spoint=0; spoint<PeakSize; ++spoint)//找起点
			{
				if (Peak[spoint].PeakSema.x - _ConfigArgument[i].BeginTime >= 0.000000)
				{
					BgIdx = spoint;
					if (dequeBuffIdx.empty())
					{
						findBgIdx = true;
						break;
					}
					else if (!dequeBuffIdx.empty() && BgIdx > dequeBuffIdx[dequeBuffIdx.size()-1].FinishPeakNo)
					{
						findBgIdx = true;
						break;
					}
				}
			}
			for (int epoint=PeakSize-1; epoint>=0; --epoint)//找终点
			{
				if (Peak[epoint].PeakSema.x - _ConfigArgument[i].EndTime <= 0.000000)
				{
					EdIdx = epoint;
					if (EdIdx!=BgIdx) //不对单一波切线分峰
						findEdIdx = true;
					break;
				}
			}

		}
		if (findBgIdx && findEdIdx)
		{
			AnalysisConfigArguments::PeakSepaArg tangConfig(BgIdx, EdIdx, (int)_ConfigArgument[i].InputValue);
			dequeBuffIdx.push_back(tangConfig);
		}
	}


	//峰类型的筛选
	for (unsigned nowsize=0; nowsize<dequeBuffIdx.size(); ++nowsize)
	{
		int val = dequeBuffIdx[nowsize].SepaType;
		bool findBg = false;
		bool findEd = false;
		unsigned bg = 0;
		unsigned ed = 0;
		int pType = 0; //峰类型

		for (; dequeBuffIdx[nowsize].StartPeakNo!=dequeBuffIdx[nowsize].FinishPeakNo+1;
			dequeBuffIdx[nowsize].StartPeakNo++)
		{
			if (abs(Peak[dequeBuffIdx[nowsize].StartPeakNo].Type)==1)//单峰
			{
				if (findBg)
				{
					ed = dequeBuffIdx[nowsize].StartPeakNo-1;
					//std::pair<unsigned, unsigned> p = std::make_pair(bg, ed);
					AnalysisConfigArguments::PeakSepaArg tangConfig(bg, ed, val);
					psArgs.push_back(tangConfig);
					findBg = false;
				}
			}
			else //连续峰
			{
				if (findBg && (Peak[dequeBuffIdx[nowsize].StartPeakNo].Type != pType))
				{
					ed = dequeBuffIdx[nowsize].StartPeakNo-1;
					//std::pair<unsigned, unsigned> p = std::make_pair(bg, ed);
					AnalysisConfigArguments::PeakSepaArg tangConfig(bg, ed, val);
					psArgs.push_back(tangConfig);
					pType = Peak[dequeBuffIdx[nowsize].StartPeakNo].Type;
					findBg = false;	
				}
				if (!findBg)
				{
					bg = dequeBuffIdx[nowsize].StartPeakNo;
					pType = Peak[bg].Type;
					findBg = true;
				}
				if ((dequeBuffIdx[nowsize].StartPeakNo==dequeBuffIdx[nowsize].FinishPeakNo)
					&& (bg!=dequeBuffIdx[nowsize].FinishPeakNo)) //收尾但排除单峰
				{
					AnalysisConfigArguments::PeakSepaArg tangConfig(bg, dequeBuffIdx[nowsize].FinishPeakNo, val);
					psArgs.push_back(tangConfig);
				}
			}
		}
	}
}

//解析手工分峰的峰号
VOID AnalysisConfigArguments::Analysis_HandPeakSeparation
	(const vector<CPeak> &Peak, deque<double> &HandPeakArgs)
{//当前的手工分峰[起始时间]必须在一个完整峰的区间内
 //对满足条件的手工分峰[起始时间]加入一个容器内
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("手工分峰"))
		{
			//增加正确的输出配置参数
			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);

			for (unsigned j=0; j<PeakSize; ++j)
			{//分峰点必须位于一个完整峰的区间内
				if ((Peak[j].FinSema.x - _ConfigArgument[i].BeginTime > 0.000000)
				  &&(_ConfigArgument[i].BeginTime - Peak[j].StartSema.x > 0.000000))
				{//直接放入容器，未对时间排序
					_HandPeakSpra.push_back(_ConfigArgument[i].BeginTime);
				}
			}
		}
	}
	HandPeakArgs.insert(HandPeakArgs.end(), _HandPeakSpra.begin(), _HandPeakSpra.end());
}

//解析手工分峰需要的一个参数，返回峰容器内对应的峰下标。-1表示找不到符合条件的峰下标
int  AnalysisConfigArguments::Analysis_OneHandPeakSepar
	(const vector<CPeak> &Peak, double starttime)
{
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<PeakSize; ++i)
	{
		if ((Peak[i].FinSema.x - starttime > 0.000000)
			&&(starttime - Peak[i].StartSema.x > 0.000000))
		{
			return (int)i; //找到对应下标
		}
	}
	return -1; //not found
}


VOID AnalysisConfigArguments::Analysis_HandAdjustPeak
	(const vector<CPeak> &Peak)
{//当前的手工调峰[调峰时间]必须在一个完整峰的区间内
	//对满足条件的手工调峰[调峰时间]分别加入3个容器内
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("手工调峰"))
		{

			if (_ConfigArgument[i].InputValue == 11) //调峰类型:起点
			{
				//_AdjustStart.push_back(_ConfigArgument[i].BeginTime);
			}
			else if (_ConfigArgument[i].InputValue == 33) //调峰类型:终点
			{
				//_AdjustFinish.push_back(_ConfigArgument[i].BeginTime);
			}
			else if (_ConfigArgument[i].InputValue == 22) //调峰类型:峰谷
			{//遍历峰容器，判断该调整时刻是否处于某个峰的区间内
				for (unsigned j=0; j<PeakSize; ++j)
				{
					if((_ConfigArgument[i].BeginTime > Peak[j].StartSema.x)
					&&(_ConfigArgument[i].BeginTime < Peak[j].FinSema.x))
					{
						_AdjustPeak.push_back(_ConfigArgument[i].BeginTime);
						break;
					}
				}
			}
			//增加正确的输出配置参数
			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);
		}	
	}
}

VOID AnalysisConfigArguments::Analysis_HandworkBaseLine
	(const vector<CPeak> &Peak) {

	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("手工基线"))
		{
			double matchingTime = _ConfigArgument[i].BeginTime;
			double targetTime = _ConfigArgument[i].EndTime;
			//找跟matchtime最接近的一个点
			double diff = (double)INT_MAX;
			unsigned srt_no = 0; // 确定调整的峰号-起点
			unsigned fin_no = 0; // 确定调整的峰号-终点
			bool isStart = false;
			for (unsigned j=0; j<PeakSize; ++j)
			{
				double sx = Peak[j].StartSema.x;
				double fx = Peak[j].FinSema.x;

				if (fabs(matchingTime - sx)<=diff)
				{
					diff = fabs(matchingTime - sx);
					srt_no = j;
					isStart = true;
				}
				if (fabs(matchingTime - fx)<=diff)
				{
					diff = fabs(matchingTime - fx);
					fin_no = j;
					isStart = false;
				}
			}
			if (isStart)
				_AdjustStart.push_back(std::make_pair(srt_no, targetTime));
			else
				_AdjustFinish.push_back(std::make_pair(fin_no, targetTime));
		}
	}
}


//VOID AnalysisConfigArguments::Analysis_MergePeak
//	(const vector<CPeak> &Peak)
//{//合并峰参数的判定条件：
// //区间内必然存在二或者二个以上的峰
// //满足条件放入成员变量容器内
// //不满足则舍去
//	const unsigned ArgsSize = _ConfigArgument.size();
//	const unsigned PeakSize = Peak.size();
//	for (unsigned ai=0; ai<ArgsSize; ++ai)
//	{
//		if (_ConfigArgument[ai].ArgName == "合并峰")
//		{
//			double start = _ConfigArgument[ai].BeginTime; //起点
//			double end = _ConfigArgument[ai].EndTime;     //终点
//			unsigned sNo = INT_MAX; //合并峰起点下标
//			unsigned eNo = INT_MAX; //合并峰终点下标
//			//对区间起点确定处于第N个峰之前
//			for (unsigned pi=0; pi<PeakSize; ++pi)
//			{
//				if (Peak[pi].PeakSema.x >= start) //判定条件：只要波峰大于起点
//				{
//					sNo = pi;
//					break;
//				}
//			}
//			//对区间终点确定处于第N个峰之后
//			for (signed pi=PeakSize-1; pi>=0; --pi)
//			{
//				if (Peak[pi].PeakSema.x <= end)
//				{
//					eNo = pi;
//					break;
//				}
//			}
//			//对合并峰下标判断
//			if (sNo!=INT_MAX && eNo!=INT_MAX)
//			{
//				unsigned diff = eNo - sNo;
//				if (diff >= 1)//合并峰参数满足条件，可以合并
//				{
//					//增加正确的输出配置参数
//					ConfigArgument configArg(_ConfigArgument[ai].ArgName, _ConfigArgument[ai].BeginTime, 
//						_ConfigArgument[ai].EndTime,_ConfigArgument[ai].InputValue);
//					_ResultArgument.push_back(configArg);
//
//					pair<double, double> buffPair = std::make_pair(start, end);
//					_MergePeak.push_back(buffPair);
//				}
//			}
//		}
//	}
//}

VOID AnalysisConfigArguments::Analysis_MergePeak
	(const vector<CPeak> &Peak)
{//合并峰参数的判定条件：
	//区间内必然存在二或者二个以上的峰
	//如果（开始时间，结束时间）内包含了正峰和负峰，“不能将正峰和负峰进行合并”，忽略该积分事件；
	//满足条件放入成员变量容器内
	//不满足则舍去
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	for (unsigned ai=0; ai<ArgsSize; ++ai)
	{
		if (_ConfigArgument[ai].ArgName == _T("合并峰"))
		{
			double start = _ConfigArgument[ai].BeginTime; //起点
			double end = _ConfigArgument[ai].EndTime;     //终点
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
			for (signed pi=PeakSize-1; pi>=0; --pi)
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
					bool isPlus = abs(Peak[sNo].Type)==Peak[sNo].Type? true : false;
					for (unsigned i=sNo+1; i<=eNo; ++i)
					{
						bool isClear;
						if (isPlus)
							isClear = abs(Peak[i].Type)==Peak[i].Type?  true: false;
						else
							isClear = abs(Peak[i].Type)!=Peak[i].Type?  true: false;

						if (!isClear) break;

						if (i==eNo)
						{
							//增加正确的输出配置参数
							//ConfigArgument configArg(_ConfigArgument[ai].ArgName, _ConfigArgument[ai].BeginTime, 
							//	_ConfigArgument[ai].EndTime,_ConfigArgument[ai].InputValue);
							//_ResultArgument.push_back(configArg);

							pair<unsigned, unsigned> bufPair = std::make_pair(sNo, eNo);
							_MergePeakNo.push_back(bufPair);
						}
					}
				}
			}
		}
	}
}

VOID AnalysisConfigArguments::Analysis_HandAddPeak(const vector<CPeak> &Peak)
{//手工增加峰处理过程：
 //1，首先对参数遍历一遍，保证没有重叠区间，如果发现重叠区间，则舍弃后者
 //2，对上述得到的参数对比已经积分的波形区间，如果发现有重叠区间，则舍弃该参数

	//把增加峰和其他配置参数分离
	deque<ConfigArgument> addArgu; //增加峰参数
	deque<ConfigArgument> othArgu; //其他参数

	const unsigned ArgsSize = _ConfigArgument.size();
	for (unsigned ai=0; ai<ArgsSize; ++ai)
	{
		if (_ConfigArgument[ai].ArgName == _T("增加峰"))
			addArgu.push_back(_ConfigArgument[ai]);
		else
			othArgu.push_back(_ConfigArgument[ai]);
	}

	//1,对增加峰参数遍历
	deque<ConfigArgument> NoOverlapArgs; //无重叠区间的参数
	if (addArgu.empty()) return;
	const unsigned addArgsSize = addArgu.size();
	NoOverlapArgs.push_back(addArgu[0]); 
	for (unsigned i=0; i<addArgsSize; ++i)
	{
		bool isPutIn = true;
		for (unsigned j=0; j<NoOverlapArgs.size(); ++j)
		{
			double dA = addArgu[i].BeginTime;
			double dB = addArgu[i].EndTime;
			double dC = NoOverlapArgs[j].BeginTime;
			double dD = NoOverlapArgs[j].EndTime;
		
			if(IsOverlapping(dA,dB,dC,dD))
			{
				isPutIn = false;
				break;
			}
		}
		if (isPutIn)
		{
			NoOverlapArgs.push_back(addArgu[i]);
		}
		
	}

	//2,对上述得到的参数对比已经积分的波形区间，如果发现有重叠区间，则舍弃该参数
	_AddPeak.clear();  //清空，装入最终符合条件的参数
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<NoOverlapArgs.size(); ++i)
	{
		bool isPutIn = true;
		for (unsigned j=0; j<PeakSize; ++j)
		{
			double dA = Peak[j].StartSema.x;
			double dB = Peak[j].FinSema.x;
			double dC = NoOverlapArgs[i].BeginTime;
			double dD = NoOverlapArgs[i].EndTime;

			if (IsOverlapping(dA,dB,dC,dD))
			{
				isPutIn = false;
				break;
			}
		}
		if (isPutIn)
		{
			_AddPeak.push_back(NoOverlapArgs[i]);

			//增加正确的输出配置参数
			_ResultArgument.push_back(NoOverlapArgs[i]);
		}
	}
}

VOID AnalysisConfigArguments::Analysis_HandDelPeak(const vector<CPeak> &Peak){
	
	const unsigned ArgsSize = _ConfigArgument.size();
	for (unsigned ai=0; ai<ArgsSize; ++ai)
	{
		if (_ConfigArgument[ai].ArgName == _T("删除峰"))
			_DelPeak.push_back(_ConfigArgument[ai]);
	} 
}

VOID AnalysisConfigArguments::Analysis_AddelPeak(const vector<CPeak> &Peak){

	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();

	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("增加峰"))
		{
			{
				_AddelPeak.push_back(_ConfigArgument[i]);

				//增加正确的输出配置参数
				_ResultArgument.push_back(_ConfigArgument[i]);
			}

		}
		if (_ConfigArgument[i].ArgName == _T("删除峰"))
			_AddelPeak.push_back(_ConfigArgument[i]);
	}
}

BOOL AnalysisConfigArguments::IsOverlapping(double dA, double dB, double dC, double dD)
{
	if (dA > dB)
	{
		double tmp = dB;
		dB = dA;
		dA = tmp;
	}

	if (dC > dD)
	{
		double tmp = dD;
		dD = dC;
		dC = tmp;
	}

	if ((dC >= dB) || (dD <= dA))
		return FALSE;
	else
		return TRUE;
}


//解析手工封顶（增加峰）需要的参数
/*VOID AnalysisConfigArguments::Analysis_HandAddPeak
	(const vector<CPeak> &Peak)
{*///当前的手工峰顶[起始时间，结束时间]必须在一个完整峰的区间内
	//且其任意一点不能被包含在任意一个峰的区间内
	//对满足条件的手工分峰[起始时间，结束时间]加入一个成员变量容器内（vector<pair<double, double> >）
	//缺少对重复区间的判断！
//	const unsigned ArgsSize = _ConfigArgument.size();
//	const unsigned PeakSize = Peak.size();
//	for (unsigned i=0; i<ArgsSize; ++i)
//	{
//		bool startOK = true;
//		bool finishOK = true;
//		if (_ConfigArgument[i].ArgName == "手工封顶")
//		{
//			for (unsigned j=0; j<PeakSize; ++j)
//			{
//				if ((_ConfigArgument[i].BeginTime > Peak[j].StartSema.x)
//					&&(Peak[j].FinSema.x > _ConfigArgument[i].BeginTime)) //封顶起点位于某个峰区间内
//				{
//					startOK = false;
//					break;
//				}
//			}
//			if (startOK)
//			{
//				for (unsigned k=0; k<PeakSize; ++k)
//				{
//					if ((_ConfigArgument[i].EndTime > Peak[k].StartSema.x)
//						&&(Peak[k].FinSema.x > _ConfigArgument[i].EndTime)) //封顶终点位于某个峰区间内
//					{
//						finishOK = false;
//						break;
//					}
//				}
//			}
//			if (startOK && finishOK)
//			{
//				std::pair<double,double> buffpair = std::make_pair(_ConfigArgument[i].BeginTime, 
//																_ConfigArgument[i].EndTime);
//				_AddPeak.push_back(buffpair);
//			}
//		}
//	}
//}

//解析禁止积分的峰号
VOID AnalysisConfigArguments::Analysis_BanPeak
	(const vector<CPeak> &Peak)
{
	//传入的参数已经排序,预处理
	//遍历现有的参数，找到“禁止积分”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("禁止积分"))
		{
			//增加正确的输出配置参数
			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);

			for (unsigned j=0; j<PeakSize; ++j)
			{//只要峰顶包含在时间区段内
				if ((Peak[j].PeakSema.x - _ConfigArgument[i].BeginTime > 0.000000)
					&&(_ConfigArgument[i].EndTime - Peak[j].PeakSema.x > 0.000000))
				{
					_BanPeakNo.push_back(j);
				}
			}
		}
	}
}

VOID AnalysisConfigArguments::Analysis_MinHalfPeakWidth
	(const vector<CPeak> &Peak)
{
	//传入的参数已经排序,预处理
	//遍历现有的参数，找到“分段最小半峰宽”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段最小半峰宽"))
		{
			for (unsigned j=0; j<PeakSize; ++j)
			{//只要峰顶包含在时间区段内
				if ((Peak[j].PeakSema.x - _ConfigArgument[i].BeginTime > 0.000000)
					&&(_ConfigArgument[i].EndTime - Peak[j].PeakSema.x > 0.000000))
				{
					_MinHalfWidth[j] = _ConfigArgument[i].InputValue; //改变默认值
				}
				//不改变默认值
			}
		}
	}
}

VOID AnalysisConfigArguments::Analysis_MinPeakArea
	(const vector<CPeak> &Peak)
{
	//传入的参数已经排序,预处理
	//遍历现有的参数，找到“分段最小峰面积”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	if (_MinPeakArea.size() != PeakSize)
	{
		ASSERT(_MinPeakArea.size() != PeakSize);
	}
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段最小峰面积"))
		{
			

			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);

			for (unsigned j=0; j<PeakSize; ++j)
			{//只要峰顶包含在时间区段内
				if ((Peak[j].PeakSema.x - _ConfigArgument[i].BeginTime > 0.000000)
					&&(_ConfigArgument[i].EndTime - Peak[j].PeakSema.x > 0.000000))
				{
					_MinPeakArea[j] = _ConfigArgument[i].InputValue; //改变默认值
				}
			}
		}
	}
}

VOID AnalysisConfigArguments::Analysis_MinPeakHeight
	(const vector<CPeak> &Peak) 
{
	const unsigned ArgsSize = _ConfigArgument.size();
	const unsigned PeakSize = Peak.size();
	if (_MinPeakHeight.size() != PeakSize)
	{
		ASSERT(_MinPeakHeight.size() != PeakSize);
	}
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == _T("分段最小峰高"))
		{

			ConfigArgument configArg(_ConfigArgument[i].ArgName, _ConfigArgument[i].BeginTime, 
				_ConfigArgument[i].EndTime,_ConfigArgument[i].InputValue);
			_ResultArgument.push_back(configArg);

			for (unsigned j=0; j<PeakSize; ++j)
			{//只要峰顶包含在时间区段内
				if ((Peak[j].PeakSema.x - _ConfigArgument[i].BeginTime > 0.000000)
					&&(_ConfigArgument[i].EndTime - Peak[j].PeakSema.x > 0.000000))
				{
					_MinPeakHeight[j] = _ConfigArgument[i].InputValue; //改变默认值
				}
			}
		}
	}
}


//---------------------------------------------------------------------------------
// 获得各层扫描所需要的参数
//---------------------------------------------------------------------------------
vector<double>& AnalysisConfigArguments::GetPeakSearchArgs()
{ return _SlopeBased; }

deque<bool>& AnalysisConfigArguments::GetMinusCheckArgs()
{ return _MinusCheck; }

vector<unsigned>& AnalysisConfigArguments::GetBanPeakArgs()
{ return _BanPeakNo; }

vector<double>& AnalysisConfigArguments::GetMinHalfPeakWidthArgs()
{ return _MinHalfWidth; }

vector<double>& AnalysisConfigArguments::GetMinPeakAreaArgs()
{ return _MinPeakArea; }

vector<double>& AnalysisConfigArguments::GetMinPeakHeightArgs()
{ return _MinPeakHeight; }

vector<double>& AnalysisConfigArguments::GetHandAdjustPeakArgs()
{ return _AdjustPeak; }

vector<pair<unsigned, double> >& AnalysisConfigArguments::GetHandAdjustStartArgs()
{ return _AdjustStart; }

vector<pair<unsigned, double> >& AnalysisConfigArguments::GetHandAdjustFinishArgs()
{ return _AdjustFinish; }

vector<pair<double, double> >& AnalysisConfigArguments::GetMergePeakArgs()
{ return _MergePeak; }

vector<pair<unsigned, unsigned> >& AnalysisConfigArguments::GetMergePeakNo()
{ return _MergePeakNo; }

deque<ConfigArgument>& AnalysisConfigArguments::GetAddPeakArgs()
{ return _AddPeak; }

deque<ConfigArgument>& AnalysisConfigArguments::GetDelPeakArgs()
{ return _DelPeak; }

deque<ConfigArgument>& AnalysisConfigArguments::GetAddDelPeakArgs()
{ return _AddelPeak; }

bool AnalysisConfigArguments::GetMinusCheakStatus(double point)
{
	//遍历现有的参数，找到“分段负峰检测”参数名
	const unsigned ArgsSize = _ConfigArgument.size();
	deque<double> dequeBuff;
	double lastTime = -INT_MAX;
	for (unsigned i=0; i<ArgsSize; ++i)
	{
		if (_ConfigArgument[i].ArgName == "分段负峰检测")
		{//重叠区域的处理
			if (_ConfigArgument[i].BeginTime - lastTime >= 0.000000 )
			{
				dequeBuff.push_back(_ConfigArgument[i].BeginTime);
				dequeBuff.push_back(_ConfigArgument[i].EndTime);
			}
			lastTime = _ConfigArgument[i].EndTime;
		}
	}
	if (dequeBuff.empty()) return false;
	while(!dequeBuff.empty())
	{
		if ((dequeBuff[0]<=point)
			&(dequeBuff[1]>=point))
		{
			return true;
		}
		dequeBuff.pop_front();
		dequeBuff.pop_front();
	}
	return false;
}

deque<ConfigArgument>& AnalysisConfigArguments::GetAllArguments()
{ return _ConfigArgument; }

deque<ConfigArgument>& AnalysisConfigArguments::GetResultArgument()
{ return _ResultArgument; }

double& AnalysisConfigArguments::GetDefaultPeak2Valley()
{ return _defaultPeak2Valley; }

bool AnalysisConfigArguments::IsPeak2Valley()
{
	bool bResult = true;
	int iCount = 0;
	const unsigned ArgsSize = _ConfigArgument.size();
	for (unsigned ai=0; ai<ArgsSize; ++ai)
	{
		if (_ConfigArgument[ai].ArgName == "重叠峰处理")
		{
			iCount++;
			if (iCount >= 2) //因为有1个参数是用来作为全曲线垂直分峰的，所以再出现一个合理参数则不用峰谷比
			{
				bResult = false;
				break;
			}
		}
	}
	return bResult;
}