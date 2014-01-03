/////////////////////////////////////////////////////////////////////////
// function.h is for functions.cpp.						               //
/////////////////////////////////////////////////////////////////////////
#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__



//___________________________________________________________________________
//
// Time wait macro                         (by PHH)
//___________________________________________________________________________
//
//This macro can delay 'dwDalayTime' ms .
#define DELAY(dwDalayTime)\
 {\
    DWORD dwBegin = GetTickCount()  ;\
    DWORD dwEnd ;\
    for(;;) {\
    dwEnd = GetTickCount()  ;\
    if(dwEnd-dwBegin>=dwDalayTime || dwEnd-dwBegin<0) break ;\
       }\
  }

void UnitedGrid(CGridCtrl &grid, int row, int column);

//___________________________________________________________________________
//
// from functions.c                         (by PHH)
//___________________________________________________________________________
//
//BOOL FAR PASCAL CopyFile(LPSTR lpszName1, LPSTR lpszName2)  ;
//BOOL FAR PASCAL ChangeFileExt(LPSTR lpszSource, LPSTR lpszExt, LPSTR lpszNew, int nNewSize);
//BOOL far pascal TakeOffFileName(LPCSTR lpszFullName, LPSTR lpszFileName, size_t uSize);
/*
BOOL far PASCAL _export 
     PHH_Rectangle(
                   HDC hDC, 
                   int nLeft, 
                   int nTop, 
                   int nRight, 
                   int nBottom,
                   COLORREF colorLine,
                   COLORREF colorFill
                  ) ;
*/
//BOOL far PASCAL _export DrawBitmap (HDC hdc, HBITMAP hBitmap, short xStart, short yStart); //PHH 2003编译器需要
BOOL PASCAL DrawBitmap (HDC hdc, HBITMAP hBitmap, short xStart, short yStart);
//线性回归
//void far PASCAL _export Linear(float far x[],float far y[],int n,float far *a,float far *b,float far *r);
//void PASCAL Linear(float far x[],float far y[],int n,float far *a,float far *b,float far *r);


#endif