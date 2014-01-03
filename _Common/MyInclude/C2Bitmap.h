
// C2Bitmap.h

/*
分类:SDK
//*/

#ifndef _TWen_C2Bitmap_H
#define _TWen_C2Bitmap_H

class C2Bitmap
{
public:
	static HANDLE DDBToDIB(CBitmap*pBitmap,DWORD dwCompression,CPalette*pPal,CDC*pDC=NULL);//转换CBitmap的内容(DDB)为DIB
	static BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB);//保存hDIB的内容到bmp文件
	static BOOL SaveToFile(CBitmap*pBitmap,LPCTSTR lpszFile,CDC*pDC);//保存CBitmap的内容到bmp文件
	static BOOL SaveToFile(CDC*pDC,LPCRECT rc,LPCTSTR lpszFile,int w=0,int h=0);//保存pDC的rc区域的内容为指定w和h的bmp，0表示无缩放
	static BOOL SaveAsFile(CBitmap*pBitmap,LPCTSTR lpszFile,CDC*pDC,LPCRECT rc=NULL,int w=0,int h=0);//拉伸保存CBitmap的rc区域到宽w,高h的位图文件,rc为空表示整个有效区域,w或h<1表示不拉伸宽或高
	static HBITMAP LoadFromFile(LPCTSTR fileName);//从位图文件加载位图,返回位图句柄,失败返回NULL
	static void _32To24(LPCTSTR file32, LPCTSTR file24);//如果不是32位位图，则不转换
//	static BOOL LoadFromFile(CBitmap&bitmap,LPCTSTR fileName);//谨慎使用,从位图文件加载位图,并Attach到指定的CBitmap对象

	static bool CaptureScreen(LPCTSTR fileName);
};

#endif//_TWen_C2Bitmap_H

