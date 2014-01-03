
#include "stdafx.h"
#include "Steps.h"


int CSteps::AppendStep(const CString& description, enumState state)
{
	states.Add(state);
	descriptions.Add(description);
	return states.GetUpperBound();
}
void CSteps::ModifyStepState(int step, enumState state)
{
	if(states.GetUpperBound() < step) return;
	states.SetAt(step, state);
}
CString GenerateDescription(CSteps::enumState state, const CString& description)
{
	switch(state)
	{
	case CSteps::WAITING:return _T("");
	case CSteps::RUNNING:return description + _T("...");
	case CSteps::FINISHED:return description + _T("Íê³É!");
	default:return _T("");
	}
}
CString CSteps::GenerateFullDescriptions()
{
	CString fullDescriptions = _T("");
	for(int i=0; i<=states.GetUpperBound(); ++i)
	{
		if(!fullDescriptions.IsEmpty()) fullDescriptions += _T("\r\n");
		fullDescriptions += GenerateDescription((enumState)states.GetAt(i), descriptions.GetAt(i));
	}
	return fullDescriptions;
}