#pragma once

#include "Utilities/Debugging/Debugged.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Stopwatch.h"
#include "wondermake-utility/TimerManager.h"

#include <chrono>

class ScheduleSystem;

class TimeKeeperSingleton
	: public System<
		Policy::Set<>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
	, public Debugged
{
public:
	using Duration	= std::chrono::high_resolution_clock::duration;
	using TimePoint	= std::chrono::high_resolution_clock::time_point;

	inline TimeKeeperSingleton() noexcept
		: Debugged("Time Keeper")
	{}

	void Update() noexcept;

	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetDeltaTime() const noexcept
	{
		return std::chrono::duration_cast<TDuration>(myPreviousDeltaTime);
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetDeltaTimeReal() const noexcept
	{
		return std::chrono::duration_cast<TDuration>(myPreviousDeltaTimeReal);
	}
	
	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetTotalTime() const noexcept
	{
		return std::chrono::duration_cast<TDuration>(myTimePassed);
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetTotalTimeReal() const noexcept
	{
		return std::chrono::duration_cast<TDuration>(myTimePassedReal);
	}

	template<typename TDuration>
	inline [[nodiscard]] Future<void> StartTimer(const TDuration& aDuration)
	{
		return myTimerManager.AddTimer(
			TimePoint(myTimePassed + std::chrono::duration_cast<Duration>(aDuration)));
	}
	template<typename TDuration>
	inline [[nodiscard]] Future<void> StartTimerReal(const TDuration& aDuration)
	{
		return myTimerManagerReal.AddTimer(
			TimePoint(myTimePassedReal + std::chrono::duration_cast<Duration>(aDuration)));
	}

protected:
	virtual void Debug() override;

	Stopwatch<>	myStopwatchDeltaTime;
	Stopwatch<>	myStopwatchTotalTime;
	
	TimerManager<> myTimerManager		= TimerManager<>(GetExecutor());
	TimerManager<> myTimerManagerReal	= TimerManager<>(GetExecutor());

	Duration	myPreviousDeltaTime		= Duration(0);
	Duration	myPreviousDeltaTimeReal	= Duration(0);

	Duration	myTimePassed		= Duration(0);
	Duration	myTimePassedReal	= Duration(0);

	f32 myTimeDilation = 1.f;

};

class TimeKeeper
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>,
			PAdd<TimeKeeperSingleton, PRead>>,
		STrait::Set<
			STWonderMake>>
{
public:
	using Duration	= TimeKeeperSingleton::Duration;
	using TimePoint = TimeKeeperSingleton::TimePoint;
	
	void Initialize() override;

	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetDeltaTime() const noexcept
	{
		return Get<TimeKeeperSingleton>().GetDeltaTime<TDuration>();
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetDeltaTimeReal() const noexcept
	{
		return Get<TimeKeeperSingleton>().GetDeltaTimeReal<TDuration>();
	}

	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetTotalTime() const noexcept
	{
		return std::chrono::duration_cast<TDuration>(
			Get<TimeKeeperSingleton>().GetTotalTime<Duration>() -
			myStartTime);
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline [[nodiscard]] TDuration GetTotalTimeReal() const noexcept
	{
		return std::chrono::duration_cast<TDuration>(
			Get<TimeKeeperSingleton>().GetTotalTimeReal<Duration>() -
			myStartTimeReal);
	}

	template<typename TDuration>
	inline [[nodiscard]] Future<void> StartTimer(const TDuration& aDuration)
	{
		return myTimerManager.AddTimer(
			TimePoint(GetTotalTime<Duration>() + std::chrono::duration_cast<Duration>(aDuration)));
	}
	template<typename TDuration>
	inline [[nodiscard]] Future<void> StartTimerReal(const TDuration& aDuration)
	{
		return myTimerManagerReal.AddTimer(
			TimePoint(GetTotalTimeReal<Duration>() + std::chrono::duration_cast<Duration>(aDuration)));
	}

private:
	void Tick();

	EventSubscriber	myEventTick;
	Duration		myStartTime		= Duration(0);
	Duration		myStartTimeReal	= Duration(0);

	TimerManager<> myTimerManager		= TimerManager<>(GetExecutor());
	TimerManager<> myTimerManagerReal	= TimerManager<>(GetExecutor());

};
