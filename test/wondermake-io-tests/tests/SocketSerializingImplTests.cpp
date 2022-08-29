#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketSerializingImpl.h"

#include "WonderMakeIoTests/SocketMock.h"
#include "WonderMakeIoTests/SocketSerializingMock.h"

using namespace MemoryUnitLiterals;

constexpr auto locDummyMaxBufferSize = 1_GiB;
constexpr auto locDummySerializable = [](const SerializableMock&)
{
	return std::vector<u8>();
};
constexpr auto locDummyDeserializable = [](auto&&)
{
	return Err();
};

TEST(SocketSerializingImplTests, readmessage_returns_error_when_socket_returns_error_result)
{
	constexpr auto expectedError = Socket::EReadError::InternalError;
	constexpr u64 expectedReason = 1234;

	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(Err(Socket::SReadError{ expectedError, expectedReason })));

	const auto result = socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());

	ASSERT_FALSE(result);

	EXPECT_EQ(result.Err().Error, expectedError);
	EXPECT_EQ(result.Err().Reason, expectedReason);
}

TEST(SocketSerializingImplTests, readmessage_returns_success_when_socket_returns_success_result)
{
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(Ok(Socket::EAsynchronicity::Synchronous)));

	const auto result = socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());

	ASSERT_TRUE(result);
}

TEST(SocketSerializingImplTests, readmessage_calls_callback_with_correct_buffer)
{
	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1 };

	NiceMock<SerializerMock> serializerMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce([&dummyBuffer](auto&& aCallback)
			{
				std::move(aCallback)(Ok(dummyBuffer));

				return Ok(Socket::EAsynchronicity::Synchronous);
			});
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBuffer)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 1 }, dummyBuffer.size()))));
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(ReadMessageResultMatcher(Ok(SerializableMock{ 1 }))));

	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
}

TEST(SocketSerializingImplTests, readmessage_do_not_call_callback_with_when_deserialize_fail)
{
	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1 };

	NiceMock<SerializerMock> serializerMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce([&dummyBuffer](auto&& aCallback)
			{
				std::move(aCallback)(Ok(dummyBuffer));

				return Ok(Socket::EAsynchronicity::Synchronous);
			})
		.WillOnce(Return(Ok(Socket::EAsynchronicity::Asynchronous)));

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBuffer)))
		.WillOnce(Return(Err()));

	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
}

TEST(SocketSerializingImplTests, readmessage_is_able_to_deserialize_two_messages_from_two_buffers)
{
	const std::vector<u8> dummyBufferA = { 4, 2, 3, 1 };
	const std::vector<u8> dummyBufferB = { 5, 8, 7 };

	NiceMock<SerializerMock> serializerMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce([&dummyBufferA](auto&& aCallback)
			{
				std::move(aCallback)(Ok(dummyBufferA));

				return Ok(Socket::EAsynchronicity::Synchronous);
			})
		.WillOnce([&dummyBufferB](auto&& aCallback)
			{
				std::move(aCallback)(Ok(dummyBufferB));

				return Ok(Socket::EAsynchronicity::Synchronous);
			});

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferA)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 1 }, dummyBufferA.size()))));
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferB)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 2 }, dummyBufferB.size()))));

	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(ReadMessageResultMatcher(Ok(SerializableMock{ 1 }))));
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(ReadMessageResultMatcher(Ok(SerializableMock{ 2 }))));
	
	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
}

TEST(SocketSerializingImplTests, readmessage_is_able_to_deserialize_two_messages_from_one_buffer)
{
	const std::vector<u8> dummyBufferOriginal = { 4, 2, 3, 1, 5, 8, 7 };
	const std::vector<u8> dummyBufferA = { 4, 2, 3, 1 };
	const std::vector<u8> dummyBufferB = { 5, 8, 7 };

	NiceMock<SerializerMock> serializerMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce([&dummyBufferOriginal](auto&& aCallback)
			{
				std::move(aCallback)(Ok(dummyBufferOriginal));

				return Ok(Socket::EAsynchronicity::Synchronous);
			});

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferOriginal)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 1 }, dummyBufferA.size()))));
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferB)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 2 }, dummyBufferB.size()))));

	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(ReadMessageResultMatcher(Ok(SerializableMock{ 1 }))));
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(ReadMessageResultMatcher(Ok(SerializableMock{ 2 }))));

	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
}

TEST(SocketSerializingImplTests, readmessage_calls_onreadmessage_with_outofmemory_error_when_buffer_is_full)
{
	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1, 5, 9, 8, 6, 0, 7 };
	constexpr auto maxBufferSize = 5_B;

	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(maxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce([&dummyBuffer](auto&& aCallback)
			{
				std::move(aCallback)(Ok(dummyBuffer));

				return Ok(Socket::EAsynchronicity::Synchronous);
			});
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(ReadMessageResultMatcher(Err(Socket::SReadError{ Socket::EReadError::MessageToBig }))));

	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
}

TEST(SocketSerializingImplTests, writemessage_returns_error_when_socket_returns_error_result)
{
	constexpr auto expectedError = Socket::EWriteError::InternalError;
	constexpr u64 expectedReason = 1234;

	SerializableMock serializableMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(Err(Socket::SWriteError{ expectedError, expectedReason })));

	const auto result = socketSerializing->WriteMessage(serializableMock, socketSerializingCallbackMock.CreateOnWriteMessage());

	ASSERT_FALSE(result);

	EXPECT_EQ(result.Err().Error, expectedError);
	EXPECT_EQ(result.Err().Reason, expectedReason);
}

TEST(SocketSerializingImplTests, writemessage_returns_success_when_socket_returns_success_result)
{
	SerializableMock serializableMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(Ok(Socket::EAsynchronicity::Synchronous)));

	const auto result = socketSerializing->WriteMessage(serializableMock, socketSerializingCallbackMock.CreateOnWriteMessage());

	ASSERT_TRUE(result);
}

TEST(SocketSerializingImplTests, writemessage_calls_callback_with_correct_buffer)
{
	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1 };

	SerializableMock serializableMock = { 1 };
	NiceMock<SerializerMock> serializerMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, serializerMock.CreateSerialize(), locDummyDeserializable, socketMock);

	EXPECT_CALL(serializerMock, Serialize(serializableMock))
		.WillOnce(Return(dummyBuffer));
	EXPECT_CALL(*socketMock, Write(dummyBuffer, _))
		.WillOnce([](auto&&, auto&& aOnWrite)
			{
				std::move(aOnWrite)(Ok());
			
				return Ok(Socket::EAsynchronicity::Synchronous);
			});
	EXPECT_CALL(socketSerializingCallbackMock, OnWriteMessage(WriteMessageResultMatcher(Ok())));

	(void)socketSerializing->WriteMessage(serializableMock, socketSerializingCallbackMock.CreateOnWriteMessage());
}

TEST(SocketSerializingImplTests, onclose_call_correct_function)
{
	constexpr auto dummyOnClose = [](auto&&) {};

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, OnClose);

	socketSerializing->OnClose(dummyOnClose);
}

TEST(SocketSerializingImplTests, close_call_correct_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Close);

	socketSerializing->Close();
}
