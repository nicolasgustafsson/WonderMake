#pragma once
#include "System/System.h"
#include "Utilities/Debugging/Debugged.h"
#include "Utilities/Stopwatch.h"

namespace Wm
{
    class TimeKeeper
        : public System<ScheduleSystem>
        , public Debugged
    {
    public:
        TimeKeeper()
            : Debugged("Time Keeper")
        {
        }
        ~TimeKeeper() = default;

        f32 Update() noexcept;

        [[nodiscard]] f32 TimeSince(const f32 aTime) const noexcept;
        [[nodiscard]] f64 TimeSincePrecise(const f64 aTime) const noexcept;

        [[nodiscard]] inline f32 GetDeltaSeconds() const noexcept;
        [[nodiscard]] inline f64 GetDeltaSecondsPrecise() const noexcept;

        [[nodiscard]] inline f32 GetGameTime() const noexcept;
        [[nodiscard]] inline f64 GetGameTimePrecise() const noexcept;

        void SetTimeScale(f32 aTimeScale);
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
}

[[nodiscard]] inline f32 Wm::TimeKeeper::GetDeltaSeconds() const noexcept
{
	return myPreviousDeltaSeconds;
}

[[nodiscard]] inline f64 Wm::TimeKeeper::GetDeltaSecondsPrecise() const noexcept
{
	return myPreviousDeltaSecondsPrecise;
}

[[nodiscard]] inline f32 Wm::TimeKeeper::GetGameTime() const noexcept
{
	return myTotalTimePassed;
}

[[nodiscard]] inline f64 Wm::TimeKeeper::GetGameTimePrecise() const noexcept
{
	return myTotalTimePassedPrecise;
}
