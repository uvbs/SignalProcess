#include "StdAfx.h"
#include "CSVFile.h"
#include <cstring>
#include <fstream>


CCSVFile::CCSVFile(LPCTSTR lpszFilename, Mode mode)
: CStdioFile(lpszFilename, (mode == modeRead) ?
			 CFile::modeRead|CFile::shareDenyWrite|CFile::typeText   
			 :
CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText)
{

#ifdef _DEBUG
	m_nMode = mode;
#endif
}
 

CRow::CRow(CStringArray* row)  
{  
	m_row = row;  
}  
int CRow::getColumnCount(void)  
{  
	return m_row->GetCount();  
}  
CString CRow::getColumn(int i)  
{  
	return m_row->GetAt(i);  
}

CSheet::CSheet(void)  
{  
}  
CSheet::~CSheet(void)  
{  
	for (int i = 0; i<m_rows.GetCount(); i++)  
	{  
		delete m_rows.GetAt(i);  
	}  
}  
int CSheet::loadFrom(std::ifstream& in)  
{  
	int lines = 0;  
	while(!in.eof())  
	{  
		//读取其中一行  
		char line[256] = {0};  
		in.getline(line, 255);  
		CString s = /*(CString)*/static_cast<CString>(line);  

		//空白行则跳过  
		if (s.IsEmpty())  
			continue;  
		//#为注释标记，跳过  
		if (s[0]=='#')  
			continue;  
		//如果读到字母则跳过
		if (s[0]>='a' && s[0]<='z')
			continue; 
		CStringArray* pRow = new CStringArray();  
		int i = 0;  
		CString token = s.Tokenize(_T(",\t"), i);  
		while (token!=_T(""))  
		{  
			pRow->Add(token);  
			token = s.Tokenize(_T(",\t"), i);  
		}  
		m_rows.Add(pRow);  
		lines++;  
	}  
	return lines;  
}  
int CSheet::getRowCount(void)  
{  
	return m_rows.GetCount();  
}  
CRow CSheet::getRow(int i)  
{  
	return CRow(m_rows.GetAt(i));  
}  







CCSVFile::~CCSVFile(void)
{
}

bool CCSVFile::ReadData(CStringArray &arr)
{
	// Verify correct mode in debug build
	ASSERT(m_nMode == modeRead);

	// Read next line
	CString sLine;
	int nValue = 0;
    int nColumn = 0; //统计数据列
	bool bCheakCol = true;
	while (ReadString(sLine))
	{
		if( !(sLine[0]>='0' && sLine.GetAt(0)<='9') ) //不读每行的首个字符不为数字的数据
			continue;
		LPCTSTR p = sLine;
		while (*p != '\0')
		{
			CString s;  // String to hold this value

			// Parse unquoted value
			while (*p != '\0' && *p != ',')
			{
				s.AppendChar(*p++);
			}
			
			// Advance to next character (if not already end of string)
				if (*p != '\0')
					p++;
				if (bCheakCol)
				{
					nColumn++; // 计算数据列数
				}

			// Add this string to value array
				if (nValue < arr.GetCount())
					arr[nValue] = s;
				else
					arr.Add(s);
				nValue++;
				
		}
		bCheakCol =false;
			
	}
	// Trim off any unused array values
	if (arr.GetCount() > nValue)
		arr.RemoveAt(nValue, arr.GetCount() - nValue);
	// We return true if ReadString() succeeded--even if no values
	return true;
	

	// Parse values in this line  需要给数据分列在此
	
}

void CCSVFile::WriteData(CStringArray &arr)
{
	static TCHAR chQuote = '"';
	static TCHAR chComma = ',';

	// Verify correct mode in debug build
	ASSERT(m_nMode == modeWrite);

	// Loop through each string in array
	for (int i = 0; i < arr.GetCount(); i++)
	{
		// Separate this value from previous
		if (i > 0)
			WriteString(_T(","));
		// We need special handling if string contains
		// comma or double quote
		bool bComma = (arr[i].Find(chComma) != -1);
		bool bQuote = (arr[i].Find(chQuote) != -1);
		if (bComma || bQuote)
		{
			Write(&chQuote, sizeof(TCHAR));
			if (bQuote)
			{
				for (int j = 0; j < arr[i].GetLength(); i++)
				{
					// Pairs of quotes interpreted as single quote
					if (arr[i][j] == chQuote)
						Write(&chQuote, sizeof(TCHAR));
					TCHAR ch = arr[i][j];
					Write(&ch, sizeof(TCHAR));
				}
			}
			else
			{
				WriteString(arr[i]);
			}
			Write(&chQuote, sizeof(TCHAR));
		}
		else
		{
			WriteString(arr[i]);
		}
	}
	WriteString(_T("\n"));
}