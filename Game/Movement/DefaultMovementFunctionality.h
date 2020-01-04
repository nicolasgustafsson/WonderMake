#pragma once
#include "MovementInputFunctionality.h"
#include "Collision/CollisionFunctionality.h"

struct SDefaultMovementComponent
	: public SComponent
{
	SVector2f CurrentVelocity;
	float AccelerationSpeed = 6700.f;
	float Friction = 7.0f;
	float MaxMovementSpeed = 911.f;
	
	static const i32 CollisionIterations = 4;
	i32 CollisionIterationsLeft = CollisionIterations;
	bool Collided = false;
};

class DefaultMovementFunctionality
	: public Functionality<DefaultMovementFunctionality, SMovementInputComponent, STransformComponent, SDefaultMovementComponent>
{
public:
	DefaultMovementFunctionality(Object& aOwner);

	void AddForce(const SVector2f aForce);
	void AddImpulse(const SVector2f aImpulse);

	void Tick() noexcept;

	void Inspect();

	SystemPtr<TimeKeeper> myTimeKeeper;

private:
	void HandleCollision(const Colliders::SSphere& aThisSphere, const Colliders::SLine& aOtherLine);
	void TestCollision();
};

