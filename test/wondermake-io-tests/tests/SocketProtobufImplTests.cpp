#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketProtobufImpl.h"

#include "WonderMakeIoTests/SocketMock.h"

#pragma warning(push)
#pragma warning(disable: 5054)
#include "wondermake-io-tests/protobuf/SocketProtobufTest.pb.h"
#pragma warning(pop)

using ProtoSocketProtobufTest::SocketProtobufTestRead;
using ProtoSocketProtobufTest::SocketProtobufTestWrite;

using namespace MemoryUnitLiterals;

using SocketProtobufType = SocketProtobufImpl<SocketProtobufTestWrite, SocketProtobufTestRead>;

MATCHER_P(ProtobufReadMessageMatcher, aResult, "")
{
	Result<SocketProtobufTestRead, Socket::SReadError> argResult = aResult;

	if (arg && argResult)
		return arg.Unwrap().SerializeAsString() == argResult.Unwrap().SerializeAsString();

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Socket::SReadError::Error, argResult.Err().Error),
				Field("Reason", &Socket::SReadError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}
MATCHER_P(ProtobufWriteMessageMatcher, aResult, "")
{
	Result<void, Socket::SWriteError> argResult = aResult;

	if (arg && argResult)
		return true;

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Socket::SWriteError::Error, argResult.Err().Error),
				Field("Reason", &Socket::SWriteError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}

const auto CreateDummyProtoRead(std::string aText)
{
	SocketProtobufTestRead dummyProto;

	dummyProto.set_test_string(std::move(aText));

	return dummyProto;
}
const auto CreateDummyProtoWrite(std::string aText)
{
	SocketProtobufTestWrite dummyProto;

	dummyProto.set_test_string(std::move(aText));

	return dummyProto;
}
template<typename TProto>
std::vector<u8> ProtoToBuffer(const TProto& aProto)
{
	const u32 size = static_cast<u32>(aProto.ByteSizeLong());

	std::vector<u8> serializedData(sizeof(size) + size, 0);

	*reinterpret_cast<u32*>(serializedData.data()) = ToEndianNetwork(size);

	aProto.SerializeToArray(serializedData.data() + sizeof(size), static_cast<int>(serializedData.size()));

	return serializedData;
}

inline constexpr auto locDummyMaxBufferSize	= 1_GiB;
inline constexpr auto locDummyTestText		= "test_string";

const auto locDummyProtoRead				= CreateDummyProtoRead(locDummyTestText);
const auto locDummyProtoWrite				= CreateDummyProtoWrite(locDummyTestText);
const auto locDummyBufferRead				= ProtoToBuffer(CreateDummyProtoRead(locDummyTestText));
const auto locDummyBufferWrite				= ProtoToBuffer(CreateDummyProtoWrite(locDummyTestText));

TEST(SocketProtobufImplTests, readmessage_returns_non_completed_future_when_socket_read_retuns_non_completed_future)
{
	auto [_, future] = MakeAsync<Socket::ResultTypeRead>();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(future));

	const auto futureRead = socketSerializing->ReadMessage();

	ASSERT_TRUE(futureRead.IsValid());
	ASSERT_FALSE(futureRead.IsCompleted());
	ASSERT_FALSE(futureRead.IsCanceled());
}

TEST(SocketProtobufImplTests, readmessage_returns_error_when_socket_returns_error_result)
{
	static constexpr auto expectedResult = Err(Socket::SReadError{ Socket::EReadError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(expectedResult)));

	const auto futureRead = socketSerializing->ReadMessage();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ProtobufReadMessageMatcher(expectedResult));
}

TEST(SocketProtobufImplTests, readmessage_returns_protobuf_object_when_socket_returns_serialized_protobuf_object)
{
	const auto expectedSocketResult = Ok(locDummyBufferRead);
	const auto expectedResult = Ok(locDummyProtoRead);

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(expectedSocketResult)));

	const auto futureRead = socketSerializing->ReadMessage();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ProtobufReadMessageMatcher(expectedResult));
}

TEST(SocketProtobufImplTests, readmessage_returns_valid_non_completed_future_when_socket_returns_invalid_data)
{
	const std::vector<u8> dummyBuffer = { 0 };

	const auto expectedSocketResult = Ok(dummyBuffer);
	
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);
	auto [_, future] = MakeAsync<Socket::ResultTypeRead>();

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(expectedSocketResult)))
		.WillOnce(Return(future));

	const auto futureRead = socketSerializing->ReadMessage();

	ASSERT_TRUE(futureRead.IsValid());
	ASSERT_FALSE(futureRead.IsCompleted());
	ASSERT_FALSE(futureRead.IsCanceled());
}

TEST(SocketProtobufImplTests, readmessage_is_able_to_deserialize_two_protobuf_objects_from_two_messages_from_two_buffers)
{
	const auto dummyProtoA = CreateDummyProtoRead("TEST_A");
	const auto dummyProtoB = CreateDummyProtoRead("TEST_B");

	const auto dummyBufferA = ProtoToBuffer(dummyProtoA);
	const auto dummyBufferB = ProtoToBuffer(dummyProtoB);

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBufferA))))
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBufferB))));

	const auto futureReadA = socketSerializing->ReadMessage();
	const auto futureReadB = socketSerializing->ReadMessage();

	ASSERT_TRUE(futureReadA.GetResult());
	ASSERT_TRUE(futureReadB.GetResult());

	EXPECT_THAT(*futureReadA.GetResult(), ProtobufReadMessageMatcher(Ok(dummyProtoA)));
	EXPECT_THAT(*futureReadB.GetResult(), ProtobufReadMessageMatcher(Ok(dummyProtoB)));
}

TEST(SocketProtobufImplTests, readmessage_returns_messagetobig_error_when_buffer_is_full)
{
	const std::vector<u8> dummyBuffer = { 4, 2, 3, 1, 5, 9, 8, 6, 0, 7 };
	static constexpr auto maxBufferSize = 5_B;
	static constexpr auto expectedResult = Err(Socket::SReadError{ Socket::EReadError::MessageToBig });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), maxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Read)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeRead>(Ok(dummyBuffer))));

	const auto futureRead = socketSerializing->ReadMessage();

	ASSERT_TRUE(futureRead.GetResult());

	EXPECT_THAT(*futureRead.GetResult(), ProtobufReadMessageMatcher(expectedResult));
}

TEST(SocketProtobufImplTests, writemessage_returns_non_completed_future_when_socket_write_retuns_non_completed_future)
{
	auto [_, future] = MakeAsync<Socket::ResultTypeWrite>();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(future));

	const auto futureWrite = socketSerializing->WriteMessage(locDummyProtoWrite);

	ASSERT_TRUE(futureWrite.IsValid());
	ASSERT_FALSE(futureWrite.IsCompleted());
	ASSERT_FALSE(futureWrite.IsCanceled());
}

TEST(SocketProtobufImplTests, writemessage_returns_error_when_socket_returns_error_result)
{
	static constexpr auto expectedResult = Err(Socket::SWriteError{ Socket::EWriteError::InternalError, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeWrite>(expectedResult)));

	const auto futureWrite = socketSerializing->WriteMessage(locDummyProtoWrite);

	ASSERT_TRUE(futureWrite.GetResult());

	EXPECT_THAT(*futureWrite.GetResult(), ProtobufWriteMessageMatcher(expectedResult));
}

TEST(SocketProtobufImplTests, writemessage_returns_success_when_socket_returns_success_result)
{
	static constexpr auto expectedResult = Ok();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Write)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeWrite>(expectedResult)));

	const auto resultWrite = socketSerializing->WriteMessage(locDummyProtoWrite);

	ASSERT_TRUE(resultWrite.GetResult());

	EXPECT_THAT(*resultWrite.GetResult(), ProtobufWriteMessageMatcher(expectedResult));
}

TEST(SocketProtobufImplTests, writemessage_calls_socket_write_with_correct_buffer)
{
	static constexpr auto expectedResult = Ok();

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Write(locDummyBufferWrite));

	(void)socketSerializing->WriteMessage(locDummyProtoWrite);
}

TEST(SocketProtobufImplTests, onclose_call_correct_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, OnClose);

	(void)socketSerializing->OnClose();
}

TEST(SocketProtobufImplTests, onclose_returns_correct_value)
{
	static constexpr auto expectedResult = Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedLocally, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, OnClose)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeClose>(expectedResult)));

	const auto resultClose = socketSerializing->OnClose();

	ASSERT_TRUE(resultClose.GetResult());

	EXPECT_THAT(*resultClose.GetResult(), CloseResultMatcher(expectedResult));
}

TEST(SocketProtobufImplTests, close_call_correct_function)
{
	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Close);

	(void)socketSerializing->Close();
}

TEST(SocketProtobufImplTests, close_returns_correct_value)
{
	static constexpr auto expectedResult = Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedLocally, 1234 });

	auto socketMock = MakeSharedReference<NiceMock<SocketMock>>();

	socketMock->DelegateToFake();

	auto socketSerializing = MakeSharedReference<SocketProtobufType>(InlineExecutor(), locDummyMaxBufferSize, socketMock);

	EXPECT_CALL(*socketMock, Close)
		.WillOnce(Return(MakeCompletedFuture<Socket::ResultTypeClose>(expectedResult)));

	const auto resultClose = socketSerializing->Close();

	ASSERT_TRUE(resultClose.GetResult());

	EXPECT_THAT(*resultClose.GetResult(), CloseResultMatcher(expectedResult));
}
