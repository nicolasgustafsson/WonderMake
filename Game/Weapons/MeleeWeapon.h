#pragma once
#include "Weapons/WeaponProperty/WeaponProperty.h"
#include "Generation/GeneratedObject.h"
#include "Generation/GenerationParameters/Strength.h"

class MeleeWeapon
	: public NonCopyable
	, public GeneratedObject<SStrength>
{
public:
	MeleeWeapon() = default;
	MeleeWeapon(MeleeWeapon&& aOther);

	void Inspect();

	virtual void Strengthen(const SStrength aStrength) override;
	virtual void Weaken(const SStrength aStrength) override;
	virtual SStrength GetStrength() const override;

	WeaponSwingProperty mySwingProperty;

	std::vector<std::unique_ptr<WeaponProperty>> myProperties;

	virtual void Generate(SStrength aStrength) override;
};

