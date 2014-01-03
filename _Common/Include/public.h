/******************************************************************************\
*       Copyright (C) 1993.12.8 Pu Honghao.
*       All rights reserved. 
\******************************************************************************/

#ifndef __PUBLIC_H__
#define __PUBLIC_H__

//For the DLL export function declare
#ifndef _DLLEXP_
	#define _DLLEXP_ __declspec(dllexport)
#endif

#ifndef __INPUTPAR_H__
	#include "INPUTPAR.H"
#endif


#if !defined NDEBUG
#define WinAssert(exp)\
         {\
          if(!(exp))\
             {\
              char szBuffer[50] ;\
              wsprintf(szBuffer, "File %s, line %d", __FILE__, __LINE__);\
if(IDABORT==MessageBox((HWND)NULL, (LPSTR)szBuffer, (LPSTR)"Assertion Error",\
MB_ABORTRETRYIGNORE | MB_ICONHAND)) FatalExit(-1) ; \
             }\
          }
#else
#define WinAssert(exp) (exp)
#endif

//Define the stadard output console
#define VALID_CONSOLE(hConsoleW) ((hConsoleW=GetStdHandle(STD_OUTPUT_HANDLE))!=(HANDLE)INVALID_HANDLE_VALUE)


//========== Struct Define ================================================
union BITOFBYTE {
	BYTE by;
	struct {
		BYTE b0: 1;
		BYTE b1: 1;
		BYTE b2: 1;
		BYTE b3: 1;
		BYTE b4: 1;
		BYTE b5: 1;
		BYTE b6: 1;
		BYTE b7: 1;
	};
};

//========== Const Define =================================================

//---------- File System ------------
const int MAX_FILENAME_LENGTH = 256	;
const int MAX_CSTRING_LENGTH = 5120	;
const int MAX_DIR_LENGTH = 260	;
//---------- DDE -------------------------------------
const int MAX_DDESERVICE_LENGTH = MAX_FILENAME_LENGTH;
const int MAX_DDETOPIC_LENGTH = 100  ;
const int MAX_DDEITEM_LENGTH = 100  ;

#define STR_COM1_DLL	"series1.dll"
#define STR_COM2_DLL	"series2.dll"
#define STR_COM3_DLL	"series3.dll"
#define STR_COM4_DLL	"series4.dll"


//========== End of Const Define ==========================================


//========== Function Define ==============================================

//---------- publicfun.cpp -----------------------------------------
BOOL pascal 
	TakeOffFileName(PCSTR lpszFullName, PSTR lpszFileName, size_t uSize);
BOOL RegisterError(int nErrCode, PCSTR szErrString);
	const int ERR_DLLNOTFOUND = 1; //szErrString: The DLL file name
BOOL IsValidFileName(PSTR szFileName);

int GetBit(const void *pValue, 
		   const BYTE bySize,
		   char *szBuffer, 
		   const int nBufferSize,
		   BOOL bDelZero  = FALSE
		  );

//----------ValueToString
//BYTE
int ValueToString(const unsigned char byValue, 
				  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
//WORD
int ValueToString(const unsigned short wValue, 
				  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
//DWORD
int ValueToString(const unsigned long dwValue, 
				  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
//short
int ValueToString(const short nValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
//long
int ValueToString(const long lValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
//double
int ValueToString(const double dValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
//float
int ValueToString(const float dValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
inline int ValueToString(const float dValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt//Default = VALUE_DEC
				 )
{return(ValueToString((double)dValue, szBuffer, nMaxSize, typeInt));}

//CString
int ValueToString(const CString cstrValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt = VALUE_DEC
				 );
inline int ValueToString(const CString cstrValue, 
		  		  char *szBuffer, 
				  const int nMaxSize, 
				  const INT_TYPE typeInt//Default = VALUE_DEC
				 )
{strncpy(szBuffer, cstrValue, nMaxSize);return strlen(szBuffer);}


//----------ValueToString 13890164000
//BYTE
BOOL StringToValue(const char *szBuffer, 
				   unsigned char  &byValue, 
				   const INT_TYPE typeInt//Default = VALUE_DEC
				  );
//WORD 
BOOL StringToValue(const char *szBuffer, 
				   unsigned short &nValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );
//DWORD 
BOOL StringToValue(const char *szBuffer, 
				   unsigned long &nValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );
//int
BOOL StringToValue(const char *szBuffer, 
				   short &nValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );
//long
BOOL StringToValue(const char *szBuffer, 
				   long &lValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );
//double
BOOL StringToValue(const char *szBuffer, 
				   double &dValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );

inline BOOL StringToValue(const char *szBuffer, 
						  double &dValue, 
						  const INT_TYPE typeInt
						 )
{
  ASSERT(typeInt==VALUE_DEC);
  return(sscanf(szBuffer, "%f", &dValue));
}

//float
BOOL StringToValue(const char *szBuffer, 
				   float &dValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );
inline BOOL StringToValue(const char *szBuffer, 
				   float &dValue, 
				   const INT_TYPE typeInt
				  )
{return(StringToValue(szBuffer, (double &)dValue, typeInt));}

//CString
BOOL StringToValue(const char *szBuffer, 
				   CString &cstrValue, 
				   const INT_TYPE typeInt = VALUE_DEC
				  );
inline BOOL StringToValue(const char *szBuffer, 
				   CString &cstrValue, 
				   const INT_TYPE typeInt
				  )
{cstrValue = szBuffer;return TRUE;}


//---------- Interfac.cpp (from COMMPORT.MAK)------------------------
BOOL WINAPI CP_IsOK();
BOOL WINAPI CP_Setting();
BOOL WINAPI CP_Read(
		 LPVOID   lpBuffer,
	     const DWORD    nNumberOfBytesToRead,
	     LPDWORD  lpNumberOfBytesRead,
	     LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine
		);
BOOL WINAPI CP_Write( 
	     const LPCVOID  lpBuffer,
	     const DWORD  nNumberOfBytesToWrite,
		 LPDWORD  lpNumberOfBytesWriten,
	     LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine
		);
void WINAPI CP_Hold();
BOOL WINAPI CP_Free();
BOOL WINAPI CP_AbortRead();
BOOL WINAPI CP_AbortWrite();

typedef BOOL (WINAPI *PFNCOMM_IsOK)();
typedef BOOL (WINAPI *PFNCOMM_Setting)();
typedef void (WINAPI *PFNCOMM_Hold)();
typedef BOOL (WINAPI *PFNCOMM_Free)();
typedef BOOL (WINAPI *PFNCOMM_Read)(
							 LPVOID,
						     const DWORD,
						     LPDWORD,
						     LPOVERLAPPED_COMPLETION_ROUTINE
							);
typedef BOOL (WINAPI *PFNCOMM_Write)(
							 LPCVOID,
						     const DWORD,
							 LPDWORD,
						     LPOVERLAPPED_COMPLETION_ROUTINE
							);
typedef BOOL (WINAPI *PFNCOMM_AbortRead)();
typedef BOOL (WINAPI *PFNCOMM_AbortWrite)();
typedef BOOL (WINAPI *PFNCOMM_GetDCB)(DCB *);
typedef BOOL (WINAPI *PFNCOMM_SetDCB)(DCB *);

//---------- Interface for users ------------------------
_DLLEXP_ WORD CreateInputDlg(CWnd* pParent);
_DLLEXP_ BOOL AddAEdit(WORD wID, CBaseInputParam *pcInput);
_DLLEXP_ BOOL AddASelect(WORD wID, CBaseInputParam *pcInput);
_DLLEXP_ BOOL DeleteInputDlg(WORD wID);
_DLLEXP_ int StartInputDlg(WORD wID);



//========== End of Function Define =======================================


/*
typedef struct tagCHRO_PIC_DATA
{
	int  dwNo;
	int  lVal;
}CHRO_PIC_DATA;
*/



#endif //#ifndef __PUBLIC_H__
