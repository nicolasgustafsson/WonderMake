#pragma once

#include "System.h"
#include "SystemContainer.h"

#include "wondermake-utility/DependencyInjector.h"

#include <magic_enum.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <typeindex>
#include <vector>

class SystemRegistry
{
public:
	enum class ECreateError
	{
		MissingSystemDependency,
		InternalError
	};

	struct SError
	{
		ECreateError	Error = ECreateError::InternalError;
		std::string		Reason;
	};

	struct Filter
	{
		std::vector<SystemTraits::SetList> RequiredAnyTraits;
		std::optional<SystemTraits::SetList> DisallowedTraits;
	};

	template<typename TSystem, typename TBaseSystem = TSystem, typename TCreateFunc>
	inline void AddSystem(TCreateFunc&& aCreateFunc)
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to add system that does not inherit from System.");

		AddSystemHelper<TSystem, TBaseSystem>(std::forward<TCreateFunc>(aCreateFunc), TupleWrapper<typename TSystem::Dependencies>());
	}

	[[nodiscard]] Result<SystemContainer, SError> CreateSystems(const Filter& aFilter);

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	struct SystemElement
	{
		SystemTraits::SetList TraitSet;
		std::function<void(SystemRegistry&, DependencyInjector&)> InjectFunc;
	};

	template<typename TSystem, typename TBaseSystem, typename TCreateFunc, typename... TDependencies>
	inline void AddSystemHelper(TCreateFunc&& aCreateFunc, TupleWrapper<std::tuple<SharedReference<TDependencies>...>>&&)
	{
		mySystemList.emplace_back(
			SystemElement
			{
				TSystem::template TraitSet::ToObject(),
				[createFunc = std::move(aCreateFunc)] (SystemRegistry& aSelf, DependencyInjector& aDI)
				{
					auto construct = [&aSelf, createFunc = std::move(createFunc)](SharedReference<std::decay_t<TDependencies>>... aDependencies)
					{
						TSystem::InjectDependencies(std::make_tuple(std::move(aDependencies)...));

						auto ref = createFunc();

						aSelf.myConstructingContainer.Add<TSystem>(ref);

						return ref;
					};

					aDI.Add<SharedReference<TSystem>, decltype(construct), SharedReference<std::decay_t<TDependencies>>...>(std::move(construct));

					if constexpr (!std::is_same_v<TSystem, TBaseSystem>)
					{
						auto constructBase = [&aSelf](DependencyInjector& aDI)
						{
							SharedReference<TSystem> ref = aDI.Get<SharedReference<TSystem>>()
								.Unwrap()
								.get();

							aSelf.myConstructingContainer.Add<TBaseSystem>(ref);

							return ref;
						};

						aDI.Add<SharedReference<TBaseSystem>, decltype(constructBase), DependencyInjector>(std::move(constructBase));
					}
				}
			});
	}

	SystemContainer myConstructingContainer;

	std::vector<SystemElement> mySystemList;

};

inline static void WmLogStream(std::ostream& aStream, const SystemRegistry::SError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
