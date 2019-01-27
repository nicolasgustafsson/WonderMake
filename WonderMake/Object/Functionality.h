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
template<typename SelfType, typename ... TDependencies>
class Functionality
	:public BaseFunctionality
{
public:

	virtual void Destroy(Object* aObject) override final;

	using Super = Functionality<SelfType, TDependencies...>;

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
	friend SelfType;
};

template<typename SelfType, typename ... TDependencies>
void Functionality<SelfType, TDependencies...>::Destroy(Object* aObject)
{
	myDependencies.Destroy(aObject);
	SystemPtr<FunctionalitySystem<SelfType>>()->RemoveFunctionality(static_cast<SelfType*>(this));
}

template<typename SelfType, typename ... TDependencies>
Functionality<SelfType, TDependencies...>::Functionality(Object* aObject)
	: myDependencies(aObject)
{
}

