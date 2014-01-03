#pragma once

#include <vector>
#include <deque>
using std::vector;
using std::deque;

struct CaliItem {
	CString szNO;				//编号
	CString szReserveTime;		//保留时间
	CString szComponentName;	//组份名
	CString szLevel;			//级别
	CString szContents;			//含量
	CString szPeakArea;			//峰面积
	CString szPeakHeight;		//峰高
	CString szResFactor;		//响应因子
	CString szIsReference;		//是否参比(参考峰
	CString szIsInterior;		//是否内标(内标峰
	CString szStdContent;		//内标含量！内标绝对含量
	CString szInteriorNo;		
	//内标编号
	CString szSampleFullName;	//样品文件全路径名
	CString szPeakBeginX;		//起始点
	CString szPeakEndX;			//终止点
	CString szSlope;			//斜率
	CString szIntercept;		//截距
	CString szCoefficient;		//相关系数
	CString szPeakType;         //峰类型
	CString szPeakAreaPercent;  //峰面积%
	CString szPeakHighPercent;  //峰高%
	CString szConcentration;    //含量[ug/mL] (定量计算结果
	
	CaliItem() { }
	CaliItem(const CStringArray& data);
	CaliItem(const CStringArray& data, const CString tableName);
	bool operator== (const CaliItem& rhs);
};



struct CaliCurveData {
	double _x; //x轴
	double _y; //y轴
	CaliCurveData() : _x(0.0), _y(0.0) { }
	CaliCurveData(double dx, double dy) : _x(dx), _y(dy) { } 
};





class CCaliData {
private:
	vector<CaliItem> m_CaliItems; 
	CString m_tableType; //定制表格类型，峰面积||峰高
	CString m_tableName; //转化的表格名

public:
	CCaliData(void);
	~CCaliData(void);
public: //开放接口

	//外部表格数据填充内部数据
	void InitCaliData(CIADataTable* pCIADataTable); 

	//返回完整表格
	bool ItemSaveToTable(CIADataTable* pCIADataTable);
	bool ItemSaveToCalibrateResultTable(CIADataTable* pCIADataTable);

	//生成定制表格数据
	bool GenerateTable(CIADataTable &cIADataTable, const CString& caliMethod);
	bool GenerateCalibrateResultTable(CIADataTable &cIADataTable); //定制校正结果表
	
	//当前有多少行数据
	int GetItemSize() { return static_cast<int>(m_CaliItems.size()); }

	//新进样品的保留时间判定
	bool VerifyReserveTime(const CString& szReserveTime, int& match_no, int& instA, int& instB);

	//插入数据到i行之后 iStart是同名组份开始的行号
	bool InsertRowData(int iRow, int iStart, const CStringArray& strArry, LPCTSTR lpcszPathName);
	bool InsertRowData(int iRow, const CStringArray& strArry, LPCTSTR lpcszPathName);
	//更新行数据
	bool UpdateRowData(int iRow, const CStringArray& strArry);

	//调整插入的组份级别
	bool ModifyInsertItemLevel();

	//获得样品文件的全路径（不重复）
	bool GetSampleFullPath(CStringArray& strArry);
	bool GetSampleFullPath(int row, CString& strVal);

	//删除i行数据
	bool DeleteRowData(int iRow);
	bool DeleteRowData(CString& szPath); //删除凡是带有szPath的样品文件
	bool DeleteNotFirstRow();

	//验证内标含量是否应该存在
	void VerifyContent();

	//修改i行数据中的内标含量
	bool ModifyContent(int nRow, CString& szContent); 
	
	//删除i行数据中的内标含量
	bool ClearContent(int nRow);

	//获取i行的内标含量 
	bool GetPreContent(int nRow, CString& szContent); 

	//表格内数据转移
	void CaliTable2CaliResultTable(const CCaliData& CaliTable);

	//清空数据
	void ClearAllData() { m_CaliItems.clear(); }

//*****内标编号相关函数
	//自动增加内标编号
	bool AddInteriorNo(int nRow);
	//检测数据中是否已存在内标编号
	bool IsExistInteriorNo();
	//每个组份的内标编号为1
	bool AllInteriorNoBe1();
	//找出合理的内标编号
	int AutoFindInteriorNo();
	//找出当前最小内标编号
	int FindMinInteriorNo();
	//测试当前行是否是唯一内标编号为1的
	bool IsOnlyInteriorNo1(int row);
	//当前的内标编号是否唯一
	bool IsInteriorNoOnlyOne();
	//检查修改的编号是否已经存在
	bool CheckExistInteriorNo(int row, const CString& strVal);
	//改变同内标编号的所有组份
	bool ChangeSameCompnentInteriorNo(int row, const CString& dest);
	//测试strNo是否在当前内标编号范围内
	bool VertifyInteriorNoInterval(const CString& strNo);
	//同步内标编号数据，手动添加
	bool ManualInteriorNo(int col, int row, LPCTSTR szVal);
	//自动检查在数据被删除后内标的正确性
	void AutoCheckInteriorNoAfterDel();
	//自动重新对编号进行重排序
	void AutoReorderID();
	//指定为参比
	bool DesignatedReference(int row, bool bIsReference);
	//指定为内标
	bool DesignatedInterior(int row, bool bIsInterior); 
	//同步删除，该内标组分和此前基于该内标的非内标组分的编号变为空
	bool SyncDelInterior(int row);
	//清空所有内标编号
	void ClearAllInteriorNo();
	//同步编辑的单元格和内部数据的值
	bool SynEditedCell(int col, int row, LPCTSTR szVal);
	 
	// 测试当前行是否是一组组份的首行
	bool VertifyFirstLine(int row);

	// 测试row行数据是否是内标
	bool VertifyIsReference(int row);
	bool VertifyIsInterior(int row);
	
	//清除非一组组份首行的其他单元格内数据
	void ClearUnnecessaryCell();
	void ClearUnnecessaryCell(int row);

	// 统一小数位格式
	void UnifyDecimalDigits();

	//获取row行对应的组分名
	void GetComponentName(int row, CString& strName);

	//获取row行对应的保留时间
	void GetReserveTime(int row, CString& strVal);
	void GetOpEdXTime(int row, double &dOp, double& dEd); //优化：Op，Ed应该作为一个类参数传入

	//[外标法/外标百分比法/外标归一化法]+[峰面积/峰高]的数据
	bool GetCaliCurveData_TypeA(int row, deque<CaliCurveData>& data);

	//[内标法/内标百分比]+[峰面积/峰高]的数据
	bool GetCaliCurveData_TypeB(int row, deque<CaliCurveData>& data);
	
	//设置斜率，截距，相关系数
	bool SetSlope_Intercept_Coefficient(int row, double s, double i, double c);

	//校参表部分
	bool _GenerateCabParamPageTable(CIADataTable &cIADataTable);

	//定量计算部分
	void AddData(const CaliItem& item);
	void GetRefPeakData(CCaliData& data);
	void GetIntPeakData(CCaliData& data);
	void GetcOtherPeakData(CCaliData& data);
	void GenerateDuplicate(CCaliData& data) const; //获得一份跟当前变量相同的副本
	void ModifyResTime(vector<double> &afterResTime); 
	void GetRefPeakInterval(int n, int& before, int& after);
	int  Row2No(int row); //数据行数转换为第几个参考峰
	void CancelRefPeakMark(int no); //取消第no个参考峰标记
	void CancelIntPeakMark(int no); //取消第no个内标峰标记
	CaliItem& GetRowDate(int row); //获得当前行数据

	bool GetRowData(int nRow, CStringArray& strArry); //获得第i行数据
	bool GetInteriorNoDate(CStringArray& strArry);    //获得【内标编号】列数据

private: //私有接口
	bool GetComponentIndex(int row, int& b, int& e);

	bool GeneratePHTable(CIADataTable &cIADataTable);
	bool GeneratePATable(CIADataTable &cIADataTable);
	
	
	bool GetCaliResultRowData(int nRow, CStringArray& strArry); //获得第i行数据
	

	// 获得下一个匹配的级别
	bool GetSampleLevel(int& iValue); 
	void SubOne(vector<int> &vec, int i);

	// 删除的是否是组份的首行,是的话复制.保证在删除行时不丢失样品信息
	bool CheakCopyInfo(int iRow);

	// 保留小数位数
	void ConvertDecimalDigits(CString& obj, int dts);

	// 把科学计数格式指数位数e000变成e00
	void EraseZeroPlus(CString& str); 
	void EraseZeroMinus(CString& str); 

	//测试字符串是否是数字
	bool IsDigital(CString& str); 
	//测试字符串是否是自然数
	bool IsNaturalNumber(CString& str);
};



class CCaliMethod
{
public:
	static const CStringArray& StringIDs();
	static const CStringArray& StringNames();
	static int IndexByStringID(const CString& id);
	static const CString& StringIDByIndex(int index);
	static const CString* NameByID(const CString& id);
};
class CCaliMethodOn
{
public:
	static const CStringArray& StringIDs();
	static const CStringArray& StringNames();
	static int IndexByStringID(const CString& id);
	static const CString& StringIDByIndex(int index);
	static const CString* NameByID(const CString& id);
};

class CCaliUI
{
public:
	static void InitComboBox(CComboBox* pCB, const CStringArray* pSA);
};