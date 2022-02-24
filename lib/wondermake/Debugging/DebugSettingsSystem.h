#pragma once

#include "wondermake-base/System.h"

#include "json/json.hpp"

#include <functional>
#include <string>
#include <unordered_map>

class DebugSettingsSystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STSingleton>>
{
public:
	DebugSettingsSystem();

	void TickAllWindows();

	void AddDebugWindowTick(const std::string& aWindowName, std::function<void()> aTickCallback);

	void SaveSettings();

	template<typename TSettingType>
	TSettingType GetOrCreateDebugValue(const std::string aSettingName, TSettingType aDefault);

	template<typename TSettingType>
	void SetDebugValue(const std::string aSettingName, TSettingType aSettingValue);

protected:
	void Tick();

	std::pair<nlohmann::json&, std::string> GetLeaf(const std::string aSettingName);

	std::unordered_map<std::string, std::function<void()>> myTickCallbacks;
	nlohmann::json mySettings;
};

template<typename TSettingType>
TSettingType DebugSettingsSystem::GetOrCreateDebugValue(const std::string aSettingName, TSettingType aDefault)
{
	std::pair<nlohmann::json&, std::string> leaf = GetLeaf(aSettingName);

	auto it = leaf.first.find(leaf.second);
	if (it != leaf.first.end())
		return (*it).get<TSettingType>();
	
	leaf.first[leaf.second] = aDefault;

	//[Nicos]: Make sure that the settings will save even if they arent changed.
	SaveSettings();

	return aDefault;
}

template<typename TSettingType>
void DebugSettingsSystem::SetDebugValue(const std::string aSettingName, TSettingType aSettingValue)
{
	std::pair<nlohmann::json&, std::string> leaf = GetLeaf(aSettingName);
	leaf.first[leaf.second] = aSettingValue;
}
