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
		const i32 glfwKeyState = glfwGetKey(myWindowSystemPtr->myGlfwWindow, GetGlfwKey(static_cast<EKeyboardKey>(i)));

		const EInputItemState startingState = myKeyboardKeyStates[i];

		if (glfwKeyState == GLFW_PRESS)
		{
			if (startingState == EInputItemState::Up || startingState == EInputItemState::Released)
				myKeyboardKeyStates[i] = EInputItemState::Pressed;
			else
				myKeyboardKeyStates[i] = EInputItemState::Down;
		}
		else
		{
			if (startingState == EInputItemState::Down || startingState == EInputItemState::Pressed)
				myKeyboardKeyStates[i] = EInputItemState::Released;
			else
				myKeyboardKeyStates[i] = EInputItemState::Up;
		}
	}
}

void InputSystem::UpdateMouse() noexcept
{
	for (u32 i = 0; i < MouseButtonCount; i++)
	{
		const i32 glfwKeyState = glfwGetMouseButton(myWindowSystemPtr->myGlfwWindow, GetGlfwMouseButton(static_cast<EMouseButton>(i)));

		const EInputItemState startingState = myMouseButtonStates[i];

		if (glfwKeyState == GLFW_PRESS)
		{
			if (startingState == EInputItemState::Up || startingState == EInputItemState::Released)
				myMouseButtonStates[i] = EInputItemState::Pressed;
			else
				myMouseButtonStates[i] = EInputItemState::Down;
		}
		else
		{
			if (startingState == EInputItemState::Down || startingState == EInputItemState::Pressed)
				myMouseButtonStates[i] = EInputItemState::Released;
			else
				myMouseButtonStates[i] = EInputItemState::Up;
		}
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
		const i32 glfwKeyState = inputArray[i];

		const EInputItemState startingState = myGamepadButtonStates[i];

		if (glfwKeyState == GLFW_PRESS)
		{
			if (startingState == EInputItemState::Up || startingState == EInputItemState::Released)
				myGamepadButtonStates[i] = EInputItemState::Pressed;
			else
				myGamepadButtonStates[i] = EInputItemState::Down;
		}
		else
		{
			if (startingState == EInputItemState::Down || startingState == EInputItemState::Pressed)
				myGamepadButtonStates[i] = EInputItemState::Released;
			else
				myGamepadButtonStates[i] = EInputItemState::Up;
		}
	}
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

			std::string keyInfo = std::string(GetNameOfKey(key));
			keyInfo += ": " + std::string(GetNameOfState(state));
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

			std::string mouseInfo = std::string(GetNameOfMouseButton(mouseButton));
			mouseInfo += ": " + std::string(GetNameOfState(state));
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

			std::string gamepadButtonInfo = std::string(GetNameOfGamepadButton(gamepadButton));
			gamepadButtonInfo += ": " + std::string(GetNameOfState(state));
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
			ImGui::Text(std::string(GetNameOfKey(static_cast<EKeyboardKey>(i))).c_str());
	}

	for (u32 i = 0; i < MouseButtonCount; i++)
	{
		if (myMouseButtonStates[i] == EInputItemState::Down || myMouseButtonStates[i] == EInputItemState::Pressed)
			ImGui::Text(std::string(GetNameOfMouseButton(static_cast<EMouseButton>(i))).c_str());
	}

	for (u32 i = 0; i < GamepadButtonCount; i++)
	{
		if (myGamepadButtonStates[i] == EInputItemState::Down || myGamepadButtonStates[i] == EInputItemState::Pressed)
			ImGui::Text(std::string(GetNameOfGamepadButton(static_cast<EGamepadButton>(i))).c_str());
	}

	ImGui::PopStyleVar();

	ImGui::End();
}

constexpr std::string_view InputSystem::GetNameOfKey(const EKeyboardKey aKey) const noexcept
{
	switch (aKey)
	{
	case EKeyboardKey::A:
		return "A";
	case EKeyboardKey::B:
		return "B";
	case EKeyboardKey::C:
		return "C";
	case EKeyboardKey::D:
		return "D";
	case EKeyboardKey::E:
		return "E";
	case EKeyboardKey::F:
		return "F";
	case EKeyboardKey::G:
		return "G";
	case EKeyboardKey::H:
		return "H";
	case EKeyboardKey::I:
		return "I";
	case EKeyboardKey::J:
		return "J";
	case EKeyboardKey::K:
		return "K";
	case EKeyboardKey::L:
		return "L";
	case EKeyboardKey::M:
		return "M";
	case EKeyboardKey::N:
		return "N";
	case EKeyboardKey::O:
		return "O";
	case EKeyboardKey::P:
		return "P";
	case EKeyboardKey::Q:
		return "Q";
	case EKeyboardKey::R:
		return "R";
	case EKeyboardKey::S:
		return "S";
	case EKeyboardKey::T:
		return "T";
	case EKeyboardKey::U:
		return "U";
	case EKeyboardKey::V:
		return "V";
	case EKeyboardKey::W:
		return "W";
	case EKeyboardKey::X:
		return "X";
	case EKeyboardKey::Y:
		return "Y";
	case EKeyboardKey::Z:
		return "Z";
	case EKeyboardKey::Zero:
		return "Zero";
	case EKeyboardKey::One:
		return "One";
	case EKeyboardKey::Two:
		return "Two";
	case EKeyboardKey::Three:
		return "Three";
	case EKeyboardKey::Four:
		return "Four";
	case EKeyboardKey::Five:
		return "Five";
	case EKeyboardKey::Six:
		return "Six";
	case EKeyboardKey::Seven:
		return "Seven";
	case EKeyboardKey::Eight:
		return "Eight";
	case EKeyboardKey::Nine:
		return "Nine";
	case EKeyboardKey::Space:
		return "Space";
	case EKeyboardKey::Enter:
		return "Enter";
	case EKeyboardKey::Escape:
		return "Escape";
	case EKeyboardKey::Backspace:
		return "Backspace";
	case EKeyboardKey::Comma:
		return "Comma";
	case EKeyboardKey::Period:
		return "Period";
	case EKeyboardKey::Shift:
		return "Shift";
	case EKeyboardKey::Alt:
		return "Alt";
	case EKeyboardKey::Ctrl:
		return "Ctrl";
	case EKeyboardKey::Tab:
		return "Tab";
	case EKeyboardKey::Left:
		return "Left";
	case EKeyboardKey::Right:
		return "Right";
	case EKeyboardKey::Up:
		return "Up";
	case EKeyboardKey::Down:
		return "Down";
	case EKeyboardKey::Insert:
		return "Insert";
	case EKeyboardKey::Home:
		return "Home";
	case EKeyboardKey::Delete:
		return "Delete";
	case EKeyboardKey::End:
		return "End";
	case EKeyboardKey::PageUp:
		return "Page Up";
	case EKeyboardKey::PageDown:
		return "Page Down";
	case EKeyboardKey::Break:
		return "Break";
	case EKeyboardKey::ScrollLock:
		return "Scroll Lock";
	case EKeyboardKey::PrintScreen:
		return "Print Screen";
	case EKeyboardKey::Tilde:
		return "Tilde";
	case EKeyboardKey::CapsLock:
		return "Caps Lock";
	default:
		return "Unknown Key";
	}
}
 
constexpr i32 InputSystem::GetGlfwKey(const EKeyboardKey aKey) const noexcept
{
	switch (aKey)
	{
	case EKeyboardKey::A:
		return GLFW_KEY_A;
	case EKeyboardKey::B:
		return GLFW_KEY_B;
	case EKeyboardKey::C:
		return GLFW_KEY_C;
	case EKeyboardKey::D:
		return GLFW_KEY_D;
	case EKeyboardKey::E:
		return GLFW_KEY_E;
	case EKeyboardKey::F:
		return GLFW_KEY_F;
	case EKeyboardKey::G:
		return GLFW_KEY_G;
	case EKeyboardKey::H:
		return GLFW_KEY_H;
	case EKeyboardKey::I:
		return GLFW_KEY_I;
	case EKeyboardKey::J:
		return GLFW_KEY_J;
	case EKeyboardKey::K:
		return GLFW_KEY_K;
	case EKeyboardKey::L:
		return GLFW_KEY_L;
	case EKeyboardKey::M:
		return GLFW_KEY_M;
	case EKeyboardKey::N:
		return GLFW_KEY_N;
	case EKeyboardKey::O:
		return GLFW_KEY_O;
	case EKeyboardKey::P:
		return GLFW_KEY_P;
	case EKeyboardKey::Q:
		return GLFW_KEY_Q;
	case EKeyboardKey::R:
		return GLFW_KEY_R;
	case EKeyboardKey::S:
		return GLFW_KEY_S;
	case EKeyboardKey::T:
		return GLFW_KEY_T;
	case EKeyboardKey::U:
		return GLFW_KEY_U;
	case EKeyboardKey::V:
		return GLFW_KEY_V;
	case EKeyboardKey::W:
		return GLFW_KEY_W;
	case EKeyboardKey::X:
		return GLFW_KEY_X;
	case EKeyboardKey::Y:
		return GLFW_KEY_Y;
	case EKeyboardKey::Z:
		return GLFW_KEY_Z;
	case EKeyboardKey::Zero:
		return GLFW_KEY_0;
	case EKeyboardKey::One:
		return GLFW_KEY_1;
	case EKeyboardKey::Two:
		return GLFW_KEY_2;
	case EKeyboardKey::Three:
		return GLFW_KEY_3;
	case EKeyboardKey::Four:
		return GLFW_KEY_4;
	case EKeyboardKey::Five:
		return GLFW_KEY_5;
	case EKeyboardKey::Six:
		return GLFW_KEY_6;
	case EKeyboardKey::Seven:
		return GLFW_KEY_7;
	case EKeyboardKey::Eight:
		return GLFW_KEY_8;
	case EKeyboardKey::Nine:
		return GLFW_KEY_9;
	case EKeyboardKey::Space:
		return GLFW_KEY_SPACE;
	case EKeyboardKey::Enter:
		return GLFW_KEY_ENTER;
	case EKeyboardKey::Escape:
		return GLFW_KEY_ESCAPE;
	case EKeyboardKey::Backspace:
		return GLFW_KEY_BACKSPACE;
	case EKeyboardKey::Comma:
		return GLFW_KEY_COMMA;
	case EKeyboardKey::Period:
		return GLFW_KEY_PERIOD;
	case EKeyboardKey::Shift:
		return GLFW_KEY_LEFT_SHIFT;
	case EKeyboardKey::Alt:
		return GLFW_KEY_LEFT_ALT;
	case EKeyboardKey::Ctrl:
		return GLFW_KEY_LEFT_CONTROL;
	case EKeyboardKey::Tab:
		return GLFW_KEY_TAB;
	case EKeyboardKey::Left:
		return GLFW_KEY_LEFT;
	case EKeyboardKey::Right:
		return GLFW_KEY_RIGHT;
	case EKeyboardKey::Up:
		return GLFW_KEY_UP;
	case EKeyboardKey::Down:
		return GLFW_KEY_DOWN;
	case EKeyboardKey::Insert:
		return GLFW_KEY_INSERT;
	case EKeyboardKey::Home:
		return GLFW_KEY_HOME;
	case EKeyboardKey::Delete:
		return GLFW_KEY_DELETE;
	case EKeyboardKey::End:
		return GLFW_KEY_END;
	case EKeyboardKey::PageUp:
		return GLFW_KEY_PAGE_UP;
	case EKeyboardKey::PageDown:
		return GLFW_KEY_PAGE_DOWN;
	case EKeyboardKey::Break:
		return GLFW_KEY_PAUSE;
	case EKeyboardKey::ScrollLock:
		return GLFW_KEY_SCROLL_LOCK;
	case EKeyboardKey::PrintScreen:
		return GLFW_KEY_PRINT_SCREEN;
	case EKeyboardKey::Tilde:
		return GLFW_KEY_GRAVE_ACCENT;
	case EKeyboardKey::CapsLock:
		return GLFW_KEY_CAPS_LOCK;
	default:
		return -1;
	}
}

constexpr std::string_view InputSystem::GetNameOfMouseButton(const EMouseButton aMouseButton) const noexcept
{
	switch (aMouseButton)
	{
	case EMouseButton::Left:
		return "Left Mouse Button";
	case EMouseButton::Right:
		return "Right Mouse Button";
	case EMouseButton::Middle:
		return "Middle Mouse Button";
	case EMouseButton::Mouse4:
		return "Mouse4";
	case EMouseButton::Mouse5:
		return "Mouse5";
	case EMouseButton::Mouse6:
		return "Mouse6";
	case EMouseButton::Mouse7:
		return "Mouse7";
	case EMouseButton::Mouse8:
		return "Mouse8";
	default:
		return "Unknown mouse button";
	}
}

constexpr i32 InputSystem::GetGlfwMouseButton(const EMouseButton aMouseButton) const noexcept
{
	//this enum is lined up with the glfw buttons
	return static_cast<i32>(aMouseButton);
}

constexpr std::string_view InputSystem::GetNameOfGamepadButton(const EGamepadButton aGamepadButton) const noexcept
{
	switch (aGamepadButton)
	{
	case EGamepadButton::A:
		return "A";
	case EGamepadButton::B:
		return "B";
	case EGamepadButton::X:
		return "X";
	case EGamepadButton::Y:
		return "Y";
	case EGamepadButton::LB:
		return "LB";
	case EGamepadButton::RB:
		return "RB";
	case EGamepadButton::Back:
		return "Back";
	case EGamepadButton::Start:
		return "Start";
	case EGamepadButton::Guide:
		return "Guide";
	case EGamepadButton::LeftThumbstick:
		return "Left Thumbstick";
	case EGamepadButton::RightThumbstick:
		return "Right Thumbstick";
	case EGamepadButton::DpadUp:
		return "Dpad Up";
	case EGamepadButton::DpadRight:
		return "Dpad Right";
	case EGamepadButton::DpadDown:
		return "Dpad Down";
	case EGamepadButton::DpadLeft:
		return "Dpad Left";
	case EGamepadButton::Gamepad16:
		return "Gamepad16";
	case EGamepadButton::Gamepad17:
		return "Gamepad17";
	case EGamepadButton::Gamepad18:
		return "Gamepad18";
	case EGamepadButton::Gamepad19:
		return "Gamepad19";
	case EGamepadButton::Gamepad20:
		return "Gamepad20";
	case EGamepadButton::Gamepad21:
		return "Gamepad21";
	case EGamepadButton::Gamepad22:
		return "Gamepad22";
	case EGamepadButton::Gamepad23:
		return "Gamepad23";
	case EGamepadButton::Gamepad24:
		return "Gamepad24";
	case EGamepadButton::_Count:
	default:
		return "Unknown Button";
	}
}

constexpr i32 InputSystem::GetGlfwGamepadButton(const EGamepadButton aGamepadButton) const noexcept
{
	//this enum is lined up with the glfw buttons
	return static_cast<i32>(aGamepadButton);
}

constexpr std::string_view InputSystem::GetNameOfState(const EInputItemState aState) const noexcept
{
	switch (aState)
	{
	case EInputItemState::Up:
		return "Up";
	case EInputItemState::Down:
		return "Down";
	case EInputItemState::Pressed:
		return "Pressed";
	case EInputItemState::Released:
		return "Released";
	default:
		return "Unknown";
	}
}
