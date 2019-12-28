#include "pch.h"
#include "EnemyControllerFunctionality.h"

#include "Enemy/TargetFunctionality.h"

EnemyControllerFunctionality::EnemyControllerFunctionality(Object& aOwner)
	: Super(aOwner)
{
	Get<CharacterFunctionality>().SetFaction(EFaction::Enemy);

	Get<DefaultMovementFunctionality>().Get<SDefaultMovementComponent>().myFriction = 15.f;
	Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 10.f);

	Get<ImpulseFunctionality>().Subscribe<SDiedImpulse>(*this, [&] (auto) 
		{
			OnDeath();
		});

	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/enemy.png");
}

void EnemyControllerFunctionality::Tick() noexcept
{
	auto& movementInputFunctionality = Get<MovementInputFunctionality>();
	movementInputFunctionality.SetMovementInput({ 0.f, 0.f });

	if (Get<CharacterFunctionality>().IsDead())
		return;

	const auto& targetFunctionality = Get<TargetFunctionality>();
	auto& enemyControllerComponent = Get<EnemyControllerComponent>();

	const auto target = targetFunctionality.FindTarget([&](CharacterFunctionality& aCharacter)
		{
			return !aCharacter.IsFriendlyWith(Get<CharacterFunctionality>().GetFaction());
		});

	if (!target)
		return;

	const SVector2f delta = target->GetPosition() - Get<TransformFunctionality>().GetPosition();

	if (delta.LengthSquared() < std::pow(enemyControllerComponent.FollowRangeMin, 2))
	{
		movementInputFunctionality.SetMovementInput({ 0.f, 0.f });

		return;
	}

	movementInputFunctionality.SetMovementInput(delta.GetNormalized());
}

void EnemyControllerFunctionality::Debug()
{
	auto& defaultMovementFunctionality = Get<DefaultMovementFunctionality>();
	auto& enemyControllerComponent = Get<EnemyControllerComponent>();

	ImGui::Begin("Enemy controller");

	defaultMovementFunctionality.Inspect();

	ImGui::SliderFloat("Follow Range Min", &enemyControllerComponent.FollowRangeMin, 0, 500);

	ImGui::End();
}

void EnemyControllerFunctionality::OnDeath()
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/deadEnemy.png");
	WmLog("ded");
}
