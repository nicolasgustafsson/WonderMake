#include "WonderMakeTestsCommon/GTestInclude.h"

#include "WonderMakeIo/Socket.h"

class SocketMock
	: public Socket
{
public:
	using ResultTypeWrite	= Result<EWriteError, EAsynchronicity>;
	using ResultTypeRead	= Result<EReadError, EAsynchronicity>;

	MOCK_METHOD(ResultTypeWrite, Write, (std::vector<u8> aBuffer, OnWriteCallback aOnWrite), (override));
	MOCK_METHOD(ResultTypeRead, Read, (OnReadCallback aOnRead), (override));
	MOCK_METHOD(void, OnClose, (OnCloseCallback aOnClose), (override));
	MOCK_METHOD(void, Close, (), (override));

	MOCK_METHOD(EState, GetState, (), (const, noexcept, override));

	void DelegateToFake()
	{
		ON_CALL(*this, Write)
			.WillByDefault(Return(EWriteError::InternalError));
		ON_CALL(*this, Read)
			.WillByDefault(Return(EReadError::InternalError));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(EState::Closed));
	}
};

class SocketCallbackMock
{
public:
	using ArgTypeOnWrite = Result<Socket::EWriteError>;
	using ArgTypeOnRead = Result<Socket::EReadError, std::vector<u8>>&&;
	using ArgTypeOnClose = Result<Socket::ECloseError, Socket::ECloseReason>;

	MOCK_METHOD(void, OnWrite, (ArgTypeOnWrite));
	MOCK_METHOD(void, OnRead, (ArgTypeOnRead));
	MOCK_METHOD(void, OnClose, (ArgTypeOnClose));

	auto CreateOnWrite()
	{
		return [this](auto&& aResult) { OnWrite(std::move(aResult)); };
	}
	auto CreateOnRead()
	{
		return [this](auto&& aResult) { OnRead(std::move(aResult)); };
	}
	auto CreateOnClose()
	{
		return [this](auto&& aResult) { OnClose(std::move(aResult)); };
	}
};
