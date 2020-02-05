#include "pch.h"

#include "RangedWeaponUserFunctionality.h"
#include "RangedWeaponFunctionality.h"

#include "Functionalities/TransformFunctionality.h"

RangedWeaponUserFunctionality::RangedWeaponUserFunctionality(Object& aOwner)
	: Super(aOwner)
{
	Get<SRangedWeaponUserComponent>().Weapon = &Get<SRangedWeaponUserComponent>().WeaponObject.Add<RangedWeaponFunctionality>();
}

void RangedWeaponUserFunctionality::Inspect()
{
	Get<SRangedWeaponUserComponent>().Weapon->Inspect();
}

void RangedWeaponUserFunctionality::ShootWeapon()
{
	auto& component = Get<SRangedWeaponUserComponent>();

	RangedWeapon& weapon = component.Weapon->GetWeapon();

	WmLog("Shooty!");
}

void RangedWeaponUserFunctionality::SetWeapon(RangedWeapon&& aWeapon)
{
	Get<SRangedWeaponUserComponent>().Weapon->SetWeapon(std::move(aWeapon));
}
