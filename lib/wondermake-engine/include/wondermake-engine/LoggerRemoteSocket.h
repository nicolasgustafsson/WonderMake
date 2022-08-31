#pragma once

#include "wondermake-io/IpcAcceptor.h"
#include "wondermake-io/Socket.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <unordered_map>

namespace ProtoLoggerRemote
{
	class LogLine;
}

template<typename TSerializable>
class SocketSerializing;

class LoggerRemoteSocket
	: public std::enable_shared_from_this<LoggerRemoteSocket>
{
public:
	Result<void, IpcAcceptor::SOpenError> OpenIpc(SharedReference<IpcAcceptor> aAcceptor, std::string aIpcName);

private:
	void OnConnection(std::shared_ptr<Socket>&& aConnection);

	void OnIpcClosed(Result<void, IpcAcceptor::SCloseError> aResult);

	void OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<ProtoLoggerRemote::LogLine, Socket::SReadError>&& aResult);
	void OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::SCloseLocation, Socket::SCloseError> aResult);

	std::shared_ptr<IpcAcceptor> myAcceptor;
	std::unordered_map<std::shared_ptr<Socket>, std::shared_ptr<SocketSerializing<ProtoLoggerRemote::LogLine>>> myConnections;

};
