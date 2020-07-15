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
		EnemyControllerFunctionality,
		Policy::Set<
			Policy::Add<EnemyControllerComponent, Policy::EPermission::Write>,
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<TargetFunctionality, Policy::EPermission::Write>,
			Policy::Add<MovementInputFunctionality, Policy::EPermission::Write>,
			Policy::Add<DefaultMovementFunctionality, Policy::EPermission::Write>,
			Policy::Add<CharacterFunctionality, Policy::EPermission::Write>,
			Policy::Add<CollisionFunctionality, Policy::EPermission::Write>,
			Policy::Add<FactionFunctionality, Policy::EPermission::Write>,
			Policy::Add<ImpulseFunctionality, Policy::EPermission::Write>,
			Policy::Add<SpriteRenderingFunctionality, Policy::EPermission::Write>,
			Policy::Add<ActionFunctionality, Policy::EPermission::Write>>>
	, public Debugged
{
public:
	EnemyControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

	virtual void Debug() override;

private:

	void OnDeath();
};

