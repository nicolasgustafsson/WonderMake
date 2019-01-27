#pragma once
#include "System/SystemPtr.h"
#include "Object.h"
#include "FunctionalitySystem.h"
#include <type_traits>

struct SComponent;

template<typename TDependency>
class Dependency
{
public:
	Dependency();
	~Dependency();

	void Create(Object* aOwningObject);
	void Destroy(Object* aOwningObject);

	TDependency& operator*()
	{
		return *myDependency;
	}

	TDependency* operator->()
	{
		return myDependency;
	}

private:
	TDependency* myDependency;
};

template<typename TDependency>
Dependency<TDependency>::Dependency()
{
}

template<typename TDependency>
void Dependency<TDependency>::Destroy(Object* aOwningObject)
{
	if constexpr (std::is_base_of<BaseFunctionality, TDependency>::value)
		aOwningObject->RemoveFunctionality<TDependency>();
	else if (std::is_base_of<SComponent, TDependency>::value)
		aOwningObject->_RemoveComponent<TDependency>();
}

template<typename TDependency>
void Dependency<TDependency>::Create(Object* aOwningObject)
{
	if constexpr (std::is_base_of<BaseFunctionality, TDependency>::value)
		myDependency = aOwningObject->AddFunctionality<TDependency>();
	else if (std::is_base_of<SComponent, TDependency>::value)
		myDependency = aOwningObject->_AddComponent<TDependency>();
}

template<typename TDependency>
Dependency<TDependency>::~Dependency()
{
}

template<typename ... TDependencies>
class Dependencies
{
public:
	Dependencies(Object* aObject);

	void Create(Object* aObject);
	void Destroy(Object* aObject);

	template<typename TDependency>
	__forceinline TDependency& Get();

private:
	std::tuple<Dependency<TDependencies>...> myDependencies;
};

template<typename ... TDependencies>
template<typename TDependency>
TDependency& Dependencies<TDependencies...>::Get()
{
	return *std::get<Dependency<TDependency>>(myDependencies);
}

template<typename ... TDependencies>
void Dependencies<TDependencies...>::Create(Object* aObject)
{
	(std::get<Dependency<TDependencies>>(myDependencies).Create(aObject), ...);
}

template<typename ... TDependencies>
void Dependencies<TDependencies...>::Destroy(Object* aObject)
{
	(std::get<Dependency<TDependencies>>(myDependencies).Destroy(aObject), ...);
}

template<typename ... TDependencies>
Dependencies<TDependencies...>::Dependencies(Object* aObject)
{
	Create(aObject);
}
