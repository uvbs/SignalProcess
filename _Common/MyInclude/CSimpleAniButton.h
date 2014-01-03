
#ifndef TWEN_SDK_CSimpleAniButton_
#define TWEN_SDK_CSimpleAniButton_

#include "SimpleDraw.h"

//
class CSimpleAnimation;

class CSimpleAnimationBase
{
protected:
	CSimpleAnimationBase(CSimpleAnimation* pAnimation):m_pAnimation(pAnimation),Index(NULL){}

public:
	virtual DWORD GetIndexCount() const = 0;

	void OnFrameNext()
	{
		Index++;
		if(Index>=GetIndexCount()) Index = 0;
	}

protected:
	DWORD Index;
	CSimpleAnimation* m_pAnimation;
};
//
class CSimpleAnimation_Bitmap : CSimpleAnimationBase
{
private:
	CSimpleAnimation_Bitmap(CSimpleAnimation* pAnimation):CSimpleAnimationBase(pAnimation),m_pBitmap(NULL){}
	friend class CSimpleAnimation;

public:
	void SetPieceInfo(DWORD w, DWORD h){PieceWidth=w; PieceHeight=h;}
	void AddIndex(long row, long col);
	void AddRows(long col, long rowFrom, long rowTo);
	void AddColumns(long row, long colFrom, long colTo);
	void InsertIndex(long row, long col, long index);
	void InsertRows(long col, long rowFrom, long rowTo, long index);
	void InsertColumns(long row, long colFrom, long colTo, long index);

	bool GetCurrentRect(CRect& rc) const
	{
		if(Index>=GetIndexCount()) return false;
		const POINT pt = m_indexes.GetAt(Index);
		rc = CRect(pt.x*PieceWidth, pt.y*PieceHeight, (pt.x+1)*PieceWidth, (pt.y+1)*PieceHeight);
		return true;
	}

	DWORD GetIndexCount() const{return m_indexes.GetCount();}

public:
	CBitmap* m_pBitmap;

private:
	CArray<POINT, POINT&> m_indexes;
	DWORD PieceWidth,PieceHeight;
};
//
class CSimpleAnimation_ImageList : CSimpleAnimationBase
{
private:
	CSimpleAnimation_ImageList(CSimpleAnimation* pAnimation):CSimpleAnimationBase(pAnimation),m_pImageList(NULL){}
	friend class CSimpleAnimation;

public:
	void AddIndex(long imageIndex);
	void AddIndexes(long imageIndexFrom, long imageIndexTo);
	void InsertIndex(long imageIndex, long index);
	void InsertIndexes(long imageIndexFrom, long imageIndexTo, long index);

	long GetCurrentImage() const
	{
		if(Index>=GetIndexCount()) return -1;
		return m_indexes.GetAt(Index);
	}

	DWORD GetIndexCount() const{return m_indexes.GetCount();}

public:
	CImageList* m_pImageList;

private:
	CArray<long, long> m_indexes;
};
//
class CSimpleAnimation_Bitmaps : CSimpleAnimationBase
{
private:
	CSimpleAnimation_Bitmaps(CSimpleAnimation* pAnimation):CSimpleAnimationBase(pAnimation){}
	friend class CSimpleAnimation;

public:
	void SetBitmaps(const CArray<CBitmap*, CBitmap*>* pBitmaps);
	CArray<CBitmap*, CBitmap*>* GetBitmaps(){return &m_bitmaps;}

	void AppendBitmap(CBitmap* pBitmap);
	void InsertBitmap(CBitmap* pBitmap, long lIndex);

	CBitmap* GetCurrentBitmap() const{if(Index>=GetIndexCount()) return NULL;return m_bitmaps.GetAt(Index);}

	DWORD GetIndexCount() const{return m_bitmaps.GetCount();}

private:
	CArray<CBitmap*, CBitmap*> m_bitmaps;
};
//
class CSimpleAnimation_Files : CSimpleAnimationBase
{
private:
	CSimpleAnimation_Files(CSimpleAnimation* pAnimation):CSimpleAnimationBase(pAnimation),hBitmap(NULL){}
	friend class CSimpleAnimation;

public:
	~CSimpleAnimation_Files(){Reset();}

public:
	void SetFiles(const CArray<CString, CString&>* pFiles);
	CArray<CString, CString&>* GetFiles(){return &m_files;}

	void AppendFile(LPCTSTR file);
	void InsertFile(LPCTSTR file, long lIndex);
	void AppendFiles(LPCTSTR format, long from, long to);
	void InsertFiles(LPCTSTR format, long from, long to, long lIndex);
	void AppendFiles(LPCTSTR format, TCHAR from, TCHAR to);
	void InsertFiles(LPCTSTR format, TCHAR from, TCHAR to, long lIndex);

	CBitmap* GetCurrentBitmap()
	{
		Reset();
		if(Index>=GetIndexCount()) return NULL;
		hBitmap = (HBITMAP)::LoadImage(NULL,m_files.GetAt(Index),IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
		if(!hBitmap) return NULL;
		Bitmap.Attach(hBitmap);
		return &Bitmap;
	}
	HBITMAP GetCurrentHandle() const
	{
		if(Index>=GetIndexCount()) return NULL;
		return (HBITMAP)::LoadImage(NULL,m_files.GetAt(Index),IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
	}

	DWORD GetIndexCount() const{return m_files.GetCount();}

private:
	void Reset()
	{
		if(hBitmap)
		{
			Bitmap.Detach();
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}
	}


private:
	CArray<CString, CString&> m_files;
	HBITMAP hBitmap;
	CBitmap Bitmap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAnimation
///////////////////////////////////////////////////////////////////////////////////////////////
class CSimpleAnimation
{
public:
	enum enumAnimationType
	{
		TYPE_NONE,
		TYPE_BITMAP,
		TYPE_IMAGELIST,
		TYPE_BITMAPS,
		TYPE_FILES,
	};

public:
	CSimpleAnimation():Milliseconds(50),Transparent(false),TransparentColor(NULL),Type(TYPE_NONE),Bitmap(this),ImageList(this),Bitmaps(this),Files(this){}

public:
	void OnFrameNext()
	{
		switch(Type)
		{
		case TYPE_NONE:break;
		case TYPE_BITMAP:Bitmap.OnFrameNext();break;
		case TYPE_IMAGELIST:ImageList.OnFrameNext();break;
		case TYPE_BITMAPS:Bitmaps.OnFrameNext();break;
		case TYPE_FILES:Files.OnFrameNext();break;
		}
	}
	void ForceIndex(DWORD index)
	{
		Bitmap.Index = index;
		ImageList.Index = index;
		Bitmaps.Index = index;
		Files.Index = index;
	}

public:
	DWORD Milliseconds;//每隔多少毫秒绘画一次
	bool Transparent;
	COLORREF TransparentColor;
	enumAnimationType Type;

	CSimpleAnimation_Bitmap Bitmap;
	CSimpleAnimation_ImageList ImageList;
	CSimpleAnimation_Bitmaps Bitmaps;
	CSimpleAnimation_Files Files;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// CSimpleAniButton
///////////////////////////////////////////////////////////////////////////////////////////////
class CSimpleAniButton : public CButton, CSimpleDrawDC
{
	enum enumDefine
	{
		TIMER_ID_ANIMATION = 1,
	};

	DECLARE_DYNAMIC(CSimpleAniButton)

public:
	struct structSwitchItem
	{
		CSimpleAnimation Animation;
		CSimpleText Text;
	};

public:
	CSimpleAniButton();
	virtual ~CSimpleAniButton(){ResetSwitch();}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleAniButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSimpleAniButton)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public://多开关控制，其他开关从1开始
	void SetSwitches(DWORD dwCount);//不包含开关0的开关个数
	void SetCurrentSwitch(DWORD dwSwitch){m_dwCurrentSwitch = dwSwitch;}
	DWORD GetCurrentSwitch() const { return m_dwCurrentSwitch;}

	void SetSwitchText(LPCTSTR text, DWORD dwSwitch = -1);
	void SetSwitchTextDrawType(DWORD DrawType, DWORD dwSwitch = -1);
	void SetSwitchAnimateType(CSimpleAnimation::enumAnimationType type, DWORD dwSwitch = -1);

	structSwitchItem* GetSwitch(DWORD dwSwitch);

	void SetUsingSwitchText(bool bUsingSwitchText){m_bUsingSwitchText = bUsingSwitchText;}

public:
	void StopAnimate(){KillTimer(TIMER_ID_ANIMATION);}
	void StopAnimateWith(DWORD index);
	void EnsureAnimate();

public:
	CSimpleGround EraseGround;
	CSimpleGround BackGround;
	structSwitchItem DefaultForeGround;
	CSimpleDrawType DrawType_EG,DrawType_BG,DrawType_FG;

protected:
	DWORD m_dwSwitches;//不包括开关0
	DWORD m_dwCurrentSwitch;
	structSwitchItem* m_pSwitches;
	bool m_bUsingSwitchText;

protected:
	void ResetSwitch();

public:
	void SetToolTip(LPCTSTR tooltip=NULL,BOOL bActivate=TRUE);
	void ActivateTooltip(BOOL bActivate=TRUE);
protected:
	void InitToolTip();
	CToolTipCtrl m_ToolTip;
};

#endif