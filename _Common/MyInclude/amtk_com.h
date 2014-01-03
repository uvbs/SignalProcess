#pragma once

#pragma pack(push, 2)
typedef struct tagAMTKItem {
	LPCTSTR	lpcszName;
	WORD	wType;
	WORD	wPara;
	LPCTSTR	lpcszComment;
}	AMTK_TIEM, *P_AMTK_TIEM;
typedef const struct tagAMTKItem *PC_AMTK_TIEM;
#pragma pack(pop)

//{{AFX_php_c_wizard_begin(amtk.head)
#define AMTKID_AMTKDLLShow	0
#define AMTKID_fASCheck	1
#define AMTKID_fRSCLOSE	2
#define AMTKID_fRSOPEN	3
#define AMTKID_fRSTest	4
#define AMTKID_pEquipmentCalibration	5
#define AMTKID_pEquipmentDownload	6
#define AMTKID_pEquipmentDownloadSpecial	7
#define AMTKID_pEquipmentFillTube	8
#define AMTKID_pEquipmentHome	9
#define AMTKID_pEquipmentNext	10
#define AMTKID_pEquipmentPause	11
#define AMTKID_pEquipmentPCControl	12
#define AMTKID_pEquipmentProlong	13
#define AMTKID_pEquipmentReDo	14
#define AMTKID_pEquipmentStandAlone	15
#define AMTKID_pEquipmentStart	16
#define AMTKID_pEquipmentStop	17
#define AMTKID_pNewSequence	18
#define AMTKID_pNewSequenceSpecial	19
#define AMTKID_pAnalysisSelected	20
#define AMTKID_pAnalysisTime	21
#define AMTKID_pAutoConnect	22
#define AMTKID_pAutoNeedleDown	23
#define AMTKID_pComm	24
#define AMTKID_pConnection	25
#define AMTKID_pControlMode	26
#define AMTKID_pCurrentOperation	27
#define AMTKID_pDilutionVolume	28
#define AMTKID_pDispenseSpeed	29
#define AMTKID_pDoNext	30
#define AMTKID_pDwellTime	31
#define AMTKID_pEndStatus	32
#define AMTKID_pEquipment	33
#define AMTKID_pFillTubeCount	34
#define AMTKID_pFlushVolume	35
#define AMTKID_pFormShow	36
#define AMTKID_pFormStart	37
#define AMTKID_pFormStatus	38
#define AMTKID_pFullLoop	39
#define AMTKID_pFunctionCMD	40
#define AMTKID_pGapEnd	41
#define AMTKID_pGapEndVolume	42
#define AMTKID_pGapHead	43
#define AMTKID_pGapHeadVolume	44
#define AMTKID_pInjector	45
#define AMTKID_pInjectorWashSelected	46
#define AMTKID_pInjectorWashVolume	47
#define AMTKID_pLanguage	48
#define AMTKID_pMissVial	49
#define AMTKID_pMixCount	50
#define AMTKID_pModfier1	51
#define AMTKID_pModfier2	52
#define AMTKID_pModfier3	53
#define AMTKID_pModfier4	54
#define AMTKID_pModfier5	55
#define AMTKID_pModifier1	56
#define AMTKID_pModifier2	57
#define AMTKID_pModifier3	58
#define AMTKID_pModifier4	59
#define AMTKID_pModifier5	60
#define AMTKID_pModifierIndex	61
#define AMTKID_pNeedleDownSpeed	62
#define AMTKID_pNeedleUpSpeed	63
#define AMTKID_pOEMName	64
#define AMTKID_pOperation	65
#define AMTKID_pRSPort	66
#define AMTKID_pRunStatus	67
#define AMTKID_pSampleDown	68
#define AMTKID_pSeqActiveUpdate	69
#define AMTKID_pSeqAnalysisMethodUpdate	70
#define AMTKID_pSeqCommentUpdate	71
#define AMTKID_pSeqConcentrationUpdate	72
#define AMTKID_pSeqCount	73
#define AMTKID_pSeqIndex	74
#define AMTKID_pSeqIndexBack	75
#define AMTKID_pSeqInjectModeData	76
#define AMTKID_pSeqInjectModeIndex	77
#define AMTKID_pSeqInjectModeTime	78
#define AMTKID_pSeqMethodUpdate	79
#define AMTKID_pSeqNameUpdate	80
#define AMTKID_pSeqOrderUpdate	81
#define AMTKID_pSeqRepeatUpdate	82
#define AMTKID_pSeqSampleUpdate	83
#define AMTKID_pSeqTargetUpdate	84
#define AMTKID_pSequenceProgressIndex	85
#define AMTKID_pSequenceProgressReDoIndex	86
#define AMTKID_pSeqUpdateStatus	87
#define AMTKID_pSeqVialEndUpdate	88
#define AMTKID_pSeqVialStartUpdate	89
#define AMTKID_pSeqVolumeUpdate	90
#define AMTKID_pService	91
#define AMTKID_pStartAfterDownload	92
#define AMTKID_pStatus	93
#define AMTKID_pSuctionSpeed	94
#define AMTKID_pSyringe	95
#define AMTKID_pTargetDown	96
#define AMTKID_pTotalSeq	97
#define AMTKID_pTray1	98
#define AMTKID_pWaitPosition	99
#define AMTKID_pWash	100
#define AMTKID_pWashAtInjector	101
#define AMTKID_pWashRouting	102
#define AMTKID_pWashVolume	103
#define AMTKID_pZeroLossInjection	104
#define AMTKID_SequenceActive	105
#define AMTKID_SequenceAnalysisMethod	106
#define AMTKID_SequenceComments	107
#define AMTKID_SequenceMethod	108
#define AMTKID_SequenceName	109
#define AMTKID_SequenceOrder	110
#define AMTKID_SequenceRepeat	111
#define AMTKID_SequenceSample	112
#define AMTKID_SequenceTarget	113
#define AMTKID_SequenceVialEnd	114
#define AMTKID_SequenceVialStart	115
#define AMTKID_SequenceVolume	116
#define AMTK_ITEM_ALL_NUM	117
#define AMTK_ITEM_CALL_NUM	20
#define AMTK_ITEM_GET_NUM	79
#define AMTK_ITEM_SET_NUM	65
//}}AFX_php_c_wizard_end(amtk.head)

extern BOOL WINAPI initAMTKDLL();
extern void WINAPI exitAMTKDLL();
extern BOOL WINAPI isAMTKDLLActive();

#define AMTK_ACCESS_TYPE_READ			1
#define AMTK_ACCESS_TYPE_WRITE			2
#define AMTK_ACCESS_TYPE_CALL			4

extern void WINAPI addAMTKListByType(CComboBox &cList, WORD wAccessType);
extern BOOL WINAPI checkAMTKItemAccessType(int iID, WORD wAccessType);
#define isAMTKItemReadable(iID)			checkAMTKItemAccessType(iID,AMTK_ACCESS_TYPE_READ)
#define isAMTKItemWritable(iID)			checkAMTKItemAccessType(iID,AMTK_ACCESS_TYPE_WRITE)
#define isAMTKItemCallable(iID)			checkAMTKItemAccessType(iID,AMTK_ACCESS_TYPE_CALL)

extern LPCTSTR WINAPI getAMTKItemName(int iID);
extern LPCTSTR WINAPI getAMTKItemComment(int iID);
extern int WINAPI getAMTKItemIDByName(LPCTSTR lpcszName);

extern LRESULT WINAPI callAMTKMethod(int iID);

extern LRESULT WINAPI getAMTKValue(int iID, CString &szText);
#define getAMTKValueString				getAMTKValue
extern LRESULT WINAPI getAMTKValueInt(int iID, int &iValue);
extern LRESULT WINAPI getAMTKValueFloat(int iID, double &fValue);

extern LRESULT WINAPI setAMTKValue(int iID, const CString &szText);
#define setAMTKValueString				setAMTKValue
extern LRESULT WINAPI setAMTKValueInt(int iID, int iValue);
extern LRESULT WINAPI setAMTKValueFloat(int iID, double fValue);
