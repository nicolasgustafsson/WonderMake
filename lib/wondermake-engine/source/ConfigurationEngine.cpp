#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"

namespace ConfigurationEngine
{
	void Configure(
		ConfigurationSystem& aConfigurationSystem,
		std::string aOverrideFileApplication,
		std::string aOverrideFileDevice,
		std::string aOverrideFileUser,
		EOverrideFileUserLocation aOverrideFileUserLocation)
	{
		aConfigurationSystem.Set<std::string>(OverrideFileApplication,					std::move(aOverrideFileApplication),	EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(OverrideFileDevice,						std::move(aOverrideFileDevice),			EConfigGroup::Application);
		aConfigurationSystem.Set<std::string>(OverrideFileUser,							std::move(aOverrideFileUser),			EConfigGroup::Application);
		aConfigurationSystem.Set<EOverrideFileUserLocation>(OverrideFileUserLocation,	aOverrideFileUserLocation,				EConfigGroup::Application,
			MakeAllowedValues<EOverrideFileUserLocation>(
				std::make_pair("User",		EOverrideFileUserLocation::User),
				std::make_pair("UserData",	EOverrideFileUserLocation::UserData)));
		
		aConfigurationSystem.Set<i32>(WindowWidth, 1280, EConfigGroup::Device);
		aConfigurationSystem.Set<i32>(WindowHeight, 720, EConfigGroup::Device);
	}
}
