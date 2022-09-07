#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketSerializingImpl.h"

#include "WonderMakeIoTests/SocketMock.h"
#include "WonderMakeIoTests/SocketSerializingMock.h"

using namespace MemoryUnitLiterals;

inline constexpr auto locDummyMaxBufferSize = 1_GiB;
inline constexpr auto locDummySerializable = [](const SerializableMock&)
{
	return std::vector<u8>();
};
inline constexpr auto locDummyDeserializable = [](auto&&)
{
	return Err();
};

TEST(SocketSerializingImplTests, readmessage_returns_non_completed_future_when_socket_read_retuns_non_completed_future)
{
	const std::vector<u8> dummyBuffer = { 1, 2, 3, 4 };
	auto [_, future] = MakeAsync<Socket::ResultTypeRead>();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(future));

	const auto result = socketSerializing->ReadMessage();

	ASSERT_FALSE(result.GetResult());
}

TEST(SocketSerializingImplTests, readmessage_returns_error_when_socket_returns_error_result)
{
	static constexpr auto expectedResult = Err(Socket::SReadError{ Socket::EReadError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(expectedResult)));

	const auto result = socketSerializing->ReadMessage();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), ReadMessageResultMatcher(expectedResult));
}

TEST(SocketSerializingImplTests, readmessage_returns_serializable_object_when_socket_and_deserializer_returns_success_result)
{
	const std::vector<u8> dummyBuffer = { 1, 2, 3, 4 };

	const auto expectedReadResult = Ok(dummyBuffer);
	static constexpr auto expectedResult = Ok(SerializableMock{ 1234 });

	NiceMock<SerializerMock> serializerMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(expectedReadResult)));
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBuffer)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 1234 }, dummyBuffer.size()))));

	const auto result = socketSerializing->ReadMessage();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), ReadMessageResultMatcher(expectedResult));
}

TEST(SocketSerializingImplTests, readmessage_returns_non_completed_future_when_deserializer_returns_error_result_and_socket_read_retuns_non_completed_future)
{
	const std::vector<u8> dummyBuffer = { 1, 2, 3, 4 };
	auto [_, future] = MakeAsync<Socket::ResultTypeRead>();

	NiceMock<SerializerMock> serializerMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBuffer))))
		.WillOnce(Return(future));
	EXPECT_CALL(serializerMock, Deserialize)
		.WillOnce(Return(Err()));

	const auto result = socketSerializing->ReadMessage();

	ASSERT_FALSE(result.GetResult());
}

TEST(SocketSerializingImplTests, readmessage_is_able_to_deserialize_two_messages_from_two_buffers)
{
	const std::vector<u8> dummyBufferA = { 4, 2, 3, 1 };
	const std::vector<u8> dummyBufferB = { 5, 8, 7 };

	static constexpr auto expectedSerializableA = SerializableMock{ 1 };
	static constexpr auto expectedSerializableB = SerializableMock{ 2 };

	static constexpr auto expectedResultA = Ok(expectedSerializableA);
	static constexpr auto expectedResultB = Ok(expectedSerializableB);

	NiceMock<SerializerMock> serializerMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, serializerMock.CreateDeserialize(), socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBufferA))))
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBufferB))));

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferA)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 1 }, dummyBufferA.size()))));
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferB)))
		.WillOnce(Return(Ok(std::make_pair(SerializableMock{ 2 }, dummyBufferB.size()))));

	const auto resultA = socketSerializing->ReadMessage();
	const auto resultB = socketSerializing->ReadMessage();

	ASSERT_TRUE(resultA.GetResult());
	ASSERT_TRUE(resultB.GetResult());

	EXPECT_THAT(*resultA.GetResult(), ReadMessageResultMatcher(expectedResultA));
	EXPECT_THAT(*resultB.GetResult(), ReadMessageResultMatcher(expectedResultB));
}

TEST(SocketSerializingImplTests, readmessage_returns_messagetobig_error_when_buffer_is_full)
{
	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1, 5, 9, 8, 6, 0, 7 };
	static constexpr auto maxBufferSize = 5_B;
	static constexpr auto expectedResult = Err(Socket::SReadError{ Socket::EReadError::MessageToBig });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), maxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBuffer))));

	const auto result = socketSerializing->ReadMessage();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), ReadMessageResultMatcher(expectedResult));
}

TEST(SocketSerializingImplTests, writemessage_returns_non_completed_future_when_socket_write_retuns_non_completed_future)
{
	auto [_, future] = MakeAsync<Socket::ResultTypeWrite>();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(future));

	const auto result = socketSerializing->WriteMessage(SerializableMock());

	ASSERT_FALSE(result.GetResult());
}

TEST(SocketSerializingImplTests, writemessage_returns_error_when_socket_returns_error_result)
{
	static constexpr auto expectedResult = Err(Socket::SWriteError{ Socket::EWriteError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeWrite>(expectedResult)));

	const auto result = socketSerializing->WriteMessage(SerializableMock());

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), WriteMessageResultMatcher(expectedResult));
}

TEST(SocketSerializingImplTests, writemessage_returns_success_when_socket_returns_success_result)
{
	static constexpr auto expectedResult = Ok();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeWrite>(expectedResult)));

	const auto result = socketSerializing->WriteMessage(SerializableMock());

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), WriteMessageResultMatcher(expectedResult));
}

TEST(SocketSerializingImplTests, writemessage_calls_socket_write_with_correct_buffer)
{
	static constexpr auto expectedResult = Ok();

	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1 };

	SerializableMock serializableMock = { 1 };
	NiceMock<SerializerMock> serializerMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	serializerMock.DelegateToFake();
	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, serializerMock.CreateSerialize(), locDummyDeserializable, socketMock);

	EXPECT_CALL(serializerMock, Serialize(serializableMock))
		.WillOnce(Return(dummyBuffer));
	EXPECT_CALL(*socketMock, Write(dummyBuffer));

	(void)socketSerializing->WriteMessage(serializableMock);
}

TEST(SocketSerializingImplTests, onclose_call_correct_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, OnClose);

	(void)socketSerializing->OnClose();
}

TEST(SocketSerializingImplTests, onclose_returns_correct_value)
{
	static constexpr auto expectedResult = Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedLocally, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, OnClose)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeClose>(expectedResult)));

	const auto result = socketSerializing->OnClose();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), CloseResultMatcher(expectedResult));
}

TEST(SocketSerializingImplTests, close_call_correct_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Close);

	(void)socketSerializing->Close();
}

TEST(SocketSerializingImplTests, close_returns_correct_value)
{
	static constexpr auto expectedResult = Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedLocally, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(InlineExecutor(), locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Close)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeClose>(expectedResult)));

	const auto result = socketSerializing->Close();

	ASSERT_TRUE(result.GetResult());

	EXPECT_THAT(*result.GetResult(), CloseResultMatcher(expectedResult));
}
