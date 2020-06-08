#pragma once
#include "System/System.h"
#include "Utilities/Debugging/Debugged.h"
#include <json/json.hpp>
#include <string>

class DebugSettingsSystem : public System, public Debugged
{
public:
	DebugSettingsSystem();

	void SaveSettings();

	template<typename TSettingType>
	TSettingType GetOrCreateDebugValue(const std::string aSettingName, TSettingType aDefault);

	template<typename TSettingType>
	void SetDebugValue(const std::string aSettingName, TSettingType aSettingValue);

protected:
	std::pair<nlohmann::json&, std::string> GetLeaf(const std::string aSettingName);
	virtual void Debug() override;

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

