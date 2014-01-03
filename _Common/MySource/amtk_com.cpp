#include "StdAfx.h"
#include "amtk_com.h"

#import "AMTK_DLL.dll" rename_namespace("My_AMTK")
My_AMTK::_AMTK_ASPtr g_pAMTK = NULL;
BOOL WINAPI initAMTKDLL()
{
	try {
		if (g_pAMTK != NULL) return TRUE;
		if (g_pAMTK.CreateInstance(__uuidof(My_AMTK::AMTK_AS)) == S_OK) return TRUE;
	}
	catch(...) {
	}
	return FALSE;
}
void WINAPI exitAMTKDLL()
{
	try {
		if (g_pAMTK != NULL) {
			g_pAMTK.Release();
			g_pAMTK = NULL;
		}
	}
	catch(...) {
		g_pAMTK = NULL;
	}
}
BOOL WINAPI isAMTKDLLActive()
{
	return (g_pAMTK != NULL) ? TRUE : FALSE;
}

static void WINAPI _setStringValue(CString &szText, int iValue){szText.Format(_T("%d"), iValue);}
static void WINAPI _setStringValue(CString &szText, double fValue){szText.Format(_T("%f"), fValue);}
static void WINAPI _setStringValue(CString &szText, LPCTSTR lpcszValue){szText = lpcszValue;}
#define _get_BYTE_Value(s)				(BYTE)_ttoi(s)
#define _get_short_Value(s)				(short)_ttoi(s)
#define _get_long_Value(s)				(long)_ttoi(s)
//#ifndef _ttof
//#define _get_float_Value(s)				(float)atof(s)
//#else
#define _get_float_Value(s)				(float)_tstof(s)
//#endif
#define _get_bstr_Value(s)				(_bstr_t)s

static const AMTK_TIEM l_tAMTKItems[AMTK_ITEM_ALL_NUM] = {
//{{AFX_php_c_wizard_begin(amtk.data)
	{_T("AMTKDLLShow"), 4, 2, _T("调入DLL")},
	{_T("fASCheck"), 4, 0, _T("")},
	{_T("fRSCLOSE"), 4, 0, _T("")},
	{_T("fRSOPEN"), 4, 0, _T("")},
	{_T("fRSTest"), 4, 0, _T("")},
	{_T("pEquipmentCalibration"), 4, 0, _T("")},
	{_T("pEquipmentDownload"), 4, 0, _T("")},
	{_T("pEquipmentDownloadSpecial"), 4, 0, _T("运行中下载流程")},
	{_T("pEquipmentFillTube"), 4, 0, _T("")},
	{_T("pEquipmentHome"), 4, 0, _T("")},
	{_T("pEquipmentNext"), 4, 0, _T("结束自动完成进样后的等待, 执行下一流程")},
	{_T("pEquipmentPause"), 4, 0, _T("暂停自动进样器")},
	{_T("pEquipmentPCControl"), 4, 0, _T("指定PC控制")},
	{_T("pEquipmentProlong"), 4, 0, _T("")},
	{_T("pEquipmentReDo"), 4, 0, _T("启动重做指令")},
	{_T("pEquipmentStandAlone"), 4, 0, _T("指定独立运行")},
	{_T("pEquipmentStart"), 4, 0, _T("启动自动进样器")},
	{_T("pEquipmentStop"), 4, 0, _T("停止自动进样器")},
	{_T("pNewSequence"), 4, 0, _T("增加流程前, 清除上一次的流程")},
	{_T("pNewSequenceSpecial"), 4, 0, _T("")},
	{_T("pAnalysisSelected"), 3, 1, _T("等待时间方式")},
	{_T("pAnalysisTime"), 3, 4, _T("分析时间（秒）")},
	{_T("pAutoConnect"), 3, 2, _T("自动联机")},
	{_T("pAutoNeedleDown"), 3, 1, _T("样品处进样针下降深度自动选择")},
	{_T("pComm"), 3, 1, _T("通讯方式")},
	{_T("pConnection"), 1, 2, _T("连接状态")},
	{_T("pControlMode"), 2, 2, _T("")},
	{_T("pCurrentOperation"), 1, 2, _T("系统正在执行的操作")},
	{_T("pDilutionVolume"), 3, 8, _T("配比量")},
	{_T("pDispenseSpeed"), 3, 1, _T("样品排出速度")},
	{_T("pDoNext"), 3, 2, _T("")},
	{_T("pDwellTime"), 3, 2, _T("进样针在进样口中停留时间（秒）")},
	{_T("pEndStatus"), 1, 2, _T("")},
	{_T("pEquipment"), 2, 2, _T("仪器种类")},
	{_T("pFillTubeCount"), 3, 2, _T("填充管路次数")},
	{_T("pFlushVolume"), 3, 8, _T("管路冲洗量")},
	{_T("pFormShow"), 3, 2, _T("打开自动进样器界面")},
	{_T("pFormStart"), 1, 2, _T("")},
	{_T("pFormStatus"), 3, 2, _T("")},
	{_T("pFullLoop"), 3, 1, _T("满环进样选择")},
	{_T("pFunctionCMD"), 2, 2, _T("")},
	{_T("pGapEnd"), 3, 2, _T("样品末端空气隔绝选择")},
	{_T("pGapEndVolume"), 3, 8, _T("样品末端空气隔绝量")},
	{_T("pGapHead"), 3, 2, _T("样品前端空气隔绝选择")},
	{_T("pGapHeadVolume"), 3, 8, _T("样品前端空气隔绝量")},
	{_T("pInjector"), 3, 8, _T("进样口深度；进样针在进样口下降深度")},
	{_T("pInjectorWashSelected"), 3, 2, _T("进样口清洗选择")},
	{_T("pInjectorWashVolume"), 3, 8, _T("进样口清洗量")},
	{_T("pLanguage"), 2, 2, _T("")},
	{_T("pMissVial"), 3, 1, _T("")},
	{_T("pMixCount"), 3, 2, _T("配比混合次数")},
	{_T("pModfier1"), 2, 8, _T("")},
	{_T("pModfier2"), 2, 8, _T("")},
	{_T("pModfier3"), 2, 8, _T("")},
	{_T("pModfier4"), 2, 8, _T("")},
	{_T("pModfier5"), 2, 8, _T("")},
	{_T("pModifier1"), 1, 8, _T("")},
	{_T("pModifier2"), 1, 8, _T("")},
	{_T("pModifier3"), 1, 8, _T("")},
	{_T("pModifier4"), 1, 8, _T("")},
	{_T("pModifier5"), 1, 8, _T("")},
	{_T("pModifierIndex"), 3, 1, _T("基体改进剂选择")},
	{_T("pNeedleDownSpeed"), 3, 8, _T("进样针下将速度")},
	{_T("pNeedleUpSpeed"), 3, 8, _T("进样针上升速度")},
	{_T("pOEMName"), 2, 2, _T("厂家代码")},
	{_T("pOperation"), 3, 1, _T("自动进样工作方式")},
	{_T("pRSPort"), 3, 1, _T("RS232通讯口号")},
	{_T("pRunStatus"), 1, 2, _T("DLL运行状态返回值")},
	{_T("pSampleDown"), 3, 8, _T("进样处针下降深度（mm）")},
	{_T("pSeqActiveUpdate"), 1, 0, _T("当前指针流程执行状态")},
	{_T("pSeqAnalysisMethodUpdate"), 1, 0, _T("当前指针流程样品分析方法")},
	{_T("pSeqCommentUpdate"), 1, 0, _T("当前指针流程备注")},
	{_T("pSeqConcentrationUpdate"), 1, 0, _T("当前指针流程样品浓度")},
	{_T("pSeqCount"), 1, 2, _T("流程总数")},
	{_T("pSeqIndex"), 2, 2, _T("流程更新当前指针")},
	{_T("pSeqIndexBack"), 1, 2, _T("流程更新当前指针反还")},
	{_T("pSeqInjectModeData"), 0, 2, _T("")},
	{_T("pSeqInjectModeIndex"), 2, 2, _T("")},
	{_T("pSeqInjectModeTime"), 0, 2, _T("")},
	{_T("pSeqMethodUpdate"), 1, 0, _T("当前指针流程进样方法")},
	{_T("pSeqNameUpdate"), 1, 0, _T("当前指针流程样品名称")},
	{_T("pSeqOrderUpdate"), 1, 0, _T("当前指针批处理进样顺序")},
	{_T("pSeqRepeatUpdate"), 1, 0, _T("当前指针流程重复次数")},
	{_T("pSeqSampleUpdate"), 1, 0, _T("当前指针流程样品位置")},
	{_T("pSeqTargetUpdate"), 1, 0, _T("当前指针流程目标位置")},
	{_T("pSequenceProgressIndex"), 3, 2, _T("要执行的流程指针")},
	{_T("pSequenceProgressReDoIndex"), 2, 2, _T("要重做的流程指针")},
	{_T("pSeqUpdateStatus"), 3, 2, _T("")},
	{_T("pSeqVialEndUpdate"), 1, 0, _T("当前指针批处理结束样品位置")},
	{_T("pSeqVialStartUpdate"), 1, 0, _T("当前指针批处理起始样品位置")},
	{_T("pSeqVolumeUpdate"), 1, 8, _T("当前指针流程进样体积")},
	{_T("pService"), 2, 2, _T("")},
	{_T("pStartAfterDownload"), 2, 2, _T("")},
	{_T("pStatus"), 1, 4, _T("")},
	{_T("pSuctionSpeed"), 3, 1, _T("样品抽取速度")},
	{_T("pSyringe"), 3, 2, _T("注射泵规格")},
	{_T("pTargetDown"), 3, 8, _T("目标处针下降深度（mm）")},
	{_T("pTotalSeq"), 1, 2, _T("")},
	{_T("pTray1"), 3, 8, _T("样品深度；进样针在样品瓶下降深度")},
	{_T("pWaitPosition"), 3, 2, _T("等待位置")},
	{_T("pWash"), 3, 8, _T("清洗深度；进样针在清洗口下降深度")},
	{_T("pWashAtInjector"), 3, 2, _T("进样口清洗选择")},
	{_T("pWashRouting"), 3, 1, _T("清洗方式")},
	{_T("pWashVolume"), 3, 8, _T("LC: 清洗体积；AA,ICP: 清洗时间")},
	{_T("pZeroLossInjection"), 3, 2, _T("无损耗进样")},
	{_T("SequenceActive"), 2, 0, _T("设置流程状态")},
	{_T("SequenceAnalysisMethod"), 1, 0, _T("")},
	{_T("SequenceComments"), 1, 0, _T("")},
	{_T("SequenceMethod"), 3, 0, _T("进样方法名称")},
	{_T("SequenceName"), 1, 0, _T("")},
	{_T("SequenceOrder"), 3, 2, _T("批处理样品进样顺序")},
	{_T("SequenceRepeat"), 3, 2, _T("进样次数")},
	{_T("SequenceSample"), 3, 0, _T("样品位置")},
	{_T("SequenceTarget"), 3, 0, _T("目标位置")},
	{_T("SequenceVialEnd"), 3, 0, _T("批处理样品结束位置")},
	{_T("SequenceVialStart"), 3, 0, _T("批处理样品起始位置")},
	{_T("SequenceVolume"), 3, 8, _T("进样体积")},
//}}AFX_php_c_wizard_end(amtk.data)
};

void WINAPI addAMTKListByType(CComboBox &cList, WORD wAccessType)
{
	int i;
	cList.ResetContent();
	for (i = 0; i < AMTK_ITEM_ALL_NUM; i++) {
		if (l_tAMTKItems[i].wType & wAccessType) {
			cList.SetItemData(cList.AddString(l_tAMTKItems[i].lpcszName), i);
		}
	}
}
BOOL WINAPI checkAMTKItemAccessType(int iID, WORD wAccessType)
{
	if (iID >= 0 && iID < AMTK_ITEM_ALL_NUM) {
		if (l_tAMTKItems[iID].wType & wAccessType) return TRUE;
	}
	return FALSE;
}
LPCTSTR WINAPI getAMTKItemName(int iID)
{
	if (iID >= 0 && iID < AMTK_ITEM_ALL_NUM) {
		return l_tAMTKItems[iID].lpcszName;
	}
	return _T("");
}
LPCTSTR WINAPI getAMTKItemComment(int iID)
{
	if (iID >= 0 && iID < AMTK_ITEM_ALL_NUM) {
		return l_tAMTKItems[iID].lpcszComment;
	}
	return _T("");
}
int WINAPI getAMTKItemIDByName(LPCTSTR lpcszName)
{
	int i;
	for (i = 0; i < AMTK_ITEM_ALL_NUM; i++) {
		if (!lstrcmp(l_tAMTKItems[i].lpcszName, lpcszName)) return i;
	}
	return -1;
}
LRESULT WINAPI callAMTKMethod(int iID)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.call)
		case AMTKID_AMTKDLLShow: g_pAMTK->AMTKDLLShow(); break;
		case AMTKID_fASCheck: g_pAMTK->fASCheck(); break;
		case AMTKID_fRSCLOSE: g_pAMTK->fRSCLOSE(); break;
		case AMTKID_fRSOPEN: g_pAMTK->fRSOPEN(); break;
		case AMTKID_fRSTest: g_pAMTK->fRSTest(); break;
		case AMTKID_pEquipmentCalibration: g_pAMTK->pEquipmentCalibration(); break;
		case AMTKID_pEquipmentDownload: g_pAMTK->pEquipmentDownload(); break;
		case AMTKID_pEquipmentDownloadSpecial: g_pAMTK->pEquipmentDownloadSpecial(); break;
		case AMTKID_pEquipmentFillTube: g_pAMTK->pEquipmentFillTube(); break;
		case AMTKID_pEquipmentHome: g_pAMTK->pEquipmentHome(); break;
		case AMTKID_pEquipmentNext: g_pAMTK->pEquipmentNext(); break;
		case AMTKID_pEquipmentPause: g_pAMTK->pEquipmentPause(); break;
		case AMTKID_pEquipmentPCControl: g_pAMTK->pEquipmentPCControl(); break;
		case AMTKID_pEquipmentProlong: g_pAMTK->pEquipmentProlong(); break;
		case AMTKID_pEquipmentReDo: g_pAMTK->pEquipmentReDo(); break;
		case AMTKID_pEquipmentStandAlone: g_pAMTK->pEquipmentStandAlone(); break;
		case AMTKID_pEquipmentStart: g_pAMTK->pEquipmentStart(); break;
		case AMTKID_pEquipmentStop: g_pAMTK->pEquipmentStop(); break;
		case AMTKID_pNewSequence: g_pAMTK->pNewSequence(); break;
		case AMTKID_pNewSequenceSpecial: g_pAMTK->pNewSequenceSpecial(); break;
//}}AFX_php_c_wizard_end(amtk.call)
		}
	}
	catch(...) {
		return -1;
	}
	return 0;
}
LRESULT WINAPI getAMTKValue(int iID, CString &szText)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.get)
		case AMTKID_pAnalysisSelected: _setStringValue(szText, (int)g_pAMTK->pAnalysisSelected); break;
		case AMTKID_pAnalysisTime: _setStringValue(szText, (int)g_pAMTK->pAnalysisTime); break;
		case AMTKID_pAutoConnect: _setStringValue(szText, (int)g_pAMTK->pAutoConnect); break;
		case AMTKID_pAutoNeedleDown: _setStringValue(szText, (int)g_pAMTK->pAutoNeedleDown); break;
		case AMTKID_pComm: _setStringValue(szText, (int)g_pAMTK->pComm); break;
		case AMTKID_pConnection: _setStringValue(szText, (int)g_pAMTK->pConnection); break;
		case AMTKID_pCurrentOperation: _setStringValue(szText, (int)g_pAMTK->pCurrentOperation); break;
		case AMTKID_pDilutionVolume: _setStringValue(szText, (double)g_pAMTK->pDilutionVolume); break;
		case AMTKID_pDispenseSpeed: _setStringValue(szText, (int)g_pAMTK->pDispenseSpeed); break;
		case AMTKID_pDoNext: _setStringValue(szText, (int)g_pAMTK->pDoNext); break;
		case AMTKID_pDwellTime: _setStringValue(szText, (int)g_pAMTK->pDwellTime); break;
		case AMTKID_pEndStatus: _setStringValue(szText, (int)g_pAMTK->pEndStatus); break;
		case AMTKID_pFillTubeCount: _setStringValue(szText, (int)g_pAMTK->pFillTubeCount); break;
		case AMTKID_pFlushVolume: _setStringValue(szText, (double)g_pAMTK->pFlushVolume); break;
		case AMTKID_pFormShow: _setStringValue(szText, (int)g_pAMTK->pFormShow); break;
		case AMTKID_pFormStart: _setStringValue(szText, (int)g_pAMTK->pFormStart); break;
		case AMTKID_pFormStatus: _setStringValue(szText, (int)g_pAMTK->pFormStatus); break;
		case AMTKID_pFullLoop: _setStringValue(szText, (int)g_pAMTK->pFullLoop); break;
		case AMTKID_pGapEnd: _setStringValue(szText, (int)g_pAMTK->pGapEnd); break;
		case AMTKID_pGapEndVolume: _setStringValue(szText, (double)g_pAMTK->pGapEndVolume); break;
		case AMTKID_pGapHead: _setStringValue(szText, (int)g_pAMTK->pGapHead); break;
		case AMTKID_pGapHeadVolume: _setStringValue(szText, (double)g_pAMTK->pGapHeadVolume); break;
		case AMTKID_pInjector: _setStringValue(szText, (double)g_pAMTK->pInjector); break;
		case AMTKID_pInjectorWashSelected: _setStringValue(szText, (int)g_pAMTK->pInjectorWashSelected); break;
		case AMTKID_pInjectorWashVolume: _setStringValue(szText, (double)g_pAMTK->pInjectorWashVolume); break;
		case AMTKID_pMissVial: _setStringValue(szText, (int)g_pAMTK->pMissVial); break;
		case AMTKID_pMixCount: _setStringValue(szText, (int)g_pAMTK->pMixCount); break;
		case AMTKID_pModifier1: _setStringValue(szText, (double)g_pAMTK->pModifier1); break;
		case AMTKID_pModifier2: _setStringValue(szText, (double)g_pAMTK->pModifier2); break;
		case AMTKID_pModifier3: _setStringValue(szText, (double)g_pAMTK->pModifier3); break;
		case AMTKID_pModifier4: _setStringValue(szText, (double)g_pAMTK->pModifier4); break;
		case AMTKID_pModifier5: _setStringValue(szText, (double)g_pAMTK->pModifier5); break;
		case AMTKID_pModifierIndex: _setStringValue(szText, (int)g_pAMTK->pModifierIndex); break;
		case AMTKID_pNeedleDownSpeed: _setStringValue(szText, (double)g_pAMTK->pNeedleDownSpeed); break;
		case AMTKID_pNeedleUpSpeed: _setStringValue(szText, (double)g_pAMTK->pNeedleUpSpeed); break;
		case AMTKID_pOperation: _setStringValue(szText, (int)g_pAMTK->pOperation); break;
		case AMTKID_pRSPort: _setStringValue(szText, (int)g_pAMTK->pRSPort); break;
		case AMTKID_pRunStatus: _setStringValue(szText, (int)g_pAMTK->pRunStatus); break;
		case AMTKID_pSampleDown: _setStringValue(szText, (double)g_pAMTK->pSampleDown); break;
		case AMTKID_pSeqActiveUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqActiveUpdate); break;
		case AMTKID_pSeqAnalysisMethodUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqAnalysisMethodUpdate); break;
		case AMTKID_pSeqCommentUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqCommentUpdate); break;
		case AMTKID_pSeqConcentrationUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqConcentrationUpdate); break;
		case AMTKID_pSeqCount: _setStringValue(szText, (int)g_pAMTK->pSeqCount); break;
		case AMTKID_pSeqIndexBack: _setStringValue(szText, (int)g_pAMTK->pSeqIndexBack); break;
		case AMTKID_pSeqMethodUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqMethodUpdate); break;
		case AMTKID_pSeqNameUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqNameUpdate); break;
		case AMTKID_pSeqOrderUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqOrderUpdate); break;
		case AMTKID_pSeqRepeatUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqRepeatUpdate); break;
		case AMTKID_pSeqSampleUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqSampleUpdate); break;
		case AMTKID_pSeqTargetUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqTargetUpdate); break;
		case AMTKID_pSequenceProgressIndex: _setStringValue(szText, (int)g_pAMTK->pSequenceProgressIndex); break;
		case AMTKID_pSeqUpdateStatus: _setStringValue(szText, (int)g_pAMTK->pSeqUpdateStatus); break;
		case AMTKID_pSeqVialEndUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqVialEndUpdate); break;
		case AMTKID_pSeqVialStartUpdate: _setStringValue(szText, (LPCTSTR)g_pAMTK->pSeqVialStartUpdate); break;
		case AMTKID_pSeqVolumeUpdate: _setStringValue(szText, (double)g_pAMTK->pSeqVolumeUpdate); break;
		case AMTKID_pStatus: _setStringValue(szText, (int)g_pAMTK->pStatus.lVal); break;
		case AMTKID_pSuctionSpeed: _setStringValue(szText, (int)g_pAMTK->pSuctionSpeed); break;
		case AMTKID_pSyringe: _setStringValue(szText, (int)g_pAMTK->pSyringe); break;
		case AMTKID_pTargetDown: _setStringValue(szText, (double)g_pAMTK->pTargetDown); break;
		case AMTKID_pTotalSeq: _setStringValue(szText, (int)g_pAMTK->pTotalSeq); break;
		case AMTKID_pTray1: _setStringValue(szText, (double)g_pAMTK->pTray1); break;
		case AMTKID_pWaitPosition: _setStringValue(szText, (int)g_pAMTK->pWaitPosition); break;
		case AMTKID_pWash: _setStringValue(szText, (double)g_pAMTK->pWash); break;
		case AMTKID_pWashAtInjector: _setStringValue(szText, (int)g_pAMTK->pWashAtInjector); break;
		case AMTKID_pWashRouting: _setStringValue(szText, (int)g_pAMTK->pWashRouting); break;
		case AMTKID_pWashVolume: _setStringValue(szText, (double)g_pAMTK->pWashVolume); break;
		case AMTKID_pZeroLossInjection: _setStringValue(szText, (int)g_pAMTK->pZeroLossInjection); break;
		case AMTKID_SequenceAnalysisMethod: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceAnalysisMethod); break;
		case AMTKID_SequenceComments: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceComments); break;
		case AMTKID_SequenceMethod: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceMethod); break;
		case AMTKID_SequenceName: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceName); break;
		case AMTKID_SequenceOrder: _setStringValue(szText, (int)g_pAMTK->SequenceOrder); break;
		case AMTKID_SequenceRepeat: _setStringValue(szText, (int)g_pAMTK->SequenceRepeat); break;
		case AMTKID_SequenceSample: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceSample); break;
		case AMTKID_SequenceTarget: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceTarget); break;
		case AMTKID_SequenceVialEnd: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceVialEnd); break;
		case AMTKID_SequenceVialStart: _setStringValue(szText, (LPCTSTR)g_pAMTK->SequenceVialStart); break;
		case AMTKID_SequenceVolume: _setStringValue(szText, (double)g_pAMTK->SequenceVolume); break;
//}}AFX_php_c_wizard_end(amtk.get)
		default: szText.Empty();
		}
	}
	catch(...) {
#ifdef _DEBUG
		szText = _T("#Err!");
#else
		szText.Empty();
#endif
		return -1;
	}
	return 0;
}
LRESULT WINAPI getAMTKValueInt(int iID, int &iValue)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.get_i)
		case AMTKID_pAnalysisSelected: iValue = (int)g_pAMTK->pAnalysisSelected; break;
		case AMTKID_pAnalysisTime: iValue = (int)g_pAMTK->pAnalysisTime; break;
		case AMTKID_pAutoConnect: iValue = (int)g_pAMTK->pAutoConnect; break;
		case AMTKID_pAutoNeedleDown: iValue = (int)g_pAMTK->pAutoNeedleDown; break;
		case AMTKID_pComm: iValue = (int)g_pAMTK->pComm; break;
		case AMTKID_pConnection: iValue = (int)g_pAMTK->pConnection; break;
		case AMTKID_pCurrentOperation: iValue = (int)g_pAMTK->pCurrentOperation; break;
		case AMTKID_pDispenseSpeed: iValue = (int)g_pAMTK->pDispenseSpeed; break;
		case AMTKID_pDoNext: iValue = (int)g_pAMTK->pDoNext; break;
		case AMTKID_pDwellTime: iValue = (int)g_pAMTK->pDwellTime; break;
		case AMTKID_pEndStatus: iValue = (int)g_pAMTK->pEndStatus; break;
		case AMTKID_pFillTubeCount: iValue = (int)g_pAMTK->pFillTubeCount; break;
		case AMTKID_pFormShow: iValue = (int)g_pAMTK->pFormShow; break;
		case AMTKID_pFormStart: iValue = (int)g_pAMTK->pFormStart; break;
		case AMTKID_pFormStatus: iValue = (int)g_pAMTK->pFormStatus; break;
		case AMTKID_pFullLoop: iValue = (int)g_pAMTK->pFullLoop; break;
		case AMTKID_pGapEnd: iValue = (int)g_pAMTK->pGapEnd; break;
		case AMTKID_pGapHead: iValue = (int)g_pAMTK->pGapHead; break;
		case AMTKID_pInjectorWashSelected: iValue = (int)g_pAMTK->pInjectorWashSelected; break;
		case AMTKID_pMissVial: iValue = (int)g_pAMTK->pMissVial; break;
		case AMTKID_pMixCount: iValue = (int)g_pAMTK->pMixCount; break;
		case AMTKID_pModifierIndex: iValue = (int)g_pAMTK->pModifierIndex; break;
		case AMTKID_pOperation: iValue = (int)g_pAMTK->pOperation; break;
		case AMTKID_pRSPort: iValue = (int)g_pAMTK->pRSPort; break;
		case AMTKID_pRunStatus: iValue = (int)g_pAMTK->pRunStatus; break;
		case AMTKID_pSeqCount: iValue = (int)g_pAMTK->pSeqCount; break;
		case AMTKID_pSeqIndexBack: iValue = (int)g_pAMTK->pSeqIndexBack; break;
		case AMTKID_pSequenceProgressIndex: iValue = (int)g_pAMTK->pSequenceProgressIndex; break;
		case AMTKID_pSeqUpdateStatus: iValue = (int)g_pAMTK->pSeqUpdateStatus; break;
		case AMTKID_pStatus: iValue = (int)g_pAMTK->pStatus.lVal; break;
		case AMTKID_pSuctionSpeed: iValue = (int)g_pAMTK->pSuctionSpeed; break;
		case AMTKID_pSyringe: iValue = (int)g_pAMTK->pSyringe; break;
		case AMTKID_pTotalSeq: iValue = (int)g_pAMTK->pTotalSeq; break;
		case AMTKID_pWaitPosition: iValue = (int)g_pAMTK->pWaitPosition; break;
		case AMTKID_pWashAtInjector: iValue = (int)g_pAMTK->pWashAtInjector; break;
		case AMTKID_pWashRouting: iValue = (int)g_pAMTK->pWashRouting; break;
		case AMTKID_pZeroLossInjection: iValue = (int)g_pAMTK->pZeroLossInjection; break;
		case AMTKID_SequenceOrder: iValue = (int)g_pAMTK->SequenceOrder; break;
		case AMTKID_SequenceRepeat: iValue = (int)g_pAMTK->SequenceRepeat; break;
//}}AFX_php_c_wizard_end(amtk.get_i)
		default: iValue = 0;
		}
	}
	catch(...) {
		iValue = 0;
		return -1;
	}
	return 0;
}
LRESULT WINAPI getAMTKValueFloat(int iID, double &fValue)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.get_f)
		case AMTKID_pDilutionVolume: fValue = (double)g_pAMTK->pDilutionVolume; break;
		case AMTKID_pFlushVolume: fValue = (double)g_pAMTK->pFlushVolume; break;
		case AMTKID_pGapEndVolume: fValue = (double)g_pAMTK->pGapEndVolume; break;
		case AMTKID_pGapHeadVolume: fValue = (double)g_pAMTK->pGapHeadVolume; break;
		case AMTKID_pInjector: fValue = (double)g_pAMTK->pInjector; break;
		case AMTKID_pInjectorWashVolume: fValue = (double)g_pAMTK->pInjectorWashVolume; break;
		case AMTKID_pModifier1: fValue = (double)g_pAMTK->pModifier1; break;
		case AMTKID_pModifier2: fValue = (double)g_pAMTK->pModifier2; break;
		case AMTKID_pModifier3: fValue = (double)g_pAMTK->pModifier3; break;
		case AMTKID_pModifier4: fValue = (double)g_pAMTK->pModifier4; break;
		case AMTKID_pModifier5: fValue = (double)g_pAMTK->pModifier5; break;
		case AMTKID_pNeedleDownSpeed: fValue = (double)g_pAMTK->pNeedleDownSpeed; break;
		case AMTKID_pNeedleUpSpeed: fValue = (double)g_pAMTK->pNeedleUpSpeed; break;
		case AMTKID_pSampleDown: fValue = (double)g_pAMTK->pSampleDown; break;
		case AMTKID_pSeqVolumeUpdate: fValue = (double)g_pAMTK->pSeqVolumeUpdate; break;
		case AMTKID_pTargetDown: fValue = (double)g_pAMTK->pTargetDown; break;
		case AMTKID_pTray1: fValue = (double)g_pAMTK->pTray1; break;
		case AMTKID_pWash: fValue = (double)g_pAMTK->pWash; break;
		case AMTKID_pWashVolume: fValue = (double)g_pAMTK->pWashVolume; break;
		case AMTKID_SequenceVolume: fValue = (double)g_pAMTK->SequenceVolume; break;
//}}AFX_php_c_wizard_end(amtk.get_f)
		default: fValue = 0;
		}
	}
	catch(...) {
		fValue = 0;
		return -1;
	}
	return 0;

}
LRESULT WINAPI setAMTKValue(int iID, const CString &szText)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.set)
		case AMTKID_pAnalysisSelected: g_pAMTK->pAnalysisSelected = _get_BYTE_Value(szText); break;
		case AMTKID_pAnalysisTime: g_pAMTK->pAnalysisTime = _get_long_Value(szText); break;
		case AMTKID_pAutoConnect: g_pAMTK->pAutoConnect = _get_short_Value(szText); break;
		case AMTKID_pAutoNeedleDown: g_pAMTK->pAutoNeedleDown = _get_BYTE_Value(szText); break;
		case AMTKID_pComm: g_pAMTK->pComm = _get_BYTE_Value(szText); break;
		case AMTKID_pControlMode: g_pAMTK->pControlMode = _get_short_Value(szText); break;
		case AMTKID_pDilutionVolume: g_pAMTK->pDilutionVolume = _get_float_Value(szText); break;
		case AMTKID_pDispenseSpeed: g_pAMTK->pDispenseSpeed = _get_BYTE_Value(szText); break;
		case AMTKID_pDoNext: g_pAMTK->pDoNext = _get_short_Value(szText); break;
		case AMTKID_pDwellTime: g_pAMTK->pDwellTime = _get_short_Value(szText); break;
		case AMTKID_pEquipment: g_pAMTK->pEquipment = _get_short_Value(szText); break;
		case AMTKID_pFillTubeCount: g_pAMTK->pFillTubeCount = _get_short_Value(szText); break;
		case AMTKID_pFlushVolume: g_pAMTK->pFlushVolume = _get_float_Value(szText); break;
		case AMTKID_pFormShow: g_pAMTK->pFormShow = _get_short_Value(szText); break;
		case AMTKID_pFormStatus: g_pAMTK->pFormStatus = _get_short_Value(szText); break;
		case AMTKID_pFullLoop: g_pAMTK->pFullLoop = _get_BYTE_Value(szText); break;
		case AMTKID_pFunctionCMD: g_pAMTK->pFunctionCMD = _get_short_Value(szText); break;
		case AMTKID_pGapEnd: g_pAMTK->pGapEnd = _get_short_Value(szText); break;
		case AMTKID_pGapEndVolume: g_pAMTK->pGapEndVolume = _get_float_Value(szText); break;
		case AMTKID_pGapHead: g_pAMTK->pGapHead = _get_short_Value(szText); break;
		case AMTKID_pGapHeadVolume: g_pAMTK->pGapHeadVolume = _get_float_Value(szText); break;
		case AMTKID_pInjector: g_pAMTK->pInjector = _get_float_Value(szText); break;
		case AMTKID_pInjectorWashSelected: g_pAMTK->pInjectorWashSelected = _get_short_Value(szText); break;
		case AMTKID_pInjectorWashVolume: g_pAMTK->pInjectorWashVolume = _get_float_Value(szText); break;
		case AMTKID_pLanguage: g_pAMTK->pLanguage = _get_short_Value(szText); break;
		case AMTKID_pMissVial: g_pAMTK->pMissVial = _get_BYTE_Value(szText); break;
		case AMTKID_pMixCount: g_pAMTK->pMixCount = _get_short_Value(szText); break;
		case AMTKID_pModfier1: g_pAMTK->pModfier1 = _get_float_Value(szText); break;
		case AMTKID_pModfier2: g_pAMTK->pModfier2 = _get_float_Value(szText); break;
		case AMTKID_pModfier3: g_pAMTK->pModfier3 = _get_float_Value(szText); break;
		case AMTKID_pModfier4: g_pAMTK->pModfier4 = _get_float_Value(szText); break;
		case AMTKID_pModfier5: g_pAMTK->pModfier5 = _get_float_Value(szText); break;
		case AMTKID_pModifierIndex: g_pAMTK->pModifierIndex = _get_BYTE_Value(szText); break;
		case AMTKID_pNeedleDownSpeed: g_pAMTK->pNeedleDownSpeed = _get_float_Value(szText); break;
		case AMTKID_pNeedleUpSpeed: g_pAMTK->pNeedleUpSpeed = _get_float_Value(szText); break;
		case AMTKID_pOEMName: g_pAMTK->pOEMName = _get_short_Value(szText); break;
		case AMTKID_pOperation: g_pAMTK->pOperation = _get_BYTE_Value(szText); break;
		case AMTKID_pRSPort: g_pAMTK->pRSPort = _get_BYTE_Value(szText); break;
		case AMTKID_pSampleDown: g_pAMTK->pSampleDown = _get_float_Value(szText); break;
		case AMTKID_pSeqIndex: g_pAMTK->pSeqIndex = _get_short_Value(szText); break;
		case AMTKID_pSeqInjectModeIndex: g_pAMTK->pSeqInjectModeIndex = _get_short_Value(szText); break;
		case AMTKID_pSequenceProgressIndex: g_pAMTK->pSequenceProgressIndex = _get_short_Value(szText); break;
		case AMTKID_pSequenceProgressReDoIndex: g_pAMTK->pSequenceProgressReDoIndex = _get_short_Value(szText); break;
		case AMTKID_pSeqUpdateStatus: g_pAMTK->pSeqUpdateStatus = _get_short_Value(szText); break;
		case AMTKID_pService: g_pAMTK->pService = _get_short_Value(szText); break;
		case AMTKID_pStartAfterDownload: g_pAMTK->pStartAfterDownload = _get_short_Value(szText); break;
		case AMTKID_pSuctionSpeed: g_pAMTK->pSuctionSpeed = _get_BYTE_Value(szText); break;
		case AMTKID_pSyringe: g_pAMTK->pSyringe = _get_short_Value(szText); break;
		case AMTKID_pTargetDown: g_pAMTK->pTargetDown = _get_float_Value(szText); break;
		case AMTKID_pTray1: g_pAMTK->pTray1 = _get_float_Value(szText); break;
		case AMTKID_pWaitPosition: g_pAMTK->pWaitPosition = _get_short_Value(szText); break;
		case AMTKID_pWash: g_pAMTK->pWash = _get_float_Value(szText); break;
		case AMTKID_pWashAtInjector: g_pAMTK->pWashAtInjector = _get_short_Value(szText); break;
		case AMTKID_pWashRouting: g_pAMTK->pWashRouting = _get_BYTE_Value(szText); break;
		case AMTKID_pWashVolume: g_pAMTK->pWashVolume = _get_float_Value(szText); break;
		case AMTKID_pZeroLossInjection: g_pAMTK->pZeroLossInjection = _get_short_Value(szText); break;
		case AMTKID_SequenceActive: g_pAMTK->SequenceActive = _get_bstr_Value(szText); break;
		case AMTKID_SequenceMethod: g_pAMTK->SequenceMethod = _get_bstr_Value(szText); break;
		case AMTKID_SequenceOrder: g_pAMTK->SequenceOrder = _get_short_Value(szText); break;
		case AMTKID_SequenceRepeat: g_pAMTK->SequenceRepeat = _get_short_Value(szText); break;
		case AMTKID_SequenceSample: g_pAMTK->SequenceSample = _get_bstr_Value(szText); break;
		case AMTKID_SequenceTarget: g_pAMTK->SequenceTarget = _get_bstr_Value(szText); break;
		case AMTKID_SequenceVialEnd: g_pAMTK->SequenceVialEnd = _get_bstr_Value(szText); break;
		case AMTKID_SequenceVialStart: g_pAMTK->SequenceVialStart = _get_bstr_Value(szText); break;
		case AMTKID_SequenceVolume: g_pAMTK->SequenceVolume = _get_float_Value(szText); break;
//}}AFX_php_c_wizard_end(amtk.set)
		}
	}
	catch(...) {
		return -1;
	}
	return 0;
}
LRESULT WINAPI setAMTKValueInt(int iID, int iValue)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.set_i)
		case AMTKID_pAnalysisSelected: g_pAMTK->pAnalysisSelected = (BYTE)iValue; break;
		case AMTKID_pAnalysisTime: g_pAMTK->pAnalysisTime = (long)iValue; break;
		case AMTKID_pAutoConnect: g_pAMTK->pAutoConnect = (short)iValue; break;
		case AMTKID_pAutoNeedleDown: g_pAMTK->pAutoNeedleDown = (BYTE)iValue; break;
		case AMTKID_pComm: g_pAMTK->pComm = (BYTE)iValue; break;
		case AMTKID_pControlMode: g_pAMTK->pControlMode = (short)iValue; break;
		case AMTKID_pDispenseSpeed: g_pAMTK->pDispenseSpeed = (BYTE)iValue; break;
		case AMTKID_pDoNext: g_pAMTK->pDoNext = (short)iValue; break;
		case AMTKID_pDwellTime: g_pAMTK->pDwellTime = (short)iValue; break;
		case AMTKID_pEquipment: g_pAMTK->pEquipment = (short)iValue; break;
		case AMTKID_pFillTubeCount: g_pAMTK->pFillTubeCount = (short)iValue; break;
		case AMTKID_pFormShow: g_pAMTK->pFormShow = (short)iValue; break;
		case AMTKID_pFormStatus: g_pAMTK->pFormStatus = (short)iValue; break;
		case AMTKID_pFullLoop: g_pAMTK->pFullLoop = (BYTE)iValue; break;
		case AMTKID_pFunctionCMD: g_pAMTK->pFunctionCMD = (short)iValue; break;
		case AMTKID_pGapEnd: g_pAMTK->pGapEnd = (short)iValue; break;
		case AMTKID_pGapHead: g_pAMTK->pGapHead = (short)iValue; break;
		case AMTKID_pInjectorWashSelected: g_pAMTK->pInjectorWashSelected = (short)iValue; break;
		case AMTKID_pLanguage: g_pAMTK->pLanguage = (short)iValue; break;
		case AMTKID_pMissVial: g_pAMTK->pMissVial = (BYTE)iValue; break;
		case AMTKID_pMixCount: g_pAMTK->pMixCount = (short)iValue; break;
		case AMTKID_pModifierIndex: g_pAMTK->pModifierIndex = (BYTE)iValue; break;
		case AMTKID_pOEMName: g_pAMTK->pOEMName = (short)iValue; break;
		case AMTKID_pOperation: g_pAMTK->pOperation = (BYTE)iValue; break;
		case AMTKID_pRSPort: g_pAMTK->pRSPort = (BYTE)iValue; break;
		case AMTKID_pSeqIndex: g_pAMTK->pSeqIndex = (short)iValue; break;
		case AMTKID_pSeqInjectModeIndex: g_pAMTK->pSeqInjectModeIndex = (short)iValue; break;
		case AMTKID_pSequenceProgressIndex: g_pAMTK->pSequenceProgressIndex = (short)iValue; break;
		case AMTKID_pSequenceProgressReDoIndex: g_pAMTK->pSequenceProgressReDoIndex = (short)iValue; break;
		case AMTKID_pSeqUpdateStatus: g_pAMTK->pSeqUpdateStatus = (short)iValue; break;
		case AMTKID_pService: g_pAMTK->pService = (short)iValue; break;
		case AMTKID_pStartAfterDownload: g_pAMTK->pStartAfterDownload = (short)iValue; break;
		case AMTKID_pSuctionSpeed: g_pAMTK->pSuctionSpeed = (BYTE)iValue; break;
		case AMTKID_pSyringe: g_pAMTK->pSyringe = (short)iValue; break;
		case AMTKID_pWaitPosition: g_pAMTK->pWaitPosition = (short)iValue; break;
		case AMTKID_pWashAtInjector: g_pAMTK->pWashAtInjector = (short)iValue; break;
		case AMTKID_pWashRouting: g_pAMTK->pWashRouting = (BYTE)iValue; break;
		case AMTKID_pZeroLossInjection: g_pAMTK->pZeroLossInjection = (short)iValue; break;
		case AMTKID_SequenceOrder: g_pAMTK->SequenceOrder = (short)iValue; break;
		case AMTKID_SequenceRepeat: g_pAMTK->SequenceRepeat = (short)iValue; break;
//}}AFX_php_c_wizard_end(amtk.set_i)
		}
	}
	catch(...) {
		return -1;
	}
	return 0;
}
LRESULT WINAPI setAMTKValueFloat(int iID, double fValue)
{
	try {
		switch(iID) {
//{{AFX_php_c_wizard_begin(amtk.set_f)
		case AMTKID_pDilutionVolume: g_pAMTK->pDilutionVolume = (float)fValue; break;
		case AMTKID_pFlushVolume: g_pAMTK->pFlushVolume = (float)fValue; break;
		case AMTKID_pGapEndVolume: g_pAMTK->pGapEndVolume = (float)fValue; break;
		case AMTKID_pGapHeadVolume: g_pAMTK->pGapHeadVolume = (float)fValue; break;
		case AMTKID_pInjector: g_pAMTK->pInjector = (float)fValue; break;
		case AMTKID_pInjectorWashVolume: g_pAMTK->pInjectorWashVolume = (float)fValue; break;
		case AMTKID_pModfier1: g_pAMTK->pModfier1 = (float)fValue; break;
		case AMTKID_pModfier2: g_pAMTK->pModfier2 = (float)fValue; break;
		case AMTKID_pModfier3: g_pAMTK->pModfier3 = (float)fValue; break;
		case AMTKID_pModfier4: g_pAMTK->pModfier4 = (float)fValue; break;
		case AMTKID_pModfier5: g_pAMTK->pModfier5 = (float)fValue; break;
		case AMTKID_pNeedleDownSpeed: g_pAMTK->pNeedleDownSpeed = (float)fValue; break;
		case AMTKID_pNeedleUpSpeed: g_pAMTK->pNeedleUpSpeed = (float)fValue; break;
		case AMTKID_pSampleDown: g_pAMTK->pSampleDown = (float)fValue; break;
		case AMTKID_pTargetDown: g_pAMTK->pTargetDown = (float)fValue; break;
		case AMTKID_pTray1: g_pAMTK->pTray1 = (float)fValue; break;
		case AMTKID_pWash: g_pAMTK->pWash = (float)fValue; break;
		case AMTKID_pWashVolume: g_pAMTK->pWashVolume = (float)fValue; break;
		case AMTKID_SequenceVolume: g_pAMTK->SequenceVolume = (float)fValue; break;
//}}AFX_php_c_wizard_end(amtk.set_f)
		}
	}
	catch(...) {
		return -1;
	}
	return 0;
}
