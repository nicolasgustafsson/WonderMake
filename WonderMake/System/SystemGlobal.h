#pragma once

#include "System/SystemContainer_v2.h"
#include "System/SystemRegistry.h"

#include "WonderMakeBase/AutoRegister.h"

class Global
{
public:
	static SystemContainer_v2& GetSystemContainer();
	static SystemRegistry& GetSystemRegistry();
};

template<typename TSystem, typename TBaseSystem>
static void _RegisterSystem()
{
	static_assert(std::is_same_v<TBaseSystem, TSystem> || std::is_base_of_v<TBaseSystem, TSystem>, "Registered system must inherit from the base system.");

	Global::GetSystemRegistry().AddSystem<TSystem, TBaseSystem>([]() -> std::shared_ptr<TSystem>
		{
			return std::make_shared<TSystem>();
		});
}

#define _REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem) _RegisterSystem<aSystem, aBaseSystem>

#define _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystemName) WM_AUTO_REGISTER(_REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem), aSystemName)
#define _REGISTER_SYSTEM_IMPL(aSystem, aSystemName) _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aSystem, aSystemName)

#define REGISTER_SYSTEM_MASKED(aSystem, aBaseSystem) _REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystem)
#define REGISTER_SYSTEM(aSystem) _REGISTER_SYSTEM_IMPL(aSystem, aSystem)
