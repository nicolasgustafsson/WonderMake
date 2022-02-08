#include "WonderMakeUtility/DependencyInjector.h"

#include "WonderMakeUtility/Utility.h"

Result<DependencyInjector::ECreateError, decltype(Success), std::string> DependencyInjector::CreateAll()
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
			return { static_cast<ECreateError>(result), result.Meta() };
	}

	return Success;
}
