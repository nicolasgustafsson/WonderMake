#include "pch.h"
#include "DefaultMovementFunctionality.h"
#include "Collision/CollisionFunctionality.h"
#include "Physics/StaticGeometryFunctionality.h"

REGISTER_COMPONENT(SDefaultMovementComponent);
REGISTER_FUNCTIONALITY(DefaultMovementFunctionality);

void DefaultMovementFunctionality::AddForce(const SVector2f aForce)
{
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();

	const f32 deltaTime = Get<TimeKeeper>().GetDeltaSeconds();
	movementComponent.CurrentVelocity += aForce * deltaTime;
}

void DefaultMovementFunctionality::AddImpulse(const SVector2f aImpulse)
{
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();
	movementComponent.CurrentVelocity += aImpulse;
}

void DefaultMovementFunctionality::Tick() noexcept
{
	STransformComponent& transform = Get<STransformComponent>();
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();

	movementComponent.CollisionIterationsLeft = movementComponent.CollisionIterations;

	const SVector2f movementInput = Get<SMovementInputComponent>().myMovementInput;
	const f32 deltaTime = Get<TimeKeeper>().GetDeltaSeconds();

	movementComponent.CurrentVelocity += movementInput * movementComponent.AccelerationSpeed * deltaTime;

	if (movementComponent.CurrentVelocity.LengthSquared() > movementComponent.MaxMovementSpeed * movementComponent.MaxMovementSpeed)
	{
		movementComponent.CurrentVelocity.Normalize();
		movementComponent.CurrentVelocity *= movementComponent.MaxMovementSpeed;
	}

	const f32 friction = 1.f / (1.f + (deltaTime * movementComponent.Friction));


	transform.Position += movementComponent.CurrentVelocity * deltaTime;

	movementComponent.CurrentVelocity *= friction;

	do
	{
		TestCollision();
	}                                    //avoid infinite loops if we are stuck inside geometry
	while (movementComponent.Collided && movementComponent.CollisionIterationsLeft > 0);
}

void DefaultMovementFunctionality::Inspect()
{
	SDefaultMovementComponent& movement = Get<SDefaultMovementComponent>();

	ImGui::SliderFloat("acceleration", &movement.AccelerationSpeed, 0, 30000, "%.3f", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat("friction", &movement.Friction, 0.f, 20.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat("maxspeed", &movement.MaxMovementSpeed, 300, 3000, "%.3f", ImGuiSliderFlags_Logarithmic);

	ImGui::Text("velocity: %f", movement.CurrentVelocity.Length());
}

void DefaultMovementFunctionality::HandleCollision(const Colliders::SSphere& aThisSphere, const Colliders::SCollisionLine& aOtherLine)
{
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();
	STransformComponent& transform = Get<STransformComponent>();
	
	const SVector2f closestPointOnLine = CollisionSystem::GetClosestPointOnLine(aOtherLine, aThisSphere.Position);

	const SVector2f correctionDirection = (aThisSphere.Position - closestPointOnLine).GetNormalized();

	transform.Position = closestPointOnLine + correctionDirection * (aThisSphere.Radius + 0.01f);

	movementComponent.Collided = true;
}

void DefaultMovementFunctionality::TestCollision()
{
	STransformComponent& transform = Get<STransformComponent>();
	SDefaultMovementComponent& movementComponent = Get<SDefaultMovementComponent>();

	movementComponent.Collided = false;

	movementComponent.CollisionIterationsLeft -= 1;

	Get<CollisionSystem>().OverlapSphereAgainstFunctionality<StaticGeometryFunctionality>(transform.Position, 15.f,
		[&](auto&, Colliders::SCollisionInfo aCollisionInfo)
		{
			std::visit([&](auto& aOtherCollider)
				{
					using TOther = std::decay_t<decltype(aOtherCollider)>;
					if constexpr (std::is_same_v<TOther, Colliders::SCollisionLine>)
					{
						std::visit([&](auto& aThisCollider)
							{
								using TThis = std::decay_t<decltype(aThisCollider)>;
								if constexpr (std::is_same_v<TThis, Colliders::SSphere>)
								{
									HandleCollision(aThisCollider, aOtherCollider);
								}
								else
								{
									WmLog("Physics only works between spheres and lines for now!");
								}
							}, aCollisionInfo.OriginalCollider);
					}
					else
					{
						WmLog("Physics only works between spheres and lines for now!");
					}

				}, aCollisionInfo.OtherCollider);
		});

}
