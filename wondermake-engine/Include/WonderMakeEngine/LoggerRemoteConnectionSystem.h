#pragma once

#include "WonderMakeIo/IpcConnection.h"

#include "WonderMakeBase/Logger.h"
#include "WonderMakeBase/System.h"

#include "wondermake-utility/MemoryUnit.h"

#include <memory>
#include <mutex>

class IpcSystem;
struct LoggerRemoteMessageType;
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
	std::shared_ptr<SocketSerializing<LoggerRemoteMessageType>> myConnection;

};
