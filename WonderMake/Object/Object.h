#pragma once
#include "Typedefs.h"
#include "Functionalities/FunctionalitySystem.h"
#include "System/SystemPtr.h"
#include "Components/ComponentSystem.h"

struct SComponent;
class BaseFunctionality;

struct SFunctionalityCounter
{
	SFunctionalityCounter(BaseFunctionality* aFunctionality) noexcept
		: Functionality(aFunctionality)
		, RefCount(1)
	{

	}

	BaseFunctionality* Functionality;
	i32 RefCount = 0;
};

struct SComponentCounter
{
	SComponentCounter(SComponent* aComponent) noexcept
		: Component(aComponent)
		, RefCount(1)
	{

	}
	SComponent* Component;
	i32 RefCount = 0;
};

class Object
{
public:
	Object() = default;
	virtual ~Object();

	//[Nicos]: Will not add functionality if it already exists.
	template<typename TFunctionality>
	inline TFunctionality& AddFunctionality();

	//[Nicos]: Will not remove functionality if other functionalities depend on it.
	template<typename TFunctionality>
	inline void RemoveFunctionality();

	//[Nicos]: Will not add component if it already exists.
	template<typename TComponent>
	inline TComponent& _AddComponent();

	//[Nicos]: Will not remove component if existing functionalities depend on it.
	template<typename TComponent>
	inline void _RemoveComponent();

private:
	std::vector<SFunctionalityCounter> myFunctionalities;
	std::vector<SComponentCounter> myComponents;
};

template<typename TFunctionality>
TFunctionality& Object::AddFunctionality()
{
	static_assert(std::is_base_of<BaseFunctionality, TFunctionality>::value, "Functionality must inherit from BaseFunctionality!");

	for (SFunctionalityCounter& counter : myFunctionalities)
	{
		TFunctionality* functionality = dynamic_cast<TFunctionality*>(counter.Functionality);
		if (functionality)
		{
			counter.RefCount++;
			return *functionality;
		}
	}
	auto& addedFunctionality = SystemPtr<FunctionalitySystem<TFunctionality>>()->AddFunctionality(*this);
	myFunctionalities.emplace_back(&addedFunctionality);
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
			assert(counter.RefCount != 0);
			counter.RefCount--;

			if (counter.RefCount <= 0)
			{
				BaseFunctionality* functionality = counter.Functionality;
				myFunctionalities.erase(myFunctionalities.begin() + i);
				functionality->Destroy(*this);
			}

			return;
		}
	}
}

template<typename TComponent>
TComponent& Object::_AddComponent()
{
	static_assert(std::is_base_of<SComponent, TComponent>::value, "Component must inherit from SComponent!");

	for (SComponentCounter& counter : myComponents)
	{
		TComponent* component = dynamic_cast<TComponent*>(counter.Component);
		if (component)
		{
			counter.RefCount++;
			return *component;
		}
	}
	auto& addedComponent = SystemPtr<ComponentSystem<TComponent>>()->AddComponent();
	myComponents.emplace_back(&addedComponent);
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
			assert(counter.RefCount != 0);
			counter.RefCount--;

			if (counter.RefCount <= 0)
			{
				SystemPtr<ComponentSystem<TComponent>>()->RemoveComponent(static_cast<TComponent&>(*counter.Component));
				myComponents.erase(myComponents.begin() + i);
			}

			return;
		}
	}
}

