#pragma once

#include "wondermake-base/Cargo.h"
#include "wondermake-base/JobBase.h"

#include "wondermake-base/FunctionalitySystem.h"
#include "wondermake-base/Object.h"

namespace _Impl
{
	template<typename... TFunctionalities>
	using _CreateObjectJobType = Job<
		NoJobInput,
		std::pair<Object, Cargo<TFunctionalities...>>,
		Policy::Set<
			PAdd<FunctionalitySystemDelegate<std::decay_t<TFunctionalities>>, PWrite>...>>;
}

// TODO(Kevin): Make require write access for inherited policies. Do this by spawning smaller jobs.
template<typename... TFunctionalities>
class CreateObjectJob final
	: public _Impl::_CreateObjectJobType<TFunctionalities...>
{
public:
	inline void Run(Promise<std::pair<Object, Cargo<TFunctionalities...>>> aPromise) noexcept
	{
		aPromise.Complete(std::make_pair(std::move(myObject), WrapCargo(AddFunctionalityHelper<TFunctionalities>()...)));
	}

private:
	template<typename TFunctionality>
	std::decay_t<TFunctionality>& AddFunctionalityHelper()
	{
		return _Impl::_CreateObjectJobType<TFunctionalities...>::template Get<FunctionalitySystemDelegate<std::decay_t<TFunctionality>>>().AddFunctionality(myObject);
	}

	Object myObject;
};
