#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/SocketAcceptor.h"

class SocketAcceptorMock
	: public SocketAcceptor
{
public:
	MOCK_METHOD(FutureTypeConnection, OnConnection, (), (override));
	MOCK_METHOD(FutureTypeClose, OnClose, (), (override));
	MOCK_METHOD(void, Close, (), (override));

	MOCK_METHOD(EState, GetState, (), (const, noexcept, override));

	static auto ReturnSocket(auto&& aSocket)
	{
		return Return(MakeCompletedFuture<ResultTypeConnection>(Ok(std::forward<decltype(aSocket)>(aSocket))));
	}

	void DelegateToFake()
	{
		auto [promiseClose, futureClose] = MakeAsync<void>();

		ON_CALL(*this, OnConnection)
			.WillByDefault([this]()
				{
					auto [promise, future] = MakeAsync<ResultTypeConnection>();

					myConnectionPromises.emplace_back(std::move(promise));

					return future;
				});
		ON_CALL(*this, OnClose)
			.WillByDefault(Return(futureClose));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(EState::Open));
	}
	void CompleteNextConnection(ResultTypeConnection aResult)
	{
		ASSERT_FALSE(myConnectionPromises.empty());

		auto promise = std::move(myConnectionPromises.front());

		myConnectionPromises.erase(myConnectionPromises.begin());

		promise.Complete(std::move(aResult));
	}

private:
	std::vector<Promise<ResultTypeConnection>> myConnectionPromises;

};

inline std::ostream& operator<<(std::ostream& out, const SocketAcceptor::SConnectionError& aError)
{
	return out << '{'
		<< static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ','
		<< aError.Reason << '}';
}

MATCHER_P(SocketAcceptorConnectionResultMatcher, aResult, "")
{
	SocketAcceptor::ResultTypeConnection argResult = aResult;

	if (arg && argResult)
		return arg.Unwrap() == argResult.Unwrap();

	if (!arg && !argResult)
		return ExplainMatchResult(
			AllOf(
				Field("Error", &SocketAcceptor::SConnectionError::Error, argResult.Err().Error),
				Field("Reason", &SocketAcceptor::SConnectionError::Reason, argResult.Err().Reason)),
			arg.Err(), result_listener);

	return false;
}
