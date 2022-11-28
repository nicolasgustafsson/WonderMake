#include "wondermake-engine/LoggerRemoteSystem.h"

#include "wondermake-engine/LoggerRemoteConnection.h"
#include "wondermake-engine/LoggerRemoteSocket.h"

#include "wondermake-io/IpcSystem.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/SharedReference.h"

WM_REGISTER_SYSTEM(LoggerRemoteSystem);

Result<SharedReference<LoggerRemoteSocket>, IpcAcceptor::SOpenError> LoggerRemoteSystem::OpenSocketIpc(std::string aName)
{
	auto ipcAcceptor = Get<IpcSystem>().CreateAcceptor();
	auto lrSocket = MakeSharedReference<LoggerRemoteSocket>(GetExecutor());

	auto resultOpen = lrSocket->OpenIpc(std::move(ipcAcceptor), std::move(aName));

	if (!resultOpen)
		return Err(resultOpen.Err());

	return Ok(std::move(lrSocket));
}

Result<SharedReference<LoggerRemoteConnection>, IpcConnection::SConnectionError> LoggerRemoteSystem::ConnectIpc(std::string aName)
{
	auto ipcConnection = Get<IpcSystem>().CreateConnection();
	auto lrConnection = MakeSharedReference<LoggerRemoteConnection>(GetExecutor());

	auto resultConnection = lrConnection->ConnectIpc(std::move(ipcConnection), std::move(aName));

	if (!resultConnection)
		return Err(resultConnection.Err());

	return Ok(std::move(lrConnection));
}
