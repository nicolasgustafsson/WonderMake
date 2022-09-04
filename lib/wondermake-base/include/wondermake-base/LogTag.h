#pragma once

#include "wondermake-utility/FixedSizeString.h"

template<size_t TSize>
class LogTag final
{
private:
	static constexpr auto Prefix = MakeFixedSizeString("[");
	static constexpr auto Suffix = MakeFixedSizeString("] ");

public:
	static constexpr size_t FormattedSize = Prefix.size() + TSize + Suffix.size();

	inline constexpr LogTag(FixedSizeString<TSize> aTagName)
		: myFormattedTag(Prefix + aTagName + Suffix)
	{}

	inline constexpr const auto& GetFormattedString() const noexcept
	{
		return myFormattedTag;
	}

private:
	FixedSizeString<FormattedSize> myFormattedTag;

};

template<size_t TSize>
inline constexpr auto MakeLogTag(const char(&aCharArray)[TSize])
{
	return LogTag(MakeFixedSizeString(aCharArray));
}
