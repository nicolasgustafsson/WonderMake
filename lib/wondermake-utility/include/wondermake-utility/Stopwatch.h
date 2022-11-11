#pragma once
#include "Typedefs.h"

#include <chrono>
#include <type_traits>

template<
	typename TClock = std::chrono::high_resolution_clock>
	class Stopwatch
{
public:
	inline constexpr Stopwatch() noexcept
		requires(std::is_empty_v<TClock> || std::is_default_constructible_v<TClock>)
	{
		Start();
	}
	inline constexpr Stopwatch(TClock aClock) noexcept
		requires(!std::is_empty_v<TClock> && std::is_move_constructible_v<TClock>)
		: myClockBody(SClockBody<TClock>{ std::move(aClock) })
	{
		Start();
	}

	inline constexpr void Start() noexcept
	{
		if (myIsRunning)
			return;

		if (myStart == myEnd)
			myStart = myEnd = Now();
		else
			myStart = myEnd = Now() - (myEnd - myStart);
		myIsRunning = true;
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline constexpr TDuration Stop() noexcept
	{
		if (myIsRunning)
		{
			myEnd = Now();
			myIsRunning = false;
		}

		return GetElapsedTime<TDuration>();
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline constexpr TDuration Reset() noexcept
	{
		if (myIsRunning)
			myEnd = Now();

		const auto elapsedTime = std::chrono::duration_cast<TDuration>(myEnd - myStart);
		myIsRunning = false;

		myStart = myEnd;

		return elapsedTime;
	}
	template<typename TDuration = WmChrono::dSeconds>
	inline constexpr TDuration Restart() noexcept
	{
		if (myIsRunning)
			myEnd = Now();

		const auto elapsedTime = std::chrono::duration_cast<TDuration>(myEnd - myStart);

		myStart = myEnd;
		myIsRunning = true;

		return elapsedTime;
	}

	template<typename TDuration = WmChrono::dSeconds>
	inline constexpr [[nodiscard]] TDuration GetElapsedTime() const noexcept
	{
		const auto end = myIsRunning ? Now() : myEnd;

		return std::chrono::duration_cast<TDuration>(end - myStart);
	}

private:
	template<typename TClock>
	struct SClockBody;

	template<typename TClock>
		requires(std::is_empty_v<TClock>)
	struct SClockBody<TClock>
	{};
	template<typename TClock>
		requires(!std::is_empty_v<TClock>)
	struct SClockBody<TClock>
	{
		TClock Clock;
	};

	inline constexpr [[nodiscard]] auto Now() const noexcept
	{
		if constexpr (std::is_empty_v<TClock>)
			return TClock::now();
		else
			return myClockBody.Clock.now();
	}

	TClock::time_point	myStart;
	TClock::time_point	myEnd;

	SClockBody<TClock>	myClockBody;
	bool				myIsRunning = false;

};