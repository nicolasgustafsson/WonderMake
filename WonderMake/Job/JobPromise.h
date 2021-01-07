#pragma once

#include "Utilities/Utility.h"

#include <functional>

template<typename... TArgs>
class JobPromise
{
public:
	template<typename TCallable>
	inline void SetCallbackCompleted(TCallable&& aCallable)
	{
		myCallableCompleted = std::forward<TCallable>(aCallable);
	}
	template<typename TCallable>
	inline void SetCallbackSuccess(TCallable aCallable)
	{
		myCallableSuccess = std::forward<TCallable>(aCallable);
	}

	inline void Fullfill(TArgs&&... aArgs)
	{
		Utility::Invoke(myCallableSuccess, std::forward<TArgs>(aArgs)...);

		Utility::Invoke(myCallableCompleted);
	}

private:
	std::function<void()>			myCallableCompleted;
	std::function<void(TArgs&&...)>	myCallableSuccess;

};
