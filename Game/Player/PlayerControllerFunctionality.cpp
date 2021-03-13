#include "pch.h"
#include "PlayerControllerFunctionality.h"
#include "Weapons/MeleeWeapon.h"
#include "Designers/MeleeWeaponDesigner/MeleeWeaponDesigner.h"
#include "Enemy/EnemyControllerFunctionality.h"
#include "UtilityFunctionalities/TimeToLiveFunctionality.h"
#include "Levels/LevelFunctionality.h"
#include "Utility/Palette.h"

REGISTER_FUNCTIONALITY(PlayerControllerFunctionality);

PlayerControllerFunctionality::PlayerControllerFunctionality()
	: Debugged("Player Controller")
{
	CollisionFunctionality& collision = Get<CollisionFunctionality>();
	collision.AddSphereCollider(*this, SVector2f::Zero(), 10.f);

	Get<FactionFunctionality>().SetFaction(EFaction::Player);
	Get<CharacterFunctionality>().Get<CharacterStatsFunctionality>().SetBaseValue(ECharacterStat::MovementSpeed, 250.f);

	Get<MeleeWeaponUserFunctionality>().SetWeapon(Get<MeleeWeaponDesigner>().DesignWeapon());

	Get<SLevelDenizenComponent>().PersistentOnLevelChange = true;

	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/player.png");
	
	Get<SpriteRenderingFunctionality>().SetColor(Palette::PlayerColor);
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

	if (Get<InputSystem>().IsMouseButtonPressed(EMouseButton::Left))
		Get<MeleeWeaponUserFunctionality>().SwingWeapon();

	//WmDrawDebugLine(Get<TransformFunctionality>().GetPosition(), Get<InputSystem>().GetMousePositionInWorld(), SColor::White());
}

void PlayerControllerFunctionality::UpdateMovement()
{
	SVector2f movementInput;

	if (Get<InputSystem>().IsKeyDown(EKeyboardKey::A) || Get<InputSystem>().IsKeyDown(EKeyboardKey::Left))
		movementInput += {-1.f, 0.f};
	if (Get<InputSystem>().IsKeyDown(EKeyboardKey::W) || Get<InputSystem>().IsKeyDown(EKeyboardKey::Up))
		movementInput += {0.f, 1.f};
	if (Get<InputSystem>().IsKeyDown(EKeyboardKey::S) || Get<InputSystem>().IsKeyDown(EKeyboardKey::Down))
		movementInput += {0.f, -1.f};
	if (Get<InputSystem>().IsKeyDown(EKeyboardKey::D) || Get<InputSystem>().IsKeyDown(EKeyboardKey::Right))
		movementInput += {1.f, 0.f};
	if (Get<InputSystem>().IsKeyDown(EKeyboardKey::Backspace))
		Get<TransformFunctionality2D>().SetPosition(SVector2f::Zero());

	if (Get<InputSystem>().IsKeyDown(EKeyboardKey::Enter))
	{
		LevelFunctionality* level = Get<SLevelDenizenComponent>().Level;
		if (level)
		{
			Object enemy;
			Get<FunctionalitySystemDelegate<EnemyControllerFunctionality>>().AddFunctionality(enemy).Get<TransformFunctionality2D>().SetPosition(Get<TransformFunctionality2D>().GetPosition());
			Get<FunctionalitySystemDelegate<TimeToLiveFunctionality>>().AddFunctionality(enemy).SetTimeToLive(5.f);
			
			level->AddDenizen(std::move(enemy));
		}
	}

	Get<TransformFunctionality2D>().FaceDirection(SystemPtr<InputSystem>()->GetMousePositionInWorld() - Get<TransformFunctionality2D>().GetPosition());

//	WmDrawDebugLine(Get<TransformFunctionality2D>().GetPosition(), SystemPtr<InputSystem>()->GetMousePositionInWorld(), SColor::White);

	Get<MovementInputFunctionality>().SetMovementInput(movementInput);
}

void PlayerControllerFunctionality::OnDeath()
{
	Get<SpriteRenderingFunctionality>().SetTexture(std::filesystem::current_path() / "Textures/deadPlayer.png");

	WmDispatchMessage(SPlayerDiedMessage());
}

void PlayerControllerFunctionality::Debug()
{
	ImGui::Begin("Player controller");

	Get<DefaultMovementFunctionality>().Inspect();

	Get<TransformFunctionality2D>().Inspect();

	Get<MeleeWeaponUserFunctionality>().Inspect();

	Get<CharacterFunctionality>().Inspect();

	ImGui::End();
}
