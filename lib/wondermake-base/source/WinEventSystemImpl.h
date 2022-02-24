#pragma once

#include "wondermake-base/WinEventSystem.h"

#include <array>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class WinPlatformSystem;

class WinEventSystemImpl
	: public SystemSub<
		Policy::Set<
			PAdd<WinPlatformSystem, PWrite>>,
		STrait::Set<
			STFoundational,
			STSingleton,
			STThreadsafe>>
	, public WinEventSystem
{
public:
	WinEventSystemImpl() noexcept;
	~WinEventSystemImpl() noexcept;

	void RegisterEvent(
		HANDLE aEventHandle,
		Closure&& aCallback) override;
	void UnregisterEvent(
		HANDLE aEventHandle) override;

	void WaitForEvent(
		DWORD aTimeoutMs) override;

private:
	struct HandleData
	{
		HANDLE Handle;
		Closure Callback;
	};

	void TriggerWaitInterruption();

	std::mutex myMutex;
	HANDLE myInterruptEvent = INVALID_HANDLE_VALUE;

	std::vector<HandleData> myHandleData;
	std::vector<HANDLE> myHandles;

};
