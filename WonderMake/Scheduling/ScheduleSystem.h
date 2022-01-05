#pragma once

#include "WonderMakeBase/Typedefs.h"

#include "WonderMakeEngine/System.h"

#include <functional>

class ScheduleSystem
	: public System<>
{
public:
	using ScheduleProc = std::function<void(Closure)>;

	ScheduleSystem(ScheduleProc aScheduleProc, ScheduleProc aScheduleRepeatingProc) noexcept;

	// TODO(Kevin): Take policies into consideration.
	template<typename TPolicySet = Policy::Set<>>
	inline void Schedule(Closure&& aTask)
	{
		myScheduleProc(std::move(aTask));
	}
	template<typename TPolicySet = Policy::Set<>>
	inline void ScheduleRepeating(Closure&& aTask)
	{
		myScheduleRepeatingProc(std::move(aTask));
	}

private:
	ScheduleProc myScheduleProc;
	ScheduleProc myScheduleRepeatingProc;

};
