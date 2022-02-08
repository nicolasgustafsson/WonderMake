#pragma once

#include "wondermake-utility/UniqueFunction.h"
#include "wondermake-utility/WinPlatform.h"

#include "WonderMakeBase/System.h"

class WinEventSystem
	: public SystemAbstracted
{
public:
	virtual void RegisterEvent(
		HANDLE aEventHandle,
		Closure&& aCallback) = 0;
	virtual void UnregisterEvent(
		HANDLE aEventHandle) = 0;

	virtual void WaitForEvent(
		DWORD aTimeoutMs) = 0;

protected:
	WinEventSystem() noexcept = default;

};
