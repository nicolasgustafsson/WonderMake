#pragma once
#include "System/SystemPtr.h"
#include "ComponentSystem.h"
#include "Object.h"

struct SComponent;

template<typename TComponent>
class ComponentDependency
{
public:
	ComponentDependency(Object* aOwningObject);
	~ComponentDependency();

	TComponent& operator*()
	{
		return *myComponent;
	}

	TComponent* operator->()
	{
		return myComponent;
	}

private:
	TComponent* myComponent;
};

template<typename TComponent>
ComponentDependency<TComponent>::~ComponentDependency()
{
	SystemPtr<ComponentSystem<TComponent>>()->RemoveComponent(myComponent);
}

template<typename TComponent>
ComponentDependency<TComponent>::ComponentDependency(Object* aOwningObject)
{
	static_assert(std::is_base_of<SComponent, TComponent>::value, "Component must inherit from SComponent!");
	myComponent = aOwningObject->FindComponent<TComponent>();

	if (!myComponent)
	{
		myComponent = SystemPtr<ComponentSystem<TComponent>>()->AddComponent();
		aOwningObject->_AddComponent(myComponent);
	}
}

