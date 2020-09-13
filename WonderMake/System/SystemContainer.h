#pragma once

#include "Message/Messages.h"

#include "Utilities/AutoRegister.h"
#include "Utilities/Singleton.h"
#include "Utilities/DependencyInjector.h"

#include <cassert>
#include <mutex>

class SystemBase;

class SystemContainer final
	: public Singleton<SystemContainer>
{
public:
	template<typename TSystem>
	void AddSystem();

	template<typename TSystem>
	[[nodiscard]] TSystem& GetSystem();

	template<typename TSystem>
	void CreateSystem();

	void CreateAllSystems();

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	std::recursive_mutex myMutex;
	DependencyInjector myDependencyInjector;

	template<typename TSystem, typename ...TDependencies>
	void AddSystemHelper(TupleWrapper<std::tuple<TDependencies...>>&&);
};

template<typename TSystem>
static void _RegisterSystem()
{
	SystemContainer::Get().AddSystem<TSystem>();
}

#define _REGISTER_SYSTEM_IMPL(aSystem, aSystemName) WM_AUTO_REGISTER(_RegisterSystem<aSystem>, aSystemName)

#define REGISTER_SYSTEM(aSystem) _REGISTER_SYSTEM_IMPL(aSystem, aSystem)

template<typename TSystem>
void SystemContainer::AddSystem()
{
	static_assert(std::is_base_of<SystemBase, TSystem>::value, "Tried to add system that does not inherit from System.");

	AddSystemHelper<TSystem>(TupleWrapper<typename TSystem::Dependencies>());
}

template<typename TSystem>
[[nodiscard]] TSystem& SystemContainer::GetSystem()
{
	static_assert(std::is_base_of<SystemBase, TSystem>::value, "Tried to get system that does not inherit from System.");

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
void SystemContainer::CreateSystem()
{
	static_assert(std::is_base_of<SystemBase, TSystem>::value, "Tried to create system that does not inherit from System.");

	(void)GetSystem<TSystem>();
}

template<typename TSystem, typename ...TDependencies>
void SystemContainer::AddSystemHelper(TupleWrapper<std::tuple<TDependencies...>>&&)
{
	auto construct = [](std::decay_t<TDependencies>&... aDependencies) -> TSystem&
	{
		static TSystem sys(std::tie(aDependencies...));

		return sys;
	};

	myDependencyInjector.Add<TSystem, decltype(construct), std::decay_t<TDependencies>...>(std::move(construct));
}
