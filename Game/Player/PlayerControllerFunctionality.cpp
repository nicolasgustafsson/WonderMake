#include "pch.h"
#include "PlayerControllerFunctionality.h"
#include "Weapons/MeleeWeapon.h"
#include "Designers/MeleeWeaponDesigner/MeleeWeaponDesigner.h"
#include "Enemy/EnemyControllerFunctionality.h"


PlayerControllerFunctionality::PlayerControllerFunctionality(Object& aOwner)
	: Super(aOwner)
{
	CollisionFunctionality& collision = Get<CollisionFunctionality>();
	auto& collider = collision.AddSphereCollider(*this, SVector2f::Zero(), 10.f);

	Get<CharacterFunctionality>().SetFaction(EFaction::Player);
}

void PlayerControllerFunctionality::Tick() noexcept
{
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

	if (movementInput != SVector2f::Zero())
		Get<TransformFunctionality>().FaceDirection(movementInput);

	Get<MovementInputFunctionality>().SetMovementInput(movementInput);
}

void PlayerControllerFunctionality::Debug()
{
	ImGui::Begin("Player controller");

	Get<DefaultMovementFunctionality>().Inspect();

	Get<TransformFunctionality>().Inspect();

	Get<MeleeWeaponUserFunctionality>().Inspect();

	if (ImGui::Button("Send cool impulse"))
	{
		WmSendObjectImpulse(Get<OwnerFunctionality>().GetOwner(), SCoolImpulse());
	}

	if (ImGui::Button("Generate held weapon"))
	{
		Get<MeleeWeaponUserFunctionality>().SetWeapon(SystemPtr<MeleeWeaponDesigner>()->DesignWeapon());
	}

	ImGui::End();
}
