#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketSerializing.h"

#include "wondermake-utility/SharedReference.h"

#include "WonderMakeIoTests/SocketMock.h"

struct SerializableMock
{
	int myIdentifier = 0;

	bool operator==(const SerializableMock&) const noexcept = default;
};

class SerializerMock
{
public:
	using ResultTypeDeserialize = Result<std::pair<SerializableMock, size_t>>;

	MOCK_METHOD(std::vector<u8>, Serialize, (const SerializableMock&));
	MOCK_METHOD(ResultTypeDeserialize, Deserialize, (std::span<const u8>));

	void DelegateToFake()
	{
		ON_CALL(*this, Serialize)
			.WillByDefault(Return(std::vector<u8>()));
		ON_CALL(*this, Deserialize)
			.WillByDefault(Return(Err()));
	}

	inline auto CreateSerialize()
	{
		return [this](const auto& aSerializable)
		{
			return Serialize(aSerializable);
		};
	}
	inline auto CreateDeserialize()
	{
		return [this](auto aBuffer)
		{
			return Deserialize(aBuffer);
		};
	}
};

class SocketSerializingMock
	: public SocketSerializing<SerializableMock>
{
public:
	using SSerializing = SocketSerializing<SerializableMock>;

	MOCK_METHOD(SSerializing::FutureTypeWrite, WriteMessage, (const SerializableMock& aMessage), (override));
	MOCK_METHOD(SSerializing::FutureTypeRead, ReadMessage, (), (override));

	MOCK_METHOD(SSerializing::FutureTypeClose, OnClose, (), (override));
	MOCK_METHOD(SSerializing::FutureTypeClose, Close, (), (override));

	MOCK_METHOD(Socket::EState, GetState, (), (const, noexcept, override));

	void DelegateToFake()
	{
		ON_CALL(*this, WriteMessage)
			.WillByDefault(Return(MakeCompletedFuture<SSerializing::ResultTypeWrite>(Err(Socket::SWriteError{ Socket::EWriteError::InternalError }))));
		ON_CALL(*this, ReadMessage)
			.WillByDefault(Return(MakeCompletedFuture<SSerializing::ResultTypeRead>(Err(Socket::SReadError{ Socket::EReadError::InternalError }))));
		ON_CALL(*this, OnClose)
			.WillByDefault(Return(MakeCompletedFuture<SSerializing::ResultTypeClose>(Err(Socket::SCloseError{ Socket::ECloseError::InternalError }))));
		ON_CALL(*this, Close)
			.WillByDefault(Return(MakeCompletedFuture<SSerializing::ResultTypeClose>(Err(Socket::SCloseError{ Socket::ECloseError::InternalError }))));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(Socket::EState::Closed));
	}
};

MATCHER_P(WriteMessageResultMatcher, aResult, "")
{
	Result<void, Socket::SWriteError> argResult = aResult;

	if (arg && argResult)
		return true;

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Socket::SWriteError::Error, arg.Err().Error),
				Field("Reason", &Socket::SWriteError::Reason, arg.Err().Reason)),
			argResult.Err(), result_listener);

	return false;
}
MATCHER_P(ReadMessageResultMatcher, aResult, "")
{
	Result<SerializableMock, Socket::SReadError> argResult = aResult;

	if (arg && argResult)
		return ExplainMatchResult(
			Eq(argResult.Unwrap()),
			arg.Unwrap(), result_listener);

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Socket::SReadError::Error, argResult.Err().Error),
				Field("Reason", &Socket::SReadError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}
