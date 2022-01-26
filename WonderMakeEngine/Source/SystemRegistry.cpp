#include "WonderMakeEngine/SystemRegistry.h"

thread_local SystemContainer::InternalRep SystemRegistry::myConstructingContainer;

Result<SystemRegistry::ECreateError, SystemContainer, std::string> SystemRegistry::CreateSystems(const Filter& aFilter)
{
	myDependencyInjector = DependencyInjector();

	const auto requiredFilter = [&filter = aFilter.RequiredAnyTraits](auto&& aSystemInfo)
	{
		if (!filter)
			return true;

		for (auto&& trait : *filter)
			if (aSystemInfo.TraitSet.find(trait) != aSystemInfo.TraitSet.cend())
				return true;

		return false;
	};
	const auto disallowedFilter = [&filter = aFilter.DisallowedTraits](auto&& aSystemInfo)
	{
		if (!filter)
			return true;

		for (auto&& trait : aSystemInfo.TraitSet)
			if (filter->find(trait) != filter->cend())
				return false;

		return true;
	};

	for (auto&& system : std::views::all(mySystemList)
		| std::views::filter(requiredFilter)
		| std::views::filter(disallowedFilter))
	{
		system.InjectFunc();
	}

	auto result = myDependencyInjector.CreateAll();

	if (!result)
	{
		const auto err = static_cast<DependencyInjector::ECreateError>(result);

		switch (err)
		{
		case DependencyInjector::ECreateError::MissingDependency: return { ECreateError::MissingSystemDependency, result.Meta() };
		}

		const auto errorCode = static_cast<std::underlying_type_t<DependencyInjector::ECreateError>>(err);

		return { ECreateError::InternalError, "Unknown error; " + std::to_string(errorCode) };
	}

	SystemContainer::InternalRep internalRep;

	std::swap(internalRep, myConstructingContainer);

	return SystemContainer(std::move(internalRep));
}
