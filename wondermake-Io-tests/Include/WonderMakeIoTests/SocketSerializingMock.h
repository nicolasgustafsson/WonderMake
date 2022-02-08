#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketSerializing.h"

#include "wondermake-utility/SharedReference.h"

struct SerializableMock
{
	int myIdentifier = 0;

	bool operator==(const SerializableMock&) const noexcept = default;
};

class SerializerMock
{
public:
	using ResultTypeDeserialize = Result<decltype(Failure), std::pair<SerializableMock, size_t>>;

	MOCK_METHOD(std::vector<u8>, Serialize, (const SerializableMock&));
	MOCK_METHOD(ResultTypeDeserialize, Deserialize, (std::span<const u8>));

	void DelegateToFake()
	{
		ON_CALL(*this, Serialize)
			.WillByDefault(Return(std::vector<u8>()));
		ON_CALL(*this, Deserialize)
			.WillByDefault(Return(Failure));
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
	using ResultTypeWrite = Result<Socket::EWriteError, Socket::EAsynchronicity>;
	using ResultTypeRead = Result<Socket::EReadError, Socket::EAsynchronicity>;

	MOCK_METHOD(Result<Socket::EWriteError>, WriteMessage, (const SerializableMock& aMessage, SocketSerializing<SerializableMock>::OnWriteCallback aOnWrite), (override));
	MOCK_METHOD(Result<Socket::EReadError>, ReadMessage, (SocketSerializing<SerializableMock>::OnReadCallback aOnRead), (override));

	MOCK_METHOD(void, OnClose, (Socket::OnCloseCallback aOnClose), (override));
	MOCK_METHOD(void, Close, (), (override));

	MOCK_METHOD(Socket::EState, GetState, (), (const, noexcept, override));

	void DelegateToFake()
	{
		ON_CALL(*this, WriteMessage)
			.WillByDefault(Return(Socket::EWriteError::InternalError));
		ON_CALL(*this, ReadMessage)
			.WillByDefault(Return(Socket::EReadError::InternalError));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(Socket::EState::Closed));
	}
};

class SocketSerializingCallbackMock
{
public:
	using ArgTypeOnWrite = Result<Socket::EWriteError>;
	using ArgTypeOnRead = Result<Socket::EReadError, SerializableMock>&&;
	using ArgTypeOnClose = Result<Socket::ECloseError, Socket::ECloseReason>;

	MOCK_METHOD(void, OnWriteMessage, (ArgTypeOnWrite));
	MOCK_METHOD(void, OnReadMessage, (ArgTypeOnRead));
	MOCK_METHOD(void, OnClose, (ArgTypeOnClose));

	auto CreateOnWriteMessage()
	{
		return [this](auto&& aResult) { OnWriteMessage(std::move(aResult)); };
	}
	auto CreateOnReadMessage()
	{
		return [this](auto&& aResult) { OnReadMessage(std::move(aResult)); };
	}
	auto CreateOnClose()
	{
		return [this](auto&& aResult) { OnClose(std::move(aResult)); };
	}
};
