#include "wondermake-engine/SerializeConfigurationJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobGlobal.h"

#include "json/json.hpp"

WM_REGISTER_JOB(SerializeConfigurationJob);

void SerializeConfigurationJob::Run(Promise<Output> aPromise, EConfigGroup aConfigGroup)
{
	auto&& configuration = Get<ConfigurationSystem>();

	auto overrides = configuration.GetConfigs();
	json j;

	auto&& overridesScope = j["overrides"];

 //	overridesScope = json::object();

	for (auto&& [id, config] : overrides)
	{
		std::visit([aConfigGroup, &overridesScope, &name = id](const auto& aConfig)
			{
				using T = std::decay_t<decltype(aConfig)>::ConfigType;

				if (!aConfig.Override || aConfig.ConfigGroup != aConfigGroup)
					return;

				overridesScope[name] = *aConfig.Override;

			}, config);
	}

	aPromise.Complete(j.dump(4));
}
