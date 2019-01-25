#pragma once
#include "System/SystemPtr.h"
#include "Object.h"
#include "FunctionalitySystem.h"

struct SComponent;

template<typename TFunctionality>
class FunctionalityDependency
{
public:
	FunctionalityDependency(Object* aOwningObject);
	~FunctionalityDependency();

	TFunctionality& operator*()
	{
		return *myFunctionality;
	}

	TFunctionality* operator->()
	{
		return myFunctionality;
	}

private:
	TFunctionality* myFunctionality;
};

template<typename TFunctionality>
FunctionalityDependency<TFunctionality>::~FunctionalityDependency()
{
	SystemPtr<FunctionalitySystem<TFunctionality>>()->RemoveFunctionality(myFunctionality);
}

template<typename TFunctionality>
FunctionalityDependency<TFunctionality>::FunctionalityDependency(Object* aOwningObject)
{
	static_assert(std::is_base_of<BaseFunctionality, TFunctionality>::value, "Capability must inherit from BaseCapability!");
	myFunctionality = aOwningObject->FindFunctionality<TFunctionality>();

	if (!myFunctionality)
	{
		myFunctionality = SystemPtr<FunctionalitySystem<TFunctionality>>()->AddFunctionality(aOwningObject);
		aOwningObject->_AddFunctionality(myFunctionality);
	}
}
