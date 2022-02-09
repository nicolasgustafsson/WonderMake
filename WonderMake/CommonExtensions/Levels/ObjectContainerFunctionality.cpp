#include "pch.h"
#include "ObjectContainerFunctionality.h"
#include "SubobjectFunctionality.h"

REGISTER_COMPONENT(SObjectContainerComponent);
REGISTER_FUNCTIONALITY(ObjectContainerFunctionality);

ObjectContainerFunctionality::~ObjectContainerFunctionality()
{
	for (auto&& denizen : Get<SObjectContainerComponent>().Denizens)
	{
		denizen.DenizenFunctionality.Get<SSubobjectComponent>().Level = nullptr;
	}
}

void ObjectContainerFunctionality::Tick()
{
	auto&& denizens = Get<SObjectContainerComponent>().Denizens;
	
	auto&& it = denizens.begin();
	
	while (it != denizens.end())
	{
		if (it->DenizenFunctionality.Get<SSubobjectComponent>().SlatedForRemoval)
			it = denizens.erase(it);
		else
			it++;
	}
}

Object& ObjectContainerFunctionality::AddDenizen(Object&& aObject)
{
	 auto&& denizen = Get<SObjectContainerComponent>().Denizens.emplace(std::move(aObject), Get<FunctionalitySystemDelegate<SubobjectFunctionality>>().AddFunctionality(aObject));

	 denizen->DenizenFunctionality.Get<SSubobjectComponent>().Level = this;

	 return denizen->DenizenObject;
}

Object& ObjectContainerFunctionality::AddDenizen()
{
	return AddDenizen({});
}

void ObjectContainerFunctionality::RemoveDenizen(Object& aObject)
{
	auto& denizens = Get<SObjectContainerComponent>().Denizens;

	Object* objectPtr = &aObject;

	auto it = std::find_if(denizens.begin(), denizens.end(), [objectPtr](SObjectContainerComponent::SDenizen& aDenizen) { return &aDenizen.DenizenObject == objectPtr; });

	if (it != denizens.end())
		denizens.erase(it);
}

void ObjectContainerFunctionality::AddDenizens(plf::colony<Object>&& aObjects)
{
	for (auto&& object : aObjects)
		AddDenizen(std::move(object));

	aObjects.clear();
}

void ObjectContainerFunctionality::TransferToOtherContainer(ObjectContainerFunctionality& aNewLevel)
{
	for (auto&& denizen : Get<SObjectContainerComponent>().Denizens)
	{
		if (denizen.DenizenFunctionality.Get<SSubobjectComponent>().PersistentOnLevelChange)
			aNewLevel.AddDenizen(std::move(denizen.DenizenObject));
		else
			denizen.DenizenFunctionality.Get<SSubobjectComponent>().Level = nullptr;
	}

	Get<SObjectContainerComponent>().Denizens.clear();
}
