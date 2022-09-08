#include "WinProcess.h"

#include "wondermake-base/WinEventSystem.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/Bindable.h"

WinProcess::WinProcess(AnyExecutor aExecutor, WinEventSystem& aWinEvent, WinPlatformSystem& aWinPlatform, HANDLE aProcessHandle, HANDLE aThreadHandle) noexcept
	: myExecutor(std::move(aExecutor))
	, myWinEvent(aWinEvent)
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
	myCloseFuture = myWinEvent.RegisterEvent(myProcessHandle)
		.ThenRun(myExecutor, [callable = Bind(&WinProcess::OnClose, weak_from_this())](auto&&) mutable
		{
			std::move(callable)();
		});
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

	Reset(Err(SExitError{ EExitError::Terminated, aExitCode }));
}

Future<Result<i64, Process::SExitError>> WinProcess::OnExit()
{
	if (myState != EState::Running)
		return MakeCompletedFuture<Result<i64, Process::SExitError>>(Err(SExitError{ EExitError::AlreadyStopped }));
	
	auto [promise, future] = MakeAsync<Result<i64, Process::SExitError>>();

	myOnExitPromises.emplace_back(std::move(promise));

	return future;
}

void WinProcess::OnClose()
{
	if (myState != EState::Running)
		return;

	DWORD exitCode = 0;

	const BOOL result = myWinPlatform.GetExitCodeProcess(myProcessHandle, &exitCode);

	if (result)
		Reset(Ok(exitCode));
	else
	{
		const auto err = myWinPlatform.GetLastError();

		Reset(Err(SExitError{ EExitError::InternalError, err }));
	}
}

void WinProcess::Reset(Result<i64, SExitError> aResult)
{
	myState = EState::Stopped;
	myCloseFuture.Reset();

	if (myProcessHandle)
		(void)myWinPlatform.CloseHandle(myProcessHandle);
	if (myThreadHandle)
		(void)myWinPlatform.CloseHandle(myThreadHandle);

	myProcessHandle = NULL;
	myThreadHandle = NULL;

	auto onExitPromises = std::exchange(myOnExitPromises, decltype(myOnExitPromises)());

	for (auto&& promise : onExitPromises)
		promise.Complete(aResult);
}
