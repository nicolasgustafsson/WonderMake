#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/Socket.h"

class SocketMock
	: public Socket
{
public:
	using ResultTypeWrite	= Result<EAsynchronicity, SWriteError>;
	using ResultTypeRead	= Result<EAsynchronicity, SReadError>;

	MOCK_METHOD(ResultTypeWrite, Write, (std::vector<u8> aBuffer, OnWriteCallback aOnWrite), (override));
	MOCK_METHOD(ResultTypeRead, Read, (OnReadCallback aOnRead), (override));
	MOCK_METHOD(void, OnClose, (OnCloseCallback aOnClose), (override));
	MOCK_METHOD(void, Close, (), (override));

	MOCK_METHOD(EState, GetState, (), (const, noexcept, override));

	void DelegateToFake()
	{
		ON_CALL(*this, Write)
			.WillByDefault(Return(Err(SWriteError{ EWriteError::InternalError })));
		ON_CALL(*this, Read)
			.WillByDefault(Return(Err(SReadError{ EReadError::InternalError })));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(EState::Closed));
	}
};

class SocketCallbackMock
{
public:
	using ArgTypeOnWrite = Result<void, Socket::SWriteError>;
	using ArgTypeOnRead = Result<std::vector<u8>, Socket::SReadError>&&;
	using ArgTypeOnClose = Result<Socket::SCloseLocation, Socket::SCloseError>;

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

inline std::ostream& operator<<(std::ostream& out, const Socket::SWriteError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}
inline std::ostream& operator<<(std::ostream& out, const Socket::SReadError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}
inline std::ostream& operator<<(std::ostream& out, const Socket::SCloseError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}

MATCHER_P(WriteResultMatcher, aResult, "")
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
MATCHER_P(ReadResultMatcher, aResult, "")
{
	Result<std::vector<u8>, Socket::SReadError> argResult = aResult;

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
MATCHER_P(CloseResultMatcher, aResult, "")
{
	Result<Socket::SCloseLocation, Socket::SCloseError> argResult = aResult;

	if (arg && argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Socket::SCloseLocation::Location, argResult.Unwrap().Location),
				Field("Reason", &Socket::SCloseLocation::Reason, argResult.Unwrap().Reason)),
			arg.Unwrap(), result_listener);

	if(!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &Socket::SCloseError::Error, argResult.Err().Error),
				Field("Reason", &Socket::SCloseError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}
