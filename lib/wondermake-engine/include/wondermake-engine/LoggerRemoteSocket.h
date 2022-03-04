#pragma once

#include "wondermake-io/IpcAcceptor.h"
#include "wondermake-io/Socket.h"
#include "wondermake-io/SocketProtobuf.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <mutex>
#include <unordered_map>

namespace ProtoLoggerRemote
{
	class Downstream;
	class Upstream;
}

class LoggerRemoteSocket
	: public std::enable_shared_from_this<LoggerRemoteSocket>
{
public:
	LoggerRemoteSocket(AnyExecutor aExecutor) noexcept;

	IpcAcceptor::ResultTypeOpen OpenIpc(SharedReference<IpcAcceptor> aAcceptor, std::string aIpcName);

private:
	using SocketType = SocketProtobuf<ProtoLoggerRemote::Downstream, ProtoLoggerRemote::Upstream>;

	void OnConnection(IpcAcceptor::ResultTypeConnection&& aResult);
	void OnIpcClosed(const IpcAcceptor::FutureTypeClose&);

	void OnConnectionMessage(std::weak_ptr<Socket> aConnection, SocketType::ResultTypeRead aResult);
	void OnConnectionClosed(std::weak_ptr<Socket> aConnection, SocketType::ResultTypeClose aResult);

	std::mutex myMutex;

	AnyExecutor myExecutor;
	std::shared_ptr<IpcAcceptor> myAcceptor;
	std::unordered_map<SharedReference<Socket>, SharedReference<SocketType>> myConnections;

};
