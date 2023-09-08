#pragma once

#ifndef WM_SUPPRESS_SYSTEM_GLOBAL

#include "SystemContainer.h"
#include "SystemRegistry.h"

#include "wondermake-utility/AutoRegister.h"

class Global
{
public:
	static SystemContainer&	GetSystemContainer();
	static SystemRegistry&	GetSystemRegistry();
};

template<typename TSystem, typename TBaseSystem, Guid Id>
static void _RegisterSystem()
{
	static_assert(std::is_same_v<TBaseSystem, TSystem> || std::is_base_of_v<TBaseSystem, TSystem>, "Registered system must inherit from the base system.");

	std::optional<SystemId> id;

	if (Id != Guid::Zero())
		id = Id;

	Global::GetSystemRegistry().AddSystem<TSystem, TBaseSystem>([](auto aOnCreate, auto aOnDestroy) -> SharedReference<TSystem>
		{
			constexpr auto createSystem = [&aOnCreate, &aOnDestroy]()
			{
				auto destroy = [onDestroy = std::move(aOnDestroy)](TSystem* aPtr) mutable
				{
					if (!aPtr)
						return;

					std::move(onDestroy)(*aPtr);

					delete aPtr;
				};

				aOnCreate();

				auto ptr = std::shared_ptr<TSystem>(new TSystem(), destroy);

				auto ref = SharedReference<TSystem>::FromPointer(std::move(ptr))
					.Unwrap();

				ref->Initialize();

				return ref;
			};

			if constexpr (TSystem::TraitSet::template HasTrait<STSingleton>)
			{
				static auto ref = createSystem();

				return ref;
			}
			else
				return createSystem();
		},
		id);
}

template<typename TSystem>
inline TSystem& GetSystem()
	requires(
		TSystem::TraitSet::template HasTrait<STSingleton> &&
		TSystem::TraitSet::template HasTrait<STThreadsafe>)
{
	return Global::GetSystemContainer().Get<TSystem>();
}

#define _WM_REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem) _RegisterSystem<aSystem, aBaseSystem, Guid::Zero()>
#define _WM_REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystemName) WM_AUTO_REGISTER(_WM_REGISTER_SYSTEM_CLASS(aSystem, aBaseSystem), aSystemName)
#define _WM_REGISTER_SYSTEM_IMPL(aSystem, aSystemName) _WM_REGISTER_SYSTEM_MASKED_IMPL(aSystem, aSystem, aSystemName)

#define WM_REGISTER_SYSTEM_MASKED(aSystem, aBaseSystem) _WM_REGISTER_SYSTEM_MASKED_IMPL(aSystem, aBaseSystem, aSystem)
#define WM_REGISTER_SYSTEM(aSystem) _WM_REGISTER_SYSTEM_IMPL(aSystem, aSystem)

#define _WM_REGISTER_SYSTEM_CLASS_WITH_ID(aSystem, aBaseSystem, aGuidId) _RegisterSystem<aSystem, aBaseSystem, aGuidId>
#define _WM_REGISTER_SYSTEM_WITH_ID_MASKED_IMPL(aSystem, aBaseSystem, aSystemName, aGuidId) WM_AUTO_REGISTER(_WM_REGISTER_SYSTEM_CLASS_WITH_ID(aSystem, aBaseSystem, aGuidId), aSystemName)
#define _WM_REGISTER_SYSTEM_WITH_ID_IMPL(aSystem, aSystemName, aGuidId) _WM_REGISTER_SYSTEM_WITH_ID_MASKED_IMPL(aSystem, aSystem, aSystemName, aGuidId)

#define WM_REGISTER_SYSTEM_WITH_ID_MASKED(aSystem, aBaseSystem, aIdString) _WM_REGISTER_SYSTEM_WITH_ID_MASKED_IMPL(aSystem, aBaseSystem, aSystem, *Guid::Parse( aIdString ))
#define WM_REGISTER_SYSTEM_WITH_ID(aSystem, aIdString) _WM_REGISTER_SYSTEM_WITH_ID_IMPL(aSystem, aSystem, *Guid::Parse( aIdString ))

#else

#define WM_REGISTER_SYSTEM_MASKED(aSystem, aBaseSystem)
#define WM_REGISTER_SYSTEM(aSystem)

#define WM_REGISTER_SYSTEM_WITH_ID_MASKED(aSystem, aBaseSystem, aIdString)
#define WM_REGISTER_SYSTEM_WITH_ID(aSystem, aIdString)

#endif
