#pragma once

#include <optional>
#include <type_traits>

namespace Utility
{
	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(const TCallable& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)aCallable(std::forward<TArgs>(aArgs)...);
	}
	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(TCallable& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)aCallable(std::forward<TArgs>(aArgs)...);
	}

	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(std::optional<TCallable>& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)(*aCallable)(std::forward<TArgs>(aArgs)...);
	}
	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(const std::optional<TCallable>& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)(*aCallable)(std::forward<TArgs>(aArgs)...);
	}
	template<typename TCallable, typename... TArgs> requires std::is_invocable_v<TCallable, TArgs...>
	static void Invoke(std::optional<TCallable>&& aCallable, TArgs&&... aArgs)
	{
		if (aCallable)
			(void)std::move(*aCallable)(std::forward<TArgs>(aArgs)...);
	}
}