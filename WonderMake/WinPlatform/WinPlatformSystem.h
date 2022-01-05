#pragma once

#include "WinPlatform/WinPlatform.h"

#include "WonderMakeEngine/System.h"

class WinPlatformSystem
	: public System<>
{
public:
	virtual HRESULT CoCreateGuid(GUID* pguid);

	virtual HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);
	virtual void CoTaskMemFree(LPVOID pv);
};
