#pragma once
#include "Generation/BaseGenerator.h"
#include "Weapons/MeleeWeapon.h"
#include "Generation/Weapon/MeleeWeaponGenerationParameters.h"


class MeleeWeaponGenerator
	: public Generator<MeleeWeapon, SWeaponGenerationParameters>
{
public:

private:
	virtual bool ShouldApplyGrammars(const MeleeWeapon& aObject, const SWeaponGenerationParameters& aGenerationParameters) const override;

	virtual void ImplementGrammars() override;
};

