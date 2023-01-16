#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/IpcConnection.h"

#include "wondermake-base/WinPlatformSystem.h"

#include "WonderMakeBaseTests/WinEventSystemMock.h"
#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WonderMakeIoTests/IpcAcceptorMock.h"
#include "WonderMakeIoTests/SocketMock.h"

#include "WinIpcAcceptor.h"

inline constexpr auto locDummyName	= "dummy_name";
inline constexpr auto locWDummyName	= L"dummy_name";

const std::wstring locPipePrefix	= L"\\\\.\\pipe\\";

class WinIpcAcceptorTest
	: public ::testing::Test
{
protected:
	void SetUp()
	{
		myWinEventSystem.DelegateToFake();
		myWinPlatformSystem.DelegateToFake();
	}

	StrictMock<WinEventSystemMock> myWinEventSystem;
	StrictMock<WinPlatformSystemMock> myWinPlatformSystem;

};

TEST_F(WinIpcAcceptorTest, open_creates_an_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);
}

TEST_F(WinIpcAcceptorTest, open_returns_invalid_arg_when_name_is_empty)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	const auto result = ipcAcceptor->Open("");

	EXPECT_THAT(result, IpcAcceptorOpenResultMatcher(Err(IpcAcceptor::SOpenError{ IpcAcceptor::EOpenError::InvalidArgs })));
}

TEST_F(WinIpcAcceptorTest, open_returns_success_when_event_is_created)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	const auto result = ipcAcceptor->Open(locDummyName);

	EXPECT_TRUE(result);
}

TEST_F(WinIpcAcceptorTest, open_returns_error_when_event_creation_fails)
{
	static constexpr DWORD expectedError = 1234;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(expectedError));

	const auto result = ipcAcceptor->Open(locDummyName);

	EXPECT_THAT(result, IpcAcceptorOpenResultMatcher(Err(IpcAcceptor::SOpenError{ IpcAcceptor::EOpenError::InternalError, expectedError })));
}

TEST_F(WinIpcAcceptorTest, event_is_closed_when_opened_acceptor_is_destroyed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);
	
	 // The delegated function already expects the CloseHandle call, so no need to explicitly expect it.
	 // EXPECT_CALL(myWinPlatformSystem, CloseHandle);

	ipcAcceptor->Close();
}

TEST_F(WinIpcAcceptorTest, getstate_returns_closed_after_being_created)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_EQ(ipcAcceptor->GetState(), IpcAcceptor::EState::Closed);
}

TEST_F(WinIpcAcceptorTest, getstate_returns_open_after_being_opened)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_EQ(ipcAcceptor->GetState(), IpcAcceptor::EState::Open);
}

TEST_F(WinIpcAcceptorTest, getstate_returns_closed_after_failing_to_open)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_EQ(ipcAcceptor->GetState(), IpcAcceptor::EState::Closed);
}

TEST_F(WinIpcAcceptorTest, getstate_returns_closed_after_being_closed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	ipcAcceptor->Close();

	EXPECT_EQ(ipcAcceptor->GetState(), IpcAcceptor::EState::Closed);
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_invalid_state_when_closed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());
	
	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InvalidState })));
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_error_when_failing_to_create_named_pipe)
{
	static constexpr DWORD expectedError = ERROR_ACCESS_DENIED;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW)
		.WillOnce(Return(INVALID_HANDLE_VALUE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(expectedError));

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InternalError, expectedError })));
}

TEST_F(WinIpcAcceptorTest, onconnection_passes_correct_pipename)
{
	static const auto expectedName = locPipePrefix + locWDummyName;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW(StrEq(expectedName), _, _, _, _, _, _, _))
		.WillOnce(Return(INVALID_HANDLE_VALUE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_ACCESS_DENIED));

	(void)ipcAcceptor->OnConnection();
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_error_when_lasterror_returns_invalid_code_after_trying_to_connect_a_named_pipe)
{
	static constexpr DWORD expectedError = ERROR_ACCESS_DENIED;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);
	
	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(expectedError));

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InternalError, expectedError })));
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_valid_non_completed_future)
{
	const HANDLE pipeHandle = (HANDLE)0x1234;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return(pipeHandle));

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent(Eq(pipeHandle)));

	const auto future = ipcAcceptor->OnConnection();

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, CloseHandle(pipeHandle));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent)
		.Times(::testing::AnyNumber());
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_ipcconnection_when_pipe_already_connected)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_PIPE_CONNECTED));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2);

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	ASSERT_TRUE(*future.GetResult());

	EXPECT_EQ(future.GetResult()->Unwrap()->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_outofmemory_when_pipe_already_connected_but_fails_setup_on_first_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_PIPE_CONNECTED))
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::OutOfMemory, ERROR_NOT_ENOUGH_MEMORY })));
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_internalerror_when_pipe_already_connected_but_fails_setup_on_first_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_PIPE_CONNECTED))
		.WillOnce(Return(ERROR_ACCESS_DENIED));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InternalError, ERROR_ACCESS_DENIED })));
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_outofmemory_when_pipe_already_connected_but_fails_setup_on_second_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_PIPE_CONNECTED))
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(1)
		.WillOnce(Return((HANDLE)NULL));

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::OutOfMemory, ERROR_NOT_ENOUGH_MEMORY })));
}

TEST_F(WinIpcAcceptorTest, onconnection_returns_internalerror_when_pipe_already_connected_but_fails_setup_on_second_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_PIPE_CONNECTED))
		.WillOnce(Return(ERROR_ACCESS_DENIED));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(1)
		.WillOnce(Return((HANDLE)NULL));

	const auto future = ipcAcceptor->OnConnection();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InternalError, ERROR_ACCESS_DENIED })));
}

TEST_F(WinIpcAcceptorTest, onconnection_future_is_ipcconnection_when_pipeevent_is_triggered)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureConnection = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2);

	promise.Complete();

	ASSERT_TRUE(futureConnection.GetResult());

	EXPECT_TRUE(*futureConnection.GetResult());

	EXPECT_EQ(futureConnection.GetResult()->Unwrap()->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcAcceptorTest, onconnection_future_is_outofmemory_when_pipeevent_is_triggered_but_fails_setup_on_first_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureConnection = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));

	promise.Complete();

	ASSERT_TRUE(futureConnection.GetResult());

	EXPECT_THAT(*futureConnection.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::OutOfMemory, ERROR_NOT_ENOUGH_MEMORY })));
}

TEST_F(WinIpcAcceptorTest, onconnection_future_is_internalerror_when_pipeevent_is_triggered_but_fails_setup_on_first_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureConnection = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_ACCESS_DENIED));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));

	promise.Complete();

	ASSERT_TRUE(futureConnection.GetResult());

	EXPECT_THAT(*futureConnection.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InternalError, ERROR_ACCESS_DENIED })));
}

TEST_F(WinIpcAcceptorTest, onconnection_future_is_outofmemory_when_pipeevent_is_triggered_but_fails_setup_on_second_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureConnection = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(1)
		.WillOnce(Return((HANDLE)NULL));

	promise.Complete();

	ASSERT_TRUE(futureConnection.GetResult());

	EXPECT_THAT(*futureConnection.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::OutOfMemory, ERROR_NOT_ENOUGH_MEMORY })));
}

TEST_F(WinIpcAcceptorTest, onconnection_future_is_internalerror_when_pipeevent_is_triggered_but_fails_setup_on_second_event)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureConnection = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_ACCESS_DENIED));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(1)
		.WillOnce(Return((HANDLE)NULL));

	promise.Complete();

	ASSERT_TRUE(futureConnection.GetResult());

	EXPECT_THAT(*futureConnection.GetResult(), IpcAcceptorConnectionResultMatcher(Err(IpcAcceptor::SConnectionError{ IpcAcceptor::EConnectionError::InternalError, ERROR_ACCESS_DENIED })));
}

TEST_F(WinIpcAcceptorTest, next_onconnection_request_is_processed_when_current_is_completed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureConnectionA = ipcAcceptor->OnConnection();

	const auto futureConnectionB = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2);
	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	promise.Complete();

	ASSERT_TRUE(futureConnectionA.IsCompleted());
	ASSERT_FALSE(futureConnectionA.IsCanceled());

	ASSERT_FALSE(futureConnectionB.IsCompleted());
	ASSERT_FALSE(futureConnectionB.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, ResetEvent)
		.Times(::testing::AnyNumber());
}

TEST_F(WinIpcAcceptorTest, next_onconnection_request_is_completed_when_current_request_is_canceled)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	auto futureConnectionA = ipcAcceptor->OnConnection();

	const auto futureConnectionB = ipcAcceptor->OnConnection();

	futureConnectionA.Reset();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2);

	promise.Complete();

	ASSERT_TRUE(futureConnectionB.IsCompleted());
	ASSERT_FALSE(futureConnectionB.IsCanceled());
}

TEST_F(WinIpcAcceptorTest, connection_is_reset_when_all_requests_are_canceled)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(ByMove(std::move(future))));

	auto futureConnection = ipcAcceptor->OnConnection();

	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	futureConnection.Reset();

	EXPECT_FALSE(promise.IsValid());
}

TEST_F(WinIpcAcceptorTest, futures_are_canceled_when_acceptor_is_closed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	EXPECT_CALL(myWinPlatformSystem, CreateNamedPipeW);
	EXPECT_CALL(myWinPlatformSystem, ConnectNamedPipe);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	const auto futureA = ipcAcceptor->OnConnection();
	const auto futureB = ipcAcceptor->OnConnection();

	ipcAcceptor->Close();

	EXPECT_TRUE(futureA.IsCanceled());
	EXPECT_TRUE(futureB.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, ResetEvent)
		.Times(::testing::AnyNumber());
}

TEST_F(WinIpcAcceptorTest, onclose_returns_canceled_future_if_acceptor_is_closed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	const auto future = ipcAcceptor->OnClose();

	ASSERT_TRUE(future.IsCanceled());
}

TEST_F(WinIpcAcceptorTest, onclose_returns_valid_non_completed_future_if_acceptor_is_open)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	const auto future = ipcAcceptor->OnClose();

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
}

TEST_F(WinIpcAcceptorTest, onclose_future_is_completed_when_acceptor_is_closed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	const auto future = ipcAcceptor->OnClose();

	ipcAcceptor->Close();

	EXPECT_TRUE(future.IsCompleted());
}

TEST_F(WinIpcAcceptorTest, all_onclose_futures_are_completed_when_acceptor_is_closed)
{
	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);

	(void)ipcAcceptor->Open(locDummyName);

	const auto futureA = ipcAcceptor->OnClose();
	const auto futureB = ipcAcceptor->OnClose();
	const auto futureC = ipcAcceptor->OnClose();

	ipcAcceptor->Close();

	EXPECT_TRUE(futureA.IsCompleted());
	EXPECT_TRUE(futureB.IsCompleted());
	EXPECT_TRUE(futureC.IsCompleted());
}
