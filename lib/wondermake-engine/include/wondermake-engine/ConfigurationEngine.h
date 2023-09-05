#pragma once

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/MemoryUnit.h"

#include <optional>
#include <string_view>

class ConfigurationSystem;

namespace ConfigurationEngine
{
	inline constexpr std::string_view OverrideFileApplication		= "wondermake_engine.override_file.application";
	inline constexpr std::string_view OverrideFileDevice			= "wondermake_engine.override_file.device";
	inline constexpr std::string_view OverrideFileUser				= "wondermake_engine.override_file.user";
	
	inline constexpr std::string_view ResourceLoaderFile			= "wondermake_engine.resource_loader.file";

	inline constexpr std::string_view WindowTitle					= "wondermake_engine.window.title";
	inline constexpr std::string_view WindowWidth					= "wondermake_engine.window.width";
	inline constexpr std::string_view WindowHeight					= "wondermake_engine.window.height";
	inline constexpr std::string_view WindowMaximized				= "wondermake_engine.window.maximized";
	
	inline constexpr std::string_view ResolutionWidth				= "wondermake_engine.graphics.resolution.width";
	inline constexpr std::string_view ResolutionHeight				= "wondermake_engine.graphics.resolution.height";
	inline constexpr std::string_view ResolutionFitToWindow			= "wondermake_engine.graphics.resolution.fit_to_window";
	inline constexpr std::string_view ResolutionFixedAspect			= "wondermake_engine.graphics.resolution.fixed_aspect";
	inline constexpr std::string_view VSync							= "wondermake_engine.graphics.vsync";
	
	inline constexpr std::string_view DefaultRenderGraphPath		= "wondermake_engine.graphics.default_render_graph_path";
	inline constexpr std::string_view MissingTexturePath			= "wondermake_engine.graphics.missing_texture_path";

	inline constexpr std::string_view ImguiIniLocation				= "wondermake_engine.imgui.ini_location";
	inline constexpr std::string_view ImguiFontDirectory			= "wondermake_engine.imgui.font_directory";
	
	inline constexpr std::string_view AudioNodeGraphPath			= "wondermake_engine.audio.audio_node_graph_path";

	using MemoryType												= MemoryUnit<EMemoryRatio::Bytes, size_t>;

	inline constexpr std::string_view ConfigurationRemoteBufferSize	= "wondermake_engine.configuration_remote.buffer_size";

	struct SGraphics
	{
		std::string WindowTitle;

		FilePath MissingTexturePath;
		FilePath DefaultRenderGraphPath;

		FilePath ImguiIniLocation;
		FilePath ImguiFontDirectory;
	};
	struct SAudio
	{
		FilePath MainNodeGraph;
	};

	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		FilePath aOverrideFileDevice,
		FilePath aOverrideFileUser,
		FilePath aResourceLoaderFile,
		std::optional<SGraphics> aGraphics,
		std::optional<SAudio> aAudio);
}
