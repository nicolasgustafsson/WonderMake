#pragma once
#include "Utilities/Debugging/Debugged.h"
#include "System/System.h"
#include "Program/Window.h"
#include "System/SystemPtr.h"

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

class InputSystem
	: public System
	, public Debugged
{
public:
	void Update() noexcept;

	void UpdateKeyboard() noexcept;
	void UpdateMouse() noexcept;
	void UpdateGamepad() noexcept;

private:
	virtual void Debug() noexcept override;

	constexpr std::string_view GetNameOfKey(const EKeyboardKey aKey) const noexcept;
	constexpr i32 GetGlfwKey(const EKeyboardKey aKey) const noexcept;

	constexpr std::string_view GetNameOfMouseButton(const EMouseButton aMouseButton) const noexcept;
	constexpr i32 GetGlfwMouseButton(const EMouseButton aMouseButton) const noexcept;

	constexpr std::string_view GetNameOfGamepadButton(const EGamepadButton aGamepadButton) const noexcept;
	constexpr i32 GetGlfwGamepadButton(const EGamepadButton aGamepadButton) const noexcept;

	constexpr std::string_view GetNameOfState(const EInputItemState aState) const noexcept;

	std::array<EInputItemState, KeyboardKeyCount> myKeyboardKeyStates;
	std::array<EInputItemState, MouseButtonCount> myMouseButtonStates;
	std::array<EInputItemState, GamepadButtonCount> myGamepadButtonStates;

	SystemPtr<Window> myWindowSystemPtr;
};

