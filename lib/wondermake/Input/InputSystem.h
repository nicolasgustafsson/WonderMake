#pragma once
#include "Input/InputItems.h"
#include "Program/Window.h"
#include "Utilities/Debugging/Debugged.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/CancelableList.h"

#include "magic_enum.hpp"

class Camera;
class CameraManager;
class GlfwFacade;
class Renderer;
class Window;

class InputSystem
	: public System<
		Policy::Set<
			PAdd<GlfwFacade, PWrite>,
			PAdd<CameraManager, PWrite>,
			PAdd<Window, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
	, public Debugged
{
public:
	enum class EFocus
	{
		Display,
		Window
	};

	InputSystem();

	void Update() noexcept;

	void UpdateKeyboard() noexcept;
	void UpdateMouse() noexcept;
	void UpdateGamepad() noexcept;

	[[nodiscard]] SVector2f GetMousePositionInWorld() noexcept;
	[[nodiscard]] SVector2f GetMousePositionOnWindow() noexcept;

	[[nodiscard]] EInputItemState GetKeyState(const EKeyboardKey aKey, const EFocus aFocus = EFocus::Display) const noexcept;
	[[nodiscard]] EInputItemState GetMouseButtonState(const EMouseButton aMouseButton, const EFocus aFocus = EFocus::Display) const noexcept;
	[[nodiscard]] EInputItemState GetGamepadButtonState(const EGamepadButton aGamepadButton, const EFocus aFocus = EFocus::Display) const noexcept;

	template<CExecutor TExecutor, typename TCallable>
	inline EventSubscriber OnScroll(TExecutor&& aExecutor, TCallable&& aCallable)
		requires(std::is_invocable_v<const TCallable, SVector2f>)
	{
		auto [trigger, subscriber] = MakeEventTrigger<SVector2f>(std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallable));

		myScrollList.Emplace(std::move(trigger));

		return std::move(subscriber);
	}

private:
	struct SInputStates
	{
		std::array<EInputItemState, KeyboardKeyCount>	Keyboard	{ EInputItemState::Up };
		std::array<EInputItemState, MouseButtonCount>	Mouse		{ EInputItemState::Up };
		std::array<EInputItemState, GamepadButtonCount>	Gamepad		{ EInputItemState::Up };
	};

	GLFWwindow* GetCurrentWindow() const;

	[[nodiscard]] constexpr EInputItemState GetNewInputState(const EInputItemState aOldState, const bool aIsPressed) const noexcept;

	[[nodiscard]] inline constexpr bool IsStateDown(const EInputItemState aState) const noexcept
	{
		return aState == EInputItemState::Down || aState == EInputItemState::Pressed;
	}

	[[nodiscard]] inline constexpr bool IsStateUp(const EInputItemState aState) const noexcept
	{
		return !IsStateDown(aState);
	}

	virtual void Debug(bool& aIsOpen) override;

	bool DisplayHasMouseFocus() const noexcept;
	bool DisplayHasKeyboardFocus() const noexcept;

	std::array<SInputStates, magic_enum::enum_count<EFocus>()> myInputStates;

	CancelableList<EventTrigger<SVector2f>> myScrollList = { GetExecutor() };
	EventSubscriber myWindowSubscriberScroll;

};
