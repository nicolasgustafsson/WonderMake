#include "pch.h"
#include "DefaultMovementFunctionality.h"


DefaultMovementFunctionality::DefaultMovementFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void DefaultMovementFunctionality::AddForce(const SVector2f aForce)
{
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();

	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();

	movementComponent.myCurrentVelocity += aForce * deltaTime;
}

void DefaultMovementFunctionality::AddImpulse(const SVector2f aImpulse)
{
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();
	movementComponent.myCurrentVelocity += aImpulse;
}

void DefaultMovementFunctionality::Tick() noexcept
{
	STransformComponent& transform = Get<STransformComponent>();
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();
	const SVector2f movementInput = Get<SMovementInputComponent>().myMovementInput;
	const f32 deltaTime = myTimeKeeper->GetDeltaSeconds();

	movementComponent.myCurrentVelocity += movementInput * movementComponent.myAccelerationSpeed * deltaTime;

	if (movementComponent.myCurrentVelocity.LengthSquared() > movementComponent.myMaxMovementSpeed * movementComponent.myMaxMovementSpeed)
	{
		movementComponent.myCurrentVelocity.Normalize();
		movementComponent.myCurrentVelocity *= movementComponent.myMaxMovementSpeed;
	}

	const SVector2f friction = -movementComponent.myCurrentVelocity * movementComponent.myFriction;

	movementComponent.myCurrentVelocity += friction * deltaTime;

	transform.Position += movementComponent.myCurrentVelocity * deltaTime;
}

void DefaultMovementFunctionality::Inspect()
{
	SDefaultMovementComponent& movement = Get<SDefaultMovementComponent>();

	ImGui::SliderFloat("acceleration", &movement.myAccelerationSpeed, 0, 30000, "%.3f", 2.0f);
	ImGui::SliderFloat("friction", &movement.myFriction, 0.f, 20.0f, "%.3f", 2.0f);
	ImGui::SliderFloat("maxspeed", &movement.myMaxMovementSpeed, 300, 3000, "%.3f", 2.0f);

	ImGui::Text("velocity: %f", movement.myCurrentVelocity.Length());
}
