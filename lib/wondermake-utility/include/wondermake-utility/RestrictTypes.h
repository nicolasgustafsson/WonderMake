#pragma once

#include <compare>
#include <ostream>
#include <type_traits>

class NonCopyable
{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;

	NonCopyable(NonCopyable&&) noexcept = default;
	NonCopyable& operator=(NonCopyable&&) noexcept = default;
};

class NonMovable
{
protected:
	NonMovable() = default;
	~NonMovable() = default;

	NonMovable(const NonMovable&) = default;
	NonMovable& operator=(const NonMovable&) = default;

	NonMovable(NonMovable&&) noexcept = delete;
	NonMovable& operator=(NonMovable&&) noexcept = delete;
};

template<typename TType, typename TKey>
class Restricted
{
public:
	template<typename... TArgs>
	inline constexpr Restricted(TArgs&&... aArgs)
		noexcept(std::is_nothrow_constructible_v<TType, decltype(std::forward<TArgs>(aArgs))...>)
		requires(std::is_constructible_v<TType, decltype(std::forward<TArgs>(aArgs))...>)
		: myValue(std::forward<TArgs>(aArgs)...)
	{}
	constexpr Restricted(const Restricted&) noexcept = default;
	constexpr Restricted(Restricted&&) noexcept = default;

	constexpr Restricted& operator=(const Restricted&) noexcept = default;
	constexpr Restricted& operator=(Restricted&&) noexcept = default;

	constexpr [[nodiscard]] auto operator<=>(const Restricted&) const noexcept = default;

	inline constexpr [[nodiscard]] TType* operator->() noexcept
	{
		return &myValue;
	}
	inline constexpr [[nodiscard]] TType& operator*() noexcept
	{
		return myValue;
	}
	
	inline constexpr [[nodiscard]] const TType* operator->() const noexcept
	{
		return &myValue;
	}
	inline constexpr [[nodiscard]] const TType& operator*() const noexcept
	{
		return myValue;
	}

	inline constexpr [[nodiscard]] const TType& Data() const& noexcept
	{
		return myValue;
	}
	inline constexpr [[nodiscard]] TType Data() && noexcept
	{
		return std::move(myValue);
	}

private:
	TType myValue;

};

namespace std
{
	template<typename TType, typename TKey>
	struct hash<Restricted<TType, TKey>>
	{
		inline constexpr [[nodiscard]] size_t operator()(const Restricted<TType, TKey>& aValue) const noexcept
		{
			hash<TType> hasher;

			return hasher(*aValue);
		}
	};
}

template<typename TType, typename TKey>
inline std::ostream& operator<<(std::ostream& aStream, const Restricted<TType, TKey>& aValue)
{
	aStream << aValue.Data();

	return aStream;
}
