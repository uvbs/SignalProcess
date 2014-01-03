#include "StdAfx.h"
#include "Filter.h"


Filter::Filter(void)
{
}


Filter::~Filter(void)
{
}


/*---------------------------------------------
 测试小波滤波算法
---------------------------------------------*/

const int datalen=32;
const int dbN=3;
const int stages=4;

/* interface array, put your signal data here and
     pure signal is returned here after filtering*/
double dataio[datalen];
double data[datalen];// main data arrays
double har[stages][datalen];
double gar[stages][datalen];
static const double db[]={0.3326706,0.8068915,0.4598775,-0.1350110,
-0.0854413,0.0352263};

int Filter::recodeh(int nlevel,int len)
{
	int i,k,k1,len1;
	double temp;

	len1=len>>1;
	for(i=0;i<len;i++)
	{
		dataio[i]=0.0;
		if(len>(2*dbN))
		{
			for(k1=(i-dbN);k1<(i+dbN);k1++)
			{
				k=k1/2;
				if((k>=0) && (k<len1) &&((k1%2)==0) && ((temp=har[nlevel][(i-k1+len)%len])!=0))
					dataio[i]+=temp*data[k];
			}
			for(k1=(i+len-dbN);k1<(i+len+1);k1++)
			{
				k=k1/2;
				if((k>=0) && (k<len1) &&((k1%2)==0) && ((temp=har[nlevel][(i-k1+len)%len])!=0))
					dataio[i]+=temp*data[k];
			}
			for(k1=(i-len+1);k1<(i-len+dbN);k1++)
			{
				k=k1/2;
				if((k>=0) && (k<len1) &&((k1%2)==0) && ((temp=har[nlevel][(i-k1+len)%len])!=0))
					dataio[i]+=temp*data[k];
			}
		}
		else
		{
			for(k=0;k<len1;k++)
			{
				if((temp=har[nlevel][(i-2*k+len)%len])!=0)
					dataio[i]+=temp*data[k];
			}
		}
	}
	return 1;
}
int Filter::recodeg(int nlevel,int len)
{
	int i,k,k1,len1;
	double temp;

	len1=len>>1;
	for(i=0;i<len;i++)
	{
		if(len>(2*dbN))
		{
			for(k1=(i+len-dbN-2);k1<(i+len);k1++)
			{
				k=k1/2;
				if((k>=0) && (k<len1) &&((k1%2)==0) && ((temp=gar[nlevel][(i-k1+len)%len])!=0))
					dataio[i]+=temp*data[k+len1];
			}
			for(k1=(i-dbN-2);k1<(i+dbN-2);k1++)
			{
				k=k1/2;
				if((k>=0) && (k<len1) &&((k1%2)==0) && ((temp=gar[nlevel][(i-k1+len)%len])!=0))
					dataio[i]+=temp*data[k+len1];
			}
			for(k1=(i-len);k1<(i-len+dbN-2);k1++)
			{
				k=k1/2;
				if((k>=0) && (k<len1) &&((k1%2)==0) && ((temp=gar[nlevel][(i-k1+len)%len])!=0))
					dataio[i]+=temp*data[k+len1];
			}
		}
		else
		{
			for(k=0;k<len1;k++)
			{
				if((temp=gar[nlevel][(i-2*k+len)%len])!=0)
					dataio[i]+=temp*data[k+len1];
			}
		}
	}
	return 1;
}
int Filter::recompose()
{
	int nlevel,len,j;

	for(nlevel=stages-1;nlevel>0;nlevel--)
	{
		len=datalen>>(nlevel-1);
		recodeh(nlevel,len);
		recodeg(nlevel,len);
		for(j=0;j<datalen;j++)
			data[j]=dataio[j];
	}
	return 1;
}
int Filter::hperiod(int nlevel,int len)
{
	int sign,i;
	double temp;

	for(sign=0;sign<len;sign++)
	{
		temp=0.0;
		for(i=0;i<(2*dbN);i++)
		{
			if( ( (sign-i+dbN-1) % len) == 0)
				temp+=db[i];
		}
		har[nlevel][sign]=temp;
	}
	return 1;
}
int Filter::gperiod(int nlevel,int len)
{
	int sign,i,flag;
	double temp;

	for(sign=0;sign<len;sign++)
	{
		temp=0.0;
		for(i=0;i<(2*dbN);i++)
		{
			if( ( (sign+i-dbN-2) % len) == 0)
			{
				if((dbN-i+1)%2)
					flag=-1;
				else
					flag=1;
				temp+=db[i]*flag;
			}
		}
		gar[nlevel][sign]=temp;
	}
	return 1;
}
int Filter::codeh(int nlevel,int len)
{
	int i,i2,k,len1;
	double temp;

	len1=len>>1;
	for(i=0;i<len1;i++)
	{
		i2=2*i;
		data[i]=0.0;
		if(len>(2*dbN))
		{
			for(k=i2-len;k<i2-len+dbN+1;k++)
			{
				if((k>=0) && (k<len) && ( (temp=har[nlevel][(k-i2+len)%len]) != 0 ) )
					data[i]+=temp*dataio[k];
			}
			for(k=i2-dbN+1;k<i2+dbN+1;k++)
			{
				if((k>=0) && (k<len) && ( (temp=har[nlevel][(k-i2+len)%len]) != 0) )
					data[i]+=temp*dataio[k];
			}
			for(k=i2+len-dbN+1;k<i2+len;k++)
			{
				if((k>=0) && (k<len) && ( (temp=har[nlevel][(k-i2+len)%len]) != 0) )
					data[i]+=temp*dataio[k];
			}
		}
		else
		{
			for(k=0;k<len;k++)
			{
				if((temp=har[nlevel][(k-i2+len)%len]) != 0)
					data[i]+=temp*dataio[k];
			}
		}
	}
	return 1;
}
int Filter::codeg(int nlevel,int len)
{
	int i,i2,k,len1;
	double temp;

	len1=len>>1;
	for(i=0;i<len1;i++)
	{
		i2=2*i;
		data[i+len1]=0.0;
		if(len>(2*dbN))
		{
			for(k=i2-len;k<i2-len+dbN+3;k++)
			{
				if((k>=0) && (k<len) && ( (temp=gar[nlevel][(k-i2+len)%len]) != 0 ) )
					data[i+len1]+=temp*dataio[k];
			}
			for(k=i2-dbN+3;k<i2+dbN+3;k++)
			{
				if((k>=0) && (k<len) && ( (temp=gar[nlevel][(k-i2+len)%len]) != 0) )
					data[i+len1]+=temp*dataio[k];
			}
			for(k=i2+len-dbN+3;k<i2+len;k++)
			{
				if((k>=0) && (k<len) && ( (temp=gar[nlevel][(k-i2+len)%len]) != 0) )
					data[i+len1]+=temp*dataio[k];
			}
		}
		else
		{
			for(k=0;k<len;k++)
			{
				if((temp=gar[nlevel][(k-i2+len)%len]) != 0)
					data[i+len1]+=temp*dataio[k];
			}
		}
	}
	return 1;
}
int Filter::decompose()
{
	int nlevel,len,j;

	// main loop for the nlevel-th decomosition
	for(nlevel=1;nlevel<stages;nlevel++)
	{
		len=datalen>>(nlevel-1);
		codeh(nlevel,len);
		codeg(nlevel,len);
		for(j=0;j<datalen;j++)
			dataio[j]=data[j]; 
	}
	return 1;
}
int Filter::filter(/* PFILTER pFilter */)
{
	int len,j;
	double dfilter = 0.01;

	len = datalen>>(stages-1);
	for( j=len; j<datalen; j++ )
	{
		// 这里需要使用指定的过滤函数，对越高阶的小波成分，衰减越大才能起到滤波作用
		// data[j] = (*pFilter)(data[j], j);
		// data[j] = 0.0; //最初的过滤方法，直接令高阶分量为0
		data[j] *= dfilter; // 这里让高阶分量乘以小的因子
		dfilter *= 0.01; // 因子为pow(0.01, j-len+1)
	}
	return 1;
}
int Filter::dwt_filter()
{
	int nlevel,len,j;

	for(nlevel=1;nlevel<stages;nlevel++)
	{
		len=datalen>>(nlevel-1);
		hperiod(nlevel,len);
		gperiod(nlevel,len);
	}
	decompose();
	filter();// get rid of detail part of components

	for(j=0;j<datalen;j++)
		dataio[j]=data[j]; 
	recompose();

	return 1;
}

double Filter::WaveletFilter(double dData, double *dSave)
{
	int j;

	// shifting and saving the origin data
	for(j=0; j<datalen-1; j++)
	{
		dSave[j] = dSave[j+1];
	}
	dSave[datalen-1] = dData;

	for(j=0; j<datalen; j++)
		dataio[j] = dSave[j];

	// wavelet transformation
	dwt_filter();

	return dataio[datalen-1];
}