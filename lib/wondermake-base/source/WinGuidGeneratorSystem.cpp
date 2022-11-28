#include "WinGuidGeneratorSystem.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"

WM_REGISTER_SYSTEM_MASKED(WinGuidGeneratorSystem, GuidGeneratorSystem);

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
