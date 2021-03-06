#pragma once
#include "SystemContainer.h"

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
		static TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();
		return instance;
	}

	[[nodiscard]] TSystem* operator->() noexcept
	{
		static TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();
		return &instance;
	}

	[[nodiscard]] const TSystem& operator*() const noexcept
	{
		static const TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();
		return instance;
	}

	[[nodiscard]] const TSystem* operator->() const noexcept
	{
		static const TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();
		return &instance;
	}

private:
	std::recursive_mutex myMutex;
};

