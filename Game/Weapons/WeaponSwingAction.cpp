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
	switch (myCurrentState)
	{
	case ESwingState::Charge:
		UpdateCharge();
		break;
	case ESwingState::Swing:
		UpdateSwing();
		break;
	case ESwingState::Backswing:
		UpdateBackswing();
		break;
	default:
		break;
	}

	WmLog("myStateProgress: ", myStateProgress);

	if (myStateProgress > 1.0f && !IsCompleted())
	{
		myStateProgress -= 1.0f;

		myCurrentState = (myCurrentState == ESwingState::Charge) ? ESwingState::Swing : ESwingState::Backswing;
	}
}

bool WeaponSwingAction::IsCompleted()
{
	return myStateProgress >= 1.0f && myCurrentState == ESwingState::Backswing;
}

void WeaponSwingAction::EndAction()
{
	myWeaponFunctionality.GetSprite().Hide();
}

bool WeaponSwingAction::CanBeInterrupted()
{
	return myCurrentState == ESwingState::Backswing && myStateProgress > 0.5f;
}

void WeaponSwingAction::SetSwingTransform(const f32 aPercentageInSwing)
{
	auto& swing = myWeaponFunctionality.GetWeapon().mySwing;
	auto& weaponTransform = myWeaponFunctionality.GetTransform();

	SVector2f swingLocation = swing.mySwingPath.GetConstantLocationAt(aPercentageInSwing);
	swingLocation.Rotate((myUserTransform.GetRotation()));
	weaponTransform.SetPosition(myUserTransform.GetPosition() + swingLocation);

	weaponTransform.SetRotation(
		swingLocation.GetRotation() + (Constants::Pi / 12.f) * (std::powf(aPercentageInSwing, 3.f)) - Constants::HalfPi * 1.5f);
}

void WeaponSwingAction::UpdateCharge()
{
	SetSwingTransform((1.0f - myStateProgress * myStateProgress) * 0.1f);

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	const auto& swing = myWeaponFunctionality.GetWeapon().mySwing;
	myStateProgress += (deltaTime / swing.myChargeTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate;
}

void WeaponSwingAction::UpdateSwing()
{
	SetSwingTransform(myStateProgress * 0.95f);

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	const auto& swing = myWeaponFunctionality.GetWeapon().mySwing;
	myStateProgress += (deltaTime / swing.mySwingTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate;
}

void WeaponSwingAction::UpdateBackswing()
{
	const f32 inverseProgress = 1.0f - myStateProgress;
	const f32 inverseQuadraticProgress = 1.0f - ((inverseProgress) * (inverseProgress));

	//slowly wind down the swing
	SetSwingTransform(0.95f + inverseQuadraticProgress * 0.05f);

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	const auto& swing = myWeaponFunctionality.GetWeapon().mySwing;
	myStateProgress += (deltaTime / swing.myBackswingTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate;
}
