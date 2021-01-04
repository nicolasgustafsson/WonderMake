#pragma once

#include "Job/Job.h"

#include "Object/Object.h"

#include "Functionalities/FunctionalitySystem.h"

#include <functional>
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
	using SetupProc = std::function<void(Object&&, std::tuple<std::decay_t<TFunctionalities>&...>)>;

	inline CreateObjectJob(SetupProc aSetupProc = std::nullptr_t) noexcept
		: mySetupProc(aSetupProc)
	{}

private:
	void OnStarted() override
	{
		mySetupProc(std::move(myObject), std::tie(AddFunctionalityHelper<TFunctionalities>()...));

		_Impl::_CreateObjectJobType<TFunctionalities...>::CompleteSuccess();
	}

	template<typename TFunctionality>
	std::decay_t<TFunctionality>& AddFunctionalityHelper()
	{
		return _Impl::_CreateObjectJobType<TFunctionalities...>::template Get<FunctionalitySystemDelegate<std::decay_t<TFunctionality>>>().AddFunctionality(myObject);
	}

	Object myObject;
	SetupProc mySetupProc;
};
