#pragma once
struct SComponent;
class BaseFunctionality;

//TODO:
//HE HAS DONE IT change capability to functionality(in name) 
//allow functionalities to be added dynamically
//fix capabilities removing components regarding of other capabilities when removed(ref count?)
//change comparison when finding functionalities/components to not use dynamic cast(typeid?)
//minimize or remove boilerplate of calling dependency constructor with object pointer(base class with templates?)
//make debugging visitor to allow arbitrary debugging of objects, functionalities and components
//Allow adding components in runtime via imgui

class Object
{
public:
	Object();

	template<typename T>
	T* FindComponent() const;

	template<typename T>
	T* FindFunctionality() const;

	void _AddFunctionality(BaseFunctionality* aFunctionality);
	void _RemoveCapability(BaseFunctionality* aFunctionality);

	void _AddComponent(SComponent* aComponent);
	void _RemoveComponent(SComponent* aComponent);

private:
	std::vector<BaseFunctionality*> myFunctionalities;
	std::vector<SComponent*> myComponents;
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
TFunctionality* Object::FindFunctionality() const
{
	static_assert(std::is_base_of<BaseFunctionality, TFunctionality>::value, "Functionality must inherit from BaseFunctionality!");

	for (BaseFunctionality* functionality : myFunctionalities)
	{
		TFunctionality* correctFunctionality = dynamic_cast<TFunctionality*>(functionality);
		if (correctFunctionality)
			return correctFunctionality;
	}

	return nullptr;
}

