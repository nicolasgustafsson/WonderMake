#include "wondermake-base/ConfigurationGlobal.h"

ConfigurationRegistry& GlobalConfiguration::GetRegistry()
{
	static ConfigurationRegistry instance;

	return instance;
}
