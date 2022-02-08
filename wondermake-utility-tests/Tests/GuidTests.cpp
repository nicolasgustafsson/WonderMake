#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/Guid.h"

void StaticTest_Guid_Parse()
{
	static_assert(!Guid::Parse(""));
	static_assert(!Guid::Parse("Hello World!"));

	static_assert(Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
	static_assert(Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b"));
	static_assert(Guid::Parse("4AE5A5F8-4AB7-4FE7-A48F-323D89B1925B"));
	static_assert(Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B"));

	static_assert(!Guid::Parse("gae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
	static_assert(!Guid::Parse("4ae5a5f8-4gb7-4fe7-a48f-323d89b1925b"));
	static_assert(!Guid::Parse("4ae5a5f8-4ab74fe7-a48f-323d89b1925b"));

	constexpr auto guidAllHexLow = *Guid::Parse("00010203-0405-0607-0809-0a0b0c0d0e0f");
	constexpr auto guidAllHexHigh = *Guid::Parse("00102030-4050-6070-8090-a0b0c0d0e0f0");
	constexpr auto guidAllHex = *Guid::Parse("00112233-4455-6677-8899-aabbccddeeff");

	static_assert(std::ranges::equal(guidAllHexLow, std::array<u8, 16>({ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f })));
	static_assert(std::ranges::equal(guidAllHexHigh, std::array<u8, 16>({ 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 })));
	static_assert(std::ranges::equal(guidAllHex, std::array<u8, 16>({ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff })));

	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"), std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b"), std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(*Guid::Parse("4AE5A5F8-4AB7-4FE7-A48F-323D89B1925B"), std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(*Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B"), std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));

	static_assert(std::ranges::equal(Guid({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b }), std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(Guid::Zero(), std::array<u8, 16>({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })));

	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"), *Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b")));
	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"), *Guid::Parse("4AE5A5F8-4AB7-4FE7-A48F-323D89B1925B")));
	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b"), *Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B")));
}

void StaticTest_Guid_Comparison()
{
	static_assert(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b") == *Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B"));
	static_assert(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b") != *Guid::Parse("6864eff0-6303-4bca-9e13-ee1be8160b56"));
	static_assert(Guid::Zero() == *Guid::Parse("00000000000000000000000000000000"));
	static_assert(Guid::Zero() == *Guid::Parse("00000000-0000-0000-0000-000000000000"));
}

void StaticTest_Guid_Copy()
{
	constexpr auto original = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	constexpr auto copyConstructed = original;

	static_assert(original == *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
	static_assert(copyConstructed == *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
}

TEST(GuidTests, guid_assign_copy_copies_and_returns_correctly)
{
	constexpr auto original = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	auto copyAssigned = Guid::Zero();

	copyAssigned = original;

	EXPECT_EQ(original, *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
	EXPECT_EQ(copyAssigned, *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
	EXPECT_EQ(original, copyAssigned);
}

TEST(GuidTests, string_convesion_returns_a_valid_serialized_guid)
{
	constexpr auto guid = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	const auto serializedGuid = static_cast<std::string>(guid);

	EXPECT_EQ(serializedGuid, "4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");
}