#pragma once

#include "PlatformWindows/PlatformWindows.h"

#include "System/System.h"

class PlatformWindowsSystem
	: public System<>
{
public:
	virtual HRESULT CoCreateGuid(GUID* pguid);
};
