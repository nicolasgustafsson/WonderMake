#pragma once
#include "Actions/Action.h"
#include "Character/CharacterFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Weapons/WeaponSwing.h"

class WeaponSwingHitShapeAction
	: public Action
{
public:
	WeaponSwingHitShapeAction(CharacterFunctionality& aUser, const SSwing aSwing) noexcept;

	virtual void BeginAction() override;

	virtual void Tick() override;

	virtual bool IsCompleted() const override;

private:
	const SSwing mySwing;
	CharacterFunctionality& myUser;
	f32 myCooldown = 0.0f;
};
