#include "pch.h"
#include "MeleeWeaponUserFunctionality.h"
#include <Functionalities/SpriteRenderingFunctionality.h>
#include "Functionalities/TransformFunctionality.h"
#include "MeleeWeaponFunctionality.h"

MeleeWeaponUserFunctionality::MeleeWeaponUserFunctionality(Object& aOwner)
	: Super(aOwner)
{
	Get<SMeleeWeaponUserComponent>().myWeapon = &Get<SMeleeWeaponUserComponent>().WeaponObject.Add<MeleeWeaponFunctionality>();
	Get<SMeleeWeaponUserComponent>().myWeapon->SetParent(&Get<STransformComponent>());
}

void MeleeWeaponUserFunctionality::Tick()
{
}

void MeleeWeaponUserFunctionality::Inspect()
{
	Get<SMeleeWeaponUserComponent>().myWeapon->Inspect();
}

void MeleeWeaponUserFunctionality::SwingWeapon()
{
	Get<SMeleeWeaponUserComponent>().myWeapon->Swing();
}

void MeleeWeaponUserFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponUserComponent>().myWeapon->SetWeapon(std::move(aWeapon));
}
