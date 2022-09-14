#pragma once

#include "wondermake-utility/WinPlatform.h"

#include "wondermake-io/IpcSystem.h"

class WinEventSystem;
class WinPlatformSystem;

class WinIpcSystem
	: public IpcSystem
	, public SystemSub<
		Policy::Set<
			PAdd<WinEventSystem, PWrite>,
			PAdd<WinPlatformSystem, PWrite>>,
		STrait::Set<
			STFoundational,
			STSingleton>>
{
public:
	SharedReference<IpcAcceptor>	CreateAcceptor() override;
	SharedReference<IpcConnection>	CreateConnection() override;
};
