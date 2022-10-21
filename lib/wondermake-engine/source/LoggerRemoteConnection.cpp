#include "wondermake-engine/LoggerRemoteConnection.h"

#include "wondermake-io/SocketProtobufImpl.h"

#include "wondermake-base/LoggerTypes.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/LoggerRemote.pb.h"
#pragma warning(pop)

using namespace MemoryUnitLiterals;

using ProtoLoggerRemote::Downstream;
using ProtoLoggerRemote::LogLine;
using ProtoLoggerRemote::Upstream;

LoggerRemoteConnection::LoggerRemoteConnection(AnyExecutor aExecutor) noexcept
	: myExecutor(std::move(aExecutor))
{}

Result<void, IpcConnection::SConnectionError> LoggerRemoteConnection::ConnectIpc(SharedReference<IpcConnection> aConnection, std::string aIpcName)
{
	std::lock_guard lock(myMutex);

	auto result = aConnection->Connect(std::move(aIpcName));

	if (!result)
		return result;

	myConnection = std::make_shared<SocketProtobufImpl<Upstream, Downstream>>(myExecutor, 4_KiB, std::move(aConnection));

	myConnection->OnClose()
		.ThenRun(myExecutor, FutureRunResult(Bind(&LoggerRemoteConnection::OnClosed, weak_from_this())))
		.Detach();

	Logger::Get().AddLogger(weak_from_this());

	return result;
}

void LoggerRemoteConnection::Print(const SLogLine& aLogLine)
{
	std::lock_guard lock(myMutex);

	if (!myConnection
		|| myConnection->GetState() != Socket::EState::Open)
		return;

	Upstream upstream;

	auto& push = *upstream.mutable_push();

	auto& logline = *push.mutable_logline();

	logline.set_level(static_cast<LogLine::ELogLevel>(aLogLine.Level));
	logline.set_severity(static_cast<LogLine::ELogSeverity>(aLogLine.Severity));
	logline.set_message(aLogLine.Message);
	logline.set_file(aLogLine.File);
	logline.set_line(aLogLine.Line);
	logline.set_timestamp(aLogLine.Timestamp);
	logline.set_thread_hash(aLogLine.ThreadHash);
	logline.set_logger_name(aLogLine.LoggerName);

	myConnection->WriteMessage(upstream)
		.ThenRun(myExecutor, FutureRunResult([](auto aResult)
			{
				if (aResult)
					return;

				WmLogError(TagWonderMake << TagWmLoggerRemote << "Failed to write to log connection. Error: " << aResult.Err() << '.');
			}))
		.Detach();
}

void LoggerRemoteConnection::OnClosed(SocketType::ResultTypeClose aResult)
{
	std::lock_guard lock(myMutex);

	myConnection.reset();

	if (aResult)
		WmLogInfo(TagWonderMake << TagWmLoggerRemote << "Local IPC log connection closed. " << aResult.Unwrap() << '.');
	else
		WmLogError(TagWonderMake << TagWmLoggerRemote << "Local IPC log connection closed. Error: " << aResult.Err() << '.');
}
