#pragma once
#include "System/System.h"
#include "Utilities/Debugging/Debugged.h"
#include "Utilities/Stopwatch.h"

class TimeKeeper
	: public System
	, public Debugged
{
public:
	TimeKeeper() = default;
	~TimeKeeper() = default;

	f32 Update() noexcept;

	[[nodiscard]] __forceinline f32 GetDeltaSeconds() const noexcept;
	[[nodiscard]] __forceinline f64 GetDeltaSecondsPrecise() const noexcept;

	[[nodiscard]] __forceinline f32 GetGameTime() const noexcept;
	[[nodiscard]] __forceinline f64 GetGameTimePrecise() const noexcept;

protected:
	virtual void Debug() noexcept override;

	Stopwatch<> myStopwatch;
	Stopwatch<> myTotalTimeStopwatch;

	f64 myPreviousDeltaSecondsPrecise;
	f64 myTotalTimePassedPrecise;
	f32 myPreviousDeltaSeconds;
	f32 myTotalTimePassed;
};

[[nodiscard]] __forceinline f32 TimeKeeper::GetDeltaSeconds() const noexcept
{
	return myPreviousDeltaSeconds;
}

[[nodiscard]] __forceinline f64 TimeKeeper::GetDeltaSecondsPrecise() const noexcept
{
	return myPreviousDeltaSecondsPrecise;
}

[[nodiscard]] __forceinline f32 TimeKeeper::GetGameTime() const noexcept
{
	return myTotalTimePassed;
}

[[nodiscard]] __forceinline f64 TimeKeeper::GetGameTimePrecise() const noexcept
{
	return myTotalTimePassedPrecise;
}
