
//C2Bitmap.cpp

#include <stdafx.h>
#include "C2Bitmap.h"

HANDLE C2Bitmap::DDBToDIB(CBitmap*pBitmap,DWORD dwCompression,CPalette*pPal,CDC*pDC) 
{
	if(pBitmap==NULL || pBitmap->GetSafeHandle()==NULL) return NULL;
	//The function has no arg for bitfields 
	if(dwCompression==BI_BITFIELDS) return NULL;
	HPALETTE hPal=NULL;
	//If a palette has not been supplied use defaul palette
	if(pPal==NULL || (hPal=(HPALETTE)pPal->GetSafeHandle())==NULL)
		hPal=(HPALETTE)GetStockObject(DEFAULT_PALETTE);
	BITMAP bm;
	pBitmap->GetBitmap(&bm);
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER); 
	bi.biWidth = bm.bmWidth; 
	bi.biHeight = bm.bmHeight; 
	bi.biPlanes = 1; 
	bi.biBitCount = bm.bmPlanes * bm.bmBitsPixel; 
	bi.biCompression = dwCompression; 
	bi.biSizeImage = 0; 
	bi.biXPelsPerMeter = 0; 
	bi.biYPelsPerMeter = 0; 
	bi.biClrUsed = 0; 
	bi.biClrImportant = 0;
	//Compute the size of the infoheader and the color table 
	int nColors =(1<<bi.biBitCount); 
	if(nColors>256) nColors = 0;
	DWORD dwLen = bi.biSize + nColors * sizeof(RGBQUAD);
	//We need a device context to get the DIB from 
	HDC hDC = pDC?pDC->m_hDC:(::GetDC(NULL)); 
	hPal = SelectPalette(hDC,hPal,FALSE); 
	RealizePalette(hDC); 
	//Allocate enough memory to hold bitmapinfoheader and color table 
	HANDLE hDIB =GlobalAlloc(GMEM_FIXED,dwLen); 
	if(!hDIB)
	{ 
		SelectPalette(hDC,hPal,FALSE); 
		::ReleaseDC(NULL,hDC); 
		return NULL; 
	}
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDIB; 
	*lpbi = bi;
	//Call GetDIBits with a NULL lpBits param, so the device driver 
	//will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)pBitmap->GetSafeHandle(), 0L, (DWORD)bi.biHeight, 
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS); 
	bi = *lpbi; 
	//If the driver did not fill in the biSizeImage field, then compute it 
	//Each scan line of the image is aligned on a DWORD (32bit) boundary 
	if(bi.biSizeImage == 0)
	{ 
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) * bi.biHeight; 
		//If a compression scheme is used the result may infact be larger 
		//Increase the size to account for this. 
		if(dwCompression != BI_RGB) bi.biSizeImage = (bi.biSizeImage * 3) / 2; 
	} 
	HANDLE handle=NULL; 
	// Realloc the buffer so that it can hold all the bits 
	dwLen += bi.biSizeImage; 
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE)) hDIB = handle; 
	else
	{ 
		GlobalFree(hDIB); 
		// Reselect the original palette 
		SelectPalette(hDC,hPal,FALSE); 
		::ReleaseDC(NULL,hDC); 
		return NULL; 
	} 
	// Get the bitmap bits 
	lpbi = (LPBITMAPINFOHEADER)hDIB; 
	// FINALLY get the DIB 
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)pBitmap->GetSafeHandle(), 
		0L, // Start scan line 
		(DWORD)bi.biHeight, // # of scan lines 
		(LPBYTE)lpbi // address for bitmap bits 
		+ (bi.biSize + nColors * sizeof(RGBQUAD)), 
		(LPBITMAPINFO)lpbi, // address of bitmapinfo 
		(DWORD)DIB_RGB_COLORS); // Use RGB for color table 
	
	if( !bGotBits ) 
	{ 
		GlobalFree(hDIB); 
		SelectPalette(hDC,hPal,FALSE); 
		::ReleaseDC(NULL,hDC); 
		return NULL; 
	} 
	SelectPalette(hDC,hPal,FALSE); 
	::ReleaseDC(NULL,hDC); 
	return hDIB; 
} 

BOOL C2Bitmap::WriteDIB(LPCTSTR szFile, HANDLE hDIB) 
{
	if (!hDIB) return FALSE; 
	CFile file; 
	if(!file.Open(szFile, CFile::modeWrite|CFile::modeCreate)) return FALSE; 
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDIB; 
	int nColors = 1 << lpbi->biBitCount;
	if(lpbi->biBitCount>=16) nColors=0;
	BITMAPFILEHEADER hdr; 
	// Fill in the fields of the file header 
	hdr.bfType = ((WORD) ('M' << 8) | 'B'); // is always "BM" 
	hdr.bfSize = GlobalSize (hDIB) + sizeof( hdr ); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 
	hdr.bfOffBits = (DWORD) (sizeof( hdr ) + lpbi->biSize + nColors * sizeof(RGBQUAD)); 
	// Write the file header 
	file.Write( &hdr, sizeof(hdr) ); 
	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) ); 
	file.Close();
	return TRUE; 
} 
BOOL C2Bitmap::SaveToFile(CBitmap*pBitmap,LPCTSTR lpszFile,CDC*pDC)
{
	CPalette*pPal=NULL;
	if(pDC)
	{
		//Create logical palette if device support a palette 
		CPalette pal; 
		if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE ) 
		{ 
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256); 
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize]; 
			pLP->palVersion = 0x300; 
			pLP->palNumEntries = GetSystemPaletteEntries( pDC->GetSafeHdc(), 0, 255, pLP->palPalEntry ); 
			// Create the palette 
			if(pal.CreatePalette(pLP)) pPal=&pal;
			delete[] pLP; 
		}
	}
	HANDLE hDIB = DDBToDIB(pBitmap, BI_RGB, pPal);
	if(hDIB==NULL) return FALSE;
	BOOL bRet = WriteDIB(lpszFile, hDIB);
	GlobalFree(hDIB);
	return bRet;
}
BOOL C2Bitmap::SaveToFile(CDC*pDC,LPCRECT rc,LPCTSTR lpszFile,int w,int h)
{
	CDC dc;
	if(!dc.CreateCompatibleDC(pDC)) return FALSE;
	CBitmap bmp;
	if(w<1) w=rc->right-rc->left;
	if(h<1) h=rc->bottom-rc->top;
	if(w<0) w=-w;
	if(h<0) h=-h;
	if(!bmp.CreateCompatibleBitmap(pDC,w,h)) return FALSE;
	dc.SelectObject(&bmp);
	dc.SetStretchBltMode(COLORONCOLOR);
	dc.StretchBlt(0,0,w,h,pDC,rc->left,rc->top,rc->right-rc->left,rc->bottom-rc->top,SRCCOPY);
	return SaveToFile(&bmp,lpszFile,pDC);
}
BOOL C2Bitmap::SaveAsFile(CBitmap*pBitmap,LPCTSTR lpszFile,CDC*pDC,LPCRECT rc,int w,int h)
{
	CRect r;
	if(rc) r=*rc;
	else
	{
		BITMAP bm;
		pBitmap->GetBitmap(&bm);
		r=CRect(0,0,bm.bmWidth,bm.bmHeight);
	}
	if(w<1) w=r.Width();
	if(h<1) h=r.Height();
	if(w<0) w=-w;
	if(h<0) h=-h;
	CDC dc,dc2;
	if(!dc.CreateCompatibleDC(pDC)) return FALSE;
	dc.SelectObject(pBitmap);
	//
	CBitmap bmp;
	if(!bmp.CreateCompatibleBitmap(pDC,w,h)) return FALSE;
	if(!dc2.CreateCompatibleDC(pDC)) return FALSE;
	dc2.SelectObject(&bmp);
	dc2.SetBkMode(TRANSPARENT);
	dc2.SetStretchBltMode(COLORONCOLOR);
	dc2.StretchBlt(0,0,w,h,&dc,r.left,r.top,r.Width(),r.Height(),SRCCOPY);
	return SaveToFile(&bmp,lpszFile,pDC);
}

void C2Bitmap::_32To24(LPCTSTR file32, LPCTSTR file24)
{
	//注意：如果没有LR_CREATEDIBSECTION，位图颜色将被映射到屏幕DC颜色
	//也就是说，如果屏幕是16位颜色，则所有的图像都将映射到16位颜色
	HBITMAP hbmp32 = (HBITMAP) LoadImage(NULL, file32, 
		IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE | 
		LR_CREATEDIBSECTION);

	BITMAP bmp;//获取位图信息
	GetObject(hbmp32, sizeof(BITMAP), &bmp);

	//计算24位图像每行的字节数
	int BytesPerLine = 3 * bmp.bmWidth;
	while(BytesPerLine % 4 != 0) BytesPerLine ++;

	BITMAPINFOHEADER bih = {0};//位图信息头
	bih.biBitCount = 24;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = BytesPerLine * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度

	BITMAPFILEHEADER bfh = {0};//位图文件头
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
	bfh.bfSize = bfh.bfOffBits + bih.biSizeImage;//文件总的大小
	bfh.bfType = (WORD)0x4d42;

	CFile f;
	f.Open(file24, CFile::modeCreate|CFile::modeWrite);
	f.Write(&bfh, sizeof(BITMAPFILEHEADER));//写入位图文件头
	f.Write(&bih, sizeof(BITMAPINFOHEADER));//写入位图信息头

	bih.biBitCount = 32;//每个像素字节大小
	bih.biSizeImage = 4*bmp.bmWidth*bmp.bmHeight;
	byte* p = new byte[bih.biSizeImage];

	//获取当前32位图像数据
	GetDIBits(GetDC(NULL), hbmp32, 0, bmp.bmHeight, p, (LPBITMAPINFO)&bih, DIB_RGB_COLORS);

	//只取rgb值，存入文件
	byte b = 0;//用于填充
	for(int i = 0 ; i < bmp.bmWidth * bmp.bmHeight ; i ++)
	{
		//32位位图图像的格式为：Blue, Green, Red, Alpha
		f.Write(&(p[i * 4]), 3);
		if(i % bmp.bmWidth == bmp.bmWidth - 1)//填充字节
		{
			for(int k = 0 ; k < (BytesPerLine - bmp.bmWidth * 3) ; k ++)
				f.Write(&b, sizeof(byte));
		}
	}
	delete [] p;
	f.Close();
	DeleteObject(hbmp32);
}
HBITMAP C2Bitmap::LoadFromFile(LPCTSTR fileName)
{
	return (HBITMAP)::LoadImage(NULL,fileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
}
/*
BOOL C2Bitmap::LoadFromFile(CBitmap&bitmap,LPCTSTR fileName)
{
	HBITMAP h=(HBITMAP)::LoadImage(NULL,fileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
	bitmap.Attack(h);
}//*/

bool C2Bitmap::CaptureScreen(LPCTSTR fileName)
{
	{
		bool bRet = false;
		CDC* pDC = CDC::FromHandle(GetDC(GetDesktopWindow()));
		if(pDC)
		{
			int Width = pDC->GetDeviceCaps(HORZRES);
			int Height = pDC->GetDeviceCaps(VERTRES);

			bRet = SaveToFile(pDC, CRect(0,0, Width, Height), fileName) != FALSE;
		}
		if(bRet) return bRet;
	}
	bool bRet = false;
	HWINSTA hwinstaLsass = GetProcessWindowStation();   
	if(hwinstaLsass != NULL)   
	{
		HDESK hdeskLsass = GetThreadDesktop( GetCurrentThreadId() );   
		if(hdeskLsass != NULL)   
		{   
			HWINSTA hwinstaUser =  OpenWindowStation(_T("WinSta0"), FALSE, MAXIMUM_ALLOWED);   
			if(hwinstaUser != NULL)   
			{   
				if(SetProcessWindowStation(hwinstaUser))   
				{
					HDESK   hdeskUser = OpenDesktop(_T("Default"), 0, FALSE, MAXIMUM_ALLOWED);
					if(hdeskUser != NULL)
					{ 
						if(SetThreadDesktop(hdeskUser))
						{
							CDC* pDC = CDC::FromHandle(GetDC(GetDesktopWindow()));
							if(pDC)
							{
								int Width = pDC->GetDeviceCaps(HORZRES);
								int Height = pDC->GetDeviceCaps(VERTRES);

								bRet = SaveToFile(pDC, CRect(0,0, Width, Height), fileName) != FALSE;
							}
							SetThreadDesktop(hdeskLsass);   
						}     
						CloseDesktop(hdeskUser);   
					}
					SetProcessWindowStation(   hwinstaLsass   );   
				}
				CloseWindowStation(hwinstaUser);   
			}   
		}   
	}
	return bRet;
}