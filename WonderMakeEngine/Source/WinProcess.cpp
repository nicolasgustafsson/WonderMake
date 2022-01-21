#include "WinProcess.h"

#include "WonderMakeEngine/WinEventSystem.h"
#include "WonderMakeEngine/WinPlatformSystem.h"

#include "WonderMakeBase/Bindable.h"

WinProcess::WinProcess(WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform, HANDLE aProcessHandle, HANDLE aThreadHandle) noexcept
	: myWinEvent(aWinEvent)
	, myWinPlatform(aWinPlatform)
	, myProcessHandle(aProcessHandle)
	, myThreadHandle(aThreadHandle)
{}
WinProcess::~WinProcess()
{
	Terminate(0);
}

void WinProcess::Initialize()
{
	myWinEvent.RegisterEvent(myProcessHandle, Bind(&WinProcess::OnClose, weak_from_this()));
}

Process::EState WinProcess::GetState() const
{
	return myState;
}
	
void WinProcess::Terminate(i64 aExitCode)
{
	if (myState == EState::Stopped)
		return;

	if (myProcessHandle)
		(void)myWinPlatform.TerminateProcess(myProcessHandle, static_cast<UINT>(aExitCode));

	Reset({ EExitError::Terminated, aExitCode });
}

void WinProcess::OnExit(OnExitCallback&& aOnExit)
{
	if (myState == EState::Running)
	{
		myOnExitCallbacks.emplace_back(std::move(aOnExit));

		return;
	}

	std::move(aOnExit)(EExitError::AlreadyStopped);
}

void WinProcess::OnClose()
{
	if (myState != EState::Running)
		return;

	DWORD exitCode = 0;

	const BOOL result = myWinPlatform.GetExitCodeProcess(myProcessHandle, &exitCode);

	if (result)
		Reset(exitCode);
	else
	{
		const auto err = myWinPlatform.GetLastError();

		Reset({ EExitError::InternalError, err });
	}
}

void WinProcess::Reset(Result<EExitError, i64, i64> aResult)
{
	myState = EState::Stopped;

	if (myProcessHandle)
	{
		myWinEvent.UnregisterEvent(myProcessHandle);

		(void)myWinPlatform.CloseHandle(myProcessHandle);
	}
	if (myThreadHandle)
		(void)myWinPlatform.CloseHandle(myThreadHandle);

	myProcessHandle = NULL;
	myThreadHandle = NULL;

	decltype(myOnExitCallbacks) onExitCallbacks;

	std::swap(myOnExitCallbacks, onExitCallbacks);

	for (auto&& onClose : onExitCallbacks)
		std::move(onClose)(aResult);
}
