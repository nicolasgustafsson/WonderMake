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

	hitShapeSpawner.SpawnPunch(100.f, 0.5f, 0.1f, 50.f, 20.f);
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
