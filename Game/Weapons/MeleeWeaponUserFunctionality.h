#pragma once
#include "Components/Component.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

class MeleeWeaponFunctionality;
class MeleeWeapon;

struct SMeleeWeaponUserComponent
	: public SComponent
{
	Object WeaponObject;

	MeleeWeaponFunctionality* myWeapon = nullptr;
};

class MeleeWeaponUserFunctionality
	: public Functionality<MeleeWeaponUserFunctionality, SMeleeWeaponUserComponent, TransformFunctionality>
{
public:
	MeleeWeaponUserFunctionality(Object& aOwner);

	void Inspect();

	void SwingWeapon();
	void SetWeapon(MeleeWeapon&& aWeapon);
};