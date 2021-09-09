#pragma once

class Object;

class ObjectInterfaceFunctionality : public Functionality<>
{
public:
	void OnNewObject(Object& aObjectAddedTo) override;

	template<typename TFunctionalityOrComponent>
	TFunctionalityOrComponent* Find();

private:
	Object* myObject{};
};

template <typename TFunctionalityOrComponent>
TFunctionalityOrComponent* ObjectInterfaceFunctionality::Find()
{
	TFunctionalityOrComponent* functionalityOrComponent = nullptr;
	myObject->Visit([&functionalityOrComponent](auto&&, auto&& aFunctionality) mutable
		{
			if (typeid(*aFunctionality) == typeid(TFunctionalityOrComponent))
				functionalityOrComponent = reinterpret_cast<TFunctionalityOrComponent*>(aFunctionality);
		});

	return functionalityOrComponent;
}

