#include "wondermake-tests-common/GTestInclude.h"

#include "WinEventSystemImpl.h"
#include "WinPlatformSystemImpl.h"

#include "WinIpcAcceptor.h"
#include "WinIpcConnection.h"

#include "WonderMakeIoTests/SocketMock.h"

class AcceptorCallbackMock
{
public:
	MOCK_METHOD(void, OnConnection, (std::shared_ptr<IpcConnection>&& aConnection), ());
};
class ConnectionCallbackMock
{
public:
	using WriteResult = Result<void, Socket::SWriteError>;
	using ReadResult = Result<std::vector<u8>, Socket::SReadError>;

	MOCK_METHOD(void, OnWrite, (WriteResult&& aResult), ());
	MOCK_METHOD(void, OnRead, (ReadResult&& aResult), ());
};

constexpr auto onConnection = [](auto& aMock)
{
	return [&mock = aMock](auto&& aConnection)
	{
		mock.OnConnection(std::move(aConnection));
	};
};
constexpr auto onWrite = [](auto& aMock)
{
	return [&mock = aMock](auto&& aResult)
	{
		mock.OnWrite(std::move(aResult));
	};
};
constexpr auto onRead = [](auto& aMock)
{
	return [&mock = aMock](auto&& aResult)
	{
		mock.OnRead(std::move(aResult));
	};
};

TEST(WinIpcConnectionManualTests, connection_fails_when_no_acceptor_exists)
{
	auto winPlatform = MakeSystem<WinPlatformSystemImpl>(std::make_tuple());

	auto winEvent = MakeSystem<WinEventSystemImpl>(std::make_tuple(winPlatform));

	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), *winEvent, *winPlatform);

	auto connectResult = ipcConnection->Connect("wondermake__manual_test_connection");

	ASSERT_FALSE(connectResult);

	EXPECT_EQ(connectResult.Err().Error, IpcConnection::EConnectionError::NoConnection);
}

TEST(WinIpcConnectionManualTests, connection_connects_when_acceptor_exists)
{
	auto winPlatform = MakeSystem<WinPlatformSystemImpl>(std::make_tuple());

	auto winEvent = MakeSystem<WinEventSystemImpl>(std::make_tuple(winPlatform));

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), *winEvent, *winPlatform);
	auto ipcConnection = std::make_shared<WinIpcConnection>(InlineExecutor(), *winEvent, *winPlatform);

	auto openResult = ipcAcceptor->Open("wondermake__manual_test_connection");

	ASSERT_TRUE(openResult);

	ipcAcceptor->OnConnection()
		.Detach();

	auto connectResult = ipcConnection->Connect("wondermake__manual_test_connection");

	ASSERT_TRUE(connectResult);

	winEvent->WaitForEvent(0);
}

TEST(WinIpcConnectionManualTests, acceptor_accepts_multiple_connections)
{
	auto winPlatform = MakeSystem<WinPlatformSystemImpl>(std::make_tuple());

	auto winEvent = MakeSystem<WinEventSystemImpl>(std::make_tuple(winPlatform));

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), *winEvent, *winPlatform);
	auto ipcConnectionA = std::make_shared<WinIpcConnection>(InlineExecutor(), *winEvent, *winPlatform);
	auto ipcConnectionB = std::make_shared<WinIpcConnection>(InlineExecutor(), *winEvent, *winPlatform);

	auto openResult = ipcAcceptor->Open("wondermake__manual_test_connection");

	ASSERT_TRUE(openResult);

	auto acceptorConnectionFuture = ipcAcceptor->OnConnection();

	auto connectResultA = ipcConnectionA->Connect("wondermake__manual_test_connection");

	ASSERT_TRUE(connectResultA);

	winEvent->WaitForEvent(0);

	ASSERT_TRUE(acceptorConnectionFuture.GetResult());

	ASSERT_TRUE(*acceptorConnectionFuture.GetResult());

	acceptorConnectionFuture = ipcAcceptor->OnConnection();

	auto connectResultB = ipcConnectionB->Connect("wondermake__manual_test_connection");

	ASSERT_TRUE(connectResultB);

	winEvent->WaitForEvent(0);

	ASSERT_TRUE(acceptorConnectionFuture.IsCompleted());
}

TEST(WinIpcConnectionManualTests, connection_closes_when_other_connection_is_closed)
{
	constexpr auto pipeName = "wondermake__manual_test_connection";

	auto winPlatform = MakeSystem<WinPlatformSystemImpl>(std::make_tuple());

	auto winEvent = MakeSystem<WinEventSystemImpl>(std::make_tuple(winPlatform));

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), *winEvent, *winPlatform);
	auto ipcConnectionClient = std::make_shared<WinIpcConnection>(InlineExecutor(), *winEvent, *winPlatform);
	
	auto openResult = ipcAcceptor->Open(pipeName);

	ASSERT_TRUE(openResult);

	auto acceptorConnectionFuture = ipcAcceptor->OnConnection();

	auto connectResult = ipcConnectionClient->Connect(pipeName);

	ASSERT_TRUE(connectResult);

	winEvent->WaitForEvent(0);

	ASSERT_TRUE(acceptorConnectionFuture.GetResult());

	ASSERT_TRUE(*acceptorConnectionFuture.GetResult());

	auto ipcConnectionServer = acceptorConnectionFuture.GetResult()->Unwrap();

	ipcConnectionClient->Read()
		.Detach();

	winEvent->WaitForEvent(0);

	ipcConnectionServer->Close();

	winEvent->WaitForEvent(0);

	EXPECT_EQ(ipcConnectionClient->GetState(), Socket::EState::Closed);
}

TEST(WinIpcConnectionManualTests, connection_can_read_and_write)
{
	StrictMock<ConnectionCallbackMock>	callbackMockClient;
	StrictMock<ConnectionCallbackMock>	callbackMockServer;

	constexpr auto pipeName = "wondermake__manual_test_connection";
	const std::vector<u8> dummyDataA = { 34, 12, 198, 201 };
	const std::vector<u8> dummyDataB = { 65, 35, 212, 98 };

	auto winPlatform = MakeSystem<WinPlatformSystemImpl>(std::make_tuple());

	auto winEvent = MakeSystem<WinEventSystemImpl>(std::make_tuple(winPlatform));

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(InlineExecutor(), *winEvent, *winPlatform);
	auto ipcConnectionClient = std::make_shared<WinIpcConnection>(InlineExecutor(), *winEvent, *winPlatform);

	EXPECT_CALL(callbackMockClient, OnWrite(WriteResultMatcher(Ok())));
	EXPECT_CALL(callbackMockServer, OnRead(ReadResultMatcher(Ok(dummyDataA))));

	auto openResult = ipcAcceptor->Open(pipeName);

	ASSERT_TRUE(openResult);

	auto acceptorConnectionFuture = ipcAcceptor->OnConnection();

	auto connectResult = ipcConnectionClient->Connect(pipeName);

	ASSERT_TRUE(connectResult);

	winEvent->WaitForEvent(0);

	ASSERT_TRUE(acceptorConnectionFuture.GetResult());

	ASSERT_TRUE(*acceptorConnectionFuture.GetResult());

	auto ipcConnectionServer = acceptorConnectionFuture.GetResult()->Unwrap();

	ipcConnectionClient->Write(dummyDataA)
		.ThenRun(InlineExecutor(), FutureRunResult(onWrite(callbackMockClient)));
	ipcConnectionServer->Read()
		.ThenRun(InlineExecutor(), FutureRunResult(onRead(callbackMockServer)));

	winEvent->WaitForEvent(0);

	EXPECT_CALL(callbackMockServer, OnWrite(WriteResultMatcher(Ok())));
	EXPECT_CALL(callbackMockClient, OnRead(ReadResultMatcher(Ok(dummyDataB))));

	ipcConnectionServer->Write(dummyDataB)
		.ThenRun(InlineExecutor(), FutureRunResult(onWrite(callbackMockServer)));
	ipcConnectionClient->Read()
		.ThenRun(InlineExecutor(), FutureRunResult(onRead(callbackMockClient)));

	winEvent->WaitForEvent(0);
}
