#pragma once

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/System.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/PopulateObjectSystem.h"

using PopulatedSystemPolicySet
	= Policy::Set<
		PAdd<ConfigurationSystem, PWrite>,
		PAdd<JobSystem, PWrite>,
		PAdd<ScheduleSystem, PWrite>,
		PAdd<PopulateObjectSystem, PWrite>>;

template<typename TPolicySet = Policy::Set<>, typename TTraitSet = SystemTraits::Set<>>
class PopulatedSystem
	: public System<
		Policy::Concat<TPolicySet, PopulatedSystemPolicySet>,
		TTraitSet>
{
protected:
	template<CConfigType TConfigType, typename TDefaultArg>
	[[nodiscard]] inline TConfigType GetConfig(const auto& aId, TDefaultArg&& aDefaultValue)
	{
		return this->Get<ConfigurationSystem>().Get<TConfigType>(aId, std::forward<TDefaultArg>(aDefaultValue));
	}

};
