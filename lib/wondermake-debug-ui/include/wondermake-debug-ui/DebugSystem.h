#pragma once

#include "wondermake-base/System.h"

#include <functional>
#include <string>
#include <vector>

class DebugSettingsSystem;

class DebugSystem
	: public System<
		Policy::Set<
			PAdd<DebugSettingsSystem, PWrite>>,
		STrait::Set<
			STSingleton>>
{
public:
	void Initialize() override;

	void ToggleToolbar();
	void Tick();

	void AddDebugWindow(std::string aWindowName, std::function<void()> aTickCallback);
	[[nodiscard]] bool IsDebugWindowVisible(std::string_view aWindowName) const noexcept;

private:
	struct SWindowData
	{
		std::string				Name;
		std::function<void()>	TickFunc;
	};

	void TickAllWindows();

	std::vector<SWindowData> myWindows;

};
