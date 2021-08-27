#include "pch.h"

#include "WinPlatform/WinPlatformSystem.h"

REGISTER_SYSTEM(WinPlatformSystem);

HRESULT WinPlatformSystem::CoCreateGuid(GUID* pguid)
{
	return ::CoCreateGuid(pguid);
}
