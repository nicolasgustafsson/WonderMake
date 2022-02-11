#pragma once

#include "wondermake-io/IpcConnection.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/MemoryUnit.h"

#include <memory>
#include <mutex>

namespace ProtoLoggerRemote
{
	class LogLine;
}

class IpcSystem;
template<typename TSerializable>
class SocketSerializing;

class LoggerRemoteConnectionSystem
	: public System<
		Policy::Set<
			PAdd<IpcSystem, PWrite>>,
		STrait::Set<
			STLogger,
			STSingleton>>
	, public LoggerBase
	, public std::enable_shared_from_this<LoggerRemoteConnectionSystem>
{
public:
	Result<IpcConnection::ConnectionError> ConnectIpc(std::string aIpcName);

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) override;

private:
	void OnClosed(Result<Socket::ECloseError, Socket::ECloseReason> aResult);

	std::recursive_mutex myMutex;
	std::shared_ptr<SocketSerializing<ProtoLoggerRemote::LogLine>> myConnection;

};
