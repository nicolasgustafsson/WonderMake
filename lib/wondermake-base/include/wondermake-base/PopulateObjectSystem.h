#pragma once

#include "wondermake-base/FunctionalitySystem.h"
#include "wondermake-base/DependencySystem.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/Object.h"
#include "wondermake-base/System.h"
#include "wondermake-base/SystemPtr.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Future.h"

inline constexpr auto TagWmPopulate = MakeLogTag("WmPopulate");

class PopulateObjectSystem
	: public System<
		Policy::Set<
			PAdd<DependencySystem, PWrite>>,
		STrait::Set<
			STWonderMake>>
{
public:
	template<typename TFunctionality>
	inline Future<std::reference_wrapper<TFunctionality>> AddFunctionality(Object& aObject)
	{
		using FunctionalityDelegate = FunctionalitySystemDelegate<std::decay_t<TFunctionality>>;

		return Get<DependencySystem>()
			.Fetch<FunctionalityDelegate>()
			.ThenApply(GetExecutor(), FutureApplyResult([&aObject](auto aResult) -> Future<std::reference_wrapper<TFunctionality>>
				{
					if (aResult)
						return MakeCompletedFuture<std::reference_wrapper<TFunctionality>>(aResult.Unwrap().get().AddFunctionality(aObject));

					WmLogError(TagWonderMake << TagWmPopulate << "Failed to fetch functionality system: " << typeid(TFunctionality).name() << ", error: " << aResult.Err() << '.');
					
					return MakeCanceledFuture<std::reference_wrapper<TFunctionality>>();
				}));
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
