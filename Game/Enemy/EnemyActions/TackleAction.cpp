#include "pch.h"
#include "TackleAction.h"
#include "Functionalities/TransformFunctionality.h"
#include "Character/CharacterFunctionality.h"
#include "Movement/DefaultMovementFunctionality.h"

TackleAction::TackleAction(CharacterFunctionality& aUser, TransformFunctionality& aTarget) noexcept
{
	myCharacter = &aUser;
	myTarget = &aTarget;
}

void TackleAction::Tick()
{
	if (myChargeTime > 0.f)
		TickCharging();
	else if (myTackleTime > 0.f)
		TickTackling();
	else if (myCooldownTime > 0.f)
		TickCooldown();
}

bool TackleAction::IsCompleted() const
{
	return myCooldownTime <= 0.f;
}

void TackleAction::TickCharging()
{
	myChargeTime -= SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	myCharacter->Get<TransformFunctionality>().FacePosition(myTarget->GetPosition());
}

void TackleAction::StartTackling()
{
	myStartPosition = myCharacter->Get<TransformFunctionality>().GetPosition();

	const f32 movementSpeedMultiplier = myCharacter->Get<CharacterStatsFunctionality>().GetStatMultiplier(ECharacterStat::MovementSpeed);
	myEndPosition = (myTarget->GetPosition() - myStartPosition).GetNormalized() * myChargeDistance * movementSpeedMultiplier + myStartPosition;

	myVelocity = (myEndPosition - myStartPosition) / myTackleTime;

	myHasBegunTackling = true;
}

void TackleAction::TickTackling()
{
	if (!myHasBegunTackling)
		StartTackling();

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	myTackleTime -= deltaTime;

	myCharacter->Get<TransformFunctionality>().Move(myVelocity * deltaTime);

	const SVector2f selfPosition = myCharacter->Get<TransformFunctionality>().GetPosition();

	SystemPtr<CollisionSystem>()->OverlapSphereAgainstFunctionality<CharacterFunctionality>(
		myCharacter->Get<TransformFunctionality>().GetPosition(), 20.f, [&](CharacterFunctionality& aHitCharacter, Colliders::SCollisionInfo)
		{
			if (myCharacter->IsFriendlyWith(aHitCharacter.GetFaction()))
				return;

			if (std::find(myHitCharacters.begin(), myHitCharacters.end(), &aHitCharacter) != myHitCharacters.end())
				return;

			aHitCharacter.Damage(50.f * myCharacter->Get<CharacterStatsFunctionality>().GetStatMultiplier(ECharacterStat::MeleeAttackDamage));
			aHitCharacter.Get<DefaultMovementFunctionality>().AddImpulse((aHitCharacter.Get<TransformFunctionality>().GetPosition() - selfPosition).GetNormalized() * 1000.f);

			myHitCharacters.push_back(&aHitCharacter);
		});
}

void TackleAction::TickCooldown()
{
	myCooldownTime -= SystemPtr<TimeKeeper>()->GetDeltaSeconds();
}
