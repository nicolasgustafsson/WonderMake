#pragma once

#include "wondermake-utility/FilePath.h"

#include <string_view>

class ConfigurationSystem;

namespace ConfigurationUi
{
	inline constexpr std::string_view SettingsFile		= "wondermake_ui.settings_file";
	inline constexpr std::string_view ShaderSearchPath	= "wondermake_ui.shader_search_path";

	void Configure(
		ConfigurationSystem& aConfigurationSystem);
}
