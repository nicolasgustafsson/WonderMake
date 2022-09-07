#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketDecorator.h"

#include "WonderMakeIoTests/SocketMock.h"

class SocketDecoratorTestImplementation
	: public SocketDecorator
{
public:
	SocketDecoratorTestImplementation(SharedReference<Socket> aSocket)
		: SocketDecorator(std::move(aSocket))
	{}
};

const std::vector<u8> locDummyBuffer = { 0, 1, 2, 3 };

TEST(SocketDecoratorTests, write_calls_socket_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, Write);

	(void)decorator.Write(locDummyBuffer);
}

TEST(SocketDecoratorTests, write_forwards_buffer_correctly)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, Write(Eq(locDummyBuffer)));

	(void)decorator.Write(locDummyBuffer);
}

TEST(SocketDecoratorTests, write_return_correct_value)
{
	static constexpr auto expectedResult = Err(Socket::SWriteError{ Socket::EWriteError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeWrite>(expectedResult)));

	const auto result = decorator.Write(locDummyBuffer);

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), WriteResultMatcher(expectedResult));
}

TEST(SocketDecoratorTests, read_calls_socket_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, Read);

	(void)decorator.Read();
}

TEST(SocketDecoratorTests, read_return_correct_value)
{
	const auto expectedResult = Err(Socket::SReadError{ Socket::EReadError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(expectedResult)));

	const auto result = decorator.Read();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), ReadResultMatcher(expectedResult));
}

TEST(SocketDecoratorTests, onclose_calls_socket_function)
{
	static constexpr auto expectedResult = Err(Socket::SCloseError{ Socket::ECloseError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, OnClose)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeClose>(expectedResult)));

	const auto result = decorator.OnClose();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), CloseResultMatcher(expectedResult));
}

TEST(SocketDecoratorTests, close_calls_socket_function)
{
	static constexpr auto expectedResult = Err(Socket::SCloseError{ Socket::ECloseError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();
	
	SocketDecoratorTestImplementation decorator(socketMock);
	
	EXPECT_CALL(*socketMock, Close)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeClose>(expectedResult)));

	const auto result = decorator.Close();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), CloseResultMatcher(expectedResult));
}

TEST(SocketDecoratorTests, getstate_calls_socket_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, GetState);

	(void)decorator.GetState();
}

TEST(SocketDecoratorTests, getstate_returns_correct_value)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	SocketDecoratorTestImplementation decorator(socketMock);

	EXPECT_CALL(*socketMock, GetState)
		.WillOnce(Return(Socket::EState::Open));

	const auto state = decorator.GetState();

	EXPECT_EQ(state, Socket::EState::Open);
}
