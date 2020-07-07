#include "pch.h"
#include "WeaponSwingHitShapeAction.h"
#include "Input/InputSystem.h"

WeaponSwingHitShapeAction::WeaponSwingHitShapeAction(CharacterFunctionality& aUser) noexcept
	: myUser(aUser)
{

}

void WeaponSwingHitShapeAction::BeginAction()
{
	myUser.Get<TransformFunctionality>().FacePosition(SystemPtr<InputSystem>()->GetMousePositionInWorld());

	HitShapeSpawnerFunctionality& hitShapeSpawner = myUser.Get<HitShapeSpawnerFunctionality>();

	hitShapeSpawner.SpawnPunch(100.f, 0.0f, 0.1f, 50.f, 100.f);
}

void WeaponSwingHitShapeAction::Tick()
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	myCooldown -= deltaTime;
}

bool WeaponSwingHitShapeAction::IsCompleted() const
{

	return myCooldown < 0.f;
}