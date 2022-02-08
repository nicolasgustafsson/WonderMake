#pragma once

#include "WonderMakeBase/System.h"

#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include <functional>

class ScheduleSystem
	: public System<>
{
public:
	using ScheduleProc = std::function<void(Closure)>;
	using ScheduleRepeatingProc = std::function<void(std::function<void()>)>;

	ScheduleSystem(ScheduleProc aScheduleProc, ScheduleRepeatingProc aScheduleRepeatingProc) noexcept;

	// TODO(Kevin): Take policies into consideration.
	template<typename TPolicySet = Policy::Set<>>
	inline void Schedule(Closure&& aTask)
	{
		myScheduleProc(std::move(aTask));
	}
	template<typename TPolicySet = Policy::Set<>>
	inline void ScheduleRepeating(std::function<void()>&& aTask)
	{
		myScheduleRepeatingProc(std::move(aTask));
	}

private:
	ScheduleProc myScheduleProc;
	ScheduleRepeatingProc myScheduleRepeatingProc;

};
