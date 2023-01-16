#pragma once

#include "wondermake-base/Process.h"
#include "wondermake-base/WinEventHandle.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/WinPlatform.h"

#include <memory>

class WinEventSystem;
class WinPlatformSystem;

class WinProcess final
	: public Process
	, public std::enable_shared_from_this<WinProcess>
{
public:
	WinProcess(AnyExecutor aExecutor, WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform, WinEventHandle aProcessHandle, HANDLE aThreadHandle) noexcept;
	~WinProcess();

	void Initialize();

	EState GetState() const override;
	
	void Terminate(i64 aExitCode) override;

	Future<Result<i64, SExitError>> OnExit() override;

private:
	void OnClose();

	void Reset(Result<i64, SExitError> aResult);

	AnyExecutor myExecutor;
	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;

	WinEventHandle myProcessHandle;
	HANDLE myThreadHandle = NULL;

	EState myState = EState::Running;
	Future<void> myCloseFuture;

	std::vector<Promise<Result<i64, SExitError>>> myOnExitPromises;

};
