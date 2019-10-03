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

	//uncomment to render the actual bezier curve
	//TransformFunctionality* parent = Get<SMeleeWeaponComponent>().ParentTransform;
	//WeaponComp.Weapon->DrawSwing(parent->GetPosition(), parent->GetRotation());

	Get<SpriteRenderingFunctionality>().Show();

	WeaponComp.CurrentSwing.IsActive = true;
	WeaponComp.CurrentSwing.Progress = 0.f;
}

void MeleeWeaponFunctionality::SetWeapon(MeleeWeapon&& aWeapon)
{
	Get<SMeleeWeaponComponent>().Weapon.emplace(std::move(aWeapon));
}

void MeleeWeaponFunctionality::SetParent(TransformFunctionality* aParentTransform)
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

		TransformFunctionality* parent = Get<SMeleeWeaponComponent>().ParentTransform;

		if (parent)
		{
			SVector2f SwingLocation = Weapon.mySwing.mySwingPath.GetLocationAt(CurrentSwing.Progress);

			SwingLocation.Rotate((parent->GetRotation()));

			Get<TransformFunctionality>().SetPosition(parent->GetPosition() + SwingLocation);

			Get<TransformFunctionality>().SetRotation(
				SwingLocation.GetRotation() + (Constants::Pi / 12.f) * (std::powf(CurrentSwing.Progress, 3.f)) - Constants::HalfPi * 1.5f);
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

void MeleeWeaponFunctionality::StopSwing()
{
	auto& WeaponComp = Get<SMeleeWeaponComponent>();

	WeaponComp.CurrentSwing.IsActive = false;
	Get<SpriteRenderingFunctionality>().Hide();
}

MeleeWeapon& MeleeWeaponFunctionality::GetWeapon()
{
	auto& WeaponComp = Get<SMeleeWeaponComponent>();

	return *WeaponComp.Weapon;
}

