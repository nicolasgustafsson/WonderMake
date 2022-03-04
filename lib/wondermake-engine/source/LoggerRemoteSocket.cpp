#include "wondermake-engine/LoggerRemoteSocket.h"

#include "wondermake-io/IpcConnection.h"
#include "wondermake-io/IpcSystem.h"
#include "wondermake-io/SocketProtobufImpl.h"

#include "wondermake-base/LoggerTypes.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-engine-proto/LoggerRemote.pb.h"
#pragma warning(pop)

using namespace MemoryUnitLiterals;

LoggerRemoteSocket::LoggerRemoteSocket(AnyExecutor aExecutor) noexcept
	: myExecutor(std::move(aExecutor))
{}

Result<void, IpcAcceptor::SOpenError> LoggerRemoteSocket::OpenIpc(SharedReference<IpcAcceptor> aAcceptor, std::string aIpcName)
{
	std::unique_lock lock(myMutex);

	myAcceptor = std::move(aAcceptor);

	const auto result = myAcceptor->Open(std::move(aIpcName));

	if (result)
	{
		auto futureClose = myAcceptor->OnClose();
		auto futureConnection = myAcceptor->OnConnection();

		lock.unlock();

		futureClose
			.ThenRun(myExecutor, Bind(&LoggerRemoteSocket::OnIpcClosed, weak_from_this()))
			.Detach();
		futureConnection
			.ThenRun(myExecutor, FutureRunResult(Bind(&LoggerRemoteSocket::OnConnection, weak_from_this())))
			.Detach();
	}

	return result;
}

void LoggerRemoteSocket::OnConnection(IpcAcceptor::ResultTypeConnection&& aResult)
{
	if (!aResult)
	{
		WmLogError(TagWonderMake << TagWmLoggerRemote << "IPC connection error: " << aResult.Err() << '.');

		return;
	}
	
	std::unique_lock lock(myMutex);

	auto& connection = aResult.Unwrap();

	auto socket = MakeSharedReference<SocketProtobufImpl<ProtoLoggerRemote::Downstream, ProtoLoggerRemote::Upstream>>(myExecutor, 4_KiB, connection);

	myConnections.emplace(connection, socket);

	auto self = weak_from_this();

	lock.unlock();

	socket->OnClose()
		.ThenRun(myExecutor, FutureRunResult(Bind(&LoggerRemoteSocket::OnConnectionClosed, self, std::weak_ptr<Socket>(connection))))
		.Detach();

	socket->ReadMessage()
		.ThenRun(myExecutor, FutureRunResult(Bind(&LoggerRemoteSocket::OnConnectionMessage, std::move(self), std::weak_ptr<Socket>(connection))))
		.Detach();
}

void LoggerRemoteSocket::OnIpcClosed(const IpcAcceptor::FutureTypeClose&)
{
	WmLogInfo(TagWonderMake << TagWmLoggerRemote << "Remote IPC log socket closed.");
}

void LoggerRemoteSocket::OnConnectionMessage(std::weak_ptr<Socket> aConnection, SocketType::ResultTypeRead aResult)
{
	auto connectionResult = SharedReference<Socket>::FromPointer(aConnection.lock());

	if (!connectionResult)
		return;

	auto& connection = connectionResult.Unwrap();

	std::unique_lock lock(myMutex);

	const auto it = myConnections.find(connection);

	if (it == myConnections.cend())
	{
		WmLogError(TagWonderMake << TagWmLoggerRemote << "IPC log connection read from unknown socket.");

		return;
	}

	if (!aResult)
	{
		WmLogError(TagWonderMake << TagWmLoggerRemote << "IPC log connection read failed, error: " << aResult.Err() << '.');

		connection->Close();

		return;
	}

	ProtoLoggerRemote::LogLine message = std::move(aResult).Unwrap();

	Logger::Get().Print(static_cast<ELogSeverity>(message.severity()), static_cast<ELogLevel>(message.level()), message.log());
	
	auto future = it->second->ReadMessage();

	lock.unlock();

	future.ThenRun(myExecutor, FutureRunResult(Bind(&LoggerRemoteSocket::OnConnectionMessage, weak_from_this(), std::move(aConnection))))
		.Detach();
}

void LoggerRemoteSocket::OnConnectionClosed(std::weak_ptr<Socket> aConnection, SocketType::ResultTypeClose aResult)
{
	std::lock_guard lock(myMutex);

	auto connectionResult = SharedReference<Socket>::FromPointer(aConnection.lock());

	if (!connectionResult)
		return;

	auto& connection = connectionResult.Unwrap();

	const auto it = myConnections.find(connection);

	if (it == myConnections.cend())
	{
		if (aResult)
			WmLogWarning(TagWonderMake << TagWmLoggerRemote << "Unknown remote IPC log connection closed. Possibly closed twice? " << aResult.Unwrap() << '.');
		else
			WmLogError(TagWonderMake << TagWmLoggerRemote << "Unknown remote IPC log connection closed. Possibly closed twice? Error: " << aResult.Err() << '.');

		return;
	}

	myConnections.erase(it);

	if (aResult)
		WmLogInfo(TagWonderMake << TagWmLoggerRemote << "Remote IPC log connection closed. " << aResult.Unwrap() << '.');
	else
		WmLogError(TagWonderMake << TagWmLoggerRemote << "Remote IPC log connection closed. Error: " << aResult.Err() << '.');
}
