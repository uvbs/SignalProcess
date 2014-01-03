
#include "stdafx.h"
#include "SystemTray.h"

CSystemTray::CSystemTray()
{
	bCanUse=false;
}
CSystemTray::~CSystemTray()
{
}
void CSystemTray::Add(HWND hWnd,HICON hIcon,UINT IDIcon,DWORD wm_systemtray,LPCTSTR strTip)
{
	memset(&nid,0,sizeof(NOTIFYICONDATA));
	nid.cbSize=sizeof(NOTIFYICONDATA);
	nid.hWnd=hWnd;
	nid.uID=IDIcon;
	nid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	nid.uCallbackMessage=wm_systemtray;
	nid.hIcon=hIcon;
	_tcscpy(nid.szTip,strTip);
	Shell_NotifyIcon(NIM_ADD,&nid);
	bCanUse=true;
}
void CSystemTray::Delete()
{
	if(bCanUse) Shell_NotifyIcon(NIM_DELETE,&nid);
}
void CSystemTray::Modify(HWND hWnd,HICON hIcon,UINT IDIcon,DWORD wm_systemtray,LPCTSTR strTip)
{
	if(!bCanUse) Add(hWnd,hIcon,IDIcon,wm_systemtray,strTip);
	else
	{
	//	if(hWnd) nid.hWnd=hWnd;
	//	if(IDIcon) nid.uID=IDIcon;
	//	if(WM_SYSTEMTRAY) nid.uCallbackMessage=WM_SYSTEMTRAY;
		if(hIcon) nid.hIcon=hIcon;
		if(strTip) _tcscpy(nid.szTip,strTip);
		Shell_NotifyIcon(NIM_MODIFY,&nid);
	}
}
void CSystemTray::ReAdd()
{
	if(!bCanUse) return;
	Shell_NotifyIcon(NIM_ADD,&nid);
}