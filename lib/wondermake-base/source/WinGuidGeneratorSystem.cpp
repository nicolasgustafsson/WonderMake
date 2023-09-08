#include "WinGuidGeneratorSystem.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"

WM_REGISTER_SYSTEM_WITH_ID_MASKED(WinGuidGeneratorSystem, GuidGeneratorSystem, "459b919a-5617-4dc1-ada9-1b8f2f58154a");

std::optional<Guid> WinGuidGeneratorSystem::GenerateNew()
{
	static_assert(sizeof(GUID) <= sizeof(Guid));

	GUID gidReference;

	HRESULT hCreateGuid = Get<WinPlatformSystem>().CoCreateGuid(&gidReference);

	if (hCreateGuid != S_OK)
		return std::nullopt;

	std::array<u8, 16> retVal;

	memcpy(&retVal, &gidReference, sizeof(gidReference));

	return retVal;
}
