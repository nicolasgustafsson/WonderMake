#include "pch.h"
#include "PunchAction.h"

PunchAction::PunchAction(CharacterFunctionality& aUser, TransformFunctionality& aTarget) noexcept
	: myUser(aUser), myTarget(aTarget)
{
	
}

void PunchAction::BeginAction()
{	
	myUser.Get<TransformFunctionality>().FacePosition(myTarget.GetPosition());

	HitShapeSpawnerFunctionality& hitShapeSpawner = myUser.Get<HitShapeSpawnerFunctionality>();

	hitShapeSpawner.SpawnPunch(50);
	hitShapeSpawner.SpawnPunch(75);
	hitShapeSpawner.SpawnPunch(100);
}

void PunchAction::Tick()
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	myCooldown -= deltaTime;
}

bool PunchAction::IsCompleted() const
{
	return myCooldown < 0.f;
}
