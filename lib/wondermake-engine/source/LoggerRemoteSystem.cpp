#include "wondermake-engine/LoggerRemoteSystem.h"

#include "wondermake-engine/LoggerRemoteConnection.h"
#include "wondermake-engine/LoggerRemoteSocket.h"

#include "wondermake-io/IpcSystem.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/SharedReference.h"

REGISTER_SYSTEM(LoggerRemoteSystem);

Result<IpcAcceptor::EOpenError, std::shared_ptr<LoggerRemoteSocket>> LoggerRemoteSystem::OpenSocketIpc(std::string aName)
{
	auto ipcAcceptor = Get<IpcSystem>().CreateAcceptor();

	auto ipcAcceptorRef = SharedReference<IpcAcceptor>::FromPointer(std::move(ipcAcceptor));

	if (!ipcAcceptorRef)
		return IpcAcceptor::EOpenError::OutOfMemory;

	auto lrSocket = std::make_shared<LoggerRemoteSocket>();

	if (!lrSocket)
		return IpcAcceptor::EOpenError::OutOfMemory;

	auto resultOpen = lrSocket->OpenIpc(std::move(ipcAcceptorRef), std::move(aName));

	if (!resultOpen)
		return static_cast<IpcAcceptor::EOpenError>(resultOpen);

	return lrSocket;
}

Result<IpcConnection::ConnectionError, std::shared_ptr<LoggerRemoteConnection>> LoggerRemoteSystem::ConnectIpc(std::string aName)
{
	auto ipcConnection = Get<IpcSystem>().CreateConnection();

	auto ipcConnectionRef = SharedReference<IpcConnection>::FromPointer(std::move(ipcConnection));

	if (!ipcConnectionRef)
		return IpcConnection::ConnectionError::OutOfMemory;

	auto lrConnection = std::make_shared<LoggerRemoteConnection>();

	if (!lrConnection)
		return IpcConnection::ConnectionError::OutOfMemory;

	auto resultConnection = lrConnection->ConnectIpc(std::move(ipcConnectionRef), std::move(aName));

	if (!resultConnection)
		return static_cast<IpcConnection::ConnectionError>(resultConnection);

	return lrConnection;
}
