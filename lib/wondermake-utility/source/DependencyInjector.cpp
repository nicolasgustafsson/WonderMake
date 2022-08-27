#include "wondermake-utility/DependencyInjector.h"

#include "wondermake-utility/Utility.h"

Result<void, DependencyInjector::SError> DependencyInjector::CreateAll()
{
	for (auto& [key, createFunc] : myCreateFuncs)
	{
		const auto depIt = myDependencies.find(key);

		if (depIt != myDependencies.cend())
		{
			continue;
		}

		auto result = std::move(createFunc)(*this);

		if (!result)
			return result;
	}

	return Ok();
}
