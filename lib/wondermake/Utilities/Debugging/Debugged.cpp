#include "Debugged.h"

std::vector<Debugged::SDebugData> Debugged::ourDebugged;

Debugged::Debugged(std::string aName, AnyExecutor aExecutor)
{
	ourDebugged.emplace_back(
		SDebugData
		{
			.Name				= std::move(aName),
			.Executor			= std::move(aExecutor),
			.TickFunc			= [this]() { Debug(); },
			.SetSubscriberFunc	= [this](EventSubscriber aSub) { myTickSubscriber = std::move(aSub); }
		});
}

std::vector<Debugged::SDebugData> Debugged::GetAndResetDebugged()
{
	return std::exchange(ourDebugged, decltype(ourDebugged)());
}