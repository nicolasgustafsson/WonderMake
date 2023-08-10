#include "wondermake-ui/DebugSettingsSystem.h"

#include "ConfigurationDebugUi.h"
#include "JsonInspector.h"

#include "wondermake-ui/ImguiInclude.h"

#include "wondermake-io/ReadFileJob.h"
#include "wondermake-io/WriteFileJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

#include <magic_enum.hpp>

WM_REGISTER_SYSTEM(DebugSettingsSystem);

DebugSettingsSystem::DebugSettingsSystem()
{
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

bool DebugSettingsSystem::Tick()
{
	std::hash<json> hasher;

	std::size_t hash = hasher(mySettings);

	const bool open = ImGui::JsonInspector::Inspect(mySettings, "Debug Settings");

	const bool changed = hash != hasher(mySettings);

	if (changed)
		SaveSettings();

	return open;
}

std::pair<nlohmann::json&, std::string> DebugSettingsSystem::GetLeaf(const std::string& aSettingName)
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

std::pair<const nlohmann::json*, std::string> DebugSettingsSystem::GetLeaf(const std::string& aSettingName) const noexcept
{
	const nlohmann::json* currentJson = &mySettings;

	size_t last = 0;
	size_t next = 0;

	while ((next = aSettingName.find('/', last)) != std::string::npos)
	{ 
		std::string category = aSettingName.substr(last, next - last);

		auto it = currentJson->find(category);

		if (it == currentJson->end())
			return { nullptr, category };

		currentJson = &*it;
		last = next + 1;
	} 
	
	return { currentJson, aSettingName.substr(last) };
}