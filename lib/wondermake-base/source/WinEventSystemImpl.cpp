#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/Utility.h"

#include "WinEventSystemImpl.h"

#include <algorithm>

REGISTER_SYSTEM_MASKED(WinEventSystemImpl, WinEventSystem);

WinEventSystemImpl::WinEventSystemImpl() noexcept
{
	myHandles.reserve(MAXIMUM_WAIT_OBJECTS);

	myInterruptEvent = Get<WinPlatformSystem>().CreateEventW(NULL, TRUE, FALSE, NULL);
}

WinEventSystemImpl::~WinEventSystemImpl() noexcept
{
	TriggerWaitInterruption();
}

Future<void> WinEventSystemImpl::RegisterEvent(
	HANDLE aEventHandle)
{
	if (aEventHandle == NULL)
		return MakeCanceledFuture<void>();

	auto [promise, future] = MakeAsync<void>();

	{
		std::lock_guard<std::mutex> lock(myMutex);

		myHandleData.emplace_back(HandleData{ aEventHandle, std::move(promise) });
	}

	future.OnCancel(GetExecutor(), Bind(&WinEventSystemImpl::UnregisterEvent, weak_from_this(), aEventHandle));

	TriggerWaitInterruption();

	return future;
}

void WinEventSystemImpl::WaitForEvent(
	DWORD aTimeoutMs)
{
	auto&& winPlatformSystem = Get<WinPlatformSystem>();

	myHandles.clear();

	myHandles.emplace_back(myInterruptEvent);

	winPlatformSystem.ResetEvent(myInterruptEvent);

	{
		std::lock_guard<std::mutex> lock(myMutex);

		const auto endIt = myHandleData.size() < MAXIMUM_WAIT_OBJECTS
			? myHandleData.end()
			: myHandleData.begin() + (MAXIMUM_WAIT_OBJECTS - 1);

		myHandles.resize(std::distance(myHandleData.begin(), endIt) + 1);

		std::transform(myHandleData.begin(), endIt, myHandles.begin() + 1, [](const auto& aHandleData)
			{
				return aHandleData.Handle;
			});
	}

	const DWORD result = winPlatformSystem.WaitForMultipleObjects(static_cast<DWORD>(myHandles.size()), myHandles.data(), FALSE, aTimeoutMs);

	if (result < WAIT_OBJECT_0 || result >= WAIT_OBJECT_0 + myHandles.size())
		return;

	std::vector<HANDLE> signaledEvents;
	std::vector<Promise<void>> signaledPromises;

	signaledEvents.reserve(MAXIMUM_WAIT_OBJECTS - 1);
	signaledPromises.reserve(MAXIMUM_WAIT_OBJECTS - 1);

	for (size_t i = 1; i < myHandles.size(); ++i)
		if (winPlatformSystem.WaitForSingleObject(myHandles[i], 0) == WAIT_OBJECT_0)
			signaledEvents.emplace_back(myHandles[i]);

	{
		std::lock_guard<std::mutex> lock(myMutex);

		const auto itBegin = std::partition(myHandleData.begin(), myHandleData.end(), [&signaledEvents](const auto& aHandleData)
			{
				return std::find(signaledEvents.begin(), signaledEvents.end(), aHandleData.Handle) == signaledEvents.end();
			});

		for (auto it = itBegin; it != myHandleData.end(); ++it)
			signaledPromises.emplace_back(std::move(it->Promise));

		myHandleData.erase(itBegin, myHandleData.end());
	}

	for (auto&& promise : signaledPromises)
		promise.Complete();
}

void WinEventSystemImpl::TriggerWaitInterruption()
{
	Get<WinPlatformSystem>().SetEvent(myInterruptEvent);
}

void WinEventSystemImpl::UnregisterEvent(
	HANDLE aEventHandle)
{
	{
		std::lock_guard<std::mutex> lock(myMutex);

		const auto it = std::find_if(myHandleData.begin(), myHandleData.end(), [aEventHandle](auto& aHandleData)
			{
				return aHandleData.Handle == aEventHandle;
			});

		if (it == myHandleData.cend())
			return;

		myHandleData.erase(it);
	}

	TriggerWaitInterruption();
}
