#pragma once

#include "WonderMakeEngine/WinPlatform.h"

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

	Result<OpenError> Open(std::string aName, CallbackInfo&& aCallbackInfo) override;
	void Close() override;

	State GetState() const noexcept override;

private:
	Result<OpenError> ListenForConnection();
	void OnConnection();

	void Reset(Result<CloseReason> aResult);

	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;

	State myState = State::Closed;
	bool myIsListening = false;

	std::wstring myPipeName;

	HANDLE myPipeHandle = INVALID_HANDLE_VALUE;
	OVERLAPPED myPipeOverlapped = {};

	CallbackInfo myCallbackInfo;

};
