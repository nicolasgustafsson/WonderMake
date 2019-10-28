#pragma once
#include "Weapons/WeaponProperty/WeaponProperty.h"
#include "Weapons/WeaponSwing.h"

class MeleeWeapon
	: public NonCopyable
{
public:
	MeleeWeapon() = default;
	MeleeWeapon(MeleeWeapon&& aOther) noexcept;

	void Inspect();

	std::vector<SSwing> mySwings;

	f32 myBaseWeaponDamage = 0.f;
	f32 myBaseWeaponSwingRate = 1.f;

private:
	std::vector<std::unique_ptr<WeaponProperty>> myProperties;
};

