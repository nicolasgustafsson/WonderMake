#pragma once

#ifndef WM_SUPRESS_SYSTEM_GLOBAL

#include "SystemContainer.h"
#include "SystemRegistry.h"

#include "wondermake-utility/AutoRegister.h"

class Global
{
public:
	static SystemContainer& GetSystemContainer();
	static SystemRegistry& GetSystemRegistry();
};

template<typename TSystem, typename TBaseSystem>
static void _RegisterSystem()
{
	static_assert(std::is_same_v<TBaseSystem, TSystem> || std::is_base_of_v<TBaseSystem, TSystem>, "Registered system must inherit from the base system.");

	Global::GetSystemRegistry().AddSystem<TSystem, TBaseSystem>([]() -> std::shared_ptr<TSystem>
		{
			constexpr auto createSystem = []()
			{
				auto ptr = std::make_shared<TSystem>();

				ptr->Initialize();

				return ptr;
			};

			if constexpr (TSystem::TraitSet::template HasTrait<STSingleton>)
			{
				static auto ptr = createSystem();

				return ptr;
			}
			else
				return createSystem();
		});
}

#define _REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem) _RegisterSystem<aSystem, aBaseSystem>

#define _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystemName) WM_AUTO_REGISTER(_REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem), aSystemName)
#define _REGISTER_SYSTEM_IMPL(aSystem, aSystemName) _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aSystem, aSystemName)

#define REGISTER_SYSTEM_MASKED(aSystem, aBaseSystem) _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystem)
#define REGISTER_SYSTEM(aSystem) _REGISTER_SYSTEM_IMPL(aSystem, aSystem)

#else

#define REGISTER_SYSTEM_MASKED(aSystem, aBaseSystem)
#define REGISTER_SYSTEM(aSystem)

#endif
