#pragma once
#include "afx.h"

class CRow  
{  
public:  
	CRow(CStringArray* row);  
	int getColumnCount(void);  
	CString getColumn(int i);  
private:  
	CStringArray* m_row;  
};  


class CSheet  
{  
public:  
	CSheet(void);  
	~CSheet(void);  
	int loadFrom(std::ifstream& in);  
	int getRowCount(void);  
	CRow getRow(int i);

private:  
	CTypedPtrArray<CPtrArray, CStringArray*> m_rows;  
};  


class CCSVFile : public CStdioFile
{

public:

	enum Mode { modeRead, modeWrite };
	CCSVFile(LPCTSTR lpszFilename, Mode mode = modeRead);
	~CCSVFile(void);

	bool ReadData(CStringArray &arr);
	void WriteData(CStringArray &arr);

#ifdef _DEBUG

	Mode m_nMode;

#endif


};