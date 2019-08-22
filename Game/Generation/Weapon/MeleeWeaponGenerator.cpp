#include "pch.h"
#include "MeleeWeaponGenerator.h"
#include "Generation/Weapon/MeleeWeaponGrammars.h"
#include "Randomizer.h"

bool MeleeWeaponGenerator::ShouldApplyGrammars(const MeleeWeapon& aObject, const SWeaponGenerationParameters& aGenerationParameters) const
{
	return aGenerationParameters.Potential > 0.f;
}

void MeleeWeaponGenerator::ImplementGrammars()
{
	AddGrammar<AddWeaponSwingGrammar>();
}
