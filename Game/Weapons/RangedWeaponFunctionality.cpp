#include "pch.h"

#include "RangedWeaponFunctionality.h"
#include "Designers/WeaponDesigners/RangedWeaponDesigner.h"

RangedWeaponFunctionality::RangedWeaponFunctionality(Object& aObject)
	: Super(aObject)
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/bow.png");
	Get<SpriteRenderingFunctionality>().SetOrigin({ 0.f, 0.8f });
	Get<SpriteRenderingFunctionality>().Hide();

	SetWeapon(SystemPtr<RangedWeaponDesigner>()->DesignWeapon());
}

void RangedWeaponFunctionality::SetWeapon(RangedWeapon&& aWeapon)
{
	Get<SRangedWeaponComponent>().Weapon.emplace(std::move(aWeapon));
}

void RangedWeaponFunctionality::Inspect()
{
	GetWeapon().Inspect();
}

RangedWeapon& RangedWeaponFunctionality::GetWeapon()
{
	auto& WeaponComp = Get<SRangedWeaponComponent>();

	return *WeaponComp.Weapon;
}
