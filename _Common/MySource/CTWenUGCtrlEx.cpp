#include "stdafx.h"
#include "CTWenUGCtrlEx.h"
#include "CApplication.h"


bool CTWenUGCtrlEx::SelfOrSub(HWND hWnd) const
{
	HWND hSelf = GetSafeHwnd();
	for(;hWnd;)
	{
		if(hSelf == hWnd) return true;
		hWnd = ::GetParent(hWnd);
	}
	return false;
}

CTWenUGCtrlEx::CTWenUGCtrlEx():m_nIndexBMP_Delete(-1),m_nIndexBMP_Add(-1),m_nIndexBMP_INF(-1),
m_nFontID_Title(-1),m_nFontID_Content(-1),m_crContent(RGB(255,255,255)),
m_bDislplayFirstColumn(false),m_bAutoAdd_RowButtonAdd(false)
{
	m_nFontID_Title = AddFont(_T("微软雅黑"), 19, 5);
	m_nFontID_Content = AddFont(_T("微软雅黑"), 18, 4);
}

BOOL CTWenUGCtrlEx::CreateTable(CWnd *pcParent, CRect rect)
{
	if(!CreateGrid(WS_CHILD|WS_VISIBLE,rect, pcParent, 1234)) return FALSE;

	if(!m_bDislplayFirstColumn) SetColWidth(-1, 0);

	return TRUE;
}

void CTWenUGCtrlEx::OnSetup(void)
{
	CFont * pcFont = GetFont(m_nFontID_Content); 

	CUGCell	cell;
	GetGridDefault(&cell); 
	cell.SetTextColor(RGB(0,0,0)); 
	cell.SetFont(pcFont);
	cell.SetReadOnly(true);
	SetGridDefault(&cell); 

	CString AppPath = CApplication::GetApplicationPath();
	AppPath += _T('\\');


	m_nIndexBMP_Delete		= AddBitmap(AppPath + _T("_Action_Delete.bmp"));
	m_nIndexBMP_INF			= AddBitmap(AppPath + _T("_Action_Info.bmp"));
	m_nIndexBMP_Add			= AddBitmap(AppPath + _T("_Action_Add.bmp"));

	CString s;
	if(m_nIndexBMP_Delete<0)
		s += (_T("\r\n") + AppPath + _T("_Action_Delete.bmp"));
	if(m_nIndexBMP_INF<0) 
		s += (_T("\r\n") + AppPath + _T("_Action_Info.bmp"));
	if(m_nIndexBMP_Add<0) 
		s += (_T("\r\n") + AppPath + _T("_Action_Add.bmp"));
	if(!s.IsEmpty()) AfxMessageBox(_T("以下图片未能加载：") + s);
}

BOOL CTWenUGCtrlEx::SetTitle(int nCol, CString cStrTitleName, int nColWidth, int nTitleNO)
{
	if(nCol>=GetNumberCols()) return FALSE;

	QuickSetText(nCol, nTitleNO, cStrTitleName);
	QuickSetFont(nCol, nTitleNO, m_nFontID_Title);
	SetColWidth(nCol, nColWidth);

	return TRUE;
}
int CTWenUGCtrlEx::EditCtrlFinished(LPCTSTR string, BOOL cancelFlag, BOOL continueFlag, int continueCol, long continueRow)
{
	int ret = CUGCtrl::EditCtrlFinished(string, cancelFlag, continueFlag, continueCol, continueRow);
	OnEditCtrlFinished();
	return ret;
}

long CTWenUGCtrlEx::RowIndexOfMinValue(bool emptyFirst, int col, long rowFrom, long rowTo)
{
	long index = rowFrom;
	CString minValue;
	QuickGetText(col, index, &minValue);
	if(minValue.IsEmpty() && emptyFirst) return index;
	
	CString si;
	for(long i=rowFrom+1; i<=rowTo; i++)
	{
		QuickGetText(col, i, &si);
		if(si.IsEmpty())
		{
			if(emptyFirst) return i;
			else continue;
		}

		if(minValue.IsEmpty() || (_tstof(minValue) > _tstof(si)))
		{
			minValue = si;
			index = i;
		}
	}

	return index;
}
void CTWenUGCtrlEx::Sort_ByNumber(bool emptyFirst, int col, long rowFrom, long rowTo, long rowHead)
{/*
	CUGCell cell;
	GetCell(col,rowHead, &cell); 
	cell.SetCellType(UGCT_ARROW); 
	cell.SetCellTypeEx(UGCT_ARROWDRIGHT); 
	SetCell(col, rowHead, &cell);//*/

	for(long i=rowFrom; i<=rowTo; i++)
	{
		long index = RowIndexOfMinValue(emptyFirst, col, i, rowTo);
		if(index != i) SwapRow(i, index);
	}
}
void CTWenUGCtrlEx::SwapRow(long i, long j)
{
	CUGCell ci, cj;
	for(int cols=0; cols<GetNumberCols(); cols++)
	{
		GetCell(cols, i, &ci);
		GetCell(cols, j, &cj);
		SetCell(cols, i, &cj);
		SetCell(cols, j, &ci);
	}
}

void CTWenUGCtrlEx::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed)
{
	CUGCell cell;
	GetCell(col, row, &cell);

	if(updn==FALSE)//只有当鼠标放开的时候才执行
	{
		if(cell.GetBitmap()==GetBitmap(m_nIndexBMP_Delete)) Action_DeleteRow(row);
		else if(cell.GetBitmap()==GetBitmap(m_nIndexBMP_INF)) Action_DisplayRowInf(row);
		else if(cell.GetBitmap()==GetBitmap(m_nIndexBMP_Add)) Action_AddRow();
		else CUGCtrl::OnLClicked(col,row, updn, rect, point, processed);
	}
	else CUGCtrl::OnLClicked(col,row, updn, rect, point, processed);
}
void CTWenUGCtrlEx::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	if(!cell.GetReadOnly()) StartEdit(col, row, 0);
}
void CTWenUGCtrlEx::RomoveRow_ButtonAdd(void)
{
	int nRow = CUGCtrl::GetNumberRows();

	if(nRow == 0) return;

	CUGCell cell;
	GetCell(0, nRow-1, &cell);

	if(cell.GetBitmap()==GetBitmap(m_nIndexBMP_Add))//如果有增加行
	{
		DeleteRow(nRow-1);
	}
}

void CTWenUGCtrlEx::AppendRow_ButtonAdd(void)
{
	int nRow = CUGCtrl::GetNumberRows();

	if(nRow > 0)
	{
		CUGCell cell;
		GetCell(0, nRow-1, &cell);

		if(cell.GetBitmap()==GetBitmap(m_nIndexBMP_Add))//如果有增加行
		{
			return;
		}
	}

	if(AppendRow() != UG_SUCCESS) return;

	int nCol = GetNumberCols();

	JoinCells(1, nRow, nCol-1, nRow);

	if(nCol>1)
	{
		QuickSetBackColor(1, nRow, RGB(240,240,240));
		QuickSetBorder(1, nRow, UG_BDR_RAISED|UG_BDR_LTHIN|UG_BDR_RTHIN|UG_BDR_TTHIN|UG_BDR_BTHIN); 
	}

	QuickSetBitmap(0, nRow, m_nIndexBMP_Add);
}