#include "wondermake-engine/LoggerRemoteSystem.h"

#include "wondermake-engine/LoggerRemoteConnection.h"
#include "wondermake-engine/LoggerRemoteSocket.h"

#include "wondermake-io/IpcSystem.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/SharedReference.h"

REGISTER_SYSTEM(LoggerRemoteSystem);

Result<std::shared_ptr<LoggerRemoteSocket>, IpcAcceptor::SOpenError> LoggerRemoteSystem::OpenSocketIpc(std::string aName)
{
	auto ipcAcceptor = Get<IpcSystem>().CreateAcceptor();

	auto ipcAcceptorRef = SharedReference<IpcAcceptor>::FromPointer(std::move(ipcAcceptor));

	if (!ipcAcceptorRef)
		return Err(IpcAcceptor::SOpenError{ IpcAcceptor::EOpenError::OutOfMemory });

	auto lrSocket = std::make_shared<LoggerRemoteSocket>();

	if (!lrSocket)
		return Err(IpcAcceptor::SOpenError{ IpcAcceptor::EOpenError::OutOfMemory });

	auto resultOpen = lrSocket->OpenIpc(std::move(ipcAcceptorRef).Unwrap(), std::move(aName));

	if (!resultOpen)
		return Err(std::move(resultOpen).Err());

	return Ok(lrSocket);
}

Result<std::shared_ptr<LoggerRemoteConnection>, IpcConnection::SConnectionError> LoggerRemoteSystem::ConnectIpc(std::string aName)
{
	auto ipcConnection = Get<IpcSystem>().CreateConnection();

	auto ipcConnectionRef = SharedReference<IpcConnection>::FromPointer(std::move(ipcConnection));

	if (!ipcConnectionRef)
		return Err(IpcConnection::SConnectionError{ IpcConnection::EConnectionError::OutOfMemory });

	auto lrConnection = std::make_shared<LoggerRemoteConnection>();

	if (!lrConnection)
		return Err(IpcConnection::SConnectionError{ IpcConnection::EConnectionError::OutOfMemory });

	auto resultConnection = lrConnection->ConnectIpc(std::move(ipcConnectionRef).Unwrap(), std::move(aName));

	if (!resultConnection)
		return Err(std::move(resultConnection).Err());

	return Ok(lrConnection);
}
