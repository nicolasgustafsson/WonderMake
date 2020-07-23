#pragma once
#include "Utilities/plf_colony.h"
#include "System/System.h"

template<typename TData>
class ComponentSystem
	: public System<>
{
public:
	using Super::Super;

	TData& AddComponent();
	void RemoveComponent(TData& aComponent);

	bool IsEmpty() const;

private:
	plf::colony<TData> myData;
};

#define REGISTER_COMPONENT_SYSTEM(aComponent) _REGISTER_SYSTEM_IMPL(ComponentSystem<aComponent>, aComponent)

template<typename TData>
bool ComponentSystem<TData>::IsEmpty() const
{
	return myData.empty();
}

template<typename TData>
void ComponentSystem<TData>::RemoveComponent(TData& aComponent)
{
	myData.erase(myData.get_iterator_from_pointer(&aComponent));
}

template<typename TData>
typename TData& ComponentSystem<TData>::AddComponent()
{
	return (*myData.emplace());
}

