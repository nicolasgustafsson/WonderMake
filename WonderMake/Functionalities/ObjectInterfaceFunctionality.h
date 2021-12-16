#pragma once

class BaseObject;

class ObjectInterfaceFunctionality : public Functionality<>
{
public:
	void OnNewObject(BaseObject& aObjectAddedTo) override;

	template<typename TFunctionalityOrComponent>
	TFunctionalityOrComponent* Find();

private:
	BaseObject* myObject{};
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

