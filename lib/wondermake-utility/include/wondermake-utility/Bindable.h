#pragma once

#include "wondermake-utility/Result.h"

#include <memory>
#include <tuple>

namespace _Impl
{
	struct Bind {};
	struct WeakBind {};
}

template<typename TBinding, typename TCallable, typename... TPackedArgs>
class Bindable
{
public:
	using Binding = TBinding;

	inline constexpr Bindable(const TBinding&, TCallable&& aCallable, TPackedArgs&&... aArgs)
		: myCallable(std::forward<TCallable>(aCallable))
		, myPackedArgs(std::forward<TPackedArgs>(aArgs)...)
	{}

	template<typename... TArgs>
	inline constexpr auto operator()(TArgs&&... aArgs) &
	{
		return std::apply(myCallable, std::tuple_cat(myPackedArgs, std::forward_as_tuple(std::forward<TArgs>(aArgs)...)));
	}

	template<typename... TArgs>
	inline constexpr auto operator()(TArgs&&... aArgs) const&
	{
		return std::apply(myCallable, std::tuple_cat(myPackedArgs, std::forward_as_tuple(std::forward<TArgs>(aArgs)...)));
	}

	template<typename... TArgs>
	inline constexpr auto operator()(TArgs&&... aArgs) &&
	{
		return std::apply(std::move(myCallable), std::tuple_cat(std::move(myPackedArgs), std::forward_as_tuple(std::forward<TArgs>(aArgs)...)));
	}

	template<typename... TArgs>
	inline constexpr auto operator()(TArgs&&... aArgs) const&&
	{
		return std::apply(std::move(myCallable), std::tuple_cat(std::move(myPackedArgs), std::forward_as_tuple(std::forward<TArgs>(aArgs)...)));
	}

private:
	TCallable myCallable;
	std::tuple<TPackedArgs...> myPackedArgs;

};

template<typename TCallable, typename... TArgs>
auto Bind(TCallable&& aCallable, TArgs&&... aArgs)
{
	return Bindable(::_Impl::Bind(), std::forward<TCallable>(aCallable), std::forward<TArgs>(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, TSelf& aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable(::_Impl::Bind(), [aFunction, &self = aSelf](auto&&... aAllArgs)
		{
			return (self.*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
		}, std::move(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, TSelf* aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable(::_Impl::Bind(), [aFunction, self = aSelf](auto&&... aAllArgs)
		{
			return (self->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
		}, std::move(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, std::shared_ptr<TSelf> aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable(::_Impl::Bind(), [aFunction, self = std::move(aSelf)](auto&&... aAllArgs)
	{
		return (self.get()->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
	}, std::move(aArgs)...);
}

namespace _Impl
{
	struct WeakCallableError {};
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, std::weak_ptr<TSelf> aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable(::_Impl::WeakBind(), [aFunction, self = std::move(aSelf)](auto&&... aAllArgs)
	{
		using RetType = decltype((self.lock().get()->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...));

	 	static constexpr auto Invoke = [](TFunc aFunction, const std::weak_ptr<TSelf>& aSelf, auto&&... aAllArgs) -> Result<RetType, _Impl::WeakCallableError>
		{
			auto ptr = aSelf.lock();
	
			if (!ptr)
				return Err(::_Impl::WeakCallableError {});
	
			if constexpr (std::is_void_v<RetType>)
			{
				(ptr.get()->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);

				return Ok();
			}
			else
				return Ok((ptr.get()->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...));
		};

		return Invoke(aFunction, self, std::forward<decltype(aAllArgs)>(aAllArgs)...);
	}, std::move(aArgs)...);
}

template<typename TCallable>
concept CWeakBindedCallable = std::is_same_v<typename TCallable::Binding, ::_Impl::WeakBind>;
