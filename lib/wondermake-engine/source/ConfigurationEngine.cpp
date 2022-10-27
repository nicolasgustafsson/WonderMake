#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"

#include "wondermake-utility/Vector.h"

using namespace MemoryUnitLiterals;

inline constexpr	SVector2i	locDefaultSize(1280, 720);
const				FilePath	locRenderGraphPath(FilePath::EFolder::Bin, "NodeGraphs/Render/Main Camera Main Display.json");

namespace ConfigurationEngine
{
	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		FilePath aOverrideFileDevice,
		FilePath aOverrideFileUser,
		bool aIsHeadless)
	{
		aConfigurationSystem.Set<std::string>(OverrideFileApplication,					std::move(aOverrideFileApplication),	EConfigGroup::Application);
		aConfigurationSystem.Set<FilePath>(OverrideFileDevice,							std::move(aOverrideFileDevice),			EConfigGroup::Application);
		aConfigurationSystem.Set<FilePath>(OverrideFileUser,							std::move(aOverrideFileUser),			EConfigGroup::Application);

		if (!aIsHeadless)
		{
			aConfigurationSystem.Set<i32>(ResolutionWidth,			locDefaultSize.X,	EConfigGroup::Device);
			aConfigurationSystem.Set<i32>(ResolutionHeight,			locDefaultSize.Y,	EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(ResolutionFitToWindow,	true,				EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(ResolutionFixedAspect,	false,				EConfigGroup::Device);
			aConfigurationSystem.Set<bool>(VSync,					true,				EConfigGroup::Device);
			aConfigurationSystem.Set<FilePath>(MainRenderGraphPath, locRenderGraphPath,	EConfigGroup::Device);

			aConfigurationSystem.Set<i32>(WindowWidth,				locDefaultSize.X,	EConfigGroup::Device);
			aConfigurationSystem.Set<i32>(WindowHeight,				locDefaultSize.Y,	EConfigGroup::Device);
		}

		aConfigurationSystem.Set<MemoryType>(ConfigurationRemoteBufferSize, 4_KiB, EConfigGroup::Application);
	}
}
