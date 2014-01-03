
#ifndef _TWEN_STEPS_H
#define _TWEN_STEPS_H

#include <afxcoll.h>

class CSteps
{
public:
	enum enumState
	{
		WAITING,
		RUNNING,
		FINISHED,
	};

public:
	int AppendStep(const CString& description, enumState state = WAITING);
	void ModifyStepState(int step, enumState state);
	CString GenerateFullDescriptions();

private:
	CUIntArray states;
	CStringArray descriptions;
};

#endif//_TWEN_STEPS_H

