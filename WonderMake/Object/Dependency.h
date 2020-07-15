#pragma once

#include "Object.h"

#include "Policies/Policy.h"

#include <type_traits>

struct SComponent;

template<typename TDependency>
class Dependency
{
public:
	Dependency() = default;
	~Dependency() = default;

	inline void Create(Object& aOwningObject);
	inline void Destroy(Object& aOwningObject, _BaseFunctionality& aFunctionality);

	__forceinline TDependency& operator*() const noexcept
	{
		return *myDependency;
	}

	__forceinline TDependency* operator->() const noexcept
	{
		return myDependency;
	}

private:
	TDependency* myDependency;
};

template<typename TDependency>
void Dependency<TDependency>::Create(Object& aOwningObject)
{
	myDependency = &aOwningObject.Add<TDependency>();
}

template<typename TDependency>
void Dependency<TDependency>::Destroy(Object& aOwningObject, [[maybe_unused]] _BaseFunctionality& aFunctionality)
{
	aOwningObject.Remove<TDependency>();
}

template<typename PolicySet>
class Dependencies
{
public:
	inline Dependencies(Object& aObject);

	inline void Create(Object& aObject);
	inline void Destroy(Object& aObject, _BaseFunctionality& aFunctionality);

	template<typename TDependency>
	constexpr __forceinline TDependency& Get() const;

private:
	using DependencyTuple = typename PolicySet::template ExtractDependencies<Dependency>;

	DependencyTuple myDependencies;
};

template<typename PolicySet>
void Dependencies<PolicySet>::Create(Object& aObject)
{
	const auto create = [&aObject](auto& aDependency) -> auto&
	{
		aDependency.Create(aObject);
		return aDependency;
	};

	std::apply([&create](auto& ...x) { auto t = std::make_tuple(create(x)...); t; }, myDependencies);
}

template<typename PolicySet>
void Dependencies<PolicySet>::Destroy(Object& aObject, _BaseFunctionality& aFunctionality)
{
	const auto destroy = [&aObject, &aFunctionality](auto& aDependency) -> auto &
	{
		aDependency.Destroy(aObject, aFunctionality);
		return aDependency;
	};

	std::apply([&destroy](auto& ...x) { auto t = std::make_tuple(destroy(x)...); t; }, myDependencies);
}

template<typename PolicySet>
template<typename TDependency>
constexpr TDependency& Dependencies<PolicySet>::Get() const
{
	return *std::get<Dependency<TDependency>>(myDependencies);
}

template<typename PolicySet>
Dependencies<PolicySet>::Dependencies(Object& aObject)
{
	Create(aObject);
}
