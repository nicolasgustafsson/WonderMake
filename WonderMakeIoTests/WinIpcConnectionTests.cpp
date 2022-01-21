#include "WonderMakeTestsCommon/GTestInclude.h"
#include "WonderMakeTestsCommon/WinEventSystemMock.h"
#include "WonderMakeTestsCommon/WinPlatformSystemMock.h"

#include "WonderMakeEngine/WinPlatformSystem.h"

#include "WinIpcConnection.h"

constexpr auto locDummyOnRead = [](auto&&) {};
constexpr auto locDummyOnWrite = [](auto&&) {};
const std::vector<u8> locDummyData = { 0, 2, 65, 23 };

constexpr auto locPipePrefix = "\\\\.\\pipe\\";

class IpcConnectionCallbackMock
{
public:
	using ReadResult = Result<Socket::EReadError, std::vector<u8>>;
	using WriteResult = Result<Socket::EWriteError>;

	MOCK_METHOD(void, OnRead, (ReadResult&& aResult), ());
	MOCK_METHOD(void, OnWrite, (WriteResult&& aResult), ());
};

class WinIpcConnectionTest
	: public ::testing::Test
{
protected:
	StrictMock<WinEventSystemMock> myWinEventSystem;
	StrictMock<WinPlatformSystemMock> myWinPlatformSystem;

};

TEST_F(WinIpcConnectionTest, connect_returns_error_when_passing_empty_name)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
	constexpr auto emptyName = "";

	const auto result = ipcConnection->Connect(emptyName);

	ASSERT_FALSE(result);
	EXPECT_EQ(result, IpcConnection::ConnectionError::InvalidArgs);
}

TEST_F(WinIpcConnectionTest, connect_call_create_file)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
	constexpr auto pipeName = "test";

	EXPECT_CALL(myWinPlatformSystem, CreateFileW)
		.WillOnce(Return(INVALID_HANDLE_VALUE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_FILE_NOT_FOUND));

	const auto result = ipcConnection->Connect(pipeName);

	ASSERT_FALSE(result);
	EXPECT_EQ(result, IpcConnection::ConnectionError::NoConnection);
}

TEST_F(WinIpcConnectionTest, connect_calls_create_event)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_EQ(result, IpcConnection::ConnectionError::InternalError);
}

TEST_F(WinIpcConnectionTest, connect_returns_error_when_unable_to_create_write_event)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_EQ(result, IpcConnection::ConnectionError::InternalError);
}

TEST_F(WinIpcConnectionTest, connect_returns_success)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
//         Read tests
// --------------------------

TEST_F(WinIpcConnectionTest, read_returns_invalid_state_when_not_open)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);

	const auto result = ipcConnection->Read(locDummyOnRead);

	ASSERT_FALSE(result);
	EXPECT_EQ(result, Socket::EReadError::InvalidState);
}

TEST_F(WinIpcConnectionTest, read_passes_file_handle)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	(void)ipcConnection->Read(locDummyOnRead);
}

TEST_F(WinIpcConnectionTest, read_returns_synchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	auto result = ipcConnection->Read(locDummyOnRead);

	ASSERT_TRUE(result);
	EXPECT_EQ(result, Socket::EAsynchronicity::Synchronous);
}

TEST_F(WinIpcConnectionTest, synchronous_read_closes_on_internal_error)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::InternalError)));

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_read_closes_on_state_change)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::StateChanged)));

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_read_remains_open_on_out_of_memory)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::OutOfMemory)));

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, synchronous_read_calls_onread_with_data)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnRead(Eq(locDummyData)));

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});
}

TEST_F(WinIpcConnectionTest, read_returns_asynchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	EXPECT_CALL(myWinEventSystem, RegisterEvent(dummyReadHandle, _));
	EXPECT_CALL(myWinEventSystem, UnregisterEvent(dummyReadHandle));

	auto result = ipcConnection->Read(locDummyOnRead);

	ASSERT_TRUE(result);
	EXPECT_EQ(result, Socket::EAsynchronicity::Asynchronous);
}

TEST_F(WinIpcConnectionTest, asynchronous_read_calls_onread)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	EXPECT_CALL(callbackMock, OnRead);

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	std::move(registerdCallback)();
}

TEST_F(WinIpcConnectionTest, asynchronous_read_closes_on_internal_error)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::InternalError)));

	std::move(registerdCallback)();

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_read_closes_on_state_changed)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_BROKEN_PIPE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::StateChanged)));

	std::move(registerdCallback)();

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_read_remains_open_on_out_of_memory)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::OutOfMemory)));

	std::move(registerdCallback)();

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, asynchronous_read_calls_onread_with_data)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	EXPECT_CALL(callbackMock, OnRead(Eq(locDummyData)));

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	std::move(registerdCallback)();
}

TEST_F(WinIpcConnectionTest, asynchronous_queue_reads)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(myWinEventSystem, UnregisterEvent);

	const auto onRead = [&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		};

	(void)ipcConnection->Read(onRead);
	
	auto result = ipcConnection->Read(onRead);

	ASSERT_TRUE(result);
	EXPECT_EQ(result, Socket::EAsynchronicity::Asynchronous);

	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::StateChanged)))
		.Times(2);
}

TEST_F(WinIpcConnectionTest, asynchronous_dequeues_read)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Read(locDummyOnRead);

	(void)ipcConnection->Read([&callbackMock](auto&& aResult)
		{
			callbackMock.OnRead(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(TRUE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, ReadFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_SUCCESS))
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	std::move(registerdCallback)();

	EXPECT_CALL(callbackMock, OnRead(Eq(Socket::EReadError::StateChanged)));
	EXPECT_CALL(myWinEventSystem, UnregisterEvent);
}

 // --------------------------
 //        Write tests
 // --------------------------

TEST_F(WinIpcConnectionTest, write_returns_invalid_state_when_not_open)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);

	const auto result = ipcConnection->Write(locDummyData, locDummyOnWrite);

	ASSERT_FALSE(result);
	EXPECT_EQ(result, Socket::EWriteError::InvalidState);
}

TEST_F(WinIpcConnectionTest, write_passes_file_handle)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	(void)ipcConnection->Write(locDummyData, locDummyOnWrite);
}

TEST_F(WinIpcConnectionTest, write_returns_synchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	auto result = ipcConnection->Write(locDummyData, locDummyOnWrite);

	ASSERT_TRUE(result);
	EXPECT_EQ(result, Socket::EAsynchronicity::Synchronous);
}

TEST_F(WinIpcConnectionTest, synchronous_write_closes_on_internal_error)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::InternalError)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_write_closes_on_state_change)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::StateChanged)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, synchronous_write_remains_open_on_out_of_memory)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::OutOfMemory)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, synchronous_write_calls_onwrite_with_data)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(callbackMock, OnWrite(Eq(Success)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});
}

TEST_F(WinIpcConnectionTest, write_returns_asynchronous)
{
	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	EXPECT_CALL(myWinEventSystem, RegisterEvent(dummyWriteHandle, _));
	EXPECT_CALL(myWinEventSystem, UnregisterEvent(dummyWriteHandle));

	auto result = ipcConnection->Write(locDummyData, locDummyOnWrite);

	ASSERT_TRUE(result);
	EXPECT_EQ(result, Socket::EAsynchronicity::Asynchronous);
}

TEST_F(WinIpcConnectionTest, synchronous_write_splits_buffer_if_not_all_bytes_were_written)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	EXPECT_CALL(callbackMock, OnWrite(Eq(Success)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});
}

TEST_F(WinIpcConnectionTest, asynchronous_write_calls_onwrite)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	EXPECT_CALL(callbackMock, OnWrite);

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	std::move(registerdCallback)();
}

TEST_F(WinIpcConnectionTest, asynchronous_write_closes_on_internal_error)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::InternalError)));

	std::move(registerdCallback)();

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_closes_on_state_changed)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_BROKEN_PIPE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::StateChanged)));

	std::move(registerdCallback)();

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Closed);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_remains_open_on_out_of_memory)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_NOT_ENOUGH_MEMORY));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::OutOfMemory)));

	std::move(registerdCallback)();

	EXPECT_EQ(ipcConnection->GetState(), Socket::EState::Open);
}

TEST_F(WinIpcConnectionTest, asynchronous_write_calls_onwrite_on_success)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	EXPECT_CALL(callbackMock, OnWrite(Eq(Success)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce([&dummyData = locDummyData](auto&&, auto&&, LPDWORD lpNumberOfBytesTransferred, auto&&)
			{
				*lpNumberOfBytesTransferred = static_cast<DWORD>(dummyData.size());

				return TRUE;
			});
	EXPECT_CALL(myWinPlatformSystem, GetLastError);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);

	std::move(registerdCallback)();
}

TEST_F(WinIpcConnectionTest, asynchronous_write_splits_buffer_if_not_all_bytes_were_written)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	EXPECT_CALL(callbackMock, OnWrite(Eq(Success)));

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

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

	std::move(registerdCallback)();
}

TEST_F(WinIpcConnectionTest, asynchronous_queue_writes)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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
	EXPECT_CALL(myWinEventSystem, UnregisterEvent);

	const auto onWrite = [&callbackMock](auto&& aResult)
	{
		callbackMock.OnWrite(std::move(aResult));
	};

	(void)ipcConnection->Write(locDummyData, onWrite);

	auto result = ipcConnection->Write(locDummyData, onWrite);

	ASSERT_TRUE(result);
	EXPECT_EQ(result, Socket::EAsynchronicity::Asynchronous);

	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::StateChanged)))
		.Times(2);
}

TEST_F(WinIpcConnectionTest, asynchronous_dequeues_write)
{
	StrictMock<IpcConnectionCallbackMock> callbackMock;

	auto ipcConnection = std::make_shared<WinIpcConnection>(myWinEventSystem, myWinPlatformSystem);
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

	Closure registerdCallback = []() {};

	EXPECT_CALL(myWinEventSystem, RegisterEvent)
		.WillOnce([&registerdCallback](auto&&, auto&& aCallback)
			{
				registerdCallback = std::move(aCallback);
			});

	(void)ipcConnection->Write(locDummyData, locDummyOnWrite);

	(void)ipcConnection->Write(locDummyData, [&callbackMock](auto&& aResult)
		{
			callbackMock.OnWrite(std::move(aResult));
		});

	EXPECT_CALL(myWinPlatformSystem, GetOverlappedResult)
		.WillOnce(Return(TRUE));
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WriteFile)
		.WillOnce(Return(FALSE));
	EXPECT_CALL(myWinPlatformSystem, GetLastError)
		.WillOnce(Return(ERROR_SUCCESS))
		.WillOnce(Return(ERROR_IO_PENDING));
	EXPECT_CALL(myWinEventSystem, RegisterEvent);

	std::move(registerdCallback)();

	EXPECT_CALL(callbackMock, OnWrite(Eq(Socket::EWriteError::StateChanged)));
	EXPECT_CALL(myWinEventSystem, UnregisterEvent);
}