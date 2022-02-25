#pragma once

#include <string_view>

class ConfigurationSystem;

namespace ConfigurationEngine
{
	enum class EOverrideFileUserLocation
	{
		User,
		UserData
	};

	inline constexpr std::string_view OverrideFileApplication	= "wondermake_engine.override_file.application";
	inline constexpr std::string_view OverrideFileDevice		= "wondermake_engine.override_file.device";
	inline constexpr std::string_view OverrideFileUser			= "wondermake_engine.override_file.user";
	inline constexpr std::string_view OverrideFileUserLocation	= "wondermake_engine.override_file.user_location";

	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		std::string aOverrideFileDevice,
		std::string aOverrideFileUser,
		EOverrideFileUserLocation aOverrideFileUserLocation);
}
