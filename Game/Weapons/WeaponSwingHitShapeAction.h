#pragma once
#include "Actions/Action.h"
#include "Character/CharacterFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Weapons/WeaponSwing.h"

class WeaponSwingHitShapeAction
	: public Action
{
public:
	WeaponSwingHitShapeAction(CharacterFunctionality& aUser, SVector2f aDirection, const SAttackMove aSwing) noexcept;

	virtual void BeginAction() override;

	virtual void Tick(const f32 aDeltaTime) override;

	virtual bool IsCompleted() const override;

private:
	const SAttackMove mySwing;
	SVector2f myDirection;
	CharacterFunctionality& myUser;
	f32 myCooldown = 0.0f;
};
