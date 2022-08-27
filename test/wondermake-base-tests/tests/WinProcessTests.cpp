#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/Process.h"

#include "WonderMakeBaseTests/WinEventSystemMock.h"
#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WinProcess.h"

inline std::ostream& operator<<(std::ostream& out, const Process::SExitError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}

MATCHER_P(ExitResultMatcher, aResult, "")
{
	Result<i64, Process::SExitError> argResult = aResult;

	if (arg && argResult)
		return ExplainMatchResult(
			Eq(argResult.Unwrap()),
			arg.Unwrap(), result_listener);

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Process::SExitError::Error, argResult.Err().Error),
				Field("Reason", &Process::SExitError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}

class OnExitCallbackMock
{
public:
	using ArgType = Result<i64, Process::SExitError>;

	MOCK_METHOD(void, OnExit, (ArgType));

	auto CreateOnExit()
	{
		return [this](ArgType aResult)
		{
			OnExit(aResult);
		};
	}

};

class WinProcessTest
	: public ::testing::Test
{
protected:
	void SetUp()
	{
		myWinPlatformSystem.DelegateToFake();
	}

	NiceMock<WinEventSystemMock>		myWinEventSystem;
	NiceMock<WinPlatformSystemMock>		myWinPlatformSystem;

};

const HANDLE locDummyHandle = (HANDLE)0x00010000;

TEST_F(WinProcessTest, passed_handles_are_closed_when_deconstruted)
{
	const HANDLE dummyProcessHandle	= (HANDLE)0x00010001;
	const HANDLE dummyThreadHandle	= (HANDLE)0x00010002;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyProcessHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyThreadHandle));
}

TEST_F(WinProcessTest, passed_process_handle_is_passed_to_terminateprocess_when_deconstruted)
{
	const HANDLE dummyProcessHandle	= (HANDLE)0x00010001;
	const HANDLE dummyThreadHandle	= (HANDLE)0x00010002;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(dummyProcessHandle, _));
}

TEST_F(WinProcessTest, passed_handles_are_closed_when_terminated)
{
	const HANDLE dummyProcessHandle	= (HANDLE)0x00010001;
	const HANDLE dummyThreadHandle	= (HANDLE)0x00010002;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyProcessHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyThreadHandle));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, passed_process_handle_is_passed_to_terminateprocess_when_terminated)
{
	const HANDLE dummyProcessHandle	= (HANDLE)0x00010001;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(dummyProcessHandle, _));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_terminateprocess_and_error_code_is_zero)
{
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(_, exitCode));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_terminateprocess_and_error_code_is_dword_max)
{
	constexpr i64 exitCode = std::numeric_limits<DWORD>::max();

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(_, exitCode));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_terminateprocess_and_error_code_is_dword_min)
{
	constexpr i64 exitCode = std::numeric_limits<DWORD>::min();

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(_, exitCode));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, onexit_is_called_when_deconstructed)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated }))));

	process->OnExit(onExitCallbackMock.CreateOnExit());
}

TEST_F(WinProcessTest, onexit_is_called_when_terminated)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated }))));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_onexit_when_terminated_and_error_code_is_zero)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated, exitCode }))));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_onexit_when_terminated_and_error_code_is_dword_max)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = std::numeric_limits<DWORD>::max();

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated, exitCode }))));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_onexit_when_terminated_and_error_code_is_dword_min)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = std::numeric_limits<DWORD>::min();

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated, exitCode }))));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, process_handle_is_passed_to_register_event_when_initialized)
{
	const HANDLE dummyProcessHandle = (HANDLE)0x00010001;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinEventSystem, RegisterEvent(dummyProcessHandle, _));

	process->Initialize();
}

TEST_F(WinProcessTest, passed_handles_are_closed_when_process_exits)
{
	const HANDLE dummyProcessHandle = (HANDLE)0x00010001;
	const HANDLE dummyThreadHandle = (HANDLE)0x00010002;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyProcessHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyThreadHandle));

	std::move(callback)();
}

TEST_F(WinProcessTest, process_handle_is_passed_to_getexitcodeprocess_when_process_exits)
{
	const HANDLE dummyProcessHandle = (HANDLE)0x00010001;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess(dummyProcessHandle, _));

	std::move(callback)();
}

TEST_F(WinProcessTest, onexit_is_called_when_process_exits)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(onExitCallbackMock, OnExit);

	std::move(callback)();
}

TEST_F(WinProcessTest, error_code_is_passed_to_onexit_when_process_exits_and_error_code_is_zero)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce([exitCode](auto, LPDWORD lpExitCode)
			{
				*lpExitCode = exitCode;

				return TRUE;
			});
	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Ok(exitCode))));

	std::move(callback)();
}

TEST_F(WinProcessTest, error_code_is_passed_to_onexit_when_process_exits_and_error_code_is_dword_max)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = std::numeric_limits<DWORD>::max();

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce([exitCode](auto, LPDWORD lpExitCode)
			{
				*lpExitCode = exitCode;

				return TRUE;
			});
	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Ok(exitCode))));

	std::move(callback)();
}

TEST_F(WinProcessTest, error_code_is_passed_to_onexit_when_process_exits_and_error_code_is_dword_min)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = std::numeric_limits<DWORD>::min();

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce([exitCode](auto, LPDWORD lpExitCode)
			{
				*lpExitCode = exitCode;

				return TRUE;
			});
	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Ok(exitCode))));

	std::move(callback)();
}

TEST_F(WinProcessTest, internal_error_is_passed_to_onexit_when_unable_to_get_exit_code)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr DWORD errorCode = NO_ERROR;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	auto [callback, action] = WinEventSystemMock::CreateRegisterEvent();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(action);

	process->Initialize();

	process->OnExit(onExitCallbackMock.CreateOnExit());

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(errorCode));
	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::InternalError, errorCode }))));

	std::move(callback)();
}

TEST_F(WinProcessTest, onexit_is_called_immediately_when_process_is_already_stopped)
{
	StrictMock<OnExitCallbackMock> onExitCallbackMock;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	process->Terminate(exitCode);

	EXPECT_CALL(onExitCallbackMock, OnExit(ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::AlreadyStopped }))));

	process->OnExit(onExitCallbackMock.CreateOnExit());
}

TEST_F(WinProcessTest, get_state_returns_running)
{
	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);

	process->Initialize();

	EXPECT_EQ(process->GetState(), Process::EState::Running);
}

TEST_F(WinProcessTest, get_state_returns_stopped_when_terminated)
{
	auto process = std::make_shared<WinProcess>(myWinEventSystem, myWinPlatformSystem, locDummyHandle, locDummyHandle);
	constexpr i64 exitCode = 0;

	process->Terminate(exitCode);

	EXPECT_EQ(process->GetState(), Process::EState::Stopped);
}
