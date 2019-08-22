#include "pch.h"
#include "MeleeWeaponSwingPropertyGenerator.h"
#include "Generation/Weapon/WeaponProperty/MeleeWeaponSwingGrammars.h"

bool MeleeWeaponSwingPropertyGenerator::ShouldApplyGrammars(const WeaponSwingProperty& aObject, const SMeleeWeaponSwingPropertyGenerationParameters& aGenerationParameters) const
{
	return false;
}

void MeleeWeaponSwingPropertyGenerator::ImplementGrammars()
{
	//AddGrammar<AddWeaponSwingGrammar>();
}
