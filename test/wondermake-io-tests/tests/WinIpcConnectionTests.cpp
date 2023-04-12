#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/WinPlatformSystem.h"

#include "WonderMakeBaseTests/WinEventSystemMock.h"
#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WonderMakeIoTests/SocketMock.h"

#include "WinIpcConnection.h"

const std::vector<u8> locDummyData = { 0, 2, 65, 23 };

inline constexpr auto locPipePrefix = "\\\\.\\pipe\\";

class WinIpcConnectionTest
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

TEST_F(WinIpcConnectionTest, connect_returns_error_when_passing_empty_name)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto emptyName = "";

	const auto result = ipcConnection->Connect(emptyName);

	ASSERT_FALSE(result);

	EXPECT_EQ(result.Err().Error, IpcConnection::EConnectionError::InvalidArgs);
}

TEST_F(WinIpcConnectionTest, connect_call_create_file)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	std::string pipeName = "test";

	const		auto expFilename = locPipePrefix + pipeName;

	const		auto					expFilenameW = std::wstring(expFilename.begin(), expFilename.end());
	constexpr	DWORD					expPermissions = GENERIC_READ | GENERIC_WRITE;
	constexpr	DWORD					expShareMode = NULL;
	constexpr	LPSECURITY_ATTRIBUTES	expSecurityAttributes = NULL;
	constexpr	DWORD					expCreationDisposition = OPEN_EXISTING;
	constexpr	DWORD					expFlags = FILE_FLAG_OVERLAPPED;
	constexpr	HANDLE					expTemplateFile = NULL;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW(StrEq(expFilenameW), expPermissions, expShareMode, expSecurityAttributes, expCreationDisposition, expFlags, expTemplateFile))
		.WillOnce(Return(INVALID_HANDLE_VALUE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);

	(void)ipcConnection->Connect(pipeName);
}

TEST_F(WinIpcConnectionTest, connect_returns_error_when_unable_to_open_pipe)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(INVALID_HANDLE_VALUE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_FILE_NOT_FOUND));

	const auto result = ipcConnection->Connect(pipeName);

	ASSERT_FALSE(result);

	EXPECT_EQ(result.Err().Error, IpcConnection::EConnectionError::NoConnection);
}

TEST_F(WinIpcConnectionTest, connect_calls_create_event)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyFileHandle = (HANDLE)0x1234ABCD;

	constexpr LPSECURITY_ATTRIBUTES lpEventAttributes = NULL;
	constexpr BOOL bManualReset = TRUE;
	constexpr BOOL bInitialState = FALSE;
	constexpr LPCWSTR lpName = NULL;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyFileHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName))
		.WillOnce(Return((HANDLE)NULL));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyFileHandle));

	(void)ipcConnection->Connect(pipeName);
}

TEST_F(WinIpcConnectionTest, connect_file_handle_is_closed)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyFileHandle = (HANDLE)0x1234ABCD;

	constexpr LPSECURITY_ATTRIBUTES lpEventAttributes = NULL;
	constexpr BOOL bManualReset = TRUE;
	constexpr BOOL bInitialState = FALSE;
	constexpr LPCWSTR lpName = NULL;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyFileHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName))
		.WillOnce(Return((HANDLE)NULL));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyFileHandle));

	ipcConnection->Connect(pipeName);
}

TEST_F(WinIpcConnectionTest, connect_returns_error_when_unable_to_create_read_event)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyFileHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyFileHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return((HANDLE)NULL));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, CloseHandle);

	const auto result = ipcConnection->Connect(pipeName);

	ASSERT_FALSE(result);

	EXPECT_EQ(result.Err().Error, IpcConnection::EConnectionError::InternalError);
}

TEST_F(WinIpcConnectionTest, connect_returns_error_when_unable_to_create_write_event)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyFileHandle = (HANDLE)0x12340001;
	const HANDLE dummyReadHandle = (HANDLE)0x12340002;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyFileHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return(dummyReadHandle))
		.WillOnce(Return((HANDLE)NULL));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(2);

	const auto result = ipcConnection->Connect(pipeName);

	ASSERT_FALSE(result);

	EXPECT_EQ(result.Err().Error, IpcConnection::EConnectionError::InternalError);
}

TEST_F(WinIpcConnectionTest, connect_returns_success)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle(dummyHandle))
		.Times(3);

	const auto result = ipcConnection->Connect(pipeName);

	ASSERT_TRUE(result);
}

// --------------------------
//         Close tests
// --------------------------

TEST(WinIpcConnectionTests, onclose_is_called_when_a_connection_is_already_closed)
{
	NiceMock<WinEventSystemMock> winEventSystem;
	NiceMock<WinPlatformSystemMock> winPlatformSystem;

	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), winEventSystem, winPlatformSystem);

	auto future = ipcConnection->OnClose();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), CloseResultMatcher(Err(Socket::SCloseError{ Socket::ECloseError::AlreadyClosed })));
}

TEST(WinIpcConnectionTests, onclose_is_called_when_a_connection_is_closed_locally)
{
	NiceMock<WinEventSystemMock> winEventSystem;
	NiceMock<WinPlatformSystemMock> winPlatformSystem;

	winPlatformSystem.DelegateToFake();

	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), winEventSystem, winPlatformSystem);
	constexpr auto pipeName = "test";

	ipcConnection->Connect(pipeName);

	auto future = ipcConnection->OnClose();

	(void)ipcConnection->Close();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), CloseResultMatcher(Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedLocally })));
}

TEST(WinIpcConnectionTests, onclose_is_called_when_a_connection_is_closed_remotely)
{
	static constexpr auto expectedLocation = Socket::ECloseLocation::ClosedRemotely;
	static constexpr u64 expectedReason = ERROR_BROKEN_PIPE;

	NiceMock<WinEventSystemMock> winEventSystem;
	NiceMock<WinPlatformSystemMock> winPlatformSystem;

	winPlatformSystem.DelegateToFake();

	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), winEventSystem, winPlatformSystem);
	constexpr auto pipeName = "test";

	(void)ipcConnection->Connect(pipeName);

	auto futureClose = ipcConnection->OnClose();
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(winEventSystem, RegisterEvent)
		.WillOnce(Return(future));
	EXPECT_CALL(winPlatformSystem, GetLastError())
		.WillOnce(Return(ERROR_IO_PENDING));

	const auto _ = ipcConnection->Read();

	EXPECT_CALL(winPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(winPlatformSystem, GetLastError())
		.WillOnce(Return(expectedReason));

	promise.Complete();

	ASSERT_TRUE(futureClose.GetResult());

	EXPECT_THAT(*futureClose.GetResult(), CloseResultMatcher(Ok(Socket::SCloseLocation{ expectedLocation, expectedReason })));
}

TEST(WinIpcConnectionTests, onclose_is_called_when_a_connection_is_closed_due_to_error)
{
	NiceMock<WinEventSystemMock> winEventSystem;
	NiceMock<WinPlatformSystemMock> winPlatformSystem;

	winPlatformSystem.DelegateToFake();

	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), winEventSystem, winPlatformSystem);
	constexpr auto pipeName = "test";

	(void)ipcConnection->Connect(pipeName);

	auto futureClose = ipcConnection->OnClose();
	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(winEventSystem, RegisterEvent)
		.WillOnce(Return(future));
	EXPECT_CALL(winPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	const auto _ = ipcConnection->Read();

	EXPECT_CALL(winPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(winPlatformSystem, GetLastError);

	promise.Complete();

	ASSERT_TRUE(futureClose.GetResult());

	EXPECT_THAT(*futureClose.GetResult(), CloseResultMatcher(Err(Socket::SCloseError{ Socket::ECloseError::InternalError })));
}

// --------------------------
//         Read tests
// --------------------------

TEST_F(WinIpcConnectionTest, read_returns_invalid_state_when_not_open)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	auto future = ipcConnection->Read();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::InvalidState })));
}

TEST_F(WinIpcConnectionTest, read_passes_file_handle)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyFileHandle = (HANDLE)0x12340001;
	const HANDLE dummyEventHandle = (HANDLE)0x12340002;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyFileHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyEventHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile(dummyFileHandle, _, _, _, _))
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);

	(void)ipcConnection->Read();
}

TEST_F(WinIpcConnectionTest, read_returns_synchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(TRUE));

	auto future = ipcConnection->Read();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ReadResultMatcher(Ok(std::vector<u8>())));
}

TEST_F(WinIpcConnectionTest, synchronous_read_closes_on_internal_error)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);

	auto future = ipcConnection->Read();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::InternalError })));
}

TEST_F(WinIpcConnectionTest, synchronous_read_closes_on_state_change)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_BROKEN_PIPE));

	const auto future = ipcConnection->Read();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::StateChanged })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_read_remains_open_on_out_of_memory)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));

	const auto future = ipcConnection->Read();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::OutOfMemory })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, synchronous_read_calls_onread_with_data)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;
	
	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile(_, _, Ge(locDummyData.size()), _, _))
		.WillOnce([&dummyData = locDummyData](auto&&, LPVOID lpBuffer, auto&&, LPDWORD lpNumberOfBytesRead, auto&&)
			{
				memcpy(lpBuffer, dummyData.data(), dummyData.size());
				*lpNumberOfBytesRead = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	
	const auto future = ipcConnection->Read();

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), ReadResultMatcher(Ok(locDummyData)));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, read_returns_asynchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x12340001;
	const HANDLE dummyReadHandle = (HANDLE)0x12340002;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillOnce(Return(dummyHandle))
		.WillOnce(Return(dummyReadHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	EXPECT_CALL(myWinEventSystem, RegisterEvent(Eq(dummyReadHandle)));

	const auto future = ipcConnection->Read();

	ASSERT_FALSE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());
}

TEST_F(WinIpcConnectionTest, asynchronous_read_calls_onread)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x12340001;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureRead = ipcConnection->Read();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureRead.GetResult());
}

TEST_F(WinIpcConnectionTest, asynchronous_read_closes_on_internal_error)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureRead = ipcConnection->Read();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::InternalError })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);

}

TEST_F(WinIpcConnectionTest, asynchronous_read_closes_on_state_changed)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureRead = ipcConnection->Read();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_BROKEN_PIPE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::StateChanged, ERROR_BROKEN_PIPE })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_read_remains_open_on_out_of_memory)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureRead = ipcConnection->Read();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ReadResultMatcher(Err(Socket::SReadError{ Socket::EReadError::OutOfMemory, ERROR_NOT_ENOUGH_MEMORY })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, asynchronous_read_calls_onread_with_data)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);
	
	EXPECT_CALL(myWinPlatformSystem, ReadFile(_, _, Ge(locDummyData.size()), _, _))
		.WillOnce([&dummyData = locDummyData](auto&&, LPVOID lpBuffer, auto&&, auto&&, auto&&)
			{
				memcpy(lpBuffer, dummyData.data(), dummyData.size());

				return FALSE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();
	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));
	
	const auto futureRead = ipcConnection->Read();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ReadResultMatcher(Ok(locDummyData)));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, asynchronous_queue_reads)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	ipcConnection->Read()
		.Detach();
	
	const auto future = ipcConnection->Read();

	ASSERT_FALSE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());
}

TEST_F(WinIpcConnectionTest, asynchronous_dequeues_read)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	ipcConnection->Read()
		.Detach();
	ipcConnection->Read()
		.Detach();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(TRUE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_SUCCESS))
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	promise.Complete();
}

TEST_F(WinIpcConnectionTest, canceling_current_read_request_will_start_next_queued_read_request)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(ByMove(std::move(future))));

	auto futureReadA = ipcConnection->Read();

	const auto futureReadB = ipcConnection->Read();

	futureReadA.Reset();

	ASSERT_FALSE(futureReadB.IsCompleted());
	ASSERT_FALSE(futureReadB.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureReadB.IsCompleted());
}

TEST_F(WinIpcConnectionTest, canceling_queued_read_request_will_dequeue_it)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(ByMove(std::move(future))));

	const auto futureRead = ipcConnection->Read();

	(void)ipcConnection->Read();

	ASSERT_FALSE(futureRead.IsCompleted());
	ASSERT_FALSE(futureRead.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureRead.IsCompleted());
}

TEST_F(WinIpcConnectionTest, canceling_current_read_request_will_store_read_data_for_next_request)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce([&dummyData = locDummyData](auto&&, LPVOID lpBuffer, auto&&, auto&&, auto&&)
			{
				memcpy(lpBuffer, dummyData.data(), dummyData.size());

				return FALSE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(ByMove(std::move(future))));

	(void)ipcConnection->Read();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	const auto futureRead = ipcConnection->Read();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ReadResultMatcher(Ok(locDummyData)));
}

 // --------------------------
 //        Write tests
 // --------------------------

TEST_F(WinIpcConnectionTest, write_returns_invalid_state_when_not_open)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);

	const auto future = ipcConnection->Write(locDummyData);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::InvalidState })));
}

TEST_F(WinIpcConnectionTest, write_passes_file_handle)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyFileHandle = (HANDLE)0x12340001;
	const HANDLE dummyEventHandle = (HANDLE)0x12340002;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyFileHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyEventHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile(dummyFileHandle, _, _, _, _))
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);

	(void)ipcConnection->Write(locDummyData);
}

TEST_F(WinIpcConnectionTest, write_returns_synchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, auto&&, LPDWORD lpNumberOfBytesWritten, auto&&)
			{
				*lpNumberOfBytesWritten = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});

	auto future = ipcConnection->Write(locDummyData);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), WriteResultMatcher(Ok()));
}

TEST_F(WinIpcConnectionTest, synchronous_write_closes_on_internal_error)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);

	const auto future = ipcConnection->Write(locDummyData);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::InternalError })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_write_closes_on_state_change)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_BROKEN_PIPE));

	const auto future = ipcConnection->Write(locDummyData);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::StateChanged })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_write_remains_open_on_out_of_memory)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));

	const auto future = ipcConnection->Write(locDummyData);

	ASSERT_TRUE(future.GetResult());

	EXPECT_THAT(*future.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::OutOfMemory })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, synchronous_write_calls_onwrite_with_data)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile(_, _, Ge(locDummyData.size()), _, _))
		.With(Args<1, 2>(ElementsAreArrayVoidPointer(locDummyData.data(), locDummyData.size())))
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, auto&&, LPDWORD lpNumberOfBytesWritten, auto&&)
			{
				*lpNumberOfBytesWritten = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	
	(void)ipcConnection->Write(locDummyData);
}

TEST_F(WinIpcConnectionTest, write_returns_asynchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x12340001;
	const HANDLE dummyWriteHandle = (HANDLE)0x12340002;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillOnce(Return(dummyWriteHandle))
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	EXPECT_CALL(myWinEventSystem, RegisterEvent(Eq(dummyWriteHandle)));

	const auto future = ipcConnection->Write(locDummyData);

	ASSERT_FALSE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());
}

TEST_F(WinIpcConnectionTest, synchronous_write_splits_buffer_if_not_all_bytes_were_written)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;
	const std::vector<u8> secondWriteDummyData = { 65, 23 };

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce([](auto&&, auto&&, auto&&, LPDWORD lpNumberOfBytesWritten, auto&&)
			{
				*lpNumberOfBytesWritten = 2;

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, WriteFile(_, _, 2, _, _))
		.With(Args<1, 2>(ElementsAreArrayVoidPointer(secondWriteDummyData.data(), secondWriteDummyData.size())))
		.WillOnce([](auto&&, auto&&, auto&&, LPDWORD lpNumberOfBytesWritten, auto&&)
			{
				*lpNumberOfBytesWritten = 2;

				return TRUE;
			});

	const auto future = ipcConnection->Write(locDummyData);

	ASSERT_TRUE(future.GetResult());
	
	EXPECT_THAT(*future.GetResult(), WriteResultMatcher(Ok()));
}

TEST_F(WinIpcConnectionTest, asynchronous_write_calls_onwrite)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x12340001;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureWrite = ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureWrite.IsCompleted());
}

TEST_F(WinIpcConnectionTest, asynchronous_write_closes_on_internal_error)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureWrite = ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureWrite.GetResult());

	EXPECT_THAT(*futureWrite.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::InternalError })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_closes_on_state_changed)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureWrite = ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_BROKEN_PIPE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureWrite.GetResult());

	EXPECT_THAT(*futureWrite.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::StateChanged })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_remains_open_on_out_of_memory)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureWrite = ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureWrite.GetResult());

	EXPECT_THAT(*futureWrite.GetResult(), WriteResultMatcher(Err(Socket::SWriteError{ Socket::EWriteError::OutOfMemory })));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_calls_onwrite_on_success)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureWrite = ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();

	ASSERT_TRUE(futureWrite.GetResult());

	EXPECT_THAT(*futureWrite.GetResult(), WriteResultMatcher(Ok()));
	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_splits_buffer_if_not_all_bytes_were_written)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile);
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	const auto futureWrite = ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = 2;

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(myWinPlatformSystem, WriteFile(_, _, 2, _, _))
		.WillOnce([](auto&&, auto&&, auto&&, LPDWORD lpNumberOfBytesWritten, auto&&)
			{
				*lpNumberOfBytesWritten = 2;

				return TRUE;
			});

	promise.Complete();

	ASSERT_TRUE(futureWrite.GetResult());

	EXPECT_THAT(*futureWrite.GetResult(), WriteResultMatcher(Ok()));
}

TEST_F(WinIpcConnectionTest, asynchronous_queue_writes)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	ipcConnection->Write(locDummyData)
		.Detach();

	const auto futureWrite = ipcConnection->Write(locDummyData);

	ASSERT_FALSE(futureWrite.IsCompleted());
	ASSERT_FALSE(futureWrite.IsCanceled());
}

TEST_F(WinIpcConnectionTest, asynchronous_dequeues_write)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(future));

	ipcConnection->Write(locDummyData)
		.Detach();
	ipcConnection->Write(locDummyData)
		.Detach();

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(TRUE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_SUCCESS))
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	promise.Complete();
}

TEST_F(WinIpcConnectionTest, canceled_write_will_still_complete)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promise, future] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(ByMove(std::move(future))));

	(void)ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	promise.Complete();
}

TEST_F(WinIpcConnectionTest, asynchronous_write_followed_by_synchronous_write_continues_write_queue)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";
	const HANDLE dummyHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.Times(2)
		.WillRepeatedly(Return(dummyHandle));
	EXPECT_CALL(myWinPlatformSystem, CloseHandle)
		.Times(3);

	(void)ipcConnection->Connect(pipeName);

	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_IO_PENDING));

	auto [promiseAsync, futureAsync] = MakeAsync<void>();

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce(Return(ByMove(std::move(futureAsync))));

	(void)ipcConnection->Write(locDummyData);
	(void)ipcConnection->Write(locDummyData);
	(void)ipcConnection->Write(locDummyData);

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_SUCCESS));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.Times(2)
		.WillRepeatedly([&dummyData = locDummyData](auto&&, auto&&, auto&&, LPDWORD lpNumberOfBytesWritten, auto&&)
			{
				*lpNumberOfBytesWritten = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});

	promiseAsync.Complete();
}
