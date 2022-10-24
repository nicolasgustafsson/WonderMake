#include "pch.h"
#include "Debugged.h"

std::vector<std::pair<std::string, std::function<void()>>> Debugged::ourDebugged;

Debugged::Debugged(std::string aName) noexcept
	: myDebugName(std::move(aName))
{
	ourDebugged.emplace_back(std::make_pair(myDebugName, [this]() { Debug(); }));
}

std::vector<std::pair<std::string, std::function<void()>>> Debugged::GetAndResetDebugged()
{
	return std::exchange(ourDebugged, decltype(ourDebugged)());
}