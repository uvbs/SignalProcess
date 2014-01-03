// CProcessTools.h: interface for the CProcessTools class.


#if !defined(AFX_CProcessTools_H__3D91DB41_C873_4321_8343_4829079D466A__INCLUDED_)
#define AFX_CProcessTools_H__3D91DB41_C873_4321_8343_4829079D466A__INCLUDED_

#include <afxmt.h>


class CProcessTools //数据池 
{
public:
	static long KillProcess(LPCTSTR processName,long lWhich=-1);
	//强行关闭任务管理器中映像名称为processName的进程
	//lWhich:关闭第几个映像名称为processName的进程，-1表示全部，0表示第一个
	//返回成功关闭的个数
	static long ProcessKill(LPCTSTR processName,DWORD dwExceptPID);
	
	//关闭映像名在processNames中且ID不在dwExceptPIDs中的所有进程
	static long KillProcess(LPCTSTR processNames,LPCTSTR strExceptPIDs,LPCTSTR sep);
	static long KillProcess(LPCTSTR processNames,LPCTSTR sep,DWORD dwExceptPID);
};

#endif // !defined(AFX_CProcessTools_H__3D91DB41_C873_4321_8343_4829079D466A__INCLUDED_)
