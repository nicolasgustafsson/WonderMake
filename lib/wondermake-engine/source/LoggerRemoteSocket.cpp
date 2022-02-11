#include "wondermake-engine/LoggerRemoteSocket.h"

#include "LoggerRemoteMessage.h"

#include "wondermake-io/IpcConnection.h"
#include "wondermake-io/IpcSystem.h"
#include "wondermake-io/SocketSerializingImpl.h"

#include "wondermake-base/LoggerTypes.h"

#include "wondermake-utility/Bindable.h"

using namespace MemoryUnitLiterals;

Result<IpcAcceptor::EOpenError> LoggerRemoteSocket::OpenIpc(SharedReference<IpcAcceptor> aAcceptor, std::string aIpcName)
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
	auto socket = std::make_shared<SocketSerializingImpl<ProtoLoggerRemote::LogLine>>(4_KiB, &SerializeLogline, &DeserializeLogline, SharedReference<Socket>::FromPointer(aConnection));

	myConnections.emplace(aConnection, socket);

	socket->OnClose(Bind(&LoggerRemoteSocket::OnConnectionClosed, weak_from_this(), std::weak_ptr(aConnection)));

	const auto result = socket->ReadMessage(Bind(&LoggerRemoteSocket::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WM_LOG_ERROR("Failed to read from remote connection, error: ", static_cast<Socket::EReadError>(result), ".");
}

void LoggerRemoteSocket::OnIpcClosed(Result<IpcAcceptor::ECloseReason> aResult)
{
	if (aResult)
		WM_LOG_INFO("Remote IPC log socket closed.");
	else
		WM_LOG_ERROR("Remote IPC log socket closed, error: ", static_cast<IpcAcceptor::ECloseReason>(aResult), ".");
}


void LoggerRemoteSocket::OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<Socket::EReadError, ProtoLoggerRemote::LogLine>&& aResult)
{
	auto connection = aConnection.lock();

	if (!connection)
		return;

	const auto it = myConnections.find(connection);

	if (it == myConnections.cend())
	{
		WM_LOG_ERROR("IPC log connection read from unknown socket.");

		return;
	}

	if (!aResult)
	{
		WM_LOG_ERROR("IPC log connection read failed, error: ", static_cast<Socket::EReadError>(aResult), ".");

		connection->Close();

		return;
	}

	ProtoLoggerRemote::LogLine message = std::move(aResult);

	Logger::Get().Print(static_cast<ELogSeverity>(message.severity()), static_cast<ELogLevel>(message.level()), message.log());
	
	const auto result = it->second->ReadMessage(Bind(&LoggerRemoteSocket::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WM_LOG_ERROR("Failed to read from log connection, error: ", static_cast<Socket::EReadError>(result), ".");
}

void LoggerRemoteSocket::OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::ECloseError, Socket::ECloseReason> aResult)
{
	const auto connection = aConnection.lock();

	if (!connection)
		return;

	const auto it = myConnections.find(connection);

	if (it == myConnections.cend())
	{
		if (aResult)
			WM_LOG_WARNING("Unknown remote IPC log connection closed. Possibly closed twice? ", (aResult == Socket::ECloseReason::ClosedLocally ? "Closed locally." : "Closed remotely."));
		else
			WM_LOG_ERROR("Unknown remote IPC log connection closed. Possibly closed twice? Error: ", static_cast<Socket::ECloseError>(aResult), ".");

		return;
	}

	myConnections.erase(it);

	if (aResult)
		WM_LOG_INFO("Remote IPC log connection closed. ", (aResult == Socket::ECloseReason::ClosedLocally ? "Closed locally." : "Closed remotely."));
	else
		WM_LOG_ERROR("Remote IPC log connection closed. Error: ", static_cast<Socket::ECloseError>(aResult), ".");
}