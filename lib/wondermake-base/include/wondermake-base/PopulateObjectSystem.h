#pragma once

#include "wondermake-base/FunctionalitySystem.h"
#include "wondermake-base/Object.h"
#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/System.h"
#include "wondermake-base/SystemPtr.h"

#include "wondermake-utility/Future.h"

class PopulateObjectSystem
	: public System<
		Policy::Set<
			PAdd<ScheduleSystem, PWrite>>,
		STrait::Set<
			STWonderMake>>
{
public:
	template<typename TFunctionality>
	inline Future<std::reference_wrapper<TFunctionality>> AddFunctionality(Object& aObject)
	{
		auto [promise, future] = MakeAsync<std::reference_wrapper<TFunctionality>>();

		using FunctionalityDelegate = FunctionalitySystemDelegate<std::decay_t<TFunctionality>>;

		Get<ScheduleSystem>().Schedule<FunctionalityDelegate>(GetExecutor(), [promise = std::move(promise), &aObject]() mutable
			{
				promise.Complete(SystemPtr<FunctionalityDelegate>()->AddFunctionality(aObject));
			})
			.Detach();

		return std::move(future);
	}

};

template<typename TCallable>
auto FutureAddFunctionality(TCallable&& aCallable)
{
	return[callable = std::forward<TCallable>(aCallable)](auto&& aFuture) mutable
	{
		if (!aFuture.IsCompleted())
			return;

		auto result = std::move(aFuture).GetResult();

		if (!result)
			return;

		std::move(callable)(result->get());
	};
}
