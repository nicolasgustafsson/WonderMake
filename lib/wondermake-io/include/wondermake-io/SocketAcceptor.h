#pragma once

#include "wondermake-utility/Future.h"
#include "wondermake-utility/Result.h"
#include "wondermake-utility/SharedReference.h"

#include <magic_enum.hpp>

class Socket;

class SocketAcceptor
{
public:
	enum class EState
	{
		Open,
		Closed
	};

	enum class EConnectionError
	{
		InvalidState,
		OutOfMemory,
		InternalError
	};

	struct SConnectionError
	{
		EConnectionError Error = EConnectionError::InternalError;
		u64 Reason = 0;
	};

	using ResultTypeConnection	= Result<SharedReference<Socket>, SConnectionError>;

	using FutureTypeConnection	= Future<ResultTypeConnection>;
	using FutureTypeClose		= Future<void>;

	virtual ~SocketAcceptor() noexcept = default;

	virtual FutureTypeConnection	OnConnection() = 0;
	virtual FutureTypeClose			OnClose() = 0;
	virtual void					Close() = 0;

	virtual EState GetState() const noexcept = 0;

protected:
	SocketAcceptor() noexcept = default;

};

inline static void WmLogStream(std::ostream& aStream, const SocketAcceptor::SConnectionError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
