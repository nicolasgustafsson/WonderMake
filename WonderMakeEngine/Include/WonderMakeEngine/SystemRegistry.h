#pragma once

#include "System.h"
#include "SystemContainer.h"

#include "WonderMakeBase/DependencyInjector.h"

#include <cassert>
#include <functional>
#include <memory>
#include <ranges>
#include <typeindex>
#include <vector>

class SystemRegistry
{
public:
	template<typename TSystem, typename TBaseSystem = TSystem, typename TCreateFunc>
	inline void AddSystem(TCreateFunc&& aCreateFunc)
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to add system that does not inherit from System.");

		AddSystemHelper<TSystem, TBaseSystem>(std::forward<TCreateFunc>(aCreateFunc), TupleWrapper<typename TSystem::Dependencies>());
	}

	inline SystemContainer CreateSystems(SystemTraits::SetList aTraitNotFilter)
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

		try
		{
			myDependencyInjector.CreateAll();
		}
		catch (DependencyInjector::MissingDependencyException aException)
		{
			// WmLog("Missing dependency: ", aException.myMissingType, "."); TODO: Logging

			assert(false && "Missing dependency.");

			return SystemContainer();
		}

		SystemContainer::InternalRep internalRep;

		std::swap(internalRep, myConstructingContainer);

		return internalRep;
	}

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	struct SystemElement
	{
		SystemTraits::SetList TraitSet;
		std::function<void()> InjectFunc;
	};

	template<typename TSystem, typename TBaseSystem, typename TCreateFunc, typename ...TDependencies>
	inline void AddSystemHelper(TCreateFunc&& aCreateFunc, TupleWrapper<std::tuple<TDependencies...>>&&)
	{
		mySystemList.emplace_back(
			SystemElement {
				TSystem::template TraitSet::ToObject(),
				[this, createFunc = std::move(aCreateFunc)] ()
				{
					auto construct = [this, createFunc = std::move(createFunc)](std::decay_t<TDependencies>&... aDependencies)->TBaseSystem&
					{
						TSystem::InjectDependencies(std::tie(aDependencies...));

						auto ptr = createFunc();

						auto&& system = *ptr;

						myConstructingContainer.emplace(std::make_pair<std::type_index, std::shared_ptr<SystemAbstracted>>(typeid(TBaseSystem), std::move(ptr)));

						return system;
					};

					myDependencyInjector.Add<TBaseSystem, decltype(construct), std::decay_t<TDependencies>...>(std::move(construct));
				}
			});
	}

	static thread_local SystemContainer::InternalRep myConstructingContainer;

	DependencyInjector myDependencyInjector;
	std::vector<SystemElement> mySystemList;

};
