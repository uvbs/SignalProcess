
#include "stdafx.h"
#include "MovingAverage.h"


CMovingAverage::CMovingAverage(DWORD MAPointsCount):srcData(MAPointsCount),nextIndex(0),newData(NULL),newDataLength(0)
{
}
CMovingAverage::~CMovingAverage()
{
	delete []newData;
	newData = NULL;
	newDataLength = 0;
}

const double* CMovingAverage::Process(const double* src, DWORD count)
{
	if(newDataLength < count)
	{
		newDataLength = count;
		delete []newData;
		newData = new double[newDataLength];
	}
	for(DWORD i=0; i<count; ++i)
	{
		srcData.SetItem(nextIndex++, src[i]);

		newData[i] = 0;
		if(nextIndex <= srcData.GetCount())
		{
			const double* pData = srcData.GetPointer(0);
			for(DWORD j=0; j<nextIndex; ++j) newData[i] += pData[j];
			newData[i] /= nextIndex;
		}
		else 
		{
			DWORD index = nextIndex - srcData.GetCount();
			const double* pData = srcData.GetPointer(index);
			for(DWORD j=index; j<nextIndex; ++j) newData[i] += pData[j];
			newData[i] /= srcData.GetCount();
		}
	}
	return newData;
}