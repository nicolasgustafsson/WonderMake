#pragma once
#include "Utilities/Debugging/Debugged.h"
#include "System/System.h"
#include "Program/Window.h"
#include "System/SystemPtr.h"
#include "InputItems.h"

class InputSystem
	: public System
	, public Debugged
{
public:
	void Update() noexcept;

	void UpdateKeyboard() noexcept;
	void UpdateMouse() noexcept;
	void UpdateGamepad() noexcept;

	bool IsKeyDown(const EKeyboardKey aKey) const noexcept;
	bool IsMouseButtonDown(const EMouseButton aKey) const noexcept;

private:
	[[nodiscard]] constexpr EInputItemState GetNewInputState(const EInputItemState aOldState, const bool aIsPressed) const noexcept;

	[[nodiscard]] inline constexpr bool IsStateDown(const EInputItemState aState) const noexcept
	{
		return aState == EInputItemState::Down || aState == EInputItemState::Pressed;
	}

	[[nodiscard]] inline constexpr bool IsStateUp(const EInputItemState aState) const noexcept
	{
		return !IsStateDown(aState);
	}

	virtual void Debug() noexcept override;

	std::array<EInputItemState, KeyboardKeyCount> myKeyboardKeyStates;
	std::array<EInputItemState, MouseButtonCount> myMouseButtonStates;
	std::array<EInputItemState, GamepadButtonCount> myGamepadButtonStates;

	SystemPtr<Window> myWindowSystemPtr;
};

