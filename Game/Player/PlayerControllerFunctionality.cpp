#include "pch.h"
#include "PlayerControllerFunctionality.h"
#include "Weapons/MeleeWeapon.h"
#include "Designers/MeleeWeaponDesigner/MeleeWeaponDesigner.h"
#include <Enemy/EnemyControllerFunctionality.h>
#include <UtilityFunctionalities/TimeToLiveFunctionality.h>
#include <Levels/LevelFunctionality.h>


PlayerControllerFunctionality::PlayerControllerFunctionality(Object& aOwner)
	: Super(aOwner), Debugged("Player Controller")
{
	CollisionFunctionality& collision = Get<CollisionFunctionality>();
	auto& collider = collision.AddSphereCollider(*this, SVector2f::Zero(), 10.f);

	Get<FactionFunctionality>().SetFaction(EFaction::Player);
	Get<CharacterFunctionality>().Get<CharacterStatsFunctionality>().SetBaseValue(ECharacterStat::MovementSpeed, 200.f);

	Get<ImpulseFunctionality>().Subscribe<SDiedImpulse>(*this, [&](auto) 
		{
			OnDeath();
		});

	Get<MeleeWeaponUserFunctionality>().SetWeapon(SystemPtr<MeleeWeaponDesigner>()->DesignWeapon());

	Get<SLevelDenizenComponent>().PersistentOnLevelChange = true;
}

void PlayerControllerFunctionality::Tick() noexcept
{
	if (Get<CharacterFunctionality>().IsDead())
		return;

	Action* currentAction = Get<ActionFunctionality>().GetCurrentAction();

	const bool canMove = !(currentAction && currentAction->BlocksMovementInput());
	if (canMove)
		UpdateMovement();
	else
		Get<MovementInputFunctionality>().SetMovementInput({0.f, 0.f});

	if (myInputSystem->IsMouseButtonPressed(EMouseButton::Left))
		Get<MeleeWeaponUserFunctionality>().SwingWeapon();

	WmDrawDebugLine(Get<TransformFunctionality>().GetPosition(), myInputSystem->GetMousePositionInWorld(), SColor::White);
}

void PlayerControllerFunctionality::UpdateMovement()
{
	SVector2f movementInput;

	if (myInputSystem->IsKeyDown(EKeyboardKey::A) || myInputSystem->IsKeyDown(EKeyboardKey::Left))
		movementInput += {-1.f, 0.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::W) || myInputSystem->IsKeyDown(EKeyboardKey::Up))
		movementInput += {0.f, -1.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::S) || myInputSystem->IsKeyDown(EKeyboardKey::Down))
		movementInput += {0.f, 1.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::D) || myInputSystem->IsKeyDown(EKeyboardKey::Right))
		movementInput += {1.f, 0.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::Backspace))
		Get<TransformFunctionality>().SetPosition(SVector2f::Zero());

	if (myInputSystem->IsKeyDown(EKeyboardKey::Enter))
	{
		LevelFunctionality* level = Get<SLevelDenizenComponent>().Level;
		if (level)
		{
			Object enemy;
			enemy.Add<EnemyControllerFunctionality>().Get<TransformFunctionality>().SetPosition(Get<TransformFunctionality>().GetPosition());
			enemy.Add<TimeToLiveFunctionality>().SetTimeToLive(5.f);
			
			level->AddDenizen(std::move(enemy));
		}
	}

	if (movementInput != SVector2f::Zero())
		Get<TransformFunctionality>().FaceDirection(movementInput);

	Get<MovementInputFunctionality>().SetMovementInput(movementInput);
}

void PlayerControllerFunctionality::OnDeath()
{
	Get<SpriteRenderingFunctionality>().SetTexture("Textures/deadPlayer.png");

	WmDispatchMessage(SPlayerDiedMessage());

	Get<TimerFunctionality>().AddTimer(3.f, []() {WmLog("wao!"); });
	Get<TimerFunctionality>().AddTimer(5.f, [&]() {WmLog("do it again!"); 	Get<TimerFunctionality>().AddTimer(5.f, [&]() {WmLog("and again!"); }); });
}

void PlayerControllerFunctionality::Debug()
{
	ImGui::Begin("Player controller");

	Get<DefaultMovementFunctionality>().Inspect();

	Get<TransformFunctionality>().Inspect();

	Get<MeleeWeaponUserFunctionality>().Inspect();

	Get<CharacterFunctionality>().Inspect();

	ImGui::End();
}
