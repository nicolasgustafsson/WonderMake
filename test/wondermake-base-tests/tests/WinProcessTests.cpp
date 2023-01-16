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
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010001, myWinPlatformSystem);
	const HANDLE dummyThreadHandle	= (HANDLE)0x00010002;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyProcessHandle.Get()));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyThreadHandle));
}

TEST_F(WinProcessTest, passed_process_handle_is_passed_to_terminateprocess_when_deconstruted)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010001, myWinPlatformSystem);
	const HANDLE dummyThreadHandle	= (HANDLE)0x00010002;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(dummyProcessHandle.Get(), _));
}

TEST_F(WinProcessTest, passed_handles_are_closed_when_terminated)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010001, myWinPlatformSystem);
	const HANDLE dummyThreadHandle	= (HANDLE)0x00010002;
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyProcessHandle.Get()));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyThreadHandle));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, passed_process_handle_is_passed_to_terminateprocess_when_terminated)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010001, myWinPlatformSystem);
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(dummyProcessHandle.Get(), _));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_terminateprocess_and_error_code_is_zero)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(_, exitCode));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_terminateprocess_and_error_code_is_dword_max)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = std::numeric_limits<DWORD>::max();

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(_, exitCode));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, exit_code_is_passed_to_terminateprocess_and_error_code_is_dword_min)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = std::numeric_limits<DWORD>::min();

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinPlatformSystem, TerminateProcess(_, exitCode));

	process->Terminate(exitCode);
}

TEST_F(WinProcessTest, onexit_returns_a_valid_non_completed_future)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	const auto future = process->OnExit();

	ASSERT_TRUE(future.IsValid());
	ASSERT_FALSE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());
}

TEST_F(WinProcessTest, onexit_is_called_when_deconstructed)
{
	const auto perform = [this]()
	{
		const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);

		auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

		return process->OnExit();
	};

	const auto future = perform();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated })));
}

TEST_F(WinProcessTest, onexit_is_called_when_terminated)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	const auto future = process->OnExit();

	process->Terminate(exitCode);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated })));
}

TEST_F(WinProcessTest, exit_code_is_passed_to_onexit_when_terminated_and_error_code_is_zero)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	const auto future = process->OnExit();

	process->Terminate(exitCode);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated, exitCode })));
}

TEST_F(WinProcessTest, exit_code_is_passed_to_onexit_when_terminated_and_error_code_is_dword_max)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = std::numeric_limits<DWORD>::max();

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	const auto future = process->OnExit();

	process->Terminate(exitCode);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated, exitCode })));
}

TEST_F(WinProcessTest, exit_code_is_passed_to_onexit_when_terminated_and_error_code_is_dword_min)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = std::numeric_limits<DWORD>::min();

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	const auto future = process->OnExit();

	process->Terminate(exitCode);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::Terminated, exitCode })));
}

TEST_F(WinProcessTest, process_handle_is_passed_to_register_event_when_initialized)
{
	const HANDLE dummyHandle = (HANDLE)0x00010001;

	const WinEventHandle dummyProcessHandle(dummyHandle, myWinPlatformSystem);

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	EXPECT_CALL(myWinEventSystem, RegisterEvent(Eq(dummyHandle)));

	process->Initialize();
}

TEST_F(WinProcessTest, passed_handles_are_closed_when_process_exits)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010001, myWinPlatformSystem);
	const HANDLE dummyThreadHandle = (HANDLE)0x00010002;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, dummyThreadHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyProcessHandle.Get()));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyThreadHandle));

	promise.Complete();
}

TEST_F(WinProcessTest, process_handle_is_passed_to_getexitcodeprocess_when_process_exits)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010001, myWinPlatformSystem);

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess(dummyProcessHandle.Get(), _));

	promise.Complete();
}

TEST_F(WinProcessTest, onexit_is_called_when_process_exits)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();
	
	const auto futureExit = process->OnExit();

	promise.Complete();

	ASSERT_TRUE(futureExit.IsCompleted());
}

TEST_F(WinProcessTest, error_code_is_passed_to_onexit_when_process_exits_and_error_code_is_zero)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();

	const auto futureExit = process->OnExit();

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce([exitCode](HANDLE, LPDWORD lpExitCode) -> BOOL
			{
				*lpExitCode = static_cast<DWORD>(exitCode);

				return TRUE;
			});
	
	promise.Complete();

	ASSERT_TRUE(futureExit.GetResult());

	EXPECT_THAT(*futureExit.GetResult(), ExitResultMatcher(Ok(exitCode)));
}

TEST_F(WinProcessTest, error_code_is_passed_to_onexit_when_process_exits_and_error_code_is_dword_max)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = std::numeric_limits<DWORD>::max();

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();

	const auto futureExit = process->OnExit();

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce([exitCode](HANDLE, LPDWORD lpExitCode) -> BOOL
			{
				*lpExitCode = static_cast<DWORD>(exitCode);

				return TRUE;
			});
	
	promise.Complete();

	ASSERT_TRUE(futureExit.GetResult());

	EXPECT_THAT(*futureExit.GetResult(), ExitResultMatcher(Ok(exitCode)));
}

TEST_F(WinProcessTest, error_code_is_passed_to_onexit_when_process_exits_and_error_code_is_dword_min)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = std::numeric_limits<DWORD>::min();

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();

	const auto futureExit = process->OnExit();

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce([exitCode](HANDLE, LPDWORD lpExitCode) -> BOOL
			{
				*lpExitCode = static_cast<DWORD>(exitCode);

				return TRUE;
			});
	
	promise.Complete();

	ASSERT_TRUE(futureExit.GetResult());

	EXPECT_THAT(*futureExit.GetResult(), ExitResultMatcher(Ok(exitCode)));
}

TEST_F(WinProcessTest, internal_error_is_passed_to_onexit_when_unable_to_get_exit_code)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr DWORD errorCode = NO_ERROR;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	process->Initialize();

	const auto futureExit = process->OnExit();

	EXPECT_CALL(myWinPlatformSystem, GetExitCodeProcess)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(errorCode));
	
	promise.Complete();

	ASSERT_TRUE(futureExit.GetResult());

	EXPECT_THAT(*futureExit.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::InternalError, errorCode })));
}

TEST_F(WinProcessTest, onexit_is_called_immediately_when_process_is_already_stopped)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);
	constexpr i64 exitCode = 0;

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	process->Terminate(exitCode);

	const auto future = process->OnExit();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ExitResultMatcher(Err(Process::SExitError{ Process::EExitError::AlreadyStopped })));
}

TEST_F(WinProcessTest, get_state_returns_running)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);

	process->Initialize();

	EXPECT_EQ(process->GetState(), Process::EState::Running);
}

TEST_F(WinProcessTest, get_state_returns_stopped_when_terminated)
{
	const WinEventHandle dummyProcessHandle((HANDLE)0x00010000, myWinPlatformSystem);

	auto process = std::make_shared<WinProcess>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem, dummyProcessHandle, locDummyHandle);
	constexpr i64 exitCode = 0;

	process->Terminate(exitCode);

	EXPECT_EQ(process->GetState(), Process::EState::Stopped);
}
