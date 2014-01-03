#include "stdafx.h"
#include "Base_AnalysisSample.h"
#include "QuantitativeCalculation.h"
#include "C2Bitmap.h"
#include "PeakSeak.h" 


CBase_AnalysisSample::CBase_AnalysisSample(CBase_AnalysisTemplate *pTemplate) : 
	m_pTemplate(pTemplate),
	m_pMethod(NULL),
	m_pChartCtrl(NULL),
	m_statusCurrent(CBase_AnalysisSample::null),
	m_dStartAcquireTime_mins(0),
	m_cResultList(XMLPATH_SAMPLE_RESULT_RESULT, &m_data),
	m_cPeakList(XMLPATH_SAMPLE_RESULT_PEAKLIST, &m_data),
	m_cAllRealTimePeakList(XMLPATH_SAMPLE_RESULT_PEAKLIST, &m_data),
	m_dMaxThreShold(10000.0),
	m_dMinThreShold(0.0),
	m_bNeedStop(false),m_dblAutoStopMins(0),m_bZero(false),
	m_ptrRefCount(0) // [ 2013-02-21 chainyu ] 引用计数
{
	m_data.SetFlags(CAnalysisData::ADTF_VALIDATEONGETTING_DISABLED|CAnalysisData::ADTF_VALIDATEONSETTING_COMPATIBLE|CAnalysisData::ADTF_MODEONSETTING_CREATE|CAnalysisData::ADTF_USINGFORMATONSETTING|CAnalysisData::ADTF_USINGFORMATONGETTING);
	m_data.Create(XMLROOT_SAMPLE);
	
	m_strSavePath = m_pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE);
}

CBase_AnalysisSample::~CBase_AnalysisSample()
{
	Save();

	m_pTemplate->Notify_SampleDead(this);
}


///////////////////////////////////////////////////////////////////////////
CString CBase_AnalysisSample::GetSampleName() const
{
	CString value;
	if(m_data.GetValue_String(XMLPATH_SAMPLE_SAMPLENAME, value) == IAERROR_SUCCESS) return value;
	return _T("");
}
bool CBase_AnalysisSample::SetSampleName(LPCTSTR lpcszName)
{
	return m_data.SetValue_String(XMLPATH_SAMPLE_SAMPLENAME, lpcszName) == IAERROR_SUCCESS;
}

CString CBase_AnalysisSample::GetReportName() const{
	CString value;
	if(m_data.GetValue_String(XMLPATH_SAMPLE_REPORTNAME, value) == IAERROR_SUCCESS) return value;
	return _T("");
}

bool CBase_AnalysisSample::SetReportName(LPCTSTR lpcszName)
{
	return m_data.SetValue_String(XMLPATH_SAMPLE_REPORTNAME, lpcszName) == IAERROR_SUCCESS;
}

int CBase_AnalysisSample::GetInjectNO() const
{
	int value;
	if(m_data.GetValue_Int(XMLPATH_SAMPLE_SAMPLEINJECTNO, value) == IAERROR_SUCCESS) return value;
	return 0;
}
bool CBase_AnalysisSample::SetInjectNO(int iNO)
{
	ASSERT(iNO > 0);
	return m_data.SetValue_Int(XMLPATH_SAMPLE_SAMPLEINJECTNO, iNO) == IAERROR_SUCCESS;
}

CBase_AnalysisSample::enumSampleType CBase_AnalysisSample::GetType() const
{
	CString value;
	m_data.GetValue_String(XMLPATH_SAMPLE_SAMPLEPURPOSE, value);
	return FromTypeString(value);
}
bool CBase_AnalysisSample::SetType(CBase_AnalysisSample::enumSampleType eType)
{
	return m_data.SetValue_String(XMLPATH_SAMPLE_SAMPLEPURPOSE, ToTypeString(eType)) == IAERROR_SUCCESS;
}
LPCTSTR CBase_AnalysisSample::TypeDescription() const
{
	return ToTypeDescription(GetType());
}
LPCTSTR CBase_AnalysisSample::ToTypeDescription(CBase_AnalysisSample::enumSampleType eType)
{
	switch(eType)
	{
	case SAMPLE_TEST:return _T("试样");
	case SAMPLE_STANDARD:return _T("标样");
	default:return _T("");
	}
}
LPCTSTR CBase_AnalysisSample::ToTypeString(CBase_AnalysisSample::enumSampleType eType)
{
	switch(eType)
	{
	case SAMPLE_TEST:return _T("analysis");
	case SAMPLE_STANDARD:return _T("standard");
	default:return _T("");
	}
}
CBase_AnalysisSample::enumSampleType CBase_AnalysisSample::FromTypeString(const CString& typeString, enumSampleType eDefault)
{
	if(typeString == _T("analysis")) return SAMPLE_TEST;
	if(typeString == _T("standard")) return SAMPLE_STANDARD;
	return eDefault;
}

int CBase_AnalysisSample::GetInstrumentChannel(void) const
{
	int value;
	if(m_data.GetValue_Int(XMLPATH_SAMPLE_PARAM_INSTRU_SC, value) == IAERROR_SUCCESS) return value;
	return 0;
}

CString CBase_AnalysisSample::GetSourceMethod() const
{
	CString value;
	if(m_data.GetValue_String(XMLPATH_SAMPLE_METHODNAME, value) == IAERROR_SUCCESS) return value;
	return _T("");
}
bool CBase_AnalysisSample::SetSourceMethod(LPCTSTR lpcszMethodName)
{
	return m_data.SetValue_String(XMLPATH_SAMPLE_METHODNAME, lpcszMethodName) == IAERROR_SUCCESS;
	//需要重新加载m_pMethod,并替换掉内部的方法段
	//m_pTemplate->FreeMethod(m_pMethod);
	//m_pMethod = m_pTemplate->LoadMethod(lpcszMethodName);
	//ReplaceMethod();
}

CBase_AnalysisSample::enumDrawType CBase_AnalysisSample::GetDrawType() const
{
	int value;
	//if(m_data.GetValue_Int(XMLPATH_SAMPLE_DRAWTYPE, value) == IAERROR_SUCCESS) return (enumDrawType)value;
	//以后应该成曲线属性里读出来
	return DT_LINE;
}
bool CBase_AnalysisSample::SetDrawType(CBase_AnalysisSample::enumDrawType eDrawType)
{
	//以后应该设置到曲线属性里，该函数还需要增加参数:哪一条曲线

	//return m_data.SetValue_Int(XMLPATH_SAMPLE_DRAWTYPE, eDrawType) == IAERROR_SUCCESS;
	return TRUE;
}

LPCTSTR CBase_AnalysisSample::GetStatusString(void) const
{
	switch(m_statusCurrent)
	{
	case null:return _T("空样品");
	case preparing:return _T("样品准备");
	case ready:return _T("样品准备好");		
	case running:return _T("采样中...");		
	case idle:return _T("采样结束");		
	case finish:return _T("完成");		
	case abort:return _T("异常终止");		
	case busy:return _T("样品忙");
	default:return _T("未知状态");
	}
}
void CBase_AnalysisSample::SetStatus(CBase_AnalysisSample::STATUS status)
{
	if(m_statusCurrent == status) return;

	m_statusCurrent = status;
	m_pTemplate->Notify_SampleStatusChanged(this);
}

CString CBase_AnalysisSample::GetCurMethod() const
{
	if(m_pMethod == NULL) return GetSourceMethod();
	return m_pMethod->GetName();
}

bool CBase_AnalysisSample::SaveChart2File(int iWidth, int iHeight, const CString& file)
{
	if(!m_pChartCtrl) return false;
	if(!::IsWindow(m_pChartCtrl->GetSafeHwnd())) return false;

	m_pChartCtrl->EnableRefresh(false);

	CWnd* pParent = m_pChartCtrl->GetParent();
	CRect oldRCParent, oldRCChart;
	m_pChartCtrl->GetWindowRect(oldRCChart);
	pParent->ScreenToClient(oldRCChart);
	pParent->GetWindowRect(oldRCParent);
	if(pParent->GetParent()) pParent->GetParent()->ScreenToClient(oldRCParent);

	int offsetW = iWidth - oldRCChart.Width();
	int offsetH = iHeight - oldRCChart.Height();
	CRect rc(oldRCParent);
	rc.right += offsetW;
	rc.bottom += offsetH;
	pParent->MoveWindow(rc);

	rc = oldRCChart;
	rc.right = rc.left + iWidth;
	rc.bottom = rc.top + iHeight;
	m_pChartCtrl->MoveWindow(rc);

	COLORREF oldBkColor = m_pChartCtrl->GetBackColor();
	m_pChartCtrl->SetBackColor(RGB(255,255,255));

	bool oldLeftGrid = m_pChartCtrl->GetLeftAxis()->GetGrid()->IsVisible();
	bool oldBottomGrid = m_pChartCtrl->GetBottomAxis()->GetGrid()->IsVisible();
	m_pChartCtrl->GetLeftAxis()->GetGrid()->SetVisible(false);
	m_pChartCtrl->GetBottomAxis()->GetGrid()->SetVisible(false);

	m_pChartCtrl->EnableRefresh(true);
	m_pChartCtrl->RefreshCtrl();

	C2Bitmap::SaveToFile(m_pChartCtrl->GetDC(), CRect(0,0,iWidth,iHeight), file, iWidth, iHeight);//保存pDC的rc区域的内容为指定w和h的bmp，0表示无缩放

	//恢复
	m_pChartCtrl->EnableRefresh(false);
	m_pChartCtrl->MoveWindow(oldRCChart);
	pParent->MoveWindow(oldRCParent);
	m_pChartCtrl->SetBackColor(oldBkColor);
	m_pChartCtrl->GetLeftAxis()->GetGrid()->SetVisible(oldLeftGrid);
	m_pChartCtrl->GetBottomAxis()->GetGrid()->SetVisible(oldBottomGrid);
	m_pChartCtrl->EnableRefresh(true);
	m_pChartCtrl->RefreshCtrl();

	return true;
}

void CBase_AnalysisSample::RefreshDraw(enumDrawType eDrawType)
{
	//ASSERT(false);
	SetDrawType(eDrawType);
	//刷新绘图
	if(!m_pChartCtrl) return;
	CWnd* parent = m_pChartCtrl->GetParent();
	if(parent) parent->SendMessage(WM_USER+847, eDrawType, NULL);
}

///////////////////////////////////////////////////////////////////////////
CString CBase_AnalysisSample::CreateSampleFileName(CString cStrSampleName, int nInjectNO) 
{
	ASSERT(nInjectNO > 0);
	CString cStrTemp;cStrTemp.Format(_T("%s_#%d%s"), cStrSampleName, nInjectNO, ANALYSIS_SAMPLE_FILE_EXT);
	return cStrTemp;
}
CString CBase_AnalysisSample::GetSampleFileTitle() const
{
	CString cStrTemp;cStrTemp.Format(_T("%s_#%d"), GetSampleName(), GetInjectNO());
	return cStrTemp;
}
CString CBase_AnalysisSample::GetCurveFileName(const CString& cStrMainGroup, const CString& cStrMainSerie)
{
	CString cStrName;
	CIADataCurve2D xmlC2D;
	VERIFY(PASUCCESS(GetIADataCurve2D(xmlC2D)));

	VERIFY(xmlC2D.QuickGetCurve2DFile(cStrMainGroup, cStrMainSerie, cStrName));
	return cStrName;
}

bool CBase_AnalysisSample::UpdateNames()
{
	CString strNewSampleFileName = CreateSampleFileName(GetSampleName(), GetInjectNO());
	if(CStringExt::File_Exist(strNewSampleFileName)) return false;

	if(strNewSampleFileName == m_strOldSampleFileName) return true;

	if(m_strOldSampleFileName.IsEmpty())
	{
		m_strSampleFileName = strNewSampleFileName;
		return true;
	}

	if(!MoveFile(m_strSavePath + m_strOldSampleFileName, m_strSavePath + m_strSampleFileName)) return false;
	
	m_strOldSampleFileName = m_strSampleFileName;
	m_strSampleFileName = strNewSampleFileName;

	CIADataCurve2D xmlC2D;
	if(PASUCCESS(GetIADataCurve2D(xmlC2D)))
	{
		CString type, name, fileName, newFileName;
		CIADataCurve2DSingle single;
		xmlC2D.BeginEnum();
		while(xmlC2D.GetNext(type, single))
		{
			single.BeginEnum();
			while(single.GetNext(name, fileName))
			{
				bool isMainCurve = (type == STRING_MAINCURVE_GROUP) && (name == STRING_MAINCURVE_NAME);
				if(isMainCurve) newFileName = CreateMainCurveFileName();
				else newFileName = CreateCurveFileName(type, name);

				if(newFileName == fileName) continue;
				
				if(!CStringExt::File_Exist(m_strSavePath + fileName) || MoveFile(m_strSavePath + fileName, m_strSavePath + newFileName)) single.SetEnum(newFileName);
			}
		}

		m_data.SetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D);
	}

	return true;
}
bool CBase_AnalysisSample::UpdateSampleName(LPCTSTR lpszSampleName, int nInjectNO)
{
	CString strOldSampleName = GetSampleName();
	int nOldInjectNO = GetInjectNO();

	SetSampleName(lpszSampleName);
	SetInjectNO(nInjectNO);

	if(UpdateNames()) return true;

	SetSampleName(strOldSampleName);
	SetInjectNO(nOldInjectNO);
	return false;
}

///////////////////////////////////////////////////////////////////////////
int CBase_AnalysisSample::GetIADataCurve2D(CIADataCurve2D& xmlC2D) const
{
	return m_data.GetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D);
}

BOOL CBase_AnalysisSample::Create(LPCTSTR lpszSampleName, int nInjectNO)
{
	SetSampleName(lpszSampleName);
	SetInjectNO(nInjectNO);
	UpdateNames();

	CreateID();
	XML_InitMainCurveDataAndClearOtherCurves();

	m_data.SetValue_Time(XMLPATH_SAMPLE_PARAM_INFO_ANALYSISTIME, CTime::GetCurrentTime());

	return true;
}
BOOL CBase_AnalysisSample::Load(LPCTSTR lpcszName, int nInjectNO)
{
	return LoadSampleFile(m_strSavePath + CreateSampleFileName(lpcszName, nInjectNO));
}


BOOL CBase_AnalysisSample::LoadSampleFile(const CString& strPathName)
{
	CFileFind find;
	if(!find.FindFile(strPathName)) return FALSE;

	find.FindNextFile();
	CString strNewSavePath = find.GetFilePath();
	strNewSavePath = strNewSavePath.Left(strNewSavePath.GetLength()-find.GetFileName().GetLength());
	strNewSavePath.Replace(_T('/'), _T('\\'));

	if(strNewSavePath.GetAt(strNewSavePath.GetLength()-1) != _T('\\')) strNewSavePath += _T('\\');

	if (m_data.LoadFromFile(strPathName) != IAERROR_SUCCESS) return false;

	//此处可加入样品文件名和文件中保存的样品名是否一致的验证与处理

	m_strSavePath = strNewSavePath;
	m_strSampleFileName = m_strOldSampleFileName = find.GetFileName();

	m_statusCurrent = DetectSampleStatusAndLoadMainCurveData();

	CParamChanged item;
	item.FullMark();
	m_pTemplate->Notify_SampleDataChanged(this, item);

	return TRUE;
}

BOOL CBase_AnalysisSample::Save(void)
{
	if(m_strSampleFileName.IsEmpty()) return false;

	//保存图表信息
	if(m_pChartCtrl && IsWindow(m_pChartCtrl->GetSafeHwnd()))
	{
		CIADataCurve2D xmlC2D;
		GetIADataCurve2D(xmlC2D);

		CIADataCurve2DSingle single;
		xmlC2D.AutoCurve2DSingle(_T("ResultPoints"), single);

		CString tmp;

		CChartAxis* pXAxis = m_pChartCtrl->GetBottomAxis();
		double dblMin, dblMax;
		pXAxis->GetMinMax(dblMin, dblMax);
		tmp.Format(_T("%.5f"), dblMin);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMIN_BOTTOM, tmp);
		tmp.Format(_T("%.5f"), dblMax);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMAX_BOTTOM, tmp);

		CChartAxis* pYAxis = m_pChartCtrl->GetLeftAxis();
		pYAxis->GetMinMax(dblMin, dblMax);
		tmp.Format(_T("%.5f"), dblMin);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMIN_LEFT, tmp);
		tmp.Format(_T("%.5f"), dblMax);
		single.SetProperty(XMLPROP_SAMPLECHART_RANGEMAX_LEFT, tmp);

		tmp.Format(_T("%d"), GetDrawType());
		single.SetProperty(_T("nDrawType"), tmp);

		m_data.SetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D);
	}

	if(m_data.SaveToFile(m_strSavePath + m_strSampleFileName) != IAERROR_SUCCESS) return false;

	m_strOldSampleFileName = m_strSampleFileName;
	return true;
}
BOOL CBase_AnalysisSample::Delete()
{
	CString fpn = m_strSavePath + m_strSampleFileName;
	if(CStringExt::File_Exist(fpn))
	{
		if(!DeleteFile(fpn)) return false;

		//删除图表信息
		CIADataCurve2D xmlC2D;
		if(PASUCCESS(GetIADataCurve2D(xmlC2D)))
		{
			CIADataCurve2DSingle single;
			long lPos = 0;
			xmlC2D.BeginEnum(lPos);
			CString type, name, file;
			while(xmlC2D.GetNext(lPos, type, single))
			{
				single.BeginEnum();
				while(single.GetNext(name, file))
				{
					DeleteFile(m_strSavePath + file);
				}
			}
		}
	}

	m_strOldSampleFileName = _T("");
	m_strSampleFileName = _T("");
	return true;
}

void CBase_AnalysisSample::GetDataFrom(const CBase_AnalysisMethod &src)
{
	CString cStrMethod = src.m_cCurrentMethodData.GetXMLString();

	//这样做不太好，临时凑合
	VERIFY(cStrMethod.Replace(_T("<Method>"), _T("<Sample>"))==1);
	VERIFY(cStrMethod.Replace(_T("</Method>"), _T("</Sample>"))==1);
	m_data.SetXMLString(cStrMethod);
}

void CBase_AnalysisSample::XML_ClearData()
{
	XML_InitMainCurveDataAndClearOtherCurves();

	CIADataTable table;
	if(m_data.GetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, table) == IAERROR_SUCCESS)
	{
		table.DeleteAllRows();
		m_data.SetValue_Table(XMLPATH_SAMPLE_RESULT_PEAKLIST, table);
	}
	if(m_data.GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, table) == IAERROR_SUCCESS)
	{
		table.DeleteAllRows();
		m_data.SetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, table);
	}
}

///////////////////////////////////////////////////////////////////////////
void CBase_AnalysisSample::CreateID()
{
	m_data.SetValue_Int(XMLPATH_SAMPLE_SAMPLEID, ++g_cConfigIni.CurrentSystem().m_iNextSampleID);
}
void CBase_AnalysisSample::XML_InitMainCurveDataAndClearOtherCurves()
{
	CIADataCurve2D curve;
	curve.QuickSetCurve2DFile(STRING_MAINCURVE_GROUP, STRING_MAINCURVE_NAME, CreateMainCurveFileName());
	m_data.SetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE,curve);
}
CBase_AnalysisSample::STATUS CBase_AnalysisSample::DetectSampleStatusAndLoadMainCurveData()
{
	CIADataCurve2D xmlC2D;
	if(m_data.GetValue_Curve2D(XMLPATH_SAMPLE_DATA_CURVE, xmlC2D) != IAERROR_SUCCESS) return null;
	
	CString strMainGroup, strMainSerie;
	if(!GetIADataCurve2D_MainInfo(&strMainGroup, &strMainSerie)) return null;

	//调入样品后确定该样品的状态
	CString strFileName;
	if(!xmlC2D.QuickGetCurve2DFile(strMainGroup, strMainSerie, strFileName)) return null;

	LoadMainCurveData(m_strSavePath + GetMainCurveFileName());
	
	if(m_dataMainCurve.m_arrayData.GetCount() < 1) return null;//无数据

	//再查看有没有结果数据
	CIADataTable dt;
	if(m_data.GetValue_Table(XMLPATH_SAMPLE_RESULT_RESULT, dt) == IAERROR_SUCCESS && dt.GetRowsNumber() > 0) return finish;
	else return idle;
}
void CBase_AnalysisSample::LoadMainCurveData(const CString& strMainCurvePathName)
{
	CIAFileCurve2D file;
	if(!file.Open(strMainCurvePathName)) return;

	m_dataMainCurve.RemoveAll();

	file.BeginEnum();
	double x=0, y=0;
	CTime t;
	while(file.GetNext(x, y, t)) m_dataMainCurve.AppendData(x, y);
	file.Close();
}

///////////////////////////////////////////////////////////////////////////
BOOL CBase_AnalysisSample::IsNameExist(LPCTSTR lpcszName, int nInjectNO,CBase_AnalysisTemplate* pTemplate)
{
	ASSERT(nInjectNO > 0);

	return CStringExt::File_Exist(pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE) + CreateSampleFileName(lpcszName, nInjectNO));
}
BOOL CBase_AnalysisSample::Delete(LPCTSTR lpcszName, int nInjectNO, CBase_AnalysisTemplate* pTemplate)
{
	return DeleteFile(pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE) + CreateSampleFileName(lpcszName, nInjectNO));
}
BOOL CBase_AnalysisSample::GetSampleNameList(CStringArray &arrayName, CBase_AnalysisTemplate* pTemplate)
{
	CString path = pTemplate->GetDataFolder(PA_DATATYPENAME_SAMPLE) + _T("*") + ANALYSIS_SAMPLE_FILE_EXT;

	CFileFind ff;
	BOOL ret = ff.FindFile(path);
	while(ret)
	{
		ret = ff.FindNextFile();
		if(ff.IsDirectory() || ff.IsDots() || ff.IsHidden()) continue;		

		CString fileName = ff.GetFileName();
		fileName = fileName.Left(fileName.GetLength() - _tcslen(ANALYSIS_SAMPLE_FILE_EXT));
		arrayName.Add(fileName);
	}	

	return true;
}
BOOL CBase_AnalysisSample::GetReportNameList(CStringArray &arrayName, CBase_AnalysisTemplate* pTemplate)
{
	CString path = pTemplate->GetDataFolder(PA_DATATYPENAME_CRYSTALREPORT) + _T("*") + CRYSTAL_REPORT_FILE_EXT;

	CFileFind ff;
	BOOL ret = ff.FindFile(path);
	while(ret)
	{
		ret = ff.FindNextFile();
		if(ff.IsDirectory() || ff.IsDots() || ff.IsHidden()) continue;		

		CString fileName = ff.GetFileName();
		fileName = fileName.Left(fileName.GetLength() - _tcslen(CRYSTAL_REPORT_FILE_EXT));
		arrayName.Add(fileName);
	}	

	return true;
}

///////////////////////////////////////////////////////////////////////////
bool CBase_AnalysisSample::IsStartRealtimeProcess() const
{ 
	////实时曲线积分功能开关
	if (m_pTemplate)
	{
		CAnalysisTask* pTask = m_pTemplate->m_pcWindowTask->GetTask();
		if (pTask)
		{
			const CAnalysisTaskItem* pItem = pTask->GetItem(this);
			if(pItem) 
				return pItem->m_bRealTimeProcess;
		}
	}
	return false;
}
int CBase_AnalysisSample::Base_RealtimeData(const CCurveData &curveData)
{
	if(GetStatus() == ready)
	{
		if(m_dStartAcquireTime_mins == -1 && curveData.m_arrayData.GetCount() > 0)
			m_dStartAcquireTime_mins = (DWORD)(curveData.m_arrayData.GetAt(0).dX*60000);
	}
	double xOffset = m_dStartAcquireTime_mins/-60000.0;

	CCurveData cd;

	//如果样品正在运行才保存数据
	if(GetStatus()==running) 
	{
		for(int i=0; i<curveData.m_arrayData.GetCount(); ++i) 
		{
			double x = curveData.m_arrayData[i].dX+xOffset;
			if(x < 0) continue;
			if(m_bZero && (x < 0.05))
			{
				m_dataMainCurve.AppendData(x, 0);
				cd.AppendData(curveData.m_arrayData[i].dX, 0);
			}
			else
			{
				m_dataMainCurve.AppendData(x, curveData.m_arrayData[i].dY);
				cd.AppendData(curveData.m_arrayData[i].dX, curveData.m_arrayData[i].dY);
			}
		}
	}
	else cd.AppendData(curveData);

	switch (GetStatus()){
// 		case idle:
// 		case finish:
// 			//AfxMessageBox(_T("Internal Error: 样品状态不正常！"));
// 			return IAERROR_CG_SAMPLE_STATUSERROR;
// 			break;
// 		default:
	case ready:
		if(m_pChartCtrl) 
			m_pChartCtrl->AddRealTimeCurveDataMainWithProcess(this, false, cd, 1.0, 1.0, xOffset, 0.0);  
		break;

	case running:
			//画实时曲线 单位从毫秒变成分钟；微伏变成毫伏

		if(m_pChartCtrl) 
			m_pChartCtrl->AddRealTimeCurveDataMainWithProcess(this, true, cd, 1.0, 1.0, xOffset, 0.0); // 函数内部包含实时曲线积分
		break;
	}

	if(GetStatus()==running && m_bNeedStop)
	{
		int count = curveData.m_arrayData.GetCount();
		if(curveData.m_arrayData[count-1].dX+xOffset >= m_dblAutoStopMins)
		{
			//TRACE(("Base_RealtimeData(%.3f)：%s\r\n"), curveData.m_arrayData[count-1].dX+xOffset, (LPCTSTR)(COleDateTime::GetCurrentTime()).Format(_T("%Y-%m-%dT%H:%M:%S")));
			m_pTemplate->Sample_StopRun(this);
		}
	}

	return IAERROR_SUCCESS;
}


BOOL CBase_AnalysisSample::Base_GetReady()
{
	m_dStartAcquireTime_mins = -1;
	SetStatus(ready);
	return TRUE;
}
//
//----------开始样品运行--------------------------------------
//
//返回值：
//	IAERROR_CG_SAMPLE_RUNNING：		错误，样品已经在运行
//	IAERROR_CG_SAMPLE_DATAEXIST：	错误，样品已经有数据
//	IAERROR_CG_INSTRUMENT_ERROR：	错误，仪器无法设置参数
//	IAERROR_CG_SAMPLE_BUSY：			样品正忙
//	IAERROR_CG_SAMPLE_NOTREADY		样品尚未准备好
//	IAERROR_SUCCESS：				样品已经准备好，可以进行下一步操作
//
//

int CBase_AnalysisSample::Base_StartRun(double dTimeMins)
{

	//
	//确定当前样品状态判断是否能进入运行状态
	//
	switch (GetStatus()){
	case ready		://样品准备好了	
		break;
	case running:	//采样							
		return IAERROR_CG_SAMPLE_RUNNING;
		break;
	case idle		://运行状态结束，可中断，但未完成数据处理
		return IAERROR_CG_SAMPLE_DATAEXIST;
		break;
	case finish		://完成						
		return IAERROR_CG_SAMPLE_DATAEXIST;
		break;
	case busy		://样品忙，不可中断状态
		return IAERROR_CG_SAMPLE_BUSY;
		break;
	case preparing	://样品准备(等待仪器参数到位)		
		return IAERROR_CG_SAMPLE_BUSY;
		break;
	case abort		://异常终止，或出错，数据应该都废除
	case null		://空闲（空样品）	
		return IAERROR_CG_SAMPLE_NOTREADY;
		break;
	default:
		ASSERT(FALSE);
		break;
	}


	//
	// 记录开始采样的时间，以便以后计算真正的保留时间
	// 
	m_dStartAcquireTime_mins = (DWORD)dTimeMins;
	m_dtStartRun = COleDateTime::GetCurrentTime();
	//TRACE("Base_StartRun（%.3f）：%s\r\n", dTimeMins, (LPCTSTR)m_dtStartRun.Format(_T("%Y-%m-%dT%H:%M:%S")));
	m_bNeedStop = false;
	//
	//仪器相关操作
	//
	//todo("可能需要启动程序升温或者其他仪器相关操作");
	//
	//开始记录数据
	//
	m_dataMainCurve.RemoveAll();

	//todo("与实时曲线数据记录的相关操作");
	//
	//界面操作
	//
	//在图形上加入开始标记
	//m_pChartCtrl->AddMarkLine(1);
	if(m_pChartCtrl)
	{
		m_pChartCtrl->EnableRefresh(false);
		m_pChartCtrl->m_pcCurveMain->ClearSerie();
		m_pChartCtrl->EnableRefresh(true);
		m_pChartCtrl->ResetAxis();
		m_pChartCtrl->RefreshCtrl();

		m_pChartCtrl->m_RealtimeDataMonitor->OnStartRun(this);
	}

	//改变状态
	SetStatus(running);

	return IAERROR_SUCCESS;

}

void CBase_AnalysisSample::Base_ResetData(void)
{
	::DeleteFile(m_strSavePath+GetMainCurveFileName());

	XML_ClearData();

	//清除曲线数据
	m_dataMainCurve.RemoveAll();

	//设置状态
	SetStatus(null);
	
	if(m_pChartCtrl)
	{
		m_pChartCtrl->EnableRefresh(false);
		m_pChartCtrl->m_pcCurveMain->ClearSerie();
		m_pChartCtrl->EnableRefresh(true);
		m_pChartCtrl->RefreshCtrl();
	}
}


//
//----------停止样品运行--------------------------------------
//
//返回值：
//	IAERROR_CG_SAMPLE_STATUSERROR：	错误，样品不在采样状态
//	IAERROR_SUCCESS：				操作成功
//

int  CBase_AnalysisSample::Base_StopRun(void)
{

	//
	//确定当前样品状态判断是否能进入停止状态
	//
	if(GetStatus()!=running)
		return IAERROR_CG_SAMPLE_STATUSERROR;

	//设定状态
	SetStatus(idle);
	
	m_data.SetValue_Time  (XMLPATH_SAMPLE_PARAM_INFO_ANALYSISTIME, CTime::GetCurrentTime());

	{//储存曲线数据
		CIAFileCurve2D file;

		SetStatus(idle);

		if(!file.Open(m_strSavePath+GetMainCurveFileName())) return IAERROR_FILE_NOTFOUND;

		for(int i=0; i<m_dataMainCurve.m_arrayData.GetCount(); ++i) {
			file.ApppendPoint(m_dataMainCurve.m_arrayData[i].dX, m_dataMainCurve.m_arrayData[i].dY);
		}
		file.Close();
	}

	m_dtStopRun = COleDateTime::GetCurrentTime();
	//TRACE("Base_StopRun：%s\r\n", (LPCTSTR)m_dtStopRun.Format(_T("%Y-%m-%dT%H:%M:%S")));

	//储存其他参数
	Save();


	//
	//界面操作
	//
	//清除当前图形窗口的显示内容
	
	//显示储存的样品曲线
	if(m_pChartCtrl)
	{
		m_pChartCtrl->EnableRefresh(false);
		m_pChartCtrl->ClearPeak(this, CBase_SampleChart::FLAGMASK_ALL);
		m_pChartCtrl->ClearCompSeries();
		m_pChartCtrl->ClearCurve2DSeries();
		m_pChartCtrl->ClearMarkLines();
		m_pChartCtrl->RedrawPeak(this, CBase_SampleChart::FLAG_DATA_SOURCE);
		m_pChartCtrl->ShowPeak(this, CBase_SampleChart::FLAG_DATA_SOURCE);
		m_pChartCtrl->EnableRefresh(true);
		m_pChartCtrl->RefreshCtrl();

		m_pChartCtrl->m_RealtimeDataMonitor->OnEndRun(this);
	}
	
	CBase_AnalysisSample::CParamChanged pc;
	pc.param_information = true;
	if(m_pTemplate) m_pTemplate->Notify_SampleDataChanged(this, pc);

	return IAERROR_SUCCESS;

}

int  CBase_AnalysisSample::Base_AbortReady(void)
{
	if(GetStatus() != ready) return IAERROR_CG_SAMPLE_STATUSERROR;
	
	Base_ResetData();//清除数据
	SetStatus(null);

	return IAERROR_SUCCESS;

}

//
//----------(异常)终止样品运行--------------------------------------
//
//返回值：
//	IAERROR_CG_SAMPLE_STATUSERROR：	错误，样品不在采样状态
//	IAERROR_SUCCESS：				操作成功
//

int  CBase_AnalysisSample::Base_AbortRun(void)
{

	//
	//确定当前样品状态判断是否能进入停止状态
	//
	if(GetStatus()!=running)
		return IAERROR_CG_SAMPLE_STATUSERROR;

	//停止记录数据，并且清除已采集数据
	//
	//todo("停止记录数据但不储存");
	Base_ResetData();//清除数据
	m_dtStopRun = COleDateTime::GetCurrentTime();
	//
	//界面操作
	//
	//todo("改变相关的界面显示");

	//改变状态
	SetStatus(null);

	if(m_pChartCtrl) m_pChartCtrl->m_RealtimeDataMonitor->OnEndRun(this);

	return IAERROR_SUCCESS;

}

bool CBase_AnalysisSample::Xml_PutParameter_Method(CAnalysisData& adMethod)
{
	CString xmlString;/*
	adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM_INSTRUMENT, xmlString);
	if(!Xml_PutParameter_Instrument(xmlString)) return false;
	adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM_CALIBRATE, xmlString);
	if(!Xml_PutParameter_Calibrate(xmlString)) return false;
	adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM_PEAKPROCESS, xmlString);
	if(!Xml_PutParameter_PeakProcess(xmlString)) return false;
	adMethod.GetValue_XMLString(XMLPATH_METHOD_REPORT, xmlString);
	if(!Xml_PutParameter_Report(xmlString)) return false;//*/
	if((adMethod.GetValue_XMLString(XMLPATH_METHOD_PARAM, xmlString) == IAERROR_SUCCESS) &&
		(m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM, xmlString) == IAERROR_SUCCESS))
	{
		CParamChanged itemChanged;
		itemChanged.Params();
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}
	return true;
}
bool CBase_AnalysisSample::Xml_PutParameter_Method(LPCTSTR xmlString)
{
	CAnalysisData adMethod;
	adMethod.SetXMLString(xmlString);

	return Xml_PutParameter_Method(adMethod);
}
bool CBase_AnalysisSample::Xml_PutParameter_Instrument(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_INSTRUMENT, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_instrument = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_Instrument2(LPCTSTR xmlString)
{
	CAnalysisData ad;
	if(ad.LoadFromDocument(xmlString) != IAERROR_SUCCESS) return false;
	CAnalysisData::static_Assign(CAnalysisData::static_Path(m_data.GetRoot(), XMLPATH_SAMPLE_PARAM_INSTRUMENT, false), ad.GetRoot(), CAnalysisData::ASSIGN_SELF|CAnalysisData::ASSIGN_CHILDREN|CAnalysisData::ASSIGN_NODE_EXISTING|CAnalysisData::ASSIGN_PROPERTY_EXISTING);
	return true;
// 	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_INSTRUMENT, xmlString) == IAERROR_SUCCESS;
// 	if(bRet)
// 	{
// 		CParamChanged itemChanged;
// 		itemChanged.param_instrument = 1;
// 		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
// 	}
// 
// 	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_Calibrate(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_CALIBRATE, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_calibration = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_PeakProcess(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_integration = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_PutParameter_Report(LPCTSTR xmlString)
{
	bool bRet = m_data.SetValue_XMLString(XMLPATH_SAMPLE_REPORT, xmlString) == IAERROR_SUCCESS;
	if(bRet)
	{
		CParamChanged itemChanged;
		itemChanged.param_information = 1;
		m_pTemplate->Notify_SampleDataChanged(this, itemChanged);
	}

	return bRet;
}
bool CBase_AnalysisSample::Xml_GetParameter_Method(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_Instrument(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_INSTRUMENT, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_Calibrate(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_CALIBRATE, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_PeakProcess(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, xmlString) == IAERROR_SUCCESS;
}
bool CBase_AnalysisSample::Xml_GetParameter_Report(CString& xmlString) const
{
	return m_data.GetValue_XMLString(XMLPATH_SAMPLE_REPORT, xmlString) == IAERROR_SUCCESS;
}


int	 CBase_AnalysisSample::Base_DataProcess(void)
{
	CPeakSeak	cPreaSeek;
	CAnalysisData cAnalysisData;
//	CIADataTable  cIAResultTable(NULL, _T("<PeakListTable></PeakListTable>"));
	CString cStrPeakProcess;

	//获得积分参数
	CString cStrTemp = m_data.GetXMLString();
	double *x, *y;
	int nNum = m_dataMainCurve.ConvertData(x, y, 1.0, 1.0, 0.0, 0.0);
	if(nNum<1)
		return IAERROR_SAMPLE_NODATA;

	int n;
		
	RETURNifERROR(m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_PEAKPROCESS, cStrPeakProcess));
	RETURNifERROR(cAnalysisData.LoadFromDocument(cStrPeakProcess));

	cStrTemp = cAnalysisData.GetXMLString();

	cPreaSeek.SetSemaDate(x, y, nNum);
	delete []x;
	delete []y;
	//cPreaSeek.SignalProcess(&cAnalysisData, &cIAResultTable);
	m_cPeakList.RemoveAll();
	m_arrayMPW.RemoveAll();
	m_arrayMPA.RemoveAll();
	m_arrayMPH.RemoveAll();
	m_arrayVLines.RemoveAll();
	//先不用该函数确定噪声最大值最小值
//	cPreaSeek.SignalProcess(&cAnalysisData, m_cPeakList, &m_arrayInterRes1, &m_arrayInterRes2, &m_dMinThreShold, &m_dMaxThreShold);
	double dTemp;
	cPreaSeek.SignalProcess(&cAnalysisData, m_cPeakList, &m_arrayMPW, &m_arrayMPA, &m_arrayMPH, &m_nFilteredMPW, &m_nFilteredMPA, &m_nFilteredMPH, &m_arrayVLines, &dTemp, &dTemp);
 	VERIFY(m_cPeakList.SynchronizeToXML());


	/*const unsigned PeakSize = cPreaSeek.m_Peak.size();
	for (unsigned i=0; i<PeakSize-1; ++i)
	{
		if (cPreaSeek.m_Peak[i].dType == CPeak::VERTDIVIDE)
		{
			structLine line;
			line.begin.x = line.end.x = cPreaSeek.m_Peak[i].FinSema.x;
			line.begin.y = cPreaSeek.m_OrigSema[cPreaSeek.m_Peak[i].FinSema.idx].y;
			line.end.y = cPreaSeek.m_Peak[i].FinSema.y;
			m_arrayVLines.Add(line);
		}
	}*/


	
	cStrTemp = m_data.GetXMLString();

	CParamChanged item;
	item.result_integration = 1;
	m_pTemplate->Notify_SampleDataChanged(this, item);

	return IAERROR_SUCCESS;
}


int	 CBase_AnalysisSample::Base_DataCalibrate(void)
{//和 定量计算 接口
	CString cStrTemp = m_data.GetXMLString();

	CAnalysisData cAnalysisData;
	CString cStrCalibrate;

	RETURNifERROR(m_data.GetValue_XMLString(XMLPATH_SAMPLE_PARAM_CALIBRATE, cStrCalibrate));
	RETURNifERROR(cAnalysisData.LoadFromDocument(cStrCalibrate));

	cStrTemp = cAnalysisData.GetXMLString();//调试

	CString cStrCalibrateMethod, cStrCalibrateObject;

	m_data.GetValue_String(XMLPATH_SAMPLE_PARAM_CALI_METHOD, cStrCalibrateMethod);
	m_data.GetValue_String(XMLPATH_SAMPLE_PARAM_CALI_OBJECT, cStrCalibrateObject);

	if(cStrCalibrateObject!=XMLVALUE_CONSTENT_c_peak_area&&cStrCalibrateObject!=XMLVALUE_CONSTENT_c_peak_height) {
		//ASSERT(FALSE);
		return IAERROR_XML_NOTVALID;
	}

	//峰匹配，定量计算
	CQuantitativeCalculation *pQC = new CQuantitativeCalculation(&m_data);
	int IRESULT = pQC->QuantitativeCalculationProcess();
	delete pQC;
	if (IRESULT!=IAERROR_SUCCESS) return IRESULT;
	
	//更新 m_cPeakList保留用积分结果的数据，m_cResultList从XML读数据（组分名+浓度），匹配3位精度后绘图
 	//ASSERT(m_cPeakList.SynchronizeFromXML());
 	ASSERT(m_cResultList.SynchronizeFromXML());

// 	在显示样品窗口过程中，会自动调用Base_DataProcess和Base_DataCalibrate；
//	Base_DataProcess已经把积分结果数据保存到m_cPeakList和样品文件；
//	此时再执行m_cPeakList.SynchronizeFromXML()，会将样品文件中的积分结果数据（就是刚刚保存大）读入到m_cPeakList
//  这样不仅进行了重复调用，关键是样品文件中的积分结果数据会丢失精度，导致图表中绘画出来的基线不在主曲线上
	  
/* //这段大概是要在外标归一化法时候处理的业务逻辑。把峰面积或峰高百分比赋值给浓度(?)
	if(cStrCalibrateMethod==XMLVALUE_CONST_c_normalization) {
		int nCount = m_cPeakList.m_cArrayData.GetCount();
		m_cResultList.RemoveAll();
		m_cResultList.m_cArrayData.Copy(m_cPeakList.m_cArrayData);
		for(int i=0; i<nCount; ++i) {
			if(cStrCalibrateObject==XMLVALUE_CONSTENT_c_peak_area)
				m_cResultList.m_cArrayData[i].m_dResultConcentration = m_cResultList.m_cArrayData[i].m_dAreaPer; 
			else 
				m_cResultList.m_cArrayData[i].m_dResultConcentration = m_cResultList.m_cArrayData[i].m_dHeightPer; 
		}
	}
*/

	//else if(cStrCalibrateMethod==XMLVALUE_CONST_c_cnormalization) {
	//	AfxMessageBox(_T("校正归一化法 尚未实现"));
	//	return IAERROR_XML_NOTVALID;
	//}
	//else if(cStrCalibrateMethod==XMLVALUE_CONST_c_external) {
	//	AfxMessageBox(_T("外标法 尚未实现"));
	//	return IAERROR_XML_NOTVALID;
	//}
	//else if(cStrCalibrateMethod==XMLVALUE_CONST_c_internal) {
	//	AfxMessageBox(_T("内标法 尚未实现"));
	//	return IAERROR_XML_NOTVALID;
	//}
	//else {
	//	CString cStrTemp;
	//	cStrTemp.Format(_T("定量方法名称错误：%s"), cStrCalibrateMethod);
	//	AfxMessageBox(cStrTemp);
	//	//ASSERT(FALSE);
	//	return IAERROR_XML_NOTVALID;
	//}


	//VERIFY(m_cResultList.SynchronizeToXML());

	cStrTemp = m_data.GetXMLString();

	CParamChanged item;
	item.result_analysis = 1;
	m_pTemplate->Notify_SampleDataChanged(this, item);

	//每次积分后保存一下
	//  [1/5/2013 洪浩]
	Save();

	return IAERROR_SUCCESS;


}

int CBase_AnalysisSample::GetVLines()
{
	const int peakListSize = m_cPeakList.m_cArrayData.GetSize();
	//const int resultListSize = m_cResultList.m_cArrayData.GetSize();
	if (!peakListSize)
	{ 
		return IAERROR_SAMPLE_NODATA;
	}

	// read file to get orignal line points
	double *x, *y;
	int nNum = m_dataMainCurve.ConvertData(x, y, 1.0, 1.0, 0.0, 0.0);
	if (nNum<1)
	{
		return IAERROR_SAMPLE_NODATA;
	}

	// get vertical peak separate line points
	int flag = 0;
	for (int i=0; i <= m_cPeakList.m_cArrayData.GetUpperBound(); i++)
	{
		const CPeakStandard& peak = m_cPeakList.m_cArrayData.GetAt(i);

		// must keep peak result in order
		double peak_X = peak.m_dataEnd.dX;
		for (; flag<nNum-1; flag++)
		{
			double curr_X = x[flag];
			double next_X = x[flag+1];

			// hack process data，but it can't make any guarantee for the precision
			if (curr_X <= peak_X && peak_X <= next_X)
			{
				double peak_Y = peak.m_dataEnd.dY;
				double curr_Y = y[flag];
				double next_Y = y[flag+1];

				if (curr_Y >= peak_Y || peak_Y >=peak_Y )
				{
					structLine line;

					line.begin.x = peak_X;
					line.end.x   = peak_X;
					line.begin.y = curr_Y;  //orig_Y;
					line.end.y   = peak_Y;

					m_arrayVLines.Add(line);
					break;
				}


			}
		}
	}

	delete []x;
	delete []y;

	return IAERROR_SUCCESS;
}