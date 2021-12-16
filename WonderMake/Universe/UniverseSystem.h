#pragma once
#include "UniverseManagerSystem.h"
#include "System/System.h"

template <typename TUniverseSystem>
class UniverseSystemCollection : public System<UniverseManagerSystem>
{
public:
	TUniverseSystem& GetSystemFromCurrentUniverse() const
	{
		const Id universeId = Get<const UniverseManagerSystem>().GetCurrentUniverse();
		const u64 key = Get<const UniverseManagerSystem>().GetCurrentUniverse().GetRawId();
        auto&& universeSystems = GetUniverseSystems();
		if (universeSystems.KeyExists(key))
			return universeSystems.Get(key);
		else
		{
            universeSystems.Emplace(key);
			TUniverseSystem& ref = universeSystems.Get(key);
			
			ref.myUniverseId = universeId;
			return ref;
		}
	}

	Container<TUniverseSystem*> GetAllSystems()
	{
        auto&& universeSystems = GetUniverseSystems();

        Container<TUniverseSystem*> universes;
		for (auto& i : universeSystems)
		{
			universes.Add(&i.second);
		}

		return universes;
	}

private:

    Container<TUniverseSystem, Key<u64>>& GetUniverseSystems() const
    {
        static Container<TUniverseSystem, Key<u64>> universeSystems;
        return universeSystems;
    }

};

template <class CRTP, typename ... TDependencies>
class UniverseSystem : public System<UniverseSystemCollection<CRTP>, TDependencies...>
{
public:
	UniverseSystem() = default;
	using Super = UniverseSystem;

	[[nodiscard]] CRTP& GetSystem() const
	{
		return this->template Get<const UniverseSystemCollection<CRTP>>().GetSystemFromCurrentUniverse();
	}

	template<typename T>
	T& Resolve()
	{
		return GetSystem();
	}

	template<typename T>
	T& Resolve() const
	{
		return GetSystem();
	}



protected:
	Id myUniverseId;

private:
	friend CRTP;
	friend UniverseSystemCollection<CRTP>;
};

#define REGISTER_UNIVERSE_SYSTEM(aSystem) _REGISTER_SYSTEM_IMPL(aSystem, aSystem); _REGISTER_SYSTEM_IMPL(UniverseSystemCollection<aSystem>, UniverseSystemCollection##aSystem)
