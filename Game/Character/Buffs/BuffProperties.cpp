#include "pch.h"
#include "BuffProperties.h"
#include "Character/CharacterFunctionality.h"

void BuffBlueprintPropertyInstance::Tick()
{
	myBlueprintProperty.Tick(*this);
}

void BuffBlueprintPropertyInstance::Inspect()
{
	myBlueprintProperty.Inspect();
	myBlueprintProperty.InspectInstance(*this);
}

bool BuffBlueprintPropertyInstance::BuffShouldDie() const
{
	return myBlueprintProperty.BuffShouldDie(*this);
}
