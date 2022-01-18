#include "WonderMakeEngine/SystemGlobal.h"
#include "WonderMakeEngine/WinPlatformSystem.h"

#include "WonderMakeBase/Bindable.h"
#include "WonderMakeBase/Utility.h"

#include "WinEventSystemImpl.h"

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

void WinEventSystemImpl::RegisterEvent(
	HANDLE aEventHandle,
	Closure&& aCallback)
{
	{
		std::lock_guard<std::mutex> lock(myMutex);

		myHandleData.emplace_back(HandleData{ aEventHandle, std::move(aCallback) });
	}

	TriggerWaitInterruption();
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
	std::vector<Closure> signaledCallbacks;

	signaledEvents.reserve(MAXIMUM_WAIT_OBJECTS - 1);
	signaledCallbacks.reserve(MAXIMUM_WAIT_OBJECTS - 1);

	for (size_t i = 1; i < myHandles.size(); ++i)
		if (winPlatformSystem.WaitForSingleObject(myHandles[i], 0) == WAIT_OBJECT_0)
			signaledEvents.emplace_back(myHandles[i]);

	{
		std::lock_guard<std::mutex> lock(myMutex);

		for (auto handle : signaledEvents)
		{
			const auto it = std::find_if(myHandleData.begin(), myHandleData.end(), [handle](auto& aHandleData)
				{
					return aHandleData.Handle == handle;
				});

			if (it == myHandleData.cend())
				continue;

			signaledCallbacks.emplace_back(std::move(it->Callback));

			myHandleData.erase(it);
		}
	}

	for (auto&& callback : signaledCallbacks)
		std::move(callback)();
}

void WinEventSystemImpl::TriggerWaitInterruption()
{
	Get<WinPlatformSystem>().SetEvent(myInterruptEvent);
}
