#include "wondermake-base/Configuration.h"

void Configuration::ResetOverride(const std::string& aId)
{
	std::unique_lock<std::shared_mutex> lock(myMutex);

	const auto it = myConfigs.find(aId);

	if (it == myConfigs.cend())
		return;

	std::visit([](auto& aConfig)
		{
			aConfig.Override.reset();
		}, it->second.Config);
}