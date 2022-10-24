#pragma once

#include "wondermake-base/ConfigurationSystem.h"

#include "wondermake-utility/AutoRegister.h"

#include <functional>
#include <vector>

class ConfigurationRegistry
{
public:
	template<typename TCallable>
	inline void AddConfigureFunc(TCallable&& aCallable)
	{
		myConfigureFuncs.emplace_back(std::forward<TCallable>(aCallable));
	}

	inline void Configure(ConfigurationSystem& aConfigSys)
	{
		for (const auto& configure : myConfigureFuncs)
			Utility::Invoke(configure, aConfigSys);
	}

private:
	std::vector<std::function<void(ConfigurationSystem&)>> myConfigureFuncs;

};

class GlobalConfiguration
{
public:
	static ConfigurationRegistry& GetRegistry();
};

#define WM_REGISTER_CONFIGURATION(aConfigureNamespace) WM_AUTO_REGISTER([](){ GlobalConfiguration::GetRegistry().AddConfigureFunc([](ConfigurationSystem& aConfigSys){ aConfigureNamespace ::Configure(aConfigSys); }); }, aConfigureNamespace)
