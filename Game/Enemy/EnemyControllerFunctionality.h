#pragma once

#include "Character/CharacterFunctionality.h"

#include "Enemy/EnemyControllerComponent.h"
#include "Enemy/TargetFunctionality.h"

#include "Movement/DefaultMovementFunctionality.h"
#include "Movement/MovementInputFunctionality.h"

#include "Functionalities/TransformFunctionality.h"

class EnemyControllerFunctionality
	: public Functionality<
		EnemyControllerFunctionality,
		EnemyControllerComponent,
		TransformFunctionality,
		TargetFunctionality,
		MovementInputFunctionality,
		DefaultMovementFunctionality,
		CharacterFunctionality>
	, public Debugged
{
public:
	EnemyControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

	virtual void Debug() override;

private:
};

