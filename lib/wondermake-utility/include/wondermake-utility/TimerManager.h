#pragma once

#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/Executor.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Math.h"

#include <chrono>
#include <type_traits>
#include <vector>

template<typename TTimePoint>
concept CTimePoint = WmMath::Comparable<TTimePoint, TTimePoint>
	&& std::is_move_constructible_v<TTimePoint>
	&& std::is_move_assignable_v<TTimePoint>;

template<CTimePoint TTimePoint = std::chrono::high_resolution_clock::time_point>
class TimerManager
{
public:
	template<CExecutor TExecutor>
	inline TimerManager(TExecutor&& aExecutor) noexcept
		: myTimers(std::forward<TExecutor>(aExecutor))
	{}

	inline [[nodiscard]] Future<void> AddTimer(TTimePoint aExpiryPoint)
	{
		const auto pred = [&aExpiryPoint](const auto& aTimer)
		{
			return aExpiryPoint <= aTimer.ExpiryPoint;
		};

		auto it = std::find_if(myTimers.begin(), myTimers.end(), pred);
		auto [promise, future] = MakeAsync<void>();

		myTimers.insert(it, STimer{ std::move(promise), std::move(aExpiryPoint) });

		return std::move(future);
	}

	inline void RunExpiredTimers(const TTimePoint& aCurrentTime)
	{
		auto it = myTimers.begin();
		std::vector<Promise<void>> promises;

		for (; it != myTimers.end(); ++it)
		{
			if (it->ExpiryPoint > aCurrentTime)
				break;

			promises.emplace_back(std::move(it->Promise));
		}

		myTimers.erase(myTimers.begin(), it);

		for (auto& promise : promises)
			promise.Complete();
	}

private:
	struct STimer
	{
		Promise<void>	Promise;
		TTimePoint		ExpiryPoint;

		template<CExecutor TExecutor, typename TOnCancel>
		inline void OnCancel(TExecutor&& aExecutor, TOnCancel&& aOnCancel)
		{
			Promise.OnCancel(std::forward<TExecutor>(aExecutor), std::forward<TOnCancel>(aOnCancel));
		}
	};

	CancelableList<STimer> myTimers;

};
