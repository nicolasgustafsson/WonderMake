#pragma once

#include "WinPlatform/WinPlatform.h"

#include "System/System.h"

class WinPlatformSystem
	: public System<>
{
public:
	virtual HRESULT CoCreateGuid(GUID* pguid);
};
