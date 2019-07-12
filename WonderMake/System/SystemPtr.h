#pragma once
#include "SystemContainer.h"
template<typename TSystem>
class SystemPtr
{
public:
	SystemPtr() noexcept
		:myReference(SystemContainer::Get().GetSystem<TSystem>())
	{

	}

	TSystem& operator*() noexcept
	{
		return myReference;
	}

	TSystem* operator->() noexcept
	{
		return &myReference;
	}

private:
	TSystem& myReference;
};

