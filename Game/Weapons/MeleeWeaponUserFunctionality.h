#pragma once
#include "Components/Component.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Actions/ActionFunctionality.h"
#include "Character/CharacterFunctionality.h"
#include "Weapons/MeleeWeapon.h"

class MeleeWeaponFunctionality;
class MeleeWeapon;

struct SMeleeWeaponUserComponent
	: public SComponent
{
	std::optional<MeleeWeapon> Weapon;

	i32 CurrentSwingIndex = 0;
};

class MeleeWeaponUserFunctionality
	: public Functionality<
		MeleeWeaponUserFunctionality,
		Policy::Set<
			Policy::Add<SMeleeWeaponUserComponent, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<ActionFunctionality, Policy::EPermission::Write>,
			Policy::Add<CharacterFunctionality, Policy::EPermission::Write>>>
{
public:
	MeleeWeaponUserFunctionality(Object& aOwner);

	void Inspect();

	void SwingWeapon();
	void SetWeapon(MeleeWeapon&& aWeapon);
};

REGISTER_FUNCTIONALITY(MeleeWeaponUserFunctionality);
