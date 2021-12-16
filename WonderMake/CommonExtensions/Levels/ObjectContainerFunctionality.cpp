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

BaseObject& ObjectContainerFunctionality::AddDenizen(BaseObject&& aObject)
{
	 auto&& denizen = Get<SObjectContainerComponent>().Denizens.emplace(std::move(aObject), Get<FunctionalitySystemDelegate<SubobjectFunctionality>>().AddFunctionality(aObject));

	 denizen->DenizenFunctionality.Get<SSubobjectComponent>().Level = this;

	 return denizen->DenizenObject;
}

BaseObject& ObjectContainerFunctionality::AddDenizen()
{
	return AddDenizen({});
}

void ObjectContainerFunctionality::RemoveDenizen(BaseObject& aObject)
{
	auto& denizens = Get<SObjectContainerComponent>().Denizens;

	BaseObject* objectPtr = &aObject;

	auto it = std::find_if(denizens.begin(), denizens.end(), [objectPtr](SObjectContainerComponent::SDenizen& aDenizen) { return &aDenizen.DenizenObject == objectPtr; });

	if (it != denizens.end())
		denizens.erase(it);
}

void ObjectContainerFunctionality::AddDenizens(plf::colony<BaseObject>&& aObjects)
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
