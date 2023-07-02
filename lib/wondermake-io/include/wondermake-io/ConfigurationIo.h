#pragma once

#include <filesystem>
#include <string_view>

class ConfigurationSystem;

namespace ConfigurationIo
{
	inline constexpr std::string_view ConfigDirectoryExtraBins	= "wondermake_io.directory.extra_bin";
	inline constexpr std::string_view ConfigDirectoryData		= "wondermake_io.directory.data";
	inline constexpr std::string_view ConfigDirectoryUser		= "wondermake_io.directory.user";
	inline constexpr std::string_view ConfigDirectoryUserData	= "wondermake_io.directory.userdata";

	void Configure(
		ConfigurationSystem&	aConfigurationSystem,
		std::filesystem::path	aPathData,
		std::filesystem::path	aPathUser,
		std::filesystem::path	aPathUserData);
}
