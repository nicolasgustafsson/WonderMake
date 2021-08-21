#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Utilities/Guid.h"

void StaticTest_Guid()
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

	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"),	std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b"),		std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(*Guid::Parse("4AE5A5F8-4AB7-4FE7-A48F-323D89B1925B"),	std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(*Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B"),		std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));

	static_assert(std::ranges::equal(Guid({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b }), std::array<u8, 16>({ 0x4a, 0xe5, 0xa5, 0xf8, 0x4a, 0xb7, 0x4f, 0xe7, 0xa4, 0x8f, 0x32, 0x3d, 0x89, 0xb1, 0x92, 0x5b })));
	static_assert(std::ranges::equal(Guid::Zero(), std::array<u8, 16>({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })));

	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"),	*Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b")));
	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b"),	*Guid::Parse("4AE5A5F8-4AB7-4FE7-A48F-323D89B1925B")));
	static_assert(std::ranges::equal(*Guid::Parse("4ae5a5f84ab74fe7a48f323d89b1925b"),		*Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B")));

	static_assert(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b") == *Guid::Parse("4AE5A5F84AB74FE7A48F323D89B1925B"));
	static_assert(*Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b") != *Guid::Parse("6864eff0-6303-4bca-9e13-ee1be8160b56"));
	static_assert(Guid::Zero() == *Guid::Parse("00000000000000000000000000000000"));
	static_assert(Guid::Zero() == *Guid::Parse("00000000-0000-0000-0000-000000000000"));
}

TEST_CASE("String conversion returns a valid serialized guid.", "[Guid]")
{
	constexpr auto guid = *Guid::Parse("4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");

	const auto serializedGuid = static_cast<std::string>(guid);

	REQUIRE(serializedGuid == "4ae5a5f8-4ab7-4fe7-a48f-323d89b1925b");
}