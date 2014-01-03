/*
归档：开发包
描述：String的split处理
测试：

用法：
CStringExt ext("1;12;123;;1234;",';');
///////11111111111111111111
cout<<ext.GetCount()<<"\n";
ext.BeginEnum();
CString value;
while(ext.GetNext(value)) cout<<(LPCTSTR)value<<"\n";
///////22222222222222222222
DWORD len=ext.GetCount();
for(DWORD i=0;i<len;i++) cout<<(LPCTSTR)ext[i]<<"\n";
cout<<(LPCTSTR)ext[len]<<"\n";
说明：
需要使用遍历时，尽量使用ext.BeginEnum();while(ext.GetNext(value));
GetEnum和[]单个使用
最后修改日期：
最后修改者：tiewen
//*/
#ifndef _TWEN_SDK_CStringExt_
#define _TWEN_SDK_CStringExt_

typedef void (*EnumCallBack)(const CString&value);

//可增加Attach源字符串方式
class CStringExt
{
public:
	static bool File_Exist(LPCTSTR filePathName);
	static bool GetAppPath(CString &path);//不包括最后的'\\'

public:
	enum enumEnumAllSubs_FileList
	{
		EASFL_SubFilesOnly=0x00000001,
		EASFL_ReturnPath=0x00000002,
		EASFL_NoExt=0x00000004,//不返回文件后缀，包括.
		EASFL_SemicolonSep=0x00000008,//分号,若EASFL_CommaSep同时设置，则本项优先
		EASFL_CommaSep=0x00000010,//逗号
	};
public:
	static void ToTime(LPCTSTR txt, CTime& time);//将字符串转化为时间，字符串格式为2009-12-31 23:59:59
	static bool IsLong(LPCTSTR txt);//-2147483648-2147483647,static
	static bool IsDWORD(LPCTSTR txt);//0-4294967295,static
	static bool IsLongLong(LPCTSTR txt);//–9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
	static bool IsDouble(LPCTSTR txt);//没有比较数值范围

	static bool Split(LPCTSTR srcString,LPCTSTR splitString,BYTE nNum,...);//...:CString*,static
	static bool Split(LPCTSTR srcString,LPCTSTR splitString,BYTE nNum,CString*values);//CString values[nNum],static

	static int LastIndexOf(CString in,TCHAR c);
	static CString GetFileExt(CString fn,LPCTSTR notFound);
	//回调函数中，fileName不包括文件的后缀名
	static void EnumAllSubs(LPCTSTR folder,LPCTSTR types,LPCTSTR typeSep,
		void (*cb)(LPCTSTR path,LPCTSTR fileName,LPCTSTR fileType,DWORD extraData),bool bSubFilesOnly,DWORD extraData);
	static void EnumAllSubs_Ex(LPCTSTR folder,LPCTSTR types,LPCTSTR typeSep,
		DWORD (*cbFolder)(LPCTSTR path,LPCTSTR folderName,DWORD extraData),
		void (*cbFolderEnd)(LPCTSTR path,LPCTSTR folderName,DWORD extraData),
		void (*cbFile)(LPCTSTR path,LPCTSTR fileName,DWORD extraData),bool bSubFilesOnly,DWORD extraData);
	//枚举folder目录下的types类型文件，将文件列表（以;号分割）返回
	//bSubFilesOnly表示是否不要递归子目录
	//bPath表示返回的文件列表是否要包含路径
	static CString EnumAllSubs_FileList(LPCTSTR folder,LPCTSTR types,LPCTSTR typeSep,DWORD flag);

private:
	static void EnumAllSubs_CB_FileList(LPCTSTR path,LPCTSTR fileName,LPCTSTR fileType,DWORD extraData);
	static CString m_stc_strEnumAllSubs_CB_FileList;

public:
	CStringExt();
	CStringExt(LPCTSTR srcString,TCHAR splitChar);
	CStringExt(LPCTSTR srcString,LPCTSTR splitString);
	~CStringExt();

	void Clone(const CStringExt& ext);
	CStringExt& operator =( const CStringExt& ext ){Clone(ext);return *this;}

	void SetSRCString(LPCTSTR srcString);
	void SetSplitString(LPCTSTR splitString);
	void SetSplitString(TCHAR splitChar);
	LPCTSTR GetSplitString() const{return m_splitString;}
	DWORD GetCount();

	//for while support
	void SetNullNext(bool bNullNext);//循环遍历时,若当前值为空串,是否自动获取下一个
	void BeginEnum();//尽量使用循环遍历
	bool GetNext(CString&value);

	//效率有待改进
	void BeginMove();//指向首条记录
	void MoveNext();
	void MovePrev();
	void MoveFirst();
	void MoveLast();
	void Move(long lCount);
	bool GetCurrent(CString&value);
	bool IsBOF() const;
	bool IsEOF() const;

	//for single support
	bool GetEnum(DWORD dwIndex,CString&value);
	CString operator[](DWORD dwIndex);

private:
	void Count();
	void Positions();

private:
	CString m_srcString;
	CString m_splitString;
	DWORD m_curPos;
	DWORD m_count;
	DWORD m_curCount;
	bool m_bNullNext;//当枚举时，若当前值为空，则枚举下一个，默认为false
	DWORD *m_pPositions;//保留（首先解析字符串，把各分隔符位置保存到本数组中）
	long m_lMoveIndex;
};

/////////////////////////////////////////////////////////////////////////////////
//互斥量,唯一访问,可跨进程使用
/*
进程A:CSingleAccess sa("mutex");
sa.Access();
...
sa.LeaveAccess();
或者:
CWaitingSignal sa;
sa.Setup("mutex");
sa.Access();
...
sa.LeaveAccess();
进程B:CSingleAccess sa;
sa.Open("mutex");
sa.Access();
...
sa.LeaveAccess();
*/
class CSingleAccess  
{
public:
	CSingleAccess(LPCTSTR signalName=NULL);
	virtual ~CSingleAccess();

	bool Setup(LPCTSTR signalName);
	bool Open(LPCTSTR signalName);
	void UnInit();
	void Access(bool bUI=false);
	void LeaveAccess();
protected:
	HANDLE m_hSignal;
#ifdef _DEBUG
	CString m_signalName;
#endif
};
/////////////////////////////////////////////////////////////////////////////////
/*
作用:进程A中创建信号量,等待进程B设置该信号量后,进程A再继续执行
进程A:CWaitingSignal ws("mutex",true);
	ws.Wait();
	或者:
	CWaitingSignal ws;
	ws.Open("mutex");
	ws.Reset();
	ws.Wait();
进程B:CWaitingSignal ws("mutex",false);
	ws.EndWait();
	或者:
	CWaitingSignal ws;
	ws.Open("mutex");
	ws.EndWait();
*/
class CWaitingSignal
{
public:
	static bool Wait(HANDLE handle,bool bUI=false,DWORD dwMilliseconds=INFINITE);
	//静态函数,等待handle结束/被设置(如等待线程结束,等待信号量被设置)
public:
	CString m_signalName;
	bool m_bInWaitProccess;
public:
	CWaitingSignal(LPCTSTR signalName=NULL,bool bInWaitProccess=true);
	virtual ~CWaitingSignal();

	bool Setup(LPCTSTR signalName,bool bInWaitProccess);//创建信号量
	void UnInit();//关闭/删除信号量
	bool Open(LPCTSTR signalName);//打开现有的信号量
	void Reset();//重置创建或打开的信号量
	bool Wait(bool bUI=false,DWORD dwMilliseconds=INFINITE);
	//等待其他线程/进程设置信号量
	//返回是否成功等待到信号量被正确设置(失败的原因有很多,但成功只有一个)
	void EndWait();//设置信号量
protected:
	HANDLE m_hSignal;
};

#endif