#pragma once
#include "Components/Component.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Actions/ActionFunctionality.h"
#include "Character/CharacterFunctionality.h"
#include "Weapons/MeleeWeapon.h"

class MeleeWeaponFunctionality;
class MeleeWeapon;
class InputSystem;

struct SMeleeWeaponUserComponent
	: public SComponent
{
	std::optional<MeleeWeapon> Weapon;

	i32 CurrentSwingIndex = 0;
};

class MeleeWeaponUserFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<SMeleeWeaponUserComponent, PWrite>,
			PAdd<TransformFunctionality2D, PWrite>,
			PAdd<ActionFunctionality, PWrite>,
			PAdd<CharacterFunctionality, PWrite>,
			PAdd<InputSystem, PWrite>>>
{
public:
	void Inspect();

	void SwingWeapon();
	void SetWeapon(MeleeWeapon&& aWeapon);
};
