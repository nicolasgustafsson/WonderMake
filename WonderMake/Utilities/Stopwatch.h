#pragma once
#include <chrono>
#include "Typedefs.h"

template
	< typename TTime = WmChrono::dSeconds
	, typename TClock = std::chrono::high_resolution_clock>
	class Stopwatch
{
public:
	using TDurationRep = typename TTime::rep;

	inline Stopwatch() noexcept
	{
		Start();
	}

    inline void Start() noexcept
	{
		myIsRunning = true;
		myStart = myEnd = TClock::now();
	}

    inline TDurationRep Stop() noexcept
	{
		myEnd = TClock::now();
		myIsRunning = false;
		return GetElapsedTime();
	}

    inline TDurationRep Restart() noexcept
	{
		TDurationRep elapsedTime = Stop();
		Start();
		return elapsedTime;
	}

    [[nodiscard]] inline TDurationRep GetElapsedTime() const noexcept
	{
		const std::chrono::time_point<TClock> end = myIsRunning ? TClock::now() : myEnd;

		const TTime duration = end - myStart;

		return duration.count();
	}

private:
	std::chrono::time_point<TClock> myStart;
	std::chrono::time_point<TClock> myEnd;
	bool myIsRunning = false;
};