/*************************************************************************\
*       This is a part of the SCADAS.                                    *
*       Copyright (C) 1995.5.28 Pu Honghao.                               *
*       All rights reserved.                                             *
\*************************************************************************/

//Prevent include this files twice
#ifndef __INPUTPAR_H__
#define __INPUTPAR_H__

#ifndef __AFXTEMPL_H__
	#include <afxtempl.h>
#endif

#define MAX_VALUESTRING_SIZE 30

enum INT_TYPE {VALUE_HEX, VALUE_DEC, VALUE_OCT, VALUE_BIN};



class CBaseInputParam
{
  protected:
	CString  m_cstrParamName;
	CString  m_cstrNotice;

  public: 
  	CBaseInputParam(){}

	void SetNotice(char *szNotice) 	{m_cstrNotice = szNotice;}
	void SetName(char *szName) 	{m_cstrParamName = szName;}
	const CString GetNotice() 	{return(m_cstrNotice);}
	const CString GetName() 	{return(m_cstrParamName);}

  	virtual BOOL IsValueInRange() = 0;
	virtual operator LPCSTR() = 0;
	virtual void operator =(char *) = 0;
	virtual BOOL GetValueStr(int nIndex, CString &cStr)=0;
	virtual int  GetValueTotalNumber()		{ASSERT(FALSE);return -1;}
};


template <class T, INT_TYPE TtypeInt = VALUE_DEC>
class CInputParam : public CBaseInputParam
{
  public:
  	T 		 m_TValue;

  protected:
  	BOOL bJudgeRange;
  	T m_Tmin;
	T m_Tmax;
	CString m_cstrValueStr;

  public:
  	CInputParam();
  	CInputParam(T TinitValue, char *szName = NULL);
	
	//----------Virtual function derived from my father---------------
	virtual BOOL GetValueStr(int nIndex, CString &cStr)
		{return(FALSE);}//This function NOT used

  	virtual BOOL IsValueInRange();
	virtual operator LPCSTR();
	virtual void operator =(char *szValueStr);
	//-----------------------------------------------------------------
	virtual operator T();
	CInputParam<T, TtypeInt> &operator =(T TValue);
	virtual void SetRange(T Tmin, T Tmax);
};


template <class T, INT_TYPE TtypeInt = VALUE_DEC>
class CSelectParam : public CInputParam<T, TtypeInt>
{
  protected:
  	CArray<T, T> m_cArrayAllMember;

  public:
	CSelectParam();
	CSelectParam(T TinitValue, char *szName = NULL);

	//----------Virtual function derived from my father----------------
	CSelectParam<T, TtypeInt> &operator =(T TValue);
	virtual BOOL GetValueStr(int nIndex, CString &cStr);
	virtual int  GetValueTotalNumber();
};


#include "inputpar.inl"

#endif //#ifndef __INPUTPAR_H__
