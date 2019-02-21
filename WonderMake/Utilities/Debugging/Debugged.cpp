#include "stdafx.h"
#include "Debugged.h"


Debugged::Debugged()
	: myDebugSubscriber(ERoutineId::Debug, 
		BindHelper(&Debugged::OnDebugMessage, this))
{
}

void Debugged::OnDebugMessage(const SDebugMessage&)
{
	Debug();
}
