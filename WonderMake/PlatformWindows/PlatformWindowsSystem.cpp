#include "pch.h"

#include "PlatformWindows/PlatformWindowsSystem.h"

REGISTER_SYSTEM(PlatformWindowsSystem);

HRESULT PlatformWindowsSystem::CoCreateGuid(GUID* pguid)
{
	return ::CoCreateGuid(pguid);
}
