#include "WonderMakeEngine/LoggerRemoteSocketSystem.h"

#include "LoggerRemoteMessageType.h"

#include "WonderMakeIo/IpcConnection.h"
#include "WonderMakeIo/IpcSystem.h"
#include "WonderMakeIo/SocketSerializingImpl.h"

#include "WonderMakeBase/SystemGlobal.h"

#include "wondermake-utility/Bindable.h"

using namespace MemoryUnitLiterals;

REGISTER_SYSTEM(LoggerRemoteSocketSystem);

Result<IpcAcceptor::EOpenError> LoggerRemoteSocketSystem::OpenIpc(std::string aIpcName)
{
	myAcceptor = Get<IpcSystem>().CreateAcceptor();

	return myAcceptor->Open(std::move(aIpcName),
		{
			Bind(&LoggerRemoteSocketSystem::OnConnection, weak_from_this()),
			Bind(&LoggerRemoteSocketSystem::OnIpcClosed, weak_from_this())
		});
}

void LoggerRemoteSocketSystem::OnConnection(std::shared_ptr<Socket>&& aConnection)
{
	auto socket = std::make_shared<SocketSerializingImpl<LoggerRemoteMessageType>>(4_KiB, &LoggerRemoteMessageType::Serialize, &LoggerRemoteMessageType::Deserialize, SharedReference<Socket>::FromPointer(aConnection));

	myConnections.emplace(aConnection, socket);

	socket->OnClose(Bind(&LoggerRemoteSocketSystem::OnConnectionClosed, weak_from_this(), std::weak_ptr(aConnection)));

	const auto result = socket->ReadMessage(Bind(&LoggerRemoteSocketSystem::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WM_LOG_ERROR("Failed to read from remote connection, error: ", static_cast<Socket::EReadError>(result), ".");
}

void LoggerRemoteSocketSystem::OnIpcClosed(Result<IpcAcceptor::ECloseReason> aResult)
{
	if (aResult)
		WM_LOG_INFO("Remote IPC log socket closed.");
	else
		WM_LOG_ERROR("Remote IPC log socket closed, error: ", static_cast<IpcAcceptor::ECloseReason>(aResult), ".");
}


void LoggerRemoteSocketSystem::OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<Socket::EReadError, LoggerRemoteMessageType>&& aResult)
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

	LoggerRemoteMessageType message = std::move(aResult);

	Logger::Get().Print(message.Severity, message.Level, message.Message);
	
	const auto result = it->second->ReadMessage(Bind(&LoggerRemoteSocketSystem::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WM_LOG_ERROR("Failed to read from log connection, error: ", static_cast<Socket::EReadError>(result), ".");
}

void LoggerRemoteSocketSystem::OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::ECloseError, Socket::ECloseReason> aResult)
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