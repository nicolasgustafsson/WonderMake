#include "wondermake-engine/SerializeConfigurationJob.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobGlobal.h"

#include "json/json.hpp"

#include <magic_enum.hpp>

WM_REGISTER_JOB(SerializeConfigurationJob);

std::string_view FilePathFolderToString(FilePath::EFolder aLocation)
{
	switch (aLocation)
	{
	case FilePath::EFolder::Unset:		return "Unset";
	case FilePath::EFolder::Bin:		return "Bin";
	case FilePath::EFolder::Data:		return "Data";
	case FilePath::EFolder::User:		return "User";
	case FilePath::EFolder::UserData:	return "UserData";
	}

	return "Unknown";
}

void SerializeConfigurationJob::Run(Promise<Output> aPromise, EConfigGroup aConfigGroup)
{
	auto&& configuration = Get<ConfigurationSystem>();

	auto overrides = configuration.GetConfigs();
	json j;

	auto& overridesScope = j["overrides"];

	for (auto&& [id, config] : overrides)
	{
		std::visit([aConfigGroup, &overridesScope, &name = id](const auto& aConfig)
			{
				using T = std::decay_t<decltype(aConfig)>::ConfigType;

				if (!aConfig.Override || aConfig.ConfigGroup != aConfigGroup)
					return;

				if constexpr (std::is_same_v<T, FilePath>)
				{
					json& filePath = overridesScope[name];

					filePath["path"] = aConfig.Override->Path.string();
					filePath["location"] = magic_enum::enum_name(aConfig.Override->Location);
				}
				else
					overridesScope[name] = *aConfig.Override;

			}, config);
	}

	aPromise.Complete(j.dump(4));
}
