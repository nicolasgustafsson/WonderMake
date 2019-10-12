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
	Get<ActionFunctionality>().StartAction(WeaponSwingAction(*Get<SMeleeWeaponUserComponent>().Weapon, Get<TransformFunctionality>()));
}

void MeleeWeaponUserFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponUserComponent>().Weapon->SetWeapon(std::forward<MeleeWeapon>(aWeapon));
}
