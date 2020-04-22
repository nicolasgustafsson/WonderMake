#include "pch.h"
#include "DebugSettingsSystem.h"
#include "Imgui/JsonInspector.h"

DebugSettingsSystem::DebugSettingsSystem()
	: Debugged("DebugSettings")
{
	std::ifstream debugSettingsFile("debugSettings.json", std::fstream::app);

	std::string fileContents((std::istreambuf_iterator<char>(debugSettingsFile)),
		(std::istreambuf_iterator<char>()));

	if(json::accept(fileContents))
		mySettings = json::parse(fileContents);

	SetDebugValue<bool>("DebugWindows/DebugSettings", true);
}

void DebugSettingsSystem::SaveSettings()
{
	std::ofstream debugSettingsFile("debugSettings.json");

	debugSettingsFile << mySettings.dump();
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

void DebugSettingsSystem::Debug()
{
	std::hash<json> hasher;

	std::size_t hash = hasher(mySettings);

	JsonInspector::Inspect(mySettings, "Debug Settings");

	const bool changed = hash != hasher(mySettings);

	if (changed)
		SaveSettings();
}
