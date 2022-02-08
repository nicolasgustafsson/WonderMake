#include "WinIpcSystem.h"

#include "WonderMakeBase/SystemGlobal.h"
#include "WonderMakeBase/WinPlatformSystem.h"
#include "WonderMakeBase/WinEventSystem.h"

#include "WinIpcAcceptor.h"
#include "WinIpcConnection.h"

REGISTER_SYSTEM_MASKED(WinIpcSystem, IpcSystem);

std::shared_ptr<IpcAcceptor> WinIpcSystem::CreateAcceptor()
{
	return std::make_shared<WinIpcAcceptor>(Get<WinEventSystem>(), Get<WinPlatformSystem>());
}

std::shared_ptr<IpcConnection> WinIpcSystem::CreateConnection()
{
	return std::make_shared<WinIpcConnection>(Get<WinEventSystem>(), Get<WinPlatformSystem>());
}
