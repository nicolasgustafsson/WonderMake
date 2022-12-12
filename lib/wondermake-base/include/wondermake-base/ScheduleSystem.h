#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include <functional>

class ScheduleSystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STWonderMake>>
{
public:
	using ScheduleProc			= std::function<Future<void>(AnyExecutor, Closure)>;
	using ScheduleRepeatingProc	= std::function<EventSubscriber(AnyExecutor, std::function<void()>)>;

	ScheduleSystem(ScheduleProc aScheduleProc, ScheduleRepeatingProc aScheduleRepeatingProc) noexcept;

	// TODO(Kevin): Take policies into consideration.
	template<typename TPolicySet = Policy::Set<>>
	inline Future<void> Schedule(AnyExecutor aExecutor, Closure&& aTask)
	{
		return myScheduleProc(std::move(aExecutor), std::move(aTask));
	}
	template<typename TPolicySet = Policy::Set<>>
	inline EventSubscriber ScheduleRepeating(AnyExecutor aExecutor, std::function<void()>&& aTask)
	{
		return myScheduleRepeatingProc(std::move(aExecutor), std::move(aTask));
	}

private:
	ScheduleProc myScheduleProc;
	ScheduleRepeatingProc myScheduleRepeatingProc;

};
