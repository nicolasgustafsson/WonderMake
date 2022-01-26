#pragma once

#include "Object/DependencyDestructor.h"
#include "Object/Object.h"

#include "WonderMakeEngine/System.h"

#include "WonderMakeUtility/plf_colony.h"

class Object;

template<typename TData>
class ComponentSystem
	: public System<>
{
public:
	ComponentSystem();

	TData& AddComponent(Object& aObject, const bool aExplicitlyAdded = true);
	void RemoveComponent(TData& aComponent);

	bool IsEmpty() const;

private:
	plf::colony<TData>		myData;
	DependencyDestructor	myDependencyDestructor;
};

#define REGISTER_COMPONENT_SYSTEM(aComponent) _REGISTER_SYSTEM_IMPL(ComponentSystem<aComponent>, aComponent)

template<typename TData>
ComponentSystem<TData>::ComponentSystem()
	: myDependencyDestructor([this](Object& /*aObject*/, auto* aComponent)
		{
			RemoveComponent(*static_cast<TData*>(aComponent));
		})
{}

template<typename TData>
typename TData& ComponentSystem<TData>::AddComponent(Object& aObject, const bool aExplicitlyAdded)
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
