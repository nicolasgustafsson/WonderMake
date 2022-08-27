#include "wondermake-base/SystemRegistry.h"

thread_local SystemContainer::InternalRep SystemRegistry::myConstructingContainer;

Result<SystemContainer, SystemRegistry::SError> SystemRegistry::CreateSystems(const Filter& aFilter)
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
		const auto err = result.Err();

		switch (err.Error)
		{
		case DependencyInjector::ECreateError::MissingDependency: return Err(SError{ ECreateError::MissingSystemDependency, err.Reason });
		}

		const auto errorCode = static_cast<std::underlying_type_t<DependencyInjector::ECreateError>>(err.Error);

		return Err(SError{ ECreateError::InternalError, "Unknown error; " + std::to_string(errorCode) });
	}

	SystemContainer::InternalRep internalRep;

	std::swap(internalRep, myConstructingContainer);

	return Ok(std::move(internalRep));
}
