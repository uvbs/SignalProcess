
#ifndef TWEN_SDK_CFileSystemEx_
#define TWEN_SDK_CFileSystemEx_

class CFileSystemEx
{
public:
	static bool GetOpenFile(CString&fpn, LPCTSTR fileType, LPCTSTR text, CWnd*parent);
	static bool GetSaveFile(CString&fpn, LPCTSTR fileType, LPCTSTR text, LPCTSTR defaultExt, LPCTSTR defaultFN, CWnd*parent);

};

#endif