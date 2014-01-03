// TWenChartCtrl.cpp : 实现文件
//
#ifdef _AFX_NO_RICHEDIT_SUPPORT
#undef _AFX_NO_RICHEDIT_SUPPORT
#endif

#include "stdafx.h"
#include "TWenBMEditorCtrl.h"
#include "CStringExt.h"


void BMEditor_SaveLong(CFile& file, long l)
{
	file.Write(&l, sizeof(long));
}
void BMEditor_SaveString(CFile& file, const CString& cstring)
{
	BMEditor_SaveLong(file, cstring.GetLength());
	file.Write((LPCTSTR)cstring, (cstring.GetLength()+1)*sizeof(TCHAR));
}

void BMEditor_LoadLong(CFile& file, long& l)
{
	file.Read(&l, sizeof(long));
}
void BMEditor_LoadString(CFile& file, CString& cstring)
{
	long l;
	BMEditor_LoadLong(file, l);
	TCHAR* ps = new TCHAR[l+1];
	file.Read(ps, (l+1)*sizeof(TCHAR));
	cstring = ps;
	delete []ps;
}

//以下代码用于保存图像列表为位图
//*
HANDLE DDBToDIB(CBitmap*pBitmap,DWORD dwCompression,CPalette*pPal,CDC*pDC) 
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

BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB) 
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
BOOL SaveBitmapToFile(CBitmap*pBitmap,LPCTSTR lpszFile,CDC*pDC)
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
	HANDLE hDIB = DDBToDIB(pBitmap, BI_RGB, pPal, NULL);
	if(hDIB==NULL) return FALSE;
	BOOL bRet = WriteDIB(lpszFile, hDIB);
	GlobalFree(hDIB);
	return bRet;
}//*/
//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenFont
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenFont::CTWenFont():DefaultFont(NULL),Color(NULL),Transparent(true),BkColor(NULL)
{
}

bool CTWenFont::FromFont(CFont& font)
{
	LOGFONT lf;
	if(font.GetLogFont(&lf) == FALSE) return false;
	return FromLogFont(lf);
}
bool CTWenFont::ToFont(CFont& font) const
{
	return font.CreateFont(
		Arguments.Size,       // nHeight
		0,                         // nWidth
		Arguments.Escapement, // nEscapement
		Arguments.Orientation,// nOrientation
		Arguments.Bold?FW_BOLD:FW_NORMAL,     // nWeight
		Arguments.Italic,                     // bItalic
		Arguments.Underline,  // bUnderline
		Arguments.StrikeOut,  // cStrikeOut
		DEFAULT_CHARSET,           // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		Arguments.Name) != FALSE;      // lpszFacename
}
CFont* CTWenFont::GetFont(CFont* pFont) const
{
	if(DefaultFont) return DefaultFont;
	if(pFont) ToFont(*pFont);
	return pFont;
}
bool CTWenFont::FromLogFont(const LOGFONT& lf)
{
	Arguments.Size = lf.lfHeight/20;
	Arguments.Escapement = lf.lfEscapement;
	Arguments.Orientation = lf.lfOrientation;
	Arguments.Bold = (lf.lfWeight == FW_BOLD)?true:false;
	Arguments.Italic = lf.lfItalic?true:false;
	Arguments.Underline = lf.lfUnderline?true:false;
	Arguments.StrikeOut = lf.lfStrikeOut?true:false;
	Arguments.Name = lf.lfFaceName;
	return true;
}
void CTWenFont::ToLogFont(LOGFONT& lf) const
{
	if(DefaultFont) DefaultFont->GetLogFont(&lf);
	else
	{
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfEscapement = Arguments.Escapement;
		_tcscpy(lf.lfFaceName, Arguments.Name);
		lf.lfHeight = Arguments.Size * 20;
		lf.lfItalic = Arguments.Italic;
		lf.lfOrientation = Arguments.Orientation;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfStrikeOut = Arguments.StrikeOut;
		lf.lfUnderline = Arguments.Underline;
		lf.lfWeight = Arguments.Bold?FW_BOLD:FW_NORMAL;
		lf.lfWidth = 0;
	}
}

void CTWenFont::Save(CFile& file) const
{
	LOGFONT lf;
	ToLogFont(lf);
	file.Write(&lf, sizeof(LOGFONT));
	BMEditor_SaveLong(file, Color);
	BMEditor_SaveLong(file, Transparent);
	BMEditor_SaveLong(file, BkColor);
}
void CTWenFont::Load(CFile& file)
{
	LOGFONT lf;
	file.Read(&lf, sizeof(LOGFONT));
	FromLogFont(lf);
	long l;
	BMEditor_LoadLong(file, l);
	Color = (COLORREF)l;
	BMEditor_LoadLong(file, l);
	Transparent = l?true:false;
	BMEditor_LoadLong(file, l);
	BkColor = (COLORREF)l;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenPen
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenPen::CTWenPen():DefaultPen(NULL)
{
}

bool CTWenPen::FromPen(CPen& pen)
{
	LOGPEN lp;
	if(pen.GetLogPen(&lp) == FALSE) return false;
	Arguments.Color = lp.lopnColor;
	Arguments.Style = lp.lopnStyle;
	Arguments.Width = lp.lopnWidth.x;
	return true;
}
bool CTWenPen::ToPen(CPen& pen) const
{
	return pen.CreatePen(Arguments.Style, Arguments.Width, Arguments.Color) != FALSE;
}
CPen* CTWenPen::GetPen(CPen* pPen) const
{
	if(DefaultPen) return DefaultPen;
	if(pPen) ToPen(*pPen);
	return pPen;
}

void CTWenPen::Save(CFile& file) const
{
	file.Write(&Arguments, sizeof(structPenArguments));
}
void CTWenPen::Load(CFile& file)
{
	file.Read(&Arguments, sizeof(structPenArguments));
}
//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenBrush
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBrush::CTWenBrush():DefaultBrush(NULL)
{

}

bool CTWenBrush::FromBrush(CBrush& Brush)
{
	LOGBRUSH lb;
	if(Brush.GetLogBrush(&lb) == FALSE) return false;
	Arguments.Color = lb.lbColor;
	Arguments.Style = lb.lbStyle;
	Arguments.Hatch = lb.lbHatch;
	return true;
}
bool CTWenBrush::ToBrush(CBrush& Brush) const
{
	LOGBRUSH lb;
	lb.lbColor = Arguments.Color;
	lb.lbStyle = Arguments.Style;
	lb.lbHatch = Arguments.Hatch;
	return Brush.CreateBrushIndirect(&lb) == TRUE;
}
CBrush* CTWenBrush::GetBrush(CBrush* Brush) const
{
	if(DefaultBrush) return DefaultBrush;
	if(Brush) ToBrush(*Brush);
	return Brush;
}

void CTWenBrush::Save(CFile& file) const
{
	file.Write(&Arguments, sizeof(structBrushArguments));
}
void CTWenBrush::Load(CFile& file)
{
	file.Read(&Arguments, sizeof(structBrushArguments));
}
//////////////////////////////////////////////////////////////////////////////////////////////
// structSelectDCFont
//////////////////////////////////////////////////////////////////////////////////////////////
structSelectDCFont::structSelectDCFont(CDC* pDC, const CTWenFont& ChartFont, CFont& font):m_pDC(pDC),m_pOldFont(NULL)
{
	if(m_pDC)
	{
		m_pOldFont = m_pDC->SelectObject(ChartFont.GetFont(&font));
		m_pDC->SetTextColor(ChartFont.Color);
		if(ChartFont.Transparent) m_pDC->SetBkMode(TRANSPARENT);
		else
		{
			m_pDC->SetBkMode(OPAQUE);
			m_pDC->SetBkColor(ChartFont.BkColor);
		}
	}
}
structSelectDCFont::~structSelectDCFont()
{
	if(m_pDC) m_pDC->SelectObject(m_pOldFont);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenGraphicsSerie
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenGraphicsSerie::CTWenGraphicsSerie(CTWenBMEditorCtrl* parent, CTWenGraphicsSerie::enumSerieType serieType):Parent(parent),
SerieType(serieType),Visible(true),Color(NULL),FillType(CTWenGraphicsSerie::FT_NOTFILL),PositionType(CTWenGraphicsSerie::POSITION_LEFTTOP),
Rect(0,0, 150,120),CenterPoint(0,0)
{
	static DWORD SerieID = 0;
	SerieID++;
	ID = SerieID;
}

void CTWenGraphicsSerie::SetRect(const RECT& rc)
{

}
void CTWenGraphicsSerie::SetCenterPoint(long x, long y)
{

}
LPCTSTR CTWenGraphicsSerie::GetSerieTypeName() const
{
	switch(SerieType)
	{
	case CTWenGraphicsSerie::ST_TEXT:return _T("文本");break;
	case CTWenGraphicsSerie::ST_RECT:return _T("矩形");break;
	case CTWenGraphicsSerie::ST_ROUNDRECT:return _T("圆角矩形");break;
	case CTWenGraphicsSerie::ST_ELLIPSE:return _T("(椭)圆");break;
	case CTWenGraphicsSerie::ST_ARROW:return _T("箭头");break;
	default:return _T("未知");
	}
}

void CTWenGraphicsSerie::Save(CFile& file) const
{
	BMEditor_SaveString(file, Name);
	BMEditor_SaveLong(file, Visible);
	BMEditor_SaveLong(file, Color);
	BMEditor_SaveLong(file, FillType);
	BMEditor_SaveLong(file, PositionType);
	BMEditor_SaveLong(file, SerieType);
	BMEditor_SaveLong(file, Rect.left);
	BMEditor_SaveLong(file, Rect.top);
	BMEditor_SaveLong(file, Rect.right);
	BMEditor_SaveLong(file, Rect.bottom);
	BMEditor_SaveLong(file, CenterPoint.x);
	BMEditor_SaveLong(file, CenterPoint.y);
}
void CTWenGraphicsSerie::Load(CFile& file)
{
	BMEditor_LoadString(file, Name);
	long l;
	BMEditor_LoadLong(file, l);
	Visible = l?true:false;
	BMEditor_LoadLong(file, l);
	Color = (DWORD)l;
	BMEditor_LoadLong(file, l);
	FillType = (enumFillType)l;
	BMEditor_LoadLong(file, l);
	PositionType = (enumPositionType)l;
	BMEditor_LoadLong(file, l);
	SerieType = (enumSerieType)l;
	BMEditor_LoadLong(file, l);
	Rect.left = l;
	BMEditor_LoadLong(file, l);
	Rect.top = l;
	BMEditor_LoadLong(file, l);
	Rect.right = l;
	BMEditor_LoadLong(file, l);
	Rect.bottom = l;
	BMEditor_LoadLong(file, l);
	CenterPoint.x = l;
	BMEditor_LoadLong(file, l);
	CenterPoint.y = l;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenTextSerie
//////////////////////////////////////////////////////////////////////////////////////////////
void CTWenTextSerie::Save(CFile& file) const
{
	CTWenGraphicsSerie::Save(file);
	BMEditor_SaveLong(file, DrawType);
	BMEditor_SaveString(file, Text);
	Font.Save(file);
}
void CTWenTextSerie::Load(CFile& file)
{
	CTWenGraphicsSerie::Load(file);
	long l;
	BMEditor_LoadLong(file, l);
	DrawType = (DWORD)l;
	BMEditor_LoadString(file, Text);
	Font.Load(file);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenRectSerie
//////////////////////////////////////////////////////////////////////////////////////////////
void CTWenRectSerie::Save(CFile& file) const
{
	CTWenGraphicsSerie::Save(file);
	BorderPen.Save(file);
}
void CTWenRectSerie::Load(CFile& file)
{
	CTWenGraphicsSerie::Load(file);
	BorderPen.Load(file);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenRoundRectSerie
//////////////////////////////////////////////////////////////////////////////////////////////
void CTWenRoundRectSerie::Save(CFile& file) const
{
	CTWenGraphicsSerie::Save(file);
	Pen.Save(file);
	Brush.Save(file);
	BMEditor_SaveLong(file, Round.x);
	BMEditor_SaveLong(file, Round.y);
}
void CTWenRoundRectSerie::Load(CFile& file)
{
	CTWenGraphicsSerie::Load(file);
	Pen.Load(file);
	Brush.Load(file);
	long l;
	BMEditor_LoadLong(file, l);
	Round.x = l;
	BMEditor_LoadLong(file, l);
	Round.y = l;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenEllipseSerie
//////////////////////////////////////////////////////////////////////////////////////////////
void CTWenEllipseSerie::Save(CFile& file) const
{
	CTWenGraphicsSerie::Save(file);
	Pen.Save(file);
	Brush.Save(file);
}
void CTWenEllipseSerie::Load(CFile& file)
{
	CTWenGraphicsSerie::Load(file);
	Pen.Load(file);
	Brush.Load(file);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenArrowSerie
//////////////////////////////////////////////////////////////////////////////////////////////
void CTWenArrowSerie::Save(CFile& file) const
{
	CTWenGraphicsSerie::Save(file);
	BMEditor_SaveLong(file, Direction);
	Pen.Save(file);
	BMEditor_SaveLong(file, Length);
}
void CTWenArrowSerie::Load(CFile& file)
{
	CTWenGraphicsSerie::Load(file);
	long l;
	BMEditor_LoadLong(file, l);
	Direction = (enumArrowDirection)l;
	Pen.Load(file);
	BMEditor_LoadLong(file, Length);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTWenBMEditorCtrl
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenPropertyDlg::CTWenPropertyDlg(CWnd* parent, CTWenGraphicsSerie* pSerie):CTemplateDlg(parent),Serie(pSerie)
{
	SetTitle(_T("属性"));
	CString sID, sName(Serie->Name), sSerieType, sShow, sColor, sFillType, sPositionType, sl, st, sw, sh, sx, sy;
	sID.Format(_T("%u"), Serie->GetID());
	sSerieType = pSerie->GetSerieTypeName();
	sShow = Serie->Visible ? _T("1") : _T("0");
	Color = Serie->Color;
	sColor.Format(_T("颜色(%X,%X,%X)"), GetRValue(Color), GetGValue(Color), GetBValue(Color));
	sFillType.Format(_T("%d|实心:%d,空心:%d"), Serie->FillType, CTWenGraphicsSerie::FT_FILL, CTWenGraphicsSerie::FT_NOTFILL);
	sPositionType.Format(_T("%d|左上角:%d,中心点:%d"), Serie->PositionType, CTWenGraphicsSerie::POSITION_LEFTTOP, CTWenGraphicsSerie::POSITION_CENTER);

	sl.Format(_T("%d"), Serie->Rect.left);
	st.Format(_T("%d"), Serie->Rect.top);
	sw.Format(_T("%d"), Serie->Rect.Width());
	sh.Format(_T("%d"), Serie->Rect.Height());
	sx.Format(_T("%d"), Serie->CenterPoint.x);
	sy.Format(_T("%d"), Serie->CenterPoint.y);

	SetCtrlInfo(13,
		_T("ID"),CTemplateDlg::CT_Edit,WS_BORDER|ES_READONLY,NULL,CTemplateDlg::VT_Text,(LPCTSTR)sID,
		_T("名称"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Text,(LPCTSTR)sName,
		_T("类型"),CTemplateDlg::CT_Edit,WS_BORDER|ES_READONLY,NULL,CTemplateDlg::VT_Text,(LPCTSTR)sSerieType,
		_T(""),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,sShow+_T("|可见"),
		(LPCTSTR)sColor,CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置颜色|1234567"),
		_T("填充类型"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sFillType,
		_T("位置基准"),CTemplateDlg::CT_ComboBoxDropList,WS_DISABLED,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sPositionType,
		_T("矩形区域(左):"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Long,(LPCTSTR)sl,
		_T("矩形区域(上):"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Long,(LPCTSTR)st,
		_T("矩形宽度:"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_DWORD,(LPCTSTR)sw,
		_T("矩形高度:"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_DWORD,(LPCTSTR)sh,
		_T("中心点(x):"),CTemplateDlg::CT_Edit,WS_BORDER|ES_READONLY,NULL,CTemplateDlg::VT_Long,(LPCTSTR)sx,
		_T("中心点(y):"),CTemplateDlg::CT_Edit,WS_BORDER|ES_READONLY,NULL,CTemplateDlg::VT_Long,(LPCTSTR)sy
		);
}
BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenPropertyDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenPropertyDlg::OnButtonColor)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenPropertyDlg::OnButtonColor()
{
	if(!Serie) return;
	if(CTWenBMEditorCtrl::ChartColor(Color, this))
	{
		CString sColor;
		sColor.Format(_T("颜色(%X,%X,%X)"), GetRValue(Color), GetGValue(Color), GetBValue(Color));
		m_pStatics[4].SetWindowText(sColor);
	}
}
void CTWenBMEditorCtrl::CTWenPropertyDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	CString v[13];
	CStringExt::Split(m_strValue,m_strSep, 13, v);
	Serie->Name = v[1];
	Serie->Visible = _tstol(v[3])?true:false;
	Serie->Color = Color;
	Serie->FillType = (CTWenGraphicsSerie::enumFillType)_tstol(v[5]);
	Serie->PositionType = (CTWenGraphicsSerie::enumPositionType)_tstol(v[6]);
	Serie->Rect = CRect(_tstol(v[7]), _tstol(v[8]), _tstol(v[9]), _tstol(v[10]));
	Serie->Rect.right += Serie->Rect.left;
	Serie->Rect.bottom += Serie->Rect.top;
	Serie->CenterPoint = CPoint(_tstol(v[11]), _tstol(v[12]));
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenTextDlg::CTWenTextDlg(CWnd* parent, CTWenTextSerie* pText):CTemplateDlg(parent),Text(pText),AutoRect(true)
{
	SetTitle(_T("高级属性"));
	CString sText, sShow(_T("1")), sAuto, sX, sY, sS;
	sText = Text->Text;
	sShow = Text->Visible ? _T("1") : _T("0");
	sAuto = AutoRect ? _T("1") : _T("0");
	sX.Format(_T("%d|DT_LEFT:%d,DT_CENTER:%d,DT_RIGHT:%d"), Text->DrawType & (DT_LEFT | DT_CENTER | DT_RIGHT), DT_LEFT, DT_CENTER, DT_RIGHT);
	sY.Format(_T("%d|DT_TOP:%d,DT_VCENTER:%d,DT_BOTTOM:%d"), Text->DrawType & (DT_TOP | DT_VCENTER | DT_BOTTOM), DT_TOP, DT_VCENTER, DT_BOTTOM);
	sS = (Text->DrawType & DT_SINGLELINE) ? _T("1") : _T("0");

	SetCtrlInfo(7,
		_T("文本"),CTemplateDlg::CT_Edit,WS_BORDER|ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,NULL,CTemplateDlg::VT_Text|CTemplateDlg::VT_NoZero,sText,
		_T(""),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,sShow+_T("|可见"),
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置字体|1234567"),
		_T(""),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,sAuto+_T("|自动设置文本区域"),
		_T("x对齐"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sX,
		_T("y对齐"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sY,
		_T(""),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,sS+_T("|单行绘画文本")
		);
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenTextDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenTextDlg::OnButtonFont)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenTextDlg::OnButtonFont()
{
	if(!Text) return;
	CTWenBMEditorCtrl::ChartFont(Text->Font, this);
}
void CTWenBMEditorCtrl::CTWenTextDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Text) return;
	CString v[7];
	CStringExt::Split(m_strValue,m_strSep, 7, v);
	Text->Text = v[0];
	Text->Visible = _tstol(v[1])?true:false;
	AutoRect = _tstol(v[3])?true:false;
	bool bSingleline = _tstol(v[6]) ? true:false;
	Text->DrawType = ((DWORD)_tstol(v[4])) | ((DWORD)_tstol(v[5])) | (bSingleline?DT_SINGLELINE:NULL);
	if(AutoRect)
	{
		CRect rcText(0,0,0,0);
		CFont font;
		CDC* pDC = GetParent()->GetDC();
		structSelectDCFont _selectFont(pDC, Text->Font, font);
		pDC->DrawText(Text->Text, rcText, DT_CALCRECT|(bSingleline?DT_SINGLELINE:NULL));
		Text->Rect.right = Text->Rect.left + rcText.Width();
		Text->Rect.bottom = Text->Rect.top +rcText.Height();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenPenDlg::CTWenPenDlg(CWnd* parent, CTWenPen* pPen):CTemplateDlg(parent),Pen(pPen)
{
	SetTitle(_T("画笔属性"));
	CString sType, sWidth;
	sType.Format(_T("%d|PS_SOLID:%d,PS_DASH:%d,PS_DOT:%d,PS_DASHDOT:%d,PS_DASHDOTDOT:%d,PS_NULL:%d,PS_INSIDEFRAME:%d,PS_USERSTYLE:%d,PS_ALTERNATE:%d"), Pen->Arguments.Style, PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT, PS_NULL, PS_INSIDEFRAME, PS_USERSTYLE, PS_ALTERNATE);
	sWidth.Format(_T("%d"), Pen->Arguments.Width);

	Color = Pen->Arguments.Color;
	CString sColor;
	sColor.Format(_T("颜色(%X,%X,%X)"), GetRValue(Color), GetGValue(Color), GetBValue(Color));

	SetCtrlInfo(3,
		_T("类型"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sType,
		_T("宽度"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_DWORD,(LPCTSTR)sWidth,
		(LPCTSTR)sColor,CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置颜色|1234567")
		);
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenPenDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenPenDlg::OnButtonColor)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenPenDlg::OnButtonColor()
{
	if(!Pen) return;
	if(CTWenBMEditorCtrl::ChartColor(Color, this))
	{
		CString sColor;
		sColor.Format(_T("颜色(%X,%X,%X)"), GetRValue(Color), GetGValue(Color), GetBValue(Color));
		m_pStatics[2].SetWindowText(sColor);
	}
}
void CTWenBMEditorCtrl::CTWenPenDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Pen) return;
	CString v[3];
	CStringExt::Split(m_strValue,m_strSep, 3, v);
	Pen->Arguments.Style = _tstol(v[0]);
	Pen->Arguments.Width = _tstol(v[1]);
	Pen->Arguments.Color = Color;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenBrushDlg::CTWenBrushDlg(CWnd* parent, CTWenBrush* pBrush):CTemplateDlg(parent),Brush(pBrush)
{
	SetTitle(_T("画刷属性"));
	CString sType, sHatch;
	sType.Format(_T("%d|BS_SOLID:%d,BS_NULL:%d,BS_HOLLOW:%d,BS_HATCHED:%d,BS_PATTERN:%d,BS_INDEXED:%d,BS_DIBPATTERN:%d,BS_DIBPATTERNPT:%d,BS_PATTERN8X8:%d,BS_DIBPATTERN8X8:%d,BS_MONOPATTERN:%d"), Brush->Arguments.Style, BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_INDEXED, BS_DIBPATTERN, BS_DIBPATTERNPT, BS_PATTERN8X8, BS_DIBPATTERN8X8, BS_MONOPATTERN);
	sHatch.Format(_T("%d|HS_HORIZONTAL:%d,HS_VERTICAL:%d,HS_FDIAGONAL:%d,HS_BDIAGONAL:%d,HS_CROSS:%d,HS_DIAGCROSS:%d"), Brush->Arguments.Hatch, HS_HORIZONTAL, HS_VERTICAL, HS_FDIAGONAL, HS_BDIAGONAL, HS_CROSS, HS_DIAGCROSS);
	Color = Brush->Arguments.Color;
	CString sColor;
	sColor.Format(_T("颜色(%X,%X,%X)"), GetRValue(Color), GetGValue(Color), GetBValue(Color));

	SetCtrlInfo(3,
		_T("类型"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sType,
		(LPCTSTR)sColor,CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置颜色|1234567"),
		_T("Hatch"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sHatch
		);
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenBrushDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenBrushDlg::OnButtonColor)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenBrushDlg::OnButtonColor()
{
	if(!Brush) return;
	if(CTWenBMEditorCtrl::ChartColor(Color, this))
	{
		CString sColor;
		sColor.Format(_T("颜色(%X,%X,%X)"), GetRValue(Color), GetGValue(Color), GetBValue(Color));
		m_pStatics[1].SetWindowText(sColor);
	}
}
void CTWenBMEditorCtrl::CTWenBrushDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Brush) return;
	CString v[3];
	CStringExt::Split(m_strValue,m_strSep, 3, v);
	Brush->Arguments.Style = _tstol(v[0]);
	Brush->Arguments.Color = Color;
	Brush->Arguments.Hatch = _tstol(v[2]);
}
//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenRoundRectDlg::CTWenRoundRectDlg(CWnd* parent, CTWenRoundRectSerie* pSerie):CTemplateDlg(parent),Serie(pSerie)
{
	CString sx, sy;
	sx.Format(_T("%d"), Serie->Round.x);
	sy.Format(_T("%d"), Serie->Round.y);


	SetTitle(_T("高级属性"));
	SetCtrlInfo(4,
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置画笔|1234567"),
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置画刷|1234568"),
		_T("Round(x):"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Long,(LPCTSTR)sx,
		_T("Round(y):"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Long,(LPCTSTR)sy
		);
	Pen = Serie->Pen;
	Brush = Serie->Brush;
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenRoundRectDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenRoundRectDlg::OnButtonPen)
	ON_BN_CLICKED(1234568, &CTWenBMEditorCtrl::CTWenRoundRectDlg::OnButtonBrush)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenRoundRectDlg::OnButtonPen()
{
	if(!Serie) return;
	CTWenPenDlg dlg(this, &Pen);
	dlg.DoModal();
}
void CTWenBMEditorCtrl::CTWenRoundRectDlg::OnButtonBrush()
{
	if(!Serie) return;
	CTWenBrushDlg dlg(this, &Brush);
	dlg.DoModal();
}
void CTWenBMEditorCtrl::CTWenRoundRectDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Serie) return;
	CString v[4];
	Serie->Pen = Pen;
	Serie->Brush = Brush;
	CStringExt::Split(m_strValue,m_strSep, 4, v);
	Serie->Round.x = _tstol(v[2]);
	Serie->Round.y = _tstol(v[3]);
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenEllipseDlg::CTWenEllipseDlg(CWnd* parent, CTWenEllipseSerie* pSerie):CTemplateDlg(parent),Serie(pSerie)
{
	SetTitle(_T("高级属性"));
	SetCtrlInfo(2,
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置画笔|1234567"),
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置画刷|1234568")
		);
	Pen = Serie->Pen;
	Brush = Serie->Brush;
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenEllipseDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenEllipseDlg::OnButtonPen)
	ON_BN_CLICKED(1234568, &CTWenBMEditorCtrl::CTWenEllipseDlg::OnButtonBrush)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenEllipseDlg::OnButtonPen()
{
	if(!Serie) return;
	CTWenPenDlg dlg(this, &Pen);
	dlg.DoModal();
}
void CTWenBMEditorCtrl::CTWenEllipseDlg::OnButtonBrush()
{
	if(!Serie) return;
	CTWenBrushDlg dlg(this, &Brush);
	dlg.DoModal();
}
void CTWenBMEditorCtrl::CTWenEllipseDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Serie) return;
	Serie->Pen = Pen;
	Serie->Brush = Brush;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CTWenBMEditorCtrl::CTWenArrowDlg::CTWenArrowDlg(CWnd* parent, CTWenArrowSerie* pSerie):CTemplateDlg(parent),Serie(pSerie)
{
	CString sDirection, sLength;
	sDirection.Format(_T("%d|向左:%d,向上:%d,向右:%d,向下:%d"), Serie->Direction, CTWenArrowSerie::DIRECTION_LEFT, CTWenArrowSerie::DIRECTION_UP, CTWenArrowSerie::DIRECTION_RIGHT, CTWenArrowSerie::DIRECTION_DOWN);
	sLength.Format(_T("%d"), Serie->Length);

	SetTitle(_T("高级属性"));
	SetCtrlInfo(3,
		_T("方向"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,(LPCTSTR)sDirection,
		_T("长度"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Long,(LPCTSTR)sLength,
		_T(""),CTemplateDlg::CT_Button,NULL,NULL,NULL,_T("设置画笔|1234567")
		);
	Pen = Serie->Pen;
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl::CTWenArrowDlg, CTemplateDlg)
	ON_BN_CLICKED(1234567, &CTWenBMEditorCtrl::CTWenArrowDlg::OnButtonPen)
END_MESSAGE_MAP()

void CTWenBMEditorCtrl::CTWenArrowDlg::OnButtonPen()
{
	if(!Serie) return;
	CTWenPenDlg dlg(this, &Pen);
	dlg.DoModal();
}
void CTWenBMEditorCtrl::CTWenArrowDlg::OnOK()
{
	CTemplateDlg::OnOK();
	if(m_bError) return;
	if(!Serie) return;
	CString v[3];
	Serie->Pen = Pen;
	CStringExt::Split(m_strValue,m_strSep, 3, v);
	Serie->Direction = (CTWenArrowSerie::enumArrowDirection)_tstol(v[0]);
	Serie->Length = _tstol(v[1]);
}

//////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTWenBMEditorCtrl, CWnd)

CTWenBMEditorCtrl::CTWenBMEditorCtrl():CurrentSelect(NULL)
{
}

CTWenBMEditorCtrl::~CTWenBMEditorCtrl()
{
	DeleteSeries();
}

void CTWenBMEditorCtrl::Create(CWnd* parent, const CRect& rc)
{
	CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rc, parent, NULL);
}
void CTWenBMEditorCtrl::Redraw(bool bRebuilding)
{
	if(bRebuilding) Rebuild();
	Invalidate();
}

void CTWenBMEditorCtrl::OnInitMenu(CMenu& Menu)
{
	DWORD MF_CurSel = CurrentSelect?MF_ENABLED:MF_GRAYED|MF_DISABLED;

	DWORD gSeries = (DWORD)Series.GetCount();
	if(gSeries > 0)
	{
		for(DWORD i=0; i<gSeries; i++)
		{
			CTWenGraphicsSerie* pSerie = Series.GetAt(i);
			CString t;
			t.Format(_T("%u(%s):%s"), pSerie->GetID(), pSerie->GetSerieTypeName(), pSerie->Name);
			Menu.AppendMenu(MF_STRING|MF_ENABLED|(pSerie == CurrentSelect ? MF_CHECKED : NULL), ID_MENU_SERIEFROM + i, t);
		}
		Menu.AppendMenu(MF_SEPARATOR);
	}

	Menu.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SERIE_NEWTEXT, _T("(&T)添加文本"));
	Menu.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SERIE_NEWRECT, _T("(&R)添加矩形"));
	Menu.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SERIE_NEWROUNDRECT, _T("(&O)添加圆角矩形"));
	Menu.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SERIE_NEWELLIPSE, _T("(&E)添加(椭)圆"));
	Menu.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SERIE_NEWARROW, _T("(&A)添加箭头"));

	Menu.AppendMenu(MF_SEPARATOR);

	Menu.AppendMenu(MF_STRING|MF_CurSel, ID_MENU_SERIE_DELETE, _T("(&D)删除"));
	
	Menu.AppendMenu(MF_SEPARATOR);

	CMenu menuTemp;

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_LOAD_BITMAP, _T("(&B)位图"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_LOAD_EDITORINDEX, _T("(&I)索引"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_LOAD_EDITORDOCUMENT, _T("(&D)文档"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&L)加载"));
	menuTemp.DestroyMenu();

	menuTemp.CreateMenu();
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SAVE_BITMAP, _T("(&B)位图"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SAVE_EDITORINDEX, _T("(&I)索引"));
	menuTemp.AppendMenu(MF_STRING|MF_ENABLED, ID_MENU_SAVE_EDITORDOCUMENT, _T("(&D)文档"));
	Menu.AppendMenu(MF_POPUP, (UINT_PTR)menuTemp.m_hMenu, _T("(&S)保存为"));
	menuTemp.DestroyMenu();

	Menu.AppendMenu(MF_SEPARATOR);

	Menu.AppendMenu(MF_STRING|MF_CurSel, ID_MENU_PROPERTY_BASE, _T("(&P)基本属性"));
	Menu.AppendMenu(MF_STRING|MF_CurSel, ID_MENU_PROPERTY_SPECIAL, _T("(&V)高级属性"));

	Menu.AppendMenu(MF_SEPARATOR);

	DWORD MF_Bitmap = SrcImage.m_hObject?MF_ENABLED:MF_GRAYED|MF_DISABLED;
	Menu.AppendMenu(MF_STRING|MF_Bitmap, ID_MENU_PROPERTY_FITIMAGE, _T("(&F)适应位图"));
	
}
bool GetOpenFile(CString&fpn,LPCTSTR fileType,LPCTSTR text,CWnd*parent)
{
	CString type;
	CString tmp(fileType);
	if(tmp.Find(_T('|'))>-1) type.Format(_T("%s||"),fileType);
	else type.Format(_T("%s文件|*.%s||"),fileType,fileType);
	CFileDialog mFileDlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_EXPLORER,type,parent);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000)// Windows NT
		mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else// if (dwWindowsMajorVersion < 4)// Win32s
		//else// Windows Me/98/95
		mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle=text;
	if(mFileDlg.DoModal()==IDOK)
	{
		fpn=mFileDlg.GetPathName();
		return true;
	}
	return false;
}
bool GetSaveFile(CString&fpn,LPCTSTR fileType,LPCTSTR text, LPCTSTR defaultExt,LPCTSTR defaultFN,CWnd*parent)
{
	CString type;
	CString tmp(fileType);
	if(tmp.Find(_T('|'))>-1) type.Format(_T("%s||"),fileType);
	else type.Format(_T("%s文件|*.%s||"),fileType,fileType);
	CFileDialog mFileDlg(FALSE,defaultExt,defaultFN,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,type,parent);
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000) mFileDlg.m_ofn.lStructSize=88;//显示新的文件对话框
	else mFileDlg.m_ofn.lStructSize=76;//显示老的文件对话框
	mFileDlg.m_ofn.lpstrTitle=text;
	if(mFileDlg.DoModal()==IDOK)
	{
		fpn=mFileDlg.GetPathName();
		return true;
	}
	return false;
}
void CTWenBMEditorCtrl::OnMenu(DWORD dwMenuID)
{
	if(dwMenuID >= ID_MENU_SERIEFROM && dwMenuID <= ID_MENU_SERIETO) SetCurrentSelectByIndex(dwMenuID - ID_MENU_SERIEFROM);
	else
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		switch(dwMenuID)
		{
		case ID_MENU_SERIE_NEWTEXT:
			{
				CTWenTextSerie* pSerie = new CTWenTextSerie(this);
				pSerie->Rect = CRect(pt.x, pt.y, pt.x+pSerie->Rect.Width(), pt.y+pSerie->Rect.Height());
				CTWenPropertyDlg dlg(this, pSerie);
				if(dlg.DoModal() != IDOK)
				{
					delete dlg.Serie;
					return;
				}
				pSerie->Text = _T("请更改文本内容");
				AddSerie(dlg.Serie);
			}
			break;
		case ID_MENU_SERIE_NEWRECT:
			{
				CTWenRectSerie* pSerie = new CTWenRectSerie(this);
				pSerie->Rect = CRect(pt.x, pt.y, pt.x+pSerie->Rect.Width(), pt.y+pSerie->Rect.Height());
				CTWenPropertyDlg dlg(this, pSerie);
				if(dlg.DoModal() != IDOK)
				{
					delete dlg.Serie;
					return;
				}
				AddSerie(dlg.Serie);
			}
			break;
		case ID_MENU_SERIE_NEWROUNDRECT:
			{
				CTWenRoundRectSerie* pSerie = new CTWenRoundRectSerie(this);
				pSerie->Rect = CRect(pt.x, pt.y, pt.x+pSerie->Rect.Width(), pt.y+pSerie->Rect.Height());
				CTWenPropertyDlg dlg(this, pSerie);
				if(dlg.DoModal() != IDOK)
				{
					delete dlg.Serie;
					return;
				}
				AddSerie(dlg.Serie);
			}
			break;
		case ID_MENU_SERIE_NEWELLIPSE:
			{
				CTWenEllipseSerie* pSerie = new CTWenEllipseSerie(this);
				pSerie->Rect = CRect(pt.x, pt.y, pt.x+pSerie->Rect.Width(), pt.y+pSerie->Rect.Height());
				CTWenPropertyDlg dlg(this, pSerie);
				if(dlg.DoModal() != IDOK)
				{
					delete dlg.Serie;
					return;
				}
				AddSerie(dlg.Serie);
			}
			break;
		case ID_MENU_SERIE_NEWARROW:
			{
				CTWenArrowSerie* pSerie = new CTWenArrowSerie(this);
				pSerie->Rect = CRect(pt.x, pt.y, pt.x+pSerie->Rect.Width(), pt.y+pSerie->Rect.Height());
				CTWenPropertyDlg dlg(this, pSerie);
				if(dlg.DoModal() != IDOK)
				{
					delete dlg.Serie;
					return;
				}
				AddSerie(dlg.Serie);
			}
			break;

		case ID_MENU_SERIE_DELETE:
			if(CurrentSelect)
			{
				DeleteSerieByID(CurrentSelect->GetID());
				CurrentSelect = NULL;
			}
			break;
		case ID_MENU_LOAD_BITMAP:
			{
				if(!GetOpenFile(SrcImageFile, _T("bmp"), _T("打开位图文件"), this)) return;
				RefreshSrcImage();
			}
			break;
		case ID_MENU_LOAD_EDITORINDEX:
			{
				CString fileName;
				if(!GetOpenFile(fileName, _T("bmei"), _T("打开索引文件"), this)) return;
				CFile file;
				if(!file.Open(fileName, CFile::modeRead))
				{
					MessageBox(fileName, _T("无法打开文件"), MB_OK|MB_ICONERROR);
					return;
				}
				LoadIndex(file);
				RefreshSrcImage();
				file.Close();
			}
			break;
		case ID_MENU_LOAD_EDITORDOCUMENT:
			{
				CString fileName;
				if(!GetOpenFile(fileName, _T("bmed"), _T("打开索引文件"), this)) return;
				CFile file;
				if(!file.Open(fileName, CFile::modeRead))
				{
					MessageBox(fileName, _T("无法打开文件"), MB_OK|MB_ICONERROR);
					return;
				}
				LoadIndex(file);
				if(SrcImageFile.GetLength())
				{
					TCHAR tp[10240];
					GetTempPath(10240, tp);
					SrcImageFile = tp;
					SrcImageFile = SrcImageFile + _T("\\") + CTime::GetCurrentTime().Format(_T("%Y%m%dT%H%M%S.bmp"));
					CFile f;
					if(!f.Open(SrcImageFile, CFile::modeWrite|CFile::modeCreate)) MessageBox(SrcImageFile, _T("无法打开文件"), MB_OK|MB_ICONERROR);
					else
					{
						long l;
						BMEditor_LoadLong(file, l);
						BYTE* pb = new BYTE[l];
						file.Read(pb,l);
						f.Write(pb,l);
						delete []pb;
						f.Close();
					}
				}
				RefreshSrcImage();
				file.Close();
			}
			break;
		case ID_MENU_SAVE_BITMAP:
			{
				if(!ResultImage.m_hObject) return;
				CString fileName;
				if(!GetSaveFile(fileName, _T("bmp"), _T("保存为位图文件"), _T("bmp"), _T(""), this)) return;
				SaveBitmapToFile(&ResultImage, fileName, GetDC());
			}
			break;
		case ID_MENU_SAVE_EDITORINDEX:
			{
				CString fileName;
				if(!GetSaveFile(fileName, _T("bmei"), _T("保存为索引文件"), _T("bmei"), _T(""), this)) return;
				CFile file;
				if(!file.Open(fileName, CFile::modeCreate|CFile::modeWrite))
				{
					MessageBox(fileName, _T("无法打开文件"), MB_OK|MB_ICONERROR);
					return;
				}
				SaveIndex(file);
				file.Close();
				return;
			}
			break;
		case ID_MENU_SAVE_EDITORDOCUMENT:
			{
				CString fileName;
				if(!GetSaveFile(fileName, _T("bmed"), _T("保存为索引文件"), _T("bmed"), _T(""), this)) return;
				CFile file;
				if(!file.Open(fileName, CFile::modeCreate|CFile::modeWrite))
				{
					MessageBox(fileName, _T("无法打开文件"), MB_OK|MB_ICONERROR);
					return;
				}
				SaveIndex(file);
				if(SrcImageFile.GetLength())
				{
					CFile f;
					if(!f.Open(SrcImageFile, CFile::modeRead)) MessageBox(SrcImageFile, _T("无法打开文件"), MB_OK|MB_ICONERROR);
					else
					{
						long l = (long)f.GetLength();
						BMEditor_SaveLong(file, l);
						BYTE* pb = new BYTE[l];
						f.Read(pb,l);
						file.Write(pb,l);
						delete []pb;
						f.Close();
					}
				}
				file.Close();
				return;
			}
			break;
		case ID_MENU_PROPERTY_BASE:
			if(CurrentSelect)
			{
				CTWenPropertyDlg dlg(this, CurrentSelect);
				if(dlg.DoModal() != IDOK) return;
			}
			break;
		case ID_MENU_PROPERTY_SPECIAL:
			if(CurrentSelect)
			{
				bool bModified = false;
				switch(CurrentSelect->SerieType)
				{
				case CTWenGraphicsSerie::ST_TEXT:bModified = SpecialProperty_TextSerie((CTWenTextSerie*)CurrentSelect);break;
				case CTWenGraphicsSerie::ST_RECT:bModified = SpecialProperty_RectSerie((CTWenRectSerie*)CurrentSelect);break;
				case CTWenGraphicsSerie::ST_ROUNDRECT:bModified = SpecialProperty_RoundRectSerie((CTWenRoundRectSerie*)CurrentSelect);break;
				case CTWenGraphicsSerie::ST_ELLIPSE:bModified = SpecialProperty_EllipseSerie((CTWenEllipseSerie*)CurrentSelect);break;
				case CTWenGraphicsSerie::ST_ARROW:bModified = SpecialProperty_ArrowSerie((CTWenArrowSerie*)CurrentSelect);break;
				}
				if(bModified == false) return;
			}
			break;
		case ID_MENU_PROPERTY_FITIMAGE:
			{
				if(!SrcImage.m_hObject) return;
				CRect rc;
				GetClientRect(rc);
				BITMAP bm;
				SrcImage.GetBitmap(&bm);
				int ow = bm.bmWidth - rc.Width();
				int oh = bm.bmHeight - rc.Height();
				CWnd* parent = GetParent();
				CRect rcParent;
				parent->GetWindowRect(rcParent);
				rcParent.right += ow;
				rcParent.bottom += oh;
				if(parent->GetParent())
				{
					parent->GetParent()->ScreenToClient(rcParent);
					parent->GetParent()->MoveWindow(rcParent);
				}
				else parent->MoveWindow(rcParent);
				rc.right += ow;
				rc.bottom += oh;
				MoveWindow(rc);
			}
			break;
		default:return;
		}
	}
	Redraw(true);
}

BEGIN_MESSAGE_MAP(CTWenBMEditorCtrl, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND_RANGE(ID_MENU_RANGE_FROM, ID_MENU_RANGE_TO, &CTWenBMEditorCtrl::OnMenuCommandRange)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CTWenBMEditorCtrl 消息处理程序
void CTWenBMEditorCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Redraw(true);
}
BOOL CTWenBMEditorCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&ResultImage);
	BITMAP bm;
	ResultImage.GetBitmap(&bm);
	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0,0,rc.Width(),rc.Height(),&dc,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	dc.SelectObject(pOldBitmap);
	return TRUE;
}
void CTWenBMEditorCtrl::OnMenuCommandRange(UINT id)
{
	OnMenu(id);
}
void CTWenBMEditorCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(CurrentSelect && (nFlags & MK_LBUTTON))
	{
		int w = point.x - StartPoint.x;
		int h = point.y - StartPoint.y;
		if(nFlags & MK_CONTROL)//改变大小
		{
			CurrentSelect->Rect.right += w;
			CurrentSelect->Rect.bottom += h;
		}
		else if(nFlags &  MK_SHIFT)//改变中心
		{
			CurrentSelect->Rect.right += w*2;
			CurrentSelect->Rect.bottom += h*2;
		}
		else//改变Rect位置
		{
			CurrentSelect->Rect.left += w;
			CurrentSelect->Rect.top += h;
			CurrentSelect->Rect.right += w;
			CurrentSelect->Rect.bottom += h;
		}
		StartPoint = point;
		//修正坐标
		if(CurrentSelect->Rect.left > CurrentSelect->Rect.right)
		{
			long t = CurrentSelect->Rect.left;
			CurrentSelect->Rect.left = CurrentSelect->Rect.right;
			CurrentSelect->Rect.right = t;
		}
		if(CurrentSelect->Rect.top > CurrentSelect->Rect.bottom)
		{
			long t = CurrentSelect->Rect.top;
			CurrentSelect->Rect.top = CurrentSelect->Rect.bottom;
			CurrentSelect->Rect.bottom = t;
		}
		Redraw(true);
	}
	CWnd::OnMouseMove(nFlags, point);
}
void CTWenBMEditorCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	OnInitMenu(menu);
	CPoint pt(point);
	ClientToScreen(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();

	CWnd::OnRButtonDown(nFlags, point);
}
void CTWenBMEditorCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	StartPoint = point;
	CurrentSelect = FromPosition(point);
	CWnd::OnLButtonDown(nFlags, point);
}
void CTWenBMEditorCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
}
void CTWenBMEditorCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(nFlags &  MK_SHIFT) OnMenu(ID_MENU_PROPERTY_SPECIAL);
	else OnMenu(ID_MENU_PROPERTY_BASE);
	CWnd::OnLButtonDblClk(nFlags, point);
}
void CTWenBMEditorCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
BOOL CTWenBMEditorCtrl::PreTranslateMessage(MSG* pMsg)
{
	static bool shiftDown = false;
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SHIFT) shiftDown = true;
	if(pMsg->message == WM_KEYUP && pMsg->wParam == VK_SHIFT) shiftDown = false;

	static bool ctrlDown = false;
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CONTROL) ctrlDown = true;
	if(pMsg->message == WM_KEYUP && pMsg->wParam == VK_CONTROL) ctrlDown = false;

	if(pMsg->message == WM_KEYDOWN && CurrentSelect)
	{
		int pixels = shiftDown?5:1;
		if(ctrlDown)//改变大小，左是左移右边框，上是上移下边框，右是右移右边框，下是下移下边框
		{
			switch(pMsg->wParam)
			{
			case VK_LEFT:CurrentSelect->Rect.right -= pixels;Redraw(true);break;
			case VK_UP:CurrentSelect->Rect.bottom -= pixels;Redraw(true);break;
			case VK_RIGHT:CurrentSelect->Rect.right += pixels;Redraw(true);break;
			case VK_DOWN:CurrentSelect->Rect.bottom += pixels;Redraw(true);break;
			}
		}
		else
		{
			switch(pMsg->wParam)
			{
			case VK_LEFT:CurrentSelect->Rect.left -= pixels;CurrentSelect->Rect.right -= pixels;Redraw(true);break;
			case VK_UP:CurrentSelect->Rect.top -= pixels;CurrentSelect->Rect.bottom -= pixels;Redraw(true);break;
			case VK_RIGHT:CurrentSelect->Rect.left += pixels;CurrentSelect->Rect.right += pixels;Redraw(true);break;
			case VK_DOWN:CurrentSelect->Rect.top += pixels;CurrentSelect->Rect.bottom += pixels;Redraw(true);break;
			}
		}
	} 

	return CWnd::PreTranslateMessage(pMsg);
}
CTWenTextSerie* CTWenBMEditorCtrl::NewTextSerie()
{
	CTWenTextSerie* pSerie = new CTWenTextSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenRectSerie* CTWenBMEditorCtrl::NewRectSerie()
{
	CTWenRectSerie* pSerie = new CTWenRectSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenRoundRectSerie* CTWenBMEditorCtrl::NewoundRectSerie()
{
	CTWenRoundRectSerie* pSerie = new CTWenRoundRectSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenEllipseSerie* CTWenBMEditorCtrl::NewEllipseSerie()
{
	CTWenEllipseSerie* pSerie = new CTWenEllipseSerie(this);
	AddSerie(pSerie);
	return pSerie;
}
CTWenArrowSerie* CTWenBMEditorCtrl::NewArrowSerie()
{
	CTWenArrowSerie* pSerie = new CTWenArrowSerie(this);
	AddSerie(pSerie);
	return pSerie;
}

void CTWenBMEditorCtrl::DeleteSerieByID(DWORD dwSerieID)
{
	DeleteSerieByIndex(GetSerieIndexByID(dwSerieID));
}
void CTWenBMEditorCtrl::DeleteSerieByIndex(DWORD dwSerieIndex)
{
	CTWenGraphicsSerie* pSerie = GetSerieByIndex(dwSerieIndex);
	if(pSerie)
	{
		if(pSerie == CurrentSelect) CurrentSelect = NULL;
		Series.RemoveAt(dwSerieIndex);
		delete pSerie;
	}
}
CTWenGraphicsSerie* CTWenBMEditorCtrl::GetSerieByID(DWORD dwSerieID) const
{
	return GetSerieByIndex(GetSerieIndexByID(dwSerieID));
}
CTWenGraphicsSerie* CTWenBMEditorCtrl::GetSerieByIndex(DWORD dwSerieIndex) const
{
	if(dwSerieIndex >= (DWORD)Series.GetCount()) return NULL;
	return Series.GetAt(dwSerieIndex);
}
DWORD  CTWenBMEditorCtrl::GetSerieIndexByID(DWORD dwSerieID) const
{
	for(int i=0; i<Series.GetCount(); i++)
	{
		if(Series.GetAt(i)->GetID() == dwSerieID) return (DWORD)i;
	}
	return -1;
}
void CTWenBMEditorCtrl::DeleteSeries()
{
	CurrentSelect = NULL;
	for(int i=0; i<Series.GetCount(); i++)
	{
		delete Series.GetAt(i);
	}
	Series.RemoveAll();
}
void CTWenBMEditorCtrl::AddSerie(CTWenGraphicsSerie* pSerie)
{
	Series.Add(pSerie);
	if(Series.GetCount() == 1) CurrentSelect = pSerie;//是唯一的一条线，则自动设置为主线
}


void CTWenBMEditorCtrl::SetCurrentSelectByID(DWORD dwSerieID)
{
	SetCurrentSelectByIndex(GetSerieIndexByID(dwSerieID));
}
void CTWenBMEditorCtrl::SetCurrentSelectByIndex(DWORD dwSerieIndex)
{
	CurrentSelect = GetSerieByIndex(dwSerieIndex);
	Redraw(true);
}

CTWenGraphicsSerie* CTWenBMEditorCtrl::FromPosition(const CPoint& point) const
{
	for(int i=Series.GetCount()-1; i>-1; i--)
	{
		CTWenGraphicsSerie* pSerie = Series.GetAt(i);
		if(!pSerie->Visible) continue;
		if(pSerie->Rect.PtInRect(point)) return pSerie;
	}
	return NULL;
}
//
void CTWenBMEditorCtrl::Rebuild(CDC* pDC)
{
	if(ResultImage.m_hObject) ResultImage.DeleteObject();

	CRect rc;
	GetClientRect(rc);
	if(pDC == NULL) pDC = GetDC();
	CRect rcClient(rc);

	ResultImage.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&ResultImage);

	//对&dc画图
	DrawBackground(&dc, rc);

	for(int i=0; i<Series.GetCount(); i++)
	{
		CTWenGraphicsSerie* pSerie = Series.GetAt(i);
		if(!pSerie->Visible) continue;
		switch(pSerie->GetSerieType())
		{
		case CTWenGraphicsSerie::ST_TEXT:DrawTextSerie(&dc, (CTWenTextSerie*)pSerie, rc);break;
		case CTWenGraphicsSerie::ST_RECT:DrawRectSerie(&dc, (CTWenRectSerie*)pSerie, rc);break;
		case CTWenGraphicsSerie::ST_ROUNDRECT:DrawRoundRectSerie(&dc, (CTWenRoundRectSerie*)pSerie, rc);break;
		case CTWenGraphicsSerie::ST_ELLIPSE:DrawEllipseSerie(&dc, (CTWenEllipseSerie*)pSerie, rc);break;
		case CTWenGraphicsSerie::ST_ARROW:DrawArrowSerie(&dc, (CTWenArrowSerie*)pSerie, rc);break;
		}
	}

	dc.SelectObject(&pOldBitmap);
}
void CTWenBMEditorCtrl::DrawBackground(CDC* pDC, const CRect& rc)
{
	if(!SrcImage.m_hObject) return;
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&SrcImage);
	BITMAP bm;
	SrcImage.GetBitmap(&bm);
	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	dc.SelectObject(pOldBitmap);
}
void CTWenBMEditorCtrl::DrawTextSerie(CDC* pDC, CTWenTextSerie* pSerie, const CRect& rc)
{
	if(!pSerie) return;
	CFont font;
	structSelectDCFont _SelectFont(pDC, pSerie->Font, font);
	pDC->DrawText(pSerie->Text, pSerie->Rect, pSerie->DrawType);
}
void CTWenBMEditorCtrl::DrawRectSerie(CDC* pDC,CTWenRectSerie* pSerie, const CRect& rc)
{
	if(pSerie->FillType == CTWenGraphicsSerie::FT_FILL)
	{
		pDC->FillSolidRect(pSerie->Rect, pSerie->Color);
	}
	//else
	{
		CPen pen;
		CPen* pOldPen = pDC->SelectObject(pSerie->BorderPen.GetPen(&pen));
		POINT ppt[5]={
			{pSerie->Rect.left,pSerie->Rect.top},
			{pSerie->Rect.left,pSerie->Rect.bottom},
			{pSerie->Rect.right,pSerie->Rect.bottom},
			{pSerie->Rect.right,pSerie->Rect.top},
			{pSerie->Rect.left,pSerie->Rect.top}};
		pDC->Polyline(ppt, 5);
		pDC->SelectObject(pOldPen);
	}
}
void CTWenBMEditorCtrl::DrawRoundRectSerie(CDC* pDC,CTWenRoundRectSerie* pSerie, const CRect& rc)
{
	if(pSerie->FillType == CTWenGraphicsSerie::FT_FILL)
	{
		CPen pen;
		CPen* pOldPen = pDC->SelectObject(pSerie->Pen.GetPen(&pen));
		CBrush brush;
		CBrush* pOldBrush = pDC->SelectObject(pSerie->Brush.GetBrush(&brush));
		pDC->RoundRect(pSerie->Rect, pSerie->Round);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else
	{
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap bmpTemp;
		bmpTemp.CreateCompatibleBitmap(pDC, pSerie->Rect.Width(), pSerie->Rect.Height());
		CBitmap* pOldBitmap = dc.SelectObject(&bmpTemp);

		CPen pen;
		CPen* newPen = pSerie->Pen.GetPen(&pen);
		LOGPEN lp;
		newPen->GetLogPen(&lp);
		CPen* pOldPen = dc.SelectObject(newPen);
		CBrush brush;
		COLORREF Color = lp.lopnColor + 1;
		brush.CreateSolidBrush(Color);
		CBrush* pOldBrush = dc.SelectObject(&brush);
		CRect _rc(0, 0, pSerie->Rect.Width(), pSerie->Rect.Height());
		dc.FillSolidRect(_rc, Color);
		dc.RoundRect(_rc, pSerie->Round);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);

		pDC->TransparentBlt(pSerie->Rect.left, pSerie->Rect.top, _rc.Width(), _rc.Height(), &dc, 0, 0, _rc.Width(), _rc.Height(),Color);
		dc.SelectObject(pOldBitmap);
	}
}
void CTWenBMEditorCtrl::DrawEllipseSerie(CDC* pDC,CTWenEllipseSerie* pSerie, const CRect& rc)
{
	if(pSerie->FillType == CTWenGraphicsSerie::FT_FILL)
	{
		CPen pen;
		CPen* pOldPen = pDC->SelectObject(pSerie->Pen.GetPen(&pen));
		CBrush brush;
		CBrush* pOldBrush = pDC->SelectObject(pSerie->Brush.GetBrush(&brush));
		pDC->Ellipse(pSerie->Rect);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else
	{
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap bmpTemp;
		bmpTemp.CreateCompatibleBitmap(pDC, pSerie->Rect.Width(), pSerie->Rect.Height());
		CBitmap* pOldBitmap = dc.SelectObject(&bmpTemp);

		CPen pen;
		CPen* newPen = pSerie->Pen.GetPen(&pen);
		LOGPEN lp;
		newPen->GetLogPen(&lp);
		CPen* pOldPen = dc.SelectObject(newPen);
		CBrush brush;
		COLORREF Color = lp.lopnColor + 1;
		brush.CreateSolidBrush(Color);
		CBrush* pOldBrush = dc.SelectObject(&brush);
		CRect _rc(0, 0, pSerie->Rect.Width(), pSerie->Rect.Height());
		dc.FillSolidRect(_rc, Color);
		dc.Ellipse(_rc);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);

		pDC->TransparentBlt(pSerie->Rect.left, pSerie->Rect.top, _rc.Width(), _rc.Height(), &dc, 0, 0, _rc.Width(), _rc.Height(),Color);
		dc.SelectObject(pOldBitmap);
	}
}
void CTWenBMEditorCtrl::DrawArrowSerie(CDC* pDC,CTWenArrowSerie* pSerie, const CRect& rc)
{
	POINT pts[4];//0-1,1-2(箭头两侧),1-3（箭头主体）
	switch(pSerie->Direction)
	{
	case CTWenArrowSerie::DIRECTION_LEFT:
		pts[0].x = pts[2].x = pSerie->Rect.left + pSerie->Length;
		pts[0].y = pSerie->Rect.top;
		pts[2].y = pSerie->Rect.bottom;
		pts[1].x = pSerie->Rect.left;
		pts[3].x = pSerie->Rect.right;
		pts[1].y = pts[3].y = (pSerie->Rect.top + pSerie->Rect.bottom)/2;
		break;
	case CTWenArrowSerie::DIRECTION_UP:
		pts[0].y = pts[2].y = pSerie->Rect.top + pSerie->Length;
		pts[0].x = pSerie->Rect.left;
		pts[2].x = pSerie->Rect.right;
		pts[1].y = pSerie->Rect.top;
		pts[3].y = pSerie->Rect.bottom;
		pts[1].x = pts[3].x = (pSerie->Rect.left + pSerie->Rect.right)/2;
		break;
	case CTWenArrowSerie::DIRECTION_RIGHT:
		pts[0].x = pts[2].x = pSerie->Rect.right - pSerie->Length;
		pts[0].y = pSerie->Rect.top;
		pts[2].y = pSerie->Rect.bottom;
		pts[1].x = pSerie->Rect.right;
		pts[3].x = pSerie->Rect.left;
		pts[1].y = pts[3].y = (pSerie->Rect.top + pSerie->Rect.bottom)/2;
		break;
	case CTWenArrowSerie::DIRECTION_DOWN:
		pts[0].y = pts[2].y = pSerie->Rect.bottom - pSerie->Length;
		pts[0].x = pSerie->Rect.left;
		pts[2].x = pSerie->Rect.right;
		pts[1].y = pSerie->Rect.bottom;
		pts[3].y = pSerie->Rect.top;
		pts[1].x = pts[3].x = (pSerie->Rect.left + pSerie->Rect.right)/2;
		break;
	}
	CPen pen;
	CPen* pOldPen = pDC->SelectObject(pSerie->Pen.GetPen(&pen));
	pDC->MoveTo(pts[0]);
	pDC->LineTo(pts[1]);
	pDC->LineTo(pts[2]);
	pDC->MoveTo(pts[1]);
	pDC->LineTo(pts[3]);

	pDC->SelectObject(pOldPen);
}
//
bool CTWenBMEditorCtrl::SpecialProperty_TextSerie(CTWenTextSerie* pSerie)
{
	CTWenTextDlg dlg(this, pSerie);
	return dlg.DoModal() ==  IDOK;
}
bool CTWenBMEditorCtrl::SpecialProperty_RectSerie(CTWenRectSerie* pSerie)
{
	CTWenPenDlg dlg(this, &pSerie->BorderPen);
	return dlg.DoModal() ==  IDOK;
}
bool CTWenBMEditorCtrl::SpecialProperty_RoundRectSerie(CTWenRoundRectSerie* pSerie)
{
	CTWenRoundRectDlg dlg(this, pSerie);
	return dlg.DoModal() ==  IDOK;
}
bool CTWenBMEditorCtrl::SpecialProperty_EllipseSerie(CTWenEllipseSerie* pSerie)
{
	CTWenEllipseDlg dlg(this, pSerie);
	return dlg.DoModal() ==  IDOK;
}
bool CTWenBMEditorCtrl::SpecialProperty_ArrowSerie(CTWenArrowSerie* pSerie)
{
	CTWenArrowDlg dlg(this, pSerie);
	return dlg.DoModal() ==  IDOK;
}
//
void CTWenBMEditorCtrl::SaveIndex(CFile& file) const
{
	BMEditor_SaveString(file, SrcImageFile);
	long c = Series.GetCount();
	BMEditor_SaveLong(file, c);
	for(int i=0; i<c; i++)
	{
		BMEditor_SaveLong(file, Series.GetAt(i)->GetSerieType());
		Series.GetAt(i)->Save(file);
	}
}
void CTWenBMEditorCtrl::LoadIndex(CFile& file)
{
	DeleteSeries();
	BMEditor_LoadString(file, SrcImageFile);
	long c;
	BMEditor_LoadLong(file, c);
	long st;
	for(int i=0; i<c; i++)
	{
		BMEditor_LoadLong(file, st);
		CTWenGraphicsSerie* pSerie = NULL;
		switch(st)
		{
		case CTWenGraphicsSerie::ST_TEXT:pSerie = new CTWenTextSerie(this);break;
		case CTWenGraphicsSerie::ST_RECT:pSerie = new CTWenRectSerie(this);break;
		case CTWenGraphicsSerie::ST_ROUNDRECT:pSerie = new CTWenRoundRectSerie(this);break;
		case CTWenGraphicsSerie::ST_ELLIPSE:pSerie = new CTWenEllipseSerie(this);break;
		case CTWenGraphicsSerie::ST_ARROW:pSerie = new CTWenArrowSerie(this);break;
		}
		pSerie->Load(file);
		AddSerie(pSerie);
	}
}
void CTWenBMEditorCtrl::RefreshSrcImage()
{
	if(SrcImage.m_hObject) SrcImage.DeleteObject();
	HBITMAP h = (HBITMAP)::LoadImage(NULL,SrcImageFile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
	if(!h)
	{
		MessageBox(SrcImageFile, _T("读取位图发生错误"), MB_OK|MB_ICONERROR);
		SrcImageFile = _T("");
	}
	else SrcImage.Attach(h);
}
//
bool CTWenBMEditorCtrl::ChartFont(CTWenFont& font, CWnd* parent)
{
	CHARFORMAT cf;
	cf.bCharSet = DEFAULT_CHARSET;
	cf.bPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.crTextColor = font.Color;
	cf.dwEffects = NULL;
	if(font.Arguments.Bold) cf.dwEffects |= CFE_BOLD;
	if(font.Arguments.Italic) cf.dwEffects |= CFE_ITALIC;
	if(font.Arguments.StrikeOut) cf.dwEffects |= CFE_STRIKEOUT;
	if(font.Arguments.Underline) cf.dwEffects |= CFE_UNDERLINE;
	cf.dwMask = CFM_BOLD|CFM_CHARSET|CFM_COLOR|CFM_FACE|CFM_ITALIC|CFM_OFFSET|CFM_SIZE|CFM_STRIKEOUT|CFM_UNDERLINE;
	_tcscpy(cf.szFaceName, font.Arguments.Name);
	cf.yHeight = font.Arguments.Size*20;
	cf.yOffset = font.Arguments.YOffset;
	CFontDialog dlg(cf, 1, NULL, parent);
	if(dlg.DoModal() != IDOK) return false;
	dlg.GetCharFormat(cf);
	font.Color = cf.crTextColor;
	font.Arguments.Bold = (cf.dwEffects & CFE_BOLD)?true:false;
	font.Arguments.Italic = (cf.dwEffects & CFE_ITALIC)?true:false;
	font.Arguments.StrikeOut = (cf.dwEffects & CFE_STRIKEOUT)?true:false;
	font.Arguments.Underline = (cf.dwEffects & CFE_UNDERLINE)?true:false;
	font.Arguments.Name = cf.szFaceName;
	font.Arguments.Size = cf.yHeight/20;
	font.Arguments.YOffset = cf.yOffset;
	return true;
}
bool CTWenBMEditorCtrl::ChartColor(COLORREF& color, CWnd* parent)
{
	CColorDialog dlg(color, 0, parent);
	if(dlg.DoModal() != IDOK) return false;
	color = dlg.GetColor();
	return true;
}