#pragma once

#include "Message/Messages.h"

#include "Utilities/AutoRegister.h"
#include "Utilities/Singleton.h"
#include "Utilities/DependencyInjector.h"

#include <cassert>
#include <functional>
#include <mutex>

class SystemAbstracted;

class SystemContainer final
	: public Singleton<SystemContainer>
{
public:
	template<typename TSystem, typename TBaseSystem = TSystem, typename TCreateFunc>
	inline void AddSystem(TCreateFunc aCreateFunc)
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to add system that does not inherit from System.");

		AddSystemHelper<TSystem, TBaseSystem>(std::move(aCreateFunc), TupleWrapper<typename TSystem::Dependencies>());
	}

	template<typename TSystem>
	inline [[nodiscard]] TSystem& GetSystem()
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to get system that does not inherit from System.");

		std::lock_guard<decltype(myMutex)> lock(myMutex);

		try
		{
			return myDependencyInjector.Get<TSystem>();
		}
		catch (DependencyInjector::MissingDependencyException aException)
		{
			WmLog("Missing dependency: ", aException.myMissingType, ".");

			assert(false && "Missing dependency");

			TSystem* const failSystem = nullptr;

			return *failSystem;
		}
	}

	template<typename TSystem>
	inline void CreateSystem()
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to create system that does not inherit from System.");

		(void)GetSystem<TSystem>();
	}

	void CreateAllSystems();

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	std::recursive_mutex myMutex;
	DependencyInjector myDependencyInjector;

	template<typename TSystem, typename TBaseSystem, typename TCreateFunc, typename ...TDependencies>
	inline void AddSystemHelper(TCreateFunc aCreateFunc, TupleWrapper<std::tuple<TDependencies...>>&&)
	{
		auto construct = [createFunc = std::move(aCreateFunc)](std::decay_t<TDependencies>&... aDependencies)->TBaseSystem&
		{
			TSystem::InjectDependencies(std::tie(aDependencies...));

			return createFunc();
		};

		myDependencyInjector.Add<TBaseSystem, decltype(construct), std::decay_t<TDependencies>...>(std::move(construct));
	}
};

template<typename TSystem, typename TBaseSystem>
static void _RegisterSystem()
{
	static_assert(std::is_same_v<TBaseSystem, TSystem> || std::is_base_of_v<TBaseSystem, TSystem>, "Registered system must inherit from the base system.");

	SystemContainer::Get().AddSystem<TSystem, TBaseSystem>([]() -> TSystem&
		{
			static TSystem sys;

			return sys;
		});
}

#define _REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem) _RegisterSystem<aSystem, aBaseSystem>

#define _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystemName) WM_AUTO_REGISTER(_REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem), aSystemName)
#define _REGISTER_SYSTEM_IMPL(aSystem, aSystemName) _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aSystem, aSystemName)

#define REGISTER_SYSTEM_MASKED(aSystem, aBaseSystem) _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystem)
#define REGISTER_SYSTEM(aSystem) _REGISTER_SYSTEM_IMPL(aSystem, aSystem)
