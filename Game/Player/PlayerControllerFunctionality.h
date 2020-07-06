#pragma once

#include "Input/InputSystem.h"
#include "Movement/MovementInputFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"

#include "Character/CharacterFunctionality.h"

#include "Weapons/MeleeWeaponUserFunctionality.h"
#include "Collision/CollisionFunctionality.h"

#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/OwnerFunctionality.h"
#include "Functionalities/ImpulseFunctionality.h"
#include <Functionalities/TimerFunctionality.h>
#include <Levels/LevelDenizenFunctionality.h>

struct SCoolImpulse
	: public SObjectImpulse<SCoolImpulse>
{

};

struct SPlayerDiedMessage
	: public Message<SPlayerDiedMessage>
{
	
};

class PlayerControllerFunctionality
	: public Functionality<
		PlayerControllerFunctionality,
		Policy::Set<
			Policy::Add<TransformFunctionality, Policy::EPermission::Write>,
			Policy::Add<MovementInputFunctionality, Policy::EPermission::Write>,
			Policy::Add<DefaultMovementFunctionality, Policy::EPermission::Write>,
			Policy::Add<OwnerFunctionality, Policy::EPermission::Write>,
			Policy::Add<MeleeWeaponUserFunctionality, Policy::EPermission::Write>,
			Policy::Add<ActionFunctionality, Policy::EPermission::Write>,
			Policy::Add<CollisionFunctionality, Policy::EPermission::Write>,
			Policy::Add<CharacterFunctionality, Policy::EPermission::Write>,
			Policy::Add<ImpulseFunctionality, Policy::EPermission::Write>,
			Policy::Add<FactionFunctionality, Policy::EPermission::Write>,
			Policy::Add<TimerFunctionality, Policy::EPermission::Write>,
			Policy::Add<SLevelDenizenComponent, Policy::EPermission::Write>,
			Policy::Add<SpriteRenderingFunctionality, Policy::EPermission::Write>>>
	, public Debugged
{
public:
	PlayerControllerFunctionality(Object& aOwner);

	void Tick() noexcept;

	virtual void Debug() override;

private:
	void UpdateMovement();

	SystemPtr<InputSystem> myInputSystem;
	void OnDeath();
};

