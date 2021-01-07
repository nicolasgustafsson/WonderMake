#pragma once

#include "Job/Job.h"

#include "Object/Object.h"

#include "Functionalities/FunctionalitySystem.h"

#include <tuple>

namespace _Impl
{
	template<typename... TFunctionalities>
	using _CreateObjectJobType = Job<
		Policy::Set<
			PAdd<FunctionalitySystemDelegate<std::decay_t<TFunctionalities>>, PWrite>...>>;
}

// TODO(Kevin): Make require write access for inherited policies. Do this by spawning smaller jobs.
template<typename... TFunctionalities>
class CreateObjectJob final
	: public _Impl::_CreateObjectJobType<TFunctionalities...>
{
public:
	inline CreateObjectJob() noexcept
	{
		(void)std::tie(AddFunctionalityHelper<TFunctionalities>()...);

		_Impl::_CreateObjectJobType<TFunctionalities...>::CompleteSuccess();
	}
	template<typename TCallable>
	inline CreateObjectJob(const TCallable& aSetupProc) noexcept
	{
		aSetupProc(std::move(myObject), std::tie(AddFunctionalityHelper<TFunctionalities>()...));

		_Impl::_CreateObjectJobType<TFunctionalities...>::CompleteSuccess();
	}

private:
	template<typename TFunctionality>
	std::decay_t<TFunctionality>& AddFunctionalityHelper()
	{
		return _Impl::_CreateObjectJobType<TFunctionalities...>::template Get<FunctionalitySystemDelegate<std::decay_t<TFunctionality>>>().AddFunctionality(myObject);
	}

	Object myObject;
};
