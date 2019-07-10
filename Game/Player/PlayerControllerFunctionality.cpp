#include "pch.h"
#include "PlayerControllerFunctionality.h"


PlayerControllerFunctionality::PlayerControllerFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void PlayerControllerFunctionality::Tick() noexcept
{
	SVector2f movementInput;

	if (myInputSystem->IsKeyDown(EKeyboardKey::A))
		movementInput += {-1.f, 0.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::W))
		movementInput += {0.f, -1.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::S))
		movementInput += {0.f, 1.f};
	if (myInputSystem->IsKeyDown(EKeyboardKey::D))
		movementInput += {1.f, 0.f};

	Get<MovementInputFunctionality>().SetMovementInput(movementInput);
}

void PlayerControllerFunctionality::Debug()
{
	ImGui::Begin("Player controller");

	Get<DefaultMovementFunctionality>().Inspect();

	ImGui::End();
}
