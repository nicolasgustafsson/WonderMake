#include "pch.h"
#include "EnemyControllerFunctionality.h"

#include "Enemy/TargetFunctionality.h"
#include "Actions/ActionFunctionality.h"
#include "EnemyActions/PunchAction.h"
#include "Utility/Palette.h"

REGISTER_COMPONENT(EnemyControllerComponent)
REGISTER_FUNCTIONALITY(EnemyControllerFunctionality);

EnemyControllerFunctionality::EnemyControllerFunctionality()
	: Debugged("Enemy Controller")
{
	Get<FactionFunctionality>().SetFaction(EFaction::Enemy);

	Get<CharacterFunctionality>().Get<CharacterStatsFunctionality>().SetBaseValue(ECharacterStat::MovementSpeed, 250.f);
	Get<DefaultMovementFunctionality>().Get<SDefaultMovementComponent>().Friction = 15.f;
	Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), 10.f);

	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/enemy.png");
	Get<SpriteRenderingFunctionality>().SetColor(Palette::EnemyColor);
}

void EnemyControllerFunctionality::Tick() noexcept
{
	auto& movementInputFunctionality = Get<MovementInputFunctionality>();
	movementInputFunctionality.SetMovementInput({ 0.f, 0.f });

	if (Get<CharacterFunctionality>().IsDead())
		return;

	if (Get<ActionFunctionality>().IsInAction())
		return;

	auto& targetFunctionality = Get<TargetFunctionality>();
	auto& enemyControllerComponent = Get<EnemyControllerComponent>();

	const auto target = targetFunctionality.FindTarget([&](CharacterFunctionality& aCharacter)
		{
			return !aCharacter.Get<FactionFunctionality>().IsFriendlyWith(Get<FactionFunctionality>().GetFaction());
		});

	if (!target)
		return;

	const SVector2f delta = target->GetPosition() - Get<TransformFunctionality>().GetPosition();

	if (delta.LengthSquared() < std::pow(enemyControllerComponent.FollowRangeMin, 2))
	{
		movementInputFunctionality.SetMovementInput({ 0.f, 0.f });

		PunchAction action(Get<CharacterFunctionality>(), *target);

		Get<ActionFunctionality>().StartAction(action);
		return;
	}

	movementInputFunctionality.SetMovementInput(delta.GetNormalized());

	Get<TransformFunctionality>().FaceDirection(movementInputFunctionality.Get<SMovementInputComponent>().myMovementInput);
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
