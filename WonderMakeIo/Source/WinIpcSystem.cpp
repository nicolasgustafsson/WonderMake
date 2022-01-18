#include "WinIpcSystem.h"

#include "WonderMakeEngine/WinPlatformSystem.h"
#include "WonderMakeEngine/WinEventSystem.h"

#include "WinIpcAcceptor.h"
#include "WinIpcConnection.h"

std::shared_ptr<IpcAcceptor> WinIpcSystem::CreateAcceptor()
{
	return std::make_shared<WinIpcAcceptor>(Get<WinEventSystem>(), Get<WinPlatformSystem>());
}

std::shared_ptr<IpcConnection> WinIpcSystem::CreateConnection()
{
	return std::make_shared<WinIpcConnection>(Get<WinEventSystem>(), Get<WinPlatformSystem>());
}
