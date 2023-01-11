#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/Executor.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/CancelableList.h"

class ConfigurationSystem;
class GlfwFacade;
struct GLFWwindow;

class Window
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<GlfwFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
	, public std::enable_shared_from_this<Window>
{
public:
	~Window();

	void Initialize() override;

	void Update();

	inline [[nodiscard]] bool IsValid() const noexcept
	{
		return myGlfwWindow;
	}
	inline [[nodiscard]] SVector2i GetSize() const noexcept
	{
		return mySize;
	}
	inline [[nodiscard]] SVector2i GetPosition() const noexcept
	{
		return myPosition;
	}
	inline [[nodiscard]] bool HasFocus() const noexcept
	{
		return myHasFocus;
	}

	template<CExecutor TExecutor, typename TCallable>
	inline EventSubscriber OnResize(TExecutor&& aExecutor, TCallable&& aCallable)
		requires(std::is_invocable_v<const TCallable, SVector2i>)
	{
		auto [trigger, subscriber] = MakeEventTrigger<SVector2i>(std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallable));

		myResizeList.Emplace(std::move(trigger));

		return std::move(subscriber);
	}
	template<CExecutor TExecutor, typename TCallable>
	inline EventSubscriber OnScroll(TExecutor&& aExecutor, TCallable&& aCallable)
		requires(std::is_invocable_v<const TCallable, SVector2f>)
	{
		auto [trigger, subscriber] = MakeEventTrigger<SVector2f>(std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallable));

		myScrollList.Emplace(std::move(trigger));

		return std::move(subscriber);
	}

	GLFWwindow* myGlfwWindow = nullptr;

private:
	void OnWindowSizeChanged(GLFWwindow* aWindow, i32 aWidth, i32 aHeight);
	void OnScroll(GLFWwindow* aWindow, double aDeltaX, double aDeltaY);
	void UpdatePosition();

	EventSubscriber mySubscriberWindowTitle;
	EventSubscriber mySubscriberSizeWidth;
	EventSubscriber mySubscriberSizeHeight;

	CancelableList<EventTrigger<SVector2i>> myResizeList = { GetExecutor() };
	CancelableList<EventTrigger<SVector2f>> myScrollList = { GetExecutor() };

	SVector2i mySize;
	SVector2i myPosition;
	bool myHasFocus = false;

};
