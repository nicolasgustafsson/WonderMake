#include "pch.h"
#include "PlayerControllerFunctionality.h"
#include "Weapons/MeleeWeapon.h"


PlayerControllerFunctionality::PlayerControllerFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void PlayerControllerFunctionality::Tick() noexcept
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
	if (myInputSystem->IsMouseButtonPressed(EMouseButton::Left))
		Get<MeleeWeaponUserFunctionality>().SwingWeapon();

	Get<MovementInputFunctionality>().SetMovementInput(movementInput);
	const SVector2f position = Get<TransformFunctionality>().GetPosition();

	WmDrawDebugLine(Get<TransformFunctionality>().GetPosition(), myInputSystem->GetMousePositionInWorld(), SColor::White);
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

	static f32 Power = 100.f;
	if (ImGui::Button("Generate held weapon"))
	{
		Get<MeleeWeaponUserFunctionality>().SetWeapon(MeleeWeapon(Power));
	}

	ImGui::SliderFloat("Weapon Power", &Power, 0.f, 10000.f, "%.3f", 2.0f);
	 
	ImGui::End();
}
