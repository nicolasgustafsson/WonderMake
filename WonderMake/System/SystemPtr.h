#pragma once
#include "SystemContainer.h"
template<typename TSystem>
class SystemPtr
{
public:
	SystemPtr()
		:myReference(SystemContainer::Get().GetSystem<TSystem>())
	{

	}

	[[nodiscard]] TSystem& operator*() noexcept
	{
		return myReference;
	}

	[[nodiscard]] TSystem* operator->() noexcept
	{
		return &myReference;
	}

	[[nodiscard]] const TSystem& operator*() const noexcept
	{
		return myReference;
	}

	[[nodiscard]] const TSystem* operator->() const noexcept
	{
		return &myReference;
	}

private:
	TSystem& myReference;
};

