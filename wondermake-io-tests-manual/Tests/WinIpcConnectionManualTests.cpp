#include "wondermake-tests-common/GTestInclude.h"

#include "WinEventSystemImpl.h"
#include "WinPlatformSystemImpl.h"

#include "WinIpcAcceptor.h"
#include "WinIpcConnection.h"

class AcceptorCallbackMock
{
public:
	MOCK_METHOD(void, OnConnection, (std::shared_ptr<IpcConnection>&& aConnection), ());
};
class ConnectionCallbackMock
{
public:
	using WriteResult = Result<Socket::EWriteError>;
	using ReadResult = Result<Socket::EReadError, std::vector<u8>>;

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
	WinPlatformSystemImpl::InjectDependencies(std::tie());

	WinPlatformSystemImpl winPlatform;

	WinEventSystemImpl::InjectDependencies(std::tie(winPlatform));

	WinEventSystemImpl winEvent;

	auto ipcConnection = std::make_shared<WinIpcConnection>(winEvent, winPlatform);

	auto connectResult = ipcConnection->Connect("wondermake__manual_test_connection");

	ASSERT_FALSE(connectResult);
	EXPECT_EQ(connectResult, IpcConnection::ConnectionError::NoConnection);
}

TEST(WinIpcConnectionManualTests, connection_connects_when_acceptor_exists)
{
	WinPlatformSystemImpl::InjectDependencies(std::tie());

	WinPlatformSystemImpl winPlatform;

	WinEventSystemImpl::InjectDependencies(std::tie(winPlatform));

	WinEventSystemImpl winEvent;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(winEvent, winPlatform);
	auto ipcConnection = std::make_shared<WinIpcConnection>(winEvent, winPlatform);

	auto openResult = ipcAcceptor->Open("wondermake__manual_test_connection", {});

	ASSERT_TRUE(openResult);

	auto connectResult = ipcConnection->Connect("wondermake__manual_test_connection");

	ASSERT_TRUE(connectResult);

	winEvent.WaitForEvent(0);
}

TEST(WinIpcConnectionManualTests, acceptor_accepts_multiple_connections)
{
	WinPlatformSystemImpl::InjectDependencies(std::tie());

	WinPlatformSystemImpl winPlatform;

	WinEventSystemImpl::InjectDependencies(std::tie(winPlatform));

	WinEventSystemImpl winEvent;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(winEvent, winPlatform);
	auto ipcConnectionA = std::make_shared<WinIpcConnection>(winEvent, winPlatform);
	auto ipcConnectionB = std::make_shared<WinIpcConnection>(winEvent, winPlatform);

	auto openResult = ipcAcceptor->Open("wondermake__manual_test_connection", {});

	ASSERT_TRUE(openResult);

	auto connectResultA = ipcConnectionA->Connect("wondermake__manual_test_connection");

	ASSERT_TRUE(connectResultA);

	winEvent.WaitForEvent(0);

	auto connectResultB = ipcConnectionB->Connect("wondermake__manual_test_connection");

	ASSERT_TRUE(connectResultB);

	winEvent.WaitForEvent(0);
}

TEST(WinIpcConnectionManualTests, connection_closes_when_other_connection_is_closed)
{
	StrictMock<AcceptorCallbackMock>	callbackMockAcceptor;

	constexpr auto pipeName = "wondermake__manual_test_connection";

	WinPlatformSystemImpl::InjectDependencies(std::tie());

	WinPlatformSystemImpl winPlatform;

	WinEventSystemImpl::InjectDependencies(std::tie(winPlatform));

	WinEventSystemImpl winEvent;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(winEvent, winPlatform);
	auto ipcConnectionClient = std::make_shared<WinIpcConnection>(winEvent, winPlatform);
	std::shared_ptr<IpcConnection> ipcConnectionServer;

	EXPECT_CALL(callbackMockAcceptor, OnConnection)
		.WillOnce([&ipcConnectionServer](auto&& aConnection)
			{
				ipcConnectionServer = std::move(aConnection);
			});

	auto openResult = ipcAcceptor->Open(pipeName, { onConnection(callbackMockAcceptor) });

	ASSERT_TRUE(openResult);

	auto connectResult = ipcConnectionClient->Connect(pipeName);

	ASSERT_TRUE(connectResult);

	winEvent.WaitForEvent(0);

	ipcConnectionClient->Read([](auto&&) {});

	winEvent.WaitForEvent(0);

	ipcConnectionServer->Close();

	winEvent.WaitForEvent(0);

	EXPECT_EQ(ipcConnectionClient->GetState(), Socket::EState::Closed);
}

TEST(WinIpcConnectionManualTests, connection_can_read_and_write)
{
	StrictMock<AcceptorCallbackMock>	callbackMockAcceptor;
	StrictMock<ConnectionCallbackMock>	callbackMockClient;
	StrictMock<ConnectionCallbackMock>	callbackMockServer;

	constexpr auto pipeName = "wondermake__manual_test_connection";
	const std::vector<u8> dummyDataA = { 34, 12, 198, 201 };
	const std::vector<u8> dummyDataB = { 34, 12, 198, 201 };

	WinPlatformSystemImpl::InjectDependencies(std::tie());

	WinPlatformSystemImpl winPlatform;

	WinEventSystemImpl::InjectDependencies(std::tie(winPlatform));

	WinEventSystemImpl winEvent;

	auto ipcAcceptor = std::make_shared<WinIpcAcceptor>(winEvent, winPlatform);
	auto ipcConnectionClient = std::make_shared<WinIpcConnection>(winEvent, winPlatform);
	std::shared_ptr<IpcConnection> ipcConnectionServer;

	EXPECT_CALL(callbackMockAcceptor, OnConnection)
		.WillOnce([&ipcConnectionServer](auto&& aConnection)
			{
				ipcConnectionServer = std::move(aConnection);
			});
	EXPECT_CALL(callbackMockClient, OnWrite(Eq(Success)));
	EXPECT_CALL(callbackMockServer, OnRead(Eq(dummyDataA)));

	auto openResult = ipcAcceptor->Open(pipeName, { onConnection(callbackMockAcceptor) });

	ASSERT_TRUE(openResult);

	auto connectResult = ipcConnectionClient->Connect(pipeName);

	ASSERT_TRUE(connectResult);

	winEvent.WaitForEvent(0);

	ASSERT_TRUE(ipcConnectionServer);

	ipcConnectionClient->Write(dummyDataA, onWrite(callbackMockClient));
	ipcConnectionServer->Read(onRead(callbackMockServer));

	winEvent.WaitForEvent(0);

	EXPECT_CALL(callbackMockServer, OnWrite(Eq(Success)));
	EXPECT_CALL(callbackMockClient, OnRead(Eq(dummyDataB)));

	ipcConnectionServer->Write(dummyDataB, onWrite(callbackMockServer));
	ipcConnectionClient->Read(onRead(callbackMockClient));

	winEvent.WaitForEvent(0);
}
