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

			auto&& dependency = std::apply([createFunc = std::move(createFunc)](auto... aResults) -> TType&
			{
				return CreateDependencyHelper<TType>(createFunc, aResults...);
			}, results);

			void* const dependencyPtr = (void*)&dependency;

			aThis.myDependencies.emplace(typeid(Key<std::decay_t<TType>>), dependencyPtr);

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

	std::unordered_map<std::type_index, CreateFunc>	myCreateFuncs;
	std::unordered_map<std::type_index, void*>		myDependencies;

	template<typename TType>
	Result<std::reference_wrapper<TType>, SError> GetDependency()
	{
		const auto& typeIndex = typeid(Key<std::decay_t<TType>>);

		const auto depIt = myDependencies.find(typeIndex);

		if (depIt != myDependencies.cend())
		{
			return Ok(std::ref(*((TType*)depIt->second)));
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
	static TDependency& CreateDependencyHelper(TCreateFunc&& aCreateFunc, Result<std::reference_wrapper<TDependencies>, SError>... aDependencies)
	{
		return std::move(aCreateFunc)(static_cast<TDependencies&>(aDependencies.Unwrap())...);
	}
};
