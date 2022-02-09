#pragma once

template<size_t N>
struct SFixedString
{
	char buf[N + 1]{};
	constexpr SFixedString(char const* s)
	{
		for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
	}

	constexpr SFixedString(const SFixedString& aOther)
	{
		for (unsigned i = 0; i != N; ++i) buf[i] = aOther.buf[i];
	}

	//constexpr SFixedString()
	//{
	//	//for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
	//}
	constexpr operator char const* () const { return buf; }
};
template<size_t N> SFixedString(char const (&)[N])->SFixedString<N - 1>;

template<SFixedString Str>
auto consteval MakeFixedString(){return Str;}


template<SFixedString Name>
struct NameObject
{
	static std::string_view GetName()
	{
		return std::string_view(Name);
	}
};