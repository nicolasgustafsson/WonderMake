#pragma once

#include "wondermake-utility/WinPlatform.h"

#include "wondermake-io/IpcConnection.h"

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

	Result<void, SConnectionError> Connect(std::string aConnectionName) override;
	Result<void, SConnectionError> ConnectHandle(HANDLE aPipeHandle);

	Result<EAsynchronicity, SWriteError> Write(std::vector<u8> aBuffer, OnWriteCallback aOnWrite) override;
	Result<EAsynchronicity, SReadError> Read(OnReadCallback aOnRead) override;
	void OnClose(OnCloseCallback aOnClose) override;
	void Close() override;

	EState GetState() const noexcept override;

private:
	struct WriteData
	{
		std::vector<u8> Buffer;
		OnWriteCallback OnWrite;
	};

	Result<void, SConnectionError> Setup();
	
	void OnWrite(std::vector<u8> aBuffer);
	void NextWrite();
	void OnRead(std::vector<u8> aBuffer);
	void NextRead();

	void Reset(Result<SCloseLocation, SCloseError> aResult);

	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;
	
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

	std::vector<OnCloseCallback> myCloseCallbacks;

};
