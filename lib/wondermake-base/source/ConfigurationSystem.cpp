#include "wondermake-base/ConfigurationSystem.h"

#include "wondermake-base/ProcessSystem.h"

void ConfigurationSystem::Initialize()
{
	myProcessId = System::Get<ProcessSystem>().GetCurrentProcessId();
}

void ConfigurationSystem::ResetOverride(const std::string& aId)
{
	ResetOverride(myProcessId, aId);
}

void ConfigurationSystem::ResetOverride(ProcessId aProcessId, const std::string& aId)
{
	myConfigurationSets[aProcessId].ResetOverride(aId);
}

[[nodiscard]] std::unordered_map<std::string, Configuration::ConfigElement> ConfigurationSystem::GetConfigs() const
{
	return GetConfigs(myProcessId);
}

[[nodiscard]] std::unordered_map<std::string, Configuration::ConfigElement> ConfigurationSystem::GetConfigs(ProcessId aProcessId) const
{
	auto itProcess = myConfigurationSets.find(aProcessId);

	if (itProcess == myConfigurationSets.cend())
		return {};

	return itProcess->second.GetConfigs();
}

[[nodiscard]] std::unordered_map<ProcessId, std::unordered_map<std::string, Configuration::ConfigElement>> ConfigurationSystem::GetAllConfigs() const
{
	std::unordered_map<ProcessId, std::unordered_map<std::string, Configuration::ConfigElement>> retVal;

	for (auto& [processId, configuration] : myConfigurationSets)
		retVal.emplace(std::make_pair(processId, configuration.GetConfigs()));

	return retVal;
}
