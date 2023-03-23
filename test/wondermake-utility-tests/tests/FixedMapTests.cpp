#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/FixedMap.h"
#include "wondermake-utility/Typedefs.h"

void StaticTest_FixedMap()
{
	static constexpr auto map1 = MakeFixedMap<u32>();
	static constexpr auto map2 = MakeFixedMap(std::make_pair(1, "Hello"), std::make_pair(2, "World"));
	static constexpr auto map3 = MakeFixedMap(std::make_pair(1, "Kalle"), std::make_pair(2, 1234), std::make_pair(-1, "Hello World"));

	static_assert(map1.Size == 0);
	static_assert(map2.Size == 2);
	static_assert(map3.Size == 3);

	static constexpr auto noOp = [](auto) {};

	static_assert(!map1.Visit(	0,	noOp));
	static_assert(!map2.Visit(	0,	noOp));
	static_assert(map2.Visit(	1,	noOp));
	static_assert(map2.Visit(	2,	noOp));
	static_assert(!map2.Visit(	3,	noOp));
}

TEST(FixedMapTests, visit_passes_correct_value)
{
	static constexpr auto map = MakeFixedMap(std::make_pair(1, "Hello"), std::make_pair(2, "World"));

	map.Visit(2, [](auto aValue)
		{
			EXPECT_EQ(aValue, std::string_view("World"));
		});
}

TEST(FixedMapTests, visit_passes_correct_type)
{
	static constexpr auto map = MakeFixedMap(std::make_pair(1, std::string_view("Kalle")), std::make_pair(2, 1234), std::make_pair(-1, "Hello World"));

	map.Visit(1, [](auto aValue)
		{
			static constexpr bool isStringView = std::is_same_v<std::decay_t<decltype(aValue)>, std::string_view>;

			EXPECT_TRUE(isStringView);
		});
}
