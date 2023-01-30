#pragma once

#include <array>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>

#include "wondermake-utility/FixedSizeString.h"
#include "wondermake-utility/Typedefs.h"

class Guid
	: public std::array<u8, 16>
{
public:
	using String = FixedSizeString<36>;

	static constexpr [[nodiscard]] std::optional<Guid> Parse(std::string_view aGuid)
	{
		constexpr auto translateGuid = [](auto aGuid) -> std::optional<Guid>
		{
			std::array<u8, 16> retVal;

			for (size_t i = 0; i < 16; ++i)
			{
				const u8 high = CharToHex(aGuid[i * 2]);
				const u8 low = CharToHex(aGuid[i * 2 + 1]);

				if (high == InvalidHex || low == InvalidHex)
					return std::nullopt;

				retVal[i] = (high << 4) | low;
			}

			return retVal;
		};

		if (aGuid.size() == 32)
		{
			return translateGuid(aGuid);
		}
		if (aGuid.size() == 36)
		{
			std::array<char, 32> copiedGuid;

			std::copy(aGuid.begin(),		aGuid.begin() + 8,	copiedGuid.begin());
			std::copy(aGuid.begin() + 9,	aGuid.begin() + 13,	copiedGuid.begin() + 8);
			std::copy(aGuid.begin() + 14,	aGuid.begin() + 18,	copiedGuid.begin() + 12);
			std::copy(aGuid.begin() + 19,	aGuid.begin() + 23,	copiedGuid.begin() + 16);
			std::copy(aGuid.begin() + 24,	aGuid.end(),		copiedGuid.begin() + 20);

			return translateGuid(copiedGuid);
		}
		
		return std::nullopt;
	}

	static constexpr [[nodiscard]] Guid Zero()
	{
		return std::array<u8, 16> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	}

	constexpr Guid() noexcept = delete;
	constexpr Guid(const Guid& aOther) noexcept
	{
		*this = aOther;
	}
	constexpr Guid(std::array<u8, 16>&& aData) noexcept
		: std::array<u8, 16>(aData)
	{}
	constexpr Guid(u64 aHigh, u64 aLow) noexcept
		: std::array<u8, 16>(
			{
				static_cast<u8>(aHigh >> 56),
				static_cast<u8>(aHigh >> 48),
				static_cast<u8>(aHigh >> 40),
				static_cast<u8>(aHigh >> 32),
				static_cast<u8>(aHigh >> 24),
				static_cast<u8>(aHigh >> 16),
				static_cast<u8>(aHigh >> 8),
				static_cast<u8>(aHigh),
				static_cast<u8>(aLow >> 56),
				static_cast<u8>(aLow >> 48),
				static_cast<u8>(aLow >> 40),
				static_cast<u8>(aLow >> 32),
				static_cast<u8>(aLow >> 24),
				static_cast<u8>(aLow >> 16),
				static_cast<u8>(aLow >> 8),
				static_cast<u8>(aLow)
			})
	{}

	constexpr Guid& operator=(const Guid& aRhs) noexcept
	{
		std::copy(aRhs.begin(), aRhs.end(), begin());

		return *this;
	}

	constexpr [[nodiscard]] String ToFixedSizeString() const noexcept
	{
		constexpr u8 high = 0xF0;
		constexpr u8 low = 0x0F;

		std::array<char, 37> retVal = { '\0' };

		constexpr auto format = [](const auto aBegin, const auto aEnd, auto aInsertIt)
		{
			for (auto it = aBegin; it != aEnd; ++it)
			{
				const u8 val = *it;

				(*aInsertIt++) = HexToChar((val & high) >> 4);
				(*aInsertIt++) = HexToChar(val & low);
			}

			return aInsertIt;
		};

		auto insertIt = format(begin(), begin() + 4, retVal.begin());

		insertIt = format(begin() + 4, begin() + 6, ++insertIt);

		insertIt = format(begin() + 6, begin() + 8, ++insertIt);

		insertIt = format(begin() + 8, begin() + 10, ++insertIt);

		format(begin() + 10, end(), ++insertIt);

		retVal[8]	= '-';
		retVal[13]	= '-';
		retVal[18]	= '-';
		retVal[23]	= '-';

		return retVal;
	}
	inline explicit operator std::string() const
	{
		return ToFixedSizeString().ToString();
	}

	constexpr [[nodiscard]] u64 Low() const noexcept
	{
		u64 retVal = 0;

		for (size_t i = 8; i < 16; ++i)
			retVal += u64(*(begin() + i)) << ((15 - i) * 8);

		return retVal;
	}
	constexpr [[nodiscard]] u64 High() const noexcept
	{
		u64 retVal = 0;

		for (size_t i = 0; i < 8; ++i)
			retVal += u64(*(begin() + i)) << ((7 - i) * 8);

		return retVal;
	}

	[[nodiscard]] inline constexpr size_t Hash() const noexcept
	{
		constexpr size_t componentCount = 16 / sizeof(size_t);
		size_t hash = 0;
		
		for (size_t i = 0; i < componentCount; ++i)
		{
			size_t component = 0;

			for (size_t j = 0; j < sizeof(size_t); ++j)
				component |= (static_cast<size_t>((*this)[i * sizeof(size_t) + j]) << (j * 8));

			hash += component;
		}

		return hash;
	}

private:
	static constexpr u8 InvalidHex = 0xFF;

	static constexpr [[nodiscard]] char HexToChar(u8 aHex) noexcept
	{
		constexpr std::string_view digits = "0123456789abcdef";

		return digits[aHex & 0x0F];
	}
	static constexpr [[nodiscard]] u8 CharToHex(char aChar) noexcept
	{
		if (aChar >= 'a' && aChar <= 'f')
			return static_cast<u8>(aChar - 'a') + 10;
		if (aChar >= 'A' && aChar <= 'F')
			return static_cast<u8>(aChar - 'A') + 10;
		if (aChar >= '0' && aChar <= '9')
			return static_cast<u8>(aChar - '0');

		return InvalidHex;
	}
};

static constexpr [[nodiscard]] bool operator==(const Guid& aLhs, const Guid& aRhs)
{
	return std::ranges::equal(aLhs, aRhs);
}
static constexpr [[nodiscard]] bool operator!=(const Guid& aLhs, const Guid& aRhs)
{
	return !(aLhs == aRhs);
}

namespace std
{
	template <>
	struct hash<Guid>
	{
		inline [[nodiscard]] size_t operator()(const Guid aGuid) const noexcept
		{
			return aGuid.Hash();
		}
	};
}

inline std::ostream& operator<<(std::ostream& aStream, const Guid aGuid)
{
	aStream << aGuid.ToFixedSizeString();

	return aStream;
}
