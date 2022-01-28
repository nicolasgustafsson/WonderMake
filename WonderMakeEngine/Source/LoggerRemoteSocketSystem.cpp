#include "WonderMakeEngine/LoggerRemoteSocketSystem.h"

#include "LoggerRemoteMessageType.h"

#include "WonderMakeIo/IpcConnection.h"
#include "WonderMakeIo/IpcSystem.h"

#include "WonderMakeBase/SystemGlobal.h"

#include "WonderMakeUtility/Bindable.h"

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
	myConnections.emplace(aConnection);

	aConnection->OnClose(Bind(&LoggerRemoteSocketSystem::OnConnectionClosed, weak_from_this(), std::weak_ptr(aConnection)));

	const auto result = aConnection->Read(Bind(&LoggerRemoteSocketSystem::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

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


void LoggerRemoteSocketSystem::OnConnectionMessage(std::weak_ptr<Socket> aConnection, Result<Socket::EReadError, std::vector<u8>>&& aResult)
{
	auto connection = aConnection.lock();

	if (!connection)
		return;

	if (!aResult)
	{
		WM_LOG_ERROR("IPC log connection read failed, error: ", static_cast<Socket::EReadError>(aResult), ".");

		connection->Close();

		return;
	}

	std::vector<u8> dataBuffer = std::move(aResult);

	myBuffer.insert(myBuffer.end(), dataBuffer.begin(), dataBuffer.end());

	std::span<u8> remainingData = myBuffer;
	size_t readData = 0;

	for (;;)
	{
		LoggerRemoteMessageType data;

		const size_t parsedData = data.Deserialize(remainingData);

		if (parsedData == 0)
			break;

		Logger::Get().Print(data.Severity, data.Level, data.Message);

		readData += parsedData;
		remainingData = std::span<u8>(remainingData.begin() + parsedData, remainingData.end());
	}

	myBuffer.erase(myBuffer.begin(), myBuffer.begin() + readData);

	const auto result = connection->Read(Bind(&LoggerRemoteSocketSystem::OnConnectionMessage, weak_from_this(), std::weak_ptr(aConnection)));

	if (!result)
		WM_LOG_ERROR("Failed to read from log connection, error: ", static_cast<Socket::EReadError>(result), ".");
}

void LoggerRemoteSocketSystem::OnConnectionClosed(std::weak_ptr<Socket> aConnection, Result<Socket::ECloseError, Socket::ECloseReason> aResult)
{
	auto connection = aConnection.lock();

	if (!connection)
		return;

	auto it = myConnections.find(connection);

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