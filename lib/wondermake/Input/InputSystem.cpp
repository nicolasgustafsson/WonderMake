#include "InputSystem.h"

#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Camera/Display.h"
#include "Debugging/DebugSettings.h"
#include "Program/GlfwFacade.h"

#include <GLFW/glfw3.h>

#include <algorithm>

WM_REGISTER_SYSTEM(InputSystem);

inline static size_t locDisplayIndex	= static_cast<size_t>(InputSystem::EFocus::Display);
inline static size_t locWindowIndex		= static_cast<size_t>(InputSystem::EFocus::Window);

InputSystem::InputSystem()
	: Debugged("Input", GetExecutor())
{
	myWindowSubscriberScroll = Get<Window>().OnScroll(GetExecutor(), [this](SVector2f aScrollDelta)
		{
			if (!DisplayHasMouseFocus())
				return;

			for (auto& onScroll : myScrollList)
				onScroll.Trigger(aScrollDelta);
		});
}

void InputSystem::Update() noexcept
{
	UpdateKeyboard();
	UpdateMouse();
	UpdateGamepad();
}

void InputSystem::UpdateKeyboard() noexcept
{
	for (size_t i = 0; i < KeyboardKeyCount; i++)
	{
		const i32 glfwKeyState = Get<GlfwFacade>().GetKey(GetCurrentWindow(), InputUtility::GetGlfwKey(static_cast<EKeyboardKey>(i)));

		const bool bIsPressed = glfwKeyState == GLFW_PRESS;
		const bool displayKeyboardFocus = DisplayHasKeyboardFocus();
		const bool windowFocus = Get<Window>().HasFocus();

		myInputStates[locDisplayIndex].Keyboard[i]	= GetNewInputState(myInputStates[locDisplayIndex].Keyboard[i], bIsPressed && displayKeyboardFocus);
		myInputStates[locWindowIndex].Keyboard[i]	= GetNewInputState(myInputStates[locWindowIndex].Keyboard[i], bIsPressed && windowFocus);
	}
}

void InputSystem::UpdateMouse() noexcept
{
	for (size_t i = 0; i < MouseButtonCount; i++)
	{
		const i32 glfwKeyState = Get<GlfwFacade>().GetMouseButton(GetCurrentWindow(), InputUtility::GetGlfwMouseButton(static_cast<EMouseButton>(i)));

		const bool bIsPressed = glfwKeyState == GLFW_PRESS;
		const bool displayMouseFocus = DisplayHasMouseFocus();
		const bool windowFocus = Get<Window>().HasFocus();

		myInputStates[locDisplayIndex].Mouse[i]	= GetNewInputState(myInputStates[locDisplayIndex].Mouse[i], bIsPressed && displayMouseFocus);
		myInputStates[locWindowIndex].Mouse[i]	= GetNewInputState(myInputStates[locWindowIndex].Mouse[i], bIsPressed && windowFocus);
	}
}

void InputSystem::UpdateGamepad() noexcept
{
	int gamepadButtonCount;

	const u8* inputArray = Get<GlfwFacade>().GetJoystickButtons(0, &gamepadButtonCount);

	//no gamepad present
	if (gamepadButtonCount == 0)
		return;

	gamepadButtonCount = std::min(gamepadButtonCount, static_cast<int>(GamepadButtonCount));

	for (u32 i = 0; i < static_cast<u32>(gamepadButtonCount); i++)
	{
		const bool bIsPressed = inputArray[i] == GLFW_PRESS;
		const bool displayKeyboardFocus = DisplayHasKeyboardFocus();
		const bool windowFocus = Get<Window>().HasFocus();

		myInputStates[locDisplayIndex].Gamepad[i]	= GetNewInputState(myInputStates[locDisplayIndex].Gamepad[i], bIsPressed && displayKeyboardFocus);
		myInputStates[locWindowIndex].Gamepad[i]	= GetNewInputState(myInputStates[locWindowIndex].Gamepad[i], bIsPressed && windowFocus);
	}
}

SVector2f InputSystem::GetMousePositionInWorld() noexcept
{
	auto& cameraManager = Get<CameraManager>();

	const auto inputRect	= cameraManager.GetInputRegion();
	const auto mousePos		= GetMousePositionOnWindow();

	auto display = cameraManager.GetMainDisplay();

	if (!display)
		return SVector2f::Zero();

	const SVector2f position(inputRect.X, inputRect.Y);
	const SVector2f size(inputRect.Z, inputRect.W);

	SVector2f relativePos = mousePos - position;

	relativePos.X /= size.X;
	relativePos.Y /= size.Y;

	return display->ConvertToWorldPosition(relativePos, SVector2f::Zero());
}

SVector2f InputSystem::GetMousePositionOnWindow() noexcept
{
	f64 x, y;

	Get<GlfwFacade>().GetCursorPos(Get<Window>().myGlfwWindow, &x, &y);

	return { static_cast<f32>(x), static_cast<f32>(y) };
}

EInputItemState InputSystem::GetKeyState(const EKeyboardKey aKey, const EFocus aFocus) const noexcept
{
	return myInputStates[static_cast<size_t>(aFocus)].Keyboard[static_cast<size_t>(aKey)];
}

EInputItemState InputSystem::GetMouseButtonState(const EMouseButton aMouseButton, const EFocus aFocus) const noexcept
{
	return myInputStates[static_cast<size_t>(aFocus)].Mouse[static_cast<size_t>(aMouseButton)];
}

EInputItemState InputSystem::GetGamepadButtonState(const EGamepadButton aGamepadButton, const EFocus aFocus) const noexcept
{
	return myInputStates[static_cast<size_t>(aFocus)].Gamepad[static_cast<size_t>(aGamepadButton)];
}

GLFWwindow* InputSystem::GetCurrentWindow() const
{
	for (auto viewport : ImGui::GetPlatformIO().Viewports)
		if (ImGui::GetPlatformIO().Platform_GetWindowFocus(viewport))
			return reinterpret_cast<GLFWwindow*>(viewport->PlatformHandle);

	return Get<Window>().myGlfwWindow;
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

void InputSystem::Debug(bool& aIsOpen)
{
	ImGui::Begin("InputSystem", &aIsOpen);

	const SVector2f mousePositionOnScreen = GetMousePositionOnWindow();
	const SVector2f mousePositionInWorld = GetMousePositionInWorld();

	ImGui::Text("Mouse position on screen: %f, %f", mousePositionOnScreen.X, mousePositionOnScreen.Y);
	ImGui::Text("Mouse position in world: %f, %f", mousePositionInWorld.X, mousePositionInWorld.Y);

	if (ImGui::CollapsingHeader("Show Keyboard Keys"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (size_t i = 0; i < KeyboardKeyCount; i++)
		{
			const EKeyboardKey key = static_cast<EKeyboardKey>(i);
			const EInputItemState state = myInputStates[locDisplayIndex].Keyboard[i];

			std::string keyInfo = std::string(InputUtility::GetNameOfKey(key));
			keyInfo += ": " + std::string(InputUtility::GetNameOfState(state));
			ImGui::TextUnformatted(keyInfo.c_str());
		}
		ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("Show Mouse Buttons"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (size_t i = 0; i < MouseButtonCount; i++)
		{
			const EMouseButton mouseButton = static_cast<EMouseButton>(i);
			const EInputItemState state = myInputStates[locDisplayIndex].Mouse[i];

			std::string mouseInfo = std::string(InputUtility::GetNameOfMouseButton(mouseButton));
			mouseInfo += ": " + std::string(InputUtility::GetNameOfState(state));
			ImGui::TextUnformatted(mouseInfo.c_str());
		}
		ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("Show Gamepad Buttons"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (size_t i = 0; i < GamepadButtonCount; i++)
		{
			const EGamepadButton gamepadButton = static_cast<EGamepadButton>(i);
			const EInputItemState state = myInputStates[locDisplayIndex].Gamepad[i];

			std::string gamepadButtonInfo = std::string(InputUtility::GetNameOfGamepadButton(gamepadButton));
			gamepadButtonInfo += ": " + std::string(InputUtility::GetNameOfState(state));
			ImGui::TextUnformatted(gamepadButtonInfo.c_str());
		}
		ImGui::PopStyleVar();
	}

	static constexpr auto listButtons = [](const auto& aInputStates)
	{
		ImGui::Indent();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		for (size_t i = 0; i < aInputStates.Keyboard.size(); i++)
			if (aInputStates.Keyboard[i] == EInputItemState::Down || aInputStates.Keyboard[i] == EInputItemState::Pressed)
				ImGui::Text(std::string(InputUtility::GetNameOfKey(static_cast<EKeyboardKey>(i))).c_str());

		for (size_t i = 0; i < aInputStates.Mouse.size(); i++)
			if (aInputStates.Mouse[i] == EInputItemState::Down || aInputStates.Mouse[i] == EInputItemState::Pressed)
				ImGui::Text(std::string(InputUtility::GetNameOfMouseButton(static_cast<EMouseButton>(i))).c_str());

		for (size_t i = 0; i < aInputStates.Gamepad.size(); i++)
			if (aInputStates.Gamepad[i] == EInputItemState::Down || aInputStates.Gamepad[i] == EInputItemState::Pressed)
				ImGui::Text(std::string(InputUtility::GetNameOfGamepadButton(static_cast<EGamepadButton>(i))).c_str());

		ImGui::PopStyleVar();

		ImGui::Unindent();
	};

	ImGui::Text("Currently Pressed Keys");

	ImGui::Separator();

	ImGui::Text("Display");

	listButtons(myInputStates[locDisplayIndex]);

	ImGui::Separator();

	ImGui::Text("Window");

	listButtons(myInputStates[locWindowIndex]);

	ImGui::End();
}

bool InputSystem::DisplayHasMouseFocus() const noexcept
{
	return !Get<CameraManager>().GetInputDisplayMouse().expired();
}

bool InputSystem::DisplayHasKeyboardFocus() const noexcept
{
	return !Get<CameraManager>().GetInputDisplayKeyboard().expired();
}
