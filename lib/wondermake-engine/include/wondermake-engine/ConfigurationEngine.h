#pragma once

#include "wondermake-utility/MemoryUnit.h"

#include <string_view>

class ConfigurationSystem;

namespace ConfigurationEngine
{
	enum class EOverrideFileUserLocation
	{
		User,
		UserData
	};

	inline constexpr std::string_view OverrideFileApplication		= "wondermake_engine.override_file.application";
	inline constexpr std::string_view OverrideFileDevice			= "wondermake_engine.override_file.device";
	inline constexpr std::string_view OverrideFileUser				= "wondermake_engine.override_file.user";
	inline constexpr std::string_view OverrideFileUserLocation		= "wondermake_engine.override_file.user_location";
	
	inline constexpr std::string_view WindowWidth					= "wondermake_engine.window.width";
	inline constexpr std::string_view WindowHeight					= "wondermake_engine.window.height";

	using MemoryType												= MemoryUnit<EMemoryRatio::Bytes, size_t>;

	inline constexpr std::string_view ConfigurationRemoteBufferSize	= "wondermake_engine.configuration_remote.buffer_size";

	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		std::string aOverrideFileDevice,
		std::string aOverrideFileUser,
		EOverrideFileUserLocation aOverrideFileUserLocation,
		bool aIsHeadless);
}
