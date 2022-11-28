#pragma once

#include "wondermake-base/Object.h"
#include "wondermake-base/ObjectDependencyDestructor.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/plf_colony.h"

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
	plf::colony<TData>			myData;
	ObjectDependencyDestructor	myDependencyDestructor;
};

#define WM_REGISTER_COMPONENT_SYSTEM(aComponent) _WM_REGISTER_SYSTEM_IMPL(ComponentSystem<aComponent>, aComponent)

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
