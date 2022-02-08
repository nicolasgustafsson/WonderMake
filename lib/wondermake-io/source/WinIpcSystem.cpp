#include "WinIpcSystem.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"
#include "wondermake-base/WinEventSystem.h"

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
