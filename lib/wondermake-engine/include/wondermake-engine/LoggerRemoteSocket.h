#pragma once

#include "wondermake-io/IpcAcceptor.h"
#include "wondermake-io/Socket.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <mutex>
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
	LoggerRemoteSocket(AnyExecutor aExecutor) noexcept;

	IpcAcceptor::ResultTypeOpen OpenIpc(SharedReference<IpcAcceptor> aAcceptor, std::string aIpcName);

private:
	void OnConnection(IpcAcceptor::ResultTypeConnection&& aResult);
	void OnIpcClosed(const IpcAcceptor::FutureTypeClose&);

	void OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<ProtoLoggerRemote::LogLine, Socket::SReadError>&& aResult);
	void OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::SCloseLocation, Socket::SCloseError> aResult);

	std::mutex myMutex;

	AnyExecutor myExecutor;
	std::shared_ptr<IpcAcceptor> myAcceptor;
	std::unordered_map<SharedReference<Socket>, std::shared_ptr<SocketSerializing<ProtoLoggerRemote::LogLine>>> myConnections;

};
