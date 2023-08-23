#include "ConfigurationUi.h"

#include "wondermake-base/ConfigurationGlobal.h"

namespace ConfigurationUi
{
	void Configure(
		ConfigurationSystem& aConfigurationSystem)
	{
		aConfigurationSystem.Set<FilePath>(SettingsFile, FilePath(FilePath::EFolder::Bin, "debugSettings.json"), EConfigGroup::Application);
	}
}

WM_REGISTER_CONFIGURATION(ConfigurationUi);
