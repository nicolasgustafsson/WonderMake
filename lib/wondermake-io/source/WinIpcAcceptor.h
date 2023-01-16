#pragma once

#include "wondermake-io/IpcAcceptor.h"

#include "wondermake-base/WinEventHandle.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/WinPlatform.h"

#include <atomic>
#include <deque>
#include <mutex>
#include <vector>

class WinEventSystem;
class WinPlatformSystem;

class WinIpcAcceptor final
	: public IpcAcceptor
	, public std::enable_shared_from_this<WinIpcAcceptor>
{
public:
	WinIpcAcceptor(AnyExecutor aExecutor, WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform) noexcept;
	~WinIpcAcceptor() noexcept;

	ResultTypeOpen			Open(std::string aName) override;
	FutureTypeConnection	OnConnection() override;
	FutureTypeClose			OnClose() override;
	void					Close() override;

	EState					GetState() const noexcept override;

private:
	// Having the RequestId's type be that of a pointer guarantees that
	// the application runs out of memory before it runs out of ids.
	using RequestIdType = std::uintptr_t;

	struct SOnConnectionData
	{
		Promise<ResultTypeConnection>	Promise;
		RequestIdType					RequestId;
	};

	[[nodiscard]] Closure	ListenForConnection();
	void					OnNewConnection();
	ResultTypeConnection	CreateIpcConnection();
	void					CancelOnConnection(RequestIdType aRequestId);

	[[nodiscard]] Closure	Reset();

	std::mutex				 myMutex;

	AnyExecutor				myExecutor;
	WinEventSystem&			myWinEvent;
	WinPlatformSystem&		myWinPlatform;

	std::wstring			myPipeName;
	std::atomic<EState>		myState = EState::Closed;

	Future<void>			myPipeFuture;
	HANDLE					myPipeHandle = INVALID_HANDLE_VALUE;
	OVERLAPPED				myPipeOverlapped = {};
	WinEventHandle			myPipeOverlappedEvent;

	std::deque<SOnConnectionData>	myOnConnectionQueue;
	std::vector<Promise<void>>		myOnClosePromises;

};
