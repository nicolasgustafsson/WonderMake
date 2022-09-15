#pragma once

#include "wondermake-utility/Future.h"
#include "wondermake-utility/Result.h"
#include "wondermake-utility/SharedReference.h"

#include <magic_enum.hpp>

#include <memory>

class IpcConnection;

class IpcAcceptor
{
public:
	enum class EState
	{
		Open,
		Closed
	};

	enum class EOpenError
	{
		InvalidArgs,
		InvalidState,
		OutOfMemory,
		InternalError
	};
	enum class EConnectionError
	{
		InvalidState,
		OutOfMemory,
		InternalError
	};

	struct SOpenError
	{
		EOpenError Error = EOpenError::InternalError;
		u64 Reason = 0;
	};
	struct SConnectionError
	{
		EConnectionError Error = EConnectionError::InternalError;
		u64 Reason = 0;
	};

	using ResultTypeOpen		= Result<void, SOpenError>;
	using ResultTypeConnection	= Result<SharedReference<IpcConnection>, SConnectionError>;

	using FutureTypeConnection	= Future<ResultTypeConnection>;
	using FutureTypeClose		= Future<void>;

	virtual ~IpcAcceptor() noexcept = default;

	virtual ResultTypeOpen			Open(std::string aName) = 0;
	virtual FutureTypeConnection	OnConnection() = 0;
	virtual FutureTypeClose			OnClose() = 0;
	virtual void					Close() = 0;

	virtual EState GetState() const noexcept = 0;

protected:
	IpcAcceptor() noexcept = default;

};

inline static void WmLogStream(std::ostream& aStream, const IpcAcceptor::SOpenError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
inline static void WmLogStream(std::ostream& aStream, const IpcAcceptor::SConnectionError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
