#include "pch.h"

#include "Guid/WindowsGuidGeneratorSystem.h"

#include <Combaseapi.h>

REGISTER_SYSTEM_MASKED(WindowsGuidGeneratorSystem, GuidGeneratorSystem);

std::optional<Guid> WindowsGuidGeneratorSystem::GenerateNew()
{
	static_assert(sizeof(GUID) <= sizeof(Guid));

	GUID gidReference;

	HRESULT hCreateGuid = CoCreateGuid(&gidReference);

	if (hCreateGuid != S_OK)
		return std::nullopt;

	std::array<u8, 16> retVal;

	memcpy(&retVal, &gidReference, sizeof(gidReference));

	return retVal;
}
