#include "pch.h"
#include "EnemyControllerFunctionality.h"

#include "Enemy/TargetFunctionality.h"

EnemyControllerFunctionality::EnemyControllerFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void EnemyControllerFunctionality::Tick() noexcept
{
	const auto& targetFunctionality = Get<TargetFunctionality>();
	auto& movementInputFunctionality = Get<MovementInputFunctionality>();
	auto& enemyControllerComponent = Get<SEnemyControllerComponent>();

	const auto target = targetFunctionality.FindTarget([](TransformFunctionality& /*aTranform*/)
		{
			return true;
		});

	if (!target)
	{
		movementInputFunctionality.SetMovementInput({ 0.f, 0.f});

		return;
	}

	const SVector2f delta = target->GetPosition() - Get<TransformFunctionality>().GetPosition();

	if (delta.LengthSquared() < std::pow(enemyControllerComponent.FollowRangeMin, 2))
	{
		movementInputFunctionality.SetMovementInput({ 0.f, 0.f });

		return;
	}

	movementInputFunctionality.SetMovementInput(delta.GetNormalized());
}
