#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/FixedSizeString.h"
#include "wondermake-utility/Typedefs.h"

void StaticTest_FixedSizeString()
{
	static constexpr auto concatString = MakeFixedSizeString("Hello ") + MakeFixedSizeString("World");

	static_assert(concatString == "Hello World");
	static_assert(concatString.ToStringView() == "Hello World");
	static_assert(concatString.ToString() == "Hello World");
	static_assert(concatString.ToCString() == std::string_view("Hello World"));

	static_assert(MakeFixedSizeString("Hello") + '!' == "Hello!");
	static_assert('!' + MakeFixedSizeString("Hello") == "!Hello");
	static_assert(MakeFixedSizeString("Hello") + "!" == "Hello!");
	static_assert("!" + MakeFixedSizeString("Hello") == "!Hello");

	static_assert(*concatString.begin() == 'H');
	static_assert(*(concatString.end() - 1) == 'd');
	static_assert(*concatString.cbegin() == 'H');
	static_assert(*(concatString.cend() - 1) == 'd');

	static_assert(*concatString.rbegin() == 'd');
	static_assert(*(concatString.rend() - 1) == 'H');
	static_assert(*concatString.crbegin() == 'd');
	static_assert(*(concatString.crend() - 1) == 'H');

	static_assert(concatString + "!" == "Hello World!");
	static_assert("!" + concatString == "!Hello World");

	static_assert(MakeFixedSizeString("").empty());
	static_assert(!MakeFixedSizeString("Hello").empty());

	static_assert(CConstructibleFixedSizeString<char[1]>);
	static_assert(CConstructibleFixedSizeString<std::array<char, 1>>);
	static_assert(CConstructibleFixedSizeString<FixedSizeString<0>>);

	static_assert(CCanConcatWithFixedSizeString<char>);
	static_assert(CCanConcatWithFixedSizeString<char[1]>);
	static_assert(CCanConcatWithFixedSizeString<FixedSizeString<0>>);
	static_assert(!CCanConcatWithFixedSizeString<unsigned char>);
	static_assert(!CCanConcatWithFixedSizeString<i16>);
	static_assert(!CCanConcatWithFixedSizeString<u16>);
}

TEST(FixedSizeStringTests, fixedsizestring_size_is_correct_size)
{
	auto textA = MakeFixedSizeString("Hello");
	auto textB = MakeFixedSizeString("World!");

	EXPECT_EQ(textA.Size,		5);
	EXPECT_EQ(textA.size(),		5);
	EXPECT_EQ(textA.max_size(),	5);
	EXPECT_EQ(textB.Size,		6);
	EXPECT_EQ(textB.size(),		6);
	EXPECT_EQ(textB.max_size(),	6);
}

TEST(FixedSizeStringTests, fixedsizestring_can_be_compared_with_fixedsizestring)
{
	const auto textA = MakeFixedSizeString("Hello");
	const auto textB = MakeFixedSizeString("Hello");
	const auto textC = MakeFixedSizeString("World");

	EXPECT_EQ(textA, textA);
	EXPECT_EQ(textA, textB);
	EXPECT_NE(textA, textC);

	EXPECT_EQ(textB, textA);
	EXPECT_EQ(textB, textB);
	EXPECT_NE(textB, textC);

	EXPECT_NE(textC, textA);
	EXPECT_NE(textC, textB);
	EXPECT_EQ(textC, textC);
}

TEST(FixedSizeStringTests, fixedsizestring_can_be_compared_with_chararray)
{
	const auto textA = MakeFixedSizeString("Hello");
	const auto textB = MakeFixedSizeString("World");

	EXPECT_EQ(textA, "Hello");
	EXPECT_NE(textA, "World");

	EXPECT_NE(textB, "Hello");
	EXPECT_EQ(textB, "World");

	EXPECT_EQ("Hello", textA);
	EXPECT_NE("World", textA);

	EXPECT_NE("Hello", textB);
	EXPECT_EQ("World", textB);
}
