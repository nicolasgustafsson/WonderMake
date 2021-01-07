#pragma once

#include "Job/JobPromise.h"

template<typename... TArgs>
class JobFuture
{
public:
	inline JobFuture(JobPromise<TArgs...>& aPromise)
		: myPromise(aPromise)
	{}

	template<typename TCallable>
	inline void Then(TCallable aCallable)
	{
		static_assert(std::is_invocable_v<TCallable, TArgs...>, "Invalid input.");

		myPromise.SetCallbackSuccess(std::forward<TCallable>(aCallable));
	}
	template<typename TCallable, typename TCargo>
	inline void Then(TCargo aCargo, TCallable aCallable)
	{
		static_assert(std::is_invocable_v<TCallable, TCargo, TArgs...>, "Invalid input.");

		Then([cargo = std::forward<TCargo>(aCargo), callback = std::forward<TCallable>(aCallable)](auto&&... aArgs)
		{
			// TODO(Kevin): Forwarding?
			callback(cargo, aArgs...);
		});
	}
	
private:
	JobPromise<TArgs...>& myPromise;

};
