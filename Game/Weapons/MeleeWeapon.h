#pragma once
#include "Weapons/WeaponProperty/WeaponProperty.h"
#include "Weapons/WeaponSwing.h"
#include "Designers/NewDesigners/MovesetDesigner/MovesetDesigner.h"
#include "Movesets/Moveset.h"

class MeleeWeapon
	: public NonCopyable
{
public:
	MeleeWeapon() = default;
	MeleeWeapon(MeleeWeapon&& aOther) noexcept;

	void Inspect();

	SMoveset myMoveset;

	f32 myBaseWeaponDamage = 0.f;
	f32 myBaseWeaponSwingRate = 1.f;

private:
	std::vector<std::unique_ptr<WeaponProperty>> myProperties;
};

