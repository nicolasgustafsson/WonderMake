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

void MeleeWeaponFunctionality::Swing()
{
	auto& WeaponComp = Get<SMeleeWeaponComponent>();

	WeaponComp.Weapon->DrawSwing(Get<SMeleeWeaponComponent>().ParentTransform->Position);

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
	auto& Weapon = GetWeapon();
	auto& CurrentSwing = WeaponComp.CurrentSwing;

	if (CurrentSwing.IsActive)
	{
		CurrentSwing.Progress += (deltaTime / Weapon.mySwing.mySwingTime) * Weapon.myBaseWeaponSwingRate;

		STransformComponent* parent = Get<SMeleeWeaponComponent>().ParentTransform;

		if (parent)
		{
			const SVector2f SwingLocation = Weapon.mySwing.mySwingPath.GetLocationAt(CurrentSwing.Progress);
			Get<TransformFunctionality>().SetPosition(parent->Position + SwingLocation);

			Get<TransformFunctionality>().SetRotation(SwingLocation.GetRotation() + (3.141592f / 12.f) * (1.f - CurrentSwing.Progress * CurrentSwing.Progress * CurrentSwing.Progress) + 0.7f);
		}

		if (CurrentSwing.Progress > 1.0f)
		{
			StopSwing();
		}
	}
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

void MeleeWeaponFunctionality::StopSwing()
{
	auto& WeaponComp = Get<SMeleeWeaponComponent>();
	WeaponComp.CurrentSwing.IsActive = false;
	Get<SpriteRenderingFunctionality>().Hide();
}

