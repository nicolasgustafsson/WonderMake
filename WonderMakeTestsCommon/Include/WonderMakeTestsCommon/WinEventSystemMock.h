#pragma once

#include "GTestInclude.h"

#include "WonderMakeEngine/WinEventSystem.h"

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
};
