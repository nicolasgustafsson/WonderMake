#pragma once

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/Functionality.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/System.h"
#include "wondermake-base/PopulateObjectSystem.h"

using PopulatedFunctionalityPolicySet
	= Policy::Set<
		PAdd<ConfigurationSystem, PWrite>,
		PAdd<JobSystem, PWrite>,
		PAdd<PopulateObjectSystem, PWrite>>;

template<typename TPolicySet = Policy::Set<>, typename TTraitSet = SystemTraits::Set<>>
using PopulatedFunctionality
	= Functionality<
		Policy::Concat<TPolicySet, PopulatedFunctionalityPolicySet>,
		TTraitSet>;
