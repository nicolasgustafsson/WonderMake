#include "pch.h"
#include "WeaponSwingHitShapeAction.h"
#include "Input/InputSystem.h"

WeaponSwingHitShapeAction::WeaponSwingHitShapeAction(CharacterFunctionality& aUser, const SSwing aSwing) noexcept
	: myUser(aUser), mySwing(aSwing)
{

}

void WeaponSwingHitShapeAction::BeginAction()
{
	myUser.Get<TransformFunctionality>().FacePosition(SystemPtr<InputSystem>()->GetMousePositionInWorld());

	HitShapeSpawnerFunctionality& hitShapeSpawner = myUser.Get<HitShapeSpawnerFunctionality>();

	hitShapeSpawner.SpawnSwordSwing(mySwing.SwingPath, mySwing.ChargeTime, mySwing.SwingTime, 50.f, 100.f);

	myCooldown = mySwing.ChargeTime + mySwing.SwingTime;
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