#include "wondermake-engine/LoggerRemoteSocket.h"

#include "LoggerRemoteMessage.h"

#include "wondermake-io/IpcConnection.h"
#include "wondermake-io/IpcSystem.h"
#include "wondermake-io/SocketSerializingImpl.h"

#include "wondermake-base/LoggerTypes.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"

using namespace MemoryUnitLiterals;

Result<void, IpcAcceptor::SOpenError> LoggerRemoteSocket::OpenIpc(SharedReference<IpcAcceptor> aAcceptor, std::string aIpcName)
{
	myAcceptor = std::move(aAcceptor);

	return myAcceptor->Open(std::move(aIpcName),
		{
			Bind(&LoggerRemoteSocket::OnConnection, weak_from_this()),
			Bind(&LoggerRemoteSocket::OnIpcClosed, weak_from_this())
		});
}

void LoggerRemoteSocket::OnConnection(std::shared_ptr<Socket>&& aConnection)
{
	auto connectionRef = SharedReference<Socket>::FromPointer(aConnection).Unwrap();

	auto socket = std::make_shared<SocketSerializingImpl<ProtoLoggerRemote::LogLine>>(4_KiB, &SerializeLogline, &DeserializeLogline, std::move(connectionRef));

	myConnections.emplace(aConnection, socket);

	socket->OnClose(Bind(&LoggerRemoteSocket::OnConnectionClosed, weak_from_this(), std::weak_ptr(aConnection)));

	const auto result = socket->ReadMessage(Bind(&LoggerRemoteSocket::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WmLogError(TagWonderMake << TagWmLoggerRemote << "Failed to read from remote connection, error: " << result.Err() << '.');
}

void LoggerRemoteSocket::OnIpcClosed(Result<void, IpcAcceptor::SCloseError> aResult)
{
	if (aResult)
		WmLogInfo(TagWonderMake << TagWmLoggerRemote << "Remote IPC log socket closed.");
	else
		WmLogError(TagWonderMake << TagWmLoggerRemote << "Remote IPC log socket closed, error: " << aResult.Err() << '.');
}

void LoggerRemoteSocket::OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<ProtoLoggerRemote::LogLine, Socket::SReadError>&& aResult)
{
	auto connection = aConnection.lock();

	if (!connection)
		return;

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
	
	const auto result = it->second->ReadMessage(Bind(&LoggerRemoteSocket::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WmLogError(TagWonderMake << TagWmLoggerRemote << "Failed to read from log connection, error: " << result.Err() << '.');
}

void LoggerRemoteSocket::OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::SCloseLocation, Socket::SCloseError> aResult)
{
	const auto connection = aConnection.lock();

	if (!connection)
		return;

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
