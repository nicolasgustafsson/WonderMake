#include "pch.h"
#include "Guid.h"

// TODO(Kevin): We need to handle platform specific code in a better manner. ASAP
#include <Combaseapi.h>

[[nodiscard]] std::optional<Guid> Guid::Random()
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
