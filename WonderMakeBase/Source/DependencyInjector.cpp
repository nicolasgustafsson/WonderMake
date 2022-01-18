#include "WonderMakeBase/DependencyInjector.h"

#include "WonderMakeBase/Utility.h"

void DependencyInjector::CreateAll()
{
	for (auto& [key, createFunc] : myCreateFuncs)
	{
		const auto depIt = myDependencies.find(key);

		if (depIt != myDependencies.cend())
		{
			continue;
		}

		Utility::Invoke(createFunc, *this);
	}
}
