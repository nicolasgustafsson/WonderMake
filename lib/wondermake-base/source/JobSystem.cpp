#include "wondermake-base/JobSystem.h"

JobSystem::JobSystem(JobRegistry aJobRegistry, SystemContainer& aSystemContainer, AnyExecutor aExecutor) noexcept
	: myJobRegistry(std::move(aJobRegistry))
	, mySystemContainer(aSystemContainer)
	, myExecutor(std::move(aExecutor))
{}
