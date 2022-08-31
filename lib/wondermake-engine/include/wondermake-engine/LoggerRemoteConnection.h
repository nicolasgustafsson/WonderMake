#pragma once

#include "wondermake-io/IpcConnection.h"

#include "wondermake-base/Logger.h"

#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <mutex>

namespace ProtoLoggerRemote
{
	class LogLine;
}

template<typename TSerializable>
class SocketSerializing;

class LoggerRemoteConnection
	: public LoggerBase
	, public std::enable_shared_from_this<LoggerRemoteConnection>
{
public:
	Result<void, IpcConnection::SConnectionError> ConnectIpc(SharedReference<IpcConnection> aConnection, std::string aIpcName);

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) override;

private:
	void OnClosed(Result<Socket::SCloseLocation, Socket::SCloseError> aResult);

	std::recursive_mutex myMutex;
	std::shared_ptr<SocketSerializing<ProtoLoggerRemote::LogLine>> myConnection;

};
