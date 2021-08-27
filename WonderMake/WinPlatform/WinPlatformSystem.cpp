#include "pch.h"

#include "WinPlatform/WinPlatformSystem.h"

REGISTER_SYSTEM(WinPlatformSystem);

HRESULT WinPlatformSystem::CoCreateGuid(GUID* pguid)
{
	return ::CoCreateGuid(pguid);
}

HRESULT WinPlatformSystem::SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath)
{
	return ::SHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
}

void WinPlatformSystem::CoTaskMemFree(LPVOID pv)
{
	::CoTaskMemFree(pv);
}
