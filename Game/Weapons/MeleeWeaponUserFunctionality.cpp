#include "pch.h"
#include "MeleeWeaponUserFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Input/InputSystem.h"
#include "Actions/ActionFunctionality.h"
#include "Weapons/WeaponSwingHitShapeAction.h"
#include "Weapons/MeleeWeapon.h"

REGISTER_COMPONENT(SMeleeWeaponUserComponent);
REGISTER_FUNCTIONALITY(MeleeWeaponUserFunctionality);

void MeleeWeaponUserFunctionality::Inspect()
{
	Get<SMeleeWeaponUserComponent>().Weapon->Inspect();
}

void MeleeWeaponUserFunctionality::SwingWeapon()
{
	auto& meleeWeaponUserComponent = Get<SMeleeWeaponUserComponent>();

	if (!meleeWeaponUserComponent.Weapon)
		return;

	auto& actionFunctionality = Get<ActionFunctionality>();

	MeleeWeapon& weapon = *meleeWeaponUserComponent.Weapon;

	if (weapon.mySwings.empty())
		return;

	if (!actionFunctionality.WasLastActionOfType<WeaponSwingHitShapeAction>() || actionFunctionality.TimeSinceLastAction() > 0.5f)
		meleeWeaponUserComponent.CurrentSwingIndex = 0;

	meleeWeaponUserComponent.CurrentSwingIndex %= weapon.mySwings.size();

	//EActionResult result = actionFunctionality.StartAction(WeaponSwingAction
	//(Get<CharacterFunctionality>()
	//	, *component.Weapon
	//	, Get<TransformFunctionality2D>()
	//	, weapon.mySwings[component.CurrentSwingIndex]));

	EActionResult result = actionFunctionality.StartAction(WeaponSwingHitShapeAction
	(
		Get<CharacterFunctionality>()
		, weapon.mySwings[meleeWeaponUserComponent.CurrentSwingIndex]
	));

	if (result == EActionResult::Succeeded)
		meleeWeaponUserComponent.CurrentSwingIndex++;
}

void MeleeWeaponUserFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponUserComponent>().Weapon.emplace(std::forward<MeleeWeapon>(aWeapon));
}
