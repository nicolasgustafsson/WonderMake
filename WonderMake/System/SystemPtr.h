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

	TSystem& operator*()
	{
		return myReference;
	}

	TSystem* operator->()
	{
		return &myReference;
	}

private:
	TSystem& myReference;
};

