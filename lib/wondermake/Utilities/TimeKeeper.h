#pragma once

#include "Utilities/Debugging/Debugged.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/Stopwatch.h"

#include <chrono>

class TimeKeeper
	: public System<>
	, public Debugged
{
public:
	using Duration = std::chrono::high_resolution_clock::duration;

	inline TimeKeeper() noexcept
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

protected:
	virtual void Debug() override;

	Stopwatch<>	myStopwatchDeltaTime;
	Stopwatch<>	myStopwatchTotalTime;

	Duration	myPreviousDeltaTime		= Duration(0);
	Duration	myPreviousDeltaTimeReal	= Duration(0);

	Duration	myTimePassed		= Duration(0);
	Duration	myTimePassedReal	= Duration(0);

	f32 myTimeDilation = 1.f;

};
