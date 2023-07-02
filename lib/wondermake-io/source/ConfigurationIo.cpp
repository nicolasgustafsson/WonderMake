#include "wondermake-io/ConfigurationIo.h"

#include "wondermake-base/ConfigurationSystem.h"

namespace ConfigurationIo
{
	void Configure(
		ConfigurationSystem&	aConfigurationSystem,
		std::filesystem::path	aPathData,
		std::filesystem::path	aPathUser,
		std::filesystem::path	aPathUserData)
	{
		aConfigurationSystem.Set<std::string>(ConfigDirectoryExtraBins,	"",									EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(ConfigDirectoryData,		std::move(aPathData).string(),		EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(ConfigDirectoryUser,		std::move(aPathUser).string(),		EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(ConfigDirectoryUserData,	std::move(aPathUserData).string(),	EConfigGroup::Application);
	}
}
