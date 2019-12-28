#pragma once
#include "Actions/Action.h"
#include "Functionalities/TransformFunctionality.h"

class CharacterFunctionality;

enum class ETackleState
{
	Charging,
	Tackling,
	Cooldown
};

class TackleAction
	: public Action
{
public:
	TackleAction(CharacterFunctionality& aUser, TransformFunctionality& aTarget) noexcept;

	virtual void Tick() override;
	virtual bool IsCompleted() const override;

private:
	void TickCharging();

	void StartTackling();

	void TickTackling();
	void TickCooldown();

	float myChargeTime = 2.f;
	float myTackleTime = 0.3f;
	float myCooldownTime = 2.f;

	const float myChargeDistance = 150.f;

	SVector2f myStartPosition;
	SVector2f myEndPosition;

	SVector2f myVelocity;

	CharacterFunctionality* myCharacter;
	TransformFunctionality* myTarget;

	std::vector<CharacterFunctionality*> myHitCharacters;
};

