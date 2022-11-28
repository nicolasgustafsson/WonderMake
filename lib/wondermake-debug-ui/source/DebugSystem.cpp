#include "wondermake-debug-ui/DebugSystem.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

WM_REGISTER_SYSTEM(DebugSystem);

inline constexpr std::string_view locToolbarSettingName			= "Main Toolbar";
inline constexpr std::string_view locDebugWindowSettingPrefix	= "Debug Windows/";

const std::string locDebugWindowSettingPrefixString = static_cast<std::string>(locDebugWindowSettingPrefix);

void DebugSystem::Initialize()
{
	AddDebugWindow("Debug Settings", [this]()
		{
			Get<DebugSettingsSystem>().Tick();
		});
}

void DebugSystem::ToggleToolbar()
{
	auto& debugSettingsSys = Get<DebugSettingsSystem>();

	const bool isVisible = debugSettingsSys.GetOrCreateDebugValue(locToolbarSettingName, false);

	debugSettingsSys.SetDebugValue<bool>(locToolbarSettingName, !isVisible);
}

void DebugSystem::Tick()
{
	auto& debugSettingsSys = Get<DebugSettingsSystem>();

    if (debugSettingsSys.GetOrCreateDebugValue(locToolbarSettingName, false)
		&& ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Windows"))
        {
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			for (const auto& windowData : myWindows)
			{
				const auto settingName = locDebugWindowSettingPrefixString + windowData.Name;

				const bool wasVisible = debugSettingsSys.GetOrCreateDebugValue(settingName, false);

				bool visible = wasVisible;

				ImGui::MenuItem(windowData.Name.c_str(), NULL, &visible);

				if (visible != wasVisible)
					debugSettingsSys.SetDebugValue(settingName, visible);
			}

			ImGui::PopItemFlag();

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

	TickAllWindows();
}

void DebugSystem::AddDebugWindow(std::string aWindowName, std::function<void()> aTickCallback)
{
	myWindows.emplace_back(SWindowData{ std::move(aWindowName), std::move(aTickCallback) });
}

[[nodiscard]] bool DebugSystem::IsDebugWindowVisible(std::string_view aWindowName) const noexcept
{
	auto& debugSettingsSys = Get<DebugSettingsSystem>();
	const auto settingName = locDebugWindowSettingPrefixString + static_cast<std::string>(aWindowName);

	return debugSettingsSys.GetDebugValue(settingName, false);
}

void DebugSystem::TickAllWindows()
{
	for (const auto& windowData : myWindows)
	{
		if (!Get<DebugSettingsSystem>().GetOrCreateDebugValue(locDebugWindowSettingPrefixString + windowData.Name, false))
			continue;

		Utility::Invoke(windowData.TickFunc);
	}
}
