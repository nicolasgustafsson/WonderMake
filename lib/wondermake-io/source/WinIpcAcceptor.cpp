#include "WinIpcAcceptor.h"

#include "wondermake-base/WinEventSystem.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/BufferExecutor.h"
#include "wondermake-utility/MemoryUnit.h"
#include "wondermake-utility/Utility.h"

#include "WinIpcConnection.h"

using namespace MemoryUnitLiterals;

inline constexpr auto locPipePrefix = L"\\\\.\\pipe\\";
inline constexpr auto locReadBufferSize = 4_KiB;

inline constexpr auto locNoOp = []() {};

WinIpcAcceptor::WinIpcAcceptor(AnyExecutor aExecutor, WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept
	: myExecutor(std::move(aExecutor))
	, myWinEvent(aWinEvent)
	, myWinPlatform(aWinPlatform)
{}

WinIpcAcceptor::~WinIpcAcceptor() noexcept
{
	Close();
}

IpcAcceptor::ResultTypeOpen WinIpcAcceptor::Open(std::string aName)
{
	std::lock_guard lock(myMutex);

	if (aName.empty())
		return Err(SOpenError{ IpcAcceptor::EOpenError::InvalidArgs });

	if (myState == EState::Open)
		return Err(SOpenError{ IpcAcceptor::EOpenError::InvalidState });

	myPipeName = locPipePrefix + std::wstring(aName.begin(), aName.end());

	constexpr LPSECURITY_ATTRIBUTES	lpEventAttributes = NULL;
	constexpr BOOL					bManualReset = TRUE;
	constexpr BOOL					bInitialState = FALSE;
	constexpr LPCWSTR				lpName = NULL;

	myPipeOverlapped.hEvent = myWinPlatform.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (myPipeOverlapped.hEvent == NULL)
	{
		const DWORD err = myWinPlatform.GetLastError();

		(void)Reset();

		return Err(SOpenError{ EOpenError::InternalError, err });
	}

	myState = EState::Open;

	return Ok();
}

IpcAcceptor::FutureTypeConnection WinIpcAcceptor::OnConnection()
{
	auto [promise, future] = MakeAsync<IpcAcceptor::ResultTypeConnection>();

	auto perform = [this, &promise, &future]()
	{
		const auto getNextRequestId = [&requestQueue = myOnConnectionQueue]() -> RequestIdType
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

		std::lock_guard lock(myMutex);

		const auto requestId = getNextRequestId();

		myOnConnectionQueue.push_back(SOnConnectionData{ std::move(promise), getNextRequestId() });

		future.OnCancel(myExecutor, Bind(&WinIpcAcceptor::CancelOnConnection, weak_from_this(), requestId));

		return ListenForConnection();
	};

	perform()();

	return future;
}

IpcAcceptor::FutureTypeClose WinIpcAcceptor::OnClose()
{
	auto [promise, future] = MakeAsync<void>();

	std::lock_guard lock(myMutex);

	if (myState != EState::Closed)
		myOnClosePromises.emplace_back(std::move(promise));

	return future;
}

void WinIpcAcceptor::Close()
{
	auto perform = [this]()
	{
		std::lock_guard lock(myMutex);

		return Reset();
	};

	perform()();
}

WinIpcAcceptor::EState WinIpcAcceptor::GetState() const noexcept
{
	return myState;
}

[[nodiscard]] Closure WinIpcAcceptor::ListenForConnection()
{
	if (myPipeFuture.IsValid())
		return locNoOp;

	const auto completeNextPromise = [this](auto&& aResult)
	{
		BufferExecutor executor;

		while (!myOnConnectionQueue.empty())
		{
			auto requestData = std::move(myOnConnectionQueue.front());

			myOnConnectionQueue.pop_front();

			if (requestData.Promise.Complete(executor, std::move(aResult)))
				break;
		}

		return[executor = std::move(executor)]() mutable
		{
			executor.ExecuteAll();
		};
	};
	

	if (myState != EState::Open)
		return completeNextPromise(Err(SConnectionError{ EConnectionError::InvalidState }));

	const		LPCWSTR					lpName					= myPipeName.c_str();
	constexpr	DWORD					dwOpenMode				= PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
	constexpr	DWORD					dwPipeMode				= PIPE_TYPE_BYTE;
	constexpr	DWORD					nMaxInstances			= PIPE_UNLIMITED_INSTANCES;
	constexpr	DWORD					nOutBufferSize			= locReadBufferSize.To<EMemoryRatio::Bytes, DWORD>();
	constexpr	DWORD					nInBufferSize			= locReadBufferSize.To<EMemoryRatio::Bytes, DWORD>();
	constexpr	DWORD					nDefaultTimeOut			= NMPWAIT_USE_DEFAULT_WAIT;
	constexpr	LPSECURITY_ATTRIBUTES	lpSecurityAttributes	= NULL;

	myPipeHandle = myWinPlatform.CreateNamedPipeW(
		lpName,
		dwOpenMode,
		dwPipeMode,
		nMaxInstances,
		nOutBufferSize,
		nInBufferSize,
		nDefaultTimeOut,
		lpSecurityAttributes);

	constexpr auto winErrorToConnectionError = [](DWORD aError)
	{
		return aError == ERROR_NOT_ENOUGH_MEMORY
			? SConnectionError{ EConnectionError::OutOfMemory, aError }
			: SConnectionError{ EConnectionError::InternalError, aError };
	};

	if (myPipeHandle == INVALID_HANDLE_VALUE)
		return completeNextPromise(Err(winErrorToConnectionError(myWinPlatform.GetLastError())));

	(void)myWinPlatform.ConnectNamedPipe(myPipeHandle, &myPipeOverlapped);

	const DWORD err = myWinPlatform.GetLastError();

	if (err == ERROR_PIPE_CONNECTED)
		return completeNextPromise(CreateIpcConnection());

	if (err != ERROR_IO_PENDING)
		return completeNextPromise(Err(winErrorToConnectionError(err)));

	myPipeFuture = myWinEvent.RegisterEvent(myPipeOverlapped.hEvent)
		.ThenRun(myExecutor, [callback = Bind(&WinIpcAcceptor::OnNewConnection, weak_from_this())](auto&&) mutable
			{
				std::move(callback)();
			});

	return locNoOp;
}

void WinIpcAcceptor::OnNewConnection()
{
	const auto perform = [this]() -> Closure
	{
		std::lock_guard lock(myMutex);

		myPipeFuture.Reset();
		myWinPlatform.ResetEvent(myPipeOverlapped.hEvent);

		BufferExecutor executor;
		auto result = CreateIpcConnection();

		while (!myOnConnectionQueue.empty())
		{
			auto requestData = std::move(myOnConnectionQueue.front());

			myOnConnectionQueue.pop_front();

			if (requestData.Promise.Complete(executor, std::move(result)))
				break;
		}

		if (myOnConnectionQueue.empty())
			return[executor = std::move(executor)]() mutable
			{
				executor.ExecuteAll();
			};

		auto listenClosure = ListenForConnection();

		return[executor = std::move(executor), listenClosure = std::move(listenClosure)]() mutable
		{
			executor.ExecuteAll();

			std::move(listenClosure)();
		};
	};

	perform()();
}

IpcAcceptor::ResultTypeConnection WinIpcAcceptor::CreateIpcConnection()
{
	auto connection = MakeSharedReference<WinIpcConnection>(myExecutor, myWinEvent, myWinPlatform);

	auto connectResult = connection->ConnectHandle(myPipeHandle);

	myPipeHandle = INVALID_HANDLE_VALUE;

	if (!connectResult)
	{
		static constexpr auto ConnectionErrorToAcceptorError = [](const auto aError)
		{
			switch (aError)
			{
			case IpcConnection::EConnectionError::InvalidArgs:		return EConnectionError::InternalError;
			case IpcConnection::EConnectionError::InvalidState:		return EConnectionError::InternalError;
			case IpcConnection::EConnectionError::NoConnection:		return EConnectionError::InternalError;
			case IpcConnection::EConnectionError::OutOfMemory:		return EConnectionError::OutOfMemory;
			case IpcConnection::EConnectionError::InternalError:	return EConnectionError::InternalError;
			}

			return EConnectionError::InternalError;
		};

		const auto error = ConnectionErrorToAcceptorError(connectResult.Err().Error);
		const auto reason = connectResult.Err().Reason;

		return Err(SConnectionError{ error, reason });
	}

	return Ok(std::move(connection));
}

void WinIpcAcceptor::CancelOnConnection(RequestIdType aRequestId)
{
	std::lock_guard lock(myMutex);

	const auto pred = [aRequestId](const auto& aReadRequest)
	{
		return aReadRequest.RequestId == aRequestId;
	};

	const auto it = std::find_if(myOnConnectionQueue.begin(), myOnConnectionQueue.end(), pred);

	if (it == myOnConnectionQueue.end())
		return;

	myOnConnectionQueue.erase(it);

	if (!myOnConnectionQueue.empty())
		return;

	myPipeFuture.Reset();
	if (myPipeOverlapped.hEvent)
		myWinPlatform.ResetEvent(myPipeOverlapped.hEvent);
}

Closure WinIpcAcceptor::Reset()
{
	auto onConnectionPromises = std::exchange(myOnConnectionQueue, decltype(myOnConnectionQueue)());
	auto onClosePromises = std::exchange(myOnClosePromises, decltype(myOnClosePromises)());

	if (myPipeHandle != INVALID_HANDLE_VALUE)
		(void)myWinPlatform.CloseHandle(myPipeHandle);
	if (myPipeOverlapped.hEvent != NULL)
	{
		(void)myWinPlatform.CloseHandle(myPipeOverlapped.hEvent);

		myPipeOverlapped.hEvent = NULL;
	}

	myPipeName = L"";
	myState = EState::Closed;

	myPipeFuture.Reset();
	myPipeHandle = INVALID_HANDLE_VALUE;
	myPipeOverlapped = {};

	return[onConnectionPromises = std::move(onConnectionPromises), onClosePromises = std::move(onClosePromises)]() mutable
	{
		onConnectionPromises.clear();

		for (auto& onClose : onClosePromises)
			onClose.Complete();
	};
}
