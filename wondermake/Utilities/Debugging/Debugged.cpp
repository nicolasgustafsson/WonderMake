#include "pch.h"
#include "Debugged.h"
#include "Debugging/DebugSettingsSystem.h"


Debugged::Debugged(const std::string aName) noexcept
	: myDebugSubscriber(BindHelper(&Debugged::OnDebugMessage, this))
{
	myDebugName = "Debug Windows/" + aName;
}

void Debugged::OnDebugMessage(const SDebugMessage&)
{
	if (!SystemPtr<DebugSettingsSystem>()->GetOrCreateDebugValue<bool>(myDebugName, false))
		return;

	Debug();
}
