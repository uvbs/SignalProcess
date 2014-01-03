/*************************************************************************\
*       This is a part of the SCADAS.                                    *
*       Copyright (C) 1995.5.28 Pu Honghao.                               *
*       All rights reserved.                                             *
\*************************************************************************/

/*************************************************************************\
*       THIS FILE INCLUDED BY INPUTPAR.H                                   *
*       DON'T LINK THIS FILE                                             *
\*************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// CInputParam class

//--------------------------------------
template <class T, INT_TYPE TtypeInt>
CInputParam<T, TtypeInt>::
CInputParam(T TinitValue, char *szName)
 : m_TValue(TinitValue),
   bJudgeRange(FALSE)
{
 m_cstrParamName = szName;
}

//--------------------------------------
template <class T, INT_TYPE TtypeInt>
CInputParam<T, TtypeInt>::
CInputParam()
 :bJudgeRange(FALSE)
{}


//--------------------------------------
template <class T, INT_TYPE TtypeInt>
BOOL CInputParam<T, TtypeInt>::
IsValueInRange()
{
  if(bJudgeRange)
	return(m_TValue>=m_Tmin&&m_TValue<=m_Tmax);
  else return TRUE;
}

//--------------------------------------
template <class T, INT_TYPE TtypeInt>
CInputParam<T, TtypeInt>::
operator LPCSTR()
{
  char szTemp[MAX_VALUESTRING_SIZE];
  ValueToString(m_TValue, szTemp, MAX_VALUESTRING_SIZE, TtypeInt);
  m_cstrValueStr = szTemp;
  return (LPCSTR)m_cstrValueStr;
}

//------------------------------------
template <class T, INT_TYPE TtypeInt>
void CInputParam<T, TtypeInt>::
operator =(char *szValueStr)
{
  T Ttemp;
  if(StringToValue(szValueStr, Ttemp, TtypeInt)){
  	m_TValue = Ttemp;
	m_cstrValueStr = szValueStr;
   }
  return;
}

//---------------------------------------------
template <class T, INT_TYPE TtypeInt>
inline CInputParam<T, TtypeInt>::
operator T()
{
  return m_TValue;
}

//---------------------------------------------
template <class T, INT_TYPE TtypeInt>
inline CInputParam<T, TtypeInt> & CInputParam<T, TtypeInt>::
operator =(T TValue)
{
  m_TValue = TValue;
  return *this;
}

//---------------------------------------------
template <class T, INT_TYPE TtypeInt>
inline void CInputParam<T, TtypeInt>::
SetRange(T Tmin, T Tmax) 
{
  m_Tmin=Tmin; 
  m_Tmax=Tmax;
  bJudgeRange = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSelectParam class

//--------------------------------------
template <class T, INT_TYPE TtypeInt>
CSelectParam<T, TtypeInt>::
CSelectParam()
 : CInputParam<T, TtypeInt> ()
{}

//--------------------------------------
template <class T, INT_TYPE TtypeInt>
CSelectParam<T, TtypeInt>::
CSelectParam(T TinitValue, char *szName)
 :	CInputParam<T, TtypeInt>(TinitValue, szName)
{}

//------------------------------------
template <class T, INT_TYPE TtypeInt>
BOOL CSelectParam<T, TtypeInt>::
GetValueStr(int nIndex, CString &cStr)
{
  char szTemp[MAX_VALUESTRING_SIZE];

  if(nIndex<0||nIndex>(m_cArrayAllMember.GetSize()-1)) {
  	ASSERT(FALSE);
	return FALSE;
   }
  
  ValueToString(m_cArrayAllMember[nIndex], 
  				szTemp, 
  				MAX_VALUESTRING_SIZE, 
  				TtypeInt
  			   );
  
  cStr = szTemp;
  return TRUE;
}


//------------------------------------
template <class T, INT_TYPE TtypeInt>
inline int  CSelectParam<T, TtypeInt>::
GetValueTotalNumber()
{
  return(m_cArrayAllMember.GetSize());
}

//---------------------------------------------
template <class T, INT_TYPE TtypeInt>
inline CSelectParam<T, TtypeInt> & CSelectParam<T, TtypeInt>::
operator =(T TValue)
{
  m_TValue = TValue;
  return *this;
}
