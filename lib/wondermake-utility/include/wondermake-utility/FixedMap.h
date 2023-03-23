#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

template<typename TKey, typename... TValues>
class FixedMap
{
public:
	static constexpr size_t Size = sizeof...(TValues);

	inline constexpr FixedMap(std::pair<TKey, TValues>... aValues) noexcept
		: myKeys({ std::move(aValues.first)... })
		, myValues({ std::move(aValues.second)... })
	{}

	template<typename TCallable, typename... TArgs>
	inline constexpr bool Visit(const TKey& aKey, TCallable&& aVisitProc, TArgs&&... aArgs) noexcept
	{
		auto it = std::find(myKeys.begin(), myKeys.end(), aKey);

		if (it == myKeys.end())
			return false;

		return VisitKey<0>(aKey, std::forward<TCallable>(aVisitProc), std::forward<TArgs>(aArgs)...);
	}
	template<typename TCallable, typename... TArgs>
	inline constexpr bool Visit(const TKey& aKey, TCallable&& aVisitProc, TArgs&&... aArgs) const noexcept
	{
		auto it = std::find(myKeys.begin(), myKeys.end(), aKey);

		if (it == myKeys.end())
			return false;

		return VisitKey<0>(aKey, std::forward<TCallable>(aVisitProc), std::forward<TArgs>(aArgs)...);
	}

private:
	template<size_t TIndex, typename TCallable, typename... TArgs>
	inline constexpr bool VisitKey(const TKey&, TCallable&&, TArgs&&...) const noexcept
		requires(TIndex >= Size)
	{
		return false;
	}
	template<size_t TIndex, typename TCallable, typename... TArgs>
	inline constexpr bool VisitKey(const TKey& aKey, TCallable&& aVisitProc, TArgs&&... aArgs) noexcept
		requires(TIndex < Size)
	{
		if (myKeys[TIndex] == aKey)
		{
			std::forward<TCallable>(aVisitProc)(std::get<TIndex>(myValues), std::forward<TArgs>(aArgs)...);

			return true;
		}

		return VisitKey<TIndex + 1>(aKey, std::forward<TCallable>(aVisitProc), std::forward<TArgs>(aArgs)...);
	}
	template<size_t TIndex, typename TCallable, typename... TArgs>
	inline constexpr bool VisitKey(const TKey& aKey, TCallable&& aVisitProc, TArgs&&... aArgs) const noexcept
		requires(TIndex < Size)
	{
		if (myKeys[TIndex] == aKey)
		{
			std::forward<TCallable>(aVisitProc)(std::get<TIndex>(myValues), std::forward<TArgs>(aArgs)...);

			return true;
		}

		return VisitKey<TIndex + 1>(aKey, std::forward<TCallable>(aVisitProc), std::forward<TArgs>(aArgs)...);
	}

	std::array<TKey, Size>	myKeys;
	std::tuple<TValues...>	myValues;

};

template<typename TKey, typename... TValues>
inline constexpr auto MakeFixedMap(std::pair<TKey, TValues>... aValues) noexcept
{
	return FixedMap<std::decay_t<TKey>, std::decay_t<TValues>...>(std::move(aValues)...);
}
