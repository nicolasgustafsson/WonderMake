#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Guid/WindowsGuidGeneratorSystem.h"
#include "PlatformWindows/PlatformWindows.h"
#include "PlatformWindows/PlatformWindowsSystem.h"

constexpr Guid GuidData = std::array<u8, 16> { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

TEST_CASE("GenerateNew returns a valid Guid.", "[WindowsGuidGeneratorSystem]")
{
	class PlatformWindowsSystemMock
		: public PlatformWindowsSystem
	{
	public:
		HRESULT CoCreateGuid(GUID* pguid) override
		{
			memcpy(pguid, GuidData.data(), GuidData.size());

			return S_OK;
		}
	};

	PlatformWindowsSystemMock mock;

	WindowsGuidGeneratorSystem::InjectDependencies(std::tie(mock));

	WindowsGuidGeneratorSystem guidGenerator;

	const auto guid = guidGenerator.GenerateNew();

	REQUIRE(guid);
	REQUIRE(*guid == GuidData);
}

TEST_CASE("GenerateNew returns nullopt on error.", "[WindowsGuidGeneratorSystem]")
{
	class PlatformWindowsSystemMock
		: public PlatformWindowsSystem
	{
	public:
		HRESULT CoCreateGuid(GUID* /*pguid*/) override
		{
			return S_FALSE;
		}
	};

	PlatformWindowsSystemMock mock;

	WindowsGuidGeneratorSystem::InjectDependencies(std::tie(mock));

	WindowsGuidGeneratorSystem guidGenerator;

	const auto guid = guidGenerator.GenerateNew();

	REQUIRE(!guid);
}
