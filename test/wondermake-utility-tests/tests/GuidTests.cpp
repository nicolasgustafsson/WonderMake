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

void StaticTest_Guid_StringConvertible()
{
	static_assert(CConvertibleToFixedSizeString<Guid>);
}

void StaticTest_Guid_Comparison()
{
	static_assert(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b") == *Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B"));
	static_assert(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b") != *Guid::Parse("6864eff0-6303-4bca-9e13-ee1be8160b56"));
	static_assert(Guid::Zero() == *Guid::Parse("00000000000000000000000000000000"));
	static_assert(Guid::Zero() == *Guid::Parse("00000000-0000-0000-0000-000000000000"));

	static_assert(Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b")->ToFixedSizeString() == "4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");
	static_assert(Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b")->ToFixedSizeString() != "6864eff0-6303-4bca-9e13-ee1be8160b56");
}

void StaticTest_Guid_Copy()
{
	constexpr auto original = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	constexpr auto copyConstructed = original;

	static_assert(original == *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
	static_assert(copyConstructed == *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"));
}

void StaticTest_Guid_HighLow()
{
	constexpr auto guid = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	static_assert(guid.Low()	== 0xa48f323d89b1925b);
	static_assert(guid.High()	== 0x4ae5a5f84ab74fe7);

	constexpr auto constructed = Guid(guid.High(), guid.Low());

	static_assert(constructed == guid);
}

// Since the hash output is not defined, there's nothing to test except if the same input always gives the same output.
// So if something weird is going on, hope one of these ten cases will catch it.
void StaticTest_Guid_Hash()
{
	constexpr auto guidA1 = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");
	constexpr auto guidA2 = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	constexpr auto guidB1 = *Guid::Parse("f58bf19d-fc32-49d2-b94a-040e573b5482");
	constexpr auto guidB2 = *Guid::Parse("f58bf19d-fc32-49d2-b94a-040e573b5482");

	constexpr auto guidC1 = *Guid::Parse("55131962-d3cc-4867-af7c-d6cf09277ea1");
	constexpr auto guidC2 = *Guid::Parse("55131962-d3cc-4867-af7c-d6cf09277ea1");

	constexpr auto guidD1 = *Guid::Parse("984da2e7-796f-4401-9584-23f719fd0fb6");
	constexpr auto guidD2 = *Guid::Parse("984da2e7-796f-4401-9584-23f719fd0fb6");
	
	constexpr auto guidE1 = *Guid::Parse("244e307c-34a6-40b7-8fa4-3992a23957a3");
	constexpr auto guidE2 = *Guid::Parse("244e307c-34a6-40b7-8fa4-3992a23957a3");
	
	constexpr auto guidF1 = *Guid::Parse("bae1d344-020a-42dd-b1a7-10db571fa3a8");
	constexpr auto guidF2 = *Guid::Parse("bae1d344-020a-42dd-b1a7-10db571fa3a8");
	
	constexpr auto guidG1 = *Guid::Parse("e6139ff2-ce28-42d2-b4fb-563626326d7e");
	constexpr auto guidG2 = *Guid::Parse("e6139ff2-ce28-42d2-b4fb-563626326d7e");
	
	constexpr auto guidH1 = *Guid::Parse("117e3f0e-fb09-40a7-a430-ceaf3410c877");
	constexpr auto guidH2 = *Guid::Parse("117e3f0e-fb09-40a7-a430-ceaf3410c877");
	
	constexpr auto guidI1 = *Guid::Parse("e4873700-ef57-43a1-9962-161f1dce480e");
	constexpr auto guidI2 = *Guid::Parse("e4873700-ef57-43a1-9962-161f1dce480e");
	
	constexpr auto guidJ1 = *Guid::Parse("ededf910-e9b2-4bff-b775-9665c4b812ce");
	constexpr auto guidJ2 = *Guid::Parse("ededf910-e9b2-4bff-b775-9665c4b812ce");

	static_assert(guidA1.Hash() == guidA2.Hash());
	static_assert(guidB1.Hash() == guidB2.Hash());
	static_assert(guidC1.Hash() == guidC2.Hash());
	static_assert(guidD1.Hash() == guidD2.Hash());
	static_assert(guidE1.Hash() == guidE2.Hash());
	static_assert(guidF1.Hash() == guidF2.Hash());
	static_assert(guidG1.Hash() == guidG2.Hash());
	static_assert(guidH1.Hash() == guidH2.Hash());
	static_assert(guidI1.Hash() == guidI2.Hash());
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