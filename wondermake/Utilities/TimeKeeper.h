#pragma once
#include "wondermake-base/System.h"
#include "Utilities/Debugging/Debugged.h"
#include "wondermake-utility/Stopwatch.h"

class TimeKeeper
	: public System<>
	, public Debugged
{
public:
	TimeKeeper()
		: Debugged("Time Keeper")
	{}
	~TimeKeeper() = default;

	f32 Update() noexcept;

	[[nodiscard]] f32 TimeSince(const f32 aTime) const noexcept;
	[[nodiscard]] f64 TimeSincePrecise(const f64 aTime) const noexcept;

	[[nodiscard]] __forceinline f32 GetDeltaSeconds() const noexcept;
	[[nodiscard]] __forceinline f64 GetDeltaSecondsPrecise() const noexcept;

	[[nodiscard]] __forceinline f32 GetGameTime() const noexcept;
	[[nodiscard]] __forceinline f64 GetGameTimePrecise() const noexcept;

protected:
	virtual void Debug() override;

	Stopwatch<> myStopwatch;
	Stopwatch<> myTotalTimeStopwatch;

	f64 myPreviousDeltaSecondsPrecise = 0.0;
	f64 myTotalTimePassedPrecise = 0.0;
	f32 myPreviousDeltaSeconds = 0.f;
	f32 myTotalTimePassed = 0.f;
	f32 myTimeDilation = 1.f;

	constexpr static f64 myMaxDeltaTime = 1.0 / 10.0;
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
