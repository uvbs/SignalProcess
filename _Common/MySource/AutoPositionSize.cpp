#include "stdafx.h"
#include "AutoPositionSize.h"


struct structAutoPositionSize
{
	CWnd* pParent;
	DWORD Value;//如果pParent为空，表示资源id，否则为窗口指针
	enumAutoPositionSizeType eX;
	enumAutoPositionSizeType eY;
	CRect rcInit;
};

CAutoPositionSize::CAutoPositionSize():m_pParent(NULL),m_bInit(false)
{
}
CAutoPositionSize::~CAutoPositionSize()
{
	DeleteAll();
}

long CAutoPositionSize::_Index(DWORD uiResID, CWnd* parent)
{
	for(int i=0; i<m_arrInfos.GetCount(); ++i)
	{
		structAutoPositionSize* pItem = (structAutoPositionSize*)m_arrInfos.GetAt(i);
		if(pItem->pParent == parent && pItem->Value == uiResID) return i;
	}
	return -1;
}
long CAutoPositionSize::_Index(CWnd* pWnd)
{
	return _Index((DWORD)pWnd, NULL);
}


void CAutoPositionSize::Auto(DWORD uiResID, CWnd* parent, enumAutoPositionSizeType eX, enumAutoPositionSizeType eY)
{
	long index = _Index(uiResID, parent);
	structAutoPositionSize* pItem = NULL;
	if(index > -1) pItem = (structAutoPositionSize*)m_arrInfos.GetAt(index);
	else
	{
		pItem = new structAutoPositionSize;
		m_arrInfos.Add((DWORD)pItem);
		pItem->pParent = parent;
		pItem->Value = uiResID;
	}
	pItem->eX = eX;
	pItem->eY = eY;
}
void CAutoPositionSize::Auto(CWnd* pWnd, enumAutoPositionSizeType eX, enumAutoPositionSizeType eY)
{
	Auto((DWORD)pWnd, NULL, eX, eY);
}

void CAutoPositionSize::Delete(DWORD uiResID, CWnd* parent)
{
	long index = _Index(uiResID, parent);
	if(index > -1)
	{
		delete (structAutoPositionSize*)m_arrInfos.GetAt(index);
		m_arrInfos.RemoveAt(index);
	}
}
void CAutoPositionSize::Delete(CWnd* pWnd)
{
	Delete((DWORD)pWnd, NULL);
}

void CAutoPositionSize::DeleteAll()
{
	for(int i=0; i<m_arrInfos.GetCount(); ++i)
	{
		delete (structAutoPositionSize*)m_arrInfos.GetAt(i);
	}
	m_arrInfos.RemoveAll();
}

void CAutoPositionSize::InitInfo(CWnd* parent)
{
	m_pParent = parent;
	if(m_pParent == NULL) return;

	if(!::IsWindow(m_pParent->GetSafeHwnd())) return;
	m_pParent->GetWindowRect(m_rcParentInit);

	for(int i=0; i<m_arrInfos.GetCount(); ++i)
	{
		structAutoPositionSize* pItem = (structAutoPositionSize*)m_arrInfos.GetAt(i);
		CWnd* pWnd = pItem->pParent?pItem->pParent->GetDlgItem(pItem->Value):((CWnd*)pItem->Value);
		if(!::IsWindow(pWnd->GetSafeHwnd())) return;
		pWnd->GetWindowRect(pItem->rcInit);
		CWnd* pParent = pWnd->GetParent();
		if(pParent) pParent->ScreenToClient(pItem->rcInit);
	}

	m_bInit = true;
}
void CAutoPositionSize::_PS(enumAutoPositionSizeType e, int ov, long& lt, long& rb)
{
	switch(e)
	{
	case APST_LEAVE:break;
	case APST_SIZE:rb += ov;break;
	case APST_POSITION:lt += ov;rb += ov;break;
	}
}
void CAutoPositionSize::AutoPositionSize()
{
	if(!m_bInit) return;

	CRect rc;
	m_pParent->GetWindowRect(rc);
	int ox = rc.Width() - m_rcParentInit.Width();
	int oy = rc.Height() - m_rcParentInit.Height();

	for(int i=0; i<m_arrInfos.GetCount(); ++i)
	{
		structAutoPositionSize* pItem = (structAutoPositionSize*)m_arrInfos.GetAt(i);
		CWnd* pWnd = pItem->pParent?pItem->pParent->GetDlgItem(pItem->Value):((CWnd*)pItem->Value);

		rc = pItem->rcInit;
		_PS(pItem->eX, ox, rc.left, rc.right);
		_PS(pItem->eY, oy, rc.top, rc.bottom);
		pWnd->MoveWindow(rc);
	}

	m_pParent->Invalidate();
}
void CAutoPositionSize::ShowWindow(int swCommand)
{
	for(int i=0; i<m_arrInfos.GetCount(); ++i)
	{
		structAutoPositionSize* pItem = (structAutoPositionSize*)m_arrInfos.GetAt(i);
		CWnd* pWnd = pItem->pParent?pItem->pParent->GetDlgItem(pItem->Value):((CWnd*)pItem->Value);
		pWnd->ShowWindow(swCommand);
	}
}