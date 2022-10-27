#pragma once

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/MemoryUnit.h"

#include <string_view>

class ConfigurationSystem;

namespace ConfigurationEngine
{
	inline constexpr std::string_view OverrideFileApplication		= "wondermake_engine.override_file.application";
	inline constexpr std::string_view OverrideFileDevice			= "wondermake_engine.override_file.device";
	inline constexpr std::string_view OverrideFileUser				= "wondermake_engine.override_file.user";
	
	inline constexpr std::string_view ResolutionWidth				= "wondermake_engine.graphics.resolution.width";
	inline constexpr std::string_view ResolutionHeight				= "wondermake_engine.graphics.resolution.height";
	inline constexpr std::string_view ResolutionFitToWindow			= "wondermake_engine.graphics.resolution.fit_to_window";
	inline constexpr std::string_view ResolutionFixedAspect			= "wondermake_engine.graphics.resolution.fixed_aspect";
	inline constexpr std::string_view VSync							= "wondermake_engine.graphics.vsync";
	inline constexpr std::string_view MainRenderGraphPath			= "wondermake_engine.graphics.main_render_graph_path";

	inline constexpr std::string_view WindowWidth					= "wondermake_engine.window.width";
	inline constexpr std::string_view WindowHeight					= "wondermake_engine.window.height";

	using MemoryType												= MemoryUnit<EMemoryRatio::Bytes, size_t>;

	inline constexpr std::string_view ConfigurationRemoteBufferSize	= "wondermake_engine.configuration_remote.buffer_size";

	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		FilePath aOverrideFileDevice,
		FilePath aOverrideFileUser,
		bool aIsHeadless);
}
