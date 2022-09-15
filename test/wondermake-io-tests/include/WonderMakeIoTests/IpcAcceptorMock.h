#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/IpcAcceptor.h"

class IpcAcceptorMock
	: public IpcAcceptor
{
public:
	MOCK_METHOD(ResultTypeOpen,			Open, (std::string aName), (override));
	MOCK_METHOD(FutureTypeConnection,	OnConnection, (), (override));
	MOCK_METHOD(FutureTypeClose,		OnClose, (), (override));
	MOCK_METHOD(void,					Close, (), (override));

	MOCK_METHOD(EState, GetState, (), (const, noexcept, override));

	void DelegateToFake()
	{
		ON_CALL(*this, Open)
			.WillByDefault(Return(Err(SOpenError{ EOpenError::InternalError })));
		ON_CALL(*this, OnConnection)
			.WillByDefault(Return(MakeCompletedFuture<ResultTypeConnection>(Err(SConnectionError{ EConnectionError::InternalError }))));
		ON_CALL(*this, OnClose)
			.WillByDefault(Return(MakeCompletedFuture<void>()));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(EState::Closed));
	}
};

inline std::ostream& operator<<(std::ostream& out, const IpcAcceptor::SOpenError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}
inline std::ostream& operator<<(std::ostream& out, const IpcAcceptor::SConnectionError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}

MATCHER_P(IpcAcceptorOpenResultMatcher, aResult, "")
{
	IpcAcceptor::ResultTypeOpen argResult = aResult;

	if (arg && argResult)
		return true;

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &IpcAcceptor::SOpenError::Error, argResult.Err().Error),
				Field("Reason", &IpcAcceptor::SOpenError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}
MATCHER_P(IpcAcceptorConnectionResultMatcher, aResult, "")
{
	IpcAcceptor::ResultTypeConnection argResult = aResult;

	if (arg && argResult)
		return ExplainMatchResult(
			Eq(argResult.Unwrap()),
			arg.Unwrap(), result_listener);

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &IpcAcceptor::SConnectionError::Error, argResult.Err().Error),
				Field("Reason", &IpcAcceptor::SConnectionError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}
