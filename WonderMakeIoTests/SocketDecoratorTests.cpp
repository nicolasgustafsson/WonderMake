#include "WonderMakeTestsCommon/GTestInclude.h"

#include "WonderMakeIo/SocketDecorator.h"

#include "SocketMock.h"

class SocketDecoratorTestImplementation
	: public SocketDecorator
{
public:
	SocketDecoratorTestImplementation(SharedReference<Socket> aSocket)
		: SocketDecorator(std::move(aSocket))
	{}
};

const std::vector<u8> locDummyBuffer = { 0, 1, 2, 3 };
constexpr auto locDummyCallback = [](auto&&) {};

TEST(SocketDecoratorTests, write_calls_socket_function)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Write);

	(void)decorator.Write(locDummyBuffer, locDummyCallback);
}

TEST(SocketDecoratorTests, write_forwards_buffer_correctly)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Write(Eq(locDummyBuffer), _));

	(void)decorator.Write(locDummyBuffer, locDummyCallback);
}

TEST(SocketDecoratorTests, write_return_correct_value)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(Socket::EAsynchronicity::Synchronous));

	const auto result = decorator.Write(locDummyBuffer, locDummyCallback);

	EXPECT_EQ(result, Socket::EAsynchronicity::Synchronous);
}

TEST(SocketDecoratorTests, write_forwards_correct_callback)
{
	SocketCallbackMock callback;

	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Write)
		.WillOnce([](auto&&, auto&& aCallback)
			{
				std::move(aCallback)(Socket::EWriteError::InternalError);

				return Socket::EAsynchronicity::Synchronous;
			});
	EXPECT_CALL(callback, OnWrite(Eq(Socket::EWriteError::InternalError)));

	(void)decorator.Write(locDummyBuffer, callback.CreateOnWrite());
}

TEST(SocketDecoratorTests, read_calls_socket_function)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Read);

	(void)decorator.Read(locDummyCallback);
}

TEST(SocketDecoratorTests, read_return_correct_value)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(Socket::EAsynchronicity::Synchronous));

	const auto result = decorator.Read(locDummyCallback);

	EXPECT_EQ(result, Socket::EAsynchronicity::Synchronous);
}

TEST(SocketDecoratorTests, read_forwards_correct_callback)
{
	SocketCallbackMock callback;

	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Read)
		.WillOnce([](auto&& aCallback)
			{
				std::move(aCallback)(Socket::EReadError::InternalError);

				return Socket::EAsynchronicity::Synchronous;
			});
	EXPECT_CALL(callback, OnRead(Eq(Socket::EReadError::InternalError)));

	(void)decorator.Read(callback.CreateOnRead());
}

TEST(SocketDecoratorTests, onclose_calls_socket_function)
{
	SocketCallbackMock callback;

	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, OnClose)
		.WillOnce([](auto&& aCallback)
			{
				std::move(aCallback)(Socket::ECloseError::InternalError);
			});
	EXPECT_CALL(callback, OnClose(Eq(Socket::ECloseError::InternalError)));

	decorator.OnClose(callback.CreateOnClose());
}

TEST(SocketDecoratorTests, close_calls_socket_function)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, Close);

	decorator.Close();
}

TEST(SocketDecoratorTests, getstate_calls_socket_function)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, GetState);

	(void)decorator.GetState();
}

TEST(SocketDecoratorTests, getstate_returns_correct_value)
{
	auto socketMock = std::make_shared<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(SharedReference<Socket>::FromPointer(socketMock));

	EXPECT_CALL(*socketMock, GetState)
		.WillOnce(Return(Socket::EState::Open));

	const auto state = decorator.GetState();

	EXPECT_EQ(state, Socket::EState::Open);
}
