#pragma once

#include "WonderMakeEngine/WinPlatform.h"

#include "WonderMakeIo/IpcConnection.h"

#include <queue>
#include <vector>

class WinEventSystem;
class WinPlatformSystem;

class WinIpcConnection final
	: public IpcConnection
	, public std::enable_shared_from_this<WinIpcConnection>
{
public:
	WinIpcConnection(WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept;
	~WinIpcConnection() noexcept;

	Result<ConnectionError> Connect(std::string aConnectionName, CallbackInfo&& aCallbackInfo) override;
	Result<ConnectionError> ConnectHandle(HANDLE aPipeHandle, CallbackInfo&& aCallbackInfo);

	Result<EWriteError, EAsynchronicity> Write(std::vector<u8> aBuffer, OnWriteCallback&& aOnWrite) override;
	Result<EReadError, EAsynchronicity> Read(OnReadCallback&& aOnRead) override;
	void Close() override;

	EState GetState() const noexcept override;

private:
	struct WriteData
	{
		std::vector<u8> Buffer;
		OnWriteCallback OnWrite;
	};

	Result<ConnectionError> Setup();
	
	void OnWrite(std::vector<u8> aBuffer);
	void NextWrite();
	void OnRead(std::vector<u8> aBuffer);
	void NextRead();

	void Reset(Result<CloseError, CloseReason> aResult);

	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;
	
	CallbackInfo myCallbackInfo;

	EState myState = EState::Closed;
	HANDLE myFileHandle = INVALID_HANDLE_VALUE;

	bool myCurrentlyWriting = false;
	OnWriteCallback myCurrentWriteCallback = [](auto&&) {};
	OVERLAPPED myWriteOverlapped = {};
	std::queue<WriteData> myWriteQueue;

	bool myCurrentlyReading = false;
	OnReadCallback myCurrentReadCallback = [](auto&&) {};
	OVERLAPPED myReadOverlapped = {};
	std::queue<OnReadCallback> myReadQueue;

};
