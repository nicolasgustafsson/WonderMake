#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Utilities/Alias.h"

#include <string>

template<typename TType>
struct AliasMock
{
	char Data[8];
};

void StaticTest_Alias()
{
	static_assert(Alias<AliasMock<int>>::IsValidAlias<AliasMock<int>>);
	static_assert(Alias<AliasMock<int>>::IsValidAlias<AliasMock<double>>);
	static_assert(Alias<AliasMock<int>>::IsValidAlias<AliasMock<std::string>>);
	static_assert(Alias<AliasMock<double>>::IsValidAlias<AliasMock<int>>);
	static_assert(Alias<AliasMock<double>>::IsValidAlias<AliasMock<double>>);
	static_assert(Alias<AliasMock<double>>::IsValidAlias<AliasMock<std::string>>);
	static_assert(Alias<AliasMock<std::string>>::IsValidAlias<AliasMock<int>>);
	static_assert(Alias<AliasMock<std::string>>::IsValidAlias<AliasMock<double>>);
	static_assert(Alias<AliasMock<std::string>>::IsValidAlias<AliasMock<std::string>>);

	static_assert(!Alias<AliasMock<int>>::IsValidAlias<int>);
	static_assert(!Alias<AliasMock<int>>::IsValidAlias<double>);
	static_assert(!Alias<AliasMock<int>>::IsValidAlias<std::string>);
}

TEST_CASE("Alias can be constructed and destroyed", "[Alias]")
{
	Alias<AliasMock<int>> mock = AliasMock<float>();

	mock;
}