#pragma once

#include "System/System.h"
#include "System/SystemContainer.h"

#include "Scheduling/ScheduleSystem.h"

#include <tuple>

class JobBase;

class JobSystem
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>>>
{
public:
	JobSystem(SystemContainer& aSystemContainer) noexcept;

	template<typename TJob, typename... TArgs>
	inline void Run(TArgs&&... aArgs)
	{
		Get<ScheduleSystem>().Schedule<typename TJob::PolicySet>([this, ...args = std::forward<TArgs>(aArgs)]()
		{
			TJob::InjectDependencies(GetDependenciesHelper(TupleWrapper<typename TJob::Dependencies>()));

			// TODO(Kevin): Forwarding?
			TJob job(args...);
		});
	}

private:
	template<typename TDependencyTuple>
	struct TupleWrapper {};

	template<typename... TDependencies>
	inline std::tuple<TDependencies...> GetDependenciesHelper(TupleWrapper<std::tuple<TDependencies...>>)
	{
		return std::tie(mySystemContainer.GetSystem<std::decay_t<TDependencies>>()...);
	}

	SystemContainer& mySystemContainer;

};
