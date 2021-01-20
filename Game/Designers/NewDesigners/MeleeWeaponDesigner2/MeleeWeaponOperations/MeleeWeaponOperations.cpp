#include "pch.h"
#include "MeleeWeaponOperations.h"
#include "Weapons/MeleeWeapon.h"
#include "../MeleeWeaponAttributes/MeleeWeaponAttributes.h"

bool SmashTogetherWeapon::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SMovesetAttribute>();
}

void SmashTogetherWeapon::Perform(Sketch& aSketch) const
{
	SDesignedObjectAttribute<MeleeWeapon> attribute;

	attribute.FinishedDesign.myMoveset = aSketch.GetAttribute<SMovesetAttribute>()->Moveset;

	aSketch.AddAttribute(std::move(attribute));
}

bool GenerateMoveset::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void GenerateMoveset::Perform(Sketch& aSketch) const
{
	MovesetDesigner designer;
	SMovesetAttribute moveset;
	moveset.Moveset = designer.Design(aSketch);

	aSketch.AddAttribute(std::move(moveset));
}
