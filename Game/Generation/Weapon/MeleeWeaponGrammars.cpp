#include "pch.h"
#include "MeleeWeaponGrammars.h"
#include "Weapons/MeleeWeapon.h"
#include "Weapons/WeaponProperty/WeaponProperty.h"
#include "Generation/Weapon/WeaponProperty/MeleeWeaponSwingPropertyGenerator.h"


f32 AddWeaponSwingGrammar::GetPriority(MeleeWeapon& aObject, const SWeaponGenerationParameters& aParameters) const
{
	return 5.0f - aObject.myProperties.size() + aParameters.Potential;
}

void AddWeaponSwingGrammar::Modify(MeleeWeapon& aObject, SWeaponGenerationParameters& aParameters) const
{
	aParameters.Potential -= 10.f;
	
	SMeleeWeaponSwingPropertyGenerationParameters params;
	params.Potential = 10.f;

	aObject.mySwingProperty = SystemPtr<MeleeWeaponSwingPropertyGenerator>()->Generate(params);

}
