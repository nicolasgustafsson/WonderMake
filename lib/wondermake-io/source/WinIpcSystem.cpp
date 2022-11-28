#include "WinIpcSystem.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"
#include "wondermake-base/WinEventSystem.h"

#include "WinIpcAcceptor.h"
#include "WinIpcConnection.h"

WM_REGISTER_SYSTEM_MASKED(WinIpcSystem, IpcSystem);

SharedReference<IpcAcceptor> WinIpcSystem::CreateAcceptor()
{
	return MakeSharedReference<WinIpcAcceptor>(GetExecutor(), Get<WinEventSystem>(), Get<WinPlatformSystem>());
}

SharedReference<IpcConnection> WinIpcSystem::CreateConnection()
{
	return MakeSharedReference<WinIpcConnection>(GetExecutor(), Get<WinEventSystem>(), Get<WinPlatformSystem>());
}
