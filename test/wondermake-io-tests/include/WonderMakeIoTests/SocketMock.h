#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/Socket.h"

class SocketMock
	: public Socket
{
public:
	MOCK_METHOD(FutureTypeWrite, Write, (std::vector<u8> aBuffer), (override));
	MOCK_METHOD(FutureTypeRead, Read, (), (override));
	MOCK_METHOD(FutureTypeClose, OnClose, (), (override));
	MOCK_METHOD(FutureTypeClose, Close, (), (override));

	MOCK_METHOD(EState, GetState, (), (const, noexcept, override));

	void DelegateToFake()
	{
		ON_CALL(*this, Write)
			.WillByDefault(Return(MakeCompletedFuture<ResultTypeWrite>(Err(SWriteError{ EWriteError::InternalError }))));
		ON_CALL(*this, Read)
			.WillByDefault(Return(MakeCompletedFuture<ResultTypeRead>(Err(SReadError{ EReadError::InternalError }))));
		ON_CALL(*this, OnClose)
			.WillByDefault(Return(MakeCompletedFuture<ResultTypeClose>(Err(SCloseError{ ECloseError::InternalError }))));
		ON_CALL(*this, Close)
			.WillByDefault(Return(MakeCompletedFuture<ResultTypeClose>(Err(SCloseError{ ECloseError::InternalError }))));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(EState::Closed));
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
