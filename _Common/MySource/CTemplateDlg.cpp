// TemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CTemplateDlg.h"
#include "CStringExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// Note: Make sure nBufLen is big enough to hold your entire dialog template!
//
CDlgTemplateBuilder::CDlgTemplateBuilder(DWORD dwLength):m_pBuffer(NULL)
{
	SetDataLength(dwLength);
}

CDlgTemplateBuilder::~CDlgTemplateBuilder()
{
	SetDataLength(0);
}
//////////////////
void CDlgTemplateBuilder::SetDataLength(DWORD dwLength)
{
	if(m_pBuffer) delete []m_pBuffer;
	if(dwLength)
	{
		m_pBuffer   = (WORD*)new BYTE[dwLength];
		m_pNext     = m_pBuffer;
		m_pEndBuf   = m_pNext + dwLength;
	}
	else
	{
		m_pBuffer   = NULL;
		m_pNext     = NULL;
		m_pEndBuf   = NULL;
	}
}
DLGTEMPLATE* CDlgTemplateBuilder::GetTemplate()
{
	return (DLGTEMPLATE*)m_pBuffer;
}
WORD* CDlgTemplateBuilder::AddText(WORD* buf, LPCTSTR text)
{
	if (text) {
		USES_CONVERSION;
		wcscpy((WCHAR*)buf, T2W((LPTSTR)text));
		buf += wcslen((WCHAR*)buf)+1;
	} else {
		*buf++ = 0;
	}
	return buf;
}
DLGTEMPLATE* CDlgTemplateBuilder::BeginTemplate(DWORD dwStyle,
												LPCRECT rc,LPCTSTR text,DWORD dwStyleEx)
{
	DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
	hdr->style = dwStyle;
	hdr->dwExtendedStyle = dwStyleEx;
	hdr->cdit = 0;// number of items: zero

	// Set dialog rectangle.
	CRect rcDlg(*rc);
	hdr->x  = (short)rcDlg.left;
	hdr->y  = (short)rcDlg.top;
	hdr->cx = (short)rcDlg.Width();
	hdr->cy = (short)rcDlg.Height();

	// Append trailing items: menu, class, caption. I only use caption.
	m_pNext = (WORD*)(hdr+1);
	*m_pNext++ = 0;// menu (none)
	*m_pNext++ = 0;// dialog class (use standard)
	m_pNext = AddText(m_pNext, text);// append dialog caption

	return hdr;
}
//////////////////
// Add dialog item (control).
void CDlgTemplateBuilder::AddItemTemplate(WORD wType, DWORD dwStyle,
										  LPCRECT rc, WORD nID, DWORD dwStyleEx)
{
	ASSERT(m_pNext < m_pEndBuf);

	// initialize DLGITEMTEMPLATE 
	DLGITEMTEMPLATE& it = *((DLGITEMTEMPLATE*)AlignDWORD(m_pNext));
	it.style = dwStyle;
	it.dwExtendedStyle = dwStyleEx;

	CRect rcDlg(*rc);
	it.x  = (short)rcDlg.left;
	it.y  = (short)rcDlg.top;
	it.cx = (short)rcDlg.Width();
	it.cy = (short)rcDlg.Height();
	it.id = nID;

	// add class (none)
	m_pNext = (WORD*)(&it+1);
	*m_pNext++ = 0xFFFF;						 // next WORD is atom
	*m_pNext++ = wType;						 // ..atom identifier
	ASSERT(m_pNext < m_pEndBuf);			 // check not out of range

	// increment control/item count
	DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
	hdr->cdit++;
}
void CDlgTemplateBuilder::AddItem(WORD wType, DWORD dwStyle,
								  LPCRECT rc, LPCTSTR text, WORD nID, DWORD dwStyleEx)
{
	AddItemTemplate(wType, dwStyle, rc, nID, dwStyleEx);
	m_pNext = AddText(m_pNext, text);	 // append title
	*m_pNext++ = 0;							 // no creation data
	ASSERT(m_pNext < m_pEndBuf);
}
void CDlgTemplateBuilder::AddItem(WORD wType, DWORD dwStyle,
								  LPCRECT rc, WORD wResID, WORD nID, DWORD dwStyleEx)
{
	AddItemTemplate(wType, dwStyle, rc, nID, dwStyleEx);
	*m_pNext++ = 0xFFFF;						 // next is resource id
	*m_pNext++ = wResID;						 // ..here it is
	*m_pNext++ = 0;							 // no extra stuff
	ASSERT(m_pNext < m_pEndBuf);
}
WORD* CDlgTemplateBuilder::AlignDWORD(WORD* ptr)// align ptr to nearest DWORD
{
	ptr++;									 // round up to nearest DWORD
	LPARAM lp = (LPARAM)ptr;			 // convert to long
	lp &= 0xFFFFFFFC;						 // make sure on DWORD boundary
	return (WORD*)lp;
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog


CTemplateDlg::CTemplateDlg(CWnd* pParent /*=NULL*/)
:m_strTitle(_T("请输入数据")),m_strText(_T("")),
m_nNum(0),m_pStatics(NULL),m_pCtrls(NULL),
m_pdwCtrlTypes(NULL),m_pdwSelectTypes(NULL),m_pdwValueTypes(NULL),
m_pdwOptionalStyles(NULL),m_pDefaultValues(NULL),
m_iColumn2Width(320),m_iCBHeight(50),m_iOKCancelTopPadding(5),
m_pDefaultFont(NULL),m_bEnableBtnOK(true),m_bEnableBtnCancel(true),
m_bShowBtnOK(true),m_bShowBtnCancel(true),m_strSep(_T(";")),m_bError(false),m_bUsingOKFont(true),
m_uiControlID(ID_Begin)
{
	//{{AFX_DATA_INIT(CTemplateDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetDlgMargin(5,5,5,5);
	SetColumn1Margin(2,2,2,2);
	SetColumn1WH(0,0,true);
	SetSpacing(5,5);
	m_pParentWnd = pParent;
}

CTemplateDlg::~CTemplateDlg()
{
	POSITION pos = m_mapEnabled.GetStartPosition();
	UINT    id;
	CTemplateDlg_MapItem *item=NULL;
	while (pos != NULL)
	{
		m_mapEnabled.GetNextAssoc(pos, id, item);
		delete item;
	}
	pos = m_mapVisible.GetStartPosition();
	while (pos != NULL)
	{
		m_mapVisible.GetNextAssoc(pos, id, item);
		delete item;
	}

	pos = m_mapEnabled_ComboBox.GetStartPosition();
	CTemplateDlg_MapItem_ComboBox *item_ComboBox=NULL;
	while (pos != NULL)
	{
		m_mapEnabled_ComboBox.GetNextAssoc(pos, id, item_ComboBox);
		delete item_ComboBox;
	}
	pos = m_mapVisible_ComboBox.GetStartPosition();
	while (pos != NULL)
	{
		m_mapVisible_ComboBox.GetNextAssoc(pos, id, item_ComboBox);
		delete item_ComboBox;
	}

	CTemplateDlg_MapItem_Value *item_Value=NULL;
	pos = m_mapValue.GetStartPosition();
	while (pos != NULL)
	{
		m_mapValue.GetNextAssoc(pos, id, item_Value);
		delete item_Value;
	}

	CTemplateDlg_MapItem_Setup *item_Setup=NULL;
	pos = m_mapSetup.GetStartPosition();
	while (pos != NULL)
	{
		m_mapSetup.GetNextAssoc(pos, id, item_Setup);
		delete item_Setup;
	}
}
void CTemplateDlg::MakeSureRelation()
{
	POSITION pos = m_mapEnabled.GetStartPosition();
	UINT    id;
	CTemplateDlg_MapItem *item=NULL;
	while (pos != NULL)
	{
		m_mapEnabled.GetNextAssoc(pos, id, item);
		Relation_Enabled(id, *item);
	}
	pos = m_mapVisible.GetStartPosition();
	while (pos != NULL)
	{
		m_mapVisible.GetNextAssoc(pos, id, item);
		Relation_Visible(id, *item);
	}

	pos = m_mapEnabled_ComboBox.GetStartPosition();
	CTemplateDlg_MapItem_ComboBox *item_ComboBox=NULL;
	while (pos != NULL)
	{
		m_mapEnabled_ComboBox.GetNextAssoc(pos, id, item_ComboBox);
		Relation_Enabled(id, *item_ComboBox);
	}
	pos = m_mapVisible_ComboBox.GetStartPosition();
	while (pos != NULL)
	{
		m_mapVisible_ComboBox.GetNextAssoc(pos, id, item_ComboBox);
		Relation_Visible(id, *item_ComboBox);
	}

	CTemplateDlg_MapItem_Value *item_Value=NULL;
	pos = m_mapValue.GetStartPosition();
	while (pos != NULL)
	{
		m_mapValue.GetNextAssoc(pos, id, item_Value);
		Relation_Value(id, *item_Value);
	}

	CTemplateDlg_MapItem_Setup *item_Setup=NULL;
	pos = m_mapSetup.GetStartPosition();
	while (pos != NULL)
	{
		m_mapSetup.GetNextAssoc(pos, id, item_Setup);
		Relation_Setup(id, *item_Setup);
	}
	_RefreshDialog();
}
void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateDlg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_Begin, ID_End, &CTemplateDlg::OnRangeIDs )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg message handlers
CTemplateDlg::CTemplateDlg_MapItem* CTemplateDlg::GetTemplateDlg_MapItem(CTemplateDlg_Map&map, UINT key, bool bAutoNew)
{
	CTemplateDlg_MapItem*pMapItem=NULL;
	if(map.Lookup(key,pMapItem)) return pMapItem;
	else
	{
		if(bAutoNew)
		{
			pMapItem=new CTemplateDlg_MapItem;
			map.SetAt(key,pMapItem);
			return pMapItem;
		}
		else return NULL;
	}
}
void CTemplateDlg::SaveRelationItem(CTemplateDlg_MapItem*pMapItem,UINT uiIDHandled,bool bSame)
{
	structRelationMapItem item;
	item.uiID=uiIDHandled;
	item.bSame=bSame;
	pMapItem->Add(item);
}
void CTemplateDlg::SetRelation_Enabled(UINT uiIDDetected,UINT uiIDHandled,bool bSame)
{
	CTemplateDlg_MapItem *pMapItem=GetTemplateDlg_MapItem(m_mapEnabled, uiIDDetected, true);
	SaveRelationItem(pMapItem,uiIDHandled,bSame);
}
void CTemplateDlg::SetRelation_Visible(UINT uiIDDetected,UINT uiIDHandled,bool bSame)
{
	CTemplateDlg_MapItem *pMapItem=GetTemplateDlg_MapItem(m_mapVisible, uiIDDetected, true);
	SaveRelationItem(pMapItem,uiIDHandled,bSame);
}

void CTemplateDlg::OnRangeIDs(UINT nID)
{
	CTemplateDlg_MapItem *pMapItem=GetTemplateDlg_MapItem(m_mapEnabled, nID, false);
	if(pMapItem) Relation_Enabled(nID, *pMapItem);
	pMapItem=GetTemplateDlg_MapItem(m_mapVisible, nID, false);
	if(pMapItem) Relation_Visible(nID, *pMapItem);
	_RefreshDialog();
}
void CTemplateDlg::Relation_Enabled(UINT id, CTemplateDlg_MapItem& item)
{
	for (int i = 0; i <= item.GetUpperBound(); i++)
	{
		structRelationMapItem &_item = item.GetAt(i);
		Relation_Enabled(id, _item);
	}
}
void CTemplateDlg::Relation_Visible(UINT id, CTemplateDlg_MapItem& item)
{
	for (int i = 0; i <= item.GetUpperBound(); i++)
	{
		structRelationMapItem &_item = item.GetAt(i);
		Relation_Visible(id, _item);
	}
}
void CTemplateDlg::Relation_Enabled(UINT id, structRelationMapItem& item)
{
	CButton*pButton=(CButton*)GetDlgItem(id);
	bool bChecked=pButton->GetCheck()==BST_CHECKED;
	CWnd*pWnd=GetDlgItem(item.uiID);
	if(pWnd) pWnd->EnableWindow((!(bChecked^item.bSame)) && pButton->IsWindowEnabled());
}
void CTemplateDlg::Relation_Visible(UINT id, structRelationMapItem& item)
{
	CButton*pButton=(CButton*)GetDlgItem(id);
	bool bChecked=pButton->GetCheck()==BST_CHECKED;
	CWnd*pWnd=GetDlgItem(item.uiID);
	bool bVisible=bChecked^item.bSame?false:true;
	if(pWnd) pWnd->ShowWindow(bVisible && _IsControlVisible(pButton)?SW_SHOW:SW_HIDE);
}
///////////////////////////////////
BOOL CTemplateDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT   nID   =   LOWORD(wParam);   
	HWND   hWndCtrl   =   (HWND)lParam;   
	int   nCode   =   HIWORD(wParam);//message
	if(nCode == CBN_SELCHANGE)
	{
		CTemplateDlg_MapItem_ComboBox *pMapItem=GetTemplateDlg_MapItem(m_mapEnabled_ComboBox, nID, false);
		if(pMapItem) Relation_Enabled(nID, *pMapItem);
		pMapItem=GetTemplateDlg_MapItem(m_mapVisible_ComboBox, nID, false);
		if(pMapItem) Relation_Visible(nID, *pMapItem);

		CTemplateDlg_MapItem_Value *pMapItemValue=GetTemplateDlg_MapItem(m_mapValue, nID, false);
		if(pMapItemValue) Relation_Value(nID, *pMapItemValue);

		CTemplateDlg_MapItem_Setup *pMapItemSetup=GetTemplateDlg_MapItem(m_mapSetup, nID, false);
		if(pMapItemSetup) Relation_Setup(nID, *pMapItemSetup);

		_RefreshDialog();
	}

	return CDialog::OnCommand(wParam, lParam);
}
CTemplateDlg::CTemplateDlg_MapItem_ComboBox* CTemplateDlg::GetTemplateDlg_MapItem(CTemplateDlg_Map_ComboBox&map, UINT key, bool bAutoNew)
{
	CTemplateDlg_MapItem_ComboBox*pMapItem=NULL;
	if(map.Lookup(key,pMapItem)) return pMapItem;
	else
	{
		if(bAutoNew)
		{
			pMapItem=new CTemplateDlg_MapItem_ComboBox;
			map.SetAt(key,pMapItem);
			return pMapItem;
		}
		else return NULL;
	}
}
void CTemplateDlg::SaveRelationItem(CTemplateDlg_MapItem_ComboBox*pMapItem,int i,UINT uiIDHandled,LPCTSTR values,LPCTSTR sep)
{
	structRelationMapItem_ComboBox item;
	item.i=i;
	item.uiID=uiIDHandled;
	item.values=values;
	item.sep=sep;
	item.values=sep+item.values+sep;//方便查找
	pMapItem->Add(item);
}
void CTemplateDlg::SetRelation_Enabled(int i,UINT uiIDDetected,UINT uiIDHandled,LPCTSTR values,LPCTSTR sep)
{
	CTemplateDlg_MapItem_ComboBox *pMapItem=GetTemplateDlg_MapItem(m_mapEnabled_ComboBox, uiIDDetected, true);
	SaveRelationItem(pMapItem,i,uiIDHandled,values,sep);
}
void CTemplateDlg::SetRelation_Visible(int i,UINT uiIDDetected,UINT uiIDHandled,LPCTSTR values,LPCTSTR sep)
{
	CTemplateDlg_MapItem_ComboBox *pMapItem=GetTemplateDlg_MapItem(m_mapVisible_ComboBox, uiIDDetected, true);
	SaveRelationItem(pMapItem,i,uiIDHandled,values,sep);
}

void CTemplateDlg::Relation_Enabled(UINT id, CTemplateDlg_MapItem_ComboBox& item)
{
	for (int i = 0; i <= item.GetUpperBound(); i++)
	{
		structRelationMapItem_ComboBox &_item = item.GetAt(i);
		Relation_Enabled(id, _item);
	}
}
void CTemplateDlg::Relation_Visible(UINT id, CTemplateDlg_MapItem_ComboBox& item)
{
	for (int i = 0; i <= item.GetUpperBound(); i++)
	{
		structRelationMapItem_ComboBox &_item = item.GetAt(i);
		Relation_Visible(id, _item);
	}
}
void CTemplateDlg::Relation_Enabled(UINT id, structRelationMapItem_ComboBox& item)
{
	CString v;
	if(item.i == -1) item.i = GetIndexByID(id);
	CComboBox*pCB=(CComboBox*)m_pCtrls[item.i];
	GetSelectValue(v,pCB,m_pdwValueTypes[item.i] & VT_GetType);
	v=item.sep+v+item.sep;
	bool bEnabled = item.values.Find(v) > -1;
	CWnd*pWnd=GetDlgItem(item.uiID);
	if(pWnd) pWnd->EnableWindow(bEnabled && pCB->IsWindowEnabled());
}
void CTemplateDlg::Relation_Visible(UINT id, structRelationMapItem_ComboBox& item)
{
	CString v;
	if(item.i == -1) item.i = GetIndexByID(id);
	CComboBox*pCB=(CComboBox*)m_pCtrls[item.i];
	GetSelectValue(v,pCB,m_pdwValueTypes[item.i] & VT_GetType);
	v=item.sep+v+item.sep;
	bool bVisible = item.values.Find(v) > -1;
	CWnd*pWnd=GetDlgItem(item.uiID);
	if(pWnd) pWnd->ShowWindow(bVisible && _IsControlVisible(pCB)?SW_SHOW:SW_HIDE);
}
////////////////////////////////////////
void CTemplateDlg::SetRelation_Value(int i,UINT uiIDDetected,int j,UINT uiIDHandled,LPCTSTR value,LPCTSTR values,LPCTSTR sep)
{
	CTemplateDlg_MapItem_Value *pMapItem=GetTemplateDlg_MapItem(m_mapValue, uiIDDetected, true);
	structValueMapItem item;
	item.i=i;
	item.uiID=uiIDHandled;
	item.j=j;
	item.value=value;
	item.values=values;
	item.sep=sep;
	item.values=sep+item.values+sep;//方便查找
	pMapItem->Add(item);
}
CTemplateDlg::CTemplateDlg_MapItem_Value* CTemplateDlg::GetTemplateDlg_MapItem(CTemplateDlg_Map_Value&map, UINT key, bool bAutoNew)
{
	CTemplateDlg_MapItem_Value*pMapItem=NULL;
	if(map.Lookup(key,pMapItem)) return pMapItem;
	else
	{
		if(bAutoNew)
		{
			pMapItem=new CTemplateDlg_MapItem_Value;
			map.SetAt(key,pMapItem);
			return pMapItem;
		}
		else return NULL;
	}
}
void CTemplateDlg::Relation_Value(UINT id, CTemplateDlg_MapItem_Value& item)
{
	for (int i = 0; i <= item.GetUpperBound(); i++)
	{
		structValueMapItem &_item = item.GetAt(i);
		Relation_Value(id, _item);
	}
}
void CTemplateDlg::Relation_Value(UINT id, structValueMapItem& item)
{
	//获取item.i的值，目前只支持combobox
	CString v;
	if(item.i == -1) item.i = GetIndexByID(id);
	if(item.j == -1) item.j = GetIndexByID(item.uiID);
	CComboBox*pCB=(CComboBox*)m_pCtrls[item.i];
	GetSelectValue(v,pCB,m_pdwValueTypes[item.i] & VT_GetType);
	v=item.sep+v+item.sep;
	bool bSet = item.values.Find(v) > -1;
	CWnd*pWnd=GetDlgItem(item.uiID);
	if(pWnd && bSet)
	{
		//根据pWnd的值类型设置其值
		switch(m_pdwCtrlTypes[item.j])
		{
		case CT_Label:
		case CT_Button:
		case CT_Edit:pWnd->SetWindowText(item.value);break;
		case CT_CheckBox:
			((CButton*)pWnd)->SetCheck((item.value.GetLength()>0 && _tstoi(item.value))?BST_CHECKED:BST_UNCHECKED);
			break;
		case CT_ComboBoxDropList:
		case CT_ComboBoxDropdown:
			SetCBDropListSel((CComboBox*)pWnd,m_pdwValueTypes[item.j],item.value);
			break;
		case CT_Date:SetDate((CDateTimeCtrl*)pWnd,item.value);break;
		case CT_Time:SetTime((CDateTimeCtrl*)pWnd,item.value);break;
		}
	}
}
/////////////////////////////////////////////////////////
void CTemplateDlg::SetRelation_Setup(int i,UINT uiIDDetected,int j,UINT uiIDHandled,LPCTSTR newValue,LPCTSTR newValueSep,LPCTSTR values,LPCTSTR sep)
{
	CTemplateDlg_MapItem_Setup *pMapItem=GetTemplateDlg_MapItem(m_mapSetup, uiIDDetected, true);
	structSetupMapItem item;
	item.i=i;
	item.uiID=uiIDHandled;
	item.j=j;
	item.newValue=newValue;
	item.newValueSep=newValueSep?newValueSep:_T(",");
	item.values=values;
	item.sep=sep;
	item.values=sep+item.values+sep;//方便查找
	pMapItem->Add(item);
}
void CTemplateDlg::Relation_Setup(UINT id, CTemplateDlg_MapItem_Setup& item)
{
	for (int i = 0; i <= item.GetUpperBound(); i++)
	{
		structSetupMapItem &_item = item.GetAt(i);
		Relation_Setup(id, _item);
	}
}
void CTemplateDlg::Relation_Setup(UINT id, structSetupMapItem& item)
{
	//获取item.i的值，目前只支持combobox
	CString v;
	if(item.i == -1) item.i = GetIndexByID(id);
	if(item.j == -1) item.j = GetIndexByID(item.uiID);
	CComboBox*pCB=(CComboBox*)m_pCtrls[item.i];
	GetSelectValue(v,pCB,m_pdwValueTypes[item.i] & VT_GetType);
	v=item.sep+v+item.sep;
	bool bSet = item.values.Find(v) > -1;
	CWnd*pWnd=GetDlgItem(item.uiID);
	if(pWnd && bSet)
	{
		//根据pWnd的值类型设置其值
		switch(m_pdwCtrlTypes[item.j])
		{
		case CT_ComboBoxDropList:
		case CT_ComboBoxDropdown:
			{
				SetupCB((CComboBox*)pWnd,m_pdwValueTypes[item.j],item.newValue);
				CTemplateDlg_MapItem_Setup *pMapItemSetup=GetTemplateDlg_MapItem(m_mapSetup, item.uiID, false);
				if(pMapItemSetup) Relation_Setup(item.uiID, *pMapItemSetup);
			}
			break;
		}
	}
}
//有则返回，若无，若bNew为true增加后返回，否则返回NULL
CTemplateDlg::CTemplateDlg_MapItem_Setup* CTemplateDlg::GetTemplateDlg_MapItem(CTemplateDlg_Map_Setup&map, UINT key, bool bAutoNew)
{
	CTemplateDlg_MapItem_Setup*pMapItem=NULL;
	if(map.Lookup(key,pMapItem)) return pMapItem;
	else
	{
		if(bAutoNew)
		{
			pMapItem=new CTemplateDlg_MapItem_Setup;
			map.SetAt(key,pMapItem);
			return pMapItem;
		}
		else return NULL;
	}
}
////////////////////////////////////////////////////////////
void CTemplateDlg::MakeSureC1WH()
{
	if(!m_bAutoSet) return;
	CSize sz;
	CStringExt ext(m_strText,_T(";"));
	ext.BeginEnum();
	CString txt(_T("Sample"));
	CDC*pDC=GetDC();
	m_ptC1.x=0;m_ptC1.y=0;
	sz=pDC->GetTextExtent(txt);
	if(sz.cx>m_ptC1.x){m_ptC1.x=sz.cx;m_ptC1.y=sz.cy;}
	for(BYTE i=0;i<m_nNum;i++)
	{
		ext.GetNext(txt);
		if(m_pdwCtrlTypes[i]==CT_None) continue;
		sz=pDC->GetTextExtent(txt);
		if(sz.cx>m_ptC1.x){m_ptC1.x=sz.cx;m_ptC1.y=sz.cy;}
	}
	m_ptC1.x=m_ptC1.x+m_rcC1Margin.left+m_rcC1Margin.right;
	m_ptC1.y=m_ptC1.y+m_rcC1Margin.top+m_rcC1Margin.bottom;
}
void CTemplateDlg::MakeSureSys(CRect&rcDlg,POINT&ptOKCancel)
{
	m_btnOK.GetClientRect(&rcDlg);
	ptOKCancel.x=rcDlg.Width();
	ptOKCancel.y=rcDlg.Height();
	m_ptDlgClient.x=m_rcDlgMargin.left+m_ptC1.x+m_ptSpacing.x+m_iColumn2Width+m_rcDlgMargin.right;//客户区宽
	m_ptDlgClient.y=m_rcDlgMargin.top+(m_ptC1.y+m_ptSpacing.y)*m_nNum+m_iOKCancelTopPadding+ptOKCancel.y+m_rcDlgMargin.bottom;
	CRect wr,cr;
	GetWindowRect(&wr);
	GetClientRect(&cr);
	rcDlg=wr;
	rcDlg.right=wr.Width()-cr.Width()+m_ptDlgClient.x+wr.left;
	rcDlg.bottom=wr.Height()-cr.Height()+m_ptDlgClient.y+wr.top;
}
BOOL CTemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(!m_pDefaultFont)
	{
		CWnd*p=this->GetParent();
		if(p) m_pDefaultFont=p->GetFont();
	}
	if(m_pDefaultFont) SetFont(m_pDefaultFont,false);
	//	SetWindowText(m_strTitle);
	MakeSureC1WH();
	MakeSureSys(m_rcDlg,m_ptOKCancel);
	MoveWindow(&m_rcDlg);
	SetupUI();
	BeforeRelation();
	MakeSureRelation();
	m_btnOK.EnableWindow(m_bEnableBtnOK);
	m_btnCancel.EnableWindow(m_bEnableBtnCancel);
	m_btnOK.ShowWindow(m_bShowBtnOK?SW_SHOW:SW_HIDE);
	m_btnCancel.ShowWindow(m_bShowBtnCancel?SW_SHOW:SW_HIDE);
	Invalidate();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CTemplateDlg::SetCBDropListSel(CComboBox*pCB,DWORD dwValueType,LPCTSTR value)
{
	if(!value || _tcslen(value)<1) return;
	int i=0;
	DWORD dw=0;
	switch(dwValueType)
	{
	case VT_Index:pCB->SetCurSel(_tstoi(value));break;
	case VT_Text:pCB->SelectString(-1, value);break;
	case VT_ItemData:
		dw=(DWORD)_tstol(value);
		for(i=0;i<pCB->GetCount();i++) if(pCB->GetItemData(i)==dw)
		{
			pCB->SetCurSel(i);break;
		}
		break;
	case VT_ItemDataString:
		for(i=0;i<pCB->GetCount();i++) if(*((CString*)pCB->GetItemData(i))==value)
		{
			pCB->SetCurSel(i);break;
		}
		break;
	}
}
void CTemplateDlg::SetupCB(CComboBox*pCB,DWORD dwValueType,LPCTSTR value)
{
	for(int i=pCB->GetCount()-1;i>-1;i--) pCB->DeleteString(i);
	CString initValue,list;
	CString src(value);
	int i=src.Find(_T('|'));
	if(i<0)
	{
		initValue=_T("");
		list=src;
	}
	else
	{
		initValue=src.Left(i);
		list=src.Right(src.GetLength()-i-1);
	}
	CStringExt ext(list,_T(","));
	ext.BeginEnum();
	CString item,dword;
	switch(dwValueType)
	{
	case VT_Index:
	case VT_Text:
		while(ext.GetNext(src)){if(src.GetLength()>0) pCB->AddString(src);}
		break;
	case VT_ItemData:
		while(ext.GetNext(src))
		{
			if(src.GetLength()<1) continue;
			if(CStringExt::Split(src,_T(":"),2,&item,&dword))
			{
				i=pCB->AddString(item);
				pCB->SetItemData(i,(DWORD)_tstol(dword));
			}
			else
			{
				i=pCB->AddString(src);
				pCB->SetItemData(i,(DWORD)i);
			}
		}
		break;
	case VT_ItemDataString:
		while(ext.GetNext(src))
		{
			if(src.GetLength()<1) continue;
			if(CStringExt::Split(src,_T(":"),2,&item,&dword))
			{
				i=pCB->AddString(item);
				pCB->SetItemData(i,(DWORD)new CString(dword));
			}
			else
			{
				i=pCB->AddString(src);
				pCB->SetItemData(i,(DWORD)new CString(_T("NULL")));
			}
		}
		break;
	}
	SetCBDropListSel(pCB,dwValueType,initValue);
}
void CTemplateDlg::SetDate(CDateTimeCtrl*pDT,LPCTSTR date)
{
	if(!pDT || !date || _tcslen(date)<1) return;
	CString year,month,day;
	if(!CStringExt::Split(date,_T("-"),3,&year,&month,&day)) return;
	CTime timeTime(_tstoi(year),_tstoi(month),_tstoi(day),0,0,0);
	pDT->SetTime(&timeTime);
}
void CTemplateDlg::SetTime(CDateTimeCtrl*pDT,LPCTSTR time)
{
	if(!pDT || !time || _tcslen(time)<1) return;
	CString hour,minute,second;
	if(!CStringExt::Split(time,_T(":"),3,&hour,&minute,&second)) return;
	CTime timeTime(2000,1,1,_tstoi(hour),_tstoi(minute),_tstoi(second));
	pDT->SetTime(&timeTime);
}
bool CTemplateDlg::_IsControlVisible(CWnd*pWnd)
{
	if(!pWnd) return false;
	return (::GetWindowLong(pWnd->GetSafeHwnd(),   GWL_STYLE)   &   WS_VISIBLE)?true:false;
}
void CTemplateDlg::_RefreshDialog()
{
	RECT rcc1={m_rcDlgMargin.left,m_rcDlgMargin.top,m_rcDlgMargin.left+m_ptC1.x,m_rcDlgMargin.top+m_ptC1.y};
	RECT rcc2={rcc1.right+m_ptSpacing.x,rcc1.top,m_iColumn2Width+rcc1.right+m_ptSpacing.x,rcc1.bottom};
	RECT rccb;
	RECT rcLabel;
	int stepv=m_ptC1.y+m_ptSpacing.y;
	int count=0;
	for(int i=0;i<m_nNum;i++)
	{
		//获取隐藏关系，确定m_pStatics[i]是否要隐藏
		bool bVisible=true;
		if(m_pCtrls[i]) bVisible=_IsControlVisible(m_pCtrls[i]);
		m_pStatics[i].ShowWindow(bVisible?SW_SHOW:SW_HIDE);
		if(!bVisible) continue;
		count++;
		m_pStatics[i].MoveWindow(&rcc1);
		switch(m_pdwCtrlTypes[i])
		{
		case CT_None:
			rcLabel=rcc1;
			rcLabel.right=rcc2.right;
			m_pStatics[i].MoveWindow(&rcLabel);
			break;
		case CT_Edit:
			rccb=rcc2;
			//rccb.top+=2;
			//rccb.bottom-=2;
			m_pCtrls[i]->MoveWindow(&rccb);
			break;
		case CT_Label:
		case CT_CheckBox:
		case CT_Date:
		case CT_Time:
			m_pCtrls[i]->MoveWindow(&rcc2);
			break;
		case CT_Button:
			rccb=rcc2;
			rccb.top-=2;
			rccb.bottom+=2;
			m_pCtrls[i]->MoveWindow(&rccb);
			break;
		case CT_ComboBoxDropList:
			rccb=rcc2;
			rccb.bottom+=m_iCBHeight;
			m_pCtrls[i]->MoveWindow(&rccb);
			break;
		case CT_ComboBoxDropdown:
			rccb=rcc2;
			rccb.bottom+=m_iCBHeight;
			m_pCtrls[i]->MoveWindow(&rccb);
			break;
		}
		rcc1.top+=stepv;
		rcc1.bottom+=stepv;
		rcc2.top+=stepv;
		rcc2.bottom+=stepv;
	}

	CRect rc;
	GetWindowRect(rc);
	rc.bottom=rc.top+m_rcDlg.Height()-(m_nNum-count)*(m_ptC1.y+m_ptSpacing.y);
	MoveWindow(&rc);
	GetClientRect(rc);

	int hs2=40;//ok,cancel间距
	int l=(rc.Width()-hs2)/2-m_ptOKCancel.x;
	int t=rc.Height()-m_rcDlgMargin.bottom-m_ptOKCancel.y;
	m_btnOK.MoveWindow(l,t,m_ptOKCancel.x,m_ptOKCancel.y);
	l=l+m_ptOKCancel.x+hs2;
	m_btnCancel.MoveWindow(l,t,m_ptOKCancel.x,m_ptOKCancel.y);	
}

void CTemplateDlg::SetupUI()
{
	m_pStatics=new CStatic[m_nNum];
	m_pCtrls=new CWnd*[m_nNum];
	CStringExt ext(m_strText,_T(";"));
	ext.BeginEnum();
	CString txt;
	RECT rcc1={m_rcDlgMargin.left,m_rcDlgMargin.top,m_rcDlgMargin.left+m_ptC1.x,m_rcDlgMargin.top+m_ptC1.y};
	RECT rcc2={rcc1.right+m_ptSpacing.x,rcc1.top,m_iColumn2Width+rcc1.right+m_ptSpacing.x,rcc1.bottom};
	RECT rccb;
	RECT rcLabel;
	int stepv=m_ptC1.y+m_ptSpacing.y;
	CString Value,ID,Text;
	DWORD dwID;
	CFont* pFont=m_pDefaultFont;
	if (m_bUsingOKFont) pFont=m_btnOK.GetFont();

	for(int i=0;i<m_nNum;i++)
	{
		ext.GetNext(txt);
		m_pStatics[i].Create(txt,WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE,rcc1,this,IDC_STATIC);
		m_pStatics[i].SetFont(pFont,false);
		dwID=NewControlID();
		switch(m_pdwCtrlTypes[i])
		{
		case CT_None:
			rcLabel=rcc1;
			rcLabel.right=rcc2.right;
			m_pStatics[i].MoveWindow(&rcLabel);
			m_pCtrls[i]=NULL;
			break;
		case CT_Label:
			m_pCtrls[i]=new CStatic;
			((CStatic*)m_pCtrls[i])->Create(m_pDefaultValues[i],WS_CHILD|WS_VISIBLE|m_pdwOptionalStyles[i],rcc2,this,dwID);
			((CStatic*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_Button:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),2,&Text,&ID))
			{
				dwID=(DWORD)_tstol(ID);
			}
			else Text=m_pDefaultValues[i];
			rccb=rcc2;
			rccb.top-=2;
			rccb.bottom+=2;
			m_pCtrls[i]=new CButton;
			((CButton*)m_pCtrls[i])->Create(Text,WS_CHILD|WS_VISIBLE|m_pdwOptionalStyles[i],rccb,this,dwID);
			((CButton*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_Edit:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),2,&Text,&ID))
			{
				dwID=(DWORD)_tstol(ID);
			}
			else Text=m_pDefaultValues[i];
			m_pCtrls[i]=new CEdit;
			rccb=rcc2;
			//rccb.top+=2;
			//rccb.bottom-=2;
			((CEdit*)m_pCtrls[i])->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|m_pdwOptionalStyles[i],rccb,this,dwID);
			((CEdit*)m_pCtrls[i])->SetWindowText(Text);
			((CEdit*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_CheckBox:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),3,&Value,&Text,&ID))
			{
				dwID=(DWORD)_tstol(ID);
			}
			else if(m_pDefaultValues[i].Find(_T('|'))==-1) Text=_T("");
			m_pCtrls[i]=new CButton;
			((CButton*)m_pCtrls[i])->Create(Text,WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|m_pdwOptionalStyles[i],rcc2,this,dwID);
			if(Value.GetLength()>0 && _tstoi(Value)) ((CButton*)m_pCtrls[i])->SetCheck(BST_CHECKED);
			((CButton*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_ComboBoxDropList:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),3,&Value,&Text,&ID))
			{
				dwID=(DWORD)_tstol(ID);
				Value=Value+_T('|')+Text;
			}
			else Value=m_pDefaultValues[i];
			m_pCtrls[i]=new CComboBox;
			rccb=rcc2;
			rccb.bottom+=m_iCBHeight;
			((CComboBox*)m_pCtrls[i])->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST|m_pdwOptionalStyles[i],rccb,this,dwID);
			SetupCB((CComboBox*)m_pCtrls[i],m_pdwValueTypes[i],Value);
			((CComboBox*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_ComboBoxDropdown:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),3,&Value,&Text,&ID))
			{
				dwID=(DWORD)_tstol(ID);
				Value=Value+_T('|')+Text;
			}
			else Value=m_pDefaultValues[i];
			m_pCtrls[i]=new CComboBox;
			rccb=rcc2;
			rccb.bottom+=m_iCBHeight;
			((CComboBox*)m_pCtrls[i])->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWN|m_pdwOptionalStyles[i],rccb,this,dwID);
			SetupCB((CComboBox*)m_pCtrls[i],VT_Text,Value);
			((CComboBox*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_Date:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),2,&Value,&ID))
			{
				dwID=(DWORD)_tstol(ID);
			}
			else Value=m_pDefaultValues[i];
			m_pCtrls[i]=new CDateTimeCtrl;
			((CDateTimeCtrl*)m_pCtrls[i])->Create(WS_CHILD|WS_VISIBLE|DTS_LONGDATEFORMAT|m_pdwOptionalStyles[i],rcc2,this,dwID);
			SetDate((CDateTimeCtrl*)m_pCtrls[i],Value);
			((CDateTimeCtrl*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		case CT_Time:
			if(CStringExt::Split(m_pDefaultValues[i],_T("|"),2,&Value,&ID))
			{
				dwID=(DWORD)_tstol(ID);
			}
			else Value=m_pDefaultValues[i];
			m_pCtrls[i]=new CDateTimeCtrl;
			((CDateTimeCtrl*)m_pCtrls[i])->Create(WS_CHILD|WS_VISIBLE|DTS_TIMEFORMAT|DTS_UPDOWN|m_pdwOptionalStyles[i],rcc2,this,dwID);
			SetTime((CDateTimeCtrl*)m_pCtrls[i],Value);
			((CDateTimeCtrl*)m_pCtrls[i])->SetFont(pFont,false);
			break;
		}
		rcc1.top+=stepv;
		rcc1.bottom+=stepv;
		rcc2.top+=stepv;
		rcc2.bottom+=stepv;

		SetMapIndexID(i, dwID);
	}
	int hs2=40;//ok,cancel间距
	int l=(m_ptDlgClient.x-hs2)/2-m_ptOKCancel.x;
	int t=m_ptDlgClient.y-m_rcDlgMargin.bottom-m_ptOKCancel.y;
	m_btnOK.MoveWindow(l,t,m_ptOKCancel.x,m_ptOKCancel.y);
	if(!m_bUsingOKFont) m_btnOK.SetFont(m_pDefaultFont,false);
	l=l+m_ptOKCancel.x+hs2;
	m_btnCancel.MoveWindow(l,t,m_ptOKCancel.x,m_ptOKCancel.y);
	if(!m_bUsingOKFont) m_btnCancel.SetFont(m_pDefaultFont,false);
	SetTimer(1,100,NULL);
}
void CTemplateDlg::SetTitle(LPCTSTR title)
{
	m_strTitle=title;
}
void CTemplateDlg::SetCtrlInfo(BYTE nNum,...)//必须设置且只能1次
//text,ctrltype,dwOptionalStyle,selecttype,valuetype,strDefaultValue
{
	if(nNum<1 || m_nNum>0) return;
	m_nNum=nNum;
	m_strText=_T("");
	m_pdwCtrlTypes=new DWORD[m_nNum];
	m_pdwOptionalStyles=new DWORD[m_nNum];
	m_pdwSelectTypes=new DWORD[m_nNum];
	m_pdwValueTypes=new DWORD[m_nNum];
	m_pDefaultValues=new CString[m_nNum];
	va_list argList;
	va_start(argList, nNum);
	LPCTSTR pStr=NULL;
	LPCTSTR pStr2=NULL;
	for(BYTE i=0;i<nNum;i++)
	{
		pStr2=va_arg(argList,LPCTSTR);
		m_strText=m_strText+pStr2+_T(";");
		m_pdwCtrlTypes[i]=va_arg(argList,DWORD);
		m_pdwOptionalStyles[i]=va_arg(argList,DWORD);
		m_pdwSelectTypes[i]=va_arg(argList,DWORD);
		m_pdwValueTypes[i]=va_arg(argList,DWORD);
		pStr=va_arg(argList,LPCTSTR);
		if(pStr) m_pDefaultValues[i]=pStr;
		else m_pDefaultValues[i]=_T("");
	}
	va_end(argList);
}
void CTemplateDlg::SetCtrlInfo(BYTE nNum, const CString* pText, const DWORD* pdwCtrlType, const DWORD* pdwOptionalStyle, const DWORD* pdwSelectType, const DWORD* pdwValueType, const CString* pstrDefault)
{
	if(nNum<1 || m_nNum>0) return;
	m_nNum=nNum;
	m_strText=_T("");
	m_pdwCtrlTypes=new DWORD[m_nNum];
	m_pdwOptionalStyles=new DWORD[m_nNum];
	m_pdwSelectTypes=new DWORD[m_nNum];
	m_pdwValueTypes=new DWORD[m_nNum];
	m_pDefaultValues=new CString[m_nNum];
	LPCTSTR pStr=NULL;
	LPCTSTR pStr2=NULL;
	for(BYTE i=0;i<nNum;i++)
	{
		pStr2=pText[i];
		m_strText=m_strText+pStr2+_T(";");
		pStr=pstrDefault[i];
		if(pStr) m_pDefaultValues[i]=pStr;
		else m_pDefaultValues[i]=_T("");
	}
	if(pdwCtrlType) memcpy(m_pdwCtrlTypes, pdwCtrlType, sizeof(DWORD)*m_nNum);
	else memset(m_pdwCtrlTypes, 0, sizeof(DWORD)*m_nNum);
	if(pdwOptionalStyle) memcpy(m_pdwOptionalStyles, pdwOptionalStyle, sizeof(DWORD)*m_nNum);
	else memset(m_pdwOptionalStyles, 0, sizeof(DWORD)*m_nNum);
	if(pdwSelectType) memcpy(m_pdwSelectTypes, pdwSelectType, sizeof(DWORD)*m_nNum);
	else memset(m_pdwSelectTypes, 0, sizeof(DWORD)*m_nNum);
	if(pdwValueType) memcpy(m_pdwValueTypes, pdwValueType, sizeof(DWORD)*m_nNum);
	else memset(m_pdwValueTypes, 0, sizeof(DWORD)*m_nNum);
}
void CTemplateDlg::SetDlgMargin(int l,int t,int r,int b)
{
	m_rcDlgMargin.left=l;
	m_rcDlgMargin.top=t;
	m_rcDlgMargin.right=r;
	m_rcDlgMargin.bottom=b;
}
void CTemplateDlg::SetColumn1Margin(int l,int t,int r,int b)
{
	m_rcC1Margin.left=l;
	m_rcC1Margin.top=t;
	m_rcC1Margin.right=r;
	m_rcC1Margin.bottom=b;
}
void CTemplateDlg::SetColumn2Width(int ew)
{
	m_iColumn2Width=ew;
}
void CTemplateDlg::SetColumn1WH(int w,int h,bool bAutoSet)
{
	m_ptC1.x=w;
	m_ptC1.y=h;
	m_bAutoSet=bAutoSet;
}
void CTemplateDlg::SetSpacing(int hs,int vs)
{
	m_ptSpacing.x=hs;
	m_ptSpacing.y=vs;
}
void CTemplateDlg::SetCBHeight(int iCBHeight)
{
	m_iCBHeight=iCBHeight;
}
void CTemplateDlg::SetOKCancelTopPadding(int iTopPadding)
{
	m_iOKCancelTopPadding=iTopPadding;
}
int CTemplateDlg::GetFirstSel(CListBox*pLB)
{
	for(int i=0;i<pLB->GetCount();i++)
		if(pLB->GetSel(i)>0) return i;
	return -1;
}
void CTemplateDlg::GetSelectValue(CString&st,CListBox*pLB,DWORD dwSelectType,DWORD dwValueType)
{
	if(dwSelectType & ST_None) st="";
	else if(dwSelectType & ST_Single) GetSelectValue(st,pLB,GetFirstSel(pLB),dwValueType);
	else if(dwSelectType & ST_Multiple)
	{
		st="";
		CString value;
		for(int i=0;i<pLB->GetCount();i++)
			if(pLB->GetSel(i)>0)
			{
				GetSelectValue(value,pLB,i,dwValueType);
				st=st+value+_T(",");
			}
	}
}
int CTemplateDlg::GetCurSel(CListCtrl&m_lst)
{
	POSITION pos = m_lst.GetFirstSelectedItemPosition();
	if (pos == NULL) return -1;
	return m_lst.GetNextSelectedItem(pos);
}
void CTemplateDlg::GetSelectValue(CString&st,CListCtrl*pLC,int iSubItem,DWORD dwSelectType,DWORD dwValueType)
{
	if(dwSelectType & ST_None) st="";
	else if(dwSelectType & ST_Single) GetSelectValue(st,pLC,GetCurSel(*pLC),iSubItem,dwValueType);
	else if(dwSelectType & ST_Multiple)
	{
		st="";
		CString value;
		for(int i=0;i<pLC->GetItemCount();i++)
		{
			if(pLC->GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED)
			{
				GetSelectValue(value,pLC,i,iSubItem,dwValueType);
				st=st+value+_T(",");
			}
		}
	}
}
void CTemplateDlg::GetSelectValue(CString&st,CListBox*pLB,int iItem,DWORD dwValueType)
{
	switch(dwValueType)
	{
	case VT_Index:if(iItem<0) st=_T("-1");
				  else st.Format(_T("%d"),iItem);
				  break;
	case VT_Text:if(iItem<0) st=_T("iItem行号错误");
				 else pLB->GetText(iItem,st);break;
	case VT_ItemData:st.Format(_T("%d"),pLB->GetItemData(iItem));break;
	case VT_ItemDataString:st=*((CString*)pLB->GetItemData(iItem));break;
	default:st=_T("");
	}
}
void CTemplateDlg::GetSelectValue(CString&st,CListCtrl*pLC,int iItem,int iSubItem,DWORD dwValueType)
{
	switch(dwValueType)
	{
	case VT_Index:if(iItem<0) st=_T("-1");
				  else st.Format(_T("%d"),iItem);
				  break;
	case VT_Text:if(iItem<0) st=_T("iItem行号错误");
				 else st=pLC->GetItemText(iItem,iSubItem);break;
	case VT_ItemData:st.Format(_T("%d"),pLC->GetItemData(iItem));break;
	case VT_ItemDataString:st=*((CString*)pLC->GetItemData(iItem));break;
	default:st=_T("");
	}
}
void CTemplateDlg::GetSelectValue(CString&st,CComboBox*pCB,DWORD dwValueType)
{
	switch(dwValueType)
	{
	case VT_Index:st.Format(_T("%d"),pCB->GetCurSel());break;
	case VT_Text:pCB->GetWindowText(st);break;
	case VT_ItemData:st.Format(_T("%d"),pCB->GetItemData(pCB->GetCurSel()));break;
	case VT_ItemDataString:st=*((CString*)pCB->GetItemData(pCB->GetCurSel()));break;
	default:st=_T("");
	}
}
void CTemplateDlg::GetDateTime(CString&st,CDateTimeCtrl*pDT,DWORD dwValueType)
{
	CTime time;
	pDT->GetTime(time);
	switch(dwValueType)
	{
	case VT_DateTime:st=time.Format(_T("%Y-%m-%d %H:%M:%S"));break;
	case VT_Date:st=time.Format(_T("%Y-%m-%d"));break;
	case VT_Time:st=time.Format(_T("%H:%M:%S"));break;
	default:st=_T("");
	}
}
bool CTemplateDlg::MakeSureText(BYTE i,CString&text,DWORD dwValueType,CString&error)
{
	bool isLong=CStringExt::IsLong(text);
	bool isDWORD=CStringExt::IsDWORD(text);
	long l=_tstol(text);
	DWORD dw=(DWORD)l;
	switch(dwValueType&VT_GetType)
	{
	case VT_Text:
		{
			CString str;
			m_pStatics[i].GetWindowText(str);
			error.Format(_T("\"%s\"不能为空"),str);return (dwValueType&VT_NoZero)==NULL || text.GetLength()>0;
		}
	case VT_Char:error=_T("请输入单个ASCII字符");return strlen((LPCSTR)(LPCTSTR)text)==1;
	case VT_SignedChar:error=_T("请输入整数(-128-127)");return isLong && l<128 && l>-129;
	case VT_BYTE:error=_T("请输入非负整数(0-255)");return isDWORD && dw<256;
	case VT_Short:error=_T("请输入整数(-32768-32767)");return isLong && l<32768l && l>-32769l;
	case VT_WORD:error=_T("请输入非负整数(0-65535)");return isDWORD && dw<(DWORD)65536;
	case VT_Long:error=_T("请输入整数(-2147483648-2147483647)");return isLong;
	case VT_DWORD:error=_T("请输入非负整数(0-4294967295)");return isDWORD;
	case VT_LongLong:error=_T("请输入整数(-9223372036854775808-9223372036854775807)");return CStringExt::IsLongLong(text);
	case VT_Float:error=_T("请输入单精度浮点数(3.4E +/- 38 (7 digits))");return CStringExt::IsDouble(text);
	case VT_Double:error=_T("请输入双精度浮点数(1.7E +/- 308 (15 digits))");return CStringExt::IsDouble(text);
		/*	case VT_URLHTTPWeak://弱http,暂不支持,按VT_Text
		//如localhost->http://localhost/,163->http://www.163.net,
		//163.com->http://www.163.com
		case VT_URLHTTPStrong://http,暂不支持,按VT_Text
		//如localhost(error),163.com(error),
		//http://localhost/(ok),http://www.163.com(OK)
		case VT_URLFTPWeak://暂不支持,按VT_Text
		case VT_URLFTPStrong://*/
	default:return true;
	}
}
void CTemplateDlg::DoSaveData()
{
	m_strValue=_T("");
	CString st;
	CString error;
	CString label;
	for(BYTE i=0;i<m_nNum;i++)
	{
		m_pStatics[i].GetWindowText(label);
		switch(m_pdwCtrlTypes[i])
		{
		case CT_Edit:
			((CEdit*)m_pCtrls[i])->GetWindowText(st);
			if(!MakeSureText(i,st,m_pdwValueTypes[i],error))
			{
				MessageBox(error,_T("输入错误:")+label,MB_OK|MB_ICONERROR);
				((CEdit*)m_pCtrls[i])->SetFocus();
				m_bError=true;
				return;
			}
			break;
		case CT_CheckBox:st.Format(_T("%d"),((CButton*)m_pCtrls[i])->GetCheck()==BST_CHECKED?1:0);break;
		case CT_ComboBoxDropList:
		case CT_ComboBoxDropdown:
			GetSelectValue(st,(CComboBox*)m_pCtrls[i],m_pdwValueTypes[i] & VT_GetType);
			if(!MakeSureText(i,st,m_pdwValueTypes[i],error))
			{
				MessageBox(error,_T("输入错误:")+label,MB_OK|MB_ICONERROR);
				((CComboBox*)m_pCtrls[i])->SetFocus();
				m_bError=true;
				return;
			}
			break;
		case CT_Date:GetDateTime(st,(CDateTimeCtrl*)m_pCtrls[i],VT_Date);break;
		case CT_Time:GetDateTime(st,(CDateTimeCtrl*)m_pCtrls[i],VT_Time);break;
		default:st=_T("");
		}
		m_strValue=m_strValue+st+m_strSep;
	}
}
void CTemplateDlg::OnOK() 
{
	m_bError=false;
	DoSaveData();
	if(m_bError) return;
	EndDialog(IDOK);
}
void CTemplateDlg::Free()
{
	if(m_pStatics){delete []m_pStatics;m_pStatics=NULL;}
	if(m_pCtrls)
	{
		for(BYTE i=0;i<m_nNum;i++)
		{
			switch(m_pdwCtrlTypes[i])
			{
			case CT_None:break;
			case CT_Label:delete (CStatic*)m_pCtrls[i];break;
			case CT_Button:delete (CButton*)m_pCtrls[i];break;
			case CT_Edit:delete (CEdit*)m_pCtrls[i];break;
			case CT_CheckBox:delete (CButton*)m_pCtrls[i];break;
			case CT_ComboBoxDropList:
			case CT_ComboBoxDropdown:
				if ((m_pdwValueTypes[i] & VT_GetType)==VT_ItemDataString)
				{
					CComboBox*pCB=(CComboBox*)m_pCtrls[i];
					for (int j=0;j<pCB->GetCount();j++) delete (CString*)pCB->GetItemData(j);
				}
				delete (CComboBox*)m_pCtrls[i];
				break;
			case CT_Date:
			case CT_Time:delete (CDateTimeCtrl*)m_pCtrls[i];break;
			}
		}
		delete []m_pCtrls;m_pCtrls=NULL;
	}
	if(m_pdwCtrlTypes){delete []m_pdwCtrlTypes;m_pdwCtrlTypes=NULL;}
	if(m_pdwOptionalStyles){delete []m_pdwOptionalStyles;m_pdwOptionalStyles=NULL;}
	if(m_pdwSelectTypes){delete []m_pdwSelectTypes;m_pdwSelectTypes=NULL;}
	if(m_pdwValueTypes){delete []m_pdwValueTypes;m_pdwValueTypes=NULL;}
	if(m_pDefaultValues){delete []m_pDefaultValues;m_pDefaultValues=NULL;}
	m_nNum=0;
}
void CTemplateDlg::OnDestroy() {Free();CDialog::OnDestroy();}
void CTemplateDlg::DoTemplate(DWORD dwStyle)
{
	m_builder.SetDataLength(10240);
	CRect rc(0,0,600,400);
	m_builder.BeginTemplate(DS_MODALFRAME | dwStyle,&rc,m_strTitle);
	rc=CRect(0,0,50,18);
	//WS_BORDER
	m_builder.AddItem(CDlgTemplateBuilder::BUTTON,WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_CENTER|BS_VCENTER|BS_TEXT,&rc,_T("确定"),IDOK);
	m_builder.AddItem(CDlgTemplateBuilder::BUTTON,WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_CENTER|BS_VCENTER|BS_TEXT,&rc,_T("取消"),IDCANCEL);
}
int CTemplateDlg::DoModal() 
{
	DoTemplate();
	InitModalIndirect(m_builder.GetTemplate());
	return CDialog::DoModal();
}
BOOL CTemplateDlg::Create(CWnd* pParentWnd,DWORD dwStyle)
{
	m_pParentWnd = pParentWnd;
	DoTemplate(dwStyle);
	return CreateIndirect(m_builder.GetTemplate(),pParentWnd);
}
void CTemplateDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		KillTimer(1);
		for(int i=0;i<m_nNum;i++)
		{
			if(m_pCtrls[i])
			{
				m_pCtrls[i]->SetFocus();
				break;
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

//条目索引和ID映射开始
UINT CTemplateDlg::NewControlID()
{
	return m_uiControlID++;
}
CString CTemplateDlg::NewControlIDString()
{
	CString ret;
	ret.Format(_T("%d"),NewControlID());
	return ret;
}
void CTemplateDlg::SetRelation_Enabled_ID(UINT idDetected,UINT idHandled,bool bSame)
{
	SetRelation_Enabled(idDetected, idHandled, bSame);
}
void CTemplateDlg::SetRelation_Visible_ID(UINT idDetected,UINT idHandled,bool bSame)
{
	SetRelation_Visible(idDetected, idHandled, bSame);
}

void CTemplateDlg::SetRelation_Enabled_ID(UINT idDetected,UINT idHandled,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Enabled(-1, idDetected, idHandled, values, sep);
}
void CTemplateDlg::SetRelation_Visible_ID(UINT idDetected,UINT idHandled,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Visible(-1, idDetected, idHandled, values, sep);
}

void CTemplateDlg::SetRelation_Value_ID(UINT idDetected,UINT idHandled,LPCTSTR value,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Value(-1,idDetected,-1, idHandled, value,values, sep);
}

void CTemplateDlg::SetRelation_Setup_ID(UINT idDetected,UINT idHandled,LPCTSTR newValue,LPCTSTR newValueSep,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Setup(-1,idDetected,-1, idHandled, newValue, newValueSep,values, sep);
}
//以下函数必须在SetupUI之后调用
void CTemplateDlg::SetMapIndexID(UINT index, UINT id)
{
	m_mapIndexID.SetAt(index,id);
}
UINT CTemplateDlg::GetIndexByID(UINT id) const
{
	UINT _index,_id;
	POSITION pos = m_mapIndexID.GetStartPosition();
	while (pos != NULL)
	{
		m_mapIndexID.GetNextAssoc(pos, _index, _id);
		if(_id == id) return _index;
	}
	return -1;
}
UINT CTemplateDlg::GetIDByIndex(UINT index) const
{
	UINT v;
	if (m_mapIndexID.Lookup(index,v)) return v;
	return -1;
}
void CTemplateDlg::SetRelation_Enabled_Index(UINT indexDetected,UINT indexHandled,bool bSame)
{
	SetRelation_Enabled(GetIDByIndex(indexDetected), GetIDByIndex(indexHandled), bSame);
}
void CTemplateDlg::SetRelation_Visible_Index(UINT indexDetected,UINT indexHandled,bool bSame)
{
	SetRelation_Visible(GetIDByIndex(indexDetected), GetIDByIndex(indexHandled), bSame);
}

void CTemplateDlg::SetRelation_Enabled_Index(UINT indexDetected,UINT indexHandled,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Enabled(indexDetected, GetIDByIndex(indexDetected), GetIDByIndex(indexHandled), values, sep);
}
void CTemplateDlg::SetRelation_Visible_Index(UINT indexDetected,UINT indexHandled,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Visible(indexDetected, GetIDByIndex(indexDetected), GetIDByIndex(indexHandled), values, sep);
}

void CTemplateDlg::SetRelation_Value_Index(UINT indexDetected,UINT indexHandled,LPCTSTR value,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Value(indexDetected,GetIDByIndex(indexDetected),indexHandled, GetIDByIndex(indexHandled), value,values, sep);
}

void CTemplateDlg::SetRelation_Setup_Index(UINT indexDetected,UINT indexHandled,LPCTSTR newValue,LPCTSTR newValueSep,LPCTSTR values,LPCTSTR sep)
{
	SetRelation_Setup(indexDetected,GetIDByIndex(indexDetected),indexHandled, GetIDByIndex(indexHandled), newValue, newValueSep,values, sep);
}

//条目索引和ID映射结束