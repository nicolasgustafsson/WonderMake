#pragma once
#include "Typedefs.h"
#include <string>

enum class EKeyboardKey
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	Space, Enter, Escape, Backspace, Comma, Period, Shift, Alt, Ctrl, Tab, Left, Right, Up, Down, Insert, Home, Delete, End, PageUp, PageDown,
	Break, ScrollLock, PrintScreen, Tilde, CapsLock,
	_Count
};

enum class EMouseButton
{
	Left, Right, Middle, Mouse4, Mouse5, Mouse6, Mouse7, Mouse8,
	_Count
};

enum class EGamepadButton
{
	A, B, X, Y, LB, RB, Back, Start,
	Guide, LeftThumbstick, RightThumbstick, DpadUp, DpadRight, DpadDown, DpadLeft,
	Gamepad16, Gamepad17, Gamepad18, Gamepad19, Gamepad20, Gamepad21, Gamepad22, Gamepad23, Gamepad24,

	_Count
};

constexpr u32 KeyboardKeyCount = static_cast<u32>(EKeyboardKey::_Count);
constexpr u32 MouseButtonCount = static_cast<u32>(EMouseButton::_Count);
constexpr u32 GamepadButtonCount = static_cast<u32>(EGamepadButton::_Count);

enum class EInputItemState
{
	Up,
	Down,
	Pressed,
	Released
};

namespace InputUtility
{
	constexpr std::string_view GetNameOfKey(const EKeyboardKey aKey) noexcept
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

	extern constexpr i32 GetGlfwKey(const EKeyboardKey aKey) noexcept;

	constexpr std::string_view GetNameOfMouseButton(const EMouseButton aMouseButton) noexcept
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

	constexpr i32 GetGlfwMouseButton(const EMouseButton aMouseButton) noexcept
	{
		//this enum is lined up with the glfw buttons
		return static_cast<i32>(aMouseButton);
	}

	constexpr std::string_view GetNameOfGamepadButton(const EGamepadButton aGamepadButton) noexcept
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

	constexpr i32 GetGlfwGamepadButton(const EGamepadButton aGamepadButton) noexcept
	{
		//this enum is lined up with the glfw buttons
		return static_cast<i32>(aGamepadButton);
	}

	constexpr std::string_view GetNameOfState(const EInputItemState aState) noexcept
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
}