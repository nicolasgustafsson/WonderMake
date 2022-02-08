#include "WinIpcConnection.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/MemoryUnit.h"

#include "WonderMakeBase/WinEventSystem.h"
#include "WonderMakeBase/WinPlatformSystem.h"

using namespace MemoryUnitLiterals;

constexpr auto locPipePrefix = L"\\\\.\\pipe\\";
constexpr auto locReadBufferSize = 4_KiB;

Socket::EReadError WindowsErrorCodeToReadError(const DWORD aErrorCode)
{
	switch (aErrorCode)
	{
	case ERROR_NOT_ENOUGH_MEMORY:	return Socket::EReadError::OutOfMemory;
	case ERROR_BROKEN_PIPE:			return Socket::EReadError::StateChanged;
	}

	return Socket::EReadError::InternalError;
}
Socket::EWriteError WindowsErrorCodeToWriteError(const DWORD aErrorCode)
{
	switch (aErrorCode)
	{
	case ERROR_NOT_ENOUGH_MEMORY:	return Socket::EWriteError::OutOfMemory;
	case ERROR_BROKEN_PIPE:			return Socket::EWriteError::StateChanged;
	}

	return Socket::EWriteError::InternalError;
}

WinIpcConnection::WinIpcConnection(WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept
	: myWinEvent(aWinEvent)
	, myWinPlatform(aWinPlatform)
{}

WinIpcConnection::~WinIpcConnection() noexcept
{
	Close();
}

Result<IpcConnection::ConnectionError> WinIpcConnection::Connect(std::string aConnectionName)
{
	if (aConnectionName.empty())
		return ConnectionError::InvalidArgs;

	if (myState != EState::Closed)
		return ConnectionError::InvalidState;

	const std::wstring wideConnectionName = locPipePrefix + std::wstring(aConnectionName.begin(), aConnectionName.end());
	
	const		LPCWSTR					lpFileName				= wideConnectionName.c_str();
	constexpr	DWORD					dwDesiredAccess			= GENERIC_READ | GENERIC_WRITE;
	constexpr	DWORD					dwShareMode				= NULL;
	constexpr	LPSECURITY_ATTRIBUTES	lpSecurityAttributes	= NULL;
	constexpr	DWORD					dwCreationDisposition	= OPEN_EXISTING;
	constexpr	DWORD					dwFlagsAndAttributes	= FILE_FLAG_OVERLAPPED;
	constexpr	HANDLE					hTemplateFile			= NULL;

	HANDLE handle = myWinPlatform.CreateFileW(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);

	if (handle == INVALID_HANDLE_VALUE)
	{
		const DWORD err = myWinPlatform.GetLastError();
		auto connectionErr = ConnectionError::InternalError;

		switch (err)
		{
		case ERROR_FILE_NOT_FOUND:		connectionErr = ConnectionError::NoConnection;	break;
		case ERROR_NOT_ENOUGH_MEMORY:	connectionErr = ConnectionError::OutOfMemory;	break;
		}

		return { connectionErr, err };
	}

	myFileHandle = handle;

	return Setup();
}

Result<IpcConnection::ConnectionError> WinIpcConnection::ConnectHandle(HANDLE aPipeHandle)
{
	myFileHandle = aPipeHandle;

	return Setup();
}

Result<Socket::EWriteError, Socket::EAsynchronicity> WinIpcConnection::Write(std::vector<u8> aBuffer, OnWriteCallback aOnWrite)
{
	if (aBuffer.empty())
		return EWriteError::InvalidArgs;

	if (myState != EState::Open)
		return EWriteError::InvalidState;

	if (myCurrentlyWriting)
	{
		myWriteQueue.emplace(WriteData{ std::move(aBuffer), std::move(aOnWrite) });

		return EAsynchronicity::Asynchronous;
	}

	DWORD bytesWritten = 0;
	BOOL result = TRUE;

	while (result)
	{
		result = myWinPlatform.WriteFile(myFileHandle, aBuffer.data(), static_cast<DWORD>(aBuffer.size()), &bytesWritten, &myWriteOverlapped);

		if (result)
		{
			if (bytesWritten >= aBuffer.size())
				break;
			else
				aBuffer.erase(aBuffer.begin(), aBuffer.begin() + bytesWritten);
		}
	}

	if (result)
	{
		std::move(aOnWrite)(Success);

		return EAsynchronicity::Synchronous;
	}

	const DWORD err = myWinPlatform.GetLastError();

	if (err == ERROR_IO_PENDING)
	{

		myCurrentlyWriting = true;
		myCurrentWriteCallback = std::move(aOnWrite);
		myWinEvent.RegisterEvent(myWriteOverlapped.hEvent, Bind(&WinIpcConnection::OnWrite, weak_from_this(), std::move(aBuffer)));

		return EAsynchronicity::Asynchronous;
	}

	const EWriteError writeErr = WindowsErrorCodeToWriteError(err);

	switch (writeErr)
	{
	case EWriteError::InvalidArgs:	break;
	case EWriteError::InvalidState:	break;
	case EWriteError::StateChanged:
		Reset({ ECloseReason::ClosedRemotely, err });

		break;
	case EWriteError::OutOfMemory:	break;
	case EWriteError::InternalError:
		Reset({ ECloseError::InternalError , err });

		break;
	}

	std::move(aOnWrite)(writeErr);

	return { writeErr, err };
}

Result<Socket::EReadError, Socket::EAsynchronicity> WinIpcConnection::Read(OnReadCallback aOnRead)
{
	if (myState != EState::Open)
		return EReadError::InvalidState;

	if (myCurrentlyReading)
	{
		myReadQueue.emplace(std::move(aOnRead));

		return EAsynchronicity::Asynchronous;
	}

	std::vector<u8> readBuffer;

	try
	{
		readBuffer.resize(locReadBufferSize.To<EMemoryRatio::Bytes, size_t>(), 0);
	}
	catch (const std::bad_alloc&)
	{
		std::move(aOnRead)(EReadError::OutOfMemory);

		return EReadError::OutOfMemory;
	}

	DWORD bytesRead = 0;

	const BOOL result = myWinPlatform.ReadFile(myFileHandle, static_cast<void*>(readBuffer.data()), static_cast<DWORD>(readBuffer.size()), &bytesRead, &myReadOverlapped);

	if (result)
	{
		std::vector<u8> buffer;
		readBuffer.resize(bytesRead);

		std::swap(readBuffer, buffer);

		std::move(aOnRead)(std::move(buffer));

		return EAsynchronicity::Synchronous;
	}

	const DWORD err = myWinPlatform.GetLastError();

	if (err == ERROR_IO_PENDING)
	{
		myCurrentlyReading = true;
		myCurrentReadCallback = std::move(aOnRead);
		myWinEvent.RegisterEvent(myReadOverlapped.hEvent, Bind(&WinIpcConnection::OnRead, weak_from_this(), std::move(readBuffer)));

		return EAsynchronicity::Asynchronous;
	}

	const EReadError readErr = WindowsErrorCodeToReadError(err);

	switch (readErr)
	{
	case EReadError::InvalidState:	break;
	case EReadError::StateChanged:
		Reset({ ECloseReason::ClosedRemotely, err });

		break;
	case EReadError::OutOfMemory:	break;
	case EReadError::MessageToBig:	break;
	case EReadError::InternalError:
		Reset({ ECloseError::InternalError , err });
		
		break;
	}

	std::move(aOnRead)(readErr);

	return { readErr, err };
}

void WinIpcConnection::OnClose(OnCloseCallback aOnClose)
{
	if (myState == EState::Closed)
	{
		std::move(aOnClose)(ECloseError::AlreadyClosed);

		return;
	}

	myCloseCallbacks.emplace_back(std::move(aOnClose));
}

void WinIpcConnection::Close()
{
	Reset(ECloseReason::ClosedLocally);
}

Socket::EState WinIpcConnection::GetState() const noexcept
{
	return myState;
}

Result<IpcConnection::ConnectionError> WinIpcConnection::Setup()
{
	constexpr LPSECURITY_ATTRIBUTES	lpEventAttributes = NULL;
	constexpr BOOL					bManualReset = TRUE;
	constexpr BOOL					bInitialState = FALSE;
	constexpr LPCWSTR				lpName = NULL;

	myWriteOverlapped.hEvent = myWinPlatform.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (myWriteOverlapped.hEvent == NULL)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset({ err == ERROR_NOT_ENOUGH_MEMORY ? ECloseError::OutOfMemory : ECloseError::InternalError, err });

		return { err == ERROR_NOT_ENOUGH_MEMORY ? ConnectionError::OutOfMemory : ConnectionError::InternalError, err };
	}

	myReadOverlapped.hEvent = myWinPlatform.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (myReadOverlapped.hEvent == NULL)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset({ err == ERROR_NOT_ENOUGH_MEMORY ? ECloseError::OutOfMemory : ECloseError::InternalError, err });

		return { err == ERROR_NOT_ENOUGH_MEMORY ? ConnectionError::OutOfMemory : ConnectionError::InternalError, err };
	}

	myState = EState::Open;

	return Success;
}

void WinIpcConnection::OnWrite(std::vector<u8> aBuffer)
{
	DWORD bytesWritten = 0;

	const BOOL result = myWinPlatform.GetOverlappedResult(myFileHandle, &myWriteOverlapped, &bytesWritten, FALSE);

	const DWORD err = myWinPlatform.GetLastError();

	myWinPlatform.ResetEvent(myWriteOverlapped.hEvent);

	auto onWrite = std::move(myCurrentWriteCallback);

	myCurrentlyWriting = false;
	myCurrentWriteCallback = [](auto&&) {};

	if (!result)
	{
		const EWriteError writeErr = WindowsErrorCodeToWriteError(err);

		switch (writeErr)
		{
		case EWriteError::InvalidArgs:	break;
		case EWriteError::InvalidState:	break;
		case EWriteError::StateChanged:
			Reset({ ECloseReason::ClosedRemotely, err });

			break;
		case EWriteError::OutOfMemory:	break;
		case EWriteError::InternalError:
			Reset({ ECloseError::InternalError , err });

			break;
		}

		std::move(onWrite)(writeErr);

		NextWrite();

		return;
	}

	if (bytesWritten < aBuffer.size())
	{
		aBuffer.erase(aBuffer.begin(), aBuffer.begin() + bytesWritten);

		(void)WinIpcConnection::Write(std::move(aBuffer), std::move(onWrite));
		
		return;
	}

	std::move(onWrite)(Success);

	NextWrite();
}

void WinIpcConnection::NextWrite()
{
	if (myWriteQueue.empty())
		return;

	auto writeData = std::move(myWriteQueue.front());

	myWriteQueue.pop();

	(void)WinIpcConnection::Write(std::move(writeData.Buffer), std::move(writeData.OnWrite));
}

void WinIpcConnection::OnRead(std::vector<u8> aBuffer)
{
	DWORD bytesRead = 0;

	const BOOL result = myWinPlatform.GetOverlappedResult(myFileHandle, &myReadOverlapped, &bytesRead, FALSE);

	const DWORD err = myWinPlatform.GetLastError();

	myWinPlatform.ResetEvent(myReadOverlapped.hEvent);

	auto onRead = std::move(myCurrentReadCallback);

	myCurrentlyReading = false;
	myCurrentReadCallback = [](auto&&) {};

	if (!result)
	{
		const EReadError readErr = WindowsErrorCodeToReadError(err);

		switch (readErr)
		{
		case EReadError::InvalidState:	break;
		case EReadError::StateChanged:
			Reset({ ECloseReason::ClosedRemotely, err });

			break;
		case EReadError::OutOfMemory:	break;
		case EReadError::MessageToBig:	break;
		case EReadError::InternalError:
			Reset({ ECloseError::InternalError , err });

			break;
		}

		std::move(onRead)(readErr);

		NextRead();

		return;
	}

	aBuffer.resize(bytesRead);

	std::move(onRead)(std::move(aBuffer));

	NextRead();
}

void WinIpcConnection::NextRead()
{
	if (myReadQueue.empty())
		return;

	auto readData = std::move(myReadQueue.front());

	myReadQueue.pop();

	(void)WinIpcConnection::Read(std::move(readData));
}

void WinIpcConnection::Reset(Result<ECloseError, ECloseReason> aResult)
{
	auto currentOnWrite = std::move(myCurrentWriteCallback);
	auto currentOnRead = std::move(myCurrentReadCallback);
	std::queue<WriteData> writeQueue;
	std::queue<OnReadCallback> readQueue;
	std::vector<OnCloseCallback> closeCallbacks;

	std::swap(myWriteQueue, writeQueue);
	std::swap(myReadQueue, readQueue);
	std::swap(myCloseCallbacks, closeCallbacks);

	if (myFileHandle != INVALID_HANDLE_VALUE)
		(void)myWinPlatform.CloseHandle(myFileHandle);
	if (myReadOverlapped.hEvent != NULL)
	{
		if (myCurrentlyReading)
			myWinEvent.UnregisterEvent(myReadOverlapped.hEvent);

		(void)myWinPlatform.CloseHandle(myReadOverlapped.hEvent);
	}
	if (myWriteOverlapped.hEvent != NULL)
	{
		if (myCurrentlyWriting)
			myWinEvent.UnregisterEvent(myWriteOverlapped.hEvent);

		(void)myWinPlatform.CloseHandle(myWriteOverlapped.hEvent);
	}

	myState = EState::Closed;
	myFileHandle = INVALID_HANDLE_VALUE;

	myCurrentlyWriting = false;
	myCurrentWriteCallback = [](auto&&) {};
	myWriteOverlapped = {};

	myCurrentlyReading = false;
	myCurrentReadCallback = [](auto&&) {};
	myReadOverlapped = {};

	std::move(currentOnWrite)(EWriteError::StateChanged);
	std::move(currentOnRead)(EReadError::StateChanged);

	while (!writeQueue.empty())
	{
		std::move(writeQueue.front()).OnWrite(EWriteError::StateChanged);

		writeQueue.pop();
	}
	while (!readQueue.empty())
	{
		std::move(readQueue.front())(EReadError::StateChanged);

		readQueue.pop();
	}

	for (auto&& onClose : closeCallbacks)
		std::move(onClose)(aResult);
}
