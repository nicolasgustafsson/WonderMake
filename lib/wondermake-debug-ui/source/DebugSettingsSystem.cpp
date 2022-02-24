#include "wondermake-debug-ui/DebugSettingsSystem.h"

#include "JsonInspector.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

#include <fstream>

REGISTER_SYSTEM(DebugSettingsSystem);

DebugSettingsSystem::DebugSettingsSystem()
{
	AddDebugWindowTick("Debug Settings", Bind(&DebugSettingsSystem::Tick, this));

	std::ifstream debugSettingsFile("debugSettings.json", std::fstream::app);

	std::string fileContents((std::istreambuf_iterator<char>(debugSettingsFile)),
		(std::istreambuf_iterator<char>()));

	if(json::accept(fileContents))
		mySettings = json::parse(fileContents);

	SetDebugValue<bool>("Debug Windows/Debug Settings", true);
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
	std::ofstream debugSettingsFile("debugSettings.json");

	debugSettingsFile << mySettings.dump();
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
