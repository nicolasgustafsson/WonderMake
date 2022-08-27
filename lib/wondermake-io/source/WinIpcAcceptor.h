#pragma once

#include "wondermake-utility/WinPlatform.h"

#include "wondermake-io/IpcAcceptor.h"

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

	Result<void, SOpenError> Open(std::string aName, CallbackInfo&& aCallbackInfo) override;
	void Close() override;

	EState GetState() const noexcept override;

private:
	Result<void, SOpenError> ListenForConnection();
	Result<void, SOpenError> OnConnection();

	void Reset(Result<void, SCloseError> aResult);

	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;

	EState myState = EState::Closed;

	std::wstring myPipeName;

	HANDLE myPipeHandle = INVALID_HANDLE_VALUE;
	OVERLAPPED myPipeOverlapped = {};

	CallbackInfo myCallbackInfo;

};
