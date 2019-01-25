#pragma once
struct SComponent;
class BaseCapability;

class Object
{
public:
	Object();

	template<typename T>
	T* FindComponent() const;

	template<typename T>
	T* FindCapability() const;

	void _AddCapability(BaseCapability* aCapability);
	void _RemoveCapability(BaseCapability* aCapability);

	void _AddComponent(SComponent* aComponent);
	void _RemoveComponent(SComponent* aComponent);

private:
	std::vector<BaseCapability*> myCapabilities;
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

template<typename TCapability>
TCapability* Object::FindCapability() const
{
	static_assert(std::is_base_of<BaseCapability, TCapability>::value, "Component must inherit from SComponent!");

	for (BaseCapability* capability : myCapabilities)
	{
		TCapability* correctCapability = dynamic_cast<TCapability*>(capability);
		if (correctCapability)
			return correctCapability;
	}

	return nullptr;
}

