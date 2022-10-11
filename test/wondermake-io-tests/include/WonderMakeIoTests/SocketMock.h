#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/Socket.h"

#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/SharedReference.h"

#include <memory>

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
		auto [promiseClose,	futureClose] = MakeAsync<ResultTypeClose>();
		
		std::make_unique<decltype(promiseClose)>(std::move(promiseClose))
			.release();

		ON_CALL(*this, Write)
			.WillByDefault(Return(MakeCompletedFuture<ResultTypeWrite>(Ok())));
		ON_CALL(*this, Read)
			.WillByDefault([this]()
				{
					auto [promise, future] = MakeAsync<ResultTypeRead>();

					myReadPromises.emplace_back(std::move(promise));

					return future;
				});
		ON_CALL(*this, OnClose)
			.WillByDefault(Return(futureClose));
		ON_CALL(*this, Close)
			.WillByDefault(Return(futureClose));

		ON_CALL(*this, GetState)
			.WillByDefault(Return(EState::Open));
	}
	void CompleteNextRead(ResultTypeRead aResult)
	{
		ASSERT_FALSE(myReadPromises.empty());

		auto promise = std::move(myReadPromises.front());

		myReadPromises.erase(myReadPromises.begin());

		promise.Complete(std::move(aResult));
	}

private:
	std::vector<Promise<ResultTypeRead>> myReadPromises;

};

enum class ELinkedSocket { A, B };

class SocketSharedState
{
public:
	inline [[nodiscard]] Socket::FutureTypeWrite Write(ELinkedSocket aSocket, std::vector<u8>&& aBuffer)
	{
		auto& data = GetSocketData(aSocket == ELinkedSocket::A ? ELinkedSocket::B : ELinkedSocket::A);

		if (data.ReadPromises.IsEmpty())
			data.Buffer.insert(data.Buffer.end(), aBuffer.begin(), aBuffer.end());
		else
		{
			auto promise = data.ReadPromises.Pop();

			promise.Complete(Ok(std::move(aBuffer)));
		}

		return MakeCompletedFuture<Socket::ResultTypeWrite>(Ok());
	}
	inline [[nodiscard]] Socket::FutureTypeRead Read(ELinkedSocket aSocket)
	{
		auto& data = GetSocketData(aSocket);

		if (!data.Buffer.empty())
		{
			auto buffer = std::exchange(data.Buffer, decltype(data.Buffer)());

			return MakeCompletedFuture<Socket::ResultTypeRead>(Ok(std::move(buffer)));
		}

		auto [promise, future] = MakeAsync<Socket::ResultTypeRead>();

		data.ReadPromises.Emplace(std::move(promise));

		return std::move(future);
	}

	inline [[nodiscard]] Socket::FutureTypeClose Close(ELinkedSocket aSocket)
	{
		if (myState != Socket::EState::Open)
			return MakeCompletedFuture<Socket::ResultTypeClose>(Ok(
				Socket::SCloseLocation
				{
					(aSocket == myCloseSocket)
						? Socket::ECloseLocation::ClosedLocally
						: Socket::ECloseLocation::ClosedRemotely
				}));

		myState = Socket::EState::Closed;
		myCloseSocket = aSocket;

		for (auto& promise : mySocketDataA.ClosePromises)
		{
			promise.Complete(Ok(
				Socket::SCloseLocation
				{
					(ELinkedSocket::A == myCloseSocket)
						? Socket::ECloseLocation::ClosedLocally
						: Socket::ECloseLocation::ClosedRemotely
				}));
		}
		for (auto& promise : mySocketDataB.ClosePromises)
		{
			promise.Complete(Ok(
				Socket::SCloseLocation
				{
					(ELinkedSocket::B == myCloseSocket)
						? Socket::ECloseLocation::ClosedLocally
						: Socket::ECloseLocation::ClosedRemotely
				}));
		}

		mySocketDataA.ClosePromises.Clear();
		mySocketDataB.ClosePromises.Clear();

		return MakeCompletedFuture<Socket::ResultTypeClose>(Ok(Socket::SCloseLocation{ Socket::ECloseLocation::ClosedLocally }));
	}

	inline [[nodiscard]] Socket::FutureTypeClose OnClose(ELinkedSocket aSocket)
	{
		if (myState != Socket::EState::Open)
			return MakeCompletedFuture<Socket::ResultTypeClose>(Ok(
				Socket::SCloseLocation
				{
					(aSocket == myCloseSocket)
						? Socket::ECloseLocation::ClosedLocally
						: Socket::ECloseLocation::ClosedRemotely
				}));

		auto& data = GetSocketData(aSocket);
		auto [promise, future] = MakeAsync<Socket::ResultTypeClose>();

		data.ClosePromises.Emplace(std::move(promise));

		return std::move(future);
	}

	inline [[nodiscard]] Socket::EState GetState() const noexcept
	{
		return myState;
	}

private:
	struct SSocketData
	{
		inline SSocketData()
			: ClosePromises(InlineExecutor())
			, ReadPromises(InlineExecutor())
		{}

		CancelableList<Promise<Socket::ResultTypeClose>>	ClosePromises;
		CancelableList<Promise<Socket::ResultTypeRead>>		ReadPromises;
		std::vector<u8>										Buffer;
	};

	inline [[nodiscard]] SSocketData& GetSocketData(ELinkedSocket aSocket) noexcept
	{
		return aSocket == ELinkedSocket::A
			? mySocketDataA
			: mySocketDataB;
	}

	SSocketData mySocketDataA;
	SSocketData mySocketDataB;

	Socket::EState	myState = Socket::EState::Open;
	ELinkedSocket	myCloseSocket = ELinkedSocket::A;

};

class SocketLinked
	: public Socket
{
public:
	inline SocketLinked(SharedReference<SocketSharedState> aState, ELinkedSocket aSocket)
		: myState(std::move(aState))
		, mySocket(aSocket)
	{}
	inline virtual ~SocketLinked()
	{
		Close();
	}

	inline FutureTypeWrite Write(std::vector<u8> aBuffer) override
	{
		return myState->Write(mySocket, std::move(aBuffer));
	}
	inline FutureTypeRead Read() override
	{
		return myState->Read(mySocket);
	}
	inline FutureTypeClose OnClose() override
	{
		return myState->OnClose(mySocket);
	}
	inline FutureTypeClose Close() override
	{
		return myState->Close(mySocket);
	}

	inline EState GetState() const noexcept override
	{
		return myState->GetState();
	}

private:
	SharedReference<SocketSharedState>	myState;
	ELinkedSocket						mySocket;

};

inline std::pair<SharedReference<Socket>, SharedReference<Socket>> MakeLinkedSockets()
{
	auto state = MakeSharedReference<SocketSharedState>();

	return std::make_pair(
		MakeSharedReference<SocketLinked>(state, ELinkedSocket::A),
		MakeSharedReference<SocketLinked>(state, ELinkedSocket::B));
}

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
