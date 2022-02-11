#pragma once

#include "wondermake-io/IpcAcceptor.h"
#include "wondermake-io/Socket.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include <memory>
#include <unordered_map>

namespace ProtoLoggerRemote
{
	class LogLine;
}

class IpcSystem;
template<typename TSerializable>
class SocketSerializing;

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

	void OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<Socket::EReadError, ProtoLoggerRemote::LogLine>&& aResult);
	void OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::ECloseError, Socket::ECloseReason> aResult);

	std::shared_ptr<IpcAcceptor> myAcceptor;
	std::unordered_map<std::shared_ptr<Socket>, std::shared_ptr<SocketSerializing<ProtoLoggerRemote::LogLine>>> myConnections;

};
