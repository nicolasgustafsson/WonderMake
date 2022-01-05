
#include "gtest/gtest.h"

#include "WonderMakeBase/Alias.h"

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

TEST(AliasTest, can_be_constructed_and_destroyed)
{
	Alias<AliasMock<int>> mock = AliasMock<float>();

	mock;
}