#include "pch.h"
#include "WeaponSwingHitShapeAction.h"
#include "Input/InputSystem.h"

WeaponSwingHitShapeAction::WeaponSwingHitShapeAction(CharacterFunctionality& aUser, SVector2f aDirection, const SAttackMove aSwing) noexcept
	: myUser(aUser), myDirection(aDirection), mySwing(aSwing)
{

}

void WeaponSwingHitShapeAction::BeginAction()
{
	myUser.Get<TransformFunctionality2D>().FaceDirection(myDirection);

	HitShapeSpawnerFunctionality& hitShapeSpawner = myUser.Get<HitShapeSpawnerFunctionality>();

	hitShapeSpawner.SpawnSwordSwing(mySwing.AttackPath, mySwing.ChargeTime, mySwing.SwingTime, 50.f, 100.f * mySwing.DamageMultipiler);

	myCooldown = mySwing.ChargeTime + mySwing.SwingTime;
}

void WeaponSwingHitShapeAction::Tick(const f32 aDeltaTime)
{
	myCooldown -= aDeltaTime;
}

bool WeaponSwingHitShapeAction::IsCompleted() const
{
	return myCooldown < 0.f;
}