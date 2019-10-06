#include "pch.h"
#include "WeaponSwingAction.h"
#include "Weapons/MeleeWeapon.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Input/InputSystem.h"
#include "Weapons/MeleeWeaponFunctionality.h"

WeaponSwingAction::WeaponSwingAction(MeleeWeaponFunctionality& aMeleeWeaponFunctionality, TransformFunctionality& aUserTransform)
	: myUserTransform(aUserTransform)
	, myWeaponFunctionality(aMeleeWeaponFunctionality) {}

void WeaponSwingAction::BeginAction()
{
	myWeaponFunctionality.GetSprite().Show();
	myUserTransform.FacePosition(SystemPtr<InputSystem>()->GetMousePositionInWorld());
}

void WeaponSwingAction::Tick()
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	const auto& swing = myWeaponFunctionality.GetWeapon().mySwing;
	auto& weaponTransform = myWeaponFunctionality.GetTransform();

	myProgress += (deltaTime / swing.mySwingTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate;

	SVector2f swingLocation = swing.mySwingPath.GetLocationAt(myProgress);
	swingLocation.Rotate((myUserTransform.GetRotation()));
	weaponTransform.SetPosition(myUserTransform.GetPosition() + swingLocation);

	weaponTransform.SetRotation(
		swingLocation.GetRotation() + (Constants::Pi / 12.f) * (std::powf(myProgress, 3.f)) - Constants::HalfPi * 1.5f);
}

bool WeaponSwingAction::IsCompleted()
{
	return myProgress >= 1.0f;
}

void WeaponSwingAction::EndAction()
{
	myWeaponFunctionality.GetSprite().Hide();
}
