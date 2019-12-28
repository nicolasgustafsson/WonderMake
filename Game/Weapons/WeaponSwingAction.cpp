#include "pch.h"
#include "WeaponSwingAction.h"
#include "Weapons/MeleeWeapon.h"
#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/SpriteRenderingFunctionality.h"
#include "Input/InputSystem.h"
#include "Weapons/MeleeWeaponFunctionality.h"
#include "Collision/CollisionSystem.h"
#include "Enemy/EnemyControllerFunctionality.h"

WeaponSwingAction::WeaponSwingAction(MeleeWeaponFunctionality& aMeleeWeaponFunctionality, TransformFunctionality& aUserTransform, const SSwing aSwingToPerform) noexcept
	: myUserTransform(aUserTransform)
	, myWeaponFunctionality(aMeleeWeaponFunctionality)
	, mySwing(aSwingToPerform)  {}

void WeaponSwingAction::BeginAction()
{
	myWeaponFunctionality.GetSprite().Show();
	myWeaponFunctionality.GetSprite().SetScale(mySwing.IsMirrored ? SVector2f{-1.0f, 1.0f} : SVector2f{1.0f, 1.0f});
	myUserTransform.FacePosition(SystemPtr<InputSystem>()->GetMousePositionInWorld());
}

void WeaponSwingAction::Tick() noexcept
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
	}

	if (myStateProgress >= 1.0f && !IsCompleted())
	{
		myStateProgress -= 1.0f;

		myCurrentState = (myCurrentState == ESwingState::Charge) ? ESwingState::Swing : ESwingState::Backswing;
	}
}

bool WeaponSwingAction::IsCompleted() const noexcept
{
	return myStateProgress >= 1.0f && myCurrentState == ESwingState::Backswing;
}

void WeaponSwingAction::EndAction() noexcept
{
	myWeaponFunctionality.GetSprite().Hide();
}

bool WeaponSwingAction::CanBeInterrupted() const noexcept
{
	if (mySwing.IsFinisher)
		return false;

	return myCurrentState == ESwingState::Backswing && myStateProgress > 0.5f;
}

void WeaponSwingAction::TestSwingCollision()
{
	auto& weaponTransform = myWeaponFunctionality.GetTransform();

	const SVector2f start = weaponTransform.GetPosition();
	const SVector2f end = start + SVector2f(0.f, -100.f).Rotate(weaponTransform.GetRotation());

	SystemPtr<CollisionSystem> collision;
	collision->OverlapLineAgainstFunctionality<EnemyControllerFunctionality>(start, end, [](EnemyControllerFunctionality&) {WmLog("pew!"); });
}

void WeaponSwingAction::SetSwingTransform(const f32 aPercentageInSwing) noexcept
{
	auto& weaponTransform = myWeaponFunctionality.GetTransform();

	SVector2f swingLocation = mySwing.SwingPath.GetConstantLocationAt(aPercentageInSwing);
	swingLocation.Rotate(myUserTransform.GetRotation());
	weaponTransform.SetPosition(myUserTransform.GetPosition() + swingLocation);

	if (!mySwing.IsMirrored)
	{
		weaponTransform.SetRotation(
			swingLocation.GetRotation() + (Constants::Pi / 12.f) * (std::powf(aPercentageInSwing, 3.f)) - Constants::HalfPi * 1.5f);
	}
	else
	{
		weaponTransform.SetRotation(
			swingLocation.GetRotation() - (Constants::Pi / 12.f) * (std::powf(aPercentageInSwing, 3.f)) + Constants::HalfPi * 1.5f);
	}

	if (myCurrentState == ESwingState::Swing)
	{
		TestSwingCollision();
	}
}

void WeaponSwingAction::UpdateCharge() noexcept
{
	SetSwingTransform((1.0f - myStateProgress * myStateProgress) * 0.1f);

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	const f32 progressIncrease = (deltaTime / mySwing.ChargeTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate;

	IncreaseProgress(progressIncrease);
}

void WeaponSwingAction::IncreaseProgress(const f32 progressIncrease)
{
	myPreviousStateProgress = myStateProgress;
	myStateProgress += progressIncrease;

	if (myStateProgress > 1.0f)
		myStateProgress = 1.0f;
}

void WeaponSwingAction::UpdateSwing() noexcept
{
	const f32 substeps = 50.f;
	const f32 substepThreshold = 1.0f / substeps;
	while (myPreviousStateProgress + substepThreshold < myStateProgress)
	{
		myPreviousStateProgress += substepThreshold;
		SetSwingTransform(myPreviousStateProgress * 0.95f);
	}

	SetSwingTransform(myStateProgress * 0.95f);

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	const f32 oldStateProgress = myStateProgress;

	IncreaseProgress((deltaTime / mySwing.SwingTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate);

	const f32 progressDelta = myStateProgress - oldStateProgress;
	myUserTransform.Move(myUserTransform.GetForwardVector() * progressDelta * mySwing.StepLength);
}

void WeaponSwingAction::UpdateBackswing() noexcept 
{
	const f32 inverseProgress = 1.0f - myStateProgress;
	const f32 inverseQuadraticProgress = 1.0f - ((inverseProgress) * (inverseProgress));

	//slowly wind down the swing
	SetSwingTransform(0.95f + inverseQuadraticProgress * 0.05f);

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	IncreaseProgress((deltaTime / mySwing.BackswingTime) * myWeaponFunctionality.GetWeapon().myBaseWeaponSwingRate);
}
