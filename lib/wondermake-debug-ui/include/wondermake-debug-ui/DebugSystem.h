#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/EventSubscriber.h"

#include <functional>
#include <string>
#include <vector>

class DebugSettingsSystem;

class DebugSystem
	: public System<
		Policy::Set<
			PAdd<DebugSettingsSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	void Initialize() override;

	void ToggleToolbar();
	void Tick();

	[[nodiscard]] EventSubscriber AddDebugWindow(std::string aWindowName, AnyExecutor aExecutor, std::function<void()> aTickCallback);
	[[nodiscard]] bool IsDebugWindowVisible(std::string_view aWindowName) const noexcept;

private:
	struct SWindowData
	{
		std::string			Name;
		EventTrigger<void>	Trigger;

		inline void OnCancel(CExecutor auto&& aExecutor, auto&& aOnCancel)
		{
			WmOnCancel(Trigger, std::forward<decltype(aExecutor)>(aExecutor), std::forward<decltype(aOnCancel)>(aOnCancel));
		}
	};

	void TickAllWindows();

	CancelableList<SWindowData> myWindows = GetExecutor();

};
