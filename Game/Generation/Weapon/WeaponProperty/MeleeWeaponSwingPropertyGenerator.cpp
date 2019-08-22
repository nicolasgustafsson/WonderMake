#include "pch.h"
#include "MeleeWeaponSwingPropertyGenerator.h"
#include "Generation/Weapon/WeaponProperty/MeleeWeaponSwingGrammars.h"

void MeleeWeaponSwingPropertyGenerator::PreProcess(WeaponSwingProperty& aProperty)
{
	aProperty.mySwing.mySwingPath.myStart = { -100.f, 0.f };
	aProperty.mySwing.mySwingPath.myFirstControl = { -100.f, 100.f };
	aProperty.mySwing.mySwingPath.mySecondControl = { 100.f, 100.f };
	aProperty.mySwing.mySwingPath.myEnd = { 100.f, 0.f };
}

bool MeleeWeaponSwingPropertyGenerator::ShouldApplyGrammars(const WeaponSwingProperty& aObject, const SMeleeWeaponSwingPropertyGenerationParameters& aGenerationParameters) const
{
	return false;
}

void MeleeWeaponSwingPropertyGenerator::ImplementGrammars()
{
	//AddGrammar<AddWeaponSwingGrammar>();
}
