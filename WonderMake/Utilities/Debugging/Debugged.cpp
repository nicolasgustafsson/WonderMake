#include "pch.h"
#include "Debugged.h"
#include "Debugging/DebugSettingsSystem.h"


Debugged::Debugged(const std::string aName) noexcept
	: myDebugSubscriber(ERoutineId::Debug, 
		BindHelper(&Debugged::OnDebugMessage, this))
{
	DebugName = "Debug Windows/" + aName;
}

void Debugged::OnDebugMessage(const SDebugMessage&)
{
	if (!SystemPtr<DebugSettingsSystem>()->GetOrCreateDebugValue<bool>(DebugName, false))
		return;

	Debug();
}
