#pragma once
#include "Actions/Action.h"
#include "Weapons/WeaponSwing.h"

class TransformFunctionality;
class SpriteRenderingFunctionality;
class MeleeWeapon;
class MeleeWeaponFunctionality;

enum class ESwingState
{
	Charge,
	Swing,
	Backswing
};

class WeaponSwingAction
	: public Action
{
public:
	WeaponSwingAction(MeleeWeaponFunctionality& aMeleeWeaponFunctionality, TransformFunctionality& aUserTransform, const SSwing aSwing);

	virtual void BeginAction() override;

	virtual void Tick() override;

	virtual bool IsCompleted() override;

	virtual void EndAction() override;

	virtual bool CanBeInterrupted() override;

private:
	void SetSwingTransform(const f32 aPercentageInSwing);
	void UpdateCharge();
	void UpdateSwing();
	void UpdateBackswing();

	TransformFunctionality& myUserTransform;
	MeleeWeaponFunctionality& myWeaponFunctionality;
	const SSwing mySwing;
	
	ESwingState myCurrentState = ESwingState::Charge;
	f32 myStateProgress = 0.f;
};

