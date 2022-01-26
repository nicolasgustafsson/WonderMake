#pragma once

#include <array>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>

#include "WonderMakeUtility/Typedefs.h"

class Guid
	: public std::array<u8, 16>
{
public:
	static [[nodiscard]] constexpr std::optional<Guid> Parse(std::string_view aGuid)
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

	static [[nodiscard]] constexpr Guid Zero()
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

	constexpr Guid& operator=(const Guid& aRhs) noexcept
	{
		std::copy(aRhs.begin(), aRhs.end(), begin());

		return *this;
	}

	inline explicit operator std::string() const
	{
		constexpr u8 high = 0xF0;
		constexpr u8 low = 0x0F;

		std::string retVal;

		retVal.reserve(36);

		retVal.resize(32, '0');

		for (size_t i = 0; i < size(); ++i)
		{
			const u8 val = (*this)[i];
			const size_t index = i * 2;

			retVal[index]		= HexToChar((val & high) >> 4);
			retVal[index + 1]	= HexToChar(val & low);
		}

		retVal.insert(8,	1, '-');
		retVal.insert(13,	1, '-');
		retVal.insert(18,	1, '-');
		retVal.insert(23,	1, '-');

		return retVal;
	}

private:
	static constexpr u8 InvalidHex = 0xFF;

	static [[nodiscard]] constexpr char HexToChar(u8 aHex) noexcept
	{
		constexpr std::string_view digits = "0123456789abcdef";

		return digits[aHex & 0x0F];
	}
	static [[nodiscard]] constexpr u8 CharToHex(char aChar) noexcept
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

static constexpr bool operator==(const Guid& aLhs, const Guid& aRhs)
{
	return std::ranges::equal(aLhs, aRhs);
}
static constexpr bool operator!=(const Guid& aLhs, const Guid& aRhs)
{
	return !(aLhs == aRhs);
}
