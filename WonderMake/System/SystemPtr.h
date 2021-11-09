#pragma once
#include "SystemContainer.h"

#include <mutex>

#include "Universe/UniverseSystem.h"

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


#pragma warning( push )
#pragma warning( disable : 4702 )
	[[nodiscard]] TSystem& operator*() noexcept
	{
		if constexpr(IsTemplateInstanceOf<TSystem::Super, UniverseSystem>::value)
		{
			TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

			return instance;
		}
		static TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

		return instance;
	}

	[[nodiscard]] TSystem* operator->() noexcept
	{
		if constexpr (IsTemplateInstanceOf<TSystem::Super, UniverseSystem>::value)
		{
			TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

			return &instance;
		}

		static TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

		return &instance;
	}

	[[nodiscard]] const TSystem& operator*() const noexcept
	{
		if constexpr (IsTemplateInstanceOf<TSystem::Super, UniverseSystem>::value)
		{
			TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

			return instance;
		}

		static const TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

		return instance;
	}

	[[nodiscard]] const TSystem* operator->() const noexcept
	{
		if constexpr (IsTemplateInstanceOf<TSystem::Super, UniverseSystem>::value)
		{
			TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

			return &instance;
		}

		static const TSystem& instance = SystemContainer::Get().GetSystem<TSystem>();

		return &instance;
	}
#pragma warning( pop )

private:
	std::recursive_mutex myMutex;
};

