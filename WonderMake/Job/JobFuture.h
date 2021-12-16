#pragma once

#include "Job/JobOutput.h"
#include "Job/JobPromise.h"

#include "Scheduling/ScheduleSystem.h"

#include "Typedefs.h"

template<typename TOutput, typename TOutputError>
class JobFuture
{
public:
	using SelfType = JobFuture<TOutput, TOutputError>;
	using FuturePromise = JobPromise<TOutput, TOutputError>;

	inline JobFuture(FuturePromise& aPromise, ScheduleSystem& aScheduleSystem)
		: myPromise(aPromise)
		, myScheduleSystem(aScheduleSystem)
	{}

	template<typename TCallable> requires FuturePromise::template IsInvocableFulfill<TCallable>
	inline void Then(TCallable&& aCallable)
	{
		using PolicySet = Policy::Set<>;

		myPromise.SetCallbackSuccess(JobCallbackImpl<TOutput>::template CreateCallback<PolicySet, TCallable>(myScheduleSystem, std::forward<TCallable>(aCallable)));
	}
	template<typename TCallable, typename TCargo> requires FuturePromise::template IsInvocableFulfill<TCallable, TCargo>
	inline void Then(TCargo&& aCargo, TCallable&& aCallable)
	{
		using PolicySet = typename TCargo::PolicySet;

		myPromise.SetCallbackSuccess(JobCallbackImpl<TOutput>::template CreateCallback<PolicySet, TCallable>(myScheduleSystem, std::forward<TCallable>(aCallable), std::forward<TCargo>(aCargo)));
	}
	
	template<typename TCallable> requires FuturePromise::template IsInvocableFail<TCallable>
	inline SelfType& Error(TCallable&& aCallable)
	{
		using PolicySet = Policy::Set<>;

		myPromise.SetCallbackFailure(JobCallbackImpl<TOutputError>::template CreateCallback<PolicySet, TCallable>(myScheduleSystem, std::forward<TCallable>(aCallable)));

		return *this;
	}
	template<typename TCallable, typename TCargo> requires FuturePromise::template IsInvocableFail<TCallable, TCargo>
	inline SelfType& Error(TCargo&& aCargo, TCallable&& aCallable)
	{
		using PolicySet = typename TCargo::PolicySet;

		myPromise.SetCallbackFailure(JobCallbackImpl<TOutputError>::template CreateCallback<PolicySet, TCallable>(myScheduleSystem, std::forward<TCallable>(aCallable), std::forward<TCargo>(aCargo)));

		return *this;
	}
	
protected:
	template<typename UOutput>
	class JobCallbackImpl
	{
		static_assert(AlwaysFalse<UOutput>, "Invalid input.");
	};
	template<typename... TArgs>
	class JobCallbackImpl<JobOutput<TArgs...>>
	{
	public:
		template<typename TPolicySet, typename TCallable, typename... TAdditionalArgs> requires std::is_invocable_v<TCallable, TAdditionalArgs..., TArgs...>
		inline static auto CreateCallback(ScheduleSystem& aScheduleSystem, TCallable&& aCallable, TAdditionalArgs&&... aAdditionalArgs)
		{
			return [&aScheduleSystem, callable = std::forward<TCallable>(aCallable), ...additionalArgs = std::forward<TAdditionalArgs>(aAdditionalArgs)](TArgs&&... aArgs) mutable
			{
				aScheduleSystem.Schedule<TPolicySet>([callable = std::move(callable), ...additionalArgs = std::move(additionalArgs), ...args = std::forward<TArgs>(aArgs)]() mutable
				{
					(void)callable(std::move(additionalArgs)..., std::move(args)...);
				});
			};
		}
	};

	FuturePromise& myPromise;
	ScheduleSystem& myScheduleSystem;

};
