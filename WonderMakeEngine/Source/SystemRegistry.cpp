#include "WonderMakeEngine/SystemRegistry.h"

thread_local SystemContainer::InternalRep SystemRegistry::myConstructingContainer;

Result<SystemRegistry::ECreateError, SystemContainer, std::string> SystemRegistry::CreateSystems(SystemTraits::SetList aTraitNotFilter)
{
	myDependencyInjector = DependencyInjector();

	const auto notFilter = [&aTraitNotFilter](auto&& aSystemInfo)
	{
		for (auto&& trait : aSystemInfo.TraitSet)
			if (aTraitNotFilter.find(trait) != aTraitNotFilter.cend())
				return false;

		return true;
	};

	for (auto&& system : std::views::all(mySystemList) | std::views::filter(notFilter))
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
