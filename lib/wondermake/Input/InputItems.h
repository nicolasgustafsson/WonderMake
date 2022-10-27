#pragma once
#include "wondermake-utility/Typedefs.h"

#include <GLFW/glfw3.h>

#include <magic_enum.hpp>

#include <string_view>

enum class EKeyboardKey
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	Space, Enter, Escape, Backspace, Comma, Period, Shift, Alt, Ctrl, Tab, Left, Right, Up, Down, Insert, Home, Delete, End, PageUp, PageDown,
	Break, ScrollLock, PrintScreen, Tilde, CapsLock,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

enum class EMouseButton
{
	Left, Right, Middle, Mouse4, Mouse5, Mouse6, Mouse7, Mouse8
};

enum class EGamepadButton
{
	A, B, X, Y, LB, RB, Back, Start,
	Guide, LeftThumbstick, RightThumbstick, DpadUp, DpadRight, DpadDown, DpadLeft,
	Gamepad16, Gamepad17, Gamepad18, Gamepad19, Gamepad20, Gamepad21, Gamepad22, Gamepad23, Gamepad24
};

inline constexpr size_t KeyboardKeyCount	= magic_enum::enum_count<EKeyboardKey>();
inline constexpr size_t MouseButtonCount	= magic_enum::enum_count<EMouseButton>();
inline constexpr size_t GamepadButtonCount	= magic_enum::enum_count<EMouseButton>();

enum class EInputItemState
{
	Up,
	Down,
	Pressed,
	Released
};

namespace InputUtility
{
	inline constexpr [[nodiscard]] std::string_view GetNameOfKey(const EKeyboardKey aKey) noexcept
	{
		switch (aKey)
		{
		case EKeyboardKey::PageUp:
			return "Page Up";
		case EKeyboardKey::PageDown:
			return "Page Down";
		case EKeyboardKey::ScrollLock:
			return "Scroll Lock";
		case EKeyboardKey::PrintScreen:
			return "Print Screen";
		case EKeyboardKey::CapsLock:
			return "Caps Lock";
		default: break;
		}

		return magic_enum::enum_name(aKey);
	}

	inline constexpr [[nodiscard]] i32 GetGlfwKey(const EKeyboardKey aKey) noexcept
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
		case EKeyboardKey::F1:
			return GLFW_KEY_F1;
		case EKeyboardKey::F2:
			return GLFW_KEY_F2;
		case EKeyboardKey::F3:
			return GLFW_KEY_F3;
		case EKeyboardKey::F4:
			return GLFW_KEY_F4;
		case EKeyboardKey::F5:
			return GLFW_KEY_F5;
		case EKeyboardKey::F6:
			return GLFW_KEY_F6;
		case EKeyboardKey::F7:
			return GLFW_KEY_F7;
		case EKeyboardKey::F8:
			return GLFW_KEY_F8;
		case EKeyboardKey::F9:
			return GLFW_KEY_F9;
		case EKeyboardKey::F10:
			return GLFW_KEY_F10;
		case EKeyboardKey::F11:
			return GLFW_KEY_F11;
		case EKeyboardKey::F12:
			return GLFW_KEY_F12;
		}

		return -1;
	}

	inline constexpr [[nodiscard]] std::string_view GetNameOfMouseButton(const EMouseButton aMouseButton) noexcept
	{
		switch (aMouseButton)
		{
		case EMouseButton::Left:
			return "Left Mouse Button";
		case EMouseButton::Right:
			return "Right Mouse Button";
		case EMouseButton::Middle:
			return "Middle Mouse Button";
		default: break;
		}

		return magic_enum::enum_name(aMouseButton);
	}

	inline constexpr [[nodiscard]] i32 GetGlfwMouseButton(const EMouseButton aMouseButton) noexcept
	{
		//this enum is lined up with the glfw buttons
		return static_cast<i32>(aMouseButton);
	}

	inline constexpr [[nodiscard]] std::string_view GetNameOfGamepadButton(const EGamepadButton aGamepadButton) noexcept
	{
		switch (aGamepadButton)
		{
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
		default:
			break;
		}

		return magic_enum::enum_name(aGamepadButton);
	}

	inline constexpr [[nodiscard]] i32 GetGlfwGamepadButton(const EGamepadButton aGamepadButton) noexcept
	{
		//this enum is lined up with the glfw buttons
		return static_cast<i32>(aGamepadButton);
	}

	inline constexpr [[nodiscard]] std::string_view GetNameOfState(const EInputItemState aState) noexcept
	{
		return magic_enum::enum_name(aState);
	}
}