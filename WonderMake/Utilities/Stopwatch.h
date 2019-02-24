#pragma once
#include <chrono>
#include "Typedefs.h"

template
	< typename TTime = WmChrono::dSeconds
	, typename TClock = std::chrono::high_resolution_clock
	, typename TDuration = f64>
	class Stopwatch
{
public:
	__forceinline Stopwatch() noexcept;
	__forceinline void Start() noexcept;
	__forceinline TDuration Restart() noexcept;
	__forceinline TDuration Stop() noexcept;
	[[nodiscard]] __forceinline TDuration GetElapsedTime() const noexcept;

private:
	std::chrono::time_point<TClock> myStart, myEnd;
	bool myIsRunning = false;
};

template<typename TTime, typename TClock, typename TDuration>
__forceinline Stopwatch<TTime, TClock, TDuration>::Stopwatch() noexcept
{
	Start();
}

template<typename TTime, typename TClock, typename TDuration>
__forceinline void Stopwatch<TTime, TClock, TDuration>::Start() noexcept
{
	myIsRunning = true;
	myStart = myEnd = TClock::now();
}

template<typename TTime, typename TClock, typename TDuration>
__forceinline TDuration Stopwatch<TTime, TClock, TDuration>::Stop() noexcept
{
	myEnd = TClock::now();
	myIsRunning = false;
	return GetElapsedTime();
}

template<typename TTime, typename TClock, typename TDuration>
__forceinline TDuration Stopwatch<TTime, TClock, TDuration>::Restart() noexcept
{
	TDuration elapsedTime = Stop();
	Start();
	return elapsedTime;
}

template<typename TTime, typename TClock, typename TDuration>
[[nodiscard]] __forceinline TDuration Stopwatch<TTime, TClock, TDuration>::GetElapsedTime() const noexcept
{
	const std::chrono::time_point<TClock> end = myIsRunning ? TClock::now() : myEnd;

	const TTime duration = end - myStart;

	return duration.count();
}
