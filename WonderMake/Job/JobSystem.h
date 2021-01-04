#pragma once

#include "System/System.h"
#include "System/SystemContainer.h"

#include "Scheduling/ScheduleSystem.h"

#include <functional>
#include <memory>

class JobBase;

class JobSystem
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>>>
{
public:
	JobSystem(SystemContainer& aSystemContainer) noexcept;

	template<typename TJob, typename... TArgs>
	inline void CreateAndRun(TArgs&&... aArgs)
	{
		Run(Create<TJob>(std::forward<TArgs>(aArgs)...));
	}
	template<typename TJob, typename... TArgs>
	inline [[nodiscard]] std::shared_ptr<TJob> Create(TArgs&&... aArgs)
	{
		TJob::InjectDependencies(GetDependenciesHelper(TupleWrapper<typename TJob::Dependencies>()));

		return std::make_shared<TJob>(std::forward<TArgs>(aArgs)...);
	}
	template<typename TJob>
	inline void Run(std::shared_ptr<TJob> aJob)
	{
		Get<ScheduleSystem>().Schedule<typename TJob::PolicySet>([job = std::move(aJob)]()
		{
			job->Start();
		});
	}

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	SystemContainer& mySystemContainer;

	template<typename... TDependencies>
	inline std::tuple<TDependencies...> GetDependenciesHelper(TupleWrapper<std::tuple<TDependencies...>>)
	{
		return std::tie(mySystemContainer.GetSystem<std::decay_t<TDependencies>>()...);
	}

};
