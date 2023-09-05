#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"

#include "wondermake-utility/Vector.h"

using namespace MemoryUnitLiterals;

inline constexpr	SVector2i	locDefaultSize(1280, 720);

namespace ConfigurationEngine
{
	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		FilePath aOverrideFileDevice,
		FilePath aOverrideFileUser,
		FilePath aResourceLoaderFile,
		std::optional<SGraphics> aGraphics,
		std::optional<SAudio> aAudio)
	{
		aConfigurationSystem.Set<std::string>(OverrideFileApplication,	std::move(aOverrideFileApplication),			EConfigGroup::Application);
		aConfigurationSystem.Set<FilePath>(OverrideFileDevice,			std::move(aOverrideFileDevice),					EConfigGroup::Application);
		aConfigurationSystem.Set<FilePath>(OverrideFileUser,			std::move(aOverrideFileUser),					EConfigGroup::Application);
		
		aConfigurationSystem.Set<FilePath>(ResourceLoaderFile,			std::move(aResourceLoaderFile),					EConfigGroup::Application);

		if (aGraphics)
		{
			aConfigurationSystem.Set<std::string>(WindowTitle,			std::move(aGraphics->WindowTitle),				EConfigGroup::Device);
			aConfigurationSystem.Set<i32>(WindowWidth,					locDefaultSize.X,								EConfigGroup::Device);
			aConfigurationSystem.Set<i32>(WindowHeight,					locDefaultSize.Y,								EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(WindowMaximized,				false,											EConfigGroup::Device);
			
			aConfigurationSystem.Set<i32>(ResolutionWidth,				locDefaultSize.X,								EConfigGroup::Device);
			aConfigurationSystem.Set<i32>(ResolutionHeight,				locDefaultSize.Y,								EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(ResolutionFitToWindow,		true,											EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(ResolutionFixedAspect,		false,											EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(VSync,						true,											EConfigGroup::Device);
			
			aConfigurationSystem.Set<FilePath>(DefaultRenderGraphPath,	std::move(aGraphics->DefaultRenderGraphPath),	EConfigGroup::Application);
			aConfigurationSystem.Set<FilePath>(MissingTexturePath,		std::move(aGraphics->MissingTexturePath),		EConfigGroup::Application);

			aConfigurationSystem.Set<FilePath>(ImguiIniLocation,		std::move(aGraphics->ImguiIniLocation),			EConfigGroup::Application);
			aConfigurationSystem.Set<FilePath>(ImguiFontDirectory,		std::move(aGraphics->ImguiFontDirectory),		EConfigGroup::Application);
		}
		if (aAudio)
		{
			aConfigurationSystem.Set<FilePath>(AudioNodeGraphPath,		std::move(aAudio->MainNodeGraph),				EConfigGroup::Application);
		}

		aConfigurationSystem.Set<MemoryType>(ConfigurationRemoteBufferSize, 4_KiB, EConfigGroup::Application);
	}
}
