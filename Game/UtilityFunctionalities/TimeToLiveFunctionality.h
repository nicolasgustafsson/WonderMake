#pragma once
#include "Functionalities/TimerFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include "Functionalities/Functionality.h"
#include <Functionalities/OwnerFunctionality.h>

class TimeToLiveFunctionality : public Functionality<
	TimeToLiveFunctionality,
	Policy::Set<
	Policy::Add<SLevelDenizenComponent, Policy::EPermission::Write>, 
	Policy::Add<TimerFunctionality, Policy::EPermission::Write>,
	Policy::Add<OwnerFunctionality, Policy::EPermission::Write >> >
{
public:
	TimeToLiveFunctionality(Object& aOwner) : Super(aOwner) {}

	void SetTimeToLive(const f32 aTimeToLive);
};

