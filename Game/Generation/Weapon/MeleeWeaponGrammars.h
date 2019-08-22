#pragma once
#include "Generation/BaseGenerator.h"
#include "Generation/Weapon/MeleeWeaponGenerationParameters.h"

class MeleeWeapon;


class MeleeWeaponGrammar
	: public Grammar<MeleeWeapon, SWeaponGenerationParameters>
{
};

class AddWeaponSwingGrammar
	: public MeleeWeaponGrammar
{
public:
	virtual f32 GetPriority(MeleeWeapon& aObject, const SWeaponGenerationParameters& aParameters) const override;
	virtual void Modify(MeleeWeapon& aObject, SWeaponGenerationParameters& aParameters) const override;
};