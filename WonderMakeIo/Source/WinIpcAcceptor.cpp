#include "WinIpcAcceptor.h"

#include "WonderMakeEngine/WinEventSystem.h"
#include "WonderMakeEngine/WinPlatformSystem.h"

#include "WonderMakeBase/Bindable.h"
#include "WonderMakeBase/MemoryUnit.h"
#include "WonderMakeBase/Utility.h"

#include "WinIpcConnection.h"

using namespace MemoryUnitLiterals;

constexpr auto locPipePrefix = L"\\\\.\\pipe\\";
constexpr auto locReadBufferSize = 4_KiB;

WinIpcAcceptor::WinIpcAcceptor(WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept
	: myWinEvent(aWinEvent)
	, myWinPlatform(aWinPlatform)
{}

WinIpcAcceptor::~WinIpcAcceptor() noexcept
{
	Close();
}

Result<IpcAcceptor::EOpenError> WinIpcAcceptor::Open(std::string aName, CallbackInfo&& aCallbackInfo)
{
	if (aName.empty())
		return Result(IpcAcceptor::EOpenError::InvalidArgs);

	if (myState == EState::Open)
		return Result(IpcAcceptor::EOpenError::InvalidState);

	myPipeName = locPipePrefix + std::wstring(aName.begin(), aName.end());
	myCallbackInfo = std::move(aCallbackInfo);

	constexpr LPSECURITY_ATTRIBUTES	lpEventAttributes = NULL;
	constexpr BOOL					bManualReset = TRUE;
	constexpr BOOL					bInitialState = FALSE;
	constexpr LPCWSTR				lpName = NULL;

	myPipeOverlapped.hEvent = myWinPlatform.CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (myPipeOverlapped.hEvent == NULL)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset(Result(ECloseReason::InternalError, err));

		return Result(EOpenError::InternalError, err);
	}

	myState = EState::Open;

	return ListenForConnection();
}

void WinIpcAcceptor::Close()
{
	Reset(Success);
}

WinIpcAcceptor::EState WinIpcAcceptor::GetState() const noexcept
{
	return myState;
}

Result<IpcAcceptor::EOpenError> WinIpcAcceptor::ListenForConnection()
{
	const		LPCWSTR					lpName = myPipeName.c_str();
	constexpr	DWORD					dwOpenMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
	constexpr	DWORD					dwPipeMode = PIPE_TYPE_BYTE;
	constexpr	DWORD					nMaxInstances = PIPE_UNLIMITED_INSTANCES;
	constexpr	DWORD					nOutBufferSize = locReadBufferSize.To<EMemoryRatio::Bytes, DWORD>();
	constexpr	DWORD					nInBufferSize = locReadBufferSize.To<EMemoryRatio::Bytes, DWORD>();
	constexpr	DWORD					nDefaultTimeOut = NMPWAIT_USE_DEFAULT_WAIT;
	constexpr	LPSECURITY_ATTRIBUTES	lpSecurityAttributes = NULL;

	if (myState != EState::Open)
		return IpcAcceptor::EOpenError::InvalidState;

	if (myIsListening)
		return IpcAcceptor::EOpenError::InternalError;

	myIsListening = true;

	myPipeHandle = myWinPlatform.CreateNamedPipeW(
		lpName,
		dwOpenMode,
		dwPipeMode,
		nMaxInstances,
		nOutBufferSize,
		nInBufferSize,
		nDefaultTimeOut,
		lpSecurityAttributes);

	if (myPipeHandle == INVALID_HANDLE_VALUE)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset(Result(ECloseReason::InternalError, err));

		return Result(EOpenError::InternalError, err);
	}

	const BOOL result = myWinPlatform.ConnectNamedPipe(myPipeHandle, &myPipeOverlapped);

	if (result == TRUE)
	{
		const DWORD err = myWinPlatform.GetLastError();

		Reset(Result(ECloseReason::InternalError, err));

		return Result(EOpenError::InternalError, err);
	}

	const DWORD err = myWinPlatform.GetLastError();

	if (err == ERROR_PIPE_CONNECTED)
	{
		return OnConnection();
	}
	
	myWinEvent.RegisterEvent(myPipeOverlapped.hEvent, Bind(&WinIpcAcceptor::OnConnection, weak_from_this()));

	if (err != ERROR_IO_PENDING)
	{
		Reset(Result(ECloseReason::InternalError, err));

		return Result(IpcAcceptor::EOpenError::InternalError, err);
	}

	return Success;
}

Result<IpcAcceptor::EOpenError> WinIpcAcceptor::OnConnection()
{
	std::shared_ptr<WinIpcConnection> connection;

	try
	{
		connection = std::make_shared<WinIpcConnection>(myWinEvent, myWinPlatform);
	}
	catch (const std::bad_alloc&)
	{
		Reset(ECloseReason::OutOfMemory);

		return EOpenError::OutOfMemory;
	}

	connection->ConnectHandle(myPipeHandle);
	myWinPlatform.ResetEvent(myPipeOverlapped.hEvent);

	myPipeHandle = INVALID_HANDLE_VALUE;

	Utility::Invoke(myCallbackInfo.OnConnection, std::move(connection));

	const auto result = ListenForConnection();

	if (result)
		return Success;

	switch (static_cast<EOpenError>(result))
	{
	case EOpenError::InvalidArgs:		break;
	case EOpenError::InvalidState:		return Success;
	case EOpenError::OutOfMemory:		break;
	case EOpenError::InternalError:		break;
	}

	return result;
}

void WinIpcAcceptor::Reset(Result<ECloseReason> aResult)
{
	auto onClose = std::move(myCallbackInfo.OnClose);
	if (myPipeHandle != INVALID_HANDLE_VALUE)
		(void)myWinPlatform.CloseHandle(myPipeHandle);
	if (myPipeOverlapped.hEvent != NULL)
	{
		myWinEvent.UnregisterEvent(myPipeOverlapped.hEvent);

		(void)myWinPlatform.CloseHandle(myPipeOverlapped.hEvent);

		myPipeOverlapped.hEvent = NULL;
	}

	myCallbackInfo = {};
	myState = EState::Closed;
	myIsListening = false;
	myPipeHandle = INVALID_HANDLE_VALUE;
	myPipeOverlapped = {};

	std::move(onClose)(aResult);
}
