#pragma once

#include "wondermake-base/Configuration.h"
#include "wondermake-base/JobBase.h"

#include "wondermake-utility/Result.h"

#include <filesystem>

class ConfigurationSystem;

enum class EDeserializeConfigurationError
{
	ParsingError,
	InvalidJsonStructure
};

class DeserializeConfigurationJob
	: public Job<
		JobInputSet<
			JobInput<EConfigGroup, std::string>>,
		Result<EDeserializeConfigurationError>,
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>>>
{
public:
	void Run(Promise<Output> aPromise, EConfigGroup aConfigGroup, std::string aJsonBlob) override;

};
