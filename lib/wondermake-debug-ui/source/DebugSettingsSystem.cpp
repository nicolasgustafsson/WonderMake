#include "wondermake-debug-ui/DebugSettingsSystem.h"

#include "ConfigurationDebugUi.h"
#include "JsonInspector.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-io/ReadFileJob.h"
#include "wondermake-io/WriteFileJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

#include <magic_enum.hpp>

REGISTER_SYSTEM(DebugSettingsSystem);

inline constexpr auto locDebugSettingsWindow = "Debug Windows/Debug Settings";

DebugSettingsSystem::DebugSettingsSystem()
{
	AddDebugWindowTick("Debug Settings", Bind(&DebugSettingsSystem::Tick, this));

	auto filePath = Get<ConfigurationSystem>().Get<FilePath>(ConfigurationDebugUi::SettingsFile, FilePath(FilePath::EFolder::Bin, "debugSettings.json"));

	Get<JobSystem>()
		.StartJob<ReadFileJob>(filePath)
		.ThenRun(GetExecutor(), FutureRunResult([this, filePath](auto&& aResult)
			{
				if (!aResult)
				{
					WmLogError(TagWonderMake << "Failed to open debug settings file. Error: " << magic_enum::enum_name(aResult.Err()) << ", path: " << filePath << '.');

					return;
				}
				
				auto json = aResult.Unwrap();

				if (json::accept(json))
					mySettings = json::parse(json);
			}))
		.Detach();
}

void DebugSettingsSystem::ToggleSettingsWindow()
{
	const bool isVisible = GetOrCreateDebugValue<bool>(locDebugSettingsWindow, false);

	SetDebugValue<bool>(locDebugSettingsWindow, !isVisible);

	SaveSettings();
}

void DebugSettingsSystem::TickAllWindows()
{
	for (auto& [name, callback] : myTickCallbacks)
	{
		if (!GetOrCreateDebugValue<bool>(name, false))
			continue;

		Utility::Invoke(callback);
	}
}

void DebugSettingsSystem::AddDebugWindowTick(const std::string& aWindowName, std::function<void()> aTickCallback)
{
	myTickCallbacks.insert(std::make_pair("Debug Windows/" + aWindowName, std::move(aTickCallback)));
}

void DebugSettingsSystem::SaveSettings()
{
	auto filePath = Get<ConfigurationSystem>().Get<FilePath>(ConfigurationDebugUi::SettingsFile, FilePath(FilePath::EFolder::Bin, "debugSettings.json"));

	Get<JobSystem>()
		.StartJob<WriteFileJob>(filePath, mySettings.dump())
		.ThenRun(GetExecutor(), FutureRunResult([this, filePath](auto&& aResult)
			{
				if (!aResult)
					WmLogError(TagWonderMake << "Failed to save debug settings file. Error: " << magic_enum::enum_name(aResult.Err()) << ", path: " << filePath << '.');
			}))
		.Detach();
}

void DebugSettingsSystem::Tick()
{
	std::hash<json> hasher;

	std::size_t hash = hasher(mySettings);

	ImGui::JsonInspector::Inspect(mySettings, "Debug Settings");

	const bool changed = hash != hasher(mySettings);

	if (changed)
		SaveSettings();
}

std::pair<nlohmann::json&, std::string> DebugSettingsSystem::GetLeaf(const std::string aSettingName)
{
	nlohmann::json* currentJson = &mySettings;

	size_t last = 0; 
	size_t next = 0; 
	while ((next = aSettingName.find('/', last)) != std::string::npos)
	{ 
		std::string category = aSettingName.substr(last, next - last);

		currentJson = &(*currentJson)[category];
		last = next + 1; 
	} 
	
	return { *currentJson, aSettingName.substr(last) };
}
