#pragma once

#include "Message/Messages.h"

#include "Utilities/AutoRegister.h"
#include "Utilities/Singleton.h"
#include "Utilities/DependencyInjector.h"

#include <cassert>
#include <functional>
#include <mutex>

class SystemBase;

class SystemContainer final
	: public Singleton<SystemContainer>
{
public:
	template<typename TSystem, typename TCreateFunc>
	void AddSystem(TCreateFunc aCreateFunc);

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

	template<typename TSystem, typename TCreateFunc, typename ...TDependencies>
	void AddSystemHelper(TCreateFunc aCreateFunc, TupleWrapper<std::tuple<TDependencies...>>&&);
};

template<typename TSystem>
static void _RegisterSystem()
{
	SystemContainer::Get().AddSystem<TSystem>([]() -> TSystem&
		{
			static TSystem sys;

			return sys;
		});
}

#define _REGISTER_SYSTEM_IMPL(aSystem, aSystemName) WM_AUTO_REGISTER(_RegisterSystem<aSystem>, aSystemName)

#define REGISTER_SYSTEM(aSystem) _REGISTER_SYSTEM_IMPL(aSystem, aSystem)

template<typename TSystem, typename TCreateFunc>
void SystemContainer::AddSystem(TCreateFunc aCreateFunc)
{
	static_assert(std::is_base_of<SystemBase, TSystem>::value, "Tried to add system that does not inherit from System.");

	AddSystemHelper<TSystem>(std::move(aCreateFunc), TupleWrapper<typename TSystem::Dependencies>());
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

template<typename TSystem, typename TCreateFunc, typename ...TDependencies>
void SystemContainer::AddSystemHelper(TCreateFunc aCreateFunc, TupleWrapper<std::tuple<TDependencies...>>&&)
{
	auto construct = [createFunc = std::move(aCreateFunc)](std::decay_t<TDependencies>&... aDependencies) -> TSystem&
	{
		TSystem::InjectDependencies(std::tie(aDependencies...));

		return createFunc();
	};

	myDependencyInjector.Add<TSystem, decltype(construct), std::decay_t<TDependencies>...>(std::move(construct));
}
