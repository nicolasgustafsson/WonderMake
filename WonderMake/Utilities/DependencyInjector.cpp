#include "pch.h"
#include "DependencyInjector.h"

void DependencyInjector::CreateAll()
{
    return;
	for (auto& [key, createFunc] : myCreateFuncs)
	{
		const auto depIt = myDependencies.find(key);

		if (depIt != myDependencies.cend())
		{
			continue;
		}

		createFunc(*this);
	}
}
