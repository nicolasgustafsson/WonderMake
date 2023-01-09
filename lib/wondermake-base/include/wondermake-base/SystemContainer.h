#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

class SystemAbstracted;

class SystemContainer
{
public:
	SystemContainer() noexcept = default;
	
	template<typename TSystem>
	inline [[nodiscard]] TSystem* TryGet() const
	{
		auto it = mySystems.find(typeid(TSystem));

		if (it == mySystems.cend())
			return nullptr;

		return static_cast<TSystem*>(it->second.Self.get());
	}
	template<typename TSystem>
	inline [[nodiscard]] TSystem& Get() const
	{
		return *static_cast<TSystem*>(mySystems.find(typeid(TSystem))->second.Self.get());
	}
	
	template<typename TSystem>
	inline void Add(std::shared_ptr<TSystem> aSystem, std::unordered_set<std::shared_ptr<SystemAbstracted>> aDependencies = {})
	{
		mySystems.emplace(std::make_pair<std::type_index, SSystem>(
			typeid(TSystem),
			SSystem
			{
				.Dependencies = std::move(aDependencies),
				.Self = std::move(aSystem)
			}));
	}

private:
	struct SSystem
	{
		inline ~SSystem()
		{
			Self.reset();

			Dependencies.clear();
		}

		std::unordered_set<std::shared_ptr<SystemAbstracted>> Dependencies;
		std::shared_ptr<SystemAbstracted> Self;
	};

	std::unordered_map<std::type_index, SSystem> mySystems;

};
