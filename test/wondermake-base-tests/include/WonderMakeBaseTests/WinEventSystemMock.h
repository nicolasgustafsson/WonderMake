#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/WinEventSystem.h"

class WinEventSystemMock
	: public WinEventSystem
{
public:
	MOCK_METHOD(Future<void>, RegisterEvent, (
		HANDLE aEventHandle), (override));
	
	MOCK_METHOD(void, WaitForEvent, (
		DWORD aTimeoutMs), (override));

	void DelegateToFake()
	{
		ON_CALL(*this, RegisterEvent)
			.WillByDefault([](auto&&)
				{
					auto [promise, future] = MakeAsync<void>();

					(void)std::make_unique<Promise<void>>(std::move(promise)).release();

					return future;
				});
	}

};
