#pragma once
#include "Actions/Action.h"
#include "Character/CharacterFunctionality.h"
#include "Functionalities/TransformFunctionality.h"

class PunchAction
	: public Action
{
public:
	PunchAction(CharacterFunctionality& aUser, TransformFunctionality& aTarget) noexcept;

	virtual void BeginAction() override;

	virtual void Tick(const f32 aDeltaTime) override;

	virtual bool IsCompleted() const override;

private:
	CharacterFunctionality& myUser;
	TransformFunctionality& myTarget;
	f32 myCooldown = 1.f;
};

