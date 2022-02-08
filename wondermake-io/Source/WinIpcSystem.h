#pragma once

#include "wondermake-utility/WinPlatform.h"

#include "WonderMakeIo/IpcSystem.h"

class WinEventSystem;
class WinPlatformSystem;

class WinIpcSystem
	: public IpcSystem
	, public SystemSub<
		Policy::Set<
			PAdd<WinEventSystem, PWrite>,
			PAdd<WinPlatformSystem, PWrite>>,
		STrait::Set<
			STPlatformInterface,
			STSingleton>>
{
public:
	std::shared_ptr<IpcAcceptor> CreateAcceptor() override;
	std::shared_ptr<IpcConnection> CreateConnection() override;
};
