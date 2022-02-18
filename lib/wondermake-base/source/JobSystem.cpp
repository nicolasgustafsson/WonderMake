#include "wondermake-base/JobSystem.h"

namespace jobs_refactor // Temporary until refactor is done.
{

JobSystem::JobSystem(JobRegistry aJobRegistry, SystemContainer& aSystemContainer, AnyExecutor aExecutor) noexcept
	: myJobRegistry(std::move(aJobRegistry))
	, mySystemContainer(aSystemContainer)
	, myExecutor(std::move(aExecutor))
{}

}
