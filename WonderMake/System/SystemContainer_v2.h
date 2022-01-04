#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

class SystemAbstracted;
class SystemRegistry;

class SystemContainer_v2
{
private:
	// So both can use InternalRep
	friend SystemRegistry;

	using InternalRep = std::unordered_map<std::type_index, std::shared_ptr<SystemAbstracted>>;

public:
	SystemContainer_v2() noexcept = default;
	inline SystemContainer_v2(InternalRep&& aMap)
		: myMap(std::move(aMap))
	{}

	template<typename TSystem>
	inline [[nodiscard]] TSystem* TryGet()
	{
		auto it = myMap.find(typeid(TSystem));

		if (it == myMap.cend())
		{
			return nullptr;
		}

		return static_cast<TSystem*>(it->second.get());
	}
	template<typename TSystem>
	inline [[nodiscard]] TSystem& Get()
	{
		return *static_cast<TSystem*>(myMap.find(typeid(TSystem))->second.get());
	}

	template<typename TSystem>
	inline void Add(std::shared_ptr<TSystem> aSystem)
	{
		myMap.emplace(std::make_pair<std::type_index, std::shared_ptr<SystemAbstracted>>(typeid(TSystem), std::move(aSystem)));
	}

private:
	InternalRep myMap;

};
