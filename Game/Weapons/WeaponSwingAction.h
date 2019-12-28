#pragma once
#include "Actions/Action.h"
#include "Weapons/WeaponSwing.h"

class TransformFunctionality;
class SpriteRenderingFunctionality;
class MeleeWeapon;
class MeleeWeaponFunctionality;
class CharacterFunctionality;

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
	WeaponSwingAction(MeleeWeaponFunctionality& aMeleeWeaponFunctionality, TransformFunctionality& aUserTransform, const SSwing aSwing) noexcept;

	virtual void BeginAction() override;

	virtual void Tick() noexcept override;

	virtual bool IsCompleted() const noexcept override;

	virtual void EndAction() noexcept override;

	virtual bool CanBeInterrupted() const noexcept override;

private:
	void TestSwingCollision();
	void SetSwingTransform(const f32 aPercentageInSwing) noexcept;
	void UpdateCharge() noexcept;

	void IncreaseProgress(const f32 progressIncrease);

	void UpdateSwing() noexcept;
	void UpdateBackswing() noexcept;

	TransformFunctionality& myUserTransform;
	MeleeWeaponFunctionality& myWeaponFunctionality;
	const SSwing mySwing;
	
	ESwingState myCurrentState = ESwingState::Charge;
	f32 myStateProgress = 0.f;
	f32 myPreviousStateProgress = 0.f;

	std::vector<CharacterFunctionality*> hitCharacters;
};

