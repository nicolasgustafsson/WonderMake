#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"

using namespace MemoryUnitLiterals;

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
			aConfigurationSystem.Set<bool>(VSync,		true,	EConfigGroup::Device);

			aConfigurationSystem.Set<i32>(WindowWidth,	1280,	EConfigGroup::Device);
			aConfigurationSystem.Set<i32>(WindowHeight,	720,	EConfigGroup::Device);
		}

		aConfigurationSystem.Set<MemoryType>(ConfigurationRemoteBufferSize, 4_KiB, EConfigGroup::Application);
	}
}
