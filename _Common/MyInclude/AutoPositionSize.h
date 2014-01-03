

#ifndef TWEN_SDK_AutoPositionSize
#define TWEN_SDK_AutoPositionSize

#include "afxcoll.h"

enum enumAutoPositionSizeType
{
	APST_LEAVE,//不改变
	APST_SIZE,//改变大小
	APST_POSITION,//改变位置
};

class CAutoPositionSize
{
public:
	CAutoPositionSize();
	~CAutoPositionSize();

	void Auto(DWORD uiResID, CWnd* parent, enumAutoPositionSizeType eX, enumAutoPositionSizeType eY);
	void Auto(CWnd* pWnd, enumAutoPositionSizeType eX, enumAutoPositionSizeType eY);//传入的pWnd要持久保存，所以不要传入GetDlgItem(IDC_XXX)的返回值

	void Delete(DWORD uiResID, CWnd* parent);
	void Delete(CWnd* pWnd);

	void DeleteAll();

	void InitInfo(CWnd* parent);
	void AutoPositionSize();//在onresized中调用

	void ShowWindow(int swCommand);

	inline bool IsInitialized() const{return m_bInit;}

private:
	long _Index(DWORD uiResID, CWnd* parent);
	long _Index(CWnd* pWnd);
	void _PS(enumAutoPositionSizeType e, int ov, long& lt, long& rb);
	CArray<DWORD, DWORD> m_arrInfos;
	CWnd* m_pParent;
	CRect m_rcParentInit;
	bool m_bInit;
};

#endif