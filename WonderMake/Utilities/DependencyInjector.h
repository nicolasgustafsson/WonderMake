#pragma once

#include <iostream>
#include "Utilities/UniqueFunction.h"

#include <typeindex>
#include <unordered_map>

class DependencyInjector
{
public:
	class MissingDependencyException
		: public std::exception
	{
	public:
		MissingDependencyException(const char* const aType) noexcept
			: myMissingType(aType)
		{}

		const char* const myMissingType;
	};

	template<typename TType, typename TCreateFunc, typename... TDependencies>
	inline void Add(TCreateFunc aCreateFunc);

	// Note: this function may throw MissingDependencyException if the type or one of its dependencies were not added to the DependencyInjector. No guarantees are given on which dependencies were constructed if this is the case.
	template<typename TType>
	inline TType& Get();

	// Note: this function may throw MissingDependencyException if a dependency is missing, if it throws. No guarantees are given on which dependencies were constructed if this is the case.
	void CreateAll();

private:
	template<typename TDependency>
	struct Key {};

	using CreateFunc = UniqueFunction<void* (DependencyInjector&)>;

	std::unordered_map<std::type_index, CreateFunc>	myCreateFuncs;
	std::unordered_map<std::type_index, void*>		myDependencies;

	template<typename TType>
	TType& GetDependency();
};

template<typename TType, typename TCreateFunc, typename... TDependencies>
inline void DependencyInjector::Add(TCreateFunc aCreateFunc)
{
	myCreateFuncs.emplace(typeid(DependencyInjector::Key<std::decay_t<TType>>), [createFunc = std::move(aCreateFunc)]([[maybe_unused]] DependencyInjector& aThis)
		{
			void* const dependency = (void*)&createFunc(aThis.GetDependency<TDependencies>()...);

			aThis.myDependencies.emplace(typeid(Key<std::decay_t<TType>>), dependency);

			return dependency;
		});
}

template<typename TType>
inline TType& DependencyInjector::Get()
{
	return GetDependency<TType>();
}

template<typename TType>
TType& DependencyInjector::GetDependency()
{
	const auto& typeIndex = typeid(DependencyInjector::Key<std::decay_t<TType>>);

	const auto depIt = myDependencies.find(typeIndex);

	if (depIt != myDependencies.cend())
	{
		return *((TType*)depIt->second);
	}

	const auto createIt = myCreateFuncs.find(typeIndex);

	if (createIt == myCreateFuncs.cend())
    {
        std::string bla = typeIndex.name();
        std::cout << bla;
		assert(false);
	}

	return *((TType*)createIt->second(*this));
}