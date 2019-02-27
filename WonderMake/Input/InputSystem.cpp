#include "stdafx.h"
#include "InputSystem.h"
#include <GLFW/glfw3.h>
#include <algorithm>

void InputSystem::Update() noexcept
{
	UpdateKeyboard();
	UpdateMouse();
	UpdateGamepad();
}

void InputSystem::UpdateKeyboard() noexcept
{
	for (u32 i = 0; i < KeyboardKeyCount; i++)
	{
		const i32 glfwKeyState = glfwGetKey(myWindowSystemPtr->myGlfwWindow, InputUtility::GetGlfwKey(static_cast<EKeyboardKey>(i)));

		const bool bIsPressed = glfwKeyState == GLFW_PRESS;

		const EInputItemState startingState = myKeyboardKeyStates[i];

		myKeyboardKeyStates[i] = GetNewInputState(startingState, bIsPressed);
	}
}

void InputSystem::UpdateMouse() noexcept
{
	for (u32 i = 0; i < MouseButtonCount; i++)
	{
		const i32 glfwKeyState = glfwGetMouseButton(myWindowSystemPtr->myGlfwWindow, InputUtility::GetGlfwMouseButton(static_cast<EMouseButton>(i)));

		const bool bIsPressed = glfwKeyState == GLFW_PRESS;

		const EInputItemState startingState = myMouseButtonStates[i];

		myMouseButtonStates[i] = GetNewInputState(startingState, bIsPressed);
	}
}

void InputSystem::UpdateGamepad() noexcept
{
	i32 gamepadButtonCount;
	const u8* inputArray = glfwGetJoystickButtons(0, &gamepadButtonCount);

	//no gamepad present
	if (gamepadButtonCount == 0)
		return;

	gamepadButtonCount = std::min(static_cast<u32>(gamepadButtonCount), GamepadButtonCount);

	for (u32 i = 0; i < static_cast<u32>(gamepadButtonCount); i++)
	{
		const bool isPressed = inputArray[i] == GLFW_PRESS;

		const EInputItemState startingState = myGamepadButtonStates[i];

		myGamepadButtonStates[i] = GetNewInputState(startingState, isPressed);
	}
}

constexpr EInputItemState InputSystem::GetNewInputState(const EInputItemState aOldState, const bool aIsPressed) const noexcept
{
	if (aIsPressed)
	{
		if (IsStateUp(aOldState))
			return EInputItemState::Pressed;

		return EInputItemState::Down;
	}

	if (IsStateDown(aOldState))
		return EInputItemState::Released;

	return EInputItemState::Up;
}

void InputSystem::Debug() noexcept
{
	ImGui::Begin("InputSystem");

	if (ImGui::CollapsingHeader("Show Keyboard Keys"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (u32 i = 0; i < KeyboardKeyCount; i++)
		{
			const EKeyboardKey key = static_cast<EKeyboardKey>(i);
			const EInputItemState state = myKeyboardKeyStates[i];

			std::string keyInfo = std::string(InputUtility::GetNameOfKey(key));
			keyInfo += ": " + std::string(InputUtility::GetNameOfState(state));
			ImGui::TextUnformatted(keyInfo.c_str());
		}
		ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("Show Mouse Buttons"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (u32 i = 0; i < MouseButtonCount; i++)
		{
			const EMouseButton mouseButton = static_cast<EMouseButton>(i);
			const EInputItemState state = myMouseButtonStates[i];

			std::string mouseInfo = std::string(InputUtility::GetNameOfMouseButton(mouseButton));
			mouseInfo += ": " + std::string(InputUtility::GetNameOfState(state));
			ImGui::TextUnformatted(mouseInfo.c_str());
		}
		ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("Show Gamepad Buttons"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (u32 i = 0; i < GamepadButtonCount; i++)
		{
			const EGamepadButton gamepadButton = static_cast<EGamepadButton>(i);
			const EInputItemState state = myGamepadButtonStates[i];

			std::string gamepadButtonInfo = std::string(InputUtility::GetNameOfGamepadButton(gamepadButton));
			gamepadButtonInfo += ": " + std::string(InputUtility::GetNameOfState(state));
			ImGui::TextUnformatted(gamepadButtonInfo.c_str());
		}
		ImGui::PopStyleVar();
	}

	ImGui::Text("Currently Pressed Keys");

	ImGui::Separator();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	for (u32 i = 0; i < KeyboardKeyCount; i++)
	{
		if (myKeyboardKeyStates[i] == EInputItemState::Down || myKeyboardKeyStates[i] == EInputItemState::Pressed)
			ImGui::Text(std::string(InputUtility::GetNameOfKey(static_cast<EKeyboardKey>(i))).c_str());
	}

	for (u32 i = 0; i < MouseButtonCount; i++)
	{
		if (myMouseButtonStates[i] == EInputItemState::Down || myMouseButtonStates[i] == EInputItemState::Pressed)
			ImGui::Text(std::string(InputUtility::GetNameOfMouseButton(static_cast<EMouseButton>(i))).c_str());
	}

	for (u32 i = 0; i < GamepadButtonCount; i++)
	{
		if (myGamepadButtonStates[i] == EInputItemState::Down || myGamepadButtonStates[i] == EInputItemState::Pressed)
			ImGui::Text(std::string(InputUtility::GetNameOfGamepadButton(static_cast<EGamepadButton>(i))).c_str());
	}

	ImGui::PopStyleVar();

	ImGui::End();
}