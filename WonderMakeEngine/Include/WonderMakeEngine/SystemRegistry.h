#pragma once

#include "System.h"
#include "SystemContainer.h"

#include "WonderMakeUtility/DependencyInjector.h"

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <typeindex>
#include <vector>

class SystemRegistry
{
public:
	enum class ECreateError
	{
		MissingSystemDependency,
		InternalError
	};

	struct Filter
	{
		std::optional<SystemTraits::SetList> RequiredAnyTraits;
		std::optional<SystemTraits::SetList> DisallowedTraits;
	};

	template<typename TSystem, typename TBaseSystem = TSystem, typename TCreateFunc>
	inline void AddSystem(TCreateFunc&& aCreateFunc)
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to add system that does not inherit from System.");

		AddSystemHelper<TSystem, TBaseSystem>(std::forward<TCreateFunc>(aCreateFunc), TupleWrapper<typename TSystem::Dependencies>());
	}

	Result<ECreateError, SystemContainer, std::string> CreateSystems(const Filter& aFilter);

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
					auto construct = [this, createFunc = std::move(createFunc)](std::reference_wrapper<std::decay_t<TDependencies>>... aDependencies) -> TBaseSystem&
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
