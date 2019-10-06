#include "pch.h"

#include "MeleeWeaponFunctionality.h"

MeleeWeaponFunctionality::MeleeWeaponFunctionality(Object& aObject)
	: Super(aObject)
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/sword.png");
	Get<SpriteRenderingFunctionality>().SetOrigin({ 0.f, 0.8f });
	Get<SpriteRenderingFunctionality>().Hide();

	SetWeapon(MeleeWeapon(100.f));
}

void MeleeWeaponFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponComponent>().Weapon.emplace(std::move(aWeapon));
}

void MeleeWeaponFunctionality::Inspect()
{
	GetWeapon().Inspect();
}

MeleeWeapon& MeleeWeaponFunctionality::GetWeapon()
{
	auto& WeaponComp = Get<SMeleeWeaponComponent>();

	return *WeaponComp.Weapon;
}

