#pragma once
#include "Dependency.h"

class Object;
class BaseFunctionality : public NonCopyable
{
public:
	BaseFunctionality()
	{
	}
	virtual ~BaseFunctionality()
	{
	}

	virtual void Destroy([[maybe_unused]]Object* aObject) {}

	void Tick() {}
};

//describes a functionality for an object. template params are the type that is derived and then dependencies
template<typename TSelfType, typename ... TDependencies>
class Functionality
	:public BaseFunctionality
{
public:

	virtual void Destroy(Object* aObject) override final;

	using Super = Functionality<TSelfType, TDependencies...>;

	template<typename TDependency>
	__forceinline TDependency& Get()
	{
		if constexpr ((std::is_same_v<TDependency, TDependencies> || ...))
		{
			return myDependencies.Get<TDependency>();
		}
		else
		{
			static_assert(false, "Functionality::Get: type is not a dependency listed in your functionality declaration!");
		}
	}

protected:
	Dependencies<TDependencies...> myDependencies;

private:
	Functionality(Object* aObject);
	friend TSelfType;
};

template<typename TSelfType, typename ... TDependencies>
void Functionality<TSelfType, TDependencies...>::Destroy(Object* aObject)
{
	myDependencies.Destroy(aObject);
	SystemPtr<FunctionalitySystem<TSelfType>>()->RemoveFunctionality(static_cast<TSelfType*>(this));
}

template<typename TSelfType, typename ... TDependencies>
Functionality<TSelfType, TDependencies...>::Functionality(Object* aObject)
	: myDependencies(aObject)
{
}

