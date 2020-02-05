#pragma once
#include "Components/Component.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

class RangedWeaponFunctionality;
struct RangedWeapon;

struct SRangedWeaponUserComponent
	: public SComponent
{
	Object WeaponObject;

	RangedWeaponFunctionality* Weapon = nullptr;

	i32 CurrentSwingIndex = 0;
};

class RangedWeaponUserFunctionality
	: public Functionality<RangedWeaponUserFunctionality, SRangedWeaponUserComponent, TransformFunctionality>
{
public:
	RangedWeaponUserFunctionality(Object& aOwner);

	void Inspect();

	void ShootWeapon();
	void SetWeapon(RangedWeapon&& aWeapon);
};