#pragma once

#include "RestrictTypes.h"

#include <type_traits>

template<typename TType>
class UniqueValue final
	: NonCopyable
{
public:
	template<typename... TArgs> requires(std::is_constructible_v<TType, TArgs...>)
	inline constexpr UniqueValue(TArgs&&... aArgs) noexcept(std::is_nothrow_constructible_v<TType, TArgs...>)
		: myValue(std::forward<TArgs>(aArgs)...)
	{}

	template<typename T>
	inline auto operator==(UniqueValue<T>&& aRhs) const noexcept requires(TType == T)
	{
		return myValue == aRhs.myValue;
	}

	inline auto operator==(auto&& aRhs) const noexcept
	{
		return myValue == aRhs;
	}

	inline constexpr [[nodiscard]] TType& operator->() noexcept
	{
		return myValue;
	}
	inline constexpr [[nodiscard]] TType& operator*() noexcept
	{
		return myValue;
	}

	inline constexpr [[nodiscard]] const TType& operator->() const noexcept
	{
		return myValue;
	}
	inline constexpr [[nodiscard]] const TType& operator*() const noexcept
	{
		return myValue;
	}

private:
	TType myValue;

};

template<typename TType, typename... TArgs> requires(std::is_constructible_v<TType, TArgs...>)
inline constexpr [[nodiscard]] auto MakeUniqueValue(TArgs&&... aArgs) noexcept(std::is_nothrow_constructible_v<TType, TArgs...>)
{
	return UniqueValue<TType>(std::forward<TArgs>(aArgs)...);
}
