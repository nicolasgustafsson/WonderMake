#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/EventSubscriber.h"

#include <functional>
#include <string>
#include <map>
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
	DebugSystem();

	void Initialize() override;

	void ToggleToolbar();
	void Tick();

	[[nodiscard]] EventSubscriber AddDebugWindow(std::string aWindowName, AnyExecutor aExecutor, std::function<void(bool&)> aTickCallback);
	[[nodiscard]] bool IsDebugWindowVisible(std::string_view aWindowName) const noexcept;

private:
	struct SWindowData
	{
		std::string			Name;
		std::string			NameReal;
		EventTrigger<bool*>	Trigger;

		inline void OnCancel(CExecutor auto&& aExecutor, auto&& aOnCancel)
		{
			WmOnCancel(Trigger, std::forward<decltype(aExecutor)>(aExecutor), std::forward<decltype(aOnCancel)>(aOnCancel));
		}
	};

	struct SWindowCategory
	{
		std::map<std::string, SWindowCategory>	Categories;
		CancelableList<SWindowData>				Windows;
	};

	void TickAllWindows();
	void TickCategory(SWindowCategory& aCategory);

	void TickCategoryToolbar(SWindowCategory& aCategory);

	static std::pair<std::vector<std::string>, std::string> GetCategories(std::string_view aName) noexcept;

	SWindowCategory	myWindows;

};
