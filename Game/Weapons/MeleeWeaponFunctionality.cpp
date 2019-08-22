#include "pch.h"
#include "MeleeWeaponFunctionality.h"

MeleeWeaponFunctionality::MeleeWeaponFunctionality(Object& aObject)
	: Super(aObject)
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/sword.png");
	Get<SpriteRenderingFunctionality>().SetOrigin({ 0.f, 1.0f });
}

void MeleeWeaponFunctionality::Swing()
{
	myRotation -= 1.f;
	Get<SpriteRenderingFunctionality>().SetRotation(myRotation * 0.1f);
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

