#pragma once
#include <Typedefs.h>
#include "FunctionalitySystem.h"
#include <System/SystemPtr.h>
#include "ComponentSystem.h"
struct SComponent;
class BaseFunctionality;

struct SFunctionalityCounter
{
	BaseFunctionality* Functionality;
	i32 RefCount = 0;
};

struct SComponentCounter
{
	SComponent* Component;
	i32 RefCount = 0;
};

class Object
{
public:
	Object();
	virtual ~Object();

	template<typename TComponent>
	TComponent* FindComponent() const;

	//will not add functionality if it already exists
	template<typename TFunctionality>
	TFunctionality* AddFunctionality();

	//will not remove functionality if other functionalities depend on it
	template<typename TFunctionality>
	void RemoveFunctionality();

	//will not add component if it already exists
	template<typename TComponent>
	TComponent* _AddComponent();

	//will not remove component if existing functionalities depend on it
	template<typename TComponent>
	void _RemoveComponent();

private:
	std::vector<SFunctionalityCounter> myFunctionalities;
	std::vector<SComponentCounter> myComponents;
};

template<typename TComponent>
TComponent* Object::FindComponent() const
{
	static_assert(std::is_base_of<SComponent, TComponent>::value, "Component must inherit from SComponent!");

	for (SComponent* component : myComponents)
	{
		TComponent* correctComponent = dynamic_cast<TComponent*>(component);
		if (correctComponent)
			return correctComponent;
	}

	return nullptr;
}

template<typename TFunctionality>
TFunctionality* Object::AddFunctionality()
{
	static_assert(std::is_base_of<BaseFunctionality, TFunctionality>::value, "Functionality must inherit from BaseFunctionality!");

	for (SFunctionalityCounter& counter : myFunctionalities)
	{
		bool correctFunctionality = dynamic_cast<TFunctionality*>(counter.Functionality) != nullptr;
		if (correctFunctionality)
		{
			counter.RefCount++;
			return static_cast<TFunctionality*>(counter.Functionality);
		}
	}
	auto addedFunctionality = SystemPtr<FunctionalitySystem<TFunctionality>>()->AddFunctionality(this);
	myFunctionalities.push_back({ addedFunctionality, 1 });
	return addedFunctionality;
}

template<typename TFunctionality>
void Object::RemoveFunctionality()
{
	static_assert(std::is_base_of<BaseFunctionality, TFunctionality>::value, "Functionality must inherit from BaseFunctionality!");

	for (auto[i, counter] : Utility::Enumerate(myFunctionalities))
	{
		bool correctFunctionality = dynamic_cast<TFunctionality*>(counter.Functionality) != nullptr;
		if (correctFunctionality)
		{
			counter.RefCount--;

			if (counter.RefCount <= 0)
			{
				counter.Functionality->Destroy(this);
				myFunctionalities.erase(myFunctionalities.begin() + i);
			}

			return;
		}
	}
}

template<typename TComponent>
TComponent* Object::_AddComponent()
{
	static_assert(std::is_base_of<SComponent, TComponent>::value, "Component must inherit from SComponent!");

	for (SComponentCounter& counter : myComponents)
	{
		bool correctComponent = dynamic_cast<TComponent*>(counter.Component) != nullptr;
		if (correctComponent)
		{
			counter.RefCount++;
			return static_cast<TComponent*>(counter.Component);
		}
	}
	auto addedComponent = SystemPtr<ComponentSystem<TComponent>>()->AddComponent();
	myComponents.push_back({ addedComponent, 1 });
	return addedComponent;
}

template<typename TComponent>
void Object::_RemoveComponent()
{
	static_assert(std::is_base_of<SComponent, TComponent>::value, "Component must inherit from SComponent!");

	for (auto[i, counter] : Utility::Enumerate(myComponents))
	{
		bool correctComponent = dynamic_cast<TComponent*>(counter.Component) != nullptr;
		if (correctComponent)
		{
			counter.RefCount--;

			if (counter.RefCount <= 0)
			{
				SystemPtr<ComponentSystem<TComponent>>()->RemoveComponent(static_cast<TComponent*>(counter.Component));
				myComponents.erase(myComponents.begin() + i);
			}

			return;
		}
	}
}

