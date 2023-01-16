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
			STPlatform,
			STSingleton,
			STThreadsafe,
			STWonderMake>>
	, public WinEventSystem
	, public std::enable_shared_from_this<WinEventSystemImpl>
{
public:
	WinEventSystemImpl() noexcept;
	~WinEventSystemImpl() noexcept;

	Future<void> RegisterEvent(
		WinEventHandle aEventHandle) override;

	void WaitForEvent(
		DWORD aTimeoutMs) override;

private:
	struct HandleData
	{
		WinEventHandle	Handle;
		Promise<void>	Promise;
	};

	void TriggerWaitInterruption();
	void UnregisterEvent(
		HANDLE aEventHandle);

	std::mutex myMutex;
	HANDLE myInterruptEvent = INVALID_HANDLE_VALUE;

	std::vector<HandleData> myHandleData;
	std::vector<HANDLE> myHandles;

};
