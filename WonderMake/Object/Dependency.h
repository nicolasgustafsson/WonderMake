#pragma once
#include "System/SystemPtr.h"
#include "Object.h"
#include "Functionalities/FunctionalitySystem.h"
#include <type_traits>

struct SComponent;

template<typename TDependency>
class Dependency
{
public:
	Dependency() = default;
	~Dependency() = default;

	inline void Create(Object& aOwningObject);
	inline void Destroy(Object& aOwningObject, BaseFunctionality& aFunctionality);

	inline TDependency& operator*() const noexcept
	{
		return *myDependency;
	}

	inline TDependency* operator->() const noexcept
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

class ImpulseFunctionality;

template<typename TDependency>
void Dependency<TDependency>::Destroy(Object& aOwningObject, [[maybe_unused]] BaseFunctionality& aFunctionality)
{
	if constexpr (std::is_same_v<TDependency, ImpulseFunctionality>)
		myDependency->UnsubscribeAll(aFunctionality);

	aOwningObject.Remove<TDependency>();
}

template<typename ... TDependencies>
class Dependencies
{
public:
	inline Dependencies(Object& aObject);

	inline void Create(Object& aObject);
	inline void Destroy(Object& aObject, BaseFunctionality& aFunctionality);

	template<typename TDependency>
	constexpr __forceinline TDependency& Get() const;

private:
	std::tuple<Dependency<TDependencies>...> myDependencies;
};

template<typename ... TDependencies>
void Dependencies<TDependencies...>::Create(Object& aObject)
{
	(std::get<Dependency<TDependencies>>(myDependencies).Create(aObject), ...);
}

template<typename ... TDependencies>
void Dependencies<TDependencies...>::Destroy(Object& aObject, BaseFunctionality& aFunctionality)
{
	(std::get<Dependency<TDependencies>>(myDependencies).Destroy(aObject, aFunctionality), ...);
}

template<typename ... TDependencies>
template<typename TDependency>
constexpr TDependency& Dependencies<TDependencies...>::Get() const
{
	return *std::get<Dependency<TDependency>>(myDependencies);
}

template<typename ... TDependencies>
Dependencies<TDependencies...>::Dependencies(Object& aObject)
{
	Create(aObject);
}
