#include "WonderMakeTestsCommon/GTestInclude.h"

#include "WonderMakeIo/SocketSerializingImpl.h"

#include "SocketMock.h"
#include "SocketSerializingMock.h"

using namespace MemoryUnitLiterals;

constexpr auto locDummyMaxBufferSize = 1_GiB;
constexpr auto locDummySerializable = [](const SerializableMock&)
{
	return std::vector<u8>();
};
constexpr auto locDummyDeserializable = [](auto&&)
{
	return Failure;
};

TEST(SocketSerializingImplTests, readmessage_returns_error_when_socket_returns_error_result)
{
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(Socket::EReadError::InternalError));

	const auto result = socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());

	ASSERT_FALSE(result);
	EXPECT_EQ(result, Socket::EReadError::InternalError);
}

TEST(SocketSerializingImplTests, readmessage_returns_success_when_socket_returns_success_result)
{
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(Socket::EAsynchronicity::Synchronous));

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
				std::move(aCallback)(dummyBuffer);

				return Socket::EAsynchronicity::Synchronous;
			});
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBuffer)))
		.WillOnce(Return(std::make_pair(SerializableMock{ 1 }, dummyBuffer.size())));
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(Eq(SerializableMock{ 1 })));

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
				std::move(aCallback)(dummyBuffer);

				return Socket::EAsynchronicity::Synchronous;
			})
		.WillOnce(Return(Socket::EAsynchronicity::Asynchronous));

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBuffer)))
		.WillOnce(Return(Failure));

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
				std::move(aCallback)(dummyBufferA);

				return Socket::EAsynchronicity::Synchronous;
			})
		.WillOnce([&dummyBufferB](auto&& aCallback)
			{
				std::move(aCallback)(dummyBufferB);

				return Socket::EAsynchronicity::Synchronous;
			});

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferA)))
		.WillOnce(Return(std::make_pair(SerializableMock{ 1 }, dummyBufferA.size())));
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferB)))
		.WillOnce(Return(std::make_pair(SerializableMock{ 2 }, dummyBufferB.size())));

	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(Eq(SerializableMock{ 1 })));
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(Eq(SerializableMock{ 2 })));
	
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
				std::move(aCallback)(dummyBufferOriginal);

				return Socket::EAsynchronicity::Synchronous;
			});

	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferOriginal)))
		.WillOnce(Return(std::make_pair(SerializableMock{ 1 }, dummyBufferA.size())));
	EXPECT_CALL(serializerMock, Deserialize(ElementsAreSpan(dummyBufferB)))
		.WillOnce(Return(std::make_pair(SerializableMock{ 2 }, dummyBufferB.size())));

	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(Eq(SerializableMock{ 1 })));
	EXPECT_CALL(socketSerializingCallbackMock, OnReadMessage(Eq(SerializableMock{ 2 })));

	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
	(void)socketSerializing->ReadMessage(socketSerializingCallbackMock.CreateOnReadMessage());
}

TEST(SocketSerializingImplTests, writemessage_returns_error_when_socket_returns_error_result)
{
	SerializableMock serializableMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(Socket::EWriteError::InternalError));

	const auto result = socketSerializing->WriteMessage(serializableMock, socketSerializingCallbackMock.CreateOnWriteMessage());

	ASSERT_FALSE(result);
	EXPECT_EQ(result, Socket::EWriteError::InternalError);
}

TEST(SocketSerializingImplTests, writemessage_returns_success_when_socket_returns_success_result)
{
	SerializableMock serializableMock;
	StrictMock<SocketSerializingCallbackMock> socketSerializingCallbackMock;
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketSerializingImpl<SerializableMock>>(locDummyMaxBufferSize, locDummySerializable, locDummyDeserializable, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(Socket::EAsynchronicity::Synchronous));

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
				std::move(aOnWrite)(Success);
			
				return Socket::EAsynchronicity::Synchronous;
			});
	EXPECT_CALL(socketSerializingCallbackMock, OnWriteMessage(Eq(Success)));

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
