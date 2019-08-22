#pragma once
#include "Weapons/WeaponProperty/WeaponProperty.h"

class MeleeWeapon
	: public NonCopyable
{
public:
	MeleeWeapon(const f32 aPotential = 1);
	~MeleeWeapon();

	MeleeWeapon(MeleeWeapon&& aOther);

	void Inspect();

	WeaponSwingProperty mySwingProperty;

	std::vector<std::unique_ptr<WeaponProperty>> myProperties;
	f32 myPotential = 0;
};

