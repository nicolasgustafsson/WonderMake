#include "wondermake-engine/LoggerRemoteConnection.h"

#include "LoggerRemoteMessage.h"

#include "wondermake-io/SocketSerializingImpl.h"

#include "wondermake-base/LoggerTypes.h"

#include "wondermake-utility/Bindable.h"

using namespace MemoryUnitLiterals;

using ProtoLoggerRemote::LogLine;

Result<IpcConnection::ConnectionError> LoggerRemoteConnection::ConnectIpc(SharedReference<IpcConnection> aConnection, std::string aIpcName)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	auto result = aConnection->Connect(std::move(aIpcName));

	if (!result)
		return result;

	myConnection = std::make_shared<SocketSerializingImpl<ProtoLoggerRemote::LogLine>>(4_KiB, &SerializeLogline, &DeserializeLogline, SharedReference<Socket>::FromPointer(std::move(aConnection)));

	myConnection->OnClose(Bind(&LoggerRemoteConnection::OnClosed, weak_from_this()));

	Logger::Get().AddLogger(weak_from_this());

	return result;
}

void LoggerRemoteConnection::Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	if (!myConnection
		|| myConnection->GetState() != Socket::EState::Open)
		return;

	LogLine logline;

	logline.set_log(std::move(aLogMessage));
	logline.set_level(static_cast<LogLine::ELogLevel>(aLevel));
	logline.set_severity(static_cast<LogLine::ELogSeverity>(aSeverity));

	myConnection->WriteMessage(logline, [](auto aResult)
		{
			if (aResult)
				return;

			WM_LOG_ERROR("Failed to write to log connection. Error: ", static_cast<Socket::EWriteError>(aResult), ".");
		});
}

void LoggerRemoteConnection::OnClosed(Result<Socket::ECloseError, Socket::ECloseReason> aResult)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myConnection.reset();

	if (aResult)
		WM_LOG_INFO("Local IPC log connection closed. ", (aResult == Socket::ECloseReason::ClosedLocally ? "Closed locally." : "Closed remotely."));
	else
		WM_LOG_ERROR("Local IPC log connection closed. Error: ", static_cast<Socket::ECloseError>(aResult), ".");
}
