#pragma once

#include "wondermake-base/System.h"
#include "wondermake-base/WinEventHandle.h"

#include "wondermake-utility/UniqueFunction.h"
#include "wondermake-utility/WinPlatform.h"
#include "wondermake-utility/Future.h"

class WinEventSystem
	: public SystemAbstracted
{
public:
	virtual Future<void> RegisterEvent(
		WinEventHandle aEventHandle) = 0;

	virtual void WaitForEvent(
		DWORD aTimeoutMs) = 0;

protected:
	WinEventSystem() noexcept = default;

};
