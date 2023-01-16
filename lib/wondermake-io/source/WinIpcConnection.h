#pragma once

#include "wondermake-io/IpcConnection.h"

#include "wondermake-base/WinEventHandle.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/UniqueFunction.h"
#include "wondermake-utility/WinPlatform.h"

#include <atomic>
#include <deque>
#include <mutex>
#include <optional>
#include <queue>
#include <vector>

class WinEventSystem;
class WinPlatformSystem;

class WinIpcConnection final
	: public IpcConnection
	, public std::enable_shared_from_this<WinIpcConnection>
{
public:
	WinIpcConnection(AnyExecutor aExecutor, WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept;
	~WinIpcConnection() noexcept;

	Result<void, SConnectionError> Connect(std::string aConnectionName) override;
	Result<void, SConnectionError> ConnectHandle(HANDLE aPipeHandle);

	FutureTypeWrite		Write(std::vector<u8> aBuffer) override;
	FutureTypeRead		Read() override;
	FutureTypeClose		OnClose() override;
	FutureTypeClose		Close() override;

	EState				GetState() const noexcept override;

private:
	// Having the RequestId's type be that of a pointer guarantees that
	// the application runs out of memory before it runs out of ids.
	using RequestIdType = std::uintptr_t;

	struct WriteData
	{
		std::vector<u8>				Buffer;
		Promise<ResultTypeWrite>	Promise;
	};
	struct ReadData
	{
		Promise<ResultTypeRead>		Promise;
		RequestIdType				RequestId;
	};

	Result<void, SConnectionError> Setup();
	
	[[nodiscard]] Closure PerformWrite(std::vector<u8>&& aBuffer, Promise<ResultTypeWrite>&& aPromise);
	void OnWrite(std::vector<u8> aBuffer);
	[[nodiscard]] Closure NextWrite();

	[[nodiscard]] Closure PerformRead(Promise<ResultTypeRead>&& aPromise, RequestIdType aRequestId);
	void OnRead(std::vector<u8> aBuffer);
	[[nodiscard]] Closure NextRead();
	void CancelRead(RequestIdType aRequestId);

	[[nodiscard]] Closure Reset(Result<SCloseLocation, SCloseError> aResult);

	std::mutex myMutex;

	AnyExecutor myExecutor;
	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;
	
	std::atomic<EState> myState = EState::Closed;
	HANDLE myFileHandle = INVALID_HANDLE_VALUE;

	bool									myCurrentlyWriting = false;
	std::optional<Promise<ResultTypeWrite>>	myCurrentWritePromise;
	OVERLAPPED								myWriteOverlapped = {};
	WinEventHandle							myWriteOverlappedEvent;
	std::queue<WriteData>					myWriteQueue;

	bool					myCurrentlyReading = false;
	std::vector<u8>			myReadBuffer;
	OVERLAPPED				myReadOverlapped = {};
	WinEventHandle			myReadOverlappedEvent;
	std::deque<ReadData>	myReadQueue;

	std::vector<Promise<ResultTypeClose>> myClosePromises;

};
