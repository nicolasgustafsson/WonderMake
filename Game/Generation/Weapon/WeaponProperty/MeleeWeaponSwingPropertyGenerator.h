#pragma once
#include "Generation/BaseGenerator.h"
#include "Utilities/Debugging/Debugged.h"
#include "Weapons/WeaponProperty/WeaponProperty.h"

#include "Generation/Weapon/WeaponProperty/MeleeWeaponSwingPropertyGenerationParameters.h"

class MeleeWeaponSwingPropertyGenerator
	: public Generator<WeaponSwingProperty, SMeleeWeaponSwingPropertyGenerationParameters>
{
public:

private:
	virtual void PreProcess(WeaponSwingProperty& aProperty);

	virtual bool ShouldApplyGrammars(const WeaponSwingProperty& aObject, const SMeleeWeaponSwingPropertyGenerationParameters& aGenerationParameters) const override;

	virtual void ImplementGrammars() override;
};

