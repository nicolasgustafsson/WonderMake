#include "pch.h"
#include "MeleeWeaponFunctionality.h"

MeleeWeaponFunctionality::MeleeWeaponFunctionality(Object& aObject)
	: Super(aObject)
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/sword.png");
	Get<SpriteRenderingFunctionality>().SetOrigin({ 0.f, 0.8f });
	Get<SpriteRenderingFunctionality>().Hide();

	SetWeapon(MeleeWeapon());
}

void MeleeWeaponFunctionality::Swing()
{

	auto& WeaponComp = Get<SMeleeWeaponComponent>();

	WeaponComp.Weapon->mySwingProperty.DrawSwing(Get<SMeleeWeaponComponent>().ParentTransform->Position);
	
	Get<SpriteRenderingFunctionality>().Show();

	WeaponComp.CurrentSwing.IsActive = true;
	WeaponComp.CurrentSwing.Progress = 0.f;
}

void MeleeWeaponFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponComponent>().Weapon.emplace(std::move(aWeapon));
}

void MeleeWeaponFunctionality::SetParent(STransformComponent* aParentTransform)
{
	Get<SMeleeWeaponComponent>().ParentTransform = aParentTransform;
}

void MeleeWeaponFunctionality::Tick()
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	auto& WeaponComp = Get<SMeleeWeaponComponent>();

	if (WeaponComp.CurrentSwing.IsActive)
	{
		WeaponComp.CurrentSwing.Progress += deltaTime / WeaponComp.Weapon->mySwingProperty.mySwing.mySwingTime;

		STransformComponent* parent = Get<SMeleeWeaponComponent>().ParentTransform;

		if (parent)
		{
			const SVector2f SwingLocation = WeaponComp.Weapon->mySwingProperty.mySwing.mySwingPath.GetLocationAt(WeaponComp.CurrentSwing.Progress * WeaponComp.CurrentSwing.Progress);
			Get<TransformFunctionality>().SetPosition(parent->Position + SwingLocation);

			Get<TransformFunctionality>().SetRotation(SwingLocation.GetRotation() + (3.141592f / 3.f) * (1.f - WeaponComp.CurrentSwing.Progress * WeaponComp.CurrentSwing.Progress * WeaponComp.CurrentSwing.Progress));
		}

		if (WeaponComp.CurrentSwing.Progress > 1.0f)
		{
			StopSwing();
		}
	}
}

void MeleeWeaponFunctionality::StopSwing()
{
	auto& WeaponComp = Get<SMeleeWeaponComponent>();
	WeaponComp.CurrentSwing.IsActive = false;
	Get<SpriteRenderingFunctionality>().Hide();
}

