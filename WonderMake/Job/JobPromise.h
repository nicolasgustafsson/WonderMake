#pragma once

#include "Job/JobOutput.h"

#include "WonderMakeBase/UniqueFunction.h"
#include "WonderMakeBase/Utility.h"

#include <functional>

template<
	typename TOutput,
	typename TOutputError>
class JobPromise
{
public:
	template<typename... TArgs>
	static constexpr bool IsInvocableWithFulfill = JobCallbackImpl<TOutput>::IsInvocableWith<TArgs...>;
	template<typename... TArgs>
	static constexpr bool IsInvocableWithFail = JobCallbackImpl<TOutputError>::IsInvocableWith<TArgs...>;
	
	template<typename TCallable, typename... TArgs>
	static constexpr bool IsInvocableFulfill = JobCallbackImpl<TOutput>::template IsInvocable<TCallable, TArgs...>;
	template<typename TCallable, typename... TArgs>
	static constexpr bool IsInvocableFail = JobCallbackImpl<TOutputError>::template IsInvocable<TCallable, TArgs...>;

	template<typename TCallable> requires std::is_invocable_v<TCallable>
	inline void SetCallbackCompleted(TCallable&& aCallable)
	{
		myCallableCompleted.emplace(std::forward<TCallable>(aCallable));
	}
	template<typename TCallable>
	inline void SetCallbackSuccess(TCallable&& aCallback)
	{
		myCallbackSuccess.SetCallback(std::forward<TCallable>(aCallback));
	}
	template<typename TCallable>
	inline void SetCallbackFailure(TCallable&& aCallback)
	{
		myCallbackFailure.SetCallback(std::forward<TCallable>(aCallback));
	}

	template<typename... TArgs> requires IsInvocableWithFulfill<TArgs...>
	inline void Fullfill(TArgs&&... aArgs)
	{
		myCallbackSuccess.Invoke(std::forward<TArgs>(aArgs)...);

		Utility::Invoke(std::move(myCallableCompleted));
	}
	template<typename... TArgs> requires IsInvocableWithFail<TArgs...>
	inline void Fail(TArgs&&... aArgs)
	{
		myCallbackFailure.Invoke(std::forward<TArgs>(aArgs)...);

		Utility::Invoke(std::move(myCallableCompleted));
	}

private:
	template<typename TOutput>
	class JobCallbackImpl
	{
		static_assert(AlwaysFalse<TOutput>, "Invalid output.");
	};
	template<typename... TArgs>
	class JobCallbackImpl<JobOutput<TArgs...>>
	{
	public:
		template<typename... TArgsCompare>
		inline static constexpr bool IsInvocableWith = std::is_invocable_v<std::function<void(TArgs&&...)>, TArgsCompare...>;
		template<typename TCallable, typename... TArgsCompare>
		inline static constexpr bool IsInvocable = std::is_invocable_v<TCallable, TArgsCompare..., TArgs...>;

		template<typename TCallable> requires IsInvocable<TCallable>
		inline void SetCallback(TCallable&& aCallable)
		{
			myCallback.emplace(std::forward<TCallable>(aCallable));
		}

		inline void Invoke(TArgs&&... aArgs)
		{
			Utility::Invoke(std::move(myCallback), std::forward<TArgs>(aArgs)...);
		}

	private:
		std::optional<UniqueFunction<void(TArgs&&...)>> myCallback;

	};

	std::optional<UniqueFunction<void()>>	myCallableCompleted;
	JobCallbackImpl<TOutput>				myCallbackSuccess;
	JobCallbackImpl<TOutputError>			myCallbackFailure;

};
