
#ifndef TWEN_SDK_CApplication
#define TWEN_SDK_CApplication

class CApplication
{
public:
	static CString GetApplicationPath();
};

////////////////////////////////////////////////////////////////////////////
// CDim
////////////////////////////////////////////////////////////////////////////
template<typename T> class CDim
{
public:
	CDim():m_dwDims(0),m_pDims(NULL),m_pData(NULL){}
	~CDim(){Reset();}

	void Reset();
	void New(DWORD dwDims,...);//dwDims:维数
	DWORD Dim(DWORD dwDim) const;//获取该位置的维数
	void Init(T value);
	T& operator()(DWORD dwDim0,...);
	const T& operator()(DWORD dwDim0,...) const;

	DWORD Index(DWORD dwDim0,...) const;

protected:
	DWORD m_dwDims;
	DWORD* m_pDims;
	T* m_pData;
};

template<typename T> void CDim<T>::Reset()
{
	m_dwDims = 0;
	if(m_pDims)
	{
		delete []m_pDims;
		m_pDims = NULL;
	}
	if(m_pData)
	{
		delete []m_pData;
		m_pData = NULL;
	}
}
template<typename T> void CDim<T>::New(DWORD dwDims,...)
{
	Reset();
	m_dwDims = dwDims;
	if(m_dwDims == 0) return;
	m_pDims = new DWORD[m_dwDims];
	DWORD dwCount = 1;
	va_list argList;
	va_start(argList, dwDims);
	for(DWORD i=0; i<m_dwDims; i++)
	{
		m_pDims[i] = va_arg(argList, DWORD);
		dwCount *= m_pDims[i];
	}
	if(dwCount) m_pData = new T[dwCount];
	va_end(argList);
}
template<typename T> DWORD CDim<T>::Dim(DWORD dwDim) const
{
	if(dwDim >= m_dwDims || m_pDims == NULL) return 0;
	return m_pDims[dwDim];
}
template<typename T> void CDim<T>::Init(T value)
{
	if(m_dwDims == 0 || m_pData == NULL) return; 
	DWORD dwCount = 1;
	for(DWORD i=0; i<m_dwDims; i++) dwCount *= m_pDims[i];
	for(DWORD j=0; j<dwCount; j++) m_pData[j] = value;
}
template<typename T> T& CDim<T>::operator()(DWORD dwDim0,...)
{
	if(m_dwDims == 0 || m_pData == NULL) throw 0;
	DWORD* pDims = new DWORD[m_dwDims];
	pDims[0] = dwDim0;
	va_list argList;
	va_start(argList, dwDim0);
	for(DWORD i=1; i<m_dwDims; i++)
	{
		pDims[i] = va_arg(argList, DWORD);
	}
	va_end(argList);

	DWORD dwIndex = pDims[m_dwDims-1];
	DWORD dwTempCount = m_pDims[m_dwDims-1];
	for(long j=m_dwDims-2; j>-1; j--)
	{
		dwIndex += dwTempCount * pDims[j];
		dwTempCount *= m_pDims[j];
	}
	delete []pDims;
	return m_pData[dwIndex];
}
template<typename T> const T& CDim<T>::operator()(DWORD dwDim0,...) const
{
	if(m_dwDims == 0 || m_pData == NULL) throw 0;
	DWORD* pDims = new DWORD[m_dwDims];
	pDims[0] = dwDim0;
	va_list argList;
	va_start(argList, dwDim0);
	for(DWORD i=1; i<m_dwDims; i++)
	{
		pDims[i] = va_arg(argList, DWORD);
	}
	va_end(argList);

	DWORD dwIndex = pDims[m_dwDims-1];
	DWORD dwTempCount = m_pDims[m_dwDims-1];
	for(long j=m_dwDims-2; j>-1; j--)
	{
		dwIndex += dwTempCount * pDims[j];
		dwTempCount *= m_pDims[j];
	}
	delete []pDims;
	return m_pData[dwIndex];
}

template<typename T> DWORD CDim<T>::Index(DWORD dwDim0,...) const
{
	if(m_dwDims == 0 || m_pData == NULL) throw 0;
	DWORD* pDims = new DWORD[m_dwDims];
	pDims[0] = dwDim0;
	va_list argList;
	va_start(argList, dwDim0);
	for(DWORD i=1; i<m_dwDims; i++)
	{
		pDims[i] = va_arg(argList, DWORD);
	}
	va_end(argList);

	DWORD dwIndex = pDims[m_dwDims-1];
	DWORD dwTempCount = m_pDims[m_dwDims-1];
	for(long j=m_dwDims-2; j>-1; j--)
	{
		dwIndex += dwTempCount * pDims[j];
		dwTempCount *= m_pDims[j];
	}
	delete []pDims;
	return dwIndex;
}

#endif