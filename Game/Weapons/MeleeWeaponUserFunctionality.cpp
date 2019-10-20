#include "pch.h"
#include "MeleeWeaponUserFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "MeleeWeaponFunctionality.h"
#include "Input/InputSystem.h"
#include "Actions/ActionFunctionality.h"
#include "Weapons/WeaponSwingAction.h"

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

	MeleeWeapon& weapon = Get<SMeleeWeaponUserComponent>().Weapon->GetWeapon();

	if (weapon.mySwings.size() == 0)
		return;

	if (!actionFunctionality.WasLastActionOfType<WeaponSwingAction>() || actionFunctionality.TimeSinceLastAction() > 10.0f)
		component.CurrentSwingIndex = 0;

	component.CurrentSwingIndex %= weapon.mySwings.size();

	Get<ActionFunctionality>().StartAction(WeaponSwingAction
		( *Get<SMeleeWeaponUserComponent>().Weapon
		, Get<TransformFunctionality>()
		, weapon.mySwings[component.CurrentSwingIndex]));

	component.CurrentSwingIndex++;
}

void MeleeWeaponUserFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponUserComponent>().Weapon->SetWeapon(std::forward<MeleeWeapon>(aWeapon));
}
