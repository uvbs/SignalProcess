#pragma once
#include <math.h>
#include "Peak.h"

//DB4小波的尺度函数系数
const double h0 = (1.0 + sqrtf(3.0)) / (4 * sqrtf(2.0));
const double h1 = (3.0 + sqrtf(3.0)) / (4 * sqrtf(2.0));
const double h2 = (3.0 - sqrtf(3.0)) / (4 * sqrtf(2.0));
const double h3 = (1.0 - sqrtf(3.0)) / (4 * sqrtf(2.0));

//DB4小波的小波函数系数
const double g0 = h3;
const double g1 = -h2;
const double g2 = h1;
const double g3 = h0;


// CWaveletTransform

class CWaveletTransform : public CWnd
{
	DECLARE_DYNAMIC(CWaveletTransform)

public:
	CWaveletTransform();
	virtual ~CWaveletTransform();
	void WatchDate(double TimeData[], double VoltageData[], int n); //监视数据
	void Reshape(double TimeData[],double VoltageData[], int n); //对得到的数据整形
	void MovingAverage(double TimeData[], double VoltageData[], int n, int Avgtimes); //用移动平均方法滤波
	void DB4DWT(double Data[], int n); // DB4小波变换
	void DB4LiftDWT(double Data[], int n); // DB4小波变换提升算法
	void HaarDWT(double Src_val[]/*原始数据*/,double** C_val/*处理数据*/,int n_total,int levl); //离散小波分解

//以下是重构的方法
	double Reshape(std::vector<std::pair<double,double> > &ReshapeData);  //对得到的数据整形
	double Reshape_v2(std::vector<CSema> &ReshapeData); //对得到的数据整形
	void MovingAverage(std::vector<std::pair<double,double> >&ReshapeData, int Avgtimes); //用移动平均方法滤波

public:
	double set_D(double a,double b);
	double set_A(double a,double b);
	

protected:
	DECLARE_MESSAGE_MAP()
};


