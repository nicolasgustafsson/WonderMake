#include "WinIpcConnection.h"

#include "wondermake-base/WinEventSystem.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/BufferExecutor.h"
#include "wondermake-utility/MemoryUnit.h"

using namespace MemoryUnitLiterals;

inline constexpr auto locPipePrefix = L"\\\\.\\pipe\\";
inline constexpr auto locReadBufferSize = 4_KiB;

inline constexpr auto locNoOp = []() {};

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

WinIpcConnection::WinIpcConnection(AnyExecutor aExecutor, WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept
	: myExecutor(std::move(aExecutor))
	, myWinEvent(aWinEvent)
	, myWinPlatform(aWinPlatform)
{}

WinIpcConnection::~WinIpcConnection() noexcept
{
	myMutex.lock();

	Closure closure = Reset(Ok(SCloseLocation{ ECloseLocation::ClosedLocally }));

	myMutex.unlock();

	std::move(closure)();
}

Result<void, IpcConnection::SConnectionError> WinIpcConnection::Connect(std::string aConnectionName)
{
	if (aConnectionName.empty())
		return Err(SConnectionError{ EConnectionError::InvalidArgs });

	if (myState != EState::Closed)
		return Err(SConnectionError{ EConnectionError::InvalidState });

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
		auto connectionErr = EConnectionError::InternalError;

		switch (err)
		{
		case ERROR_FILE_NOT_FOUND:		connectionErr = EConnectionError::NoConnection;	break;
		case ERROR_NOT_ENOUGH_MEMORY:	connectionErr = EConnectionError::OutOfMemory;	break;
		}

		return Err(SConnectionError{ connectionErr, err });
	}

	myFileHandle = handle;

	return Setup();
}

Result<void, IpcConnection::SConnectionError> WinIpcConnection::ConnectHandle(HANDLE aPipeHandle)
{
	myFileHandle = aPipeHandle;

	return Setup();
}

Socket::FutureTypeWrite WinIpcConnection::Write(std::vector<u8> aBuffer)
{
	auto [promise, future] = MakeAsync<ResultTypeWrite>();

	myMutex.lock();

	Closure closure = PerformWrite(std::move(aBuffer), std::move(promise));

	myMutex.unlock();

	std::move(closure)();

	return future;
}

Socket::FutureTypeRead WinIpcConnection::Read()
{
	const auto getNextRequestId = [&requestQueue = myReadQueue]() -> RequestIdType
	{
		for (RequestIdType requestId = 0;; ++requestId)
		{
			const auto pred = [requestId](const auto& aRequest)
			{
				return aRequest.RequestId == requestId;
			};

			if (std::find_if(requestQueue.begin(), requestQueue.end(), pred) == requestQueue.end())
				return requestId;
		}
	};
	auto [promise, future] = MakeAsync<Socket::ResultTypeRead>();

	myMutex.lock();

	const auto requestId = getNextRequestId();

	Closure closure = PerformRead(std::move(promise), requestId);

	myMutex.unlock();

	future.OnCancel(myExecutor, Bind(&WinIpcConnection::CancelRead, weak_from_this(), requestId));

	std::move(closure)();

	return future;
}

Socket::FutureTypeClose WinIpcConnection::OnClose()
{
	if (myState == EState::Closed)
		return MakeCompletedFuture<Socket::ResultTypeClose>(Err(SCloseError{ ECloseError::AlreadyClosed }));

	auto [promise, future] = MakeAsync<Socket::ResultTypeClose>();

	myClosePromises.emplace_back(std::move(promise));

	return future;
}

Socket::FutureTypeClose WinIpcConnection::Close()
{
	static constexpr auto closeResult = Ok(SCloseLocation{ ECloseLocation::ClosedLocally });

	myMutex.lock();

	Closure closure = Reset(closeResult);

	myMutex.unlock();

	std::move(closure)();

	return MakeCompletedFuture<Socket::ResultTypeClose>(closeResult);
}

Socket::EState WinIpcConnection::GetState() const noexcept
{
	return myState;
}

Result<void, IpcConnection::SConnectionError> WinIpcConnection::Setup()
{
	constexpr LPSECURITY_ATTRIBUTES	lpEventAttributes = NULL;
	constexpr BOOL					bManualReset = TRUE;
	constexpr BOOL					bInitialState = FALSE;
	constexpr LPCWSTR				lpName = NULL;

	myWriteOverlapped.hEvent = myWinPlatform.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (myWriteOverlapped.hEvent == NULL)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset(Err(SCloseError{ err == ERROR_NOT_ENOUGH_MEMORY ? ECloseError::OutOfMemory : ECloseError::InternalError, err }))();

		return Err(SConnectionError{ err == ERROR_NOT_ENOUGH_MEMORY ? EConnectionError::OutOfMemory : EConnectionError::InternalError, err });
	}

	myWriteOverlappedEvent = WinEventHandle(myWriteOverlapped.hEvent, myWinPlatform);

	myReadOverlapped.hEvent = myWinPlatform.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (myReadOverlapped.hEvent == NULL)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset(Err(SCloseError{ err == ERROR_NOT_ENOUGH_MEMORY ? ECloseError::OutOfMemory : ECloseError::InternalError, err }))();

		return Err(SConnectionError{ err == ERROR_NOT_ENOUGH_MEMORY ? EConnectionError::OutOfMemory : EConnectionError::InternalError, err });
	}

	myReadOverlappedEvent = WinEventHandle(myReadOverlapped.hEvent, myWinPlatform);

	myState = EState::Open;

	return Ok();
}

Closure WinIpcConnection::PerformWrite(std::vector<u8>&& aBuffer, Promise<ResultTypeWrite>&& aPromise)
{
	if (aBuffer.empty())
		return [promise = std::move(aPromise)]() mutable
		{
			promise.Complete(Err(SWriteError{ EWriteError::InvalidArgs }));
		};

	if (myState != EState::Open)
		return [promise = std::move(aPromise)]() mutable
		{
			promise.Complete(Err(SWriteError{ EWriteError::InvalidState }));
		};

	if (myCurrentlyWriting)
	{
		myWriteQueue.emplace(WriteData{ std::move(aBuffer), std::move(aPromise) });

		return locNoOp;
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
		auto closure = NextWrite();

		return[promise = std::move(aPromise), closure = std::move(closure)]() mutable
		{
			promise.Complete(Ok());

			std::move(closure)();
		};
	}

	const DWORD err = myWinPlatform.GetLastError();

	if (err == ERROR_IO_PENDING)
	{
		myCurrentlyWriting = true;
		myCurrentWritePromise.emplace(std::move(aPromise));

		return[&executor = myExecutor, &winEvent = myWinEvent, eventHandle = myWriteOverlappedEvent, callback = Bind(&WinIpcConnection::OnWrite, weak_from_this(), std::move(aBuffer))]() mutable
		{
			winEvent.RegisterEvent(std::move(eventHandle))
				.ThenRun(executor, [callback = std::move(callback)](auto&&) mutable
				{
					std::move(callback)();
				})
				.Detach();
		};
	}

	const EWriteError writeErr = WindowsErrorCodeToWriteError(err);
	Closure closure = locNoOp;

	switch (writeErr)
	{
	case EWriteError::InvalidArgs:	break;
	case EWriteError::InvalidState:	break;
	case EWriteError::StateChanged:
		closure = Reset(Ok(SCloseLocation{ ECloseLocation::ClosedRemotely, err }));

		break;
	case EWriteError::OutOfMemory:	break;
	case EWriteError::InternalError:
		closure = Reset(Err(SCloseError{ ECloseError::InternalError , err }));

		break;
	}

	return[closure = std::move(closure), promise = std::move(aPromise), writeErr]() mutable
	{
		std::move(closure)();

		promise.Complete(Err(SWriteError{ writeErr }));
	};
}

void WinIpcConnection::OnWrite(std::vector<u8> aBuffer)
{
	myMutex.lock();

	DWORD bytesWritten = 0;

	const BOOL result = myWinPlatform.GetOverlappedResult(myFileHandle, &myWriteOverlapped, &bytesWritten, FALSE);

	const DWORD err = myWinPlatform.GetLastError();

	myWinPlatform.ResetEvent(myWriteOverlapped.hEvent);

	if (!myCurrentWritePromise)
	{
		myMutex.unlock();

		NextWrite()();

		return;
	}

	auto writePromise = std::move(*myCurrentWritePromise);

	myCurrentlyWriting = false;
	myCurrentWritePromise.reset();

	if (!result)
	{
		const EWriteError writeErr = WindowsErrorCodeToWriteError(err);

		myMutex.unlock();

		switch (writeErr)
		{
		case EWriteError::InvalidArgs:	break;
		case EWriteError::InvalidState:	break;
		case EWriteError::StateChanged:
			Reset(Ok(SCloseLocation{ ECloseLocation::ClosedRemotely, err }))();

			break;
		case EWriteError::OutOfMemory:	break;
		case EWriteError::InternalError:
			Reset(Err(SCloseError{ ECloseError::InternalError , err }))();

			break;
		}

		writePromise.Complete(Err(SWriteError{ writeErr }));

		NextWrite()();

		return;
	}

	if (bytesWritten < aBuffer.size())
	{
		aBuffer.erase(aBuffer.begin(), aBuffer.begin() + bytesWritten);

		myMutex.unlock();

		PerformWrite(std::move(aBuffer), std::move(writePromise))();

		return;
	}

	myMutex.unlock();

	writePromise.Complete(Ok());

	NextWrite()();
}

Closure WinIpcConnection::NextWrite()
{
	if (myWriteQueue.empty())
		return locNoOp;

	auto writeData = std::move(myWriteQueue.front());

	myWriteQueue.pop();

	return PerformWrite(std::move(writeData.Buffer), std::move(writeData.Promise));
}

Closure WinIpcConnection::PerformRead(Promise<ResultTypeRead>&& aPromise, RequestIdType aRequestId)
{
	if (myState != EState::Open)
		return [promise = std::move(aPromise)]() mutable
		{
			promise.Complete(Err(SReadError{ EReadError::InvalidState }));
		};

	if (myCurrentlyReading)
	{
		myReadQueue.emplace_back(ReadData{ std::move(aPromise), aRequestId });

		return locNoOp;
	}

	if (!myReadBuffer.empty())
	{
		auto buffer = std::exchange(myReadBuffer, std::vector<u8>());

		return [promise = std::move(aPromise), buffer = std::move(buffer)]() mutable
		{
			promise.Complete(Ok(std::move(buffer)));
		};
	}

	std::vector<u8> readBuffer;

	readBuffer.resize(locReadBufferSize.To<EMemoryRatio::Bytes, size_t>(), 0);

	DWORD bytesRead = 0;

	const BOOL result = myWinPlatform.ReadFile(myFileHandle, static_cast<void*>(readBuffer.data()), static_cast<DWORD>(readBuffer.size()), &bytesRead, &myReadOverlapped);

	if (result)
	{
		readBuffer.resize(bytesRead);

		auto buffer = std::exchange(readBuffer, std::vector<u8>());

		return [promise = std::move(aPromise), buffer = std::move(buffer)]() mutable
		{
			promise.Complete(Ok(std::move(buffer)));
		};
	}

	const DWORD err = myWinPlatform.GetLastError();

	if (err == ERROR_IO_PENDING)
	{
		myCurrentlyReading = true;
		myReadQueue.emplace_back(ReadData{ std::move(aPromise), aRequestId });

		return [&executor = myExecutor, &winEvent = myWinEvent, eventHandle = myReadOverlappedEvent, callback = Bind(&WinIpcConnection::OnRead, weak_from_this(), std::move(readBuffer))]() mutable
		{
			winEvent.RegisterEvent(std::move(eventHandle))
				.ThenRun(executor, [callback = std::move(callback)](auto&&) mutable
					{
						std::move(callback)();
					})
				.Detach();
		};
	}

	const EReadError readErr = WindowsErrorCodeToReadError(err);
	Closure closure = locNoOp;

	switch (readErr)
	{
	case EReadError::InvalidState:	break;
	case EReadError::StateChanged:
		closure = Reset(Ok(SCloseLocation{ ECloseLocation::ClosedRemotely, err }));

		break;
	case EReadError::OutOfMemory:	break;
	case EReadError::MessageToBig:	break;
	case EReadError::InternalError:
		closure = Reset(Err(SCloseError{ ECloseError::InternalError , err }));
		
		break;
	}

	return [closure = std::move(closure), promise = std::move(aPromise), readErr]() mutable
	{
		std::move(closure)();

		promise.Complete(Err(SReadError{ readErr }));
	};
}

void WinIpcConnection::OnRead(std::vector<u8> aBuffer)
{
	myMutex.lock();

	DWORD bytesRead = 0;

	const BOOL readResult = myWinPlatform.GetOverlappedResult(myFileHandle, &myReadOverlapped, &bytesRead, FALSE);

	const DWORD err = myWinPlatform.GetLastError();

	myWinPlatform.ResetEvent(myReadOverlapped.hEvent);

	myCurrentlyReading = false;

	const EReadError readErr = WindowsErrorCodeToReadError(err);
	if (readResult)
		aBuffer.resize(bytesRead);

	auto completionResult = readResult
		? ResultTypeRead(Ok(std::move(aBuffer)))
		: ResultTypeRead(Err(SReadError{ readErr, err }));

	while (!myReadQueue.empty())
	{
		auto readData = std::move(myReadQueue.front());

		myReadQueue.pop_front();
		BufferExecutor executor;

		if (!readResult)
		{
			if (!readData.Promise.Complete(executor, std::move(completionResult)))
				continue;

			myMutex.unlock();

			switch (readErr)
			{
			case EReadError::InvalidState:	break;
			case EReadError::StateChanged:
				Reset(Ok(SCloseLocation{ ECloseLocation::ClosedRemotely, err }))();

				break;
			case EReadError::OutOfMemory:	break;
			case EReadError::MessageToBig:	break;
			case EReadError::InternalError:
				Reset(Err(SCloseError{ ECloseError::InternalError , err }))();

				break;
			}

			executor.ExecuteAll();

			return;
		}

		if (!readData.Promise.Complete(executor, std::move(completionResult)))
			continue;

		myMutex.unlock();

		executor.ExecuteAll();

		auto closure = NextRead();

		std::move(closure)();

		return;
	}

	const auto& buffer = completionResult.Unwrap();

	if (readResult)
		myReadBuffer.insert(myReadBuffer.end(), buffer.begin(), buffer.end());

	myMutex.unlock();
}

Closure WinIpcConnection::NextRead()
{
	std::lock_guard lock(myMutex);

	if (myReadQueue.empty())
		return locNoOp;

	auto readData = std::move(myReadQueue.front());

	myReadQueue.pop_front();

	return PerformRead(std::move(readData.Promise), readData.RequestId);
}

void WinIpcConnection::CancelRead(RequestIdType aRequestId)
{
	std::lock_guard lock(myMutex);

	const auto pred = [aRequestId](const auto& aReadRequest)
	{
		return aReadRequest.RequestId == aRequestId;
	};

	const auto it = std::find_if(myReadQueue.begin(), myReadQueue.end(), pred);

	if (it == myReadQueue.end())
		return;

	myReadQueue.erase(it);
}

Closure WinIpcConnection::Reset(Result<SCloseLocation, SCloseError> aResult)
{
	auto currentWritePromise	= std::exchange(myCurrentWritePromise, std::nullopt);
	auto writeQueue				= std::exchange(myWriteQueue, decltype(myWriteQueue)());
	auto readQueue				= std::exchange(myReadQueue, decltype(myReadQueue)());
	auto closePromises			= std::exchange(myClosePromises, decltype(myClosePromises)());

	if (myFileHandle != INVALID_HANDLE_VALUE)
		(void)myWinPlatform.CloseHandle(myFileHandle);
	myReadOverlappedEvent = WinEventHandle();
	myWriteOverlappedEvent = WinEventHandle();

	myState = EState::Closed;
	myFileHandle = INVALID_HANDLE_VALUE;

	myCurrentlyWriting = false;
	myCurrentlyReading = false;
	myCurrentWritePromise.reset();
	myWriteOverlapped = {};

	myReadOverlapped = {};

	return [currentWritePromise = std::move(currentWritePromise), writeQueue = std::move(writeQueue), readQueue = std::move(readQueue), closePromises = std::move(closePromises), aResult]() mutable
	{
		if (currentWritePromise)
			currentWritePromise->Complete(Err(SWriteError{ EWriteError::StateChanged }));

		while (!writeQueue.empty())
		{
			writeQueue.front().Promise.Complete(Err(SWriteError{ EWriteError::StateChanged }));

			writeQueue.pop();
		}
		for (auto& readData : readQueue)
			readData.Promise.Complete(Err(SReadError{ EReadError::StateChanged }));

		for (auto& closePromise : closePromises)
			closePromise.Complete(aResult);
	};
}
