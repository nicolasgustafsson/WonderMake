#pragma once
#include "Components/Component.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Weapons/WeaponSwingAction.h"
#include "Actions/ActionFunctionality.h"

class MeleeWeaponFunctionality;
class MeleeWeapon;

struct SMeleeWeaponUserComponent
	: public SComponent
{
	Object WeaponObject;

	MeleeWeaponFunctionality* Weapon = nullptr;

	std::optional<WeaponSwingAction> SwingAction;
};

class MeleeWeaponUserFunctionality
	: public Functionality<MeleeWeaponUserFunctionality, SMeleeWeaponUserComponent, TransformFunctionality, ActionFunctionality>
{
public:
	MeleeWeaponUserFunctionality(Object& aOwner);

	void Inspect();

	void SwingWeapon();
	void SetWeapon(MeleeWeapon&& aWeapon);
};