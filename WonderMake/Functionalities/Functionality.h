#pragma once
#include "Object/Dependency.h"
#include "Functionalities/BaseFunctionality.h"

class Object;

//[Nicos]: Describes a functionality for an object. Template params are 1. Type that is self type and 2. Dependencies.
template<
	typename TSelfType,
	typename TPolicySet = Policy::Set<>>
class Functionality
	: public _BaseFunctionality
{
public:
	inline virtual void Destroy(Object& aObject) override final;

	using Super = Functionality<TSelfType, TPolicySet>;
	using PolicySet = TPolicySet;

	template<typename TDependency>
	constexpr __forceinline TDependency& Get() const
	{
		static_assert(TPolicySet::template HasDependency_v<TDependency>, "Functionality::Get: type is not a dependency listed in your functionality declaration!");

		return myDependencies.Get<TDependency>();
	}

protected:
	Dependencies<TPolicySet> myDependencies;

private:
	Functionality(Object& aObject);
	friend TSelfType;
};

template<typename TSelfType, typename TPolicySet>
void Functionality<TSelfType, TPolicySet>::Destroy(Object& aObject)
{
	myDependencies.Destroy(aObject, *this);
	SystemPtr<FunctionalitySystem<TSelfType>>()->RemoveFunctionality(static_cast<TSelfType&>(*this));
}

template<typename TSelfType, typename TPolicySet>
Functionality<TSelfType, TPolicySet>::Functionality(Object& aObject)
	: myDependencies(aObject)
{}

