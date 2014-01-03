#include "stdafx.h"
#include <math.h>
#include "functions.h"

/////////////////////////////////////////////////////////////////////////
// funcions.c file give you some useful subroutines.             //
//---------------------------------------------------------------------//
//      圹 NOTICE:  Copyright 1992.9.28 by Pu Honghao  圹          //
/////////////////////////////////////////////////////////////////////////

#define MAX_INIFILE_SIZE    10240



/*************************************************************************
    Draw a rectangle, (Single line, and, Pure color)
**************************************************************************/
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
                  )
{
  HBRUSH hbrushFill ;
  HBRUSH hbrushOld  ;
  HPEN hpenLine     ;
  HPEN hpenOld     ;


  hbrushFill = CreateSolidBrush(colorFill)  ;
  hbrushOld  = ::SelectObject(hDC, hbrushFill) ;

  hpenLine = CreatePen(PS_SOLID, 1, colorLine)  ;
  hpenOld  = ::SelectObject(hDC, hpenLine) ;

  Rectangle(hDC, nLeft, nTop, nRight, nBottom);
  
  SelectObject(hDC, hbrushOld)  ;
  SelectObject(hDC, hpenOld)  ;
  
  DeleteObject(hbrushFill)  ;
  DeleteObject(hpenLine)  ;
  
  return(TRUE)  ;
}
*/
/****************************************************************************
    FUNCTION: DrawBitmap()
    Purpose:  Draw bitmap to HDC
*****************************************************************************/
BOOL  PASCAL  
DrawBitmap (HDC hdc, HBITMAP hBitmap, short xStart, short yStart)
{
    BITMAP bm   ;
    HDC hdcMem  ;
    //DWORD dwSize  ;
    POINT ptSize, ptOrg ;

    hdcMem = CreateCompatibleDC (hdc)   ;
    SelectObject(hdcMem, hBitmap)   ;
    SetMapMode(hdcMem, GetMapMode(hdc)) ;

    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm)  ;
    ptSize.x = bm.bmWidth   ;
    ptSize.y = bm.bmHeight  ;
    DPtoLP(hdc, &ptSize, 1) ;

    ptOrg.x = 0 ;
    ptOrg.y = 0 ;
    DPtoLP(hdcMem, &ptOrg, 1)   ;

    BitBlt( hdc, xStart, yStart, ptSize.x, ptSize.y,
        hdcMem, ptOrg.x, ptOrg.y, SRCCOPY)  ;

    DeleteDC(hdcMem)    ;
    return TRUE;
}

void UnitedGrid(CGridCtrl &grid, int row, int column)
{
	grid.SetRowResize(FALSE);
	grid.SetColumnCount(column);
	grid.SetRowCount(row);
	grid.SetFixedRowCount(1);
	grid.SetFixedColumnCount(1);
	grid.SetEditable(FALSE);
	grid.SetDefCellMargin(3);

	grid.SetFixedColumnSelection(FALSE);


	LOGFONT	lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = 14;                      // request a 12-pixel-height font
	lf.lfQuality = ANTIALIASED_QUALITY;
	strcpy(lf.lfFaceName, "Arial");        // request a face name "Arial"
	for(int i=0; i<row; ++i)
		for (int j = 0; j < column; j++)
			grid.SetItemFont(0, j, &lf);

}

/*
//
//线性回归程序
//
void  PASCAL  
Linear(float far x[],float far y[],int n,float far *a,float far *b, float far *r)
{
	float y_sum,x_sum,y_av,x_av,lxy,lxx,lyy;
	register int i;
	x_sum=0; y_sum=0; lxy=0; lxx=0 ;lyy=0;

	if(n==1) {
		if(x[0]<0.00000001f) {
			*a =0.0f;
			*b =0.0f;
			*r =0.0f;
		}
		else {
			*a = y[0]/x[0];
			*b = 0.0f;
			*r = 1.0f;
		}
		return;
	}



	for (i=0;i<n ;++i) {
		x_sum=x_sum+x[i];
		y_sum=y_sum+y[i];
       }

	x_av=x_sum/n;
	y_av=y_sum/n;

	for (i=0;i<n ;++i) {
		lxy=lxy+(x[i]-x_av)*(y[i]-y_av);
		lxx=lxx+(x[i]-x_av)*(x[i]-x_av);
		lyy=lyy+(y[i]-y_av)*(y[i]-y_av);
	   }
	if(fabs(lxy)<0.00000001f||fabs(lxx)<0.00000001f) {
		*b=0;
		*a=0;
		*r=0;
	   }
	else {   
		*b=lxy/lxx;
		*a=y_av-(*b)*x_av;
		*r=lxy/sqrt(lxx*lyy);
	   }

	return;
}
*/
