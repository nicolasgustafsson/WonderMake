#pragma once
#include "Utilities/Debugging/Debugged.h"
#include "System/System.h"
#include "Program/Window.h"
#include "InputItems.h"

class Camera;
class CameraManager;
class GlfwFacade;
class Renderer;
class Window;

class InputSystem
	: public System<GlfwFacade, Renderer, CameraManager, Window>
	, public Debugged
{
public:
	InputSystem()
		: Debugged("Input") {}
	void Update() noexcept;

	void UpdateKeyboard() noexcept;
	void UpdateMouse() noexcept;
	void UpdateGamepad() noexcept;

	[[nodiscard]] SVector2f GetMousePositionInWorld() noexcept;
	[[nodiscard]] SVector2f GetMousePositionOnWindow() noexcept;

	bool IsKeyPressed(const EKeyboardKey aKey) const noexcept;
	bool IsKeyDown(const EKeyboardKey aKey) const noexcept;
	bool IsMouseButtonPressed(const EMouseButton aKey) const noexcept;
	bool IsMouseButtonDown(const EMouseButton aKey) const noexcept;

	void SetMousePosition(const SVector2f aWindowPosition);

	GLFWwindow* GetCurrentWindow() const;
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

	virtual void Debug() override;

	bool ShouldCaptureMouseInput() const noexcept;

	std::array<EInputItemState, KeyboardKeyCount> myKeyboardKeyStates		{ EInputItemState::Up };
	std::array<EInputItemState, MouseButtonCount> myMouseButtonStates		{ EInputItemState::Up };
	std::array<EInputItemState, GamepadButtonCount> myGamepadButtonStates	{ EInputItemState::Up };
};
