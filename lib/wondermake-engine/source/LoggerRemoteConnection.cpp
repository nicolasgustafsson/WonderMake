#include "wondermake-engine/LoggerRemoteConnection.h"

#include "LoggerRemoteMessage.h"

#include "wondermake-io/SocketSerializingImpl.h"

#include "wondermake-base/LoggerTypes.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"

using namespace MemoryUnitLiterals;

using ProtoLoggerRemote::LogLine;

LoggerRemoteConnection::LoggerRemoteConnection(AnyExecutor aExecutor) noexcept
	: myExecutor(std::move(aExecutor))
{}

Result<void, IpcConnection::SConnectionError> LoggerRemoteConnection::ConnectIpc(SharedReference<IpcConnection> aConnection, std::string aIpcName)
{
	std::lock_guard lock(myMutex);

	auto result = aConnection->Connect(std::move(aIpcName));

	if (!result)
		return result;

	myConnection = std::make_shared<SocketSerializingImpl<ProtoLoggerRemote::LogLine>>(myExecutor, 4_KiB, &SerializeLogline, &DeserializeLogline, std::move(aConnection));

	myConnection->OnClose()
		.ThenRun(myExecutor, FutureRunResult(Bind(&LoggerRemoteConnection::OnClosed, weak_from_this())))
		.Detach();

	Logger::Get().AddLogger(weak_from_this());

	return result;
}

void LoggerRemoteConnection::Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage)
{
	std::lock_guard lock(myMutex);

	if (!myConnection
		|| myConnection->GetState() != Socket::EState::Open)
		return;

	LogLine logline;

	logline.set_log(std::move(aLogMessage));
	logline.set_level(static_cast<LogLine::ELogLevel>(aLevel));
	logline.set_severity(static_cast<LogLine::ELogSeverity>(aSeverity));

	myConnection->WriteMessage(upstream)
		.ThenRun(myExecutor, FutureRunResult([](auto aResult)
			{
				if (aResult)
					return;

				WmLogError(TagWonderMake << TagWmLoggerRemote << "Failed to write to log connection. Error: " << aResult.Err() << '.');
			}))
		.Detach();
}

void LoggerRemoteConnection::OnClosed(Result<Socket::SCloseLocation, Socket::SCloseError> aResult)
{
	std::lock_guard lock(myMutex);

	myConnection.reset();

	if (aResult)
		WmLogInfo(TagWonderMake << TagWmLoggerRemote << "Local IPC log connection closed. " << aResult.Unwrap() << '.');
	else
		WmLogError(TagWonderMake << TagWmLoggerRemote << "Local IPC log connection closed. Error: " << aResult.Err() << '.');
}
