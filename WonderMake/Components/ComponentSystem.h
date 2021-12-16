#pragma once

#include "Object/DependencyDestructor.h"
#include "Object/Object.h"

#include "System/System.h"

#include "Utilities/plf_colony.h"

class BaseObject;

template<typename TData>
class ComponentSystem
	: public System<>
{
public:
	ComponentSystem();

	TData& AddComponent(BaseObject& aObject, const bool aExplicitlyAdded = true);
	void RemoveComponent(TData& aComponent);

	bool IsEmpty() const;

private:
	plf::colony<TData>		myData;
	DependencyDestructor	myDependencyDestructor;
};

#define REGISTER_COMPONENT_SYSTEM(aComponent) _REGISTER_SYSTEM_IMPL(ComponentSystem<aComponent>, aComponent)

template<typename TData>
ComponentSystem<TData>::ComponentSystem()
	: myDependencyDestructor([this](BaseObject& /*aObject*/, auto* aComponent)
		{
			RemoveComponent(*static_cast<TData*>(aComponent));
		})
{}

template<typename TData>
TData& ComponentSystem<TData>::AddComponent(BaseObject& aObject, const bool aExplicitlyAdded)
{
	return aObject.Add<TData>([this](auto& /*aObject*/) -> auto&
		{
			return *myData.emplace();
		}, myDependencyDestructor, aExplicitlyAdded);
}

template<typename TData>
void ComponentSystem<TData>::RemoveComponent(TData& aComponent)
{
	myData.erase(myData.get_iterator_from_pointer(&aComponent));
}

template<typename TData>
bool ComponentSystem<TData>::IsEmpty() const
{
	return myData.empty();
}
