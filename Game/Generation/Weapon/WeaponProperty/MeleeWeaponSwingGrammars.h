#pragma once
#include "Generation/BaseGenerator.h"
#include "Generation/Weapon/MeleeWeaponGenerationParameters.h"
#include "Generation/Weapon/WeaponProperty/MeleeWeaponSwingPropertyGenerationParameters.h"
#include "Weapons/WeaponProperty/WeaponProperty.h"

class MeleeWeaponSwingGrammar
	: public Grammar<WeaponSwingProperty, SMeleeWeaponSwingPropertyGenerationParameters>
{
};

class IncreaseDamageGrammar
	: public MeleeWeaponSwingGrammar
{
public:
	virtual f32 GetPriority(WeaponSwingProperty& aObject, const SMeleeWeaponSwingPropertyGenerationParameters& aParameters) const override;
	virtual void Modify(WeaponSwingProperty& aObject, SMeleeWeaponSwingPropertyGenerationParameters& aParameters) const override;
};