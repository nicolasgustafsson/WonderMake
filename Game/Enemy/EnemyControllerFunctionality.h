#pragma once

#include "Character/CharacterFunctionality.h"

#include "Enemy/EnemyControllerComponent.h"
#include "Enemy/TargetFunctionality.h"

#include "Movement/DefaultMovementFunctionality.h"
#include "Movement/MovementInputFunctionality.h"

#include "Functionalities/TransformFunctionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Actions/ActionFunctionality.h"
#include "UtilityFunctionalities/FactionFunctionality.h"

class EnemyControllerFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<EnemyControllerComponent, PWrite>,
			PAdd<TransformFunctionality, PWrite>,
			PAdd<TargetFunctionality, PWrite>,
			PAdd<MovementInputFunctionality, PWrite>,
			PAdd<DefaultMovementFunctionality, PWrite>,
			PAdd<CharacterFunctionality, PWrite>,
			PAdd<CollisionFunctionality, PWrite>,
			PAdd<FactionFunctionality, PWrite>,
			PAdd<SpriteRenderingFunctionality, PWrite>,
			PAdd<ActionFunctionality, PWrite>>>
	, public Debugged
{
public:
	EnemyControllerFunctionality();

	void Tick() noexcept;

	virtual void Debug() override;

private:

	void OnDeath();
};
