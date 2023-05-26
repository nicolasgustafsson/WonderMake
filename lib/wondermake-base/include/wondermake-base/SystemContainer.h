#pragma once

#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

class SystemAbstracted;

class SystemContainer
{
public:
	SystemContainer() noexcept = default;
	
	template<typename TSystem>
	inline [[nodiscard]] std::shared_ptr<TSystem> GetPtr() const
	{
		auto it = mySystems.find(typeid(TSystem));

		if (it == mySystems.cend())
			return nullptr;

		return std::static_pointer_cast<TSystem>(static_cast<std::shared_ptr<SystemAbstracted>>(it->second));
	}
	template<typename TSystem>
	inline [[nodiscard]] TSystem* TryGet() const
	{
		auto it = mySystems.find(typeid(TSystem));

		if (it == mySystems.cend())
			return nullptr;

		return &static_cast<TSystem&>(*it->second);
	}
	template<typename TSystem>
	inline [[nodiscard]] TSystem& Get() const
	{
		return static_cast<TSystem&>(*mySystems.find(typeid(TSystem))->second);
	}
	
	template<typename TSystem>
	inline void Add(SharedReference<TSystem> aSystem)
	{
		mySystems.emplace(std::make_pair<std::type_index, SharedReference<SystemAbstracted>>(typeid(TSystem), std::move(aSystem)));
	}

private:
	std::unordered_map<std::type_index, SharedReference<SystemAbstracted>> mySystems;

};
