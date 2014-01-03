
#pragma once

//GetPointer:从任何位置开始，都可以获得完整的数据
// 示例：
// void Out(int* pData, DWORD dwCount)
// {
//     for(DWORD i=0; i<dwCount; ++i) count<<pData[i];
// }
// TCircularBuffer<int> cb(4);
// int datas[4] = {1,2,3,4};
// cb.Append(0, datas, 4);
// Out(cb.GetPointer(0));//1234
// Out(cb.GetPointer(1));//2341
// Out(cb.GetPointer(2));//3412
// Out(cb.GetPointer(3));//4123
//则GetPointer(1)的数据为
//new T[dwCount]
template<typename T> class TCircularBuffer 
{
public:
	TCircularBuffer(DWORD dwCount):m_pData(new T[dwCount*2]),m_dwCount(dwCount){}

	~TCircularBuffer()
	{
		if(m_pData) delete []m_pData;
		m_pData = NULL;
	}

public:
	DWORD GetCount() const{return m_dwCount;}
	const T* GetPointer(DWORD dwIndex = 0) const{return m_pData+dwIndex%m_dwCount;}//获取普通缓冲区指针（数据只有最近的最多m_dwCount个有效）
	const T& GetItem(DWORD dwIndex) const{return m_pData[dwIndex%m_dwCount];}
	const T& operator[](DWORD dwIndex) const{return GetItem(dwIndex);}

	void SetItem(DWORD dwIndex, const T& data)
	{
		dwIndex %= m_dwCount;
		memcpy(m_pData+dwIndex, &data, sizeof(T));
		memcpy(m_pData+m_dwCount+dwIndex, &data, sizeof(T));
	}

	void Append(DWORD dwIndexFrom, const T* pData, DWORD dwCount)
	{
		if(dwCount > m_dwCount)
		{
			pData += (dwCount - m_dwCount);
			dwIndexFrom += (dwCount - m_dwCount);
			dwCount = m_dwCount;
		}
		dwIndexFrom %= m_dwCount;

		if(dwIndexFrom + dwCount > m_dwCount)
		{//A(BC)D:CBCB
			memcpy(m_pData, pData + (dwIndexFrom + dwCount - m_dwCount), sizeof(T) * dwIndexFrom);//A->C
			memcpy(m_pData+dwIndexFrom, pData, sizeof(T) * dwCount);//CB->BC
			memcpy(m_pData+dwIndexFrom+m_dwCount, pData, sizeof(T) * (m_dwCount - dwIndexFrom));//D->B
		}
		else
		{
			memcpy(m_pData+dwIndexFrom, pData, sizeof(T) * dwCount);
			memcpy(m_pData+dwIndexFrom+m_dwCount, pData, sizeof(T) * dwCount);
		}
	}

private:
	T* m_pData;
	DWORD m_dwCount;
};