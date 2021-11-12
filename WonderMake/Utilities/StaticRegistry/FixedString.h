#pragma once
template<unsigned N>
struct SFixedString
{
	char buf[N + 1]{};
	constexpr SFixedString(char const* s)
	{
		for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
	}

	constexpr SFixedString()
	{
		//for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
	}
	constexpr operator char const* () const { return buf; }
};
template<unsigned N> SFixedString(char const (&)[N])->SFixedString<N - 1>;