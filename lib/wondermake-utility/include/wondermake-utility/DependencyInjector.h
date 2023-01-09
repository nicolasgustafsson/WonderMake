#pragma once

#include "Result.h"
#include "UniqueFunction.h"
#include "Utility.h"

#include <any>
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

	struct SError
	{
		ECreateError	Error = ECreateError::MissingDependency;
		std::string		Reason;
	};

	template<typename TType, typename TCreateFunc, typename... TDependencies>
	inline void Add(TCreateFunc aCreateFunc)
	{	
		myCreateFuncs.emplace(typeid(Key<std::decay_t<TType>>), [createFunc = std::move(aCreateFunc)]([[maybe_unused]] DependencyInjector& aThis) mutable -> Result<void*, SError>
		{
			std::optional<Result<void*, SError>> errResult;

			auto parseResult = [&errResult](auto aResult)
			{
				if (!aResult)
					errResult = aResult;

				return aResult;
			};

			auto results = std::make_tuple(parseResult(aThis.GetDependency<TDependencies>())...);

			if (errResult)
				return *errResult;

			SDependency dependency = std::apply([createFunc = std::move(createFunc)](auto... aResults) -> SDependency
			{
				return CreateDependencyHelper<TType>(createFunc, aResults...);
			}, results);

			void* const dependencyPtr = dependency.DataPtr;

			aThis.myDependencies.emplace(typeid(Key<std::decay_t<TType>>), std::move(dependency));

			return Ok(dependencyPtr);
		});
	}

	template<typename TType>
	inline Result<std::reference_wrapper<TType>, SError> Get()
	{
		return GetDependency<TType>();
	}

	Result<void, SError> CreateAll();

private:
	template<typename TDependency>
	struct Key {};

	using CreateFunc = UniqueFunction<Result<void*, SError> (DependencyInjector&)>;
	struct SDependency
	{
		std::any					Data;
		void*						DataPtr;
	};

	std::unordered_map<std::type_index, CreateFunc>		myCreateFuncs;
	std::unordered_map<std::type_index, SDependency>	myDependencies;

	template<typename TType>
	Result<std::reference_wrapper<TType>, SError> GetDependency()
	{
		const auto& typeIndex = typeid(Key<std::decay_t<TType>>);

		const auto depIt = myDependencies.find(typeIndex);

		if (depIt != myDependencies.cend())
		{
			return Ok(std::ref(*((TType*)depIt->second.DataPtr)));
		}

		const auto createIt = myCreateFuncs.find(typeIndex);

		if (createIt == myCreateFuncs.cend())
		{
			return Err(SError{ ECreateError::MissingDependency, typeIndex.name() });
		}

		auto result = std::move(createIt->second)(*this);

		if (result)
			return Ok(std::ref(*((TType*)result.Unwrap())));

		return result;
	}

	template<typename TDependency, typename TCreateFunc, typename... TDependencies>
	static SDependency CreateDependencyHelper(TCreateFunc&& aCreateFunc, Result<std::reference_wrapper<TDependencies>, SError>... aDependencies)
	{
		static constexpr bool owned = !std::is_reference_v<std::invoke_result_t<decltype(std::move(aCreateFunc)), decltype(static_cast<TDependencies&>(aDependencies.Unwrap()))...>>;

		if constexpr (owned)
		{
			auto ptr = std::make_shared<TDependency>(std::move(aCreateFunc)(static_cast<TDependencies&>(aDependencies.Unwrap())...));

			return SDependency
			{
				.Data		= ptr,
				.DataPtr	= ptr.get(),
			};
		}
		else
			return SDependency
			{
				.DataPtr	= &std::move(aCreateFunc)(static_cast<TDependencies&>(aDependencies.Unwrap())...),
			};
	}
};
