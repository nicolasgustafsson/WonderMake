#pragma once

#include "wondermake-utility/FilePath.h"

#include <string_view>

class ConfigurationSystem;

namespace ConfigurationUi
{
	inline constexpr std::string_view SettingsFile		= "wondermake_ui.settings_file";

	void Configure(
		ConfigurationSystem& aConfigurationSystem);
}
