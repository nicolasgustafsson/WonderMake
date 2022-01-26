#pragma once

#include <memory>
#include <tuple>

template<typename TCallable, typename... TPackedArgs>
class Bindable
{
public:
	inline constexpr Bindable(TCallable&& aCallable, TPackedArgs&&... aArgs)
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
	return Bindable(std::forward<TCallable>(aCallable), std::forward<TArgs>(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, TSelf& aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable([aFunction, &self = aSelf](auto&&... aAllArgs)
		{
			return (self.*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
		}, std::move(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, TSelf* aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable([aFunction, self = aSelf](auto&&... aAllArgs)
		{
			return (self->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
		}, std::move(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, std::shared_ptr<TSelf> aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable([aFunction, self = std::move(aSelf)](auto&&... aAllArgs)
	{
		return (self.get()->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
	}, std::move(aArgs)...);
}

template<typename TFunc, typename TSelf, typename... TArgs>
auto Bind(TFunc aFunction, std::weak_ptr<TSelf> aSelf, TArgs... aArgs) requires(std::is_member_function_pointer_v<TFunc>)
{
	return Bindable([aFunction, self = std::move(aSelf)](auto&&... aAllArgs)
	{
		auto ptr = self.lock();

		if (ptr)
			(void)(ptr.get()->*aFunction)(std::forward<decltype(aAllArgs)>(aAllArgs)...);
	}, std::move(aArgs)...);
}
