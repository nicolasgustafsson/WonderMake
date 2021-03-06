#pragma once
#include "MovementInputFunctionality.h"
#include "Collision/CollisionFunctionality.h"

class CollisionSystem;
class TimeKeeper;

struct SDefaultMovementComponent
	: public SComponent
{
	SVector2f CurrentVelocity;
	float AccelerationSpeed = 50000.f;
	float Friction = 30.0f;
	float MaxMovementSpeed = 911.f;
	
	static const i32 CollisionIterations = 4;
	i32 CollisionIterationsLeft = CollisionIterations;
	bool Collided = false;
};

class DefaultMovementFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionSystem, PWrite>,
			PAdd<TimeKeeper, PRead>,
			PAdd<SMovementInputComponent, PWrite>,
			PAdd<STransformComponent2D, PWrite>,
			PAdd<SDefaultMovementComponent, PWrite>>>
{
public:
	void AddForce(const SVector2f aForce);
	void AddImpulse(const SVector2f aImpulse);

	void Tick() noexcept;

	void Inspect();

private:
	void HandleCollision(const Colliders::SSphere& aThisSphere, const Colliders::SCollisionLine& aOtherLine);
	void TestCollision();
};
