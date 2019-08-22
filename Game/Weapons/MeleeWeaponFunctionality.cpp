#include "pch.h"
#include "MeleeWeaponFunctionality.h"

MeleeWeaponFunctionality::MeleeWeaponFunctionality(Object& aObject)
	: Super(aObject)
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/sword.png");
}

void MeleeWeaponFunctionality::Swing()
{

}

void MeleeWeaponFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponComponent>().myWeapon.emplace(std::move(aWeapon));
}

void MeleeWeaponFunctionality::SetParent(STransformComponent* aParentTransform)
{
	Get<SMeleeWeaponComponent>().ParentTransform = aParentTransform;
}

void MeleeWeaponFunctionality::Tick()
{
	STransformComponent* parent = Get<SMeleeWeaponComponent>().ParentTransform;

	if (parent)
		Get<TransformFunctionality>().SetPosition(parent->Position);
}

