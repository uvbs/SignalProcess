#pragma once
class Filter
{
public:
	Filter(void);
	~Filter(void);


	int Filter::recodeh(int nlevel,int len);
	int Filter::recodeg(int nlevel,int len);
	int Filter::recompose();
	int Filter::hperiod(int nlevel,int len);
	int Filter::gperiod(int nlevel,int len);
	int Filter::codeh(int nlevel,int len);
	int Filter::codeg(int nlevel,int len);
	int Filter::decompose();
	int Filter::filter();
	int Filter::dwt_filter();
	double Filter::WaveletFilter(double dData, double *dSave);
};

