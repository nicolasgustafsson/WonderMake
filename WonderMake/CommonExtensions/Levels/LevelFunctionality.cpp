#include "pch.h"
#include "LevelFunctionality.h"
#include "LevelDenizenFunctionality.h"

REGISTER_COMPONENT(SLevelComponent);
REGISTER_FUNCTIONALITY(LevelFunctionality);

LevelFunctionality::~LevelFunctionality()
{
	for (auto&& denizen : Get<SLevelComponent>().Denizens)
	{
		denizen.DenizenFunctionality.Get<SLevelDenizenComponent>().Level = nullptr;
	}
}

void LevelFunctionality::Tick()
{
	auto&& denizens = Get<SLevelComponent>().Denizens;
	
	auto&& it = denizens.begin();
	
	while (it != denizens.end())
	{
		if (it->DenizenFunctionality.Get<SLevelDenizenComponent>().SlatedForRemoval)
			it = denizens.erase(it);
		else
			it++;
	}
}

Object& LevelFunctionality::AddDenizen(Object&& aObject)
{
	 auto&& denizen = Get<SLevelComponent>().Denizens.emplace(std::move(aObject), Get<FunctionalitySystemDelegate<LevelDenizenFunctionality>>().AddFunctionality(aObject));

	 denizen->DenizenFunctionality.Get<SLevelDenizenComponent>().Level = this;

	 return denizen->DenizenObject;
}

Object& LevelFunctionality::AddDenizen()
{
	return AddDenizen({});
}

void LevelFunctionality::RemoveDenizen(Object& aObject)
{
	auto& denizens = Get<SLevelComponent>().Denizens;

	Object* objectPtr = &aObject;

	auto it = std::find_if(denizens.begin(), denizens.end(), [objectPtr](SLevelComponent::SDenizen& aDenizen) { return &aDenizen.DenizenObject == objectPtr; });

	if (it != denizens.end())
		denizens.erase(it);
}

void LevelFunctionality::AddDenizens(plf::colony<Object>&& aObjects)
{
	for (auto&& object : aObjects)
		AddDenizen(std::move(object));

	aObjects.clear();
}

void LevelFunctionality::TransferToNewLevel(LevelFunctionality& aNewLevel)
{
	for (auto&& denizen : Get<SLevelComponent>().Denizens)
	{
		if (denizen.DenizenFunctionality.Get<SLevelDenizenComponent>().PersistentOnLevelChange)
			aNewLevel.AddDenizen(std::move(denizen.DenizenObject));
		else
			denizen.DenizenFunctionality.Get<SLevelDenizenComponent>().Level = nullptr;
	}

	Get<SLevelComponent>().Denizens.clear();
}

void LevelFunctionality::SetStartPosition(const SVector2f aPosition)
{
	Get<SLevelComponent>().StartPosition = aPosition;
}
