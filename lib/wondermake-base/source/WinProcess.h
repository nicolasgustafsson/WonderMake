#pragma once

#include "wondermake-base/Process.h"

#include "wondermake-utility/WinPlatform.h"

#include <memory>

class WinEventSystem;
class WinPlatformSystem;

class WinProcess final
	: public Process
	, public std::enable_shared_from_this<WinProcess>
{
public:
	WinProcess(WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform, HANDLE aProcessHandle, HANDLE aThreadHandle) noexcept;
	~WinProcess();

	void Initialize();

	EState GetState() const override;
	
	void Terminate(i64 aExitCode) override;

	void OnExit(OnExitCallback&& aOnExit) override;

private:
	void OnClose();

	void Reset(Result<i64, SExitError> aResult);

	WinEventSystem& myWinEvent;
	WinPlatformSystem& myWinPlatform;

	HANDLE myProcessHandle = NULL;
	HANDLE myThreadHandle = NULL;

	EState myState = EState::Running;

	std::vector<OnExitCallback> myOnExitCallbacks;

};
