#pragma once

#include "Input/InputSystem.h"
#include "Movement/MovementInputFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"

#include "Character/CharacterFunctionality.h"

#include "Weapons/MeleeWeaponUserFunctionality.h"
#include "Collision/CollisionFunctionality.h"

#include "Functionalities/TransformFunctionality.h"
#include "Functionalities/TimerFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"

class MeleeWeaponDesigner;
class InputSystem;

class EnemyControllerFunctionality;
class TimeToLiveFunctionality;

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
		Policy::Set<
			PAdd<MeleeWeaponDesigner, PWrite>,
			PAdd<InputSystem, PWrite>,
			PAdd<FunctionalitySystemDelegate<EnemyControllerFunctionality>, PWrite>,
			PAdd<FunctionalitySystemDelegate<TimeToLiveFunctionality>, PWrite>,
			PAdd<TransformFunctionality, PWrite>,
			PAdd<MovementInputFunctionality, PWrite>,
			PAdd<DefaultMovementFunctionality, PWrite>,
			PAdd<MeleeWeaponUserFunctionality, PWrite>,
			PAdd<ActionFunctionality, PWrite>,
			PAdd<CollisionFunctionality, PWrite>,
			PAdd<CharacterFunctionality, PWrite>,
			PAdd<FactionFunctionality, PWrite>,
			PAdd<TimerFunctionality, PWrite>,
			PAdd<SLevelDenizenComponent, PWrite>,
			PAdd<SpriteRenderingFunctionality, PWrite>>>
	, public Debugged
{
public:
	PlayerControllerFunctionality();

	void Tick() noexcept;

	virtual void Debug() override;

private:
	void UpdateMovement();

	void OnDeath();
};
