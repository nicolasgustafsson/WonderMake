#include "wondermake-io/ConfigurationIo.h"

#include "wondermake-base/ConfigurationSystem.h"

namespace ConfigurationIo
{
	void Configure(ConfigurationSystem& aConfigurationSystem)
	{
		constexpr auto defaultDirectory = "";

		aConfigurationSystem.Set<std::string>(static_cast<std::string>(ConfigDirectoryData),		defaultDirectory,	EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(static_cast<std::string>(ConfigDirectoryUser),		defaultDirectory,	EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(static_cast<std::string>(ConfigDirectoryUserData),	defaultDirectory,	EConfigGroup::Application);
	}
}
