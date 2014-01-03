
/*
	CTemplateDlg dlg(this);
	dlg.SetTitle(_T("TemplateDlg"));
	UINT id=CTemplateDlg::ID_Begin;
	CString ids[10];
	for(int i=0; i<10; i++)
	{
		ids[i].Format(_T("%d"),id+i);
	}
	dlg.SetCtrlInfo(21,_T("edit"),CTemplateDlg::CT_Edit,ES_READONLY|WS_BORDER,NULL,CTemplateDlg::VT_Text|CTemplateDlg::VT_NoZero,_T("DefaultEditValue|12345678"),//text,dwctrltype,dwOptionalStyle,dwselecttype,dwvaluetype(|后的ID可省)
		_T("注意:"),CTemplateDlg::CT_Label,NULL,NULL,NULL,_T("这是一个普通的Label"),
		_T("这是一个普通的Label，这一行就只有这个Label"),CTemplateDlg::CT_None,NULL,NULL,NULL,NULL,
		_T("注意:"),CTemplateDlg::CT_Label,NULL,NULL,NULL,_T("又是一个普通的Label"),
		_T("checkbox"),CTemplateDlg::CT_CheckBox,NULL,NULL,NULL,_T("1|this is a checkbox|")+ids[0],//value(1,0:是否勾选)[|windowtext|ID]
		_T("ComboBoxDropList"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemData,_T("30|it1:10,it2:20,it3:30|")+ids[1],//value|list[|ID]
		_T("ComboBoxDropList"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_Index,_T("0|it1,it2,it3"),
		_T("ComboBoxDropdown"),CTemplateDlg::CT_ComboBoxDropdown,NULL,NULL,CTemplateDlg::VT_Text|CTemplateDlg::VT_NoZero,_T("it2|it1,it2,it3"),
		_T("date"),CTemplateDlg::CT_Date,NULL,NULL,NULL,_T("2008-9-10|")+ids[2],//value[|ID]
		_T("time"),CTemplateDlg::CT_Time,NULL,NULL,NULL,_T("08:9:10|")+ids[3],
		_T("char"),CTemplateDlg::CT_Edit,WS_DISABLED,NULL,CTemplateDlg::VT_Char,_T("c"),
		_T("signedchar"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_SignedChar,_T("127"),
		_T("BYTE"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_BYTE,_T("255"),
		_T("short"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Short,_T("32767"),
		_T("WORD"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_WORD,_T("65535"),
		_T("long"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Long,_T("2147483647|")+ids[4],
		_T("DWORD"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_DWORD,_T("4294967295|")+ids[5],
		_T("longlong"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_LongLong,_T("9223372036854775807|")+ids[6],
		_T("float"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Float,_T("3.4E +/- 38 (7 digits)"),
		_T("double"),CTemplateDlg::CT_Edit,NULL,NULL,CTemplateDlg::VT_Double,_T("1.7E +/- 308 (15 digits)"),
		_T("ComboBoxDropListString"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_ItemDataString,_T("V2|it1:V1,it2:V2,it3:V3")//value|list[|ID]
		);
	//checkbox
	dlg.SetRelation_Enabled(id,id+1,true);
	dlg.SetRelation_Enabled(id,id+3,false);
	dlg.SetRelation_Visible(id,id+2,true);
	dlg.SetRelation_Visible(id,id+4,false);
	//combobox
	dlg.SetRelation_Enabled(5,id+1,id+5,_T("10,30"),_T(","));//VT_ItemData
	dlg.SetRelation_Enabled(5,id+1,id+6,_T("10"),_T(","));//VT_ItemData
	dlg.SetRelation_Visible(5,id+1,id+5,_T("20"),_T(","));//VT_ItemData
	dlg.SetRelation_Visible(5,id+1,id+6,_T("30"),_T(","));//VT_ItemData
	if(dlg.DoModal()==IDOK) MessageBox(dlg.m_strValue);
	CString values[21];
	CStringExt::Split(dlg.m_strValue,dlg.m_strSep,21,values);
	values[0]:第一个控件的值
	values[20]:最后一个控件，即"ComboBoxDropListString"对应的控件的值
*/

/*
	//SetRelation_Setup演示，以省市县选择为例
	CTemplateDlg dlg(this);
	dlg.SetTitle(_T("TemplateDlg"));
	UINT id=CTemplateDlg::ID_Begin;
	CString ids[10];
	for(int i=0; i<10; i++)
	{
		ids[i].Format(_T("%d"),id+i);
	}
	dlg.SetCtrlInfo(3,
		_T("ComboBoxDropList"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_Text,_T("省1|省1,省2,省3|")+ids[0],//value|list[|ID]
		_T("ComboBoxDropList"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_Text,_T("||")+ids[1],
		_T("ComboBoxDropdown"),CTemplateDlg::CT_ComboBoxDropdown,NULL,NULL,CTemplateDlg::VT_Text,_T("||")+ids[2]
		);

	//3个省的市信息
	dlg.SetRelation_Setup(0,id,1,id+1,_T("市11|市11,市12,市13"),NULL,_T("省1"),_T(","));
	dlg.SetRelation_Setup(0,id,1,id+1,_T("市21|市21,市22,市23"),NULL,_T("省2"),_T(","));
	dlg.SetRelation_Setup(0,id,1,id+1,_T("市31|市31,市32,市33"),NULL,_T("省3"),_T(","));

	//9个市的县信息
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县111|县111,县112,县113"),NULL,_T("市11"),_T(","));
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县121|县121,县122,县113"),NULL,_T("市12"),_T(","));
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县131|县131,县132,县113"),NULL,_T("市13"),_T(","));

	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县211|县211,县212,县213"),NULL,_T("市21"),_T(","));
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县221|县221,县222,县213"),NULL,_T("市22"),_T(","));
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县231|县231,县232,县213"),NULL,_T("市23"),_T(","));

	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县311|县311,县312,县313"),NULL,_T("市31"),_T(","));
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县321|县321,县322,县313"),NULL,_T("市32"),_T(","));
	dlg.SetRelation_Setup(1,id+1,2,id+2,_T("县331|县331,县332,县313"),NULL,_T("市33"),_T(","));
//*/

/*
//从CTemplateDlg派生类演示
class CMyTDlg : public CTemplateDlg
{
public:
	CMyTDlg(CWnd* pParent = NULL);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnEnChangeEdit();
};

CMyTDlg::CMyTDlg(CWnd* pParent):CTemplateDlg(pParent)
{
	UINT id=CTemplateDlg::ID_Begin;
	CString ids[10];
	for(int i=0; i<10; i++)
	{
		ids[i].Format(_T("%d"),id+i);
	}
	SetCtrlInfo(2,
		_T("ComboBoxDropList"),CTemplateDlg::CT_ComboBoxDropList,NULL,NULL,CTemplateDlg::VT_Text,_T("省1|省1,省2,省3|")+ids[0],
		_T("Text Edit"),CTemplateDlg::CT_Edit,WS_BORDER,NULL,CTemplateDlg::VT_Text,_T("this is a text control.|")+ids[1]
	);
}

BEGIN_MESSAGE_MAP(CMyTDlg, CTemplateDlg)
	ON_CBN_SELCHANGE(CTemplateDlg::ID_Begin, &CMyTDlg::OnCbnSelchangeCombo)
	ON_EN_CHANGE(CTemplateDlg::ID_Begin+1, &CMyTDlg::OnEnChangeEdit)
END_MESSAGE_MAP()

void CMyTDlg::OnCbnSelchangeCombo()
{
	MessageBox(_T("selection changed!"));
}

void CMyTDlg::OnEnChangeEdit()
{
	MessageBox(_T("content changed!"));
}

//测试：
	CMyTDlg dlg(this);
	dlg.DoModal();
	或者：
	CMyTDlg m_dlg;
	...
	m_dlg.Create(this);
	m_dlg.ShowWindow(SW_SHOW);
//*/

#if !defined(AFX_TemplateDlg_H__D0874304_E71E_4B46_BC84_6AB7150EE4EC__INCLUDED_)
#define AFX_TemplateDlg_H__D0874304_E71E_4B46_BC84_6AB7150EE4EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateDlg.h : header file
//

////////////////////////////////////////////////////////////////////////////////////////////////
//对于SetRelation系列函数，由于没有为条目的索引和条目对应的控件的ID设置映射关系，所以在设置函数中，
//存在同时要对同一个条目设置索引和控件ID的情况，调用起来参数稍长，这一点需要改进
////////////////////////////////////////////////////////////////////////////////////////////////

//CTemplateDlg同时支持模式和非模式

class CDlgTemplateBuilder
{
public:
	enum enumWPAN// Windows predefined atom names
	{
		BUTTON=0x0080,
		EDIT,
		STATIC,
		LISTBOX,
		SCROLLBAR,
		COMBOBOX
	};
public:
	CDlgTemplateBuilder(DWORD dwLength=0);
	~CDlgTemplateBuilder();
public:// functions to build the template
	void SetDataLength(DWORD dwLength);
	DLGTEMPLATE* GetTemplate();
	DLGTEMPLATE* BeginTemplate(DWORD dwStyle,LPCRECT rc,LPCTSTR caption,DWORD dwStyleEx=0);
	void AddItem(WORD wType, DWORD dwStyle,LPCRECT rc,LPCTSTR text,WORD nID=-1,DWORD dwStyleEx=0);
	void AddItem(WORD wType, DWORD dwStyle,LPCRECT rc,WORD nResID,WORD nID=-1,DWORD dwStyleEx=0);
protected:
	WORD* AddText(WORD* buf,LPCTSTR text);
	void AddItemTemplate(WORD wType, DWORD dwStyle,LPCRECT rc,WORD nID,DWORD dwStyleEx);	
	WORD* AlignDWORD(WORD* ptr);// align ptr to nearest DWORD
protected:
	WORD* m_pBuffer;// internal buffer holds dialog template
	WORD* m_pNext;// next WORD to copy stuff
	WORD* m_pEndBuf;
};

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog

class CTemplateDlg : public CDialog
{
public:
	static int GetFirstSel(CListBox*pLB);
	static int GetCurSel(CListCtrl&m_lst);
	static bool _IsControlVisible(CWnd*pWnd);

public:
	enum enumSelectType//ListBox,ListCtrl有效
	{
		ST_Single=0x00000001,
		ST_Multiple=0x00000002,
		ST_Extended=0x00000004,//整行选中,和返回结果无影响
		ST_None=0x00000008
	};
	enum enumValueType//前8b特殊
	{
		VT_ItemData=0x00000000,//GetItemData
		VT_ItemDataString,//GetItemData对应的必须是一个字符串
		VT_Index,//has a list
		VT_Text,//暂不支持长度和数值范围
		//以下是Edit专用,暂不支持长度和数值范围
		VT_Char,
		VT_SignedChar,
		VT_BYTE,
		VT_Short,
		VT_WORD,
		VT_Long,
		VT_DWORD,
		VT_LongLong,
		VT_Float,
		VT_Double,
		VT_URLHTTPWeak,//弱http,暂不支持,按VT_Text
		//如localhost->http://localhost/,163->http://www.163.net,
		//163.com->http://www.163.com
		VT_URLHTTPStrong,//http,暂不支持,按VT_Text
		//如localhost(error),163.com(error),
		//http://localhost/(ok),http://www.163.com(OK)
		VT_URLFTPWeak,//暂不支持,按VT_Text
		VT_URLFTPStrong,//暂不支持,按VT_Text
		//时间日期需要注意返回的格式
		VT_DateTime,
		VT_Date,
		VT_Time,

		VT_Length=0x01000000,//检查长度范围(-1表不限，1到-1，-1到10等)仅对edit的VT_Text有效，暂不支持
		VT_NoZero=0x02000000,//VT_Text值类型时，不能为空

		VT_GetType=0x00FFFFFF//特殊用途
	};
	enum enumControlType
	{
		CT_None=0x00000000,//此行是一个label
		CT_Label,
		CT_Button,//相关设置同CT_Label
		CT_Edit,
		CT_CheckBox,
		CT_ComboBoxDropList,//可输入的下拉框
		CT_ComboBoxDropdown,//不可输入的下拉框
		CT_Date,
		CT_Time
	};

public:
	enum enumRangeControlID
	{
		ID_Begin=WM_USER+101,
		ID_End=WM_USER+10101,
	};

	//条目索引和ID映射开始
public:
	UINT NewControlID();//产生新的ID号
	CString NewControlIDString();

	void SetRelation_Enabled_ID(UINT idDetected,UINT idHandled,bool bSame);//for checkbox
	void SetRelation_Visible_ID(UINT idDetected,UINT idHandled,bool bSame);//for checkbox

	void SetRelation_Enabled_ID(UINT idDetected,UINT idHandled,LPCTSTR values,LPCTSTR sep);//for ComboBox
	void SetRelation_Visible_ID(UINT idDetected,UINT idHandled,LPCTSTR values,LPCTSTR sep);//for ComboBox

	void SetRelation_Value_ID(UINT idDetected,UINT idHandled,LPCTSTR value,LPCTSTR values,LPCTSTR sep);//目前i必须是combobox

	void SetRelation_Setup_ID(UINT idDetected,UINT idHandled,LPCTSTR newValue,LPCTSTR newValueSep,LPCTSTR values,LPCTSTR sep);//目前i和j必须是combobox

protected:
	CMap<UINT,UINT,UINT,UINT> m_mapIndexID;
	UINT m_uiControlID;

	void SetMapIndexID(UINT index, UINT id);

	//以下函数必须在SetupUI之后调用
	UINT GetIndexByID(UINT id) const;//必须在SetupUI之后调用
	UINT GetIDByIndex(UINT index) const;//必须在SetupUI之后调用

	void SetRelation_Enabled_Index(UINT indexDetected,UINT indexHandled,bool bSame);//for checkbox,必须在SetupUI之后调用
	void SetRelation_Visible_Index(UINT indexDetected,UINT indexHandled,bool bSame);//for checkbox,必须在SetupUI之后调用

	void SetRelation_Enabled_Index(UINT indexDetected,UINT indexHandled,LPCTSTR values,LPCTSTR sep);//for ComboBox,必须在SetupUI之后调用
	void SetRelation_Visible_Index(UINT indexDetected,UINT indexHandled,LPCTSTR values,LPCTSTR sep);//for ComboBox,必须在SetupUI之后调用

	void SetRelation_Value_Index(UINT indexDetected,UINT indexHandled,LPCTSTR value,LPCTSTR values,LPCTSTR sep);//目前i必须是combobox,必须在SetupUI之后调用

	void SetRelation_Setup_Index(UINT indexDetected,UINT indexHandled,LPCTSTR newValue,LPCTSTR newValueSep,LPCTSTR values,LPCTSTR sep);//目前i和j必须是combobox,必须在SetupUI之后调用

	//条目索引和ID映射结束

public:
	//设置使能关系
	//1.bSame为true：若ID为uiIDDetected的checkbox被选中，则uiIDHandled对应的窗口被激活，否则被灰掉
	//2.bSame为false：若ID为uiIDDetected的checkbox被选中，则uiIDHandled对应的窗口被灰掉，否则被激活
	void SetRelation_Enabled(UINT uiIDDetected,UINT uiIDHandled,bool bSame);//for checkbox,为兼容老版本和内部使用
	//设置可视关系
	//1.bSame为true：若ID为uiIDDetected的checkbox被选中，则uiIDHandled对应的窗口被显示，否则被隐藏
	//2.bSame为false：若ID为uiIDDetected的checkbox被选中，则uiIDHandled对应的窗口被隐藏，否则被显示
	void SetRelation_Visible(UINT uiIDDetected,UINT uiIDHandled,bool bSame);//for checkbox,为兼容老版本和内部使用

	//设置使能关系
	//values是ComboBox的值列表,以sep分割，该值应当和uiIDDetected的值类型对应
	//如果当前值在列表中，则使能uiIDHandled，否则灰掉uiIDHandled
	void SetRelation_Enabled(int i,UINT uiIDDetected,UINT uiIDHandled,LPCTSTR values,LPCTSTR sep);//for ComboBox,为兼容老版本和内部使用
	//设置可视关系
	void SetRelation_Visible(int i,UINT uiIDDetected,UINT uiIDHandled,LPCTSTR values,LPCTSTR sep);//for ComboBox,为兼容老版本和内部使用

	//设置值关系
	void SetRelation_Value(int i,UINT uiIDDetected,int j,UINT uiIDHandled,LPCTSTR value,LPCTSTR values,LPCTSTR sep);//目前i必须是combobox,为兼容老版本和内部使用

	//设置列表框下拉条目内容重建关系
	//newValue：参考示例中的相关条目，但不支持最后的控件ID
	//newValueSep：保留，目前无效
	void SetRelation_Setup(int i,UINT uiIDDetected,int j,UINT uiIDHandled,LPCTSTR newValue,LPCTSTR newValueSep,LPCTSTR values,LPCTSTR sep);//目前i和j必须是combobox,为兼容老版本和内部使用
/*
	DWORD NewGroup();
	void AddToGroup(DWORD dwGroup, int i);
	void AddToGroup(DWORD dwGroup, UINT id);
//*/
protected:
	void MakeSureRelation();
	void _RefreshDialog();//根据可视条件，刷新各子窗口位置和窗口大小，但不改变窗口的左上角坐标

	struct structRelationMapItem
	{
		UINT uiID;
		bool bSame;
	};
	typedef CArray<structRelationMapItem,structRelationMapItem> CTemplateDlg_MapItem;
	typedef CMap<UINT,UINT,CTemplateDlg_MapItem*,CTemplateDlg_MapItem*> CTemplateDlg_Map;
	CTemplateDlg_Map m_mapEnabled,m_mapVisible;
	void SaveRelationItem(CTemplateDlg_MapItem*pMapItem,UINT uiIDHandled,bool bSame);
	void Relation_Enabled(UINT id, CTemplateDlg_MapItem& item);
	void Relation_Visible(UINT id, CTemplateDlg_MapItem& item);
	void Relation_Enabled(UINT id, structRelationMapItem& item);
	void Relation_Visible(UINT id, structRelationMapItem& item);
	//有则返回，若无，若bNew为true增加后返回，否则返回NULL
	CTemplateDlg_MapItem* GetTemplateDlg_MapItem(CTemplateDlg_Map&map, UINT key, bool bAutoNew);

	struct structRelationMapItem_ComboBox
	{
		int i;
		UINT uiID;
		CString values;
		CString sep;
	};
	typedef CArray<structRelationMapItem_ComboBox,structRelationMapItem_ComboBox> CTemplateDlg_MapItem_ComboBox;
	typedef CMap<UINT,UINT,CTemplateDlg_MapItem_ComboBox*,CTemplateDlg_MapItem_ComboBox*> CTemplateDlg_Map_ComboBox;
	CTemplateDlg_Map_ComboBox m_mapEnabled_ComboBox,m_mapVisible_ComboBox;
	void SaveRelationItem(CTemplateDlg_MapItem_ComboBox*pMapItem,int i,UINT uiIDHandled,LPCTSTR values,LPCTSTR sep);
	void Relation_Enabled(UINT id, CTemplateDlg_MapItem_ComboBox& item);
	void Relation_Visible(UINT id, CTemplateDlg_MapItem_ComboBox& item);
	void Relation_Enabled(UINT id, structRelationMapItem_ComboBox& item);
	void Relation_Visible(UINT id, structRelationMapItem_ComboBox& item);
	//有则返回，若无，若bNew为true增加后返回，否则返回NULL
	CTemplateDlg_MapItem_ComboBox* GetTemplateDlg_MapItem(CTemplateDlg_Map_ComboBox&map, UINT key, bool bAutoNew);

	struct structValueMapItem
	{
		int i;
		UINT uiID;
		int j;
		CString value;
		CString values;
		CString sep;
	};
	typedef CArray<structValueMapItem,structValueMapItem> CTemplateDlg_MapItem_Value;
	typedef CMap<UINT,UINT,CTemplateDlg_MapItem_Value*,CTemplateDlg_MapItem_Value*> CTemplateDlg_Map_Value;
	CTemplateDlg_Map_Value m_mapValue;
	void Relation_Value(UINT id, CTemplateDlg_MapItem_Value& item);
	void Relation_Value(UINT id, structValueMapItem& item);
	//有则返回，若无，若bNew为true增加后返回，否则返回NULL
	CTemplateDlg_MapItem_Value* GetTemplateDlg_MapItem(CTemplateDlg_Map_Value&map, UINT key, bool bAutoNew);

	struct structSetupMapItem
	{
		int i;
		UINT uiID;
		int j;
		CString newValue;
		CString newValueSep;//保留
		CString values;
		CString sep;
	};
	typedef CArray<structSetupMapItem,structSetupMapItem> CTemplateDlg_MapItem_Setup;
	typedef CMap<UINT,UINT,CTemplateDlg_MapItem_Setup*,CTemplateDlg_MapItem_Setup*> CTemplateDlg_Map_Setup;
	CTemplateDlg_Map_Setup m_mapSetup;
	void Relation_Setup(UINT id, CTemplateDlg_MapItem_Setup& item);
	void Relation_Setup(UINT id, structSetupMapItem& item);
	//有则返回，若无，若bNew为true增加后返回，否则返回NULL
	CTemplateDlg_MapItem_Setup* GetTemplateDlg_MapItem(CTemplateDlg_Map_Setup&map, UINT key, bool bAutoNew);
public:
	CDlgTemplateBuilder m_builder;
	bool m_bEnableBtnOK;
	bool m_bEnableBtnCancel;
	bool m_bShowBtnOK;
	bool m_bShowBtnCancel;
	CString m_strTitle;
	CString m_strValue;//"value1;value2;",是点击确定后所保存的各控件的值，以m_strSep分割
	CString m_strSep;//默认为英文;号

// Construction
public:
	CTemplateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTemplateDlg();

	void SetTitle(LPCTSTR title);
	void SetDefaultFont(CFont* pFont){m_pDefaultFont=pFont;}
	void SetUsingOKFont(bool bUsingFont){m_bUsingOKFont=bUsingFont;}
	BOOL Create(CWnd* pParentWnd=NULL,DWORD dwStyle=WS_POPUP | WS_CAPTION | WS_SYSMENU);
	void SetCtrlInfo(BYTE nNum,...);//text,dwctrltype,dwOptionalStyle,dwselecttype,dwvaluetype,strDefault
	//strDefault:initSelect|item1:dword1,item2:dword2(:dword仅在valuetype为ItemData时有效)
	//2006-1-2(CT_Date),16:10:2(CT_Time)
	void SetCtrlInfo(BYTE nNum, const CString* pText, const DWORD* pdwCtrlType, const DWORD* pdwOptionalStyle, const DWORD* pdwSelectType, const DWORD* pdwValueType, const CString* pstrDefault);
	void SetDlgMargin(int l,int t,int r,int b);//dlg边距
	void SetColumn1Margin(int l,int t,int r,int b);
	void SetColumn2Width(int ew);
	void SetColumn1WH(int w,int h,bool bAutoSet);//不包括margin
	void SetSpacing(int hs,int vs);
	void SetCBHeight(int iCBHeight);//combobox高度
	void SetOKCancelTopPadding(int iTopPadding);

// Dialog Data
	//{{AFX_DATA(CTemplateDlg)
	CButton	m_btnOK;
	CButton	m_btnCancel;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void BeforeRelation(){}
	void DoSaveData();
	void DoTemplate(DWORD dwStyle=WS_POPUP | WS_CAPTION | WS_SYSMENU);
	void Free();
	void GetSelectValue(CString&st,CComboBox*pCB,DWORD dwValueType);
	void GetSelectValue(CString&st,CListCtrl*pLC,int iItem,int iSubItem,DWORD dwValueType);
	void GetSelectValue(CString&st,CListBox*pLB,int iItem,DWORD dwValueType);
	void GetSelectValue(CString&st,CListCtrl*pLC,int iSubItem,DWORD dwSelectType,DWORD dwValueType);
	void GetSelectValue(CString&st,CListBox*pLB,DWORD dwSelectType,DWORD dwValueType);
	void MakeSureC1WH();
	void MakeSureSys(CRect&rcDlg,POINT&ptOKCancel);
	void SetupUI();
	void SetupCB(CComboBox*pCB,DWORD dwValueType,LPCTSTR value);
	void SetCBDropListSel(CComboBox*pCB,DWORD dwValueType,LPCTSTR value);
	void SetDate(CDateTimeCtrl*pDT,LPCTSTR date);
	void SetTime(CDateTimeCtrl*pDT,LPCTSTR time);
	void GetDateTime(CString&st,CDateTimeCtrl*pDT,DWORD dwValueType);
	bool MakeSureText(BYTE i,CString&text,DWORD dwValueType,CString&error);//暂不支持长度和数值范围
	CString m_strText;
	BYTE m_nNum;
	CStatic*m_pStatics;
	CWnd**m_pCtrls;
	DWORD*m_pdwCtrlTypes,*m_pdwOptionalStyles,*m_pdwSelectTypes,*m_pdwValueTypes;
	CString*m_pDefaultValues;
	RECT m_rcDlgMargin,m_rcC1Margin;
	int m_iColumn2Width,m_iOKCancelTopPadding;
	POINT m_ptSpacing,m_ptC1;
	bool m_bAutoSet;
	CRect m_rcDlg;
	POINT m_ptDlgClient,m_ptOKCancel;
	int m_iCBHeight;
	CFont*m_pDefaultFont;
	bool m_bError;
	bool m_bUsingOKFont;//使用OK按钮的字体
	// Generated message map functions
	//{{AFX_MSG(CTemplateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRangeIDs(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TemplateDlg_H__D0874304_E71E_4B46_BC84_6AB7150EE4EC__INCLUDED_)
