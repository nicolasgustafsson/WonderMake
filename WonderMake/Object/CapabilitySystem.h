#pragma once
#include "Utilities/plf_colony.h"
#include "System/System.h"

template<typename TCapability>
class CapabilitySystem
	:public System
{
public:
	CapabilitySystem();
	TCapability* AddCapability(Object* aObject);
	void RemoveCapability(TCapability* const aCapability);

	bool IsEmpty() const;

	virtual void Tick() override;

private:
	plf::colony<TCapability> myCapabilities;
};

template<typename TCapability>
void CapabilitySystem<TCapability>::Tick()
{
	for (auto&& capability : myCapabilities)
		capability.Tick();
}

template<typename TCapability>
CapabilitySystem<TCapability>::CapabilitySystem()
{
	if (typeid(&TCapability::Tick) != typeid(&BaseCapability::Tick))
		EnableTick();
}

template<typename TCapability>
bool CapabilitySystem<TCapability>::IsEmpty() const
{
	return myCapabilities.size() == 0;
}

template<typename TCapability>
void CapabilitySystem<TCapability>::RemoveCapability(TCapability* const aCapability)
{
	myCapabilities.erase(myCapabilities.get_iterator_from_pointer(aCapability));
}

template<typename TCapability>
typename TCapability* CapabilitySystem<TCapability>::AddCapability(Object* aObject)
{
	return &(*myCapabilities.emplace(aObject));
}

