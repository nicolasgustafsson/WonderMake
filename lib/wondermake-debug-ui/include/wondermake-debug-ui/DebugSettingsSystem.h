#pragma once

#include "wondermake-base/JobSystem.h"
#include "wondermake-base/System.h"

#include "json/json.hpp"

#include <string>
#include <string_view>

class ConfigurationSystem;
class JobSystem;

class DebugSettingsSystem
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PRead>,
			PAdd<JobSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	DebugSettingsSystem();

	void SaveSettings();
	
	template<typename TSettingType>
	inline [[nodiscard]] TSettingType GetDebugValue(const char* aSettingName, TSettingType aDefault) const noexcept
	{
		return GetDebugValue(std::string_view(aSettingName), std::move(aDefault));
	}
	template<typename TSettingType>
	inline [[nodiscard]] TSettingType GetDebugValue(std::string_view aSettingName, TSettingType aDefault) const noexcept
	{
		thread_local std::string buffer;

		buffer.reserve(aSettingName.size() + 1);

		buffer.assign(aSettingName.data(), aSettingName.size());

		return GetDebugValue(buffer, std::move(aDefault));
	}
	template<typename TSettingType>
	inline [[nodiscard]] TSettingType GetDebugValue(const std::string& aSettingName, TSettingType aDefault) const noexcept
	{
		auto leaf = GetLeaf(aSettingName);

		auto it = leaf.first.find(leaf.second);
		if (it != leaf.first.end())
			return (*it).get<TSettingType>();

		return std::move(aDefault);
	}
	
	template<typename TSettingType>
	inline [[nodiscard]] TSettingType GetOrCreateDebugValue(const char* aSettingName, TSettingType aDefault)
	{
		return GetOrCreateDebugValue(std::string_view(aSettingName), std::move(aDefault));
	}
	template<typename TSettingType>
	inline [[nodiscard]] TSettingType GetOrCreateDebugValue(std::string_view aSettingName, TSettingType aDefault)
	{
		thread_local std::string buffer;

		buffer.reserve(aSettingName.size() + 1);

		buffer.assign(aSettingName.data(), aSettingName.size());

		return GetOrCreateDebugValue(buffer, std::move(aDefault));
	}
	template<typename TSettingType>
	inline [[nodiscard]] TSettingType GetOrCreateDebugValue(const std::string& aSettingName, TSettingType aDefault)
	{
		auto leaf = GetLeaf(aSettingName);

		auto it = leaf.first.find(leaf.second);
		if (it != leaf.first.end())
			return (*it).get<TSettingType>();

		leaf.first[leaf.second] = aDefault;

		//[Nicos]: Make sure that the settings will save even if they arent changed.
		SaveSettings();

		return std::move(aDefault);
	}

	template<typename TSettingType>
	inline void SetDebugValue(const char* aSettingName, TSettingType&& aSettingValue)
	{
		SetDebugValue(std::string_view(aSettingName), std::forward<TSettingType>(aSettingValue));
	}
	template<typename TSettingType>
	inline void SetDebugValue(std::string_view  aSettingName, TSettingType&& aSettingValue)
	{
		thread_local std::string buffer;

		buffer.reserve(aSettingName.size() + 1);

		buffer.assign(aSettingName.data(), aSettingName.size());

		return SetDebugValue(buffer, std::forward<TSettingType>(aSettingValue));
	}
	template<typename TSettingType>
	inline void SetDebugValue(const std::string& aSettingName, TSettingType&& aSettingValue)
	{
		std::pair<nlohmann::json&, std::string> leaf = GetLeaf(aSettingName);

		leaf.first[leaf.second] = std::forward<TSettingType>(aSettingValue);

		SaveSettings();
	}

	void Tick();

protected:
	std::pair<nlohmann::json&, std::string> GetLeaf(const std::string& aSettingName);
	std::pair<const nlohmann::json&, std::string> GetLeaf(const std::string& aSettingName) const noexcept;

	nlohmann::json mySettings;
};
