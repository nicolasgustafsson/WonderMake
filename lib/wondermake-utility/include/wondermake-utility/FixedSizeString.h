#pragma once

#include <array>
#include <ostream>
#include <string>
#include <string_view>

template<size_t TSize>
class FixedSizeString
{
public:
	static constexpr size_t Size = TSize;

	inline constexpr FixedSizeString(const std::array<char, Size + 1> aCharArray) noexcept
		: myData({ '\0' })
	{
		for (size_t i = 0; i < Size; ++i)
			myData[i] = aCharArray[i];
	}
	inline constexpr FixedSizeString(const char(&aCharArray)[Size + 1]) noexcept
		: myData({ '\0' })
	{
		for (size_t i = 0; i < Size; ++i)
			myData[i] = aCharArray[i];
	}
	constexpr FixedSizeString(const FixedSizeString&) noexcept = default;

	constexpr [[nodiscard]] bool operator==(const FixedSizeString&) const noexcept = default;
	constexpr [[nodiscard]] bool operator!=(const FixedSizeString&) const noexcept = default;
	
	inline constexpr [[nodiscard]] char operator[](const size_t aIndex) const noexcept
	{
		return myData[aIndex];
	}

	inline constexpr [[nodiscard]] std::string_view ToStringView() const noexcept
	{
		return { begin(), end() };
	}
	inline constexpr [[nodiscard]] std::string ToString() const
	{
		return { begin(), end() };
	}
	inline constexpr [[nodiscard]] const char* ToCString() const noexcept
	{
		return myData.data();
	}
	inline constexpr [[nodiscard]] const std::array<char, Size + 1>& ToArray() const noexcept
	{
		return myData;
	}

	inline constexpr [[nodiscard]] auto begin() const noexcept
	{
		return myData.begin();
	}
	inline constexpr [[nodiscard]] auto end() const noexcept
	{
		return --myData.end();
	}

	inline constexpr [[nodiscard]] auto rbegin() const noexcept
	{
		return ++myData.rbegin();
	}
	inline constexpr [[nodiscard]] auto rend() const noexcept
	{
		return myData.rend();
	}

	inline constexpr [[nodiscard]] auto cbegin() const noexcept
	{
		return myData.cbegin();
	}
	inline constexpr [[nodiscard]] auto cend() const noexcept
	{
		return --myData.cend();
	}

	inline constexpr [[nodiscard]] auto crbegin() const noexcept
	{
		return ++myData.crbegin();
	}
	inline constexpr [[nodiscard]] auto crend() const noexcept
	{
		return myData.crend();
	}

	inline constexpr [[nodiscard]] size_t size() const noexcept
	{
		return Size;
	}
	inline constexpr [[nodiscard]] size_t max_size() const noexcept
	{
		return Size;
	}

	inline constexpr [[nodiscard]] bool empty() const noexcept
	{
		return Size == 0;
	}

private:
	std::array<char, Size + 1> myData;

};

template<size_t TSize>
constexpr [[nodiscard]] auto MakeFixedSizeString(const std::array<char, TSize> aCharArray) noexcept
{
	return FixedSizeString<TSize - 1>(aCharArray);
}
template<size_t TSize>
constexpr [[nodiscard]] auto MakeFixedSizeString(const char(&aCharArray)[TSize]) noexcept
{
	return FixedSizeString<TSize - 1>(aCharArray);
}
template<size_t TSize>
constexpr [[nodiscard]] auto MakeFixedSizeString(const FixedSizeString<TSize>& aString) noexcept
{
	return aString;
}

namespace _Impl
{
	template <typename>
	struct IsFixedSizeString
		: public std::false_type
	{};
	template <template<size_t> typename TType, size_t TSize>
	struct IsFixedSizeString<TType<TSize>>
		: public std::is_same<std::decay_t<TType<TSize>>, FixedSizeString<TSize>>
	{};
}

template<typename TType>
concept CIsFixedSizeString = _Impl::IsFixedSizeString<TType>::value;
template<typename TType>
concept CConvertibleToFixedSizeString = requires(const TType& aValue)
{
	{ aValue.ToFixedSizeString() } -> CIsFixedSizeString<>;
};
template<typename TType>
concept CConstructibleFixedSizeString = requires(const TType& aValue)
{
	{ MakeFixedSizeString(aValue) } -> CIsFixedSizeString<>;
};

template<size_t TSize>
inline constexpr [[nodiscard]] bool operator==(const FixedSizeString<TSize>& aLhs, const char(&aRhs)[TSize + 1]) noexcept
{
	for (size_t i = 0; i < TSize; ++i)
		if (aLhs[i] != aRhs[i])
			return false;

	return true;
}
template<size_t TSize>
inline constexpr [[nodiscard]] bool operator!=(const FixedSizeString<TSize>& aLhs, const char(&aRhs)[TSize + 1]) noexcept
{
	return !(aLhs == aRhs);
}

template<size_t TSize>
inline constexpr [[nodiscard]] bool operator==(const char(&aLhs)[TSize + 1], const FixedSizeString<TSize>& aRhs) noexcept
{
	return aRhs == aLhs;
}
template<size_t TSize>
inline constexpr [[nodiscard]] bool operator!=(const char(&aLhs)[TSize + 1], const FixedSizeString<TSize>& aRhs) noexcept
{
	return aRhs != aLhs;
}

template<size_t TLhsSize, size_t TRhsSize>
inline constexpr [[nodiscard]] FixedSizeString<TLhsSize + TRhsSize> operator+(const FixedSizeString<TLhsSize>& aLhs, const FixedSizeString<TRhsSize>& aRhs) noexcept
{
	std::array<char, TLhsSize + TRhsSize + 1> retVal = { '\0' };

	for (size_t i = 0; i < TLhsSize; ++i)
		retVal[i] = aLhs[i];
	for (size_t i = 0; i < TRhsSize; ++i)
		retVal[TLhsSize + i] = aRhs[i];

	return retVal;
}

template<size_t TLhsSize, CConstructibleFixedSizeString TRhsType>
inline constexpr [[nodiscard]] auto operator+(const FixedSizeString<TLhsSize>& aLhs, const TRhsType& aRhs) noexcept
{
	return aLhs + MakeFixedSizeString(aRhs);
}
template<CConstructibleFixedSizeString TLhsType, size_t TRhsSize>
inline constexpr [[nodiscard]] auto operator+(const TLhsType& aLhs, const FixedSizeString<TRhsSize>& aRhs) noexcept
{
	return MakeFixedSizeString(aLhs) + aRhs;
}

template<size_t TSize>
inline constexpr [[nodiscard]] FixedSizeString<TSize + 1> operator+(const FixedSizeString<TSize>& aLhs, const char aRhs) noexcept
{
	std::array<char, TSize + 2> retVal = { '\0' };

	for (size_t i = 0; i < TSize; ++i)
		retVal[i] = aLhs[i];
	retVal[TSize] = aRhs;

	return retVal;
}
template<size_t TSize>
inline constexpr [[nodiscard]] FixedSizeString<TSize + 1> operator+(const char aLhs, const FixedSizeString<TSize>& aRhs) noexcept
{
	std::array<char, TSize + 2> retVal = { '\0' };

	retVal[0] = aLhs;
	for (size_t i = 0; i < TSize; ++i)
		retVal[i + 1] = aRhs[i];

	return retVal;
}

template<typename TType>
concept CCanConcatWithFixedSizeString = requires(const TType& aValue, const FixedSizeString<0>& aString)
{
	{ aString + aValue } -> CIsFixedSizeString<>;
	{ aValue + aString } -> CIsFixedSizeString<>;
} && (!std::is_arithmetic_v<std::decay_t<TType>> || std::is_same_v<std::decay_t<TType>, char>);

template<size_t TSize>
inline std::ostream& operator<<(std::ostream& aStream, const FixedSizeString<TSize>& aString)
{
	aStream << aString.ToStringView();

	return aStream;
}
