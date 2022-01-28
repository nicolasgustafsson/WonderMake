#pragma once

#include "WonderMakeIo/IpcAcceptor.h"
#include "WonderMakeIo/Socket.h"

#include "WonderMakeBase/Logger.h"
#include "WonderMakeBase/System.h"

#include <memory>
#include <unordered_set>

class IpcSystem;

class LoggerRemoteSocketSystem
	: public System<
		Policy::Set<
			PAdd<IpcSystem, PWrite>>,
		STrait::Set<
			STLogger,
			STSingleton>>
	, public std::enable_shared_from_this<LoggerRemoteSocketSystem>
{
public:
	Result<IpcAcceptor::EOpenError> OpenIpc(std::string aIpcName);

private:
	void OnConnection(std::shared_ptr<Socket>&& aConnection);

	void OnIpcClosed(Result<IpcAcceptor::ECloseReason> aResult);

	void OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<Socket::EReadError, std::vector<u8>>&& aResult);
	void OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::ECloseError, Socket::ECloseReason> aResult);

	std::shared_ptr<IpcAcceptor> myAcceptor;
	std::unordered_set<std::shared_ptr<Socket>> myConnections;
	std::vector<u8> myBuffer;

};
