#include "pch.h"
#include "MeleeWeaponSwingGrammars.h"


f32 IncreaseDamageGrammar::GetPriority(WeaponSwingProperty& aObject, const SMeleeWeaponSwingPropertyGenerationParameters& aParameters) const
{
	return 1;
}

void IncreaseDamageGrammar::Modify(WeaponSwingProperty& aObject, SMeleeWeaponSwingPropertyGenerationParameters& aParameters) const
{
	aObject.mySwing.mySwingDamage += aParameters.Potential;
	aParameters.Potential = 0;
}
