#pragma once

#include "Message/Messages.h"

#include "System/System.h"
#include "System/SystemContainer_v2.h"

#include "Utilities/DependencyInjector.h"

#include <cassert>
#include <functional>
#include <memory>
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

	inline SystemContainer_v2 CreateSystems()
	{
		myDependencyInjector = DependencyInjector();

		for (auto&& injectFunc : myInjectFuncList)
		{
			injectFunc();
		}

		try
		{
			myDependencyInjector.CreateAll();
		}
		catch (DependencyInjector::MissingDependencyException aException)
		{
			WmLog("Missing dependency: ", aException.myMissingType, ".");

			assert(false && "Missing dependency");

			return SystemContainer_v2();
		}

		SystemContainer_v2::InternalRep internalRep;

		std::swap(internalRep, myConstructingContainer);

		return internalRep;
	}

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	template<typename TSystem, typename TBaseSystem, typename TCreateFunc, typename ...TDependencies>
	inline void AddSystemHelper(TCreateFunc&& aCreateFunc, TupleWrapper<std::tuple<TDependencies...>>&&)
	{
		myInjectFuncList.emplace_back([this, createFunc = std::move(aCreateFunc)]() // myInjectFuncList is reset, probably something to do with extern
			{
				auto construct = [this, createFunc = std::move(createFunc)](std::decay_t<TDependencies>&... aDependencies) -> TBaseSystem&
				{
					TSystem::InjectDependencies(std::tie(aDependencies...));

					auto ptr = createFunc();

					auto&& system = *ptr;

					myConstructingContainer.emplace(std::make_pair<std::type_index, std::shared_ptr<SystemAbstracted>>(typeid(TBaseSystem), std::move(ptr)));

					return system;
				};

				myDependencyInjector.Add<TSystem, decltype(construct), std::decay_t<TDependencies>...>(std::move(construct));
			});
	}

	static thread_local SystemContainer_v2::InternalRep myConstructingContainer;

	DependencyInjector myDependencyInjector;
	std::vector<std::function<void()>> myInjectFuncList;

};
