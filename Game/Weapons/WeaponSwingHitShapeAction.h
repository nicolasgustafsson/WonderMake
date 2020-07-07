#pragma once
#include "Actions/Action.h"
#include "Character/CharacterFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

class WeaponSwingHitShapeAction
	: public Action
{
public:
	WeaponSwingHitShapeAction(CharacterFunctionality& aUser) noexcept;

	virtual void BeginAction() override;

	virtual void Tick() override;

	virtual bool IsCompleted() const override;

private:
	CharacterFunctionality& myUser;
	f32 myCooldown = 0.3f;
};
