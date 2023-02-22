#include "wondermake-debug-ui/DebugSystem.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

WM_REGISTER_SYSTEM(DebugSystem);

inline constexpr std::string_view locToolbarSettingName			= "Main Toolbar";
inline constexpr std::string_view locDebugWindowSettingPrefix	= "Debug Windows/";
inline constexpr std::string_view locNoCategory					= "";

const std::string locDebugWindowSettingPrefixString	= static_cast<std::string>(locDebugWindowSettingPrefix);

DebugSystem::DebugSystem()
	: myWindows(
		SWindowCategory
		{
			.Windows = GetExecutor()
		})
{}

void DebugSystem::Initialize()
{
	AddDebugWindow("Settings/Debug", GetExecutor(), [this](bool& aIsOpen)
		{
			aIsOpen = Get<DebugSettingsSystem>().Tick();
		})
		.Detach();
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
			TickCategoryToolbar(myWindows);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

	TickAllWindows();
}

EventSubscriber DebugSystem::AddDebugWindow(std::string aWindowName, AnyExecutor aExecutor, std::function<void(bool&)> aTickCallback)
{
	auto [trigger, subscriber] = MakeEventTrigger<bool*>(std::move(aExecutor), [callback = std::move(aTickCallback)](bool* aIsOpen) { callback(*aIsOpen); });

	auto [categories, name] = GetCategories(aWindowName);
	SWindowCategory* category = &myWindows;

	for (auto& categoryName : categories)
	{
		auto it = category->Categories.find(categoryName);

		if (it == category->Categories.end())
			it = category->Categories.emplace(std::make_pair(categoryName,
				SWindowCategory
				{
					.Windows = GetExecutor()
				})).first;

		category = &it->second;
	}
		
	category->Windows.Emplace(
		SWindowData
		{
			.Name = std::move(name),
			.NameReal = std::move(aWindowName),
			.Trigger = std::move(trigger)
		});

	return std::move(subscriber);
}

[[nodiscard]] bool DebugSystem::IsDebugWindowVisible(std::string_view aWindowName) const noexcept
{
	auto& debugSettingsSys = Get<DebugSettingsSystem>();
	const auto settingName = locDebugWindowSettingPrefixString + static_cast<std::string>(aWindowName);

	return debugSettingsSys.GetDebugValue(settingName, false);
}

void DebugSystem::TickAllWindows()
{
	TickCategory(myWindows);
}

void DebugSystem::TickCategory(SWindowCategory& aCategory)
{
	for (auto& [_, category] : aCategory.Categories)
		TickCategory(category);

	std::string settingsName;

	for (auto& windowData : aCategory.Windows)
	{
		settingsName = locDebugWindowSettingPrefixString + windowData.NameReal;

		if (!Get<DebugSettingsSystem>().GetOrCreateDebugValue(settingsName, false))
			continue;

		bool isOpen = true;

		windowData.Trigger.Trigger(&isOpen);

		if (!isOpen)
			Get<DebugSettingsSystem>().SetDebugValue(settingsName, false);
	}
}

void DebugSystem::TickCategoryToolbar(SWindowCategory& aCategory)
{
	std::string itemName;

	for (auto& [name, category] : aCategory.Categories)
	{
		itemName = name + "##category";

		if (ImGui::BeginMenu(itemName.c_str()))
		{
			TickCategoryToolbar(category);

			ImGui::EndMenu();
		}
	}

	auto& debugSettingsSys = Get<DebugSettingsSystem>();

	ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

	for (const auto& windowData : aCategory.Windows)
	{
		const auto settingName = locDebugWindowSettingPrefixString + windowData.NameReal;

		const bool wasVisible = debugSettingsSys.GetOrCreateDebugValue(settingName, false);

		bool visible = wasVisible;

		itemName = windowData.Name + "##item";

		ImGui::MenuItem(itemName.c_str(), NULL, &visible);

		if (visible != wasVisible)
			debugSettingsSys.SetDebugValue(settingName, visible);
	}

	ImGui::PopItemFlag();
}

std::pair<std::vector<std::string>, std::string> DebugSystem::GetCategories(std::string_view aName) noexcept
{
	std::vector<std::string> categories;

	size_t start = 0;
	for (auto pos = aName.find('/'); pos != std::string_view::npos; pos = aName.find('/', pos + 1))
	{
		categories.emplace_back(static_cast<std::string>(aName.substr(start, pos - start)));

		start = pos + 1;
	}

	return { std::move(categories), static_cast<std::string>(aName.substr(start)) };
}
