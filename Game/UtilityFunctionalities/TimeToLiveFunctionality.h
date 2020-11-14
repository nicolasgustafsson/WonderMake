#pragma once
#include "Functionalities/TimerFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "Functionalities/Functionality.h"
#include "Functionalities/OwnerFunctionality.h"

class TimeToLiveFunctionality : public Functionality<
	Policy::Set<
		PAdd<SLevelDenizenComponent, PWrite>, 
		PAdd<TimerFunctionality, PWrite>,
		PAdd<OwnerFunctionality, PWrite>>>
{
public:
	TimeToLiveFunctionality(Object& aOwner, Dependencies&& aDependencies) : Super(aOwner, std::move(aDependencies)) {}

	void SetTimeToLive(const f32 aTimeToLive);
};
