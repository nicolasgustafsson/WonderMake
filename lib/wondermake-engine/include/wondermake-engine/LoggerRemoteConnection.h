#pragma once

#include "wondermake-io/IpcConnection.h"
#include "wondermake-io/SocketProtobuf.h"

#include "wondermake-base/Logger.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <mutex>

namespace ProtoLoggerRemote
{
	class Downstream;
	class Upstream;
}

template<typename TProtobufWriteMessage, typename TProtobufReadMessage>
class SocketProtobuf;

class LoggerRemoteConnection
	: public LoggerBase
	, public std::enable_shared_from_this<LoggerRemoteConnection>
{
public:
	LoggerRemoteConnection(AnyExecutor aExecutor) noexcept;

	Result<void, IpcConnection::SConnectionError> ConnectIpc(SharedReference<IpcConnection> aConnection, std::string aIpcName);

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) override;

private:
	using SocketType = SocketProtobuf<ProtoLoggerRemote::Upstream, ProtoLoggerRemote::Downstream>;

	void OnClosed(SocketType::ResultTypeClose aResult);

	std::recursive_mutex myMutex;
	AnyExecutor myExecutor;
	std::shared_ptr<SocketProtobuf<ProtoLoggerRemote::Upstream, ProtoLoggerRemote::Downstream>> myConnection;

};
