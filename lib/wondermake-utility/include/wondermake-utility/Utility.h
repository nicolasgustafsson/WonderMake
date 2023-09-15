#pragma once

#include <optional>
#include <type_traits>

namespace Utility
{
	template<typename T>
	[[nodiscard]] inline std::string_view GetTypeName() noexcept
	{
		static constexpr std::string_view prefixClass	= "class ";
		static constexpr std::string_view prefixStruct	= "struct ";

		// std::type_info::name makes no guarantees of the format
		std::string_view typeName = typeid(T).name();

		if (typeName.starts_with(prefixClass))
			return typeName.substr(prefixClass.size());

		if (typeName.starts_with(prefixStruct))
			return typeName.substr(prefixStruct.size());

		return typeName;
	}

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