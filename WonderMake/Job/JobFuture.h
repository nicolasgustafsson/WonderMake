#pragma once

#include "Job/JobPromise.h"

#include "Scheduling/ScheduleSystem.h"

#include <tuple>

template<typename... TArgs>
class JobFuture
{
public:
	inline JobFuture(JobPromise<TArgs...>& aPromise, ScheduleSystem& aScheduleSystem)
		: myPromise(aPromise)
		, myScheduleSystem(aScheduleSystem)
	{}

	template<typename TCallable>
	inline void Then(TCallable&& aCallable)
	{
		static_assert(std::is_invocable_v<TCallable, TArgs...>, "Invalid input.");

		using PolicySet = Policy::Set<>;

		myPromise.SetCallbackSuccess([&schedule = myScheduleSystem, callable = std::forward<TCallable>(aCallable)](TArgs&&... aArgs)
		{
			auto args = std::forward_as_tuple(std::forward<TArgs>(aArgs)...);

			schedule.Schedule<PolicySet>([callable = std::move(callable), args = std::move(args)]()
			{
				std::apply(
					std::move(callable),
					std::move(args));
			});
		});
	}
	template<typename TCallable, typename TCargo>
	inline void Then(TCargo&& aCargo, TCallable&& aCallable)
	{
		static_assert(std::is_invocable_v<TCallable, TCargo, TArgs...>, "Invalid input.");

		using PolicySet = typename TCargo::PolicySet;

		myPromise.SetCallbackSuccess([&schedule = myScheduleSystem, callable = std::forward<TCallable>(aCallable), cargo = std::forward<TCargo>(aCargo)](TArgs&&... aArgs)
		{
			auto argsTuple = std::make_tuple(std::move(cargo), std::forward<TArgs>(aArgs)...);

			auto args = std::make_shared<decltype(argsTuple)>(std::move(argsTuple));

			schedule.Schedule<PolicySet>([callable = std::move(callable), args = std::move(args)]()
			{
				std::apply(
					std::move(callable),
					std::move(*args));
			});
		});
	}
	
protected:
	JobPromise<TArgs...>& myPromise;
	ScheduleSystem& myScheduleSystem;

};
