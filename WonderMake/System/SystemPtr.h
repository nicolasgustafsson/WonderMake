#pragma once
#include "SystemContainer.h"
template<typename TSystem>
class SystemPtr
{
public:
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
};

