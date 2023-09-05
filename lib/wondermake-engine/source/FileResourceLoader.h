#pragma once

#include "wondermake-base/System.h"

class FileResourceSystem;

class ConfigurationSystem;
class JobSystem;

class FileResourceLoader
	: public System<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<FileResourceSystem, PWrite>,
			PAdd<JobSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	void Initialize();

private:

};
