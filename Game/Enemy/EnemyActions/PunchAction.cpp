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

	//hitShapeSpawner.SpawnPunch(60, 0.5f, 0.1f, 20.f, 20.f);

	hitShapeSpawner.SpawnPunch(60.f, 0.5f, 0.1f, 30.f, 20.f);
}

void PunchAction::Tick(const f32 aDeltaTime)
{
	myCooldown -= aDeltaTime;
}

bool PunchAction::IsCompleted() const
{
	return myCooldown < 0.f;
}
