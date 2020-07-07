#include "pch.h"
#include "MeleeWeaponUserFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "MeleeWeaponFunctionality.h"
#include "Input/InputSystem.h"
#include "Actions/ActionFunctionality.h"
#include "Weapons/WeaponSwingAction.h"
#include "Weapons/WeaponSwingHitShapeAction.h"

MeleeWeaponUserFunctionality::MeleeWeaponUserFunctionality(Object& aOwner)
	: Super(aOwner)
{
	Get<SMeleeWeaponUserComponent>().Weapon = &Get<SMeleeWeaponUserComponent>().WeaponObject.Add<MeleeWeaponFunctionality>();
}

void MeleeWeaponUserFunctionality::Inspect()
{
	Get<SMeleeWeaponUserComponent>().Weapon->Inspect();
}

void MeleeWeaponUserFunctionality::SwingWeapon()
{
	auto& component = Get<SMeleeWeaponUserComponent>();
	auto& actionFunctionality = Get<ActionFunctionality>();

	MeleeWeapon& weapon = component.Weapon->GetWeapon();

	if (weapon.mySwings.empty())
		return;

	if (!actionFunctionality.WasLastActionOfType<WeaponSwingAction>() || actionFunctionality.TimeSinceLastAction() > 0.5f)
		component.CurrentSwingIndex = 0;

	component.CurrentSwingIndex %= weapon.mySwings.size();

	//EActionResult result = actionFunctionality.StartAction(WeaponSwingAction
	//(Get<CharacterFunctionality>()
	//	, *component.Weapon
	//	, Get<TransformFunctionality>()
	//	, weapon.mySwings[component.CurrentSwingIndex]));

	EActionResult result = actionFunctionality.StartAction(WeaponSwingHitShapeAction
	(
		Get<CharacterFunctionality>()
	));

	if (result == EActionResult::Succeeded)
		component.CurrentSwingIndex++;
}

void MeleeWeaponUserFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponUserComponent>().Weapon->SetWeapon(std::forward<MeleeWeapon>(aWeapon));
}
