#pragma once

#include "wondermake-base/Configuration.h"
#include "wondermake-base/JobBase.h"

#include <filesystem>

class ConfigurationSystem;

class SerializeConfigurationJob
	: public Job<
		JobInputSet<
			JobInput<EConfigGroup>>,
		std::string,
		Policy::Set<
			PAdd<ConfigurationSystem, PRead>>>
{
public:
	void Run(Promise<Output> aPromise, EConfigGroup aConfigGroup) override;

};
