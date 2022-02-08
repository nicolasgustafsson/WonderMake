#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/WinEventSystem.h"

class WinEventSystemMock
	: public WinEventSystem
{
public:
	MOCK_METHOD(void, RegisterEvent, (
		HANDLE aEventHandle,
		Closure&& aCallback), (override));
	MOCK_METHOD(void, UnregisterEvent, (
		HANDLE aEventHandle), (override));

	MOCK_METHOD(void, WaitForEvent, (
		DWORD aTimeoutMs), (override));

	static auto CreateRegisterEvent()
	{
		auto funcPtr = std::make_shared<Closure>([]() {});

		return std::make_pair([funcPtr]()
			{
				std::move(*funcPtr)();
			},
			[funcPtr](auto, auto&& aCallback)
			{
				*funcPtr = std::move(aCallback);
			});
	}

};
