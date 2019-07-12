#pragma once
#include "Object/Dependency.h"

class Object;
class BaseFunctionality 
	: public NonCopyable
{
public:
	BaseFunctionality() = default;

	virtual ~BaseFunctionality() = default;

	virtual void Destroy([[maybe_unused]]Object& aObject) = 0;

	void Tick() noexcept {}
};

//[Nicos]: Describes a functionality for an object. Template params are 1. Type that is self type and 2. Dependencies.
template<typename TSelfType, typename ... TDependencies>
class Functionality
	: public BaseFunctionality
{
public:
	inline virtual void Destroy(Object& aObject) override final;

	using Super = Functionality<TSelfType, TDependencies...>;

	template<typename TDependency>
	__forceinline TDependency& Get() const
	{
		static_assert((std::is_same_v<TDependency, TDependencies> || ...), "Functionality::Get: type is not a dependency listed in your functionality declaration!");

		return myDependencies.Get<TDependency>();
	}

protected:
	Dependencies<TDependencies...> myDependencies;

private:
	Functionality(Object& aObject);
	friend TSelfType;
};

template<typename TSelfType, typename ... TDependencies>
void Functionality<TSelfType, TDependencies...>::Destroy(Object& aObject)
{
	myDependencies.Destroy(aObject);
	SystemPtr<FunctionalitySystem<TSelfType>>()->RemoveFunctionality(static_cast<TSelfType&>(*this));
}

template<typename TSelfType, typename ... TDependencies>
Functionality<TSelfType, TDependencies...>::Functionality(Object& aObject)
	: myDependencies(aObject)
{}

