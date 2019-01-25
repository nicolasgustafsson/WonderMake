#pragma once
#include "System/SystemPtr.h"
#include "Object.h"
#include "CapabilitySystem.h"

struct SComponent;

template<typename TCapability>
class CapabilityDependency
{
public:
	CapabilityDependency(Object* aOwningObject);
	~CapabilityDependency();

	TCapability& operator*()
	{
		return *myCapability;
	}

	TCapability* operator->()
	{
		return myCapability;
	}

private:
	TCapability* myCapability;
};

template<typename TCapability>
CapabilityDependency<TCapability>::~CapabilityDependency()
{
	SystemPtr<CapabilitySystem<TCapability>>()->RemoveCapability(myCapability);
}

template<typename TCapability>
CapabilityDependency<TCapability>::CapabilityDependency(Object* aOwningObject)
{
	static_assert(std::is_base_of<BaseCapability, TCapability>::value, "Capability must inherit from BaseCapability!");
	myCapability = aOwningObject->FindCapability<TCapability>();

	if (!myCapability)
	{
		myCapability = SystemPtr<CapabilitySystem<TCapability>>()->AddCapability(aOwningObject);
		aOwningObject->_AddCapability(myCapability);
	}
}
