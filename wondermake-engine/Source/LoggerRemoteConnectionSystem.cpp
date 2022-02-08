#include "WonderMakeEngine/LoggerRemoteConnectionSystem.h"

#include "LoggerRemoteMessageType.h"

#include "wondermake-io/IpcSystem.h"
#include "wondermake-io/SocketSerializingImpl.h"

#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

using namespace MemoryUnitLiterals;

REGISTER_SYSTEM(LoggerRemoteConnectionSystem);

Result<IpcConnection::ConnectionError> LoggerRemoteConnectionSystem::ConnectIpc(std::string aIpcName)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	auto connection = Get<IpcSystem>().CreateConnection();

	auto result = connection->Connect(std::move(aIpcName));

	if (!result)
		return result;

	myConnection = std::make_shared<SocketSerializingImpl<LoggerRemoteMessageType>>(4_KiB, &LoggerRemoteMessageType::Serialize, &LoggerRemoteMessageType::Deserialize, SharedReference<Socket>::FromPointer(std::move(connection)));

	myConnection->OnClose(Bind(&LoggerRemoteConnectionSystem::OnClosed, weak_from_this()));

	Logger::Get().AddLogger(shared_from_this());

	return result;
}

void LoggerRemoteConnectionSystem::Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	if (!myConnection
		|| myConnection->GetState() != Socket::EState::Open)
		return;

	LoggerRemoteMessageType data;

	data.Severity = aSeverity;
	data.Level = aLevel;
	data.Message = std::move(aLogMessage);

	myConnection->WriteMessage(data, [](auto aResult)
		{
			if (aResult)
				return;

			WM_LOG_ERROR("Failed to write to log connection. Error: ", static_cast<Socket::EWriteError>(aResult), ".");
		});
}

void LoggerRemoteConnectionSystem::OnClosed(Result<Socket::ECloseError, Socket::ECloseReason> aResult)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myConnection.reset();

	if (aResult)
		WM_LOG_INFO("Local IPC log connection closed. ", (aResult == Socket::ECloseReason::ClosedLocally ? "Closed locally." : "Closed remotely."));
	else
		WM_LOG_ERROR("Local IPC log connection closed. Error: ", static_cast<Socket::ECloseError>(aResult), ".");
}
