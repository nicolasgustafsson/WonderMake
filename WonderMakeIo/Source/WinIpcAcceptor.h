#pragma once

#include "WonderMakeBase/WinPlatform.h"

#include "WonderMakeIo/IpcAcceptor.h"

#include <optional>
#include <vector>

class WinEventSystem;
class WinPlatformSystem;

class WinIpcAcceptor final
	: public IpcAcceptor
	, public std::enable_shared_from_this<WinIpcAcceptor>
{
public:
	WinIpcAcceptor(WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept;
	~WinIpcAcceptor() noexcept;

	Result<EOpenError> Open(std::string aName, CallbackInfo&& aCallbackInfo) override;
	void Close() override;

	EState GetState() const noexcept override;

private:
	Result<EOpenError> ListenForConnection();
	Result<EOpenError> OnConnection();

	void Reset(Result<ECloseReason> aResult);

	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;

	EState myState = EState::Closed;

	std::wstring myPipeName;

	HANDLE myPipeHandle = INVALID_HANDLE_VALUE;
	OVERLAPPED myPipeOverlapped = {};

	CallbackInfo myCallbackInfo;

};
