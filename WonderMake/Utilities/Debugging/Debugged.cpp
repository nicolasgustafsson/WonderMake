#include "pch.h"
#include "Debugged.h"


Debugged::Debugged() noexcept
	: myDebugSubscriber(ERoutineId::Debug, 
		BindHelper(&Debugged::OnDebugMessage, this))
{
}

void Debugged::OnDebugMessage(const SDebugMessage&)
{
	Debug();
}
