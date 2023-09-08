#pragma once

#include "System.h"
#include "SystemContainer.h"
#include "SystemCounter.h"

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
		std::vector<SystemTraits::SetList>		RequiredAnyTraits;
		std::optional<SystemTraits::SetList>	DisallowedTraits;
	};

	using GenerateSystemIdType = std::function<SystemId()>;

	SystemRegistry();

	template<typename TSystem, typename TBaseSystem = TSystem, typename TCreateFunc>
	inline void AddSystem(TCreateFunc&& aCreateFunc, std::optional<SystemId> aFixedId = std::nullopt)
	{
		static_assert(std::is_base_of<SystemAbstracted, TSystem>::value, "Tried to add system that does not inherit from System.");

		AddSystemHelper<TSystem, TBaseSystem>(std::forward<TCreateFunc>(aCreateFunc), aFixedId, TupleWrapper<typename TSystem::Dependencies>());
	}

	[[nodiscard]] Result<SystemContainer, SError> CreateSystems(const Filter& aFilter, GenerateSystemIdType aGenerateSystemId);
	template<typename TSystem>
	[[nodiscard]] inline Result<SystemContainer, SError> CreateSystem(GenerateSystemIdType aGenerateSystemId)
	{
		DependencyInjector dependencyInjector;

		dependencyInjector.Add<DependencyInjector>([&dependencyInjector]() -> auto&
			{
				return dependencyInjector;
			});

		for (auto& system : mySystemList)
			system.InjectFunc(*this, dependencyInjector, aGenerateSystemId);

		auto result = dependencyInjector.Get<SharedReference<TSystem>>();

		if (!result)
		{
			const auto err = result.Err();

			switch (err.Error)
			{
			case DependencyInjector::ECreateError::MissingDependency: return Err(SError{ ECreateError::MissingSystemDependency, err.Reason });
			}

			const auto errorCode = static_cast<std::underlying_type_t<DependencyInjector::ECreateError>>(err.Error);

			return Err(SError{ ECreateError::InternalError, "Unknown error; " + std::to_string(errorCode) });
		}

		SystemContainer container;

		std::swap(container, myConstructingContainer);

		return Ok(std::move(container));
	}

	[[nodiscard]] inline const auto& CurrentSystems() const noexcept
	{
		return mySharedCounter->Systems();
	}

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	struct SystemElement
	{
		using InjectFuncType = std::function<void(SystemRegistry&, DependencyInjector&, GenerateSystemIdType&)>;

		SystemTraits::SetList	TraitSet;
		InjectFuncType			InjectFunc;
	};

	template<typename TSystem, typename TBaseSystem, typename TCreateFunc, typename... TDependencies>
	inline void AddSystemHelper(TCreateFunc&& aCreateFunc, std::optional<SystemId> aFixedId, TupleWrapper<std::tuple<SharedReference<TDependencies>...>>&&)
	{
		mySystemList.emplace_back(
			SystemElement
			{
				.TraitSet = TSystem::template TraitSet::ToObject(),
				.InjectFunc = [createFunc = std::move(aCreateFunc), aFixedId, &sysCounter = mySharedCounter] (SystemRegistry& aSelf, DependencyInjector& aDI, GenerateSystemIdType& aGenSystemId)
				{
					auto construct = [&aSelf, createFunc = std::move(createFunc), aFixedId, &aGenSystemId, &sysCounter](SharedReference<std::decay_t<TDependencies>>... aDependencies)
					{
						auto onCreate = [aFixedId, &aGenSystemId, &aDependencies..., &sysCounter]()
						{
							SystemId id = aFixedId ? *aFixedId : aGenSystemId();

							sysCounter->AddSystem(id, static_cast<std::string>(GetSystemTypeName<TSystem>())); // FIXME
							(sysCounter->AddSystemDependency(id, aDependencies->Id()), ...);

							TSystem::InjectDependencies(std::make_tuple(std::move(aDependencies)...));
							SystemAbstracted::InjectId(id);
						};
						auto onDestroy = [weakSysCounter = std::weak_ptr(sysCounter)](TSystem& aSystem)
						{
							auto ptr = weakSysCounter.lock();

							if (!ptr)
								return;

							ptr->RemoveSystem(aSystem.Id());
						};

						auto ref = createFunc(onCreate, onDestroy);

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

	SystemContainer					myConstructingContainer;
	std::shared_ptr<SystemCounter>	mySharedCounter;

	std::vector<SystemElement> mySystemList;

};

inline static void WmLogStream(std::ostream& aStream, const SystemRegistry::SError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
