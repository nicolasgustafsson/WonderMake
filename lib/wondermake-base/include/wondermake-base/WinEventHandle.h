#pragma once

#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/WinPlatform.h"

#include <memory>

class WinEventHandle
{
public:
	WinEventHandle() noexcept = default;
	inline WinEventHandle(HANDLE aHandle, WinPlatformSystem& aWinPlatformSys) noexcept
		: myHandle(std::shared_ptr<HANDLE>(new HANDLE(aHandle), [&aWinPlatformSys](HANDLE* aPtr)
			{
				(void)aWinPlatformSys.CloseHandle(*aPtr);

				delete(aPtr);
			}))
	{}

	[[nodiscard]] bool operator==(const WinEventHandle&) const noexcept = default;
	[[nodiscard]] bool operator!=(const WinEventHandle&) const noexcept = default;

	inline [[nodiscard]] bool operator==(const HANDLE& aRhs) const noexcept
	{
		return Get() == aRhs;
	}
	inline [[nodiscard]] bool operator!=(const HANDLE& aRhs) const noexcept
	{
		return Get() != aRhs;
	}

	inline [[nodiscard]] HANDLE Get() const noexcept
	{
		if (!myHandle)
			return NULL;

		return *myHandle;
	}

private:
	std::shared_ptr<HANDLE> myHandle;

};
