#pragma once

//------------------------------------------------------------------------|
//------------------------------------------------------------------------|
//----------------------  分析系统XML(文件) 定义表 ------------------------|
//------------------------------------------------------------------------|
//------------------------------------------------------------------------|

//
//-------------------------------------------------------------------------
//	数据根名称定义
//-------------------------------------------------------------------------
#define		XMLROOT_INSTRUMENT		_T("Instrument")
#define		XMLROOT_TASK			_T("Task")
//#define	XMLROOT_METHOD			_T("Method")
//#define	XMLROOT_SAMPLE			_T("Sample")
//#define	XMLROOT_ACQUIRER		_T("Acquirer")
#define		XMLROOT_SYSSETTINGS		_T("SysSettings")



//-------------------------------------------------------------------------
//---------------------------进样器文件结构（路径）-------------------------
//-------------------------------------------------------------------------
#define	XMLROOT_INJECT				_T("Inject")
#define XMLPATH_INJECT_INLETVALVE	_T("Inject/InletValve")	//（多路）进样阀配置


//-------------------------------------------------------------------------
//---------------------------方法文件结构（路径）---------------------------
//-------------------------------------------------------------------------
//Method(Root)
#define	XMLROOT_METHOD							_T("Method")

//Method(Root)-->基本属性
#define XMLPATH_METHOD_METHODID					_T("Method/MethodID")			//方法ID
#define XMLPATH_METHOD_METHODNAME				_T("Method/MethodName")			//方法名称
#define XMLPATH_METHOD_SYSTEM					_T("Method/System")				//分析系统
#define XMLPATH_METHOD_APP						_T("Method/Application")		//分析程序

//Method(Root)-->样品缺省属性
#define XMLPATH_METHOD_SAMPLEID					_T("Method/SampleID")			//样品缺省
#define XMLPATH_METHOD_SAMPLENAME				_T("Method/SampleName")			//样品缺省
#define XMLPATH_METHOD_SAMPLEPURPOSE			_T("Method/SamplePurpose")		//样品类型（试样，标样）
#define XMLPATH_METHOD_SAMPLEINJECTNO			_T("Method/SampleInjectNO")		//样品进样号

//Method(Root)-->参数
#define XMLPATH_METHOD_PARAM					_T("Method/Parameter")   
//Method系统推荐参数
#define XMLPATH_METHOD_PARAM_SYSTEM_THRESHOLD			_T("Method/Parameter/PeakProcess/Threshold")  
#define XMLPATH_METHOD_PARAM_SYSTEM_MINHALFPEAKWIDTH	_T("Method/Parameter/PeakProcess/MinHalfPeakWidth")  
#define XMLPATH_METHOD_PARAM_SYSTEM_MINPEAKAREA			_T("Method/Parameter/PeakProcess/MinPeakArea")  
#define XMLPATH_METHOD_PARAM_SYSTEM_HEIGHT				_T("Method/Parameter/PeakProcess/Height")         
//Method(Root)-->参数-->样品缺省信息
#define XMLPATH_METHOD_PARAM_SAMPLE				_T("Method/Parameter/Information")  

//Method(Root)-->参数-->仪器参数
#define XMLPATH_METHOD_PARAM_INSTRUMENT			_T("Method/Parameter/Instrument")

//Method(Root)-->参数-->仪器参数-->采样参数-->信号通道
#define XMLPATH_METHOD_PARAM_INSTRU_SC			_T("Method/Parameter/Instrument/Acquisition/SignalChannel")
//Method(Root)-->参数-->仪器参数-->采样参数-->进样通道
#define XMLPATH_METHOD_PARAM_INSTRU_IC			_T("Method/Parameter/Instrument/Acquisition/InjectionChannel")
//Method(Root)-->参数-->仪器参数-->采样参数-->采样时间
#define XMLPATH_METHOD_PARAM_INSTRU_AT			_T("Method/Parameter/Instrument/Acquisition/AcquisitionTime")

//Method(Root)-->参数-->校正参数
#define XMLPATH_METHOD_PARAM_CALIBRATE			_T("Method/Parameter/Calibrate")
//Method(Root)-->参数-->校正参数-->校正方法
#define XMLPATH_METHOD_PARAM_CALI_METHOD		_T("Method/Parameter/Calibrate/CalibrateMethod")
//Method(Root)-->参数-->校正参数-->校正对象
#define XMLPATH_METHOD_PARAM_CALI_OBJEC			_T("Method/Parameter/Calibrate/CalibrateObject")
//Method(Root)-->参数-->校正参数-->校正表
#define XMLPATH_METHOD_PARAM_CALI_TABLE			_T("Method/Parameter/Calibrate/CalibrateTable")
//Sample(Root)-->参数-->校正参数-->校正结果表
#define XMLPATH_METHOD_PARAM_CALI_RESULT_TABLE	_T("Method/Parameter/Calibrate/CalibrateResultTable")
//Method(Root)-->参数-->校正参数-->校正组份表
#define XMLPATH_METHOD_PARAM_CALI_COMPONENTTAB	_T("Method/Parameter/Calibrate/CalibrateTable_Component")
//Method(Root)-->参数-->峰处理参数
#define XMLPATH_METHOD_PARAM_PEAKPROCESS		_T("Method/Parameter/PeakProcess")
//Method(Root)-->参数-->峰处理参数-->噪声阀门
#define XMLPATH_METHOD_PARAM_PEAKPROC_TRD		_T("Method/Parameter/PeakProcess/Threshold")
//Method(Root)-->参数-->峰处理参数-->最小半峰宽
#define XMLPATH_METHOD_PARAM_PEAKPROC_MPW		_T("Method/Parameter/PeakProcess/MinHalfPeakWidth")
//Method(Root)-->参数-->峰处理参数-->最小峰面积
#define XMLPATH_METHOD_PARAM_PEAKPROC_MPA		_T("Method/Parameter/PeakProcess/MinPeakArea")
//Method(Root)-->参数-->峰处理参数-->峰处理参数表
#define XMLPATH_METHOD_PARAM_PEAKPROC_PPT		_T("Method/Parameter/PeakProcess/PPTimeTable")

//Methor(Root)-->参数-->校正设定参数
#define XMPPATH_METHOD_PARAM_CALI_SAMPLEAMOUNT   _T("Method/Parameter/Calibrate/SampleAmount")
#define XMPPATH_METHOD_PARAM_CALI_SAMPLEUNIT     _T("Method/Parameter/Calibrate/SampleUnit")
#define XMPPATH_METHOD_PARAM_CALI_PRODUCTFACTOR  _T("Method/Parameter/Calibrate/ProductFactor")
#define XMPPATH_METHOD_PARAM_CALI_DILUTIONFACTOR _T("Method/Parameter/Calibrate/DilutionFactor")
#define XMPPATH_METHOD_PARAM_CALI_CURVERFITTYPE  _T("Method/Parameter/Calibrate/CurverFitType")
#define XMPPATH_METHOD_PARAM_CALI_ORIGTREATMENT  _T("Method/Parameter/Calibrate/OrigTreatment")
#define XMPPATH_METHOD_PARAM_CALI_CALIWEIGHTING  _T("Method/Parameter/Calibrate/CaliWeighting")	
#define XMPPATH_METHOD_PARAM_CALI_REFERENCEPEAK  _T("Method/Parameter/Calibrate/ReferencePeak")	
#define XMPPATH_METHOD_PARAM_CALI_OTHERCALIPEAK  _T("Method/Parameter/Calibrate/OtherCaliPeak")	


//Method(Root)-->数据
#define XMLPATH_METHOD_DATA						_T("Method/Data")          
//Method(Root)-->数据-->曲线数据
#define XMLPATH_METHOD_DATA_CURVE				_T("Method/CurveDatas")          

//Method(Root)-->结果
#define XMLPATH_METHOD_RESULT					_T("Method/Result")          
//Method(Root)-->结果-->峰列表
#define XMLPATH_METHOD_RESULT_PEAKLIST			_T("Method/Result/PeakListTable")          
//Method(Root)-->结果-->结果表
#define XMLPATH_METHOD_RESULT_RESULT			_T("Method/Result/ResultTable")   
//Method(Root)-->结果-->打印路径
#define XMLPATH_METHOD_RESULT_PRINTPIC			_T("Method/Result/PrintPic")

//Method(Root)-->报表
#define XMLPATH_METHOD_REPORT					_T("Method/Report")          


//#define XMLPATH_METHOD_INMS_QTABLE		_T("Method/Calibrate/INMS_Table")				//工业质谱定量表
//#define XMLPATH_METHOD_INMS_ST_BK		_T("Method/Calibrate/INMS_StandardSample_BK")	//工业质谱背景校正
//#define XMLPATH_METHOD_INMS_ST_FG		_T("Method/Calibrate/INMS_StandardSample_FG")	//工业质谱碎片校正
//#define XMLPATH_METHOD_INMS_ST_SN		_T("Method/Calibrate/INMS_StandardSample_SN")	//工业质谱灵敏度校正
//#define XMLPATH_METHOD_CALIBRATE_COMPOUND	_T("Method/Calibrate/Compound")	
//#define XMLPATH_METHOD_INSTRUMENTPARAM  _T("Method/Instrument/InstrumentParam")
//#define XMLPATH_METHOD_CALIBRATE_SCALE  _T("Method/Calibrate/Scale")


//------------------------------------------------------------------------+
//-------------------------- 样品文件结构(路径) ---------------------------|
//------------------------------------------------------------------------+
//Sample(Root)
#define	XMLROOT_SAMPLE						_T("Sample")


//#define XMLPATH_SAMPLE_DRAWTYPE		_T("Sample/DrawType")

//Sample(Root)-->基本属性
#define XMLPATH_SAMPLE_METHODID				_T("Sample/MethodID")			//方法ID
#define XMLPATH_SAMPLE_METHODNAME			_T("Sample/MethodName")			//方法名称
#define XMLPATH_SAMPLE_SYSTEM				_T("Sample/System")				//分析系统
#define XMLPATH_SAMPLE_APP					_T("Sample/Application")		//分析程序

//Sample(Root)-->样品属性
#define XMLPATH_SAMPLE_SAMPLEID				_T("Sample/SampleID")			//
#define XMLPATH_SAMPLE_SAMPLENAME			_T("Sample/SampleName")			//

#define XMLPATH_SAMPLE_SAMPLEPURPOSE		_T("Sample/SamplePurpose")		//样品类型（试样，标样）
#define XMLPATH_SAMPLE_SAMPLEINJECTNO		_T("Sample/SampleInjectNO")		//样品进样号
#define XMLPATH_SAMPLE_PARAM_INFO_OPERATOR			_T("Sample/Parameter/Information/Operator")
#define XMLPATH_SAMPLE_PARAM_INFO_SENDER			_T("Sample/Parameter/Information/Sender")
#define XMLPATH_SAMPLE_PARAM_INFO_SENDTIME			_T("Sample/Parameter/Information/SendTime")
#define XMLPATH_SAMPLE_PARAM_INFO_ANALYSISTIME		_T("Sample/Parameter/Information/AnalysisTime")
#define XMLPATH_SAMPLE_PARAM_INFO_WEIGHT			_T("Sample/Parameter/Information/Weight")
#define XMLPATH_SAMPLE_PARAM_INFO_INTERNALWEIGHT    _T("Sample/Parameter/Information/InternalWeight")
#define XMLPATH_SAMPLE_PARAM_INFO_INJECTVOLUME      _T("Sample/Parameter/Information/InjectVolume")
#define XMLPATH_SAMPLE_PARAM_INFO_NOTE			    _T("Sample/Parameter/Information/note")
#define XMLPATH_SAMPLE_PARAM_INFO_TESTER			_T("Sample/Parameter/Information/Tester")
#define XMLPATH_SAMPLE_PARAM_INFO_SAMPLEAMOUNT      _T("Sample/Parameter/Information/SampleAmount")


//Sample(Root)-->参数
#define XMLPATH_SAMPLE_PARAM					_T("Sample/Parameter")          
//Sample(Root)-->参数-->样品缺省信息
#define XMLPATH_SAMPLE_PARAM_SAMPLE				_T("Sample/Parameter/Information")  

//Sample(Root)-->参数-->仪器参数
#define XMLPATH_SAMPLE_PARAM_INSTRUMENT			_T("Sample/Parameter/Instrument")
//Sample(Root)-->参数-->仪器参数-->采样参数-->信号通道
#define XMLPATH_SAMPLE_PARAM_INSTRU_SC			_T("Sample/Parameter/Instrument/Acquisition/SignalChannel")
//Sample(Root)-->参数-->仪器参数-->采样参数-->进样通道
#define XMLPATH_SAMPLE_PARAM_INSTRU_IC			_T("Sample/Parameter/Instrument/Acquisition/InjectionChannel")
//Sample(Root)-->参数-->仪器参数-->采样参数-->采样时间
#define XMLPATH_SAMPLE_PARAM_INSTRU_AT			_T("Sample/Parameter/Instrument/Acquisition/AcquisitionTime")

#define	XMLPATH_SAMPLE_PARAM_INSTRU_MF_PROGTAB	_T("Sample/Parameter/Instrument/MainFrame/Temp_ProgramTable")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TCB      _T("Sample/Parameter/Instrument/MainFrame/Temp_ColumnBox")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TRD      _T("Sample/Parameter/Instrument/MainFrame/Temp_RearDectetor")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TFI      _T("Sample/Parameter/Instrument/MainFrame/Temp_FrontInjector")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TPI      _T("Sample/Parameter/Instrument/MainFrame/Temp_ProgramInit")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TPIH     _T("Sample/Parameter/Instrument/MainFrame/Temp_ProgramInitHold")
#define XMLPATH_SAMPLE_PARAM_INSTRU_PUMP_FPTAB			_T("Sample/Parameter/Instrument/Pump/Flow_ProgramTable")
#define XMLPATH_SAMPLE_PARAM_INSTRU_PUMP_INITFLOW       _T("Sample/Parameter/Instrument/Pump/InitFLow")
#define XMLPATH_SAMPLE_PARAM_INSTRU_UVD_WL				_T("Sample/Parameter/Instrument/UVDetector/WaveLength")

//Sample(Root)-->参数-->校正参数
#define XMLPATH_SAMPLE_PARAM_CALIBRATE			_T("Sample/Parameter/Calibrate")
//Sample(Root)-->参数-->校正参数-->校正方法
#define XMLPATH_SAMPLE_PARAM_CALI_METHOD		_T("Sample/Parameter/Calibrate/CalibrateMethod")
//Sample(Root)-->参数-->校正参数-->校正对象
#define XMLPATH_SAMPLE_PARAM_CALI_OBJECT		_T("Sample/Parameter/Calibrate/CalibrateObject")
//Sample(Root)-->参数-->校正参数-->校正过程表
#define XMLPATH_SAMPLE_PARAM_CALI_TABLE			_T("Sample/Parameter/Calibrate/CalibrateTable")
//Sample(Root)-->参数-->校正参数-->校正结果表
#define XMLPATH_SAMPLE_PARAM_CALI_RESULT_TABLE	_T("Sample/Parameter/Calibrate/CalibrateResultTable")
//Sample(Root)-->参数-->校正参数-->校正组份表
#define XMLPATH_SAMPLE_PARAM_CALI_COMPONENTTAB	_T("Sample/Parameter/Calibrate/CalibrateTable_Component")

//Sample(Root)-->参数-->校正设定参数
#define XMPPATH_SAMPLE_PARAM_CALI_SAMPLEAMOUNT   _T("Sample/Parameter/Calibrate/SampleAmount")
#define XMPPATH_SAMPLE_PARAM_CALI_SAMPLEUNIT     _T("Sample/Parameter/Calibrate/SampleUnit")
#define XMPPATH_SAMPLE_PARAM_CALI_PRODUCTFACTOR  _T("Sample/Parameter/Calibrate/ProductFactor")
#define XMPPATH_SAMPLE_PARAM_CALI_DILUTIONFACTOR _T("Sample/Parameter/Calibrate/DilutionFactor")
#define XMPPATH_SAMPLE_PARAM_CALI_CURVERFITTYPE  _T("Sample/Parameter/Calibrate/CurverFitType")
#define XMPPATH_SAMPLE_PARAM_CALI_ORIGTREATMENT  _T("Sample/Parameter/Calibrate/OrigTreatment")
#define XMPPATH_SAMPLE_PARAM_CALI_CALIWEIGHTING  _T("Sample/Parameter/Calibrate/CaliWeighting")	
#define XMPPATH_SAMPLE_PARAM_CALI_REFERENCEPEAK  _T("Sample/Parameter/Calibrate/ReferencePeak")	
#define XMPPATH_SAMPLE_PARAM_CALI_OTHERCALIPEAK  _T("Sample/Parameter/Calibrate/OtherCaliPeak")	


//Sample(Root)-->参数-->峰处理参数
#define XMLPATH_SAMPLE_PARAM_PEAKPROCESS		_T("Sample/Parameter/PeakProcess")
//Sample(Root)-->参数-->峰处理参数-->CP参数(连续点数)
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_CP        _T("Sample/Parameter/PeakProcess/CPcoefficient")
#define XMLPATH_PEAKPROC_CP						_T("PeakProcess/CPcoefficient")
//Sample(Root)-->参数-->峰处理参数-->AP参数(连续点数)
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_AP        _T("Sample/Parameter/PeakProcess/APcoefficient")
#define XMLPATH_PEAKPROC_AP						_T("PeakProcess/APcoefficient")
//Sample(Root)-->参数-->峰处理参数-->噪声阀门
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_TRD		_T("Sample/Parameter/PeakProcess/Threshold")
#define XMLPATH_PEAKPROC_TRD					_T("PeakProcess/Threshold")
//Sample(Root)-->参数-->峰处理参数-->最小半峰宽
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_MPW		_T("Sample/Parameter/PeakProcess/MinHalfPeakWidth")
#define XMLPATH_PEAKPROC_MPW					_T("PeakProcess/MinHalfPeakWidth")
//Sample(Root)-->参数-->峰处理参数-->最小峰面积
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_MPA		_T("Sample/Parameter/PeakProcess/MinPeakArea")
#define XMLPATH_PEAKPROC_MPA					_T("PeakProcess/MinPeakArea")
//Sample(Root)-->参数-->峰处理参数-->最小峰高
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_MPH	    _T("Sample/Parameter/PeakProcess/Height")
#define XMLPATH_PEAKPROC_MPH					_T("PeakProcess/Height")
//Sample(Root)-->参数-->峰处理参数-->峰处理参数表
#define XMLPATH_SAMPLE_PARAM_PEAKPROC_PPT		_T("Sample/Parameter/PeakProcess/PPTimeTable")
#define XMLPATH_PEAKPROC_PPT					_T("PeakProcess/PPTimeTable")
//Sample(Root)-->参数-->报表-->默认报表
#define XMLPATH_SAMPLE_PARAM_REPORT				_T("Sample/Parameter/Report")
#define XMLPATH_SAMPLE_REPORTNAME			    _T("Sample/Parameter/Report/ReportDefault")

//Sample(Root)-->数据
#define XMLPATH_SAMPLE_DATA						_T("Sample/Data")          
//Sample(Root)-->数据-->曲线数据
#define XMLPATH_SAMPLE_DATA_CURVE				_T("Sample/Data/CurveDatas")          

//Sample(Root)-->结果
#define XMLPATH_SAMPLE_RESULT					_T("Sample/Result")          
//Sample(Root)-->结果-->峰列表
#define XMLPATH_SAMPLE_RESULT_PEAKLIST			_T("Sample/Result/PeakListTable")          
//Sample(Root)-->结果-->结果表
#define XMLPATH_SAMPLE_RESULT_RESULT			_T("Sample/Result/ResultTable")   
//Sample(Root)-->结果-->打印路径
#define XMLPATH_SAMPLE_RESULT_PRINTPIC			_T("Sample/Result/PrintPic")  

//Sample(Root)-->报表
#define XMLPATH_SAMPLE_REPORT					_T("Sample/Report")          

//用户自定义输入字段
#define XMLPATH_SAMPLE_REPORT_RT				_T("Sample/Parameter/Report/ReportTitle")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_MFM      _T("Sample/Parameter/Instrument/MainFrame/MainFrameModel")
#define XMLPATH_SAMPLE_PARAM_INSTRUMENT_IM		_T("Sample/Parameter/Instrument/InjectionMethod")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TI		_T("Sample/Parameter/Instrument/MainFrame/Temp_Injector")
#define XMLPATH_SAMPLE_PARAM_INFO_IV			_T("Sample/Parameter/Information/InjectVolume") 
#define XMLPATH_SAMPLE_PARAM_INSTRU_CI		    _T("Sample/Parameter/Instrument/ColumnInfo")
#define XMLPATH_SAMPLE_PARAM_INSTRU_CG		    _T("Sample/Parameter/Instrument/CarrierGas")
#define XMLPATH_SAMPLE_PARAM_INSTRU_FR		    _T("Sample/Parameter/Instrument/FlowRate")
#define XMLPATH_SAMPLE_PARAM_INSTRU_D			_T("Sample/Parameter/Instrument/Detector")
#define XMLPATH_SAMPLE_PARAM_INSTRU_DETECTOR_CD   _T("Sample/Parameter/Instrument/Detector/CurrentDetector")
#define XMLPATH_SAMPLE_PARAM_INSTRU_DETECTOR_TEMP _T("Sample/Parameter/Instrument/Detector/Temp")
#define XMLPATH_SAMPLE_PARAM_INSTRU_DETECTOR_YZQDL _T("Sample/Parameter/Instrument/Detector/YZQDL")
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_D        _T("Sample/Parameter/Instrument/MainFrame/Detector") //待废弃
#define XMLPATH_SAMPLE_PARAM_INSTRU_MF_TD       _T("Sample/Parameter/Instrument/MainFrame/Temp_Detector")



//------------------------------------------------------------------------+
//-------------------------------- XML属性定义 ----------------------------|
//------------------------------------------------------------------------+

//--------通用属性----
#define XMLPROP_DISPLAY     _T("_E_Display_")   //显示
#define XMLPROP_NAME		_T("_E_Name_")		//名称
#define XMLPROP_VALUETYPE	_T("_E_Type_")		//数据类型
#define XMLPROP_FORMAT		_T("_E_Format_")	//数据显示格式
#define XMLPROP_UNIT		_T("_E_Unit_")		//数据单位
#define XMLPROP_VALUE		_T("_E_Value_")		//数据(缺省)值
#define XMLPROP_SOURCE      _T("_E_Source_")    //字段来源，是测量还是用户自己输入
#define XMLPROP_DEFAULTVALUE      _T("DefaultValue")


//--------表格属性----
#define XMLPROP_TAB_ORDERS			_T("Orders")
#define XMLPROP_TAB_ORDERBY			_T("OrderBy")
#define XMLPROP_TAB_ASC				_T("Asc")

//每列属性(注：每列同时具有上面5项通用属性
#define XMLPROP_TAB_COLUMN			_T("Column")		//列标记

#define XMLPROP_TAB_COL_ID			_T("ID")			//列ID
#define XMLPROP_TAB_COL_TITLE		_T("Title")			//列标题
#define XMLPROP_TAB_COL_WIDTH		_T("ColumnWidth")	//列宽度
#define XMLPROP_TAB_COL_READONLY	_T("Readonly")		//true或者false,是否能编辑
#define XMLPROP_TAB_COL_CANSIZE		_T("CanSize")		//true或者false,是否能改变列宽
#define XMLPROP_TAB_COL_ALIGN		_T("Align")			//left,center,right|top,vcenter,middle,bottom,这七个值不区分大小写
#define XMLPROP_TAB_COL_CTRLTYPE	_T("CtrlType")		//控件类型
#define XMLPROP_TAB_COL_CTRLDATA	_T("CtrlData")		//控件数据，和控件类型对应
#define XMLPROP_TAB_ROW				_T("Row")			//行标记

//------------------------------------------------------------------------+
//--------------------------------- XML值定义 -----------------------------|
//------------------------------------------------------------------------+
#define XMLVALUE_TYPE_STRING	_T("string")
#define XMLVALUE_TYPE_INT		_T("int")
#define XMLVALUE_TYPE_FLOAT		_T("float")
#define XMLVALUE_TYPE_TIME		_T("time")
#define XMLVALUE_TYPE_TABLE		_T("table")
#define XMLVALUE_TYPE_CURVE2D	_T("curve2d")
#define XMLVALUE_TYPE_ICON		_T("icon")

#define XMLVALUE_CTRLTYPE_DEFAULT		_T("default")
#define XMLVALUE_CTRLTYPE_CHECKBOX		_T("checkbox")
#define XMLVALUE_CTRLTYPE_DROPLIST		_T("droplist")

//------------------------------------------------------------------------+
//-------------------------------参数常值定义 -----------------------------|
//------------------------------------------------------------------------+
// 定量 Sample/Calibrate 下面的常量定义
#define XMLVALUE_CONST_c_percentage				    _T("百分比法")		//百分比法
#define XMLVALUE_CONST_c_normalization				_T("归一化法")	//归一化法
#define XMLVALUE_CONST_c_internal					_T("内标法")		//内标法
#define XMLVALUE_CONST_c_external					_T("外标法")		//外标法
#define XMLVALUE_CONST_c_internal_percentage   		_T("内标百分比法")	//内标百分比法
#define XMLVALUE_CONST_c_external_percentage		_T("外标百分比法")	//外标百分比法

#define XMLVALUE_CONSTENT_c_peak_area		_T("峰面积")		//峰面积
#define XMLVALUE_CONSTENT_c_peak_height		_T("峰高")		//峰高
/*
#define XMLVALUE_CONST_c_percentage				    _T("c_percentage")		//百分比法
#define XMLVALUE_CONST_c_normalization				_T("c_normalization")	//外标归一化法
#define XMLVALUE_CONST_c_internal					_T("c_internal")		//内标法
#define XMLVALUE_CONST_c_external					_T("c_external")		//外标法
#define XMLVALUE_CONST_c_internal_percentage   		_T("c_internal_percentage")	//内标百分比法
#define XMLVALUE_CONST_c_external_percentage		_T("c_external_percentage")	//外标百分比法

#define XMLVALUE_CONSTENT_c_peak_area		_T("c_peak_area")		//峰面积
#define XMLVALUE_CONSTENT_c_peak_height		_T("c_peak_height")		//峰高
*/



// 数据保留格式
#define SLOPE_DATA_FORMAT					_T("%.4f")  // 斜率、截距和相关系数保留到小数点后4位
#define INTERCEPT_DATA_FORMAT				_T("%.4f")
#define COEFFICIENT_DATA_FORMAT				_T("%.4f")  
#define RESERVETIME_DATA_FORMAT				_T("%.4f")  // 保留时间
#define CONTENTS_DATA_FORMAT				_T("%.3E")  // 含量

//------------------------------------------------------------------------ +
//----------------------        表格名称      ----------------------------- |
//------------------------------------------------------------------------ +
#define CALIBRATETABLE						 _T("CalibrateTable")		//校正过程表
#define CALIBRATERESULTTABLE				 _T("CalibrateResultTable") //校正结果表
#define PEAKLISTTABLE						 _T("PeakListTable")		//积分结果表
#define RESULTTABLE							 _T("ResultTable")		    //分析结果表

/*

//	|	
//	--->分析任务
#define XMLPATH_TASK_NAME				_T("Task/Name")			//任务名称
#define XMLPATH_TASK_CREATETIME			_T("Task/CreateTime")	//创建时间
#define XMLPATH_TASK_LIST				_T("Task/TaskList")		//任务列表
#define XMLPATH_TASK_TIMES				_T("Task/Times")		//任务列表
//	|	
*/

