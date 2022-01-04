#pragma once
#include "SystemGlobal.h"

#include <mutex>

template<typename TSystem>
class SystemPtr
{
public:
	SystemPtr()
	{
		myMutex.lock();
	}
	~SystemPtr()
	{
		myMutex.unlock();
	}

	[[nodiscard]] TSystem& operator*() noexcept
	{
		static TSystem& instance = Global::GetSystemContainer().Get<TSystem>();
		return instance;
	}

	[[nodiscard]] TSystem* operator->() noexcept
	{
		static TSystem& instance = Global::GetSystemContainer().Get<TSystem>();
		return &instance;
	}

	[[nodiscard]] const TSystem& operator*() const noexcept
	{
		static const TSystem& instance = Global::GetSystemContainer().Get<TSystem>();
		return instance;
	}

	[[nodiscard]] const TSystem* operator->() const noexcept
	{
		static const TSystem& instance = Global::GetSystemContainer().Get<TSystem>();
		return &instance;
	}

private:
	std::recursive_mutex myMutex;
};

