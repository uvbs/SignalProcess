
#pragma once


#include "CircularBuffer.h"

class CMovingAverage
{
public:
	CMovingAverage(DWORD MAPointsCount);
	~CMovingAverage();

public:
	const double* Process(const double* src, DWORD count);

private:
	TCircularBuffer<double> srcData;
	DWORD nextIndex;
	double* newData;
	DWORD newDataLength;
};