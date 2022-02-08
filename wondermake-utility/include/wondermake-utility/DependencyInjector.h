#pragma once

#include "Result.h"
#include "UniqueFunction.h"
#include "Utility.h"

#include <functional>
#include <optional>
#include <typeindex>
#include <unordered_map>

class DependencyInjector
{
public:
	enum class ECreateError
	{
		MissingDependency
	};

	template<typename TType, typename TCreateFunc, typename... TDependencies>
	inline void Add(TCreateFunc aCreateFunc)
	{	
		myCreateFuncs.emplace(typeid(Key<std::decay_t<TType>>), [createFunc = std::move(aCreateFunc)]([[maybe_unused]] DependencyInjector& aThis) mutable -> Result<ECreateError, void*, std::string>
		{
			std::optional<Result<ECreateError, void*, std::string>> errResult;

			auto parseResult = [&errResult](auto aResult)
			{
				if (!aResult)
					errResult = { static_cast<ECreateError>(aResult), std::move(aResult.Meta()) };

				return aResult;
			};

			auto results = std::make_tuple(parseResult(aThis.GetDependency<TDependencies>())...);

			if (errResult)
				return *errResult;

			auto&& dependency = std::apply([createFunc = std::move(createFunc)](auto... aResults) -> TType&
			{
				return CreateDependencyHelper<TType>(createFunc, aResults...);
			}, results);

			void* const dependencyPtr = (void*)&dependency;

			aThis.myDependencies.emplace(typeid(Key<std::decay_t<TType>>), dependencyPtr);

			return dependencyPtr;
		});
	}

	template<typename TType>
	inline Result<ECreateError, std::reference_wrapper<TType>, std::string> Get()
	{
		return GetDependency<TType>();
	}

	Result<ECreateError, decltype(Success), std::string> CreateAll();

private:
	template<typename TDependency>
	struct Key {};

	using CreateFunc = UniqueFunction<Result<ECreateError, void*, std::string> (DependencyInjector&)>;

	std::unordered_map<std::type_index, CreateFunc>	myCreateFuncs;
	std::unordered_map<std::type_index, void*>		myDependencies;

	template<typename TType>
	Result<ECreateError, std::reference_wrapper<TType>, std::string> GetDependency()
	{
		const auto& typeIndex = typeid(Key<std::decay_t<TType>>);

		const auto depIt = myDependencies.find(typeIndex);

		if (depIt != myDependencies.cend())
		{
			return std::ref(*((TType*)depIt->second));
		}

		const auto createIt = myCreateFuncs.find(typeIndex);

		if (createIt == myCreateFuncs.cend())
		{
			return { ECreateError::MissingDependency, typeIndex.name() };
		}

		auto result = std::move(createIt->second)(*this);

		if (result)
			return std::ref(*((TType*)static_cast<void*>(result)));

		return { static_cast<ECreateError>(result), result.Meta() };
	}

	template<typename TDependency, typename TCreateFunc, typename... TDependencies>
	static TDependency& CreateDependencyHelper(TCreateFunc&& aCreateFunc, Result<ECreateError, std::reference_wrapper<TDependencies>, std::string>... aDependencies)
	{
		return std::move(aCreateFunc)(static_cast<TDependencies&>(static_cast<std::reference_wrapper<TDependencies>>(aDependencies))...);
	}
};